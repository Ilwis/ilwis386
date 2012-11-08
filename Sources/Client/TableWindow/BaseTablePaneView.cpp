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
// BaseTablePaneView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\engine.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "TableSelection.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Headers\constant.h"
#include "Headers\Hs\Table.hs"
#include "Headers\Hs\ILWISGEN.hs"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Base\Framewin.h"
//#include <afxole.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BaseTablePaneView

IMPLEMENT_DYNCREATE(BaseTablePaneView, CView)

BEGIN_MESSAGE_MAP(BaseTablePaneView, CView)
	//{{AFX_MSG_MAP(BaseTablePaneView)
	ON_COMMAND(ID_SELALL, OnSelectAll)
	ON_COMMAND(ID_COPY, OnEditCopy)
	ON_COMMAND(ID_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdateEditPaste)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(ILW_GOTOFIELD, OnGotoField)
	ON_COMMAND(ID_FILE_PRINT,	OnFilePrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void BaseTablePaneView::MoveMouse(short xInc, short yInc) 
{
	mouse_event(MOUSEEVENTF_MOVE, xInc, yInc, 0, 0); 
}



BaseTablePaneView::BaseTablePaneView()
: tField(0)
, curArrow(Arrow), curSplitCol("SplitColCursor"), curColumn("Clm16Cur")
, fColMove(false), fColMoving(false)
, fULButtonDown(false), fColButtonDown(false), fRowButtonDown(false)
, fFieldDown(false), fSelecting(false)    
, fShowHeading(true), fHeaderOnAllPages(true), fLeftMostColOnAllPages(true)
, m_PrintFont(NULL), m_PrintFontBold(NULL), fHasFocus(false)
{
	iFmtTbl = RegisterClipboardFormat("IlwisTable");

	iRec1 = 0;
	iCol1 = 0;
	iHght = 10;
	iColWidth = 0;
	iColPix = 0;
	iCharWidth = 0;
	iButtonWidth = 0;
	iColPix = NULL;
	iColWidth = NULL;

}

BaseTablePaneView::~BaseTablePaneView()
{
	if (iColWidth)
		delete [] iColWidth;
	if (iColPix)
		delete [] iColPix;  
	if (tField) {
		delete tField;
		tField = 0; 
	}
}

FrameWindow* BaseTablePaneView::fwParent()
{
	CFrameWnd* fw = GetTopLevelFrame();
	return dynamic_cast<FrameWindow*>(fw);
}

void BaseTablePaneView::InitColPix(CDC* cdc)
{
	selection.setSize(RowCol(iRows(), iCols()));

	if (0 != cdc) {
		TEXTMETRIC tm;
		cdc->GetTextMetrics(&tm);
		iCharWidth = tm.tmAveCharWidth + cdc->GetTextCharacterExtra();
	}
}

/////////////////////////////////////////////////////////////////////////////
// BaseTablePaneView drawing

void BaseTablePaneView::OnDraw(CDC* cdc)
{
	if ( iColPix == 0)
		return;
	CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfTABLE);
	CFont* fntOld = cdc->SelectObject(fnt);
	InitColPix(cdc);
	if (tField) tField->update();
	CPen* penOld;
	CBrush* brOld;
	CPen penNull(PS_NULL, 0, Color(0));
	CPen penWhite(PS_SOLID, 1, SysColor(COLOR_BTNHIGHLIGHT));
	Color colLtGray = SysColor(COLOR_BTNFACE);
	Color colBkGrnd = SysColor(COLOR_WINDOW);
	if (colLtGray == colBkGrnd)
		colLtGray = SysColor(COLOR_BTNSHADOW);
	CPen penLtGray(PS_SOLID, 1, colLtGray);
	CPen penDkGray(PS_SOLID, 1, SysColor(COLOR_BTNSHADOW));
	CBrush brWhite(SysColor(COLOR_WINDOW));
	CBrush brFace(SysColor(COLOR_BTNFACE));
	CBrush brHL(fHasFocus ? SysColor(COLOR_HIGHLIGHT) : SysColor(COLOR_SCROLLBAR));
	int c, iX, iY, iRecWidth;
	zRect rect;
	GetClientRect(&rect);  
	zPoint p;
	p.x = 0;
	p.y = 0;
	int iMaxRow = iRows();
	if (fShowHeading)
		iMaxRow += 1;
	int iRow = iFirstVisibleRow();

	// draw row and column button areas
	iRecWidth = iColPix[0];
	zRectRegion rr(&rect);
	cdc->SelectClipRgn(&rr);
	penOld = cdc->SelectObject(&penNull);
	brOld = cdc->SelectObject(&brFace);
	cdc->Rectangle(0,0,iRecWidth+1,rect.bottom()+1);
	if (fShowHeading)
		cdc->Rectangle(0,0,rect.right()+1,iHeight()+1);
	cdc->SetTextColor(SysColor(COLOR_BTNTEXT));
	cdc->SetBkMode(TRANSPARENT);

	// highlight selected area
	cdc->SelectObject(penOld);
	penOld = cdc->SelectObject(&penNull);
	cdc->SelectObject(brOld);
	brOld = cdc->SelectObject(&brHL);

	if (fShowHeading) 
	{
		// upper left button
		//zRectRegion regTmp(0,0,iColPix[0],iHeight());
		//cdc->SelectClipRgn(&regTmp);
		String s = sULButton();
		int iPos = 0;

		char *str, *sTmp;
		str = s.sVal();
		sTmp = strrchr(str, '.');
		if (sTmp) {
			try {
				zIcon icon(String("%s16Ico", sTmp+1).sVal());
				if ((HICON)icon != NULL) {
					DrawIcon(*cdc, iPos, 0, icon);
					*sTmp = '\0';
				}			
			}
			catch (ErrorObject&) {
				sTmp = 0;
			}
		}
		else {
			CSize sz = cdc->GetTextExtent(s.sVal(),s.length());
			if (sz.cx < iCharWidth * iButtonWidth - 5)
				iPos += (iCharWidth * iButtonWidth - sz.cx)/2;
			else
				iPos += 2;
			cdc->TextOut(iPos, 0, s.sVal(),s.length());
		}
		cdc->SelectObject(penOld);
		penOld = cdc->SelectObject(&penWhite);
		cdc->MoveTo(0,iHeight()-2);
		cdc->LineTo(0,0);
		cdc->LineTo(iRecWidth-1,0);
		cdc->SelectObject(penOld);
		penOld = cdc->SelectObject(&penDkGray);
		cdc->LineTo(iRecWidth-1,iHeight()-1);
		cdc->LineTo(-1,iHeight()-1);
	}
	// column lines
	cdc->SelectObject(penOld);
	penOld = cdc->SelectObject(&penDkGray);
	iX = iRecWidth;
	for (c = iFirstVisibleColumn(); iX < rect.right() && c < iCols(); ++c) {
		int iW = iColPix[c+1] - iColPix[c];
		if (iW > 0) {
			if (fShowHeading) 
			{
				zRectRegion regTmp(iX,0,iColPix[1+c],iHeight());
				cdc->SelectClipRgn(&regTmp);
				String s = sColButton(c);
				int iPos = iX;
				CSize sz = cdc->GetTextExtent(s.sVal(),s.length());
				if (sz.cx < iCharWidth*iColWidth[c])
					iPos += (iCharWidth*iColWidth[c] - sz.cx)/2;
				else
					iPos += 2;
				cdc->TextOut(iPos, -1, s.sVal(), s.length());
			}
			cdc->SelectClipRgn(&rr);
			iX = iColPix[1+c];
			if (fShowHeading)
				cdc->MoveTo(iX,iHeight()+1);
			else
				cdc->MoveTo(iX,1);
			cdc->LineTo(iX,rect.bottom());
		}
	}

	cdc->SelectClipRgn(&rr);
	iX = iRecWidth;
	cdc->SelectObject(penOld);
	penOld = cdc->SelectObject(&penDkGray);
	if (fShowHeading) {
		cdc->MoveTo(iX+1,iHeight()-1);
		cdc->LineTo(rect.right()+1,iHeight()-1);
		cdc->MoveTo(iX-1,iHeight()+1);
	}
	else
		cdc->MoveTo(iX-1,1);
	cdc->LineTo(iX-1,rect.bottom()+1);

	if (fShowHeading) 
	{
		for (c = iFirstVisibleColumn(); c < iLastVisibleColumn(); ++c) {
			cdc->SelectObject(penOld);
			penOld = cdc->SelectObject(&penWhite);
			int iNext = iColPix[1+c];
			cdc->MoveTo(iX+1,iHeight()-2);
			cdc->LineTo(iX+1,0);
			cdc->LineTo(iNext,0);
			iX = iNext;
			cdc->SelectObject(penOld);
			penOld = cdc->SelectObject(&penDkGray);
			cdc->MoveTo(iX,0);
			cdc->LineTo(iX,iHeight());
		}
	}
	if (fShowHeading) { 
		cdc->SelectObject(penOld);
		penOld = cdc->SelectObject(&penWhite);
		cdc->MoveTo(iX+1,iHeight()-2);
		cdc->LineTo(iX+1,0);
		cdc->LineTo(32000,0);
	}
	iRow = iFirstVisibleRow();
	for (iY = iHeight() * (rect.top()/iHeight()); iY < rect.bottom(); iY+=iHeight(), ++iRow) {
		if (iY == 0 && fShowHeading) 
			continue;
		if (iRow > iMaxRow)
			break;
		cdc->SelectObject(penOld);
		penOld = cdc->SelectObject(&penWhite);
		cdc->MoveTo(0,iY+iHeight());
		cdc->LineTo(0,iY+1);
		cdc->LineTo(iRecWidth-1,iY+1);
		cdc->SelectObject(penOld);
		penOld = cdc->SelectObject(&penDkGray);
		cdc->MoveTo(0,iY+iHeight());
		cdc->LineTo(iRecWidth-1,iY+iHeight());
	}

	{
		zRectRegion regTmp(0,0,iColPix[0]-1,rect.Height());
		cdc->SelectClipRgn(&regTmp);
		p.x += 1;
		//    p.y += 1;
		if (!fShowHeading)
			p.y -= iHeight();
		for (long r = iFirstVisibleRow(); r <= iRows(); ++r) {
			p.y += iHeight();
			String s = sRowButton(r);
			zPoint pnt = p;

			char *str, *sTmp;
			str = s.sVal();
			sTmp = strrchr(str, '.');
			if (sTmp) {
				try {
					zIcon icon(String("%s16Ico", sTmp+1).sVal());
					if ((HICON)icon != NULL) {
						DrawIcon(*cdc, pnt.x, pnt.y-1, icon);
						*sTmp = '\0';
						pnt.x += iHeight();
					} else {
						zIcon icon(String("%s", sTmp+1).sVal());
						if ((HICON)icon != NULL) {
							DrawIcon(*cdc, pnt.x, pnt.y-1, icon);
							*sTmp = '\0';
							pnt.x += iHeight();
						}
					}
				}
				catch (ErrorObject&) {
					sTmp = 0;
				}
			} 
			else {
				CSize sz = cdc->GetTextExtent(s.sVal(),s.length());
				if (sz.cx < iCharWidth*iButtonWidth-5)
					pnt.x += (iCharWidth*iButtonWidth - sz.cx)/2;
				else
					pnt.x += 1;
			}	  
			//pnt.x() += iRecWidth - sz.cx - 3;
			cdc->TextOut(pnt.x, pnt.y, str);
			if (p.y > rect.bottom()) break;
		}
	}

	cdc->SetBkMode(OPAQUE);
	p.x += iRecWidth - 1;
	Color colHighLight = SysColor(COLOR_HIGHLIGHT);
	if (!fHasFocus) {
		colHighLight = SysColor(COLOR_SCROLLBAR);
		colHighLight.red() = min(colHighLight.red()+32, 255);
		colHighLight.green() = min(colHighLight.green()+32, 255);
		colHighLight.blue() = min(colHighLight.blue()+32, 255);
	}

	for (c = iFirstVisibleColumn(); c < iCols(); ++c) {
		int iW = iColPix[c+1] - iColPix[c];
		if (iW > 0) {
			if (p.x + iW >= rect.left()) {
				p.y = 0;
				zRectRegion regTmp(p.x,0,p.x+iW-2,rect.bottom());
				cdc->SelectClipRgn(&regTmp);
				if (!fShowHeading) 
					p.y -= iHeight();
				for (long r = iFirstVisibleRow(); r <= iRows(); ++r) 
				{
					p.y += iHeight();
					if (selection.fContains(RowCol(r,(long)c))) {
						if (fHasFocus) {
							cdc->SetTextColor(SysColor(COLOR_HIGHLIGHTTEXT));
							cdc->SetBkColor(colHighLight);
						}
						else {
							cdc->SetTextColor(SysColor(COLOR_WINDOWTEXT));
							cdc->SetBkColor(colHighLight);
						}
						zRectRegion regTmp(p.x, p.y, p.x+iW, p.y+iHeight());
						cdc->SelectClipRgn(&regTmp);
						cdc->Rectangle(p.x, p.y, p.x+iW+1, p.y+iHeight()+1);
					}
					else {
						cdc->SetTextColor(SysColor(COLOR_WINDOWTEXT));
						cdc->SetBkColor(SysColor(COLOR_WINDOW));
					}
					String s = sField(c,r);
					zPoint pnt = p;
					pnt.x += 3;
					if ( iColWidth[c] == iUNDEF)
						continue;
					if (s.length() < (unsigned int)iColWidth[c])
						s &= String(' ', iColWidth[c] - s.length());
					zRectRegion regTmp(pnt.x,pnt.y+1,pnt.x+iW-5,pnt.y+iHeight());
					cdc->SelectClipRgn(&regTmp);
					cdc->TextOut(pnt.x, pnt.y, s.sVal(), s.length());
					if (p.y > rect.bottom()) break;
				}
			}
			p.x += iW;
			if (p.x > rect.right()) break;
		}
	}
	cdc->SelectClipRgn(0);

	iRecWidth = iColPix[0];

	// draw lines
	iX = iRecWidth;
	cdc->SelectObject(penOld);
	penOld = cdc->SelectObject(&penLtGray);
	if (fShowHeading)
		cdc->MoveTo(iX,iHeight()+1);
	else
		cdc->MoveTo(iX,1);
	cdc->LineTo(iX,rect.bottom());
	iRow = iFirstVisibleRow() - 1;
	for (iY = iHeight() * (rect.top()/iHeight()); iY < rect.bottom(); iY+=iHeight(), ++iRow) {
		if (iY == 0	&& fShowHeading) 
			continue;
		if (iRow > iMaxRow)
			break;
		cdc->MoveTo(iX,iY);
		cdc->LineTo(rect.right(),iY);
	}
	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
	cdc->SelectObject(fntOld);
}

