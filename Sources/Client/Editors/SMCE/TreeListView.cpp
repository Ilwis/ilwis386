/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
// TreeListView.cpp: implementation of the TreeListView class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\constant.h"
#include "Client\Headers\formelementspch.h"
#include "Client\Editors\SMCE\TreeListView.h"
#include "Client\Editors\SMCE\CriteriaTreeItem.h"

#define ID_TREE_LIST_SCROLLBAR   377

/////////////////////////////////////////////////////////////////////////////
// TreeListItem

TreeListItem::TreeListItem()
{
}

CString TreeListItem::GetItemText(int iCol)
{
	CriteriaTreeItem* cti = (CriteriaTreeItem*)GetItemData();
	return cti->sDisplayText(iCol);
}

DWORD TreeListItem::GetItemData()
{
	return m_Data;
}

void TreeListItem::SetItemData(DWORD m_iData, HTREEITEM m_hItem)
{
	m_Data = m_iData;
	CriteriaTreeItem* cti = (CriteriaTreeItem*)m_Data;
	cti->hItem = m_hItem;
}

void TreeListItem::CustomDraw(TreeListView* pParent, NMHDR* pNMHDR, LRESULT* pResult)
{
  LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW) pNMHDR;
  switch (lptvcd->nmcd.dwDrawStage) 
	{
		case CDDS_ITEMPREPAINT:
			*pResult = CDRF_NOTIFYPOSTPAINT;
			return;
		case CDDS_ITEMPOSTPAINT:
			*pResult = CDRF_DODEFAULT;
			CDC cdc;
			cdc.Attach(lptvcd->nmcd.hdc);
			CRect rect = lptvcd->nmcd.rc;
			TreeListCtrl& tc = pParent->GetTreeCtrl();
			HTREEITEM hti = tc.HitTest(rect.TopLeft());
			tc.GetItemRect(hti, &rect, TRUE);

			rect.right = pParent->iColumnsWidth();
			UINT selflag = CDIS_SELECTED;
		
			if ( !(lptvcd->nmcd.uItemState & selflag))
			{
				cdc.SetBkMode(TRANSPARENT);
				cdc.SetTextColor(COLOR_WINDOWTEXT);
			}
			else
			{
				// If the item is selected, paint the rectangle with the system color
				if (pParent->iActiveColumn() == -1) // mark entire row
				{
					COLORREF m_highlightColor = ::GetSysColor (COLOR_HIGHLIGHT);
					CBrush brush(m_highlightColor);
					cdc.FillRect (rect, &brush);
					// draw a dotted focus rectangle
					cdc.DrawFocusRect(rect);
					cdc.SetBkColor(m_highlightColor);
					cdc.SetTextColor(::GetSysColor (COLOR_HIGHLIGHTTEXT));
				}
				else
				{
					// draw with normal .. only a local blue rectangle will be drawn, but erase the CTreeCtrl select "" garbage
					COLORREF m_normalColor = ::GetSysColor (COLOR_WINDOW);
					CBrush brush(m_normalColor);
					cdc.FillRect (rect, &brush);
					cdc.SetBkMode(TRANSPARENT);
					cdc.SetTextColor(COLOR_WINDOWTEXT);
				}
			}
			int m_nWidth = 0;
			for(int i=0; i<pParent->iColumns(); ++i)
			{
				if ((pParent->iActiveColumn() == i) && (lptvcd->nmcd.uItemState & selflag))
				{
					// draw a small rectangle
					CRect smallRect(m_nWidth+((0==i)?rect.left:0), rect.top, m_nWidth+pParent->iGetColumnWidth(i)-2, rect.bottom);
					COLORREF m_highlightColor = ::GetSysColor (COLOR_HIGHLIGHT);
					CBrush brush(m_highlightColor);
					if (!smallRect.IsRectEmpty()) // test coz FillRect fills a negative rect!!
						cdc.FillRect (smallRect, &brush);
					// draw a dotted focus rectangle
					cdc.SetTextColor(::GetSysColor (COLOR_HIGHLIGHTTEXT)); // do this before focusrect!
					cdc.DrawFocusRect(smallRect);
					cdc.SetBkColor(m_highlightColor);
				}
				else if ((pParent->iActiveColumn()>=0) || (!(lptvcd->nmcd.uItemState & selflag))) // don't draw pink when whole row is selected coz it looks ugly
				{
					// find out if there's a problem for column i of the current item
					CriteriaTreeItem* cti = (CriteriaTreeItem*)GetItemData();
					COLORREF clrRect = cti->clrUserColor(i);
					if (clrRect != RGB(0,0,0))
					{
						// a coloured box to indicate something to the user
						CRect smallRect(m_nWidth+((0==i)?rect.left:0), rect.top, m_nWidth+pParent->iGetColumnWidth(i)-2, rect.bottom);
						if (!smallRect.IsRectEmpty())
						{
							CBrush brColor(clrRect);
							cdc.FillRect(smallRect, &brColor);
						}
					}
				}
				CRect itemRect(m_nWidth+((0==i)?(rect.left+4):0), rect.top, m_nWidth+pParent->iGetColumnWidth(i)-4, rect.bottom);
				if (!itemRect.IsRectEmpty())
					ItemCustomDraw(&cdc, itemRect, i, pParent->iGetColumnAlign(i));
				if ((pParent->iActiveColumn() == i) && (lptvcd->nmcd.uItemState & selflag))
				{
					// restore
					cdc.SetBkMode(TRANSPARENT);
					cdc.SetTextColor(COLOR_WINDOWTEXT);
				}
				m_nWidth += pParent->iGetColumnWidth(i);
			}
			cdc.Detach();
	} // end switch()
}

