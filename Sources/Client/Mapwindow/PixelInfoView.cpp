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
// PixelInfoView.cpp : implementation file
//
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Client\Mapwindow\PixelInfoView.h"
#include "Headers\Hs\PIXINFO.hs"
#include <afxole.h>
#include "Headers\messages.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PixelInfoView

IMPLEMENT_DYNCREATE(PixelInfoView, BaseTablePaneView)

BEGIN_MESSAGE_MAP(PixelInfoView, BaseTablePaneView)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


PixelInfoView::PixelInfoView()
{
	iButtonWidth = 12;
	odt = new COleDropTarget;
}

PixelInfoView::~PixelInfoView()
{
	delete odt;
}


/////////////////////////////////////////////////////////////////////////////
// PixelInfoView diagnostics

#ifdef _DEBUG
void PixelInfoView::AssertValid() const
{
	BaseTablePaneView::AssertValid();
}

void PixelInfoView::Dump(CDumpContext& dc) const
{
	BaseTablePaneView::Dump(dc);
}
#endif //_DEBUG


void PixelInfoView::InitColPix(CDC* cdc)
{
	if (0 == iColPix)
		return;
	int iRecWidth = iCharWidth * iButtonWidth + 6;
	iColPix[0] = iRecWidth;
	iColPix[1] = 32000;
	iColPix[2] = 32000;
	iColWidth[0] = 100;
}


/////////////////////////////////////////////////////////////////////////////
// PixelInfoView message handlers

PixelInfoDoc* PixelInfoView::GetDocument()
{
	return (PixelInfoDoc*)m_pDocument;
}

const PixelInfoDoc* PixelInfoView::GetDocument() const
{
	return (const PixelInfoDoc*)m_pDocument;
}

int PixelInfoView::iCols() const
{
	return 1;
}

long PixelInfoView::iRows() const
{
	if ( GetDocument())
		return GetDocument()->iSize();
	return iUNDEF;
}

String PixelInfoView::sColButton(int iCol) const
{
	return "";
}

String PixelInfoView::sRowButton(long iRow) const
{
	if ( GetDocument())
		return GetDocument()->sName(iRow);
	return sUNDEF;
}

String PixelInfoView::sField(int iCol, long iRow) const
{
	if ( GetDocument())
		return GetDocument()->sValue(iRow);
	return sUNDEF;
}

String PixelInfoView::sDescrULButton() const // upper left button
{
	return SPIRemCnfPixInfo;
}

void PixelInfoView::OnInitialUpdate()
{
	ShowScrollBar(SB_VERT);
	EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
	if (iColWidth)
		delete [] iColWidth;
	if (iColPix)
		delete [] iColPix;  
	iColWidth = new int[1];
	iColPix = new int[3];
	odt->Register(this);
	BaseTablePaneView::OnInitialUpdate();
}

void PixelInfoView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	BaseTablePaneView::OnUpdate(pSender, lHint, pHint);
}

void PixelInfoView::OnULButtonPressed()  // upper left button
{
	GetDocument()->OnPixConfigure();
}

DROPEFFECT PixelInfoView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
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
			if (fn.sExt == ".mps" || fn.sExt == ".mpa" || 
				fn.sExt == ".mpp" || fn.sExt == ".mpr" ||
				fn.sExt == ".csy" || fn.sExt == ".mpl" ||
				fn.sExt == ".grf") {
					fOk = true;
					// melding op status regel?
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

void PixelInfoView::OnDragLeave() 
{
	// TODO: Add your specialized code here and/or call the base class
}

DROPEFFECT PixelInfoView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	return OnDragEnter(pDataObject, dwKeyState, point);
	//return CView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL PixelInfoView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
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
	PixelInfoDoc* pid = GetDocument();
	for (int i = 0; i < iFiles; ++i) {
		FileName fn = afn[i];
		if (!fn.fExist()) {
			FileName fnSys = fn;
			String sStdDir = IlwWinApp()->Context()->sStdDir();
			fnSys.Dir(sStdDir);
			if (fnSys.fExist())
				fn = fnSys;
		}
		try {
			if (fn.sExt == ".csy") {
				CoordSystem csy(fn);
				if (csy.fValid()) {
					pid->AddCoordSystem(csy);
					fOk = true;
				}
			}
			else if (fn.sExt == ".grf") {
				GeoRef grf(fn);
				if (grf.fValid()) {
					pid->AddGeoRef(grf);
					fOk = true;
				}
			}
			else if (fn.sExt == ".mpl") {
				MapList mpl(fn);
				pid->AddMapList(mpl);
				fOk = true;
			}
			else {
				BaseMap mp(fn);
				pid->AddMap(mp);
				fOk = true;
			}  
		}
		catch(...) {}  
	}
	if (fOk)
		pid->UpdateAllViews(0,2);
	return fOk;
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void PixelInfoView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	int iCol, iRow;
	CPoint pt = point;
	ScreenToClient(&pt);
	ColRow(pt,iCol,iRow);
	if (iCol < 0)
	{
		iRow += iFirstVisibleRow() - 1;
		FileName fn = GetDocument()->fn(iRow);
		if (fn.fValid()) {
			IlwWinApp()->ShowPopupMenu(pWnd, point, fn);
			return;
		}
	}  
	CMenu men;
	men.CreatePopupMenu();
	add(ID_COPY );
	add(ID_ADDMAPS);
	add(ID_ADDCSYS);
	add(ID_ADDGRF);
	add(ID_PIXCONFIGURE);
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void PixelInfoView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int iCol, iRow;
	ColRow(point,iCol,iRow);
	if (iCol < 0)
	{
		iRow += iFirstVisibleRow() - 1;
		FileName fn = GetDocument()->fn(iRow);
		if (fn.fValid()) {
			String sCmd("show %S", fn.sFullNameQuoted());
			char* str = sCmd.sVal();
			IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
			return;
		}
	}
	BaseTablePaneView::OnLButtonDblClk(nFlags, point);
}

