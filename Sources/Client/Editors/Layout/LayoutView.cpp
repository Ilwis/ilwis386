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
// LayoutView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Layout\LayoutView.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\FormElements\syscolor.h"
#include "Headers\constant.h"
#include "Client\Mapwindow\MapCompositionSrvItem.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Headers\Hs\Layout.hs"
#include <afxole.h>
#include <afxpriv.h>
#include <cderr.h>      // Commdlg Error definitions
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"
#include "Headers\messages.h"

const int iMINSIZE = 50;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LayoutView

IMPLEMENT_DYNCREATE(LayoutView, ZoomableView)

const int iHELPMSGSTRING = RegisterWindowMessage(HELPMSGSTRING);


BEGIN_MESSAGE_MAP(LayoutView, ZoomableView)
// implemented in ZoomableView
	ON_COMMAND(ID_ZOOMIN, OnZoomIn)
	ON_COMMAND(ID_ZOOMOUT, OnZoomOut)
	ON_COMMAND(ID_PANAREA, OnPanArea)
	ON_COMMAND(ID_SELECTAREA, OnSelectArea)
	ON_UPDATE_COMMAND_UI(ID_ZOOMIN, OnUpdateZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOMOUT, OnUpdateZoomOut)
	ON_UPDATE_COMMAND_UI(ID_SELECTAREA, OnUpdateZoomIn)
	ON_UPDATE_COMMAND_UI(ID_PANAREA, OnUpdatePanArea)
// own implementations
	ON_COMMAND(ID_ENTIRE_PAGE, OnEntirePage)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnPageSetup)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditCopy)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCopy)
	ON_COMMAND(ID_FILE_PRINT,	OnFilePrint)
	ON_COMMAND(ID_MAKE_BITMAP, OnMakeBitmap)
	ON_WM_CONTEXTMENU()
	ON_REGISTERED_MESSAGE(iHELPMSGSTRING, OnPrintHelp)
  ON_MESSAGE(ILWM_VIEWSETTINGS, OnViewSettings)
END_MESSAGE_MAP()



LayoutView::LayoutView()
{
	fStarting = true; // prevent too early start 
	liActive = 0;
	odt = new COleDropTarget;
}

LayoutView::~LayoutView()
{
	delete odt;
}

MinMax LayoutView::mmPaperBounds()
{
	LayoutDoc* ld = GetDocument();
	if (0 == ld)
		return MinMax(RowCol(0,0),RowCol(500,500));
	CSize sz = ld->szPaper;
	if (sz.cx <= 0 || sz.cy <= 0)
		return MinMax(RowCol(0,0),RowCol(500,500));
	// convert from 0.01 mm units in 0.1 mm units
	MinMax mm(RowCol(0,0),RowCol(sz.cy/10,sz.cx/10));
	return mm;
}

MinMax LayoutView::mmBounds()
{
	MinMax mm = mmPaperBounds();
// gray area of 5% around paper
	int iWhite = mm.width() / 20;
	mm.MinCol() -= iWhite;
	mm.MinRow() -= iWhite;
	mm.MaxCol() += iWhite;
	mm.MaxRow() += iWhite;
	return mm;
}

/////////////////////////////////////////////////////////////////////////////
// LayoutView drawing

void LayoutView::OnInitialUpdate()
{
	odt->Register(this);
	// fStarting = true; // prevent too early start // moved to constructor
	liActive = 0;
	LayoutDoc* ld = GetDocument();

	ReportPaperSizeOnStatusBar();
	ZoomableView::OnInitialUpdate();
	PostMessage(WM_COMMAND, ID_ENTIRE_PAGE, 0);
}