void DrawPlusMinus(CDC* pDC, int x, int y, COLORREF clrRef, bool fPlus)
{
	COLORREF clrRefDark RGB(GetRValue(clrRef) / 1.2, GetGValue(clrRef) / 1.2, GetBValue(clrRef) / 1.2);
	CPen pen(PS_SOLID, 1, clrRefDark);
	CBrush brush(clrRef);
	CPen* penOld = pDC->SelectObject(&pen);
	CBrush* brOld = pDC->SelectObject(&brush);

	pDC->Ellipse(x, y, x+7, y+7);

	pDC->SelectStockObject(BLACK_PEN);


	CPoint pPrevious = pDC->MoveTo(x+1, y+3);
	pDC->LineTo(x+6, y+3);

	if (fPlus)
	{
		pDC->MoveTo(x+3, y+1);
		pDC->LineTo(x+3, y+6);
	}

	pDC->MoveTo(pPrevious);

	pDC->SelectObject(brOld);
	pDC->SelectObject(penOld);

}

void TreeListItem::ItemCustomDraw(CDC* pDC, CRect rect, int iCol, int nFormat)
{
	// Is called to draw each item (for each col) in the TreeListCtrl
	// Responsibility:
	// for iCol==0 draw only the label within the given rect
	// (the icon, plus-sign and the checkbox (if any) are already drawn by the tree)
	// for iCol>0 draw the entire item, thus including its icon if needed within rect
	
	// In this implementation: each item's display text can be retrieved using GetItemText(iCol)
	// The display text gives us all the information we need
	// (we're responsible for interpreting directives like tab delimiters, extensions etc.)

	// Make sure everything fits in the prescribed rectangle - truncate text well if it doesn't

	CString sDisplayText(GetItemText(iCol)); // the display text, incl. directives
	CString sText(sDisplayText);
	int iDotPos = sDisplayText.ReverseFind('.');
	int iColonPos = sDisplayText.ReverseFind(':');
	if ((iCol>0) && (iDotPos == sDisplayText.GetLength()-4))
	{
		// try for an icon, and print it if available
		CString sIcon = sDisplayText.Mid(iDotPos+1);
		zIcon icon;
		if (icon.fLoad(String("%s16Ico", sIcon)))
		{
			// we found an icon .. remove it from the text to display
			sText = sDisplayText.Left(iDotPos);
			// draw the icon
			pDC->DrawIcon(rect.left+2, rect.top, icon);
			// shorten the rectangle .. icon is 16x16, leave some space
			rect.left += 22;
		}
	}
	else if ((iCol == 0) && (iColonPos >= 0))
	{
		CString sCostBenefit = sDisplayText.Mid(iColonPos+1);
		if (sCostBenefit.Compare("cost") == 0)
		{
			// remove ":cost" from the text
			sText = sDisplayText.Left(iColonPos);
			// display the symbol
			COLORREF clrRed RGB(255,128,128);
			DrawPlusMinus(pDC, rect.left - 10, rect.top, clrRed, false);
		}
		else if (sCostBenefit.Compare("benefit") == 0)
		{
			// remove ":benefit" from the text
			sText = sDisplayText.Left(iColonPos);
			// display the symbol
			COLORREF clrGreen RGB(128,255,64);
			DrawPlusMinus(pDC, rect.left - 10, rect.top, clrGreen, true);
		}
		else if (sCostBenefit.Compare("none") == 0)
		{
			// remove ":none" from the text
			sText = sDisplayText.Left(iColonPos);
		}
	}

	// in the remaining rect, draw the text

  bool fTextNeedDots = false;
	int iMaxTextWidth = rect.right - rect.left;

  while ((sText.GetLength()>0) && (pDC->GetTextExtent((LPCTSTR) sText).cx > (iMaxTextWidth - 4)))
	{
    sText = sText.Left (sText.GetLength () - 1);
    fTextNeedDots = true;
  }
  if (fTextNeedDots)
	{
		if (sText.GetLength () >= 1)
			sText = sText.Left (sText.GetLength () - 1);
		sText += "...";
	}
  // Now draw sText into the rectangle using CDC::DrawText
  rect.right = rect.left + iMaxTextWidth;
  UINT nStyle = DT_VCENTER | DT_SINGLELINE;
  if (nFormat == LVCFMT_LEFT)
		nStyle |= DT_LEFT;
  else if (nFormat == LVCFMT_CENTER)
		nStyle |= DT_CENTER;
  else 
		nStyle |= DT_RIGHT;
	
	if((sText.GetLength()>0) && (rect.right>rect.left))
		pDC->DrawText(sText, rect, nStyle);
}

/////////////////////////////////////////////////////////////////////////////
// TreeListCtrl

