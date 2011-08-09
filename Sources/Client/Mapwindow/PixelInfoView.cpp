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
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\Base\datawind.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Client\Mapwindow\PixelInfoView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Map\Feature.h"
#include "Headers\Hs\PIXINFO.hs"
#include <afxole.h>
#include "Headers\messages.h"
#include "Headers\Hs\Table.hs"


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
	selectedRowIndex = iUNDEF;
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
	return TR("Customize the Pixel Information Window");
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
				pid->AddMapList(mpl, RecItem::atNORMAL,0);
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

#define sMen(ID) ILWSF("men",ID).c_str()
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

void PixelInfoView::OnFieldPressed(int iCol, long iRow, bool fLeft) {
	PixelInfoDoc *doc = GetDocument();
	selectedRowIndex = iUNDEF;

	deleteField();
	if (iCol < 0 || iCol >= iCols()
		|| iRow <= 0 || iRow > iRows()) 
	{
		MessageBeep(MB_ICONASTERISK);
		return;
	}
	if (fLeft) {
		if (0 == doc)
			return;
		if (doc->fRowEditable(iRow) ) {
			tField = new PixInfoField(this,iCol,iRow);
			selectedRowIndex = iRow;
		}  
		else {
			tField = new PixInfoReadOnlyField(this,iCol,iRow); 
		}  
	}
}

int PixelInfoView::getSelectedRow() const{
	return selectedRowIndex;
}

//--------------------------------------------------------------------
PixInfoField::PixInfoField(PixelInfoView* pane, int col, long row)
: BaseTblField(pane,col,row, pane->GetDocument()->getItem(row)->fnObj())
{
	PixelInfoDoc *doc = pane->GetDocument();
	String str = doc->sValue(row);
	init(str);
	RecItem *item = pane->GetDocument()->getItem(row);
	
	FileName fn = item->fnObj();
	IObjectType type = IOTYPE(FileName(fn.sFile + fn.sExt));
	if ( ISBASEMAP(type)) {
		BaseMap bmp(fn);
		dm = bmp->dvrs().dm();
	}
	if ( type == IlwisObject::iotTABLE && fn.sCol != "") {
		FileName fnObj(fn);
		fnObj.sCol = "";
		Table tbl(fnObj);
		Column col = tbl->col(fn.sCol);
		dm = col->dvrs().dm();
	}
}

PixInfoField::~PixInfoField()
{
	if (fHasChanged()) 
	{
		String s = sText();
		if ("" == s)
			s = "?";
		PixelInfoView* pane = (PixelInfoView*) tbpn;
		PixelInfoDoc *doc = pane->GetDocument();
		DomainValueRangeStruct dvs(dm);
		if (("?" != s) && !dvs.fValid(s)) {
			DomainSort* ds = dvs.dm()->pdsrt();
			if (0 != ds) {
				String sMsg(TR("%S is not in the domain %S\nAdd this item to the domain?").c_str(), s, ds->sName());
				int iRet = pane->MessageBox(sMsg.c_str(), TR("Invalid Value").c_str(),
					MB_YESNO|MB_ICONEXCLAMATION);
				if (IDNO == iRet)
				{
					init(doc->sValue(iRow));
					return;
				}
				if (IDYES == iRet){ 
					long raw = ds->iAdd(s);
					doc->getEditFeature()->PutVal(raw);
				}
			}
			else {
				String sMsg(TR("%S is not a valid value").c_str(), s);
				pane->MessageBox(sMsg.sVal(), TR("Invalid Value").c_str(),
					MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		RecItem *rcItem = doc->getItem(iRow);
		if ( rcItem && rcItem->getAssociatedTool() != 0) {
			rcItem->getAssociatedTool()->mpvGetView()->Invalidate();
		}
		doc->getItem(pane->getSelectedRow())->PutVal(s);

		//doc->UpdateAllViews(0,2);
	}
}

//--------------------------------------------------------------------------------

PixInfoReadOnlyField::PixInfoReadOnlyField(PixelInfoView* pane, int col, long row)
: BaseTblField(pane,col,row,true)
{
	PixelInfoDoc *doc = pane->GetDocument();
	String str = doc->sValue(row);
	init(str);
}

PixInfoReadOnlyField::~PixInfoReadOnlyField()
{
	// never changes ;-)
}
