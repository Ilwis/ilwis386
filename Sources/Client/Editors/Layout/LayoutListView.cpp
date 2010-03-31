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
// LayoutListView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutListView.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\InPlaceEdit.h"
#include "Headers\Hs\Layout.hs"
#include "Headers\constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LayoutListView

BEGIN_MESSAGE_MAP(LayoutListView, CListView)
	//{{AFX_MSG_MAP(LayoutListView)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void MoveMouse(short xInc, short yInc) 
{
	mouse_event(MOUSEEVENTF_MOVE, xInc, yInc, 0, 0); 
}


LayoutListView::LayoutListView()
{
	fDragging = false;
	pDragImage = 0;
	fLabelEditing = false;
}

LayoutListView::~LayoutListView()
{
	if (pDragImage)
		delete pDragImage;
}

LayoutDoc* LayoutListView::GetDocument()
{
  return (LayoutDoc*)m_pDocument;
}

void LayoutListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


void LayoutListView::OnInitialUpdate() 
{
	// CListView::OnInitialUpdate(); // Would call OnUpdate() too early causing display problems on 95/98/ME
	CListCtrl& lvCtrl = GetListCtrl();
	lvCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	lvCtrl.InsertColumn(0, SLOColItem.scVal(), LVCFMT_LEFT, 200);
	lvCtrl.InsertColumn(1, SLOColLeft.scVal(), LVCFMT_RIGHT, 60);
	lvCtrl.InsertColumn(2, SLOColTop.scVal(), LVCFMT_RIGHT, 60);
	lvCtrl.InsertColumn(3, SLOColWidth.scVal(), LVCFMT_RIGHT, 60);
	lvCtrl.InsertColumn(4, SLOColHeight.scVal(), LVCFMT_RIGHT, 60);
	lvCtrl.InsertColumn(5, SLOColScale.scVal(), LVCFMT_LEFT, 80);
  IlwisWinApp *app = IlwWinApp();
	lvCtrl.SetImageList(&app->ilSmall, LVSIL_SMALL);
	lvCtrl.SetImageList(&app->ilLarge, LVSIL_NORMAL);
}

void LayoutListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint == LayoutDoc::hintPAGEBORDER) 
		return;

	LayoutDoc* ld = GetDocument();
	CListCtrl& lvCtrl = GetListCtrl();
  lvCtrl.SetItemCountEx((int)ld->lli.size());
	lvCtrl.InvalidateRect(NULL);

	vli.clear();
	copy(ld->lli.rbegin(), ld->lli.rend(), back_inserter(vli));	

	if (lHint == LayoutDoc::hintITEM) {
		POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
		if (pos) {
			int iSel = lvCtrl.GetNextSelectedItem(pos);
			lvCtrl.SetItemState(iSel, 0, 0x000F);
		}
		LayoutItem* li = dynamic_cast<LayoutItem*>(pHint);
		if (0 != li) {
			int iSel = find(vli.begin(), vli.end(), li) - vli.begin();
			lvCtrl.SetItemState(iSel, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);
			lvCtrl.EnsureVisible(iSel,FALSE);
		}
	}
}

BOOL LayoutListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= LVS_REPORT | LVS_OWNERDATA | LVS_SHAREIMAGELISTS;
	cs.style &= ~(LVS_SORTASCENDING | LVS_SORTDESCENDING);
	cs.style |= LVS_NOSORTHEADER | LVS_SINGLESEL;
	cs.style |= LVS_EDITLABELS | LVS_SHOWSELALWAYS;
	return CListView::PreCreateWindow(cs);
}

void LayoutListView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
  LVITEM& item = pDispInfo->item;	
	int iNr = item.iItem;
	String str;
  if (item.mask & LVIF_TEXT) {
		switch (item.iSubItem) {
			case 0:
				str = vli[iNr]->sName();
				if (str.length() > 100)
					str[100] = '\0';
				break;
			case 1: 
			case 2: 
			case 3: 
			case 4: 
			{
				int iVal = 0;
				MinMax mm = vli[iNr]->mmPosition();
				switch (item.iSubItem) {
					case 1: iVal = mm.MinCol(); break;
					case 2: iVal = mm.MinRow(); break;
					case 3: iVal = mm.width(); break;
					case 4: iVal = mm.height(); break;
				}
				double rVal = iVal / 10.0;
				str = String("%.1f", rVal);
			} break;
			case 5:
			{
				double rScale = vli[iNr]->rScale();
				if (rScale > 0)
					str = String("1:%.f", rScale);
			} break;
		}
		strcpy(item.pszText, str.scVal());
  }
  if (item.mask & LVIF_IMAGE) {
		String sType = vli[iNr]->sType();
    item.iImage = IlwWinApp()->iImage(sType);
	}
	*pResult = 0;
}

void LayoutListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* nmlv = (NMLISTVIEW*) pNMHDR;
	int iItem =	nmlv->iItem;
	if (iItem < 0)
		return;								
	vli[iItem]->OnItemEdit();
	*pResult = 0;
}

void LayoutListView::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

 	*pResult = 1; // Prevent editing
	if (m_iSubItem == -1)
		return;

	// name column is not editable
	if (m_iSubItem == 0)
		return;

	CListCtrl& lvCtrl = GetListCtrl();
	CString str = lvCtrl.GetItemText(m_iItem, m_iSubItem);
	if (str.IsEmpty())
		return;

	FrameWindow *frm = dynamic_cast<FrameWindow *>( GetParentFrame());
	if ( frm )
		frm->HandleAccelerators(false);  // disable accelerators when InPlace editting
	fLabelEditing = true;

	CRect rect;
	lvCtrl.GetSubItemRect(m_iItem, m_iSubItem, 0, rect);

	// Create the InPlaceEdit; there is no need to delete it afterwards, it will destroy itself
	DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL;
	m_Edit = new InPlaceEdit(m_iItem, m_iSubItem, str);
	m_Edit->Create(style, rect, this, 468);
}

void LayoutListView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	FrameWindow *frm = dynamic_cast<FrameWindow *>( GetParentFrame());
	if (frm)
		frm->HandleAccelerators(true);  // re-enable accelerators after InPlace editting
	fLabelEditing = false;

	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	int iHit = -1;
	if (pDispInfo->item.pszText != 0)
	{
		String sField(pDispInfo->item.pszText);
		double rVal = sField.rVal();
		int iVal = (int)(rVal * 10);

		MinMax mm = vli[m_iItem]->mmPosition();
		MinMax mmOld = mm;
		switch (m_iSubItem) {
			case 1: 
				iHit = CRectTracker::hitMiddle;
				mm.MaxCol() = iVal + mm.width(); 
				mm.MinCol() = iVal; 
				break;
			case 2: 
				iHit = CRectTracker::hitMiddle;
				mm.MaxRow() = iVal + mm.height(); 
				mm.MinRow() = iVal; 
				break;
			case 3: 
				iHit = CRectTracker::hitRight;
				mm.MaxCol() = mm.MinCol() + iVal; 
				break;
			case 4: 
				iHit = CRectTracker::hitBottom;
				mm.MaxRow() = mm.MinRow() + iVal; 
				break;
			case 5: 
			{
				if (sField[0] == '1' && sField[1] == ':') {
					String str = sField.sSub(2, sField.length());
					rVal = str.rVal();
				}
				if (rVal > 0)
					vli[m_iItem]->SetScale(rVal);
			} break;
		}
		if (mm != mmOld) {
			LayoutDoc* ld = GetDocument();
			ld->UpdateAllViews(this, LayoutDoc::hintITEM, vli[m_iItem]);
			vli[m_iItem]->SetPosition(mm,iHit);
			ld->UpdateAllViews(this, LayoutDoc::hintITEM, vli[m_iItem]);
		}
	}
//	if (m_Edit != 0)
//		delete m_Edit;
	*pResult = 0;
}

void LayoutListView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl& lvCtrl = GetListCtrl();

	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	LVHITTESTINFO hti;
	hti.pt = pt;
	lvCtrl.HitTest(&hti);

	if (hti.flags & LVHT_ONITEM) {
		m_iItem = hti.iItem;

		m_iSubItem = -1;

		// Now find the column in which the edit should take place
		LV_COLUMN lvc;
		lvc.mask = LVCF_WIDTH;
		int iCol = 0;
		CRect rcSubItem;
		while (lvCtrl.GetColumn(iCol, &lvc))
		{
			CRect rcItem;
			GetListCtrl().GetSubItemRect(m_iItem, iCol, LVIR_LABEL, rcItem);

			if (rcItem.PtInRect(pt))
			{
				m_iSubItem = iCol;
				rcSubItem = rcItem;
				break;
			}
			iCol++;	
		}
		GetListCtrl().EditLabel(m_iItem);
	}
	
	*pResult = 0;
}

void LayoutListView::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	CListCtrl& lvCtrl = GetListCtrl();
	POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
	pCmdUI->Enable(0 != pos);
}

void LayoutListView::OnEditClear() 
{
	CListCtrl& lvCtrl = GetListCtrl();
	POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
	if (pos) {
		int iSel = lvCtrl.GetNextSelectedItem(pos);
		LayoutDoc* ld = GetDocument();
		ld->RemoveItem(vli[iSel]);
	}
}