/////////////////////////////////////////////////////////////////////////////
// BaseTablePaneView diagnostics

#ifdef _DEBUG
void BaseTablePaneView::AssertValid() const
{
	CView::AssertValid();
}

void BaseTablePaneView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// BaseTablePaneView message handlers


void BaseTablePaneView::ColRow(zPoint p, int& iCol, int& iRow)
{
	if ( iColPix == 0 )
		return;
	iRow = p.y / iHeight();
	if (!fShowHeading)
		iRow += 1;
	iCol = -1;
	while (iColPix[iCol+1] < p.x) ++iCol;
	while (iCol > 0 && iColPix[iCol] == iColPix[iCol+1]) --iCol;
}

long BaseTablePaneView::iFirstRec() const
{
	return iRec1;
}

long BaseTablePaneView::iFirstCol() const
{
	return iCol1;
}

long BaseTablePaneView::iFirstVisibleRow() const
{
	return iFirstRec() + 1;
}

long BaseTablePaneView::iLastVisibleRow() const
{
	zRect rect;
	GetClientRect(&rect);
	return iFirstVisibleRow() + rect.height() / iHeight() - 2;
}

int BaseTablePaneView::iFirstVisibleColumn() const
{
	return iFirstCol();
}

int BaseTablePaneView::iLastVisibleColumn() const
{
	if (0 == iColPix || iCharWidth == 0)
		return iFirstVisibleColumn();
	zRect rect;
	GetClientRect(&rect);
	int iW = rect.width();
	int iLast = iCols();
	for (int i = iFirstVisibleColumn(); i < iLast; ++i)
		if (iColPix[i+1] >= iW) {
			iLast = i;
			break;
		}
		return iLast;
}