void LayoutView::OnDraw(CDC* cdc)
{
	// during printing these have an other value
	iExtraOffsetX = 0;
	iExtraOffsetY = 0;

	LayoutDoc* ld = GetDocument();
	CRect rectDirty;
	CPaintDC* pdc = dynamic_cast<CPaintDC*>(cdc);
	if (pdc)
		rectDirty = pdc->m_ps.rcPaint;

	zRect rect;
	GetClientRect(&rect);

	cdc->SetMapMode(MM_TEXT);
	// gray window background
	SysColor colBG(COLOR_APPWORKSPACE);
	Color colWhite(255,255,255);
	Color colBlack(0,0,0);
	CPen penBG(PS_SOLID,1,colBG), penBlack(PS_SOLID,1,colBlack); 
	CBrush brBG(colBG), brWhite(colWhite);
	CPen* penOld = cdc->SelectObject(&penBG);
	CBrush* brOld = cdc->SelectObject(&brBG);
	cdc->Rectangle(rect);
	// white paper background
	cdc->SelectObject(&penBlack);
	cdc->SelectObject(&brWhite);
	CRect rctBounds = rctPos(mmPaperBounds());
	cdc->Rectangle(rctBounds);
	// shadow
	cdc->MoveTo(rctBounds.right + 1, rctBounds.top + 2);
	cdc->LineTo(rctBounds.right + 1, rctBounds.bottom + 1);
	cdc->LineTo(rctBounds.left + 1, rctBounds.bottom + 1);
	cdc->MoveTo(rctBounds.right, rctBounds.top + 2);
	cdc->LineTo(rctBounds.right, rctBounds.bottom);
	cdc->LineTo(rctBounds.left + 1, rctBounds.bottom);

	DrawPageBorder(cdc);
	// Layout Items
  for (list<LayoutItem*>::iterator iter = ld->lli.begin(); iter != ld->lli.end(); ++iter) 
	{
		LayoutItem* li = *iter;
		if (li != liActive) 
			DrawItem(cdc,li);
	}
	if (liActive) {
		DrawItem(cdc,liActive);
		cdc->SelectClipRgn(0);
		CRect rect = rctPos(liActive->mmPosition());
		tracker.m_rect = rect;
		tracker.m_nStyle = CRectTracker::solidLine | CRectTracker::resizeOutside;
		tracker.Draw(cdc);
	}

	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
	cdc->SetMapMode(MM_TEXT);
}

void LayoutView::DrawItem(CDC* cdc, LayoutItem* li)
{
	cdc->SetMapMode(MM_ANISOTROPIC);
	CRect rect = li->rectPos();
	CSize sz = rect.Size();
	if (sz.cx <= 0 || sz.cy <= 0)
		return;
	cdc->SetWindowExt(sz);
	CRect rct = rctPos(li->mmPosition());
	cdc->SetViewportExt(rct.Width(), rct.Height());
	cdc->SetWindowOrg(rect.left, rect.top);
	cdc->SetViewportOrg(rct.left-iExtraOffsetX, rct.top-iExtraOffsetY);
	if (li->fClip()) {
		CRgn rgn;
		rgn.CreateRectRgnIndirect(&rct);
		cdc->SelectClipRgn(&rgn);
	}
	else
		cdc->SelectClipRgn(0);
	try {
		li->OnDraw(cdc);
	}
	catch (ErrorObject& err) {
		err.Show();
	}
	CRect rctNew = rctPos(li->mmPosition());
	if (rctNew.bottom > rct.bottom || rctNew.right > rct.right)
		InvalidateRect(rctNew);
	cdc->SelectClipRgn(0);
}

LayoutDoc* LayoutView::GetDocument()
{
  return (LayoutDoc*)m_pDocument;
}

void LayoutView::SetDirty()
{
	if (fStarting)
		return;
	Invalidate();
}

void LayoutView::OnEntirePage()
{
  _rScale = 1;
  iXpos = iXmin;
  iYpos = iYmin;

	MinMax mm = mmBounds();
	int iWidth = scale(mm.width(), true);
	int iHeight = scale(mm.height(), true);
	double rScaleX = double(dim.width()) / iWidth;
	double rScaleY = double(dim.height()) / iHeight;
	double rScale = min(rScaleX, rScaleY);
	// change to 'Ilwis scale'
	_rScale = rScale >= 1.0 ? rScale : -1.0/rScale;

	if (fStarting) 
	{
	  _rScale = 1;
		while (iWidth < 6 * iMINSIZE || iHeight < 6 * iMINSIZE) 
		{
			_rScale += 1;
			iWidth = scale(mm.width(), true);
			iHeight = scale(mm.height(), true);
		}

		CRect rectWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);
		
		int iScreenWidth = rectWorkArea.Width();
		int iScreenHeight = rectWorkArea.Height();
		int iMaxWidth = (int)(min(6 * iMINSIZE, 0.8 * iScreenWidth));
		int iMaxHeight = (int)(min(6 * iMINSIZE, 0.8 * iScreenHeight));

		while (iWidth > 0.8 * iScreenWidth ||
					 iHeight > 0.8 * iScreenHeight) 
		{
			_rScale -= 1;
			if (abs(_rScale) < 1)
				_rScale = -2;
			iWidth = scale(mm.width(), true);
			iHeight = scale(mm.height(), true);
		}

		CRect rect;
		GetClientRect(&rect);
		FrameWindow* fw = fwParent();
		if (fw && !fw->IsZoomed()) 
			// do not resize when maximized
		{
			CSize siz;
			siz.cx = iWidth - rect.Width();
			siz.cy = iHeight - rect.Height();
			fw->resize(siz);
		}
	}
	CalcFalseOffsets();
	setScrollBars();
	fStarting = false;
	SetDirty();
}