IMPLEMENT_DYNCREATE(TreeListCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(TreeListCtrl, CTreeCtrl)
		ON_WM_PAINT()
END_MESSAGE_MAP()

void TreeListCtrl::OnPaint()
{
	// OnPaint is overridden to utilise the scroll offset
	// Otherwise the CTreeCtrl keeps drawing itself at the left of the CView
	CPaintDC dc(this);

	TreeListView* tlv = (TreeListView*)GetParent();

	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// The following lines are a workaround for the real problem:
	// the CTreeCtrl can't imagine that it is bigger than the window
	// perhaps a clip-offset or extent problem, but couldn't find it
	COLORREF m_normalColor = ::GetSysColor (COLOR_WINDOW);
	dc.FillSolidRect(tlv->iGetColumnWidth(0),0,rcClient.Width(),rcClient.Height(),m_normalColor);

	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	dc.SelectClipRgn(&rgn);
	rgn.DeleteObject();

	dc.SetViewportOrg(tlv->iOffset(), 0);
	// dc.OffsetViewportOrg(tlv->iOffset(), 0);

	// Now let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
}

/////////////////////////////////////////////////////////////////////////////
// TreeListView

IMPLEMENT_DYNCREATE(TreeListView, CView)

BEGIN_MESSAGE_MAP(TreeListView, CView)
	//{{AFX_MSG_MAP(TreeListView)
		ON_WM_SIZE()
		ON_WM_HSCROLL()
		ON_WM_CONTEXTMENU()
		ON_WM_LBUTTONDOWN() // will not receive from CTreeCtrl without PreTranslateMsg arrangement
		ON_WM_RBUTTONDOWN()	// will not receive from CTreeCtrl without PreTranslateMsg arrangement
		ON_WM_LBUTTONUP() // will not receive from CTreeCtrl without PreTranslateMsg arrangement
		// ON_WM_MOUSEMOVE()	
//		ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
		ON_NOTIFY(NM_CUSTOMDRAW, ID_TREE_LIST_CTRL, OnCustomDraw)
		ON_NOTIFY(TVN_SELCHANGED, ID_TREE_LIST_CTRL, OnSelChanged)
		ON_NOTIFY(TVN_ITEMEXPANDING, ID_TREE_LIST_CTRL, OnItemExpanding)
		ON_NOTIFY(HDN_ITEMCHANGED, ID_TREE_LIST_HEADER, OnHeaderItemChanged)
		ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TreeListView construction/destruction

TreeListView::TreeListView()
: m_TLInitialized(FALSE)
, m_nOffset(0)
, m_nColumns(0)
, m_nColumnsWidth(0)
, m_iActiveColumn(-1) // none selected
{
}

TreeListView::~TreeListView()
{
}

TreeListCtrl & TreeListView::GetTreeCtrl()
{
	return tc;
}

/////////////////////////////////////////////////////////////////////////////
// TreeListView message handlers

void TreeListView::OnInitialUpdate() 
{
	if (!m_TLInitialized)
	{
		Initialize();	

		CRect m_wndRect;
		GetClientRect(&m_wndRect);
		MoveWindow(0, 0, m_wndRect.Width(), m_wndRect.Height());

		m_TLInitialized = TRUE;		
	}

	CView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// TreeListView drawing

void TreeListView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// TreeListView message handlers

void TreeListView::Initialize()
{
	// creates all the objects in frame -
	// header, tree, horizontal scroll bar

	CRect m_wndRect;
	GetWindowRect(&m_wndRect);
	CRect m_headerRect;
	
	// create the header
	{
		m_headerRect.left = m_headerRect.top = -1;
		m_headerRect.right = m_wndRect.Width();

		m_wndHeader.Create(WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_FULLDRAG | HDS_HORZ, m_headerRect, this, ID_TREE_LIST_HEADER);
		// m_wndHeader.Create(WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ, m_headerRect, this, ID_TREE_LIST_HEADER);
	}

	CSize textSize;
	// set header's pos, dimensions and image list
	{
		CDC *pDC = m_wndHeader.GetDC();
		textSize = pDC->GetTextExtent("A");
		m_wndHeader.ReleaseDC(pDC); 

		m_wndHeader.SetWindowPos(&wndTop, -1, -1, m_headerRect.Width(), textSize.cy+4, SWP_SHOWWINDOW);
	}

	CRect m_treeRect;

	// create the tree itself
	{
		GetWindowRect(&m_wndRect);

		m_treeRect.left=0;
		m_treeRect.top = textSize.cy+4;
		m_treeRect.right = m_headerRect.Width()-5;
		m_treeRect.bottom = m_wndRect.Height()-GetSystemMetrics(SM_CYHSCROLL)-4;

		tc.Create(WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,m_treeRect, this, ID_TREE_LIST_CTRL);
	}

	// finally, create the horizontal scroll bar
	{
		CRect m_scrollRect;
		m_scrollRect.left=0;
		m_scrollRect.top = m_treeRect.bottom;
		m_scrollRect.right = m_treeRect.Width()-GetSystemMetrics(SM_CXVSCROLL);
		m_scrollRect.bottom = m_wndRect.bottom;

		m_horScrollBar.Create(WS_CHILD | WS_VISIBLE | WS_DISABLED | SBS_HORZ | SBS_TOPALIGN, m_scrollRect, this, ID_TREE_LIST_SCROLLBAR);
	}
}

CImageList* TreeListView::SetImageList(CImageList * pImageList, int nImageListType)
{
	return tc.SetImageList(pImageList, nImageListType);
}

BOOL TreeListView::VerticalScrollVisible()
{	
	int sMin, sMax;
	tc.GetScrollRange(SB_VERT, &sMin, &sMax);	
	return sMax!=0;
}

BOOL TreeListView::HorizontalScrollVisible()
{
	int sMin, sMax;
	m_horScrollBar.GetScrollRange(&sMin, &sMax);
	return sMax!=0;
}

int TreeListView::StretchWidth(int m_nWidth, int m_nMeasure)
{	
	return ((m_nWidth/m_nMeasure)+1)*m_nMeasure;//put the fixed for +1 in brackets f/0 error
}

void TreeListView::ResetScrollBar()
{
	// resetting the horizontal scroll bar

	int m_nPageWidth;

	CRect m_treeRect;
	tc.GetClientRect(&m_treeRect);

	CRect m_wndRect;
	GetClientRect(&m_wndRect);

	CRect m_headerRect;
	m_wndHeader.GetClientRect(&m_headerRect);

	CRect m_barRect;
	m_horScrollBar.GetClientRect(m_barRect);

	m_nPageWidth = m_treeRect.Width();

	if(m_nColumnsWidth > m_nPageWidth)
	{
		// show the scroll bar and adjust it's size
		m_horScrollBar.EnableWindow(TRUE);
		m_horScrollBar.ShowWindow(SW_SHOW);
		// the tree becomes smaller
		tc.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

		if(!VerticalScrollVisible())
			// i.e. vertical scroll bar isn't visible
			m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_barRect.Height(), SWP_NOMOVE);
		else
			m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);

		// m_horScrollBar.SetScrollRange(0, m_nColumnsWidth-m_nPageWidth);
		m_horScrollBar.SetScrollRange(0, m_nColumnsWidth);
    SCROLLINFO scinfo;
		m_horScrollBar.GetScrollInfo(&scinfo,SIF_ALL);
		scinfo.cbSize = sizeof(scinfo);
		scinfo.fMask  = SIF_PAGE;
		scinfo.nPage  = m_nPageWidth;
		m_horScrollBar.SetScrollInfo(&scinfo,FALSE);

		// recalculate the offset
		CRect m_wndHeaderRect;
		m_wndHeader.GetWindowRect(&m_wndHeaderRect);
		ScreenToClient(&m_wndHeaderRect);

		m_nOffset = m_wndHeaderRect.left;
		m_horScrollBar.SetScrollPos(-m_nOffset);
	}
	else
	{
		m_horScrollBar.EnableWindow(FALSE);
			// we no longer need it, so hide it!
		if (m_horScrollBar.ShowWindow(SW_HIDE)) // test the previous state; don't redraw unnecessary
		{
			tc.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_headerRect.Height(), SWP_NOMOVE);
			// the tree takes scroll's place

			m_horScrollBar.SetScrollRange(0, 0);

			// set scroll offset to zero
			m_nOffset = 0;
			tc.Invalidate();
			CRect m_headerRect;
			m_wndHeader.GetWindowRect(&m_headerRect);
			CRect m_wndRect;
			GetClientRect(&m_wndRect);
			m_wndHeader.SetWindowPos(&wndTop, m_nOffset, 0, max(StretchWidth(m_nColumnsWidth,m_wndRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
		}
	}
}

void TreeListView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CRect m_treeRect;
	tc.GetClientRect(&m_treeRect);
	
	// if horizontal scroll bar 
	if(pScrollBar == &m_horScrollBar)
	{
		int m_nCurPos = m_horScrollBar.GetScrollPos();
		int m_nPrevPos = m_nCurPos;
		// decide what to do for each diffrent scroll event
		switch(nSBCode)
		{
			case SB_LEFT:			m_nCurPos = 0;
									break;
			case SB_RIGHT:			m_nCurPos = m_horScrollBar.GetScrollLimit()-1;
									break;
			case SB_LINELEFT:		m_nCurPos = max(m_nCurPos-6, 0);
									break;
			case SB_LINERIGHT:		m_nCurPos = min(m_nCurPos+6, m_horScrollBar.GetScrollLimit()-1);
									break;
			case SB_PAGELEFT:		m_nCurPos = max(m_nCurPos-m_treeRect.Width(), 0);
									break;
			case SB_PAGERIGHT:		m_nCurPos = min(m_nCurPos+m_treeRect.Width(), m_horScrollBar.GetScrollLimit()-1);
									break;
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:  if(nPos==0)
									    m_nCurPos = 0;
								    else
									    m_nCurPos = min(StretchWidth(nPos, 6), m_horScrollBar.GetScrollLimit()-1);
								    break;
		}		

		m_horScrollBar.SetScrollPos(m_nCurPos);
		m_nOffset = -m_nCurPos;

		// smoothly scroll the tree control
		{
			CRect m_scrollRect;
			tc.GetClientRect(&m_scrollRect);
			tc.ScrollWindow(m_nPrevPos - m_nCurPos, 0, &m_scrollRect, &m_scrollRect);
		}

		CRect m_headerRect;
		m_wndHeader.GetWindowRect(&m_headerRect);
		CRect m_wndRect;
		GetClientRect(&m_wndRect);		

		m_wndHeader.SetWindowPos(&wndTop, m_nOffset, 0, max(StretchWidth(m_nColumnsWidth,m_treeRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
	}

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void TreeListView::RecalcColumnsWidth()
{
	m_nColumnsWidth = 0;
	for(int i=0;i<m_nColumns;++i)
		m_nColumnsWidth += iGetColumnWidth(i);
}

void TreeListView::OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	int m_nPrevColumnsWidth = m_nColumnsWidth;
	RecalcColumnsWidth(); // gives new m_nColumnsWidth
	ResetScrollBar();

	// in case we were at the scroll bar's end,
	// and some column's width was reduced,
	// update header's position (move to the right).
	CRect m_treeRect;
	tc.GetClientRect(&m_treeRect);

	CRect m_headerRect;
	m_wndHeader.GetClientRect(&m_headerRect);

	if((m_nPrevColumnsWidth > m_nColumnsWidth) &&
		 (m_horScrollBar.GetScrollPos() == m_horScrollBar.GetScrollLimit()-1) &&
		 (m_treeRect.Width() < m_nColumnsWidth))
	{
		m_nOffset = -m_nColumnsWidth+m_treeRect.Width();
		m_wndHeader.SetWindowPos(&wndTop, m_nOffset, 0, 0, 0, SWP_NOSIZE);
	}

	m_wndHeader.Invalidate();
	tc.Invalidate();
}

void TreeListView::OnContextMenu(CWnd* pWnd, CPoint point) 
{	
	// GetParent()->SendMessage(WM_CONTEXTMENU, (WPARAM)pWnd, MAKELPARAM(point.x, point.y) ); 
}

void TreeListView::OnSize(UINT nType, int cx, int cy) 
{
	SetScrollRange(SB_HORZ, 0, 0);
	SetScrollRange(SB_VERT, 0, 0);

	if(m_TLInitialized)
	{	
		// resize all the controls	
		CRect m_wndRect;
		GetClientRect(&m_wndRect);

		CRect m_headerRect;
		m_wndHeader.GetClientRect(&m_headerRect);
		m_wndHeader.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_headerRect.Height(), SWP_NOMOVE);

		CRect m_scrollRect;
		m_horScrollBar.GetClientRect(&m_scrollRect);

		tc.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_scrollRect.Height(), SWP_NOMOVE);

		CRect m_treeRect;
		tc.GetClientRect(&m_treeRect);
		m_horScrollBar.MoveWindow(0, m_treeRect.bottom, m_wndRect.Width(), m_scrollRect.Height());

		if(m_nColumnsWidth > m_treeRect.Width())
		{
			// show the horz scroll bar
			{
				CRect m_barRect;
				m_horScrollBar.GetClientRect(&m_barRect);			

				m_horScrollBar.EnableWindow(TRUE);

				m_horScrollBar.ShowWindow(SW_SHOW);

				// the tree becomes smaller
				tc.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

				int tfdsfsd=m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL);
				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);
				
			}

			// m_horScrollBar.SetScrollRange(0, m_nColumnsWidth-m_treeRect.Width());
			m_horScrollBar.SetScrollRange(0, m_nColumnsWidth);
			SCROLLINFO scinfo;
			m_horScrollBar.GetScrollInfo(&scinfo,SIF_ALL);
			scinfo.cbSize = sizeof(scinfo);
			scinfo.fMask  = SIF_PAGE;
			scinfo.nPage  = m_treeRect.Width();
			m_horScrollBar.SetScrollInfo(&scinfo,FALSE);
		}
		else
		{
			// hide the scroll bar
			{
				m_horScrollBar.EnableWindow(FALSE);

				m_horScrollBar.ShowWindow(SW_HIDE);

				// the tree becomes larger
				tc.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_headerRect.Height(), SWP_NOMOVE);				
			}

			m_horScrollBar.SetScrollRange(0, 0);
		}

		ResetVertScrollBar();
		
		// Reset the header control position and scroll
		// the tree control as required.
		
		int m_nCurPos = m_horScrollBar.GetScrollPos();
		m_nOffset = -m_nCurPos;
	
		// smoothly scroll the tree control
		{
			CRect m_scrollRect;
			tc.GetClientRect(&m_scrollRect);
			tc.ScrollWindow(m_nCurPos, 0, &m_scrollRect, &m_scrollRect);
		}

		m_wndHeader.GetWindowRect(&m_headerRect);	
		GetClientRect(&m_wndRect);

		m_wndHeader.SetWindowPos(&wndTop, m_nOffset, 0,max(StretchWidth(m_nColumnsWidth,m_treeRect.Width()),m_wndRect.Width()), m_headerRect.Height(),SWP_SHOWWINDOW);			
	}	
}