zPoint BaseTablePaneView::pntField(int iCol, long iRec) const
{
	zPoint pnt(-32767,-32767);
	if (iCol < iFirstVisibleColumn()) return pnt;
	if (iRec < iFirstVisibleRow()) return pnt;
	if (iCol > iLastVisibleColumn()) return pnt;
	if (iRec > iLastVisibleRow()) return pnt;
	pnt.x = iColPix[iCol];
	long r = iRec - iFirstVisibleRow() + 1;
	if (r > 0)
		pnt.y = iHeight() * r;
	return pnt;
}

zRect BaseTablePaneView::rectField(int iCol, long iRec) const
{
	zPoint pnt = pntField(iCol,iRec);
	zPoint pnt2 = pnt;
	pnt.x += 1;
	pnt.y += 1;
	long dx;
	dx = long(iCharWidth)*iColWidth[iCol] + 5;
	if (dx > 20000) dx = 20000;
	pnt2.x += dx;
	pnt2.y += iHeight();

	zRect rect(pnt,pnt2);
	return rect;
}

void BaseTablePaneView::OnSelectAll()
{
	selection.selectBlock(RowCol(1L,0L),RowCol(iRows(), iCols() - 1));
	Invalidate();
}

zRect BaseTablePaneView::rectBlock(const TableSelection& sel) const
{
	zRect rect;
	rect.SetRectEmpty();
	if ( !sel.fValid()) return rect;
	if (sel.minCol() > iLastVisibleColumn()  ||
		sel.maxCol() < iFirstVisibleColumn() ||
		sel.minRow() > iLastVisibleRow()	  ||
		sel.maxRow() < iFirstVisibleRow()) return rect;

	int iCol = sel.minCol();
	if (iCol < iFirstVisibleColumn())
		iCol = 0;
	rect.left() = iColPix[iCol];
	iCol = sel.maxCol();
	if (iCol > iCols())
		iCol = iCols();
	rect.right() = iColPix[iCol+1];
	long iRow = 1 + sel.minRow() - iFirstVisibleRow();
	if (iRow < 1)
		iRow = 1;
	if (!fShowHeading)
		iRow--;
	rect.top() = iRow * iHeight() - 1;
	iRow = sel.maxRow() - iFirstVisibleRow();
	if (iRow > 1e6)
		rect.bottom() = 5000;
	else {
		iRow += 2;
		if (!fShowHeading)
			iRow--;
		rect.bottom() = shortConv(iRow * iHeight() + 1);
	}
	return rect;
}

zRect BaseTablePaneView::rectSelect() const
{
	return rectBlock(selection);
}

long BaseTablePaneView::iCols() const
{
	return 0;
}

long BaseTablePaneView::iRows() const
{
	return 0;
}

String BaseTablePaneView::sULButton() const
{
	return "";
}

String BaseTablePaneView::sColButton(int) const
{
	return "";
}

String BaseTablePaneView::sRowButton(long) const
{
	return "";
}

String BaseTablePaneView::sField(int, long) const
{
	return "";
}

String BaseTablePaneView::sDescrULButton() const // upper left button
{
	return sULButton();
}

String BaseTablePaneView::sDescrColButton(int iCol) const
{
	return sColButton(iCol);
}

String BaseTablePaneView::sDescrRowButton(long iRow) const
{
	return sRowButton(iRow);
}

String BaseTablePaneView::sDescrField(int iCol, long iRow) const
{
	return sField(iCol,iRow);
}



void BaseTablePaneView::OnULButtonPressed()
{
	//  MessageBeep(MB_ICONASTERISK);
	//  zMessage msg(this,"Not yet implemented","Upper Left Button");
}

void BaseTablePaneView::OnColButtonPressed(int)
{
	//  MessageBeep(MB_ICONASTERISK);
	//  zMessage msg(this,"Not yet implemented","Column Button");
}

void BaseTablePaneView::OnRowButtonPressed(long)
{
 //IlwWinApp()->Send
}

void BaseTablePaneView::OnFieldPressed(int, long, bool)
{
	//  MessageBeep(MB_ICONASTERISK);
}

void BaseTablePaneView::SetScrollBars()
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
	si.nMin = 0;
	si.nMax = iRows()-1;
	if (fShowHeading)
		si.nMax++;
	CRect rect;
	GetClientRect(&rect);
	si.nPage = rect.Height() / iHeight();
	if (si.nPage > (unsigned int)iRows())
		iRec1 = 0;
	si.nPos = iRec1;
	SetScrollInfo(SB_VERT, &si);
	si.nMin = 0;
	si.nMax = iCols() - 1;
	if (iCols() == 1)
		si.fMask = SIF_ALL;
	if (iCol1 >= iCols())	{
		iCol1 = iCols() - 1;
		if (iCol1 < 0)
			iCol1 = 0;
	}
	InitColPix(0);
	si.nPage = iLastVisibleColumn() - iFirstVisibleColumn();
	si.nPos = iCol1;
	SetScrollInfo(SB_HORZ, &si);
}


void BaseTablePaneView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	SetScrollBars();
	if (lHint == uhNOBUTTONS) {
		CRect rect;
		GetClientRect(rect);
		if (fShowHeading)
			rect.top += iHeight()+1;
		rect.left += iChrWidth() * (iButWidth()+1);
		InvalidateRect(&rect,FALSE);
	}
	else
		Invalidate();
}

void BaseTablePaneView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	if (cx <= 0 || cy <= 0)
		return;
	SetScrollBars();
	if (tField) tField->update();
}

void BaseTablePaneView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch (nSBCode)
	{
	case SB_TOP:
		horzLeft();
		break;
	case SB_BOTTOM:
		horzRight();
		break;
	case SB_LINEUP:
		horzLineMove(-1);
		break;
	case SB_LINEDOWN:
		horzLineMove(1);
		break;
	case SB_PAGEUP:
		horzPageMove(-1);
		break;
	case SB_PAGEDOWN:
		horzPageMove(1);
		break;
	case SB_THUMBTRACK:
		{ SCROLLINFO si;
		GetScrollInfo(SB_HORZ, &si, SIF_TRACKPOS);
		horzThumbPos(si.nTrackPos);
		}	break;
	}
}

void BaseTablePaneView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch (nSBCode)
	{
	case SB_TOP:
		vertTop();
		break;
	case SB_BOTTOM:
		vertBottom();
		break;
	case SB_LINEUP:
		vertLineMove(-1);
		break;
	case SB_LINEDOWN:
		vertLineMove(1);
		break;
	case SB_PAGEUP:
		vertPageMove(-1);
		break;
	case SB_PAGEDOWN:
		vertPageMove(1);
		break;
	case SB_THUMBTRACK:
		{ SCROLLINFO si;
		GetScrollInfo(SB_VERT, &si, SIF_TRACKPOS);
		vertThumbPos(si.nTrackPos);
		}	break;
	}
}

BOOL BaseTablePaneView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	bool fControl = nFlags & MK_CONTROL ? true : false;
	bool fShift = nFlags & MK_SHIFT ? true : false;

	if (!fControl)
		if (!fShift)
			vertPageMove(zDelta > 0 ? -1 : 1);
		else
			horzPageMove(zDelta > 0 ? -1 : 1);

	return TRUE;
}

int BaseTablePaneView::vertLineMove(int iMove)
{
	return vertPixMove(iMove);
}

int BaseTablePaneView::vertPageMove(int iMove)
{
	iMove *= 10;
	return vertPixMove(iMove);
}

int BaseTablePaneView::vertTop()
{
	if (iRec1 == 0) return 0;
	return vertPixMove(-iRec1);
}

int BaseTablePaneView::vertBottom()
{
	int iRecs = iRows();
	if (iRec1 == iRecs - 1) return 0;
	return vertPixMove(iRecs - 1 - iRec1);
}

int BaseTablePaneView::vertThumbPos(int nPos)
{
	return vertPixMove(nPos - iRec1);
}