void LayoutView::OnPageSetup()
{
	LayoutDoc* ld = GetDocument();
	htp = htpPageSetup;

	PAGESETUPDLG psd;
	memset(&psd, 0, sizeof(psd));
	psd.lStructSize = sizeof(psd);
	psd.hwndOwner = m_hWnd;
	psd.hDevMode = ld->m_hDevMode;
	psd.hDevNames = ld->m_hDevNames;
	psd.Flags = PSD_MARGINS|PSD_INHUNDREDTHSOFMILLIMETERS|PSD_SHOWHELP;
	psd.ptPaperSize = CPoint(ld->szPaper.cx, ld->szPaper.cy);
	psd.rtMargin = ld->rectMargins;
	int iRes = PageSetupDlg(&psd);

	if (IDOK == iRes) 
	{
		ld->szPaper = psd.ptPaperSize;
		ld->rectMargins = psd.rtMargin;
		if (ld->m_hDevMode != psd.hDevMode) {
			if (ld->m_hDevMode != 0)
				GlobalFree(ld->m_hDevMode);
			ld->m_hDevMode = psd.hDevMode;
		}
		if (ld->m_hDevNames != psd.hDevNames) {
			if (ld->m_hDevNames != 0)
				GlobalFree(ld->m_hDevNames);
			ld->m_hDevNames = psd.hDevNames;
		}
//		ld->SetPrinterDC(ld->m_hDevNames, ld->m_hDevMode);
		ld->SetModifiedFlag();
		MinMax mm = mmBounds();
		iXsize = mm.width();
		iYsize = mm.height();
		OnEntirePage();
		ReportPaperSizeOnStatusBar();
	}
}

LayoutItem* LayoutView::liHitTest(CPoint point)
{
	LayoutDoc* ld = GetDocument();
	RowCol rc = rcPos(point);
	LayoutItem* liHit = 0;
	double rHitArea = 0;
  for (list<LayoutItem*>::iterator iter = ld->lli.begin(); iter != ld->lli.end(); ++iter) 
	{
		LayoutItem* li = *iter;
		MinMax mm = li->mmPosition();
		if (mm.fContains(rc)) {
			double rArea = mm.width();
			rArea *= mm.height();
			if (0 == liHit || rArea < 0.8 * rHitArea) {
				liHit = li;
				rHitArea = rArea;
			}
		}
	}
	return liHit;
}


void LayoutView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iHit = CRectTracker::hitNothing;
	LayoutItem* li = liActive;
	if (liActive) 
		iHit = tracker.HitTest(point);
	if (iHit == CRectTracker::hitNothing)
	{
		li = liHitTest(point);
		if (li == liActive)
			return;
	}
	SetActiveItem(li);
	LayoutDoc* ld = GetDocument();
	ld->UpdateAllViews(this, LayoutDoc::hintITEM, liActive);
	CRect rectOld;
	tracker.GetTrueRect(&rectOld);
	tracker.SetCursor(this, HTCLIENT);
	if (tracker.Track(this, point)) {
		rectOld.right += 1;
		rectOld.bottom += 1;
		InvalidateRect(&rectOld);
		liActive->SetPosition(mmRect(tracker.m_rect),iHit);
		CRect rect;
		tracker.m_nStyle = CRectTracker::solidLine | CRectTracker::resizeOutside;
		tracker.GetTrueRect(&rect);
		rect.right += 1;
		rect.bottom += 1;
		InvalidateRect(&rect);
		ld->UpdateAllViews(NULL, LayoutDoc::hintITEMSIZE, liActive);
	}
}

void LayoutView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (0 == as && liActive) {
		int iHit = tracker.HitTest(point);
		if (iHit != CRectTracker::hitNothing) {
			tracker.SetCursor(this, HTCLIENT);
		}
	}
	FrameWindow* fw = fwParent();
	if (fw) {
	  double rRow, rCol;
		Pnt2RowCol(point, rRow, rCol);
		String strX("x = %.1f mm", rCol/10);
		String strY("y = %.1f mm", rRow/10);
		fw->status->SetPaneText(1, strX.scVal());
		fw->status->SetPaneText(2, strY.scVal());
//		fw->status->SetWindowText(str.scVal());
	}
}

void LayoutView::OnEditCut()
{
	OnEditCopy();
	OnEditClear();
}