void TreeListView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar == VK_TAB) && (!(nFlags & (2<<14))))
	{
		m_iActiveColumn = (m_iActiveColumn + 1) % iColumns(); // rotate
		tc.Invalidate();
	}
	else
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL TreeListView::PreTranslateMessage(MSG* pMsg) 
{
	// replicate some messages below .. we also want to handle them
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == tc.m_hWnd)
	{
		SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam); // also replicate for OnKeyDown
		if (pMsg->wParam == VK_TAB)
			return TRUE; // "steal" the message from the tc because it pings with tab!!
			// Perhaps we should also "steal" the corresponding WM_KEYUP, but til now I didn't observe anything strange
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_hWnd)
		tc.SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
	else if (pMsg->message == WM_LBUTTONDOWN && pMsg->hwnd == tc.m_hWnd)
		SendMessage(WM_LBUTTONDOWN, pMsg->wParam, pMsg->lParam);
	else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == tc.m_hWnd)
		SendMessage(WM_LBUTTONUP, pMsg->wParam, pMsg->lParam);
	else if (pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == tc.m_hWnd)
		SendMessage(WM_RBUTTONDOWN, pMsg->wParam, pMsg->lParam);
//	else if (pMsg->message == WM_RBUTTONUP && pMsg->hwnd == tc.m_hWnd) // to be complete
//		SendMessage(WM_RBUTTONUP, pMsg->wParam, pMsg->lParam);
	else if (pMsg->message == WM_LBUTTONDBLCLK && pMsg->hwnd == tc.m_hWnd) // bias the clickpoint if in CTreeCtrl column
	{
		SendMessage(WM_LBUTTONDBLCLK, pMsg->wParam, pMsg->lParam); // replicate it

		TVHITTESTINFO hInfo;
		CPoint point(LOWORD(pMsg->lParam),HIWORD(pMsg->lParam));
		hInfo.pt = point;	
		HTREEITEM m_doubleclickedItem = tc.HitTest(&hInfo);				
	
		if(((hInfo.flags & TVHT_ONITEMRIGHT) || (hInfo.flags & TVHT_ONITEMINDENT) ||
			 (hInfo.flags & TVHT_ONITEM)) && (point.x < iGetColumnWidth(0)))
		{
			CRect rect;
			tc.GetItemRect(m_doubleclickedItem, &rect, TRUE);
			pMsg->lParam = MAKELPARAM((WORD)rect.left, (WORD)rect.top);
		}	
	}
	else if (pMsg->message == WM_MOUSEMOVE && pMsg->hwnd == tc.m_hWnd)
		SendMessage(WM_MOUSEMOVE, pMsg->wParam, pMsg->lParam);
	return CView::PreTranslateMessage(pMsg);
}