int BaseTablePaneView::horzLineMove(int iMove)
{
	return horzPixMove(iMove);
}

int BaseTablePaneView::horzPageMove(int iMove)
{
	if (iMove > 0)
		iMove = iLastVisibleColumn() - iFirstVisibleColumn();
	if (iMove == 0)
		iMove = 1;
	if (iMove < 0)
		iMove = -4;
	return horzPixMove(iMove);
}

int BaseTablePaneView::horzLeft()
{
	iCol1 = 0;
	SetScrollBars();
	Invalidate();
	return 0;
}

int BaseTablePaneView::horzRight()
{
	iCol1 = iCols() - 1;
	SetScrollBars();
	Invalidate();
	return 0;
}

int BaseTablePaneView::horzThumbPos(int nPos)
{
	return horzPixMove(nPos - iCol1);
}

int BaseTablePaneView::vertPixMove(long iDiff)
{
	long iRecs = iRows();
	long iRecOld = iRec1;
	iRec1 += iDiff;
	if (iRec1 >= iRecs) {
		iDiff -= iRec1 - iRecs + 1;
		iRec1 = iRecs - 1;
	}
	if (iRec1 < 0) {
		iDiff -= iRec1;
		iRec1 = 0;
	}
	if (iRec1 != iRecOld) {
		SetScrollBars();
		zRect rect;
		GetClientRect(&rect);
		rect.top() += iHeight() + 1;
		long iWinDiff = iHeight() * iDiff;
		if (abs(iWinDiff) < rect.height())
			ScrollWindow(0,-iWinDiff,&rect,&rect);
		else
			Invalidate();
		SetScrollBars();
	}
	return 0;
}

int BaseTablePaneView::horzPixMove(long iDiff)
{
	long iColOld = iCol1;
	iCol1 += iDiff;
	if (iCol1 >= iCols()) iCol1 = iCols() - 1;
	if (iCol1 < 0) iCol1 = 0;
	if (iCol1 != iColOld) {
		SetScrollBars();
		Invalidate();
		SetScrollBars();
	}
	return 0;
}


void BaseTablePaneView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	bool fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	switch (nChar) 
	{
		// ESC = remove selection
	case VK_ESCAPE: {
		zRect rect = rectSelect();
		selection.reset();
		if (!rect.IsRectEmpty())
			InvalidateRect(&rect);
					} return;
		// scrolling
	case VK_PRIOR:
		if (fCtrl)
			vertTop();
		else
			vertPageMove(-1);
		return;
	case VK_NEXT:      
		if (fCtrl)
			vertBottom();
		else
			vertPageMove(1);
		return;
	case VK_HOME:  
		if (fCtrl)
			horzLeft();
		else
			horzPageMove(-1);
		return;
	case VK_END:  
		if (fCtrl)
			horzRight();
		else
			horzPageMove(1);
		return;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void BaseTablePaneView::OnMouseMove(UINT nFlags, CPoint point) 
{
	//CView::OnMouseMove(nFlags, point);
	int iCol, iRow;
	ColRow(point,iCol,iRow);
	if (nFlags & MK_LBUTTON) {
		if (fColMoving) {
			SetCursor(curSplitCol);
			if (point.x != iColPixMoving) {
				drawColMoving();
				iColPixMoving = point.x;
				drawColMoving();
			}
		}
		else if (fSelecting) {
			zRect rectInterior;
			GetClientRect(&rectInterior);

			bool fChange = false;
			int xInc = 0;
			int yInc = 0;
			if (iColMoving >= 0 && point.x < 0) {
				horzPixMove(-1);
				fChange = true;
				xInc = 1;
			}
			if (iColMoving >= 0 && point.x > rectInterior.right()) {
				horzPixMove(1);
				fChange = true;
				xInc = -1;
			}
			if (iRowMoving > 0 && point.y < 0) {
				int iDiff = iRow;
				if (iDiff > -1)
					iDiff = -1;
				vertPixMove(iDiff);
				fChange = true;
				yInc = 1;
			}
			if (iRowMoving > 0 && point.y > rectInterior.bottom()) {
				int iRowMax = rectInterior.bottom() / iHeight();
				int iDiff = iRow - iRowMax;
				if (iDiff < 1) {
					iDiff = 1;
					iRow = iRowMax + 1;
				}
				vertPixMove(iDiff);
				fChange = true;
				yInc = -1;
			}
			if (fChange) {
				UpdateWindow();
				MoveMouse(xInc, yInc);
			}

			if (pSelectStart.x != 0 || pSelectStart.y != 0) {
				if (abs(pSelectStart.x - point.x) <= 5 &&
					abs(pSelectStart.y - point.y) <= 5) {
						return;
				}
				else {
					pSelectStart.x = 0;
					pSelectStart.y = 0;
					if (fColButtonDown) {
						fColButtonDown = false;
						SetCursor(curArrow);
						ReleaseColButton();
					}
					else if (fULButtonDown) {
						fULButtonDown = false;
						ReleaseULButton();
					}
					else if (fRowButtonDown) {
						fRowButtonDown = false;
						ReleaseRowButton();
					}
					else if (fFieldDown)
						fFieldDown = false;
				}
			}
			long iRec = iRow + iFirstVisibleRow() - 1;
			TableSelection selNew = selection;
			fChange = false;
			if (iRowMoving >= 0) {

				if (iRec >= iRowMoving) {
					if (selNew.minRow() != iRowMoving) {
						fChange = true;
						selNew.setMinRow(iRowMoving);
					}
					if (selNew.maxRow() != iRec) {
						fChange = true;
						selNew.setMaxRow(iRec);
					}
				}
				else {
					if (selNew.maxRow() != iRowMoving) {
						fChange = true;
						selNew.setMaxRow(iRowMoving);
					}
					if (selNew.minRow() != iRec) {
						fChange = true;
						selNew.setMinRow(iRec);
					}
				}
			}
			if (iColMoving >= 0) {
				if (iCol >= iColMoving) {
					if (selNew.minCol() != iColMoving) {
						fChange = true;
						selNew.setMinCol(iColMoving);
					}
					if (selNew.maxCol() != iCol) {
						fChange = true;
						selNew.setMaxCol(iCol);
					}
				}
				else {
					if (selNew.maxCol() != iColMoving) {
						fChange = true;
						selNew.setMaxCol(iColMoving);
					}
					if (selNew.minCol() != iCol) {
						fChange = true;
						selNew.setMinCol(iCol);
					}
				}
			}
			if (fChange) {
				TableSelection selMax;
				TableSelection selDiff;
				zRect rDiff;
				selMax = selection;
				selMax += selNew;
				if (selNew.minCol() != selection.minCol()) {
					selDiff = selMax;
					selDiff.setMinCol(min(selNew.minCol(), selection.minCol()));
					selDiff.setMaxCol(max(selNew.minCol(), selection.minCol()) - 1);
					rDiff = rectBlock(selDiff);
					InvalidateRect(&rDiff);
				}
				if (selNew.maxCol() != selection.maxCol()) {
					selDiff = selMax;
					selDiff.setMaxCol(max(selNew.maxCol(), selection.maxCol()));
					selDiff.setMinCol(min(selNew.maxCol(), selection.maxCol()) + 1);
					rDiff = rectBlock(selDiff);
					InvalidateRect(&rDiff);
				}
				if (selNew.minRow() != selection.minRow()) {
					selDiff = selMax;
					selDiff.setMinRow(min(selNew.minRow(), selection.minRow()));
					selDiff.setMaxRow(max(selNew.minRow(), selection.minRow()) - 1);
					rDiff = rectBlock(selDiff);
					InvalidateRect(&rDiff);
				}
				if (selNew.maxRow() != selection.maxRow()) {
					selDiff = selMax;
					selDiff.setMaxRow(max(selNew.maxRow(), selection.maxRow()));
					selDiff.setMinRow(min(selNew.maxRow(), selection.maxRow())); // + 1
					rDiff = rectBlock(selDiff);
					InvalidateRect(&rDiff);
				}
				if (iRec == 2 && iCol == 2) {
					TRACE("STOP");
				}
				selection = selNew;
			}
		} // fSelecting
	}
	else {    // left button not down
		CWnd* pMessageBar = GetParentFrame()->GetMessageBar();
		String s;
		iColMoving = -1;
		if (iRow <= 0) {
			for (int c = 0;; ++c) {
				if (c > iCols()) {
					iColMoving = c-1;
					break;
				}
				iColMoving = c - 1;
				if (point.x < iColPix[c] - 2) break;
				if (point.x > iColPix[c] + 2) continue;
				fColMove = true;
				SetCursor(curSplitCol);
				s = TR("Change column width");
				if (pMessageBar)
					pMessageBar->SetWindowText(s.sVal());
				return;
			}
			if (iCol < 0)
				s = sDescrULButton();
			else if (iCol < iCols())
				s = sDescrColButton(iCol);
			else
				s = String("");
			if (pMessageBar)
				pMessageBar->SetWindowText(s.sVal());
		}
		else {   // iRow > 0
			if (iCol < 0) {
				long iR = iRow + iFirstVisibleRow() - 1;
				if (iR > iRows())
					s = String("");
				else
				{
					s = sDescrRowButton(iR);
					// Now use same solution as in OnPaint() to determine
					// if the string is a filename so that we strip-off the
					// extension (if it exists)
					// Don't do anything with the icon though (just attempt
					// to create it: if this goes wrong, it means we're dealing
					// with a non-filename that by coincidence contains a dot.
					char *str, *sTmp;
					str = s.sVal();
					sTmp = strrchr(str, '.');
					if (sTmp)
					{
						try
						{
							zIcon icon(String("%s16Ico", sTmp+1).sVal());
							if ((HICON)icon != NULL)
								*sTmp = '\0';
						}
						catch (ErrorObject&)
						{
							// Do nothing .. just the icon creation failed
						}
					}
				}
			}
			else {
				long iR = iRow + iFirstVisibleRow() - 1;
				if (iR > iRows() || iCol >= iCols())
					s = String("");
				else
					s = sDescrField(iCol,iR);
			}
			if (pMessageBar)
				pMessageBar->SetWindowText(s.sVal());
		}
		if (fColMove) {
			fColMove = false;
			SetCursor(curArrow);
		}
	}
}

void BaseTablePaneView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDown(nFlags, point);
	deleteField();
	zRect rect = rectSelect();
	if (!rect.IsRectEmpty())
		InvalidateRect(&rect);
	int iCol, iRow;
	ColRow(point,iCol,iRow);
	if (iRow+iFirstVisibleRow()-1 > iRows())
		return;

	SetCapture();
	bool fShift = nFlags & MK_SHIFT ? true : false;
	if (fColMove) {
		SetCursor(curSplitCol);
		fColMoving = true;
		iColPixMoving = point.x;
		drawColMoving();
		return;
	}
	else if (iColMoving >= 0) {
		fColButtonDown = true;
		if (!fShift) 
			PushColButton();
	}
	if (iCol < 0) {
		if (iRow <= 0) {
			fULButtonDown = true;
			if (!fShift) 
				PushULButton();
		}
		else {
			iRowMoving = iRow + iFirstVisibleRow() - 1;
			fRowButtonDown = true;
			if (!fShift) 
				PushRowButton();
		}
	}
	else if (iRow > 0) {
		fFieldDown = true;
	}

	if (iColMoving >= iCols())
		return;
	fSelecting = true;
	if (fColButtonDown && !fShift && fAllowMoveCol()) {
		fSelecting = false;
		if (iCols() > 1)
			SetCursor(curColumn);
	}  
	if (fShift) {
		pSelectStart.x = -100;
		if (iRow >= selection.minRow())
			iRowMoving = selection.minRow();
		else
			iRowMoving = selection.maxRow();
		if (iCol > selection.minCol()) 
			iColMoving = selection.minCol();
		else 
			iColMoving = selection.maxCol();

		MoveMouse(1,0);
	}
	else {
		pSelectStart = point;
		iRowMoving = iRow + iFirstVisibleRow() - 1;
		iColMoving = iCol;
		String s;
		if (iRow <= 0) {
			if (iCol < 0) {
				selection.selectBlock(RowCol(1L,0L), RowCol(iRowMoving - 1L, iCol - 1L));
				iRowMoving = 0;
				iColMoving = 0;
				s = TR("Select a block");
			}
			else {
				selection.selectBlock(RowCol(1L,iColMoving), RowCol(iRows(), iColMoving)); 
				iRowMoving = -1;
				s = TR("Select a block of columns");
			}
		}
		else {
			if (iCol < 0) {
				selection.selectBlock(RowCol(iRowMoving,0L), RowCol(iRowMoving, iCols() - 1));
				iColMoving = -1;
				s = TR("Select a block of records");
			}
			else {
				selection.selectBlock(RowCol(iRowMoving, iColMoving), RowCol(iRowMoving, iColMoving));
				s = TR("Select a block");
			}
		}
		CWnd* pMessageBar = GetParentFrame()->GetMessageBar();
		if (pMessageBar)
			pMessageBar->SetWindowText(s.sVal());
	}
	rect = rectSelect();
	InvalidateRect(&rect);
}	