void LayoutView::OnEditCopy()
{
	if (0 == liActive)
		return;
	if (!OpenClipboard())
		return;
	EmptyClipboard();

	bool fPicture = liActive->fAddExtraClipboardItems();

	if (fPicture) {
		CRect rect = liActive->rectPos();
		CSize sz = rect.Size();
		CMetaFileDC mdc;
		mdc.Create();
		CClientDC dc(this);
		dc.SetMapMode(MM_LOMETRIC);
		mdc.SetAttribDC(dc.GetSafeHdc());
		mdc.SetWindowExt(sz);
		mdc.SetWindowOrg(rect.TopLeft());
		liActive->OnDraw(&mdc);
		HMETAFILE hMF = mdc.Close();

		HANDLE hnd = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,sizeof(METAFILEPICT)+1);
		METAFILEPICT* mp = (METAFILEPICT*)GlobalLock(hnd);
		mp->mm = MM_ANISOTROPIC;
		mp->xExt = sz.cx;
		mp->yExt = -sz.cy;
		mp->hMF = hMF;
		GlobalUnlock(hnd);
		SetClipboardData(CF_METAFILEPICT, hnd);	
	}

	CloseClipboard();
}

void LayoutView::OnEditClear()
{
	if (0 == liActive)
		return;
	LayoutDoc* ld = GetDocument();
	ld->RemoveItem(liActive);
	liActive = 0;
	CRect rect;
	tracker.GetTrueRect(&rect);
	InvalidateRect(&rect);
}

void LayoutView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(liActive != 0);
}

BOOL LayoutView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	switch (nID)
	{
		// these options should exclusively work on the whole layout
		case ID_FILE_SAVE_AS:
		case ID_FILE_NEW:
		case ID_FILE_OPEN:
			break;
		default:
			if (liActive && liActive->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
	}
	return ZoomableView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void LayoutView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch (lHint) {
		case 0: 
			SetDirty();
			break;
		case LayoutDoc::hintITEM: 
		{
			LayoutItem* li = dynamic_cast<LayoutItem*>(pHint);
			SetActiveItem(li);
			if (0 == li) {
				SetDirty();
				return;
			}
			CRect rect;
			tracker.GetTrueRect(&rect);
			InvalidateRect(&rect);
			LayoutDoc* ld = GetDocument();
			for (list<LayoutItem*>::iterator iter = ld->lli.begin(); iter != ld->lli.end(); ++iter) 
			{
				LayoutItem* item = *iter;
				if (item->fDependsOn(li)) {
					CRect rect = rctPos(item->mmPosition());
					InvalidateRect(rect);			
				}
			}
		} break;
		case LayoutDoc::hintITEMSIZE: 
		{
			LayoutItem* li = dynamic_cast<LayoutItem*>(pHint);
			if (0 == li)
				return;
			LayoutDoc* ld = GetDocument();
			for (list<LayoutItem*>::iterator iter = ld->lli.begin(); iter != ld->lli.end(); ++iter) 
			{
				LayoutItem* item = *iter;
				MinMax mmOldPos = item->mmPosition();
				if (item->fOnChangedItemSize(li)) {
					InvalidateRect(rctPos(mmOldPos));
					CRect rect = rctPos(item->mmPosition());
					InvalidateRect(rect);			
				}
			}
		} break;
		case LayoutDoc::hintPAGEBORDER: 
		{
			CClientDC dc(this);
			DrawPageBorder(&dc);
		} break;
	}		
}

void LayoutView::SetActiveItem(LayoutItem* li)
{
	if (liActive) {
		CRect rect;
		tracker.GetTrueRect(&rect);
		InvalidateRect(&rect);
	}
	if (0 == li) {
		liActive = 0;
		return;
	}
	if (li == liActive) {
		CRect r0, r1;
		tracker.GetTrueRect(&r0);
		tracker.m_rect = rctPos(li->mmPosition());
		tracker.GetTrueRect(&r1);
		if (r0 != r1) {
			InvalidateRect(r0);
			InvalidateRect(r1);
		}
		return;
	}
	if (liActive != li) {
		liActive = li;
		tracker.SetLayoutItem(liActive);
		CRect rect = rctPos(li->mmPosition());
		tracker.m_rect = rect;
		tracker.m_nStyle = CRectTracker::solidLine | CRectTracker::resizeOutside;
		tracker.GetTrueRect(&rect);
		if (!li->fInitialized) {
			InvalidateRect(rect);			
		}
		else {
			CClientDC dc(this);
			tracker.Draw(&dc);
		}
	}
}