void LayoutListView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CListCtrl& lvCtrl = GetListCtrl();

	iDragItem = pNMListView->iItem;
	POINT pt;
	pt.x = 8;
	pt.y = 8;
	pDragImage = lvCtrl.CreateDragImage(iDragItem, &pt);
	pDragImage->BeginDrag(0, CPoint(8, 8));
	pDragImage->DragEnter(GetDesktopWindow(), pNMListView->ptAction);
	fDragging = true;
	SetCapture();
	CRect rect, rectHeader;
	lvCtrl.GetWindowRect(rect);
	ClipCursor(rect);
	*pResult = 0;
}

void LayoutListView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (fDragging) {
		CListCtrl& lvCtrl = GetListCtrl();
		CPoint pt = point;
		ClientToScreen(&pt);
		pDragImage->DragMove(pt);
		pDragImage->DragShowNolock(FALSE); // hide the image
		// SCROLL LIST IF NECESSARY
		int iOverItem = lvCtrl.HitTest(point);
		int iTopItem = lvCtrl.GetTopIndex();
		int iBottomItem = iTopItem + lvCtrl.GetCountPerPage() - 1;
		if (iOverItem == iTopItem && iTopItem != 0)
		{
			lvCtrl.EnsureVisible(iOverItem - 1, false);
			UpdateWindow();
			MoveMouse(0, 1);	// generate wm_mousemove but leave cursor in place:
			MoveMouse(0, -1);	// this is needed to detect that the cursor is on the top item
								// so the scrolling will continue
		}
		else if (iOverItem == iBottomItem && iBottomItem != (lvCtrl.GetItemCount() - 1))
		{
			lvCtrl.EnsureVisible(iOverItem + 1, false);
			UpdateWindow();
			MoveMouse(0, 1);	// generate wm_mousemove but leave cursor in place:
			MoveMouse(0, -1);	// this is needed to detect that the cursor is on the bottom item;
								// so the scrolling will continue
		}
		pDragImage->DragShowNolock(TRUE);	// reshow the image
	}	
	CListView::OnMouseMove(nFlags, point);
}

void LayoutListView::EndDrag()
{
	pDragImage->DragLeave(GetDesktopWindow());
	pDragImage->EndDrag();
	delete pDragImage; 
	pDragImage = 0;
	ReleaseCapture();
	ClipCursor(0);
	fDragging = false;
}
 
void LayoutListView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (fDragging) {
		EndDrag();
		LayoutDoc* ld = GetDocument();
		CListCtrl& lvCtrl = GetListCtrl();
		int iOverItem = lvCtrl.HitTest(point);
		ld->lli.remove(vli[iDragItem]);
		if (iOverItem < iDragItem) {
			ld->lli.insert(find(ld->lli.begin(), ld->lli.end(), vli[iOverItem]),vli[iDragItem]); 
		}
		else if (iOverItem > iDragItem) {
			ld->lli.insert(++find(ld->lli.begin(), ld->lli.end(), vli[iOverItem]),vli[iDragItem]); 
		}
		ld->UpdateAllViews(0, LayoutDoc::hintITEM, vli[iDragItem]);
	}
	CListView::OnLButtonUp(nFlags, point);
}

BOOL LayoutListView::PreTranslateMessage(MSG* pMsg) 
{
  if (!fLabelEditing && pMsg->message == WM_KEYDOWN)
	{
		CListCtrl& lvCtrl = GetListCtrl();
		POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
		int iItem = -1;
		if (pos) 
			iItem = lvCtrl.GetNextSelectedItem(pos);
		switch (pMsg->wParam)
		{
			case VK_ESCAPE:
				if (fDragging) {
					EndDrag();
					return TRUE;
				}
				lvCtrl.SetItemState(iItem, 0, 0x000F);
				break;
			case VK_RETURN:
				if (iItem > 0) {
					vli[iItem]->OnItemEdit();
					return TRUE;
				}
				break;
			case VK_DELETE:
				OnEditClear();
				return TRUE;
		}
	}
	return CListView::PreTranslateMessage(pMsg);
}

void LayoutListView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CListCtrl& lvCtrl = GetListCtrl();
	POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
	int iItem = -1;
	if (pos) {
		iItem = lvCtrl.GetNextSelectedItem(pos);
		vli[iItem]->OnContextMenu(pWnd, point);
	}
}

BOOL LayoutListView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (CListView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	CListCtrl& lvCtrl = GetListCtrl();
	POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
	int iItem = -1;
	if (pos) {
		iItem = lvCtrl.GetNextSelectedItem(pos);
		if (vli[iItem]->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}
	return FALSE;
}