void TreeListView::ResetVertScrollBar()
{
	
	CRect m_wndRect, m_treeRect, m_headerRect, m_barRect;
	GetClientRect(&m_wndRect);
	tc.GetClientRect(&m_treeRect);
	m_wndHeader.GetClientRect(&m_headerRect);	
	m_horScrollBar.GetClientRect(&m_barRect);

	if(!HorizontalScrollVisible())
		tc.SetWindowPos(&tc.wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_headerRect.Height(), SWP_NOMOVE);
	else
		tc.SetWindowPos(&tc.wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);
	if(VerticalScrollVisible())
		int i=0;

	if(HorizontalScrollVisible())
	{
		if(!VerticalScrollVisible())
		{
			m_horScrollBar.SetWindowPos(&tc.wndTop, 0, 0, m_wndRect.Width(), m_barRect.Height(), SWP_NOMOVE);
			
			int nMin, nMax;
			m_horScrollBar.GetScrollRange(&nMin, &nMax);
			if((nMax-nMin) == (m_nColumnsWidth-m_treeRect.Width()+GetSystemMetrics(SM_CXVSCROLL)))
				// i.e. it disappeared because of calling
				// SetWindowPos
			{
				if(nMax - GetSystemMetrics(SM_CXVSCROLL) > 0)
					m_horScrollBar.SetScrollRange(nMin, nMax - GetSystemMetrics(SM_CXVSCROLL));
				else
					// hide the horz scroll bar and update the tree
				{
					m_horScrollBar.EnableWindow(FALSE);

					// we no longer need it, so hide it!
					{
						m_horScrollBar.ShowWindow(SW_HIDE);

						tc.SetWindowPos(&tc.wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_headerRect.Height(), SWP_NOMOVE);
						// the tree takes scroll's place
					}

					m_horScrollBar.SetScrollRange(0, 0);

					// set scroll offset to zero
					{
						m_nOffset = 0;
						tc.Invalidate();
						m_wndHeader.GetWindowRect(&m_headerRect);
						m_wndHeader.SetWindowPos(&tc.wndTop, m_nOffset, 0, max(StretchWidth(m_nColumnsWidth,m_wndRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
					}
				}
			}
		}
		else
		{
			m_horScrollBar.SetWindowPos(&tc.wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);

			int nMin, nMax;
			m_horScrollBar.GetScrollRange(&nMin, &nMax);
			if((nMax-nMin) == (m_nColumnsWidth-m_treeRect.Width()-GetSystemMetrics(SM_CXVSCROLL)))
				// i.e. it appeared because of calling
				// SetWindowPos
			{
				m_horScrollBar.SetScrollRange(nMin, nMax + GetSystemMetrics(SM_CXVSCROLL));
			}
		}
	}
	else
	if(VerticalScrollVisible())
	{
		if(m_nColumnsWidth>m_treeRect.Width())
			// the vertical scroll bar takes some place
			// and the columns are a bit bigger than the client
			// area but smaller than (client area + vertical scroll width)
		{
			// show the horz scroll bar
			{
				m_horScrollBar.EnableWindow(TRUE);

				m_horScrollBar.ShowWindow(SW_SHOW);

				// the tree becomes smaller
				tc.SetWindowPos(&tc.wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

				m_horScrollBar.SetWindowPos(&tc.wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);
			}

			// m_horScrollBar.SetScrollRange(0, m_nColumnsWidth-m_treeRect.Width());
			m_horScrollBar.SetScrollRange(0, m_nColumnsWidth);
			SCROLLINFO scinfo;
			m_horScrollBar.GetScrollInfo(&scinfo,SIF_ALL);
			scinfo.cbSize = sizeof(scinfo);
			scinfo.fMask  = SIF_PAGE;
			scinfo.nPage  = m_treeRect.Width();
			m_horScrollBar.SetScrollInfo(&scinfo,FALSE);
		}
	}
}

int TreeListView::InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	HD_ITEM hdi;
	hdi.mask = HDI_TEXT | HDI_FORMAT;
	if(nWidth!=-1)
	{
		hdi.mask |= HDI_WIDTH;
		hdi.cxy = nWidth;
	}
	
	hdi.pszText = (LPTSTR)lpszColumnHeading;
	hdi.fmt = HDF_OWNERDRAW;

	if(nFormat == LVCFMT_RIGHT)
		hdi.fmt |= HDF_RIGHT;
	else
	if(nFormat == LVCFMT_CENTER)
		hdi.fmt |= HDF_CENTER;
	else
		hdi.fmt |= HDF_LEFT;

	++m_nColumns;

	int m_nReturn = m_wndHeader.InsertItem(nCol, &hdi);

	RecalcColumnsWidth();
	ResetScrollBar();

	return m_nReturn;
}

BOOL TreeListView::DeleteColumn(int nPos)
{
	--m_nColumns;
	BOOL bRet = m_wndHeader.DeleteItem(nPos);
	RecalcColumnsWidth();
	ResetScrollBar();
	return bRet;
}

int TreeListView::iGetColumnWidth(int nCol)
{
	HD_ITEM hItem;
	hItem.mask = HDI_WIDTH;
	if(!m_wndHeader.GetItem(nCol, &hItem))
		return 0;

	return hItem.cxy;
}

int TreeListView::iGetColumnAlign(int nCol)
{
	HD_ITEM hItem;
	hItem.mask = HDI_FORMAT;
	if(!m_wndHeader.GetItem(nCol, &hItem))
		return LVCFMT_LEFT;

	if(hItem.fmt & HDF_RIGHT)
		return LVCFMT_RIGHT;
	else
	if(hItem.fmt & HDF_CENTER)
		return LVCFMT_CENTER;
	else
		return LVCFMT_LEFT;
}

void TreeListView::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LPNMTVCUSTOMDRAW lptvcd = (LPNMTVCUSTOMDRAW) pNMHDR;
	*pResult = 0;
  switch (lptvcd->nmcd.dwDrawStage) 
	{	
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		default: {
			*pResult = CDRF_DODEFAULT;
			TreeListItem* tli = (TreeListItem*) lptvcd->nmcd.lItemlParam;
			if (tli)
				tli->CustomDraw(this, pNMHDR, pResult);
		}
	}
}

BOOL TreeListView::DeleteOneItem( HTREEITEM hItem )
{
	TreeListItem *pItem = (TreeListItem *)tc.GetItemData(hItem);
	
	BOOL fRetVal = tc.DeleteItem(hItem);

	if (pItem)
		delete pItem;
	
	return fRetVal;
}

void TreeListView::BackupTreeListItemPtrs(HTREEITEM hParent)
{
	HTREEITEM hItem = hParent;	
	TreeListItem *pItem;	
	
	while(hItem!=NULL)
	{			
		pItem = (TreeListItem*)tc.GetItemData(hItem);
		TreeListItemPtrs.Add(pItem);
		
		if(tc.ItemHasChildren(hItem))
			BackupTreeListItemPtrs(tc.GetChildItem(hItem));			
				
		hItem = tc.GetNextSiblingItem(hItem);		
	}	
}

BOOL TreeListView::DeleteAllItems()
{
	LockWindowUpdate();
	BeginWaitCursor();

	BackupTreeListItemPtrs(tc.GetRootItem());		
	BOOL m_bReturn = tc.DeleteAllItems();
	
	for (int i=0;i<TreeListItemPtrs.GetSize();i++)
		delete TreeListItemPtrs.GetAt(i);
	TreeListItemPtrs.RemoveAll();	

	EndWaitCursor();
	UnlockWindowUpdate();
	return m_bReturn;
}

void TreeListView::OnDestroy()
{
	if (::IsWindow(tc.m_hWnd)) // Test! If the tc is not initialised, we can't do a.o. GetRootItem
	{
		BackupTreeListItemPtrs(tc.GetRootItem());		
		tc.DeleteAllItems();
		for (int i=0;i<TreeListItemPtrs.GetSize();i++)
			delete TreeListItemPtrs.GetAt(i);
		TreeListItemPtrs.RemoveAll();
	}

	CView::OnDestroy();
}

BOOL TreeListView::DeleteItem( HTREEITEM hItem )
{
  BOOL bErr(false);

	if (tc.ItemHasChildren(hItem))
	{
		HTREEITEM hNext;
	  HTREEITEM hChild = tc.GetChildItem(hItem);

		while(hChild != NULL)
		{
			hNext = tc.GetNextItem(hChild, TVGN_NEXT);
			bErr = bErr | DeleteItem(hChild);
			hChild = hNext;
		}
	}
	bErr = bErr | (!DeleteOneItem(hItem));
  return bErr;
}

BOOL TreeListView::DeleteChildren(HTREEITEM hItem)
{
  BOOL bErr(false);

	if (tc.ItemHasChildren(hItem))
	{
		HTREEITEM hNext;
	  HTREEITEM hChild = tc.GetChildItem(hItem);

		while(hChild != NULL)
		{
			hNext = tc.GetNextItem(hChild, TVGN_NEXT);
			bErr = bErr | DeleteItem(hChild);
			hChild = hNext;
		}
	}
  return bErr;
}

BOOL TreeListView::SetItemData(HTREEITEM hItem, DWORD dwData)
{
	TreeListItem *pItem = (TreeListItem*)tc.GetItemData(hItem);
	if(!pItem)
		return FALSE;
	pItem->SetItemData(dwData, hItem);
	return tc.SetItemData(hItem, (LPARAM)pItem);
}

DWORD TreeListView::GetItemData(HTREEITEM hItem) const
{
	TreeListItem *pItem = (TreeListItem*)tc.GetItemData(hItem);
	if(!pItem)
		return NULL;
	return pItem->GetItemData();
}				

HTREEITEM TreeListView::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{	
	TreeListItem *pItem = new TreeListItem;
	// pItem->SetItemText(lpszItem);
	ResetScrollBar();
	
	return tc.InsertItem(TVIF_PARAM|TVIF_TEXT, "", 0, 0, 0, 0, (LPARAM)pItem, hParent, hInsertAfter);
}

HTREEITEM TreeListView::InsertItem( LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	TreeListItem *pItem = new TreeListItem;
	// pItem->SetItemText(lpszItem);
	ResetScrollBar();
	
	return tc.InsertItem(TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE, "", nImage, nSelectedImage, 0, 0, (LPARAM)pItem, hParent, hInsertAfter);
}

HTREEITEM TreeListView::InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter )
{
	TreeListItem *pItem = new TreeListItem;
	// pItem->SetItemText(lpszItem);
	HTREEITEM hNew = tc.InsertItem(nMask, "", nImage, nSelectedImage, nState, nStateMask, (LPARAM)pItem, hParent, hInsertAfter);
	pItem->SetItemData(lParam, hNew);
	ResetScrollBar();	
	return hNew;
}


HTREEITEM TreeListView::CopyItem(HTREEITEM hItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{	
	if(tc.ItemHasChildren(hItem))
		return NULL;
	
	TV_ITEM item;
	item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
	item.hItem = hItem;	
	tc.GetItem(&item);
	TreeListItem *pItem = (TreeListItem*)tc.GetItemData(hItem);
	TreeListItem *pNewItem = new TreeListItem(*pItem);

	item.lParam = (LPARAM)pNewItem;

	TV_INSERTSTRUCT insStruct;
	insStruct.item = item;
	insStruct.hParent = hParent;
	insStruct.hInsertAfter = hInsertAfter;	
	
	return tc.InsertItem(&insStruct);
}

HTREEITEM TreeListView::MoveItem(HTREEITEM hItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	if(tc.ItemHasChildren(hItem))
		return NULL;
		
	TV_ITEM item;
	item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
	item.hItem = hItem;	
	tc.GetItem(&item);
	TreeListItem *pItem = (TreeListItem*)tc.GetItemData(hItem);
	TreeListItem *pNewItem = new TreeListItem(*pItem);
	DeleteOneItem(hItem);
	
	item.lParam = (LPARAM)pNewItem;

	TV_INSERTSTRUCT insStruct;
	insStruct.item = item;
	insStruct.hParent = hParent;
	insStruct.hInsertAfter = hInsertAfter;
	

	return tc.InsertItem(&insStruct);
}
/*
BOOL TreeListView::SetItemText( HTREEITEM hItem, int nCol ,LPCTSTR lpszItem )
{
	TreeListItem *pItem = (TreeListItem*)tc.GetItemData(hItem);
	if(!pItem)
		return FALSE;
	// pItem->SetItemText(lpszItem, nCol);
	return tc.SetItemData(hItem, (LPARAM)pItem);
}
*/
CString TreeListView::GetItemText( HTREEITEM hItem, int nSubItem )
{
	TreeListItem *pItem = (TreeListItem*)tc.GetItemData(hItem);
		
	if(!pItem)
		return _T("");
	return pItem->GetItemText(nSubItem);
}

void TreeListView::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	bool fInvalidateNeeded = false;
	int iWidth = 0;
	int iCol = 0;
	int iXpos = point.x - iOffset(); // in case we scrolled!
	while (iXpos > iWidth && iCol < iColumns())
	{
		iWidth += iGetColumnWidth(iCol);
		++iCol;
	}
	if (iXpos <= iWidth)
	{
		if (m_iActiveColumn != (iCol - 1))
		{
			m_iActiveColumn = iCol - 1;
			fInvalidateNeeded = true;
		}
	}
	else
	{
		if (m_iActiveColumn != -1)
		{
			m_iActiveColumn = -1;
			fInvalidateNeeded = true;
		}
	}

	UINT flags;
	HTREEITEM m_selectedItem = tc.HitTest(point, &flags);

	if (!((flags & TVHT_ONITEMRIGHT) || (flags & TVHT_ONITEMINDENT) || (flags & TVHT_ONITEM)))
		m_selectedItem = 0;

	if (tc.GetSelectedItem() != m_selectedItem)
	{
		tc.SelectItem(m_selectedItem);
		fInvalidateNeeded = false;
	}

	if (fInvalidateNeeded)
		tc.Invalidate();

	CView::OnLButtonDown(nFlags, point);
}

void TreeListView::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	// full replica of OnLButtonDown
	bool fInvalidateNeeded = false;
	int iWidth = 0;
	int iCol = 0;
	int iXpos = point.x - iOffset(); // in case we scrolled!
	while (iXpos > iWidth && iCol < iColumns())
	{
		iWidth += iGetColumnWidth(iCol);
		++iCol;
	}
	if (iXpos <= iWidth)
	{
		if (m_iActiveColumn != (iCol - 1))
		{
			m_iActiveColumn = iCol - 1;
			fInvalidateNeeded = true;
		}
	}
	else
	{
		if (m_iActiveColumn != -1)
		{
			m_iActiveColumn = -1;
			fInvalidateNeeded = true;
		}
	}

	UINT flags;
	HTREEITEM m_selectedItem = tc.HitTest(point, &flags);

	if (!((flags & TVHT_ONITEMRIGHT) || (flags & TVHT_ONITEMINDENT) || (flags & TVHT_ONITEM)))
		m_selectedItem = 0;

	if (tc.GetSelectedItem() != m_selectedItem)
	{
		tc.SelectItem(m_selectedItem);
		fInvalidateNeeded = false;
	}

	if (fInvalidateNeeded)
		tc.Invalidate();

	CView::OnRButtonDown(nFlags, point);
}

void TreeListView::OnLButtonUp(UINT nFlags, CPoint point) 
{		
	HTREEITEM m_selectedItem=tc.HitTest(point);	
	tc.SelectItem(m_selectedItem);

	CView::OnLButtonUp(nFlags, point);
}

void TreeListView::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if ((pNMTreeView->itemNew.hItem == tc.GetRootItem()) && (pNMTreeView->action != TVE_EXPAND))
		*pResult = 1;
	else
		*pResult = 0;
}

BOOL TreeListView::SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage)
{
	return tc.SetItemImage(hItem, nImage, nSelectedImage);
}

BOOL TreeListView::Expand(HTREEITEM hItem, UINT nCode)
{
	return tc.Expand(hItem, nCode);
}

HTREEITEM TreeListView::GetSelectedItem()
{
	return tc.GetSelectedItem();
}

BOOL TreeListView::SelectItem(HTREEITEM hItem)
{
	return tc.SelectItem(hItem);
}

HTREEITEM TreeListView::GetRootItem()
{
	return tc.GetRootItem();
}

HTREEITEM TreeListView::HitTest(CPoint pt, UINT* pFlags)
{
	return tc.HitTest(pt, pFlags);
}

void TreeListView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
}