void BaseTablePaneView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	bool fShift = nFlags & MK_SHIFT ? true : false;
	ReleaseCapture();
	if (fColMoving) {
		fColMoving = false;
		drawColMoving();
		int iDiff = point.x - iColPix[iColMoving+1];
		if (abs(iDiff) > 0) {
			if (iDiff > 0)
				iDiff = iDiff / iCharWidth + 1;
			else
				iDiff = iDiff / iCharWidth - 1;
			if (iColMoving == -1) {
				iButtonWidth += iDiff;
				if (iButtonWidth < 2) iButtonWidth = 2;
			}
			else {
				iColWidth[iColMoving] += iDiff;
				if (iColWidth[iColMoving] < 2) iColWidth[iColMoving] = 2;
			}
			Invalidate();
			CDocument* doc = GetDocument();
			if (0 != doc)
				doc->UpdateAllViews(this, iColMoving==-1 ? 0:1);
		}
	}
	else if (fColButtonDown) {
		fColButtonDown = false;
		ReleaseColButton();
		SetCursor(curArrow);
		if (!fSelecting) {
			if (iColMoving < iCols()) {
				int iCol, iRow;
				ColRow(point,iCol,iRow);
				if (iCol != iColMoving)
					MoveCol(iColMoving, iCol);
			}
			else  
				OnColButtonPressed(iColMoving);
		}
	}
	else if (fULButtonDown) {
		fULButtonDown = false;
		ReleaseULButton();
		OnULButtonPressed();
	}
	else if (fRowButtonDown) {
		fRowButtonDown = false;
		ReleaseRowButton();
		OnRowButtonPressed(iRowMoving);
	}
	else if (fFieldDown) {
		fFieldDown = false;
		if (!fShift && iColMoving < iCols()) {
			int iCol, iRow;
			ColRow(point,iCol,iRow);
			OnFieldPressed(iCol,iRow+iFirstVisibleRow()-1);
		}  
	}
	else if (fSelecting) {
		fSelecting = false;
	}
	updateSelection();
}

void BaseTablePaneView::deleteField()
{
	if (tField) {
		delete tField;
		tField = 0;
	}
}


void BaseTablePaneView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (iColMoving >= 0) {
		fColButtonDown = false;
		ReleaseColButton();
		SetCursor(curArrow);
		OnColButtonPressed(iColMoving);
	}
}

void BaseTablePaneView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	deleteField();
	zRect rect = rectSelect();
	if (!rect.IsRectEmpty())
		InvalidateRect(&rect);
	int iCol, iRow;
	ColRow(point,iCol,iRow);
	if (iCol > 0 && iRow > 0)
		OnFieldPressed(iCol,iRow+iFirstVisibleRow()-1,false);
}


void BaseTablePaneView::drawColMoving()
{
	CClientDC cdc(this);
	int dmOld = cdc.SetROP2(R2_NOT);
	cdc.MoveTo(iColPixMoving, 0);
	cdc.LineTo(iColPixMoving, 32000);
	cdc.SetROP2(dmOld);
}