void LayoutView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (liActive)
		liActive->OnItemEdit();
}

void LayoutView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar) {
		case VK_RETURN:
			if (liActive)
				liActive->OnItemEdit();
			break;
		case VK_CLEAR:
			OnEditClear();
			break;
		case VK_TAB:
		{
			LayoutDoc* ld = GetDocument();
			if (ld->lli.empty())
				break;
		  bool fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
			LayoutItem* li = liActive;
			if (li) {
				if (fShift && ld->lli.front() == li)
					li = ld->lli.back();
				else if (!fShift && ld->lli.back() == li)
					li = ld->lli.front();
				else {
					list<LayoutItem*>::iterator it = find(ld->lli.begin(), ld->lli.end(), li);
					if (fShift)
						li = *--it;
					else
						li = *++it;
				}
			}
			else 
				li = ld->lli.front();
			SetActiveItem(li);
			ld->UpdateAllViews(this, LayoutDoc::hintITEM, liActive);
		} break;
		default:
			ZoomableView::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

DROPEFFECT LayoutView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	bool fOk = false;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
	    FileName fn(sFileName);
			if (fn.sExt == ".mpv" || 
					fn.sExt == ".bmp" || 
					fn.sExt == ".emf" || 
					fn.sExt == ".wmf") 
			{
				fOk = true;
				break;
			}
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);
	if (fOk)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

void LayoutView::OnDragLeave() 
{
	// TODO: Add your specialized code here and/or call the base class
}

DROPEFFECT LayoutView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	return OnDragEnter(pDataObject, dwKeyState, point);
	//return CView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL LayoutView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return FALSE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	int iFiles = 0;
	FileName* afn = 0;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		afn = new FileName[iFiles];
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
	    afn[i] = FileName(sFileName);
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);
	// if send by SendMessage() prevent deadlock
	ReplyMessage(0);

	bool fOk = false;
	LayoutDoc* ld = GetDocument();
	for (int i = 0; i < iFiles; ++i) {
		FileName fn = afn[i];
    if (!fn.fExist()) {
			FileName fnSys = fn;
      String sStdDir = IlwWinApp()->Context()->sStdDir();
      fnSys.Dir(sStdDir);
      if (fnSys.fExist())
        fn = fnSys;
		}
		if (fn.sExt == ".mpv")
			fOk = ld->liAddMapView(fn.sFullName()) != 0 ;
		else if (fn.sExt == ".bmp" || fn.sExt == ".emf" || fn.sExt == ".wmf") 
			fOk = ld->liAddPicture(fn.sFullName()) != 0;
	}
	return fOk;
}


BOOL LayoutView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	LayoutDoc* ld = GetDocument();
	pInfo->SetMinPage(1);	
	pInfo->SetMaxPage(1);	

	htp = htpPrintLayout;
	PRINTDLG& pd = pInfo->m_pPD->m_pd;
	pd.hwndOwner = m_hWnd;
	pd.hDevMode = ld->m_hDevMode;
	pd.hDevNames = ld->m_hDevNames;
	pd.hDC = 0;
	pd.Flags = PD_RETURNDC | PD_NOPAGENUMS | PD_NOSELECTION | PD_SHOWHELP;
	if (PrintDlg(&pd)) 
	{
		ld->m_hDevMode = pd.hDevMode;
		ld->m_hDevNames = pd.hDevNames;
		return TRUE;
	}
	else {
		switch (::CommDlgExtendedError())
		{
		// CommDlg cannot give these errors after NULLing these handles
		case PDERR_PRINTERNOTFOUND:
		case PDERR_DNDMMISMATCH:
		case PDERR_DEFAULTDIFFERENT:
			if (ld->m_hDevNames != NULL)
			{
				GlobalFree(ld->m_hDevNames);
				ld->m_hDevNames = 0;
			}
			if (ld->m_hDevMode)
			{
				GlobalFree(ld->m_hDevMode);
				ld->m_hDevMode = NULL;
			}
		}
		return FALSE;
	}

//	pInfo->m_pPD->m_pd.hDC = AfxCreateDC(ld->m_hDevNames, ld->m_hDevMode);

//	return TRUE;
}