void BaseTablePaneView::PushColButton()
{
	SetCapture();
	CClientDC cdc(this);
	CPen penDark(PS_SOLID,1,SysColor(COLOR_BTNSHADOW));
	CPen penLight(PS_SOLID,1,SysColor(COLOR_BTNHIGHLIGHT));
	CPen *penOld = cdc.SelectObject(&penDark);
	int iX = iColPix[iColMoving];
	int iNext = iColPix[1+iColMoving];
	cdc.MoveTo(iX+1,iHeight()-2);
	cdc.LineTo(iX+1,0);
	cdc.LineTo(iNext,0);
	cdc.SelectObject(penOld);
	penOld = cdc.SelectObject(&penLight);
	cdc.LineTo(iNext,iHeight()-1);
	cdc.LineTo(iX,iHeight()-1);
	cdc.SelectObject(penOld);
}

void BaseTablePaneView::ReleaseColButton()
{
	CClientDC cdc(this);
	CPen penDark(PS_SOLID,1,SysColor(COLOR_BTNSHADOW));
	CPen penLight(PS_SOLID,1,SysColor(COLOR_BTNHIGHLIGHT));
	CPen *penOld = cdc.SelectObject(&penLight);
	int iX = iColPix[iColMoving];
	int iNext = iColPix[1+iColMoving];
	cdc.MoveTo(iX+1,iHeight()-2);
	cdc.LineTo(iX+1,0);
	cdc.LineTo(iNext,0);
	cdc.SelectObject(penOld);
	penOld = cdc.SelectObject(&penDark);
	cdc.MoveTo(iNext,0);
	cdc.LineTo(iNext,iHeight()-1);
	cdc.LineTo(iX,iHeight()-1);
	cdc.SelectObject(penOld);
}

void BaseTablePaneView::PushULButton()
{
	CClientDC cdc(this);
	int iRecWidth = iColPix[0];
	SetCapture();
	CPen penDark(PS_SOLID,1,SysColor(COLOR_BTNSHADOW));
	CPen penLight(PS_SOLID,1,SysColor(COLOR_BTNHIGHLIGHT));
	CPen *penOld = cdc.SelectObject(&penDark);
	cdc.MoveTo(0,iHeight()-2);
	cdc.LineTo(0,0);
	cdc.LineTo(iRecWidth-1,0);
	cdc.SelectObject(penOld);
	penOld = cdc.SelectObject(&penLight);
	cdc.LineTo(iRecWidth-1,iHeight()-1);
	cdc.LineTo(-1,iHeight()-1);
	cdc.SelectObject(penOld);
}

void BaseTablePaneView::ReleaseULButton()
{
	CClientDC cdc(this);
	int iRecWidth = iColPix[0];
	CPen penDark(PS_SOLID,1,SysColor(COLOR_BTNSHADOW));
	CPen penLight(PS_SOLID,1,SysColor(COLOR_BTNHIGHLIGHT));
	CPen *penOld = cdc.SelectObject(&penLight);
	cdc.MoveTo(0,iHeight()-2);
	cdc.LineTo(0,0);
	cdc.LineTo(iRecWidth-1,0);
	cdc.SelectObject(penOld);
	penOld = cdc.SelectObject(&penDark);
	cdc.LineTo(iRecWidth-1,iHeight()-1);
	cdc.LineTo(-1,iHeight()-1);
	cdc.SelectObject(penOld);
}

void BaseTablePaneView::PushRowButton()
{
	SetCapture();
	CClientDC cdc(this);
	CPen penDark(PS_SOLID,1,SysColor(COLOR_BTNSHADOW));
	CPen penLight(PS_SOLID,1,SysColor(COLOR_BTNHIGHLIGHT));
	CPen *penOld = cdc.SelectObject(&penDark);
	int iX = iColPix[0] - 1;
	int iY = (iRowMoving - iFirstVisibleRow() + (fShowHeading ? 1 : 0)) * iHeight();
	cdc.MoveTo(0,iY+iHeight());
	cdc.LineTo(0,iY+1);
	cdc.LineTo(iX,iY+1);
	cdc.SelectObject(penOld);
	penOld = cdc.SelectObject(&penLight);
	cdc.LineTo(iX,iY+iHeight());
	cdc.LineTo(-1,iY+iHeight());
	cdc.SelectObject(penOld);
}

void BaseTablePaneView::ReleaseRowButton()
{
	CClientDC cdc(this);
	CPen penDark(PS_SOLID,1,SysColor(COLOR_BTNSHADOW));
	CPen penLight(PS_SOLID,1,SysColor(COLOR_BTNHIGHLIGHT));
	CPen *penOld = cdc.SelectObject(&penLight);
	int iX = iColPix[0] - 1;
	int iY = (iRowMoving - iFirstVisibleRow() + (fShowHeading ? 1 : 0)) * iHeight();
	cdc.MoveTo(0,iY+iHeight());
	cdc.LineTo(0,iY+1);
	cdc.LineTo(iX,iY+1);
	cdc.SelectObject(penOld);
	penOld = cdc.SelectObject(&penDark);
	cdc.LineTo(iX,iY+iHeight());
	cdc.LineTo(-1,iY+iHeight());
	cdc.SelectObject(penOld);
}

bool BaseTablePaneView::fAllowMoveCol() const
{
	return false;
}

void BaseTablePaneView::MoveCol(int iFrom, int iTo)
{
	MessageBeep(MB_ICONASTERISK);
}

void BaseTablePaneView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	CClientDC cdc(this);
	CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfTABLE);
	CFont* fntOld = cdc.SelectObject(fnt);
	TEXTMETRIC tm;
	cdc.GetTextMetrics(&tm);
	iHght = tm.tmHeight - 1;
	if (tm.tmExternalLeading == 0)
		iHght += 1;
	else
		iHght += tm.tmExternalLeading;
	iCharWidth = tm.tmAveCharWidth + cdc.GetTextCharacterExtra();
}

void BaseTablePaneView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fAllowCopy());
}

void BaseTablePaneView::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fAllowClear());
}

void BaseTablePaneView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fAllowPaste());
}

bool BaseTablePaneView::fValidSelection() const
{
	return selection.minRow() <= selection.maxRow() &&
		selection.minCol() <= selection.maxCol() && 
		selection.minRow() <= iRows() + (int)fShowHeading - 1 &&
		selection.minCol() < iCols();
}

bool BaseTablePaneView::fAllowCopy() const
{
	return fValidSelection();
}

bool BaseTablePaneView::fAllowClear() const
{
	return fAllowCopy();
}

bool BaseTablePaneView::fAllowPaste() const
{
	return false;
}

void BaseTablePaneView::OnEditCopy()
{
	CWaitCursor wc;
	//	COleDataSource* ods = new COleDataSource();
	OpenClipboard();
	EmptyClipboard();

	int iSize = 1000000;
	char* sBuf = new char[iSize];
	char* s = sBuf;
	char* sMax = &sBuf[iSize-100];
	bool fRowHeader = false, fColHeader = false;
	String str;
	long r, rMax;
	int c, cMax;
	r = selection.minRow();
	rMax = min(selection.maxRow(), iRows());
	cMax = min((int)selection.maxCol(), iCols() - 1);
	if (r <= 0) {
		fColHeader = true;
		r = 1;
		c = selection.minCol();
		if (c < 0) {
			c = 0;
			str = sULButton();
			strcpy(s, str.sVal());
			s += str.length();
			*s++ = '\t';
		}
		for (; c <= cMax; ++c)
		{
			str = sColButton(c);
			strcpy(s, str.sVal());
			s += str.length();
			*s++ = '\t';
		}
		--s;
		*s++ = '\r';
		*s++ = '\n';
	}
	for (; r <= rMax; ++r)
	{
		c = selection.minCol();
		if (c < 0) {
			fRowHeader = true;
			c = 0;
			str = sRowButton(r);
			if (str.sRight(4) == ".crd")
				str = str.sLeft(str.length()-4);
			else if (str.sRight(4) == ".clm")
				str = String("%S %S ", TR("Column"), str.sLeft(str.length()-4));
			strcpy(s, str.sVal());
			s += str.length();
			*s++ = '\t';
			c = 0;
		}
		for (; c <= cMax; ++c)
		{
			String str = sField(c,r);
			strcpy(s, str.sVal());
			s += str.length();
			*s++ = '\t';
			if (s > sMax) {
				MessageBox(TR("Selection does not fit in clipboard").c_str(),TR("Copy error").c_str(),MB_OK|MB_ICONSTOP);
				delete sBuf;
				return;
			}
		}
		--s;
		*s++ = '\r';
		*s++ = '\n';
	}
	*s = '\0';

	HGLOBAL hnd = GlobalAlloc(GMEM_FIXED, strlen(sBuf)+2);
	char* pc = (char*)GlobalLock(hnd);
	strcpy(pc,sBuf);
	GlobalUnlock(hnd);

	SetClipboardData(CF_TEXT,hnd);
	if (fRowHeader || fColHeader) {
		String sTblFmt("rowheader=%i colheader=%i\r\n", (int)fRowHeader, (int)fColHeader);
		hnd = GlobalAlloc(GMEM_MOVEABLE, strlen(sBuf)+sTblFmt.length()+1);
		char* pc = (char*)GlobalLock(hnd);
		strcpy(pc, sTblFmt.c_str());
		strcpy(pc+sTblFmt.length(),sBuf);
		GlobalUnlock(hnd);
		SetClipboardData(iFmtTbl,hnd);
	}
	delete sBuf;

	{ // copy as picture (only visible part)
		CRect rect;
		GetClientRect(rect);
		CSize sz = rect.Size();
		CMetaFileDC mdc;
		mdc.Create();
		CClientDC dc(this);
		dc.SetMapMode(MM_TEXT);
		mdc.SetAttribDC(dc.GetSafeHdc());
		mdc.SetWindowExt(sz);
		mdc.SetWindowOrg(rect.TopLeft());

		OnDraw(&mdc);
		HMETAFILE hMF = mdc.Close();

		HANDLE hnd = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,sizeof(METAFILEPICT)+1);
		METAFILEPICT* mp = (METAFILEPICT*)GlobalLock(hnd);
		mp->mm = MM_TEXT;
		mp->xExt = sz.cx;
		mp->yExt = -sz.cy;
		mp->hMF = hMF;
		GlobalUnlock(hnd);
		SetClipboardData(CF_METAFILEPICT, hnd);	
	}

	CloseClipboard();
}

void BaseTablePaneView::OnEditClear()
{
	zRect rect = rectSelect();
	selection.reset();
	InvalidateRect(&rect);
}

LRESULT BaseTablePaneView::OnGotoField(WPARAM wParam, LPARAM lParam)
{
	int iCol = wParam;
	long iRow = lParam;
	BaseTablePaneView::OnEditClear();
	if (tField) {
		delete tField;
		tField = 0;
		UpdateWindow();
	}
	if (iRow <= 0 && iCol == -1)
		return 0;
	if (iCol < 0) {
		iRow -= 1;
		iCol = iCols() - 1;
	}
	else if (iCol >= iCols()) {
		iCol = 0;
		iRow += 1;
	}
	if (iRow < 1)
		iRow = 1;
	else if (iRow > iRows())
		iRow = iRows();
	OnFieldPressed(iCol,iRow);

	return 1;
}


int BaseTablePaneView::iSelectedColumn() const
{
	if (selection.minCol() != selection.maxCol())
		return iUNDEF;
	return selection.minCol() < 0 ? iUNDEF : selection.minCol();
}

int BaseTablePaneView::iSelectedRow() const
{
	if (selection.minCol() != -1 || selection.maxCol() != iCols())
		return iUNDEF;
	if (selection.minRow() != selection.maxRow())
		return iUNDEF;
	return selection.minRow() < 0 ? iUNDEF : selection.minRow();
}

//////////////////////////////////////
// BaseTablePaneView Printing support code
/*
Standard MFC functions used:
OnPreparePrinting()
Called once for each print job; This function is responsible for 
displaying the Print Dialog. The function calculates the number of
pages required to print the entire domain, and also initializes
the selection radio button in case some items in the ListView are selected
OnBeginPrinting()
Called once for each print job
OnPrint()
Called once for each page. This function does the actual printing
OnEndPrinting()
Called once for each print job. This function is used for some cleanup

*/

// OnPreparePrinting is called once for each print job
BOOL BaseTablePaneView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	BOOL bResult;
	CWinApp* pApp = AfxGetApp();


	// ask our app what the default printer is
	// if there isn't any, punt to MFC so it will generate an error

	if (!pApp->GetPrinterDeviceDefaults(&pInfo->m_pPD->m_pd) ||
		pInfo->m_pPD->m_pd.hDevMode == NULL)
		return DoPreparePrinting(pInfo);

	HGLOBAL	hDevMode = pInfo->m_pPD->m_pd.hDevMode;
	HGLOBAL hDevNames = pInfo->m_pPD->m_pd.hDevNames;

	DEVMODE* pDevMode = (DEVMODE*) ::GlobalLock(hDevMode);
	DEVNAMES* pDevNames = (DEVNAMES*) ::GlobalLock(hDevNames);

	LPCSTR pstrDriverName = ((LPCSTR) pDevNames)+pDevNames->wDriverOffset;
	LPCSTR pstrDeviceName = ((LPCSTR) pDevNames)+pDevNames->wDeviceOffset;
	LPCSTR pstrOutputPort = ((LPCSTR) pDevNames)+pDevNames->wOutputOffset;
	int iCharW = 0; 
	int iCharH = 0;
	int iPageH = 0;
	int iPageW = 0;
	CDC dcPrinter;
	if (dcPrinter.CreateDC(pstrDriverName, pstrDeviceName, pstrOutputPort, NULL))
	{
		class FormPrintOptions : public FormWithDest
		{
		public:
			FormPrintOptions(CWnd* parent, int* iSel, bool* fHeaderOnAllPages, bool* fLeftMostColOnAllPages, bool fSelect, LOGFONT* lf, HDC hDC)
				: FormWithDest(parent, TR("Print Options"))
			{
				new CheckBox(root, TR("&Header on all pages"), fHeaderOnAllPages);
				new CheckBox(root, TR("&Leftmost column on all pages"), fLeftMostColOnAllPages);
				(new FieldLogFont(root, lf))->SetIndependentPos();
				if (fSelect) {
					RadioGroup* rg = new RadioGroup(root, "", iSel);
					new RadioButton(rg, TR("&All"));
					new RadioButton(rg, TR("&Selection"));
				}
				SetMenHelpTopic("ilwismen\\print_table.htm");
				create();
			}
		};
		int iSelection = selection.fValid() ? 0 : 1; // 0 is all ; 1 is selection
		if (selection.mm().width() == 0 && selection.mm().height() == 0)
			iSelection = 0;

		int iUnits = dcPrinter.GetDeviceCaps(LOGPIXELSY);

		LOGFONT logFont, logFontBold;
		if (NULL == m_PrintFont) {
			CFont* cf = const_cast<CFont *> ( IlwWinApp()->GetFont(IlwisWinApp::sfTABLE));
			cf->GetLogFont(&logFont);
			logFont.lfHeight = ::MulDiv(100, iUnits, 720);
			m_PrintFont = new CFont;
			m_PrintFont->CreateFontIndirect(&logFont);
		}
		m_PrintFont->GetLogFont(&logFont);
		logFont.lfHeight = ::MulDiv(logFont.lfHeight, 72, iUnits);

		FormPrintOptions frm(this, &iSelection, &fHeaderOnAllPages, &fLeftMostColOnAllPages, !selection.fValid(), &logFont, dcPrinter.m_hDC);
		if (!frm.fOkClicked())
			return FALSE;

		if (NULL != m_PrintFont)
			delete m_PrintFont;
		logFont.lfHeight = ::MulDiv(logFont.lfHeight, iUnits, 72);
		m_PrintFont = new CFont;
		m_PrintFont->CreateFontIndirect(&logFont);

		m_PrintFont->GetLogFont(&logFontBold);
		logFontBold.lfWidth = 0;
		logFontBold.lfWeight = FW_BOLD;
		if (NULL != m_PrintFontBold)
			delete m_PrintFontBold;
		m_PrintFontBold = new CFont;
		m_PrintFontBold->CreateFontIndirect(&logFontBold);

		CFont* pOldFont = dcPrinter.SelectObject(m_PrintFontBold);
		CSize cs = dcPrinter.GetTextExtent("X");
		iCharW = cs.cx; 
		iCharH = cs.cy;
		iPageH = dcPrinter.GetDeviceCaps(PHYSICALHEIGHT) - 2 * dcPrinter.GetDeviceCaps(PHYSICALOFFSETY);
		iPageW = dcPrinter.GetDeviceCaps(PHYSICALWIDTH) - 2 * dcPrinter.GetDeviceCaps(PHYSICALOFFSETX);
		dcPrinter.SelectObject(pOldFont);
		dcPrinter.DeleteDC();

		int iPages = 1;
		int iFirstCol = max(0, selection.minCol());
		int iLastCol = min(selection.maxCol(), iCols()-1);
		int iLeftMostColWidth = iCharW * (iButWidth() + 1);
		int iTblWidth = 0;
		for (int i=iFirstCol; i <= iLastCol; i++)
			iTblWidth += iCharW * (iColWidth[i] + 1);
		int iHorzPages = iCalcHorzPages(iLeftMostColWidth, fLeftMostColOnAllPages, iCharW, iPageW);

		int iTableRows = selection.fValid() ? iRows() : selection.maxRow() - selection.minRow()+1;

		iPages = ((iTableRows + 4 /*header*/)* iCharH + (iPageH - 1)) / iPageH;

		iMaxPages = iPages*iHorzPages;

		pInfo->SetMinPage(1);
		pInfo->SetMaxPage(iMaxPages);
		bResult = DoPreparePrinting(pInfo);
	}
	else
	{
		MessageBox("Could not create printer DC");
		bResult = FALSE;
	}
	::GlobalUnlock(hDevMode);
	::GlobalUnlock(hDevNames);

	return bResult;
}