void LayoutView::OnPrint(CDC* cdc, CPrintInfo* pInfo) 
{
	int iXpix = cdc->GetDeviceCaps(HORZRES);
	int iYpix = cdc->GetDeviceCaps(VERTRES);
	int iXmm =  cdc->GetDeviceCaps(HORZSIZE);
	int iYmm =  cdc->GetDeviceCaps(VERTSIZE);

	double rFactX = iXpix / (10.0 * iXmm);
	double rFactY = iYpix / (10.0 * iYmm);

	cdc->SetMapMode(MM_ANISOTROPIC);
	// offset of 3 mm to prevent false movement (bug 2879)
	// if this is indeed printer dependent in future it should become a property somethere
	iExtraOffsetX = 30;
	iExtraOffsetY = 30;
	cdc->SetWindowOrg(0,0);
	cdc->SetViewportOrg(-iExtraOffsetX,-iExtraOffsetY);
	cdc->SetWindowExt(CSize(10*iXmm, 10*iYmm));
	cdc->SetViewportExt(CSize(iXpix, iYpix));

	Print(cdc, rFactX, rFactY);
}

void LayoutView::Print(CDC* cdc, double rFactX, double rFactY)
{
	LayoutDoc* ld = GetDocument();

	CPen pen(PS_SOLID,1,Color(0,0,0)); 
	CBrush br(Color(255,255,255));
	CPen* penOld = cdc->SelectObject(&pen);
	CBrush* brOld = cdc->SelectObject(&br);

	if (ld->fPageBorder) {
		CRect rectMargins = ld->rectMargins;
		CRect rectBorder;
		MinMax mm = mmPaperBounds();
		rectBorder.top = mm.MinRow() + rectMargins.top/10;
		rectBorder.bottom = mm.MaxRow() -= rectMargins.bottom/10;
		rectBorder.left = mm.MinCol() + rectMargins.left/10;
		rectBorder.right = mm.MaxCol() - rectMargins.right/10;
		CPen penBorder(PS_SOLID,3,Color(0,0,0));
		cdc->SelectObject(&penBorder);
		cdc->SelectStockObject(NULL_BRUSH);
		cdc->Rectangle(&rectBorder);
		cdc->SelectObject(&pen);
	}
	
	// Layout Items
  for (list<LayoutItem*>::iterator iter = ld->lli.begin(); iter != ld->lli.end(); ++iter) 
	{
		LayoutItem* li = *iter;
		CRect rect = li->rectPos();
		CSize sz = rect.Size();
		iExtraOffsetX = 30;
		iExtraOffsetY = 30;		

		cdc->SetMapMode(MM_ANISOTROPIC);
		CPoint pt= rect.TopLeft();
		cdc->SetWindowOrg(pt);
		cdc->SetViewportOrg((int)(pt.x*rFactX-iExtraOffsetX),(int)(pt.y*rFactY-iExtraOffsetY));
		cdc->SetWindowExt(sz);
		cdc->SetViewportExt((int)(sz.cx*rFactX),(int)(sz.cy*rFactY));

		if (li->fClip()) {
			//cdc->LPtoDP(&rect);
			rect.top = (LONG)(rect.top * rFactY);
			rect.bottom = (LONG)(rect.bottom * rFactY);
			rect.left = (LONG)(rect.left * rFactX);
			rect.right = (LONG)(rect.right * rFactX);
			rect.top -= iExtraOffsetY;
			rect.bottom -= iExtraOffsetY;
			rect.left -= iExtraOffsetX;
			rect.right -= iExtraOffsetX;
			CRgn rgn;
			rgn.CreateRectRgnIndirect(&rect);
			cdc->SelectClipRgn(&rgn);
		}
		else
			cdc->SelectClipRgn(0);
		li->OnDraw(cdc);
	}

	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
}

void LayoutView::DrawPageBorder(CDC* cdc)
{
	LayoutDoc* ld = GetDocument();
	MinMax mm = mmPaperBounds();

	cdc->SetMapMode(MM_ANISOTROPIC);
	CRect rect(mm.MinCol(), mm.MinRow(),
	           mm.MaxCol(), mm.MaxRow());
	CSize sz = rect.Size();
	cdc->SetWindowExt(sz);
	CRect rct = rctPos(mm);
	cdc->SetViewportExt(rct.Width(), rct.Height());
	cdc->SetWindowOrg(rect.left, rect.top);
	cdc->SetViewportOrg(rct.left-iExtraOffsetX, rct.top-iExtraOffsetY);

	CRect rectMargins = ld->rectMargins;
	CRect rectBorder;
	rectBorder.top = mm.MinRow() + rectMargins.top/10;
	rectBorder.bottom = mm.MaxRow() -= rectMargins.bottom/10;
	rectBorder.left = mm.MinCol() + rectMargins.left/10;
	rectBorder.right = mm.MaxCol() - rectMargins.right/10;

	CPen* penOld;
	CGdiObject* brOld = cdc->SelectStockObject(NULL_BRUSH);
	CPen pen(PS_SOLID,3,Color(0,0,0));
	CPen penGray(PS_DASH,3,Color(192,192,192));
	if (ld->fPageBorder)
		penOld = cdc->SelectObject(&pen);
	else
		penOld = cdc->SelectObject(&penGray);
	cdc->Rectangle(&rectBorder);	
	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
}


BOOL LayoutView::PreTranslateMessage(MSG* pMsg) 
{
  if (0 == as && pMsg->message == WM_KEYDOWN )
	{
		switch (pMsg->wParam)
		{
			case VK_RETURN:
				if (liActive) {
					liActive->OnItemEdit();
					return TRUE;
				}
				break;
			case VK_DELETE:
				OnEditClear();
				return TRUE;
			case VK_ESCAPE:
				SetActiveItem(0);
				LayoutDoc* ld = GetDocument();
				ld->UpdateAllViews(this, LayoutDoc::hintITEM, liActive);
				return TRUE;
		}
	}
	return ZoomableView::PreTranslateMessage(pMsg);
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void LayoutView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (liActive)
		liActive->OnContextMenu(pWnd, point);
	else {
		CMenu men;
		men.CreatePopupMenu();
		pmadd(ID_NORMAL);
		pmadd(ID_ZOOMIN);
		pmadd(ID_ZOOMOUT);
		men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	}
}

void LayoutView::ReportPaperSizeOnStatusBar()
{
	FrameWindow* fw = fwParent();
	LayoutDoc* ld = GetDocument();
	String str("%.1f x %.1f mm", ld->szPaper.cx/100.0, ld->szPaper.cy/100.0);

	if (ld->m_hDevMode) {
		LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(ld->m_hDevMode);
		if (lpDevMode->dmFormName)
			str = String("%s:  %S", (char*)lpDevMode->dmFormName, str);

		::GlobalUnlock(ld->m_hDevMode);
	}
	CClientDC dcScreen(NULL);
	HFONT hFont = (HFONT)fw->status->SendMessage(WM_GETFONT);
	HGDIOBJ hOldFont = NULL;
	if (hFont != NULL)
		hOldFont = dcScreen.SelectObject(hFont);
	int iWidth = dcScreen.GetTextExtent(str.scVal()).cx;
	fw->status->SetPaneInfo(3, 0, SBPS_NORMAL, iWidth);
	fw->status->SetPaneText(3, str.scVal());
	if (hOldFont != NULL)
		dcScreen.SelectObject(hOldFont);
}

class MakeBitmapForm: public FormWithDest
{
public:
	MakeBitmapForm(CWnd* wnd, LayoutDoc* doc, String* _sFile, int* iResDpi) 
		: FormWithDest(wnd, SLOTitleMakeBitmap)
		, ld(doc), iDpi(iResDpi),
		sFile(_sFile)
	{
		fsOutName = new FieldString(root, SLOUiFileName, sFile);
		fiDpi = new FieldInt(root, SLOUiResDpi, iResDpi, ValueRange(30,1250), true);
		fiDpi->SetCallBack((NotifyProc)&MakeBitmapForm::CallBack);
		String sDummy('X', 30);
		st1 = new StaticText(root, sDummy);
		st1->SetIndependentPos();
		st2 = new StaticText(root, sDummy);
		st2->SetIndependentPos();
		SetMenHelpTopic(htpExportToBitmap);
		create();
	}
	FormEntry *CheckData()
	{
		fsOutName->StoreData();
		FileName fnFile(*sFile, ".bmp");		
		if ( fnFile != FileName() )
		{
			if ( fnFile.fExist())
			{
				int iRet = wnd()->MessageBox(String(SLOErrFileAlreadyExists_S.scVal(), fnFile.sRelative()).scVal(), 
					                         SLOTitleErrorFile.scVal(), MB_YESNO | MB_ICONERROR );
				if ( iRet == IDNO )
					return fsOutName;				
			}				
		}			
		return 0;
	}		
private:
	String *sFile;;
	int CallBack(Event*) 
	{
		fiDpi->StoreData();
		int iWidth, iHeight, iFileSize;
		ld->CalcBitmapSize(*iDpi, iWidth, iHeight, iFileSize);
		String s1(SLORemWidthHeight_ii.scVal(), iWidth, iHeight);
		String s2;
		double rMB = (double)iFileSize / 1024 / 1024;
		if (rMB < 1) 
			s2 = String(SLORemFileSizekB_i.scVal(), iFileSize / 1024);
		else
			s2 = String(SLORemFileSizeMB_f.scVal(), rMB);
		st1->SetVal(s1);
		st2->SetVal(s2);
		return 0;
	}
	FieldInt* fiDpi;
	FieldString* fsOutName;
	StaticText *st1, *st2;
	LayoutDoc* ld;
	int* iDpi;
};