void BaseTablePaneView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnBeginPrinting(pDC, pInfo);
}


void BaseTablePaneView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	delete m_PrintFont;
	delete m_PrintFontBold;
	m_PrintFont = NULL;
	m_PrintFontBold = NULL;
}

void BaseTablePaneView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	int iFirstRow, iLastRow, iFirstCol, iLastCol;
	CFont* fntOld = pDC->SelectObject(m_PrintFontBold);
	CSize cs = pDC->GetTextExtent("X");
	int iCharW = cs.cx; 
	int iCharH = cs.cy;
	CRect rectP = pInfo->m_rectDraw;  
	int iRowsPerPage = rectP.Height() / iCharH - 4;
	int iLeftMostColWidth = iCharW * (iButWidth() + 1);
	int iHorzPages = iCalcHorzPages(iLeftMostColWidth, fLeftMostColOnAllPages, iCharW, rectP.Width());
	int iColPage = (pInfo->m_nCurPage - 1) % iHorzPages;
	int iRowPage = (pInfo->m_nCurPage - 1) / iHorzPages;
	int iRowOffset = iRowPage * iRowsPerPage;
	if (selection.fValid()) {
		iFirstRow = 1 + iRowOffset; iFirstCol = 0; 
		iLastRow = min(iFirstRow + iRowsPerPage - 1, iRows()); iLastCol = iCols()-1;
	}
	else {
		iFirstRow = max(1, selection.minRow()) + iRowOffset;
		iLastRow = min(iFirstRow + iRowsPerPage - 1, min(selection.maxRow(), iRows()));
		iFirstCol = max(0, selection.minCol());
		iLastCol = min(selection.maxCol(), iCols()-1);
	}
	CalcColsForColumnPages(iColPage, iFirstCol, iLastCol, iLeftMostColWidth, fLeftMostColOnAllPages, iCharW, rectP.Width());
	String s;
	s = sPrintTitle();
	pDC->TextOut(0, 0, s.sVal()); 
	s = String("%i/%i", pInfo->m_nCurPage, iMaxPages);
	cs = pDC->GetTextExtent(s.sVal());
	pDC->TextOut(rectP.right - cs.cx, 0, s.sVal());
	int x = iColPage == 0 || fLeftMostColOnAllPages ? iLeftMostColWidth : 0;
	int y = 2 * iCharH;
	if (iRowPage == 0 || fHeaderOnAllPages)
		for (int i=iFirstCol; i <= iLastCol; i++) {
			s = sColButton(i);
			int iLen = s.length();
			for (int k = iLen; k < iColWidth[i]; k++, k++)
				s = " " + s + " ";
			pDC->TextOut(x, y, s.sVal()); 
			x += (iColWidth[i] + 1) * iCharW;
		}
		pDC->SelectObject(fntOld);
		y += 2*iCharH;
		for (int j = iFirstRow; j <= iLastRow; j ++) {
			x = 0;
			if (iColPage == 0 || fLeftMostColOnAllPages) {
				fntOld = pDC->SelectObject(m_PrintFontBold);
				String s = sRowButton(j);
				pDC->TextOut(x, y, s.sVal());
				pDC->SelectObject(fntOld);
				x += iLeftMostColWidth;
			}
			fntOld = pDC->SelectObject(m_PrintFont);
			for (int i=iFirstCol; i <= iLastCol; i++) {
				s = sField(i, j);
				pDC->TextOut(x, y, s.sVal());
				x += (iColWidth[i] + 1)* iCharW;
			}
			pDC->SelectObject(fntOld);
			y += iCharH;
		}
}

String BaseTablePaneView::sPrintTitle() const
{
	return "";
}

int BaseTablePaneView::iCalcHorzPages(int iLeftColWidth /* in dev. units */, 
									  bool fLeftMostColOnAllPages, int iCharW /* in dev. units */, int iPageWidth /* in dev. units*/)
{
	int iPages = 1;
	if (fLeftMostColOnAllPages)
		iPageWidth -= iLeftColWidth;
	int iW = fLeftMostColOnAllPages ? 0 : iLeftColWidth;
	for (int i=0; i < iCols(); i++) {
		if (iW + iColWidth[i] * iCharW > iPageWidth) {
			iPages++;
			iW = iColWidth[i] * iCharW;
		}
		else
			iW += iColWidth[i] * iCharW;
	}
	return iPages;
}

void BaseTablePaneView::CalcColsForColumnPages(int iColPage, int& iFirstCol, int& iLastCol, int iLeftColWidth /* in dev. units */, 
											   bool fLeftMostColOnAllPages, int iCharW /* in dev. units */, int iPageWidth /* in dev. units*/)
{
	int iFirstC = iFirstCol;
	int iLastC = iLastCol;
	int iPages = 1;
	if (fLeftMostColOnAllPages)
		iPageWidth -= iLeftColWidth;
	int iW = fLeftMostColOnAllPages ? 0 : iLeftColWidth;
	for (int i=iFirstC; i <= iLastC; i++) {
		if (iW + iColWidth[i] * iCharW > iPageWidth) {
			if (iColPage == iPages - 1)
				iLastCol = i-1;
			iPages++;
			if (iColPage == iPages - 1)
				iFirstCol = i;
			iW = iColWidth[i] * iCharW;
		}
		else
			iW += iColWidth[i] * iCharW;
	}
}

void BaseTablePaneView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);
	fHasFocus = false;
	CRect rect = rectSelect();
	if (!rect.IsRectEmpty())
		InvalidateRect(rect);
}

void BaseTablePaneView::OnSetFocus(CWnd* pNewWnd)
{
	CView::OnSetFocus(pNewWnd);
	fHasFocus = true;
	CRect rect = rectSelect();
	if (!rect.IsRectEmpty())
		InvalidateRect(rect);
}

void BaseTablePaneView::selectFeatures(const RowSelectInfo& inf) {
	if ( inf.sender == (long) this)
		return;

	selection.reset();
	vector<long> rows;
	for(int i=0; i < inf.raws.size(); ++i) {
		long row = inf.raws[i];
		if ( row > 0)
			rows.push_back(row - 1); 
	}
	selection.selectRows(rows);
	Invalidate();

}

const TableSelection& BaseTablePaneView::sel() const{
	return selection;
}

void BaseTablePaneView::setSelection(const MinMax& mm){
	selection.selectBlock(mm.rcMin, mm.rcMax);
}