void LayoutView::OnMakeBitmap()
{
	// during printing these have an other value
	iExtraOffsetX = 0;
	iExtraOffsetY = 0;

	String sFile;
	int iResDpi = 150;

	LayoutDoc* ld = GetDocument();
	CString str = ld->GetPathName();
	FileName fn((const char*)str);
	sFile = fn.sFile;
	MakeBitmapForm frm(this, ld, &sFile, &iResDpi);
	if (!frm.fOkClicked())
		return;

	CWaitCursor cur;

	int iWidth, iHeight, iFileSize;
	ld->CalcBitmapSize(iResDpi, iWidth, iHeight, iFileSize);

	double rPixPerMeter = iResDpi / 0.0254; // 25.4 mm/inch
	int iPixPerMeter = rounding(rPixPerMeter);
	int iImgSize = ((((iWidth * 24) + 31) & ~31) >> 3) * iHeight;
	CClientDC dcView(this);
	CDC dc;
	dc.CreateCompatibleDC(&dcView);
	CBitmap bm;
	if (!bm.CreateCompatibleBitmap(&dcView,iWidth,iHeight)) 
	{
		MessageBox(SLOErrNotEnoughMemory.scVal(), SLOTitleMakeBitmap.scVal(), MB_ICONSTOP);
		return;
	}		

	BITMAP bitmap;
	bm.GetObject(sizeof(bitmap),&bitmap);

	CBitmap* bmOld = dc.SelectObject(&bm);
	dc.SelectStockObject(WHITE_BRUSH);
	dc.SelectStockObject(WHITE_PEN);
	CRect rect(CPoint(0,0),CSize(iWidth,iHeight));
	BOOL bl = dc.Rectangle(&rect);

	double rFactX = rPixPerMeter / 10000; // pixels per 0.1 mm
	double rFactY = rFactX;

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowOrg(0,0);
	dc.SetViewportOrg(0,0);
	dc.SetWindowExt(CSize(10000, 10000));
	dc.SetViewportExt(CSize(iPixPerMeter, iPixPerMeter));

	Print(&dc, rFactX, rFactY);

	dc.SelectObject(bmOld);

	BITMAPFILEHEADER bmf;
	bmf.bfType = 0x4d42; // "BM"
	bmf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + iImgSize;
	bmf.bfReserved1 = 0;
	bmf.bfReserved2 = 0;
	bmf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	
	BITMAPINFOHEADER bmi;
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = iWidth;
	bmi.biHeight = iHeight;
	bmi.biPlanes = 1;
	bmi.biBitCount = 24;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = iImgSize;
	bmi.biXPelsPerMeter = iPixPerMeter;
	bmi.biYPelsPerMeter = iPixPerMeter;
	bmi.biClrUsed = 0;
	bmi.biClrImportant = 0;

	char* lpBits = new char[iImgSize];
	GetDIBits(dc.m_hDC, bm, 0, iHeight, lpBits, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

	FileName fln(sFile);
	if (fln.sExt != ".bmp")
		sFile &= ".bmp";
	File fil(sFile, facCRT);
	fil.Write(sizeof(bmf), &bmf);
	fil.Write(sizeof(bmi), &bmi);
	fil.Write(iImgSize, lpBits);
	delete [] lpBits;
}

LRESULT LayoutView::OnPrintHelp(WPARAM, LPARAM)
{
	String sHelpFile (ChmFinder::sFindChmFile("ilwis.chm"));

	::HtmlHelp(::GetDesktopWindow(), sHelpFile.sVal(), HH_HELP_CONTEXT, htp.iTopic);
	::HtmlHelp(0, sHelpFile.sVal(), HH_DISPLAY_TOC, 0);

	return 0;
}

LRESULT LayoutView::OnViewSettings(WPARAM wP, LPARAM lP)
{
	if (wP == SAVESETTINGS) {
		LayoutDoc* ld = GetDocument();
		for (list<LayoutItem*>::iterator iter = ld->lli.begin(); iter != ld->lli.end(); ++iter) 
		{
			LayoutItem* li = *iter;
			li->SaveModified();
		}
	}
	return TRUE;
}

void LayoutView::OnEndPrinting(CDC* pDC, CPrintInfo *info)
{
	CView::OnEndPrinting(pDC, info);

	Invalidate();

}
