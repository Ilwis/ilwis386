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
// Catalog.cpp : implementation file
//
#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "afxcview.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Base\ZappToMFC.h"
#include "Client\Base\Framewin.h"
#include "Client\MainWindow\Catalog\CatalogFrameWindow.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\ilwis.h"
#include "Engine\Base\File\COPIER.H"
#include "Client\Forms\copyobj.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Client\Forms\ObjectCopierUI.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Client\MainWindow\ASSOCSEL.H"
#include "Client\MainWindow\ACTION.H"
#include "Client\MainWindow\ACTPAIR.H"
//#include <shlobj.h>
#include <afxole.h>
#include "Client\MainWindow\ShowCursorDropSource.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\MainWindow\Catalog\CatalogPropertySheet.h"
#include "Client\Base\Menu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const POSITION pSTARTPOS=(POSITION)-1;

/////////////////////////////////////////////////////////////////////////////
// Catalog

IMPLEMENT_DYNCREATE(Catalog, CListView)

BEGIN_MESSAGE_MAP(Catalog, CListView)
	//{{AFX_MSG_MAP(Catalog)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_COMMAND(ID_VIEW_LIST, OnCatList)
	ON_COMMAND(ID_VIEW_DETAILS, OnCatDetails)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIST, OnUpdateCatList)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DETAILS, OnUpdateCatDetails)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_CONTEXTMENU()
	ON_WM_MENUSELECT()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_COMMAND(ID_CATVIEW_OPTIONS, OnCatViewOptions)
	ON_WM_KEYDOWN()
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHeaderClicked) 
	ON_NOTIFY(HDN_ITEMCHANGING, 0, OnHeaderSize)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_COMMAND(ID_FILE_OPEN, OnOpen)
	ON_COMMAND(ID_CAT_SHOW, OnShow)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEdit)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_CAT_DEL, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateSelectAll)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_CAT_DEL, OnUpdateEdit)
	ON_COMMAND(ID_OPENPIXELINFO, OnOpenPixelInfo)
	ON_COMMAND(ID_EDIT_OBJ, OnEditObject)
	ON_COMMAND(ID_CAT_PROP, OnProp)
	ON_COMMAND(ID_CAT_COPY, OnCopyTo)
	ON_COMMAND(ID_CREATE_OBJECTCOLLECTION,OnCreateObjectCollection)
	ON_UPDATE_COMMAND_UI(ID_CAT_PROP, OnPropertiesUpdateUI)
	ON_UPDATE_COMMAND_UI(ID_CAT_COPY, OnPropertiesUpdateUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OBJ, OnUpdateEditObject)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
	ON_COMMAND(ID_CAT_SHOW_MAPTBL, OnShowMapTables)
	ON_COMMAND(ID_CAT_SHOW_DATA, OnShowAllData)
	ON_COMMAND(ID_CAT_SHOW_ALL, OnShowAllObjects)
	ON_COMMAND(ID_CAT_SHOW_BASEMAPS, OnShowBaseMaps)
	ON_UPDATE_COMMAND_UI(ID_CAT_PYRAMIDS, OnUpdatePyramids)
	ON_COMMAND(ID_CAT_PYRAMIDS, OnPyramids)	
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_MWD_STARTSELBUT, ID_MWD_ENDSELBUT, OnSelBut)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MWD_STARTSELBUT, ID_MWD_ENDSELBUT, OnSelButUI)
	ON_COMMAND_RANGE(ID_CAT_SORTCOL_START, ID_CAT_SORTCOL_END, OnSortCol)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CAT_SORTCOL_START, ID_CAT_SORTCOL_END, OnSortColUI)
	ON_UPDATE_COMMAND_UI_RANGE(ID_START_CATALOGQUERIES, ID_END_CATALOGQUERIES, OnUpdateCatalogQueries)	
  ON_COMMAND_RANGE(ID_START_CATALOGQUERIES, ID_END_CATALOGQUERIES, OnCatalogQueries)	
END_MESSAGE_MAP()


Catalog::Catalog()
: assSel(0),
  iSortColumn(0),
	fSortAscending(false),
	fShowGrid(false),
	fShowAll(true),
	fFilling(false), fFillStop(false),useBasemaps(false)
{
	iFmtCopy = RegisterClipboardFormat("IlwisCopy");

	odt = new COleDropTarget;
	UINames["bit.dom"] = "Bit";
	UINames["boundsonly"] = "Boundary Only";
	UINames["bool.dom"] = "Bool";
	UINames["color.dom"] = "Color";
	UINames["coordsystemctp"] = "CTP";
	UINames["coordsystemdirect"] = "Direct";
	UINames["coordsystemformula"] = "Formula";
	UINames["count.dom"] = "Count";
	UINames["differential"] = "Differential";
	UINames["domainbinary"] = "Binary";
	UINames["domainbit"] = "Bit";
	UINames["domainbool"] = "Bool";
	UINames["domainclass"] = "Class";
	UINames["domaincolor"] = "Color";
	UINames["domaincoordbuf"] = "CoordBuf";
	UINames["domaingroup"] = "Group";
	UINames["domainidentifier"] = "Identifier";
	UINames["domainimage"] = "Image";	
	UINames["domainuniqueid"] = "Unique ID";
	UINames["domainnone"] = "None";
	UINames["domainpicture"] = "Picture";
	UINames["domainstring"] = "String";
	UINames["domainvalue"] = "Value";
	UINames["directlinear"] = "Direct Linear";
	UINames["flowdirection.dom"] = "Class";
	UINames["georefcorners"] = "Corners";
	UINames["georefctp"] = "TiePoints";
	UINames["georefdifferential"] = "Differential";
	UINames["georefdirectlinear"] = "DirectLinear";
	UINames["georeffactor"] = "Factor";
	UINames["georefmirrorrotate"] = "MirrorRotate";
	UINames["georefparallelprojective"] = "ParallelProjective";
	UINames["georefnone"] = "None";
	UINames["georeforthophoto"] = "Orthophoto";
	UINames["georefscalerotate"] = "ScaleRotate";
	UINames["georefsmpl"] = "Simple";
	UINames["georefsubmap"] = "SubMap";
	UINames["image.dom"] ="Image";
	UINames["latlon"] = "LatLon";
	UINames["latlonwgs84.csy"] = "LatLon";
	UINames["min1to1.dom"] = "Min1To1";
	UINames["noaa.dom"] = "Noaa";
	UINames["none.dom"] = "None";
	UINames["none.grf"] = "None";
	UINames["perc.dom"] = "Percentage";
	UINames["projection"] = "Projection";
	UINames["radar.dom"] = "Radar";
	UINames["tiepoints"] = "TiePoints";
	UINames["value.dom"] ="Value";
	UINames["uiniqueid.dom"] = "UniqueId";
	UINames["yesno.dom"] = "YesNo";

	farColumns.resize(ctALL);
	fill(farColumns.begin(), farColumns.end(), true);
	iImgOvlSystem =	IlwWinApp()->iImage("OverlaySystem");
	iImgOvlUseAs =	IlwWinApp()->iImage("OverlayUseAs");
}

Catalog::~Catalog()
{
	if (fFilling) {
		fFillStop = true;
		Sleep(10);
	}
	delete odt;

}

CatalogDocument* Catalog::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CatalogDocument)));
	return (CatalogDocument*)m_pDocument;
}

/////////////////////////////////////////////////////////////////////////////
// Catalog message handlers

void Catalog::EraseCatalog()
{
	CListCtrl& lvCtrl = GetListCtrl();
	lvCtrl.DeleteAllItems();
	while(lvCtrl.DeleteColumn(0));
	UpdateWindow();
}

void Catalog::OnInitialUpdate()
{
	//CListView::OnInitialUpdate();

	odt->Register(this);
    IlwisWinApp *app = IlwWinApp();

	CListCtrl& lvCtrl = GetListCtrl();

	lvCtrl.SetExtendedStyle(lvCtrl.GetExtendedStyle()|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT);

	lvCtrl.SetImageList(&app->ilSmall, LVSIL_SMALL);
	lvCtrl.SetImageList(&app->ilLarge, LVSIL_NORMAL);
	CatalogDocument* doc = GetDocument();

	CreateColumns();

	m_HeaderCtrl.SubclassWindow(::GetDlgItem( lvCtrl.m_hWnd, 0));	

	CMDIChildWnd *mdi = dynamic_cast<CMDIChildWnd *>(GetParentFrame());
	if ( mdi )
	{
		if ( doc->sRegName() != "")
		{
			MainWindow *wnd = (MainWindow *)(mdi->GetMDIFrame());
			IlwisSettings settings(wnd->sBaseRegKey());
			String sDocKey("%S\\%S", wnd->sBaseRegKey(), doc->sRegName());
			LoadViewSettings(sDocKey ,0);
		}
		else
		{
			IlwisSettings settings("MainWindow");
			if ( settings.fKeyExists("CatalogTemplate"))
			{
				String sDocKey("MainWindow\\CatalogTemplate");				
				LoadViewSettings(sDocKey, -1, true)	;
			}				

		}			
	}

	lvCtrl.SetIconSpacing(100,80);
	CFont * fnt = IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium);
	SetFont(fnt);
	FillCatalog();
}

void Catalog::CreateColumns()
{
	CListCtrl& lvCtrl = GetListCtrl();

	farColumns[ctNAME] = LVColumnInfo(true, 100, ctNAME);
	farColumns[ctTYPE] = LVColumnInfo(true, 120, ctTYPE);
	farColumns[ctDEPENDENT] = LVColumnInfo(true, 20, ctDEPENDENT);
	farColumns[ctCALCULATED] = LVColumnInfo(true, 20, ctCALCULATED);
	farColumns[ctUPTODATE] = LVColumnInfo(true, 20, ctUPTODATE);
	farColumns[ctREADONLY] = LVColumnInfo(true, 20, ctREADONLY);
	farColumns[ctMODIFIED] = LVColumnInfo(true, 100, ctMODIFIED);
	farColumns[ctDOMAIN] = LVColumnInfo(true, 70, ctDOMAIN);
	farColumns[ctDOMAINTYPE] = LVColumnInfo(true, 70, ctDOMAINTYPE);
	farColumns[ctCOORDSYSTEM] = LVColumnInfo(true, 70, ctCOORDSYSTEM);
	farColumns[ctCOORDSYSTEMTYPE] = LVColumnInfo(true, 70, ctCOORDSYSTEMTYPE);
	farColumns[ctGEOREF] = LVColumnInfo(true, 70, ctGEOREF);
	farColumns[ctGEOREFTYPE] = LVColumnInfo(true, 75, ctGEOREFTYPE);
	farColumns[ctGSIZE] = LVColumnInfo(true, 70, ctGSIZE);
	farColumns[ctATTRIBUTETABLE] = LVColumnInfo(true, 70, ctATTRIBUTETABLE);
	farColumns[ctDESCRIPTION] = LVColumnInfo(true, 200, ctDESCRIPTION);
	farColumns[ctEXPRESSION] = LVColumnInfo(true, 200, ctEXPRESSION);
	farColumns[ctOBJECTSIZE] = LVColumnInfo(true, 100, ctOBJECTSIZE);

	lvCtrl.InsertColumn(ctNAME, TR("Name").c_str(), LVCFMT_LEFT, farColumns[ctNAME].iWidth);
	lvCtrl.InsertColumn(ctTYPE, TR("Type").c_str(), LVCFMT_LEFT, farColumns[ctTYPE].iWidth);
	lvCtrl.InsertColumn(ctDEPENDENT, TR("D ").c_str(), LVCFMT_LEFT, farColumns[ctDEPENDENT].iWidth);
	lvCtrl.InsertColumn(ctCALCULATED, TR("C ").c_str(),  LVCFMT_LEFT, farColumns[ctCALCULATED].iWidth);
	lvCtrl.InsertColumn(ctUPTODATE, TR("U ").c_str(), LVCFMT_LEFT, farColumns[ctUPTODATE].iWidth);
	lvCtrl.InsertColumn(ctREADONLY, TR("R ").c_str(),  LVCFMT_LEFT, farColumns[ctREADONLY].iWidth);
	lvCtrl.InsertColumn(ctMODIFIED, TR("Modified").c_str(),  LVCFMT_LEFT, farColumns[ctMODIFIED].iWidth);
	lvCtrl.InsertColumn(ctDOMAIN, TR("Domain").c_str(), LVCFMT_LEFT, farColumns[ctDOMAIN].iWidth);
	lvCtrl.InsertColumn(ctDOMAINTYPE, TR("Domain type").c_str(), LVCFMT_LEFT, farColumns[ctDOMAINTYPE].iWidth);
	lvCtrl.InsertColumn(ctCOORDSYSTEM, TR("CoordSystem ").c_str(), LVCFMT_LEFT, farColumns[ctCOORDSYSTEM].iWidth);
	lvCtrl.InsertColumn(ctCOORDSYSTEMTYPE, TR("Csy type ").c_str(), LVCFMT_LEFT, farColumns[ctCOORDSYSTEMTYPE].iWidth);
	lvCtrl.InsertColumn(ctGEOREF, TR("Georef ").c_str(), LVCFMT_LEFT, farColumns[ctGEOREF].iWidth);
	lvCtrl.InsertColumn(ctGEOREFTYPE, TR("Georef type").c_str(),  LVCFMT_LEFT, farColumns[ctGEOREFTYPE].iWidth);
	lvCtrl.InsertColumn(ctGSIZE,  TR("Size").c_str(), LVCFMT_LEFT, farColumns[ctGSIZE].iWidth);	
	lvCtrl.InsertColumn(ctATTRIBUTETABLE, TR("Attribute ").c_str(), LVCFMT_LEFT, farColumns[ctATTRIBUTETABLE].iWidth);
	lvCtrl.InsertColumn(ctDESCRIPTION, TR("Description").c_str(), LVCFMT_LEFT, farColumns[ctDESCRIPTION].iWidth);
	lvCtrl.InsertColumn(ctEXPRESSION, TR("Expression ").c_str(), LVCFMT_LEFT, farColumns[ctEXPRESSION].iWidth);
	lvCtrl.InsertColumn(ctOBJECTSIZE, TR("Object size").c_str(), LVCFMT_LEFT, farColumns[ctOBJECTSIZE].iWidth);
}

void Catalog::FillCatalog()
{
	fFilling = true;
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	CStatusBar* pMessageBar = (CStatusBar *)pFrameWnd->GetMessageBar();
	pMessageBar->SetPaneText(MainWindow::spDESCRIPTION, TR("Reading Catalog...").c_str()); 
	
	QueryFileNames();
	SortColumn(iSortColumn);

	FillColumnStringInThread((void *)this); // initialy start in the same thread

	CListCtrl& lvCtrl = GetListCtrl();
	lvCtrl.InvalidateRect(NULL);
	lvCtrl.SetItemCountEx(vfn.size());

	pMessageBar->SetPaneText(MainWindow::spDESCRIPTION, "");
}

BOOL Catalog::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= LVS_LIST | LVS_REPORT | LVS_OWNERDATA | LVS_SHAREIMAGELISTS;
	cs.style &= ~(LVS_SORTASCENDING | LVS_SORTDESCENDING);
	cs.style |= LVS_SHOWSELALWAYS;
	return CListView::PreCreateWindow(cs);
}

void Catalog::QueryFileNames()
{
	if (!fil.fParse(sCatalogQuery))
		sCatalogQuery = "";
	vector<FileName> vfntmp, vfnDisplaytmp;
	CatalogDocument* cd = GetDocument();
	cd->GetFileNames(vfntmp, vfnDisplaytmp);
	vfn.resize(0);
	vfnDisplay.resize(0);
	viIndex.resize(0);

	int iIndex = 0;
	for(int i = 0; i<vfntmp.size(); ++i)
	{
		if ( fil.fOK(vfntmp[i]) )
		{
			vfn.push_back(vfntmp[i]);
			vfnDisplay.push_back(vfnDisplaytmp[i]);
			viIndex.push_back(iIndex++);
		}
	}
}

UINT Catalog::FillColumnStringInThread(void *data)
{
	Catalog *cat = (Catalog *)data;
	return cat->FillColumnStringInThread();
}
	
UINT Catalog::FillColumnStringInThread()
{
	vector< vector<String> > arsColStr;
	ILWISSingleLock lockStrings(&csLockColumnStrings, TRUE, SOURCE_LOCATION);

	arsColStr.resize(vfn.size());
	int iSz = vfn.size();
	lockStrings.Unlock();
	if (fFillStop) {
		fFilling = false;
		return -1;
	}

	for(int iRow = 0; iRow < iSz; ++iRow)
	{
		arsColStr[iRow].resize(ctALL);
		for (int iCol = 0; iCol < ctALL; ++iCol)
		{
			if (fFillStop) {
				fFilling = false;
				return -1;
			}
		}
	}
	int iNrCols;
	if (iSz > 0)
		iNrCols = arsColStr[0].size();
	else
		iNrCols = 0;
	
	if (fFillStop) {
		fFilling = false;
		return -1;
	}
	lockStrings.Lock(INFINITE,SOURCE_LOCATION);

	arsColumnStrings.clear();
	arsColumnStrings.resize(iSz);
	for (int iRow=0; iRow < iSz; ++iRow)
	{
		if (fFillStop) {
			fFilling = false;
			return -1;
		}
		arsColumnStrings[iRow].clear();
		arsColumnStrings[iRow].resize(iNrCols);
		copy(arsColStr[iRow].begin(), arsColStr[iRow].end(), arsColumnStrings[iRow].begin());
	}	
	Invalidate();
	fFilling = false;

	return 0;
}

void Catalog::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CListView::OnUpdate(pSender, lHint, pHint);

	mapSeenFiles.clear();
	for(unsigned int i=0; i < vfn.size(); ++i)
	{
		String sFile = vfn[i].sFullPath();
		sFile.toLower();
		mapSeenFiles[sFile] = false;
	}

	CListCtrl& lvCtrl = GetListCtrl();
	int iCurSel = lvCtrl.GetNextItem(-1, LVNI_FOCUSED);
	int iCurInd = -1;
	FileName fnLastSelected;
	if (iCurSel >= 0)
		fnLastSelected = vfn[viIndex[iCurSel]];
	if (1 == lHint) 
	{
		QueryFileNames();
		lvCtrl.SetItemCountEx(vfn.size());
		lvCtrl.InvalidateRect(NULL);
		int iRes = find(viIndex.begin(), viIndex.end(), iCurInd) - viIndex.begin();
		lvCtrl.SetItemState(iRes, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
		lvCtrl.EnsureVisible(iRes, FALSE);
	}
	lvCtrl.DeleteAllItems();
	lvCtrl.SetItemCountEx(vfn.size());
	lvCtrl.InvalidateRect(NULL);
	FillCatalog();
	if (fnLastSelected != FileName()) 
	{
		int iRes = find(vfn.begin(), vfn.end(), fnLastSelected) - vfn.begin();
		iRes = find(viIndex.begin(), viIndex.end(), iRes) - viIndex.begin();
		lvCtrl.SetItemState(iRes, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
		lvCtrl.EnsureVisible(iRes, FALSE);
	}
}

String Catalog::sODFValue(const FileName& fn, const String& sSection, const String& sEntry, bool fType, bool fExtension)
{
	if (IlwisObject::iotANY == IlwisObject::iotObjectType(fn))
		return "";
	String sVal, sReturn;
	int iRet = ObjectInfo::ReadElement(sSection.c_str(), sEntry.c_str(), fn, sVal);
	if ( iRet == 0 ) return "";
	int iLastPnt = sVal.rfind('.');
	if ( !fExtension )
		sReturn =  iLastPnt != -1 ? sVal.substr(0, iLastPnt) : sVal;
	else
		sReturn = sVal;

	if ( fType )
	{
		FileName fnType(sVal);
		// make sure we use the correct path in case of relative filenames
		// We can not be sure here that FileName::sRelative uses the correct working directory
		if (sVal.rfind('\\') == string::npos)
			fnType.Dir(fn.sPath());
		sReturn = sODFValue(fnType, sEntry.c_str(), "type", false);
		if ( sReturn == "")
			sReturn = sVal.toLower();
	}

	return sReturn;
}

long Catalog::iGetColumnString(int i, const FileName& fnObj)
{
	switch (farColumns[i].type)
	{
		case ctMODIFIED:
		{
			if ("drive" == fnObj.sExt || "directory" == fnObj.sExt)
				return 0;

			String sTime = sODFValue(fnObj, "Ilwis", "Time");
			CTime t;
			if ( sTime == "")
			{
				HANDLE hFile = CreateFile(fnObj.sFullName().c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
				                             FILE_ATTRIBUTE_NORMAL, NULL);
				if (0 == hFile) 
					return 0;
				if (INVALID_HANDLE_VALUE == hFile)
					return 0;
				FILETIME time;
				GetFileTime(hFile, NULL, NULL, &time);
				t = CTime(time);
				CloseHandle(hFile);
			}
			else
				t = CTime(sTime.iVal());
			return t.GetTime();
		}
		case ctOBJECTSIZE:
			if ("drive" == fnObj.sExt || "directory" == fnObj.sExt)
				return -1;
			return iObjectSize(fnObj);
		default:
			return 0;
	}
}

pair<long,long> Catalog::piGetColumnString(int i, const FileName& fnObj)
{
	typedef pair<long,long> pll;
	IlwisObject::iotIlwisObjectType ioType = IlwisObject::iotObjectType(fnObj);
	switch (farColumns[i].type)
	{
		case ctGSIZE:
		{
			String s1, s2;
			switch( ioType)
			{
				case IlwisObject::iotGEOREF:
					s1 = sODFValue(fnObj, "GeoRef", "Columns");
					s2 = sODFValue(fnObj, "GeoRef", "Lines");
					break;
				case IlwisObject::iotRASMAP:
				{
					String sSize = sODFValue(fnObj, "Map", "Size");
					s1 = sSize.sHead(" ");
					s2 = sSize.sTail(" ");
				} break;
				case IlwisObject::iotSEGMENTMAP:
					s1 = sODFValue(fnObj, "SegmentMapStore", "Segments");
					break;
				case IlwisObject::iotPOLYGONMAP:
					s1 = sODFValue(fnObj, "PolygonMapStore", "Polygons");
					break;
				case IlwisObject::iotPOINTMAP:
					s1 = sODFValue(fnObj, "PointMap", "Points");
					break;
				case IlwisObject::iotCOORDSYSTEM:
				{
					CoordBounds cb;
					ObjectInfo::ReadElement("CoordSystem", "CoordBounds", fnObj, cb);
					double rW = cb.width();
					double rH = cb.height();
					if (rW > MAXLONG)
						rW = MAXLONG;
					if (rH > MAXLONG)
						rH = MAXLONG;
					return pll(rW, rH);
				} break;
				case IlwisObject::iotDOMAIN:
					s1 = sODFValue(fnObj, "Table", "Records");
					break;
				case IlwisObject::iotTABLE:
					s1 = sODFValue(fnObj, "Table", "Records");
					s2 = sODFValue(fnObj, "Table", "Columns");
					break;
				default:
					return pll(iUNDEF,iUNDEF);
			}
			long i1 = s1.iVal();
			long i2 = s2.iVal();
			return pll(i1,i2);
		}
		default:
			return pll(iUNDEF,iUNDEF);
	}
}

String Catalog::sGetColumnString(int i, const FileName& fnObj, bool fExtension)
{
	String sResult;
	IlwisObject::iotIlwisObjectType ioType = IlwisObject::iotObjectType(fnObj);

	switch(farColumns[i].type)
	{
		case ctOBJECTSIZE:
		{
			if ("drive" == fnObj.sExt || "directory" == fnObj.sExt)
				return "";
			unsigned int iSize = iObjectSize(fnObj);
			if (iSize == 0 )
				return "";
			if (iSize < 1000)
				return String("%lu Byte", iSize);
			if (iSize < 1e6)
				return String("%.2f kB", (double)iSize / 1000);
			return String("%.2f MB", (double)iSize / 1e6);
		}					
		case ctMODIFIED:
		{

			if ("drive" == fnObj.sExt || "directory" == fnObj.sExt)
				return "";

			String sTime = sODFValue(fnObj, "Ilwis", "Time");
			CTime t;
			if ( sTime == "" )
			{
				HANDLE hFile = CreateFile(fnObj.sFullName().c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
					                             FILE_ATTRIBUTE_NORMAL, NULL);
				if (0 == hFile) 
					return "";
				if (INVALID_HANDLE_VALUE == hFile)
					return "";
				FILETIME time;
				GetFileTime(hFile, NULL, NULL, &time);
				t = CTime(time);
				CloseHandle(hFile);
			}
			else
				t = CTime(sTime.iVal());
			CString s = t.Format("%c");
			return String(s);
		}
			break;
		case ctGSIZE:
		{
				switch( ioType)
				{
					case IlwisObject::iotGEOREF:
					{
						String sX = sODFValue(fnObj, "GeoRef", "Columns");
						if ( sX == "") return "";
						String sY = sODFValue(fnObj, "GeoRef", "Lines");
						if ( sY == "") return "";
						return String("%S x %S", sX, sY);
					}
					case IlwisObject::iotRASMAP:
					{
						String sSize = sODFValue(fnObj, "Map", "Size");
						return String ("%S x %S", sSize.sTail(" "), sSize.sHead(" "));
					}
					case IlwisObject::iotSEGMENTMAP:
						return sODFValue(fnObj, "SegmentMapStore", "Segments");
					case IlwisObject::iotPOLYGONMAP:
						return sODFValue(fnObj, "PolygonMapStore", "Polygons");
					case IlwisObject::iotPOINTMAP:
						return sODFValue(fnObj, "PointMap", "Points");
					case IlwisObject::iotCOORDSYSTEM:
						{
							String sV = "";
							CoordBounds cb;
							ObjectInfo::ReadElement("CoordSystem", "CoordBounds", fnObj, cb);
							if (!cb.fUndef())
								sV = String("%lg %lg %lg %lg", cb.cMin.x, cb.cMin.y, cb.cMax.x, cb.cMax.y);
							return sV;
						}
					case IlwisObject::iotDOMAIN:
						return sODFValue(fnObj, "Table", "Records");
					case IlwisObject::iotTABLE:
						return String ( "%S, %S", sODFValue(fnObj, "Table", "Records"), sODFValue(fnObj, "Table", "Columns"));
				}
				
				return "";

		}
		case ctTYPE:
			{
				String sType = sODFValue(fnObj, "Ilwis", "Type");
				switch (ioType)
				{
					case IlwisObject::iotRASMAP:
					case IlwisObject::iotPOLYGONMAP:
					case IlwisObject::iotSEGMENTMAP:
					case IlwisObject::iotPOINTMAP:
					case IlwisObject::iotDOMAIN:
					case IlwisObject::iotREPRESENTATION:
						return sODFValue(fnObj, sType, "Type");
					case IlwisObject::iotHISTRAS:
						return "Histogram";
					case IlwisObject::iotHISTPOL:
						return "HistogramPol";
					case IlwisObject::iotHISTSEG:
						return "HistogramSeg";
					case IlwisObject::iotHISTPNT:
						return "HistogramPnt";
					case IlwisObject::iotTABLE2DIM:
						return "2-Dimensional Table";
					case IlwisObject::iotFILTER:
						return "Filter";
					default:
						return sType;
				}
			}
			break;
		case ctDOMAINTYPE:
		case ctDOMAIN:
		{
			bool fType = farColumns[i].type == ctDOMAINTYPE;
			switch (ioType)
			{
				case IlwisObject::iotPOINTMAP:
				case IlwisObject::iotSEGMENTMAP:
				case IlwisObject::iotRASMAP:
				case IlwisObject::iotPOLYGONMAP:
					sResult =  sODFValue(fnObj, "basemap", "domain", fType, fExtension); break;
				case IlwisObject::iotREPRESENTATION:
					sResult = sODFValue(fnObj, "representation", "domain", fType, fExtension); break;
				case IlwisObject::iotTABLE:
				case IlwisObject::iotHISTRAS:
				case IlwisObject::iotHISTPOL:
				case IlwisObject::iotHISTSEG:
				case IlwisObject::iotHISTPNT:
					sResult = sODFValue(fnObj, "table", "domain", fType, fExtension); break;
				case IlwisObject::iotSAMPLESET:
					sResult = sODFValue(fnObj, "sampleset", "domain", fType, fExtension); break;
				case IlwisObject::iotDOMAIN:
				{
					if ( fType)
					{
						sResult = fnObj.sRelative().toLower();
						break;
					}
				}
				default:
					return "";

			};
			if ( fType )
			{
				String sResultFind = UINames[sResult.toLower()];
				if ( sResultFind != "" ) 
					sResult = sResultFind;
				else
				{
					sResult = sODFValue(fnObj, "Domain", "type", false, fExtension);
					sResultFind = UINames[sResult.toLower()];
					if ( sResultFind != "" ) 
						sResult = sResultFind;
					else
						sResult = fnObj.sRelative(false);
				}
			}
			return sResult;
		};
		break;
		case ctGEOREFTYPE:
		case ctGEOREF:
		{
			bool fType = farColumns[i].type == ctGEOREFTYPE ? true : false;
			switch (ioType)
			{
				case IlwisObject::iotRASMAP:
					sResult = sODFValue(fnObj, "map", "georef", fType, fExtension); break;
				case IlwisObject::iotMAPLIST:
					sResult = sODFValue(fnObj, "maplist", "georef", fType, fExtension); break;
				case IlwisObject::iotGEOREF:
					  if ( farColumns[i].type == ctGEOREFTYPE )
							sResult = sODFValue(fnObj, "georef", "type", fType, fExtension);
					break;
				default:
					return "";
			}
			if ( fType )
			{
				String sResultFind = UINames[sResult.toLower()];
				if ( sResultFind != "" ) sResult = sResultFind;
			}
			return sResult;
		}
		break;
		case ctCOORDSYSTEMTYPE:
		case ctCOORDSYSTEM:
		{
			bool fType = farColumns[i].type == ctCOORDSYSTEMTYPE ? true : false;
			switch (ioType)
			{
				case IlwisObject::iotPOINTMAP:
				case IlwisObject::iotSEGMENTMAP:
				case IlwisObject::iotRASMAP:
				case IlwisObject::iotPOLYGONMAP:
					sResult = sODFValue(fnObj, "basemap", "coordsystem", fType, fExtension); break;
				case IlwisObject::iotCOORDSYSTEM:
					sResult = sODFValue(fnObj, "CoordSystem", "Type", fType, fExtension); break;
				default:
					return "";
			}
			if ( fType )
			{
				String sResultFind = UINames[sResult.toLower()];
				if ( sResultFind != "" ) 
					sResult = sResultFind;
				else
			{
					sResult = "";
				}
			}
			return sResult;
		}
		break;
		case ctDESCRIPTION:
			return sODFValue(fnObj, "ilwis", "description");
		case ctEXPRESSION:
		{
			switch (ioType)
			{
				case IlwisObject::iotPOINTMAP:
					return sODFValue(fnObj, "PointMapVirtual", "expression");
				case IlwisObject::iotSEGMENTMAP:
					return sODFValue(fnObj, "SegmentMapVirtual", "expression");
				case IlwisObject::iotPOLYGONMAP:
					return sODFValue(fnObj, "PolygonMapVirtual", "expression");
				case IlwisObject::iotRASMAP:
					return sODFValue(fnObj, "MapVirtual", "expression");
				case IlwisObject::iotTABLE:
					return sODFValue(fnObj, "TableVirtual", "expression");
			}
		}
			break;
		case ctDEPENDENT:
		case ctUPTODATE:
		{
			bool fCalc=false;
			if ("" == sODFValue(fnObj, "IlwisObjectVirtual", "Expression")) 
				return "";
			String sDep;
			switch (ioType)
			{
				case IlwisObject::iotPOINTMAP:
					sDep = sODFValue(fnObj, "PointMap", "type"); 
					break;
				case IlwisObject::iotSEGMENTMAP:
					sDep = sODFValue(fnObj, "SegmentMap", "type");
					break;
				case IlwisObject::iotPOLYGONMAP:
					sDep = sODFValue(fnObj, "PolygonMap", "type"); 
					break;
				case IlwisObject::iotRASMAP:
					sDep = sODFValue(fnObj, "Map", "type"); 
					break;
				case IlwisObject::iotTABLE:
					sDep = sODFValue(fnObj, "Table", "type"); 
					break;
				case IlwisObject::iotMAPLIST:
					sDep = sODFValue(fnObj, "MapList", "type");
					break;
				case IlwisObject::iotMATRIX:
					sDep = "Virtual";  // Matrix with an expression is always dependent
					break;
				case IlwisObject::iotSTEREOPAIR:
					sDep = sODFValue(fnObj, "StereoPair", "type"); 
					if (sDep == "") // not dependent stereopair, so:
						return "";  //   avoid having "?" in "D" and "U" column for stereopair
					break;
				default:
					return "";
			}	
			if (sDep == "") 
				return "?";
			bool fDependent =  fCIStrEqual(sDep.substr(sDep.size() - 7) , "virtual");
			if (farColumns[i].type == ctUPTODATE)
			{
				Catalog::CalcState csUpToDate = csTRUE;
				if ( !fDependent ) return "";
				csUpToDate = csTraceUpToDate(fnObj);
				switch (csUpToDate)
				{
					case csTRUE: return "U";
					case csFALSE: return "N";
					default: return "";  
				}
			}	
			else
				return fDependent ? "D" : "";

		}
		break;
		case ctCALCULATED:
		{
			if (sODFValue(fnObj, "IlwisObjectVirtual", "Expression") == "") 
				return "";
			if (sGetColumnString(ctDEPENDENT, fnObj) != "D") 
				return "";
			bool fCalc = false;
			switch (ioType)
			{
				case IlwisObject::iotPOINTMAP:
					fCalc = sODFValue(fnObj, "TableStore", "Col0") != "";
					break;
				case IlwisObject::iotSEGMENTMAP:
					fCalc = sODFValue(fnObj, "SegmentMapStore", "Segments") != ""; 
					break;
				case IlwisObject::iotPOLYGONMAP:
					fCalc = sODFValue(fnObj, "PolygonMapStore", "Polygons") != "";
					break;
				case IlwisObject::iotRASMAP:
					fCalc = sODFValue(fnObj, "MapStore", "Data") != "";
					break;
				case IlwisObject::iotTABLE:
					fCalc = sODFValue(fnObj, "TableStore", "Data") != "";
					break;
				case IlwisObject::iotMAPLIST:
					fCalc = sODFValue(fnObj, "MapList", "type") != "";
					break;
				case IlwisObject::iotMATRIX:
					fCalc = sODFValue(fnObj, "Matrix", "Values_Size") != "0";
					break;
				case IlwisObject::iotSTEREOPAIR:
					fCalc = sODFValue(fnObj, "StereoPair", "Left") != "";
					break;
				default:
					return "";
			}
			return fCalc ? "C" : "N";
		}
		break;
		case ctREADONLY:
		{
			if ("drive" == fnObj.sExt || "directory" == fnObj.sExt)
				return "";
			return GetFileAttributes(fnObj.sFullPath().c_str()) & FILE_ATTRIBUTE_READONLY ? "R" : "";
		}
		break;
		case ctATTRIBUTETABLE:
		{
			switch (ioType)
			{
				case IlwisObject::iotPOINTMAP:
				case IlwisObject::iotSEGMENTMAP:
				case IlwisObject::iotRASMAP:
				case IlwisObject::iotPOLYGONMAP:
					return sODFValue(fnObj, "basemap", "attributetable", fExtension);
				case IlwisObject::iotDOMAIN:
					return sODFValue(fnObj, "domainsort", "attributetable", fExtension);
			}
		}
		break;
	}	
	return "";
}

Catalog::CalcState Catalog::csTraceUpToDate(const FileName& fnObj, int iTime)
{
	String sVal;
	int iRet;
	if ( iTime == iUNDEF) // start
		iRet = ObjectInfo::ReadElement("Ilwis", "time", fnObj, iTime);
	else
	{
		int iCurTime;
		ObjectInfo::ReadElement("Ilwis", "time", fnObj, iCurTime);
		if ( iTime < iCurTime )
			return csFALSE;
	}

	String sDep = sGetColumnString(1, fnObj);
	if ( sDep == "" || sDep == "?" ) return csTRUE;
	String sCalc = sGetColumnString(2, fnObj);
	if ( sCalc == "N" || sCalc == "") return csFALSE;

	int iVal;
	ObjectInfo::ReadElement("ObjectDependency", "NrDepObjects", fnObj, iVal);

	for(int i=0; i< iVal; ++i)
	{
		FileName fnNew;
		String sFile;

		int iRet = ObjectInfo::ReadElement("ObjectDependency", String("object%d", i).c_str(), fnObj, sFile);
		if (0 == iRet) 
			return csUNKNOWN; // file does not exits anymore ??
		sFile.toLower();
		
		if (!mapSeenFiles[sFile]) // prevent erronous circular loops in dependencies
			mapSeenFiles[ sFile ] = true;
		else
		{
			mapSeenFiles[ sFile ] = false;
			return csUNKNOWN;
		}
		fnNew = FileName(sFile);
		if ( sFile.substr(0, 6) == "column")
		{
			String sColumn, sTable;
			String sTableColumn = sFile.substr(7, sFile.size());
			int iF = sTableColumn.rfind('.');
			if ( iF != string::npos )
			{
				sTable = sTableColumn.substr(0, iF);
				sColumn = sTableColumn.substr(iF);
			}	
			fnNew = FileName(sTable, ".tbt");
		}
		
		CalcState csState = csTraceUpToDate(fnNew, iTime);
		
		mapSeenFiles[ sFile ] = false;
		if ( csState == csFALSE ) return csFALSE;
		if ( csState == csUNKNOWN ) return csUNKNOWN;
	}
	return csTRUE;
}

String Catalog::sFieldValue(int iRow, int iCol)
{
	if (iRow < 0 || iRow >= vfnDisplay.size())
		return "";
	const FileName& fnDsp = vfnDisplay[iRow];
	const FileName& fn = vfn[iRow];

	switch (iCol) 
	{
	case 0:
		if (ObjectInfo::fSystemObject(fnDsp))
			return String("  %S", fnDsp.sFile);
		if (ObjectInfo::fUseAs(fnDsp) )
			return String("  %S", fnDsp.sFile);
		if ("drive" == fnDsp.sExt || "directory" == fnDsp.sExt)
			return fnDsp.sFile;
		if (IlwisObject::iotObjectType(fn) == IlwisObject::iotANY)
			return String("%S%S", fnDsp.sFile, fnDsp.sExt);
		else
			return fnDsp.sFile;
	default:
		if (farColumns[iCol].fVisible)
			return sGetColumnString(iCol, fn);
	}
	return "";
}

#pragma warning( disable : 4786 )
void Catalog::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ILWISSingleLock lockStrings(&csLockColumnStrings, TRUE, SOURCE_LOCATION);

	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM& item = pDispInfo->item;	
	int iSz = arsColumnStrings.size();
	if ( iSz == 0) return;
	int iColSz = arsColumnStrings[item.iItem].size();
	if ( viIndex.size() <= item.iItem)
		return;
	int iRow = viIndex[item.iItem];
	int iCol = item.iSubItem;			
	if ( iRow < iSz && iCol < iColSz )
	{
		if (item.mask & LVIF_TEXT) 
		{
				sTemp = arsColumnStrings[iRow][iCol]; 
				if ("" == sTemp) {
					sTemp = arsColumnStrings[iRow][iCol] = sFieldValue(iRow, iCol);
					if ("" == sTemp)
						sTemp = arsColumnStrings[iRow][iCol] = "%"; // remember that should be empty
				}
				if ("%" == sTemp)
					sTemp = "";
				item.pszText = sTemp.sVal();
		}
		if (item.mask & LVIF_IMAGE) 
		{
			const FileName& fnDsp = vfnDisplay[iRow];
			item.iImage = IlwWinApp()->iImage(fnDsp.sExt);
			if ( fnDsp.sExt == "drive")
			{
				switch(GetDriveType(item.pszText) )
				{
					case DRIVE_CDROM:
						item.iImage  = IlwWinApp()->iImage("CDRomDrive"); break;
					case DRIVE_REMOVABLE:
						item.iImage  = IlwWinApp()->iImage("FloppyDrive"); break;
					case DRIVE_REMOTE:
						item.iImage  = IlwWinApp()->iImage("NetworkDrive");break;
					default:
						item.iImage  = IlwWinApp()->iImage("drive");
				}
			}
			if ( item.iImage == 0 )
				item.iImage = IlwWinApp()->iImage("ForeignFile");
		}
		*pResult = 0;
	}
}	

struct SortItem
{
	SortItem(const string& s) 
		: sItem(s), iIndex(0), i1(0), i2(0) {}
	SortItem(long iNr=0) 
		: iIndex(0), i1(iNr), i2(0) {}
	SortItem(pair<long,long> pl) 
		: iIndex(0), i1(pl.first), i2(pl.second) {}
	long i1, i2;
	string sItem, sFile;
	int iIndex;
};

bool operator < (const SortItem& it1, const SortItem& it2)
{ 
	if (it1.i2 == it2.i2)
		if (it1.i1 == it2.i1)
			if (it1.sItem == it2.sItem)
				return it1.sFile < it2.sFile;
			else
				return it1.sItem < it2.sItem; 
		else
			return it1.i1 < it2.i1; 
	else
		return it1.i2 < it2.i2; 
}

int Catalog::iSortedColumn()
{
	return iSortColumn;
}

void Catalog::SortColumn(int i)
{
	iSortColumn = i;
	CListCtrl& lc = GetListCtrl();
	CHeaderCtrl *hdr = lc.GetHeaderCtrl();
	m_HeaderCtrl.SetSortImage(iSortColumn, fSortAscending);	
	
	if (iSortColumn < 0)
		return;  // no sorting required (maplist)

	CListCtrl& lvCtrl = GetListCtrl();
	int iCurSel = lvCtrl.GetNextItem(-1, LVNI_FOCUSED);

	if ( viIndex.size() <= iCurSel)
		return;

	int iCurInd = iCurSel >= 0 ? viIndex[iCurSel] : -1;

	vector<SortItem> items;
	int iSize = vfn.size();
	items.resize(iSize);
	viIndex.resize(iSize);
	int iStartSystem = iUNDEF, index=0;
	for(vector<FileName>::iterator cur=vfn.begin(); cur != vfn.end(); ++cur, ++index)
	{
		SortItem& si = items[index];
		FileName fn = vfnDisplay[index];
		if ("drive" == fn.sExt || "directory" == fn.sExt)
		{
			if ( iStartSystem == iUNDEF)
				iStartSystem = index;
			si = SortItem(vfn[index].sFullPath());
		}
		else
		{
			switch (iSortColumn) 
			{
				case ctNAME:
					si = SortItem(vfnDisplay[index].sShortName().toLower());
					break;
				case ctMODIFIED:
					si = SortItem(iGetColumnString(i, vfn[index]));	
					break;
				case ctGSIZE:
					si = SortItem(piGetColumnString(i, vfn[index]));	
					break;
				case ctOBJECTSIZE:
					si = SortItem(iGetColumnString(i, vfn[index]));
					break;
				default:
					si = SortItem(sGetColumnString(i, vfn[index]).toLower());	
			}
			si.sFile = vfnDisplay[index].sShortName().toLower();
		}
		si.iIndex = index;
	}
	if ( iStartSystem == iUNDEF ) // only with filters;
		iStartSystem = items.size(); // at the end;

	sort(items.begin(), items.begin() + iStartSystem);

	if ( iStartSystem == iUNDEF)
		iStartSystem = vfn.size() - 1;
	index = fSortAscending ? iStartSystem - 1: 0;
	for(vector<SortItem>::iterator cur2 = items.begin(); cur2!=items.end(); ++cur2)
	{
		if ( (*cur2).iIndex >= iStartSystem )
			continue;
		else if ( fSortAscending )
			viIndex[index--] = (*cur2).iIndex;
		else
			viIndex[index++] = (*cur2).iIndex;
	}
	for (i = iStartSystem; i < iSize; ++i)
		viIndex[i] = i;

	lvCtrl.InvalidateRect(NULL);
	int iRes = find(viIndex.begin(), viIndex.end(), iCurInd) - viIndex.begin();
	iCurSel = lvCtrl.GetNextItem(-1, LVNI_FOCUSED);
	if (iCurSel >= 0)
		lvCtrl.SetItemState(iCurSel, 0, 0x000F);                      
	lvCtrl.SetItemState(iRes, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);                      
	lvCtrl.EnsureVisible(iRes, FALSE);
}

void Catalog::OnCatList()
{
	LONG dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= ~LVS_TYPEMASK;
	dwStyle |= LVS_LIST;
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
}

void Catalog::OnCatDetails()
{
	LONG dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= ~LVS_TYPEMASK;
	dwStyle |= LVS_REPORT;
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
}

void Catalog::OnUpdateCatList(CCmdUI* pCmdUI)
{
	LONG dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= LVS_TYPEMASK;
	BOOL fOn = dwStyle == LVS_LIST;
	pCmdUI->SetRadio(fOn);
}

void Catalog::OnUpdateCatDetails(CCmdUI* pCmdUI)
{
	LONG dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= LVS_TYPEMASK;
	BOOL fOn = dwStyle == LVS_REPORT;
	pCmdUI->SetRadio(fOn);
}

bool Catalog::fIsManualSortAvail()
{
	return false;
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addsub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 

void Catalog::ShowContextViewMenu(CWnd* pWnd, CPoint point)
{
	CMenu men, menSub;
	men.CreatePopupMenu();
	pmadd(ID_VIEW_LIST);
	pmadd(ID_VIEW_DETAILS);
	men.AppendMenu(MF_SEPARATOR);
	pmadd(ID_EDIT_PASTE);
	if (IsClipboardFormatAvailable(iFmtCopy))
		men.EnableMenuItem(ID_EDIT_PASTE, MF_BYCOMMAND|MF_ENABLED);
	else
		men.EnableMenuItem(ID_EDIT_PASTE, MF_BYCOMMAND|MF_GRAYED);

	men.AppendMenu(MF_SEPARATOR);

	menSub.CreateMenu();
	addsub(ID_CAT_SHOW_MAPTBL);
	addsub(ID_CAT_SHOW_DATA);
	addsub(ID_CAT_SHOW_ALL);
	addsub(ID_CAT_SHOW_BASEMAPS);
	men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_CAT_SHOWMENU)); 
	menSub.Detach();

	menSub.CreateMenu();
	addsub(ID_CAT_SORTCOL_START + ctNAME); 
	addsub(ID_CAT_SORTCOL_START + ctTYPE); 
	addsub(ID_CAT_SORTCOL_START + ctMODIFIED); 
	addsub(ID_CAT_SORTCOL_START + ctCALCULATED); 
	addsub(ID_CAT_SORTCOL_START + ctDEPENDENT);
	addsub(ID_CAT_SORTCOL_START + ctUPTODATE); 
	addsub(ID_CAT_SORTCOL_START + ctREADONLY); 
	addsub(ID_CAT_SORTCOL_START + ctDOMAIN); 
	addsub(ID_CAT_SORTCOL_START + ctDOMAINTYPE); 
	addsub(ID_CAT_SORTCOL_START + ctCOORDSYSTEM); 
	addsub(ID_CAT_SORTCOL_START + ctCOORDSYSTEMTYPE); 
	addsub(ID_CAT_SORTCOL_START + ctGEOREF); 
	addsub(ID_CAT_SORTCOL_START + ctGEOREFTYPE); 
	addsub(ID_CAT_SORTCOL_START + ctGSIZE); 
	addsub(ID_CAT_SORTCOL_START + ctATTRIBUTETABLE); 
	addsub(ID_CAT_SORTCOL_START + ctDESCRIPTION); 
	addsub(ID_CAT_SORTCOL_START + ctOBJECTSIZE);		
	addsub(ID_CAT_SORTCOL_START + ctEXPRESSION); 
	men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_CAT_SORT)); 
	menSub.Detach();
	men.CheckMenuRadioItem(ID_CAT_SORTCOL_START, ID_CAT_SORTCOL_END, ID_CAT_SORTCOL_START + iSortedColumn(), MF_BYCOMMAND);

	pmadd(ID_CATVIEW_OPTIONS);

	LONG dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= LVS_TYPEMASK;
	if (dwStyle == LVS_LIST)
		men.CheckMenuRadioItem(ID_VIEW_LIST,ID_VIEW_LIST,ID_VIEW_LIST,MF_BYCOMMAND);
	else if (dwStyle == LVS_REPORT)
		men.CheckMenuRadioItem(ID_VIEW_DETAILS,ID_VIEW_DETAILS,ID_VIEW_DETAILS,MF_BYCOMMAND);
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
}

bool Catalog::CalcMenuProps(CWnd* pWnd, CPoint point, FileName& fn, FileName& fnFileSub, 
															int& iNr, int& iSub, bool& fReport)
{
	CListCtrl& lc = GetListCtrl();

	iNr = lc.GetSelectedCount();
	if (0 == iNr)
	{
		ShowContextViewMenu(pWnd, point);
		return false; // only show menu once
	}

	POSITION pos = lc.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return false; // something wrong do not show menu

	int id = lc.GetNextSelectedItem(pos);
	if (id < 0)
	{
		ShowContextViewMenu(pWnd, point);
		return false; // only show menu once
	}
	fnFileSub = vfn[viIndex[id]]; // default value

	// sub item
	CHeaderCtrl *hdr = lc.GetHeaderCtrl();
	int iW = 0;

	CPoint pt = point;
	ScreenToClient(&pt);
	int iSX = lc.GetScrollPos(SB_HORZ);   
	int iNColumns = hdr->GetItemCount();
	for( iSub=0 ; iSub< iNColumns ; ++iSub)
	{
		iW += lc.GetColumnWidth(iSub);
		if ( iW > iSX + pt.x)
			break;
	}
	LONG dwStyle = lc.GetStyle(); //GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= LVS_TYPEMASK;
	fReport=  dwStyle == LVS_REPORT;
	if ( fReport && iSub > 0 )
	{
		int *ar = new int[iNColumns];
		hdr->GetOrderArray(ar, iNColumns);
		int iCol = ar[iSub];
		String sFile = sGetColumnString(iCol, vfn[id], true);
		fnFileSub = FileName(sFile);
		delete [] ar;
	}

	// Context menu was started from the keyboard (Shift+F10), calculate appropriate position
	if (point.x == -1 && point.y == -1) 
	{
		CRect rect, rectWin;
		lc.GetItemRect(id, &rect, LVIR_BOUNDS);
		lc.GetClientRect(&rectWin);
		point.x = (rect.left + min(rect.right, rectWin.right))/4;
		point.y = (rect.top + rect.bottom)/2;
		ClientToScreen(&point);
	}
	fn = vfn[viIndex[id]];	
  return true; // ok, show menu
}

void Catalog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	FileName fn, fnFileSub;
	int iNr, iSub;
	bool fReport;
	// CalcMenuProps() sometimes already shows a menu, in that case return
	if (!CalcMenuProps(pWnd, point, fn, fnFileSub, iNr, iSub, fReport))
		return;

	CListCtrl& lc = GetListCtrl();
	if (iNr > 1) 
	{
		POSITION pos = lc.GetFirstSelectedItemPosition();
		FileName fn;
		bool fToPixInfo = false;
		while (pos) 
		{
			int id = lc.GetNextSelectedItem(pos);
			if (id >= 0) 
			{
				id = viIndex[id];
				fn = vfn[id];
				if ( fn.sExt == ".csy" || fn.sExt == ".mpr" || fn.sExt == ".mpp" ||
				   fn.sExt == ".mpa" || fn.sExt == ".mps")
				{
					fToPixInfo = true;
				}
			}
		}
		CMenu men;
		men.CreatePopupMenu();
		pmadd(ID_CAT_SHOW);
		men.AppendMenu(MF_SEPARATOR);
		//if ( fToPixInfo)
		//{
		//	pmadd(ID_OPENPIXELINFO);
		//	men.AppendMenu(MF_SEPARATOR);
		//}
		pmadd(ID_CREATE_OBJECTCOLLECTION);
		pmadd(ID_EDIT_COPY);
		pmadd(ID_EDIT_PASTE);
		pmadd(ID_CAT_DEL );
		men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
		men.Detach();
	}
	else if ( fReport && iSub > 0 ) 
	{
		if ( fnFileSub.sExt == "") // not a file;
		{
			ShowContextViewMenu(pWnd, point);
			return;
		}
		CMenu men;
		men.CreatePopupMenu();
		pmadd(ID_CAT_SHOW);
		pmadd(ID_CAT_PROP);
		String sCmd;
		switch (men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, this))
		{
			case ID_CAT_SHOW:
				sCmd = String("open %S", fnFileSub.sFullPathQuoted());
				break;
			case ID_CAT_PROP:
				sCmd = String("prop %S",fnFileSub.sFullPathQuoted());
				break;
		}
		char* str = sCmd.sVal();
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
		return;
	}
	else if ("drive" == fn.sExt || "directory" == fn.sExt) 
	{
		CMenu men;
		men.CreatePopupMenu();
		pmadd(ID_CAT_OPENHERE);
		pmadd(ID_CAT_SHOW);
		men.SetDefaultItem(ID_CAT_OPENHERE);
		switch (men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, this))
		{
			case ID_CAT_OPENHERE: 
			{
				CDocument* doc = GetDocument();
				doc->OnOpenDocument(fn.sFile.c_str());
				doc->UpdateAllViews(0);
				break;
			}
			case ID_CAT_SHOW:
			{
				String sCmd("opendir %S", fn.sFile);
				char* str = sCmd.sVal();
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
			}
		}
	}
	else if (fn != FileName() && IlwisObject::iotObjectType( fn) == IlwisObject::iotANY ) // foreign file
	{
		CMenu men;
		men.CreatePopupMenu();
		pmadd(ID_CAT_SHOW);
		men.AppendMenu(MF_SEPARATOR);
		pmadd(ID_EDIT_COPY);
		pmadd(ID_CAT_DEL );

		men.SetDefaultItem(ID_CAT_SHOW);
		String sCmd;
		switch (men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, this))
		{
			case ID_CAT_SHOW:
				sCmd = String("open %S", fnFileSub.sFullPathQuoted());
				break;
			case ID_EDIT_COPY:
				OnCopy();
				break;
			case ID_CAT_DEL:
				OnDelete();
				break;
		}
		char* str = sCmd.sVal();
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
		return;
	}		
	else 
	{
		IlwWinApp()->ShowPopupMenu(this, point, fn, &assSel);
	}
}

void Catalog::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) 
{
	if (assSel)
		assSel->OnMenuSelect(this, nItemID, nFlags);
}

void Catalog::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* nmlv = (NMLISTVIEW*) pNMHDR;
	int iItem = nmlv->iItem;
	if (iItem == -1)
		return;
	iItem = viIndex[iItem];
	FileName& fn = vfn[iItem];
	String sCmd;
	if ("directory" == fn.sExt || "drive" == fn.sExt) 
	{
		String sCmd("cd %S", fn.sFile);
		IlwWinApp()->SetCommandLine(sCmd);
	}
	else {
		String s;
		ActionPair* ap = IlwWinApp()->apl()->ap(s, fn);
		if (ap) {
			String sCmd = ap->sExec(fn);
			IlwWinApp()->SetCommandLine(sCmd);
		}
	}
	*pResult = 0;
}

void Catalog::Execute(int iItem)
{
	if (iItem == -1)
		return;
	iItem = viIndex[iItem];
	FileName& fn = vfn[iItem];
	if ("directory" == fn.sExt || "drive" == fn.sExt) 
	{
		CDocument* doc = GetDocument();
		doc->OnOpenDocument(fn.sFile.c_str());
		doc->UpdateAllViews(0);
	}
	else if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotANY )
	{
		String sCmd("open %S", fn.sRelativeQuoted());
		char* str = sCmd.sVal();
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
	}	
	else {
		String s;
		ActionPair* ap = IlwWinApp()->apl()->ap(s, fn);
		if (0 == ap)
			return;
		String sCmd = ap->sExec(fn);
		char* str = sCmd.sVal();
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
	}
}

void Catalog::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	NMLISTVIEW* nmlv = (NMLISTVIEW*) pNMHDR;
	int iItem = nmlv->iItem;
	Execute(iItem);
}


void Catalog::OnMouseMove(UINT nFlags, CPoint point) 
{
	CListView::OnMouseMove(nFlags, point);

	CListCtrl& lc = GetListCtrl();
	int id = lc.HitTest(point);
	ShowStatusLineInfo(id);
}

void Catalog::ShowStatusLineInfo(int id)
{
	if (-1 != id) {
		id = viIndex[id];
		CFrameWnd* pFrameWnd = GetTopLevelFrame();
		CStatusBar* pMessageBar = (CStatusBar *)pFrameWnd->GetMessageBar();
		FileName& fn = vfnDisplay[id];
		if (pMessageBar)
		{
			String sAttrib = sGetColumnString(Catalog::ctDEPENDENT, fn);
			pMessageBar->SetPaneText(MainWindow::spDEP, sAttrib.sVal()); 
			sAttrib = sGetColumnString(Catalog::ctCALCULATED, fn);
			pMessageBar->SetPaneText(MainWindow::spCALC, sAttrib.sVal()); 
			sAttrib = sGetColumnString(Catalog::ctUPTODATE, fn);
			pMessageBar->SetPaneText(MainWindow::spUPD, sAttrib.sVal()); 
			sAttrib = sGetColumnString(Catalog::ctREADONLY, fn);
			pMessageBar->SetPaneText(MainWindow::spREAD, sAttrib.sVal()); 
			String sDsc = ObjectInfo::sDescr(fn);
			if (sDsc == "") {
				String sType;
				FileName& fnDisk = vfn[id];
				ObjectInfo::ReadElement("Ilwis", "Type", fnDisk, sType);
				if (sType == "") {
					if (fn.sExt == "drive")
						sDsc = String("Drive %S", fn.sFile); 
					else if (fn.sExt == "directory")
						sDsc = String("Folder %S", fn.sFile); 
				}
				else {
					if (sType == "BaseMap")
						ObjectInfo::ReadElement("BaseMap", "Type", fnDisk, sType);
					else if (sType == "Function")
						ObjectInfo::ReadElement("Function", "Type", fnDisk, sType);
					sDsc = String("%S \"%S\"", sType, fn.sFile); 
				}
			}
			pMessageBar->SetPaneText(MainWindow::spDESCRIPTION, sDsc.sVal()); 
		}
	}
}

BOOL Catalog::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return FALSE;

	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	int iFiles = 0;
	vector<FileName> afn;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i) 
		{
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
			FileName fn(sFileName);
			if ( fn.fExist())
				afn.push_back(fn);
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);

	if (1 == afn.size()) {
		CListCtrl& lc = GetListCtrl();
		int id = lc.HitTest(point);
		if (id >= 0) {
			id = viIndex[id];
			FileName fnOn = vfn[id];
		  FileName fnDrag = afn[0];
			ActionPair* ap = IlwWinApp()->apl()->ap(fnDrag, fnOn);
			if (ap && fnDrag.fExist() ) {
				String sCmd = ap->sExec(fnDrag, fnOn);
				char* str = sCmd.sVal();
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
				return TRUE;
			}
			else 
				return FALSE;
		}
	}
	if (afn.size() > 0)
	{
		String sFiles;
		CatalogDocument *doc = GetDocument();
		FileName fnTo(doc->sGetPathName());
		for(vector<FileName>::iterator cur = afn.begin(); cur != afn.end(); ++cur)
		{
			String sPath = (*cur).sPath().toLower();
			sFiles += (*cur).sFullPathQuoted() + " ";
		}

		if ( sFiles == "") return FALSE;
		String sCmd;
		if (fnTo.sExt != "")
			sCmd = String("copy %S %S", sFiles, fnTo.sFullPathQuoted());
		else
			sCmd = String("copy %S %S", sFiles, Directory(fnTo).sFullPathQuoted());

		char* strc = sCmd.sVal();
		CWaitCursor cwr;		
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)strc);
	}
	return TRUE;
}

void Catalog::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	CListCtrl& lc = GetListCtrl();
	int iNr = lc.GetSelectedCount();
	if (0 == iNr)
		return;

	String sFileList;
	POSITION pos = lc.GetFirstSelectedItemPosition();
	FileName fn;
	while (pos) {
		int id = lc.GetNextSelectedItem(pos);
		if (id >= 0) {
			id = viIndex[id];
			fn = vfn[id];
			if ( fn.sExt != "drive" && fn.sExt != "directory" )
			{
				sFileList &= fn.sFullPathQuoted();
				sFileList &= '\0';
			}
		}
	}
	sFileList &= '\0';

	int iLen = sFileList.size();
	HGLOBAL hnd = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES)+iLen);
	DROPFILES* df = (DROPFILES*)GlobalLock(hnd);
	df->pFiles = sizeof(DROPFILES);
	df->fWide = false;
	df->pt = pNMListView->ptAction;
	df->fNC = false;
	memcpy(df + 1, sFileList.c_str(), iLen);
	GlobalUnlock(hnd);

	COleDataSource ods;
	ods.CacheGlobalData(CF_HDROP, hnd);

	if (1 == iNr) {
		const char* sExt = fn.sExt.c_str() + 1;
		String sCur("Arr%sCur", sExt);
		zCursor cur(sCur.c_str());
		ShowCursorDropSource scds(cur);
		ods.DoDragDrop(DROPEFFECT_COPY, 0, &scds);
	}
	else
		ods.DoDragDrop(DROPEFFECT_COPY);
}

DROPEFFECT Catalog::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;

	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	CStatusBar* pMessageBar = (CStatusBar *)GetParentFrame()->GetMessageBar();
	bool fOk = false;
	int id = -1;
	DROPEFFECT deResult = DROPEFFECT_NONE;
	if (hDrop)
	{
		char sFileName[MAX_PATH+1];
		int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		// if single item on item, try actionpair
		if (1 == iFiles)
		{
			CListCtrl& lc = GetListCtrl();
			id = lc.HitTest(point);
			if (id >= 0)
			{
				id = viIndex[id];
				FileName fnOn = vfn[id];
				DragQueryFile(hDrop, 0, sFileName, MAX_PATH+1);
				FileName fnDrag(sFileName);
				ActionPair* ap = IlwWinApp()->apl()->ap(fnDrag, fnOn);
				if (ap && fnDrag.fExist())
				{
					String sCmd = ap->sExec(fnDrag, fnOn);
					String sDesc = ap->sDescription(fnDrag, fnOn);
					IlwWinApp()->SetCommandLine(sCmd);
					pMessageBar->SetWindowText(sDesc.c_str());
					deResult = DROPEFFECT_COPY;
				}
				else
				{
					IlwWinApp()->SetCommandLine("");
					pMessageBar->SetWindowText("");
					deResult = DROPEFFECT_NONE;
				}
			}
		}
		if (iFiles > 1 || id < 0)
		{
			// Handle:
			//  - Number of files to drop larger than one
			//  - Single file to drop, for which no actionpair could be found (id < 0)
			IlwWinApp()->SetCommandLine("");
			pMessageBar->SetWindowText("copy"); // better message ?

			CatalogDocument* cd = GetDocument();
			String sCurDir = cd->sGetPathName();
			DragQueryFile(hDrop, 0, sFileName, MAX_PATH+1);
			FileName fn(sFileName);
			bool fSameDir = fCIStrEqual(fn.sPath(), sCurDir);

			for (int i = 0; i < iFiles; ++i)
			{
				DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
				FileName fn(sFileName);
	
				// if on white space, action would be copy
				if ( fn.fExist())
				{
					if (IlwisObject::iotObjectType(fn) == IlwisObject::iotTABLE)
					{
						bool fUseAs;
						ObjectInfo::ReadElement("Ilwis", "UseAs", fn, fUseAs);
						if (fSameDir || !fUseAs)   // UseAs tables cannot be coM_PId to another directory
						{
							deResult = DROPEFFECT_COPY;
							break;
						}
					}
					else if (IlwisObject::iotObjectType(fn) == IlwisObject::iotOBJECTCOLLECTION)
					{
						String sType;
						ObjectInfo::ReadElement("Ilwis", "Type", fn, sType);
						if (fSameDir || !fCIStrEqual(sType, "DataBaseCollection"))  // Cannot copy database collections
						{
							deResult = DROPEFFECT_COPY;  // no restrictions for other objects than tables
							break;
						}
					}
					else   // no restrictions for other objects than tables/DB Collections
					{
						deResult = DROPEFFECT_COPY;
						break;
					}
				}
			}
		}
	}
	GlobalUnlock(hDrop);

	return deResult;
}

void Catalog::SetCatalogQuery(const String& sF)
{
	sCatalogQuery = sF;
	SetFilterPane();

}

void Catalog::SetFilterPane()
{
	CStatusBar* pMessageBar = (CStatusBar *)GetParentFrame()->GetMessageBar();
	if ( pMessageBar)
	{
		if ( sCatalogQuery != "")
		{
			pMessageBar->GetStatusBarCtrl().SetText(
				String("Query : %S", sCatalogQuery).c_str(), MainWindow::spFILTER_PANE, 0);
		}
		else
			pMessageBar->GetStatusBarCtrl().SetText("Query : None", MainWindow::spFILTER_PANE, 0);
	}
}

void Catalog::SaveCatalogView(const String& sDocKey, int iNr, bool fAsTemplate)
{
	String sKey;
	if (!fAsTemplate)
	{
		String sN("view%d", iNr);
		sKey = String("%S\\%S",  sDocKey, sN);
	}
	else
		sKey = sDocKey;

	IlwisSettings catsettings(sKey);
	CFrameWnd *frm = GetParentFrame();
	if (!fAsTemplate)
	{
		WINDOWPLACEMENT wpl;
		frm->GetWindowPlacement(&wpl);
		catsettings.SetWindowPlacement("WindowPosition", wpl);
	}		
	LONG dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	catsettings.SetValue("ListStyle", dwStyle);

	CListCtrl& lvCtrl = GetListCtrl();
	CHeaderCtrl *hdr = lvCtrl.GetHeaderCtrl( );
	if ( hdr != NULL )
	{
		int n = hdr->GetItemCount();
		catsettings.SetValue("NrOfColumns", n);
		catsettings.SetValue("Query", sCatalogQuery);
		catsettings.SetValue("CurrentStandardQuery", sCurrentQuery);
		catsettings.SetValue("SortColumn", iSortColumn);
		catsettings.SetValue("Ascending", fSortAscending);
		catsettings.SetValue("Grid", fShowGrid);
		fil.SaveSettings(catsettings);

		int *ar = new int[n];
		hdr->GetOrderArray(ar, n);
		for (int i=0; i< n; ++i)
		{
			CRect rct;
			hdr->GetItemRect( i, rct);
			LVColumnInfo ci = farColumns[ar[i]];
			catsettings.SetValue(String("Column%d",i), String("%d,%d,%d", ar[i], ci.iWidth, ci.fVisible));
		}
		catsettings.SetValue("ExternalFileExt", sExternalExtensions);
		delete [] ar;
	}
}

void Catalog::LoadViewSettings(const String sDocKey, int iViewNr, bool fAsTemplate)
{
	String sKey;
	
	if (!fAsTemplate)
	{
		String sN("view%d", iViewNr);
		sKey = String("%S\\%S",  sDocKey, sN);
	}
	else
		sKey = sDocKey;
	
	IlwisSettings catsettings(sKey);
	int iN = catsettings.iValue("NrOfColumns");
	if ( iN <= 0 || iN > Catalog::ctALL ) return;
	sCatalogQuery = catsettings.sValue("Query", "");
	sCurrentQuery = catsettings.sValue("CurrentStandardQuery", "");

	iSortColumn = catsettings.iValue("SortColumn", 0);
	fSortAscending = catsettings.fValue("Ascending");
	fShowGrid = catsettings.fValue("Grid", true);
	fil.LoadSettings(catsettings);

	DWORD iVal = catsettings.iValue("ListStyle", LVS_LIST);
	LONG dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= ~LVS_TYPEMASK;
	dwStyle |= iVal;
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	CFrameWnd *frm = GetParentFrame();	

	CListCtrl& lvCtrl = GetListCtrl();
	CHeaderCtrl *hdr = lvCtrl.GetHeaderCtrl();
	int *ar = new int[iN];
	for (int i=0; i< iN; ++i)
	{
		String sKey = String("Column%d",i);
		String sV = catsettings.sValue(sKey);
		Array<String> arVals;
		Split(sV, arVals, ",");
		ar[i] = arVals[0].iVal();
		farColumns[ar[i]].iWidth = arVals[1].iVal();
		farColumns[ar[i]].fVisible = arVals[2].fVal();
	}
	hdr->SetOrderArray(iN, ar);
	for (int i=0; i< iN; ++i)
	{
		CRect rct;
		hdr->GetItemRect( i, rct);
		HDITEM  item;
		item.mask = HDI_WIDTH;
		item.cxy =  farColumns[i].fVisible ? farColumns[i].iWidth : 0;
		hdr->SetItem(i, &item);
	}
	sExternalExtensions = catsettings.sValue("ExternalFileExt", "");

	QueryFileNames();
	SortColumn(iSortColumn);
	
	SetGridOnOf(fShowGrid);
	//m_HeaderCtrl.SetSortImage(iSortColumn, fSortAscending);

	if (!fAsTemplate)
	{
		WINDOWPLACEMENT wpl;
		wpl.length = sizeof(WINDOWPLACEMENT);
		wpl.showCmd = SW_NORMAL;
		wpl.flags = 0;
		wpl.ptMaxPosition = CPoint(-1,-1);
		wpl.ptMinPosition = CPoint(-1,-1);
		wpl.rcNormalPosition = CRect(0,0,300,300);
		wpl = catsettings.GetWindowPlacement("WindowPosition", wpl);
		// complicated solution, setwindowplacement forces a redraw in the initialupdate. this
		// interferes with normal redrawing behiour causing unruly drawing behaviour
		SendMessage(WM_SETREDRAW, FALSE, 0);
		frm->SetWindowPlacement(&wpl);
		SendMessage(WM_SETREDRAW, TRUE, 0);

	}

	delete [] ar;
}

void Catalog::OnHeaderClicked(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMHEADER *hdr = (NMHEADER *) pNMHDR;

	if (iSortColumn == hdr->iItem)
		fSortAscending = !fSortAscending;
	else
		fSortAscending = false; // name is the inverse of what you would expect, so default should be false	
	SortColumn(hdr->iItem);
	//m_HeaderCtrl.SetSortImage(hdr->iItem, fSortAscending);
}

void Catalog::SetVisibleColumns(const vector<bool>& fVisibleColumns) 
{
	HDITEM item;
	item.mask = HDI_WIDTH;
	item.cxy = 0;
	CListCtrl& lvCtrl = GetListCtrl();
	lvCtrl.SendMessage(WM_SETREDRAW, FALSE, 0);
	for ( unsigned int i=0; i< farColumns.size(); ++i)
	{
		item.mask = HDI_WIDTH;
		if ( fVisibleColumns[i] )
		{
			farColumns[i].fVisible = true;
			item.cxy = farColumns[i].iWidth;
		}
		else
		{
			farColumns[i].fVisible = false;
			item.cxy = 0;

		}
		GetListCtrl().GetHeaderCtrl()->SetItem(i, &item);
	}
	lvCtrl.SendMessage(WM_SETREDRAW, TRUE, 0);

}

void Catalog::OnHeaderSize(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMHEADER* hdr = (NMHEADER*)pNMHDR;

	if (hdr->pitem->mask & HDI_WIDTH )
	{
		if ( farColumns[hdr->iItem].fVisible )
		{
			farColumns[hdr->iItem].iWidth = hdr->pitem->cxy;
			if ( hdr->pitem->cxy < 5 )
				hdr->pitem->cxy = 5;
		}
		else 
			hdr->pitem->cxy = 0;
	}
}

void Catalog::SetGridOnOf(bool fYesNo)
{
	fShowGrid = fYesNo;
	CListCtrl& lvCtrl = GetListCtrl();
	if ( fShowGrid )
		lvCtrl.SetExtendedStyle(lvCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES);
	else
	{
		DWORD wrd = lvCtrl.GetExtendedStyle();
		wrd &= ~LVS_EX_GRIDLINES;
		lvCtrl.SetExtendedStyle(wrd);
	}
}

bool Catalog::fColVisible(int iCol)
{
	if ( iCol >= farColumns.size() ) return false;
	return farColumns[iCol].fVisible ;
}


void Catalog::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pcd = (NMLVCUSTOMDRAW*) pNMHDR;
	CListCtrl& lvCtrl = GetListCtrl();
	switch (pcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		case CDDS_ITEMPREPAINT:
			*pResult = CDRF_NOTIFYPOSTPAINT;
			{
				CRect rct = pcd->nmcd.rc;
				rct.left += 3;
				pcd->nmcd.rc = rct;
			}				
			return;
		case CDDS_ITEMPOSTPAINT:
		{
			*pResult = CDRF_DODEFAULT;
			int iNr = pcd->nmcd.dwItemSpec;
			if ( viIndex.size() <= iNr)
				return;
			const FileName& fn = vfnDisplay[viIndex[iNr]];
			const FileName& fnActual = vfn[viIndex[iNr]];
			if (fn.sExt == "drive" || fn.sExt == "directory")
				return;
			if (ObjectInfo::fSystemObject(fn)) 
			{
				CDC cdc;
				cdc.Attach(pcd->nmcd.hdc);
				CPoint pt;
				int hti = lvCtrl.GetItemPosition(iNr, &pt);

				pt.x += 6;
				pt.y += 4;

				IlwWinApp()->ilSmall.Draw(&cdc, iImgOvlSystem, pt, ILD_TRANSPARENT); 
				cdc.Detach();
			}
			// For internal objects the real filename needs to be checked for UseAs
			if ( ObjectInfo::fUseAs(fn ) || ObjectInfo::fUseAs(fnActual))
			{
				CDC cdc;
				cdc.Attach(pcd->nmcd.hdc);
				CPoint pt;
				int hti = lvCtrl.GetItemPosition(iNr, &pt);

				pt.x += 6;
				pt.y += 4;

				IlwWinApp()->ilSmall.Draw(&cdc, iImgOvlUseAs, pt, ILD_TRANSPARENT); 
				cdc.Detach();
			}
			return;
		}
	}
}

void Catalog::IncludeSystemObjects( bool fYesNo)
{
	fil.IncludeSystemObjects(fYesNo);
}

void Catalog::ShowContainerContents(bool fYesNo)
{
	fil.ShowContainerContents(fYesNo);
}

bool Catalog::fShowContainerContents()
{
	return fil.fShowContainerContents();
}


bool Catalog::fIncludeSystemObjects()
{
	return fil.fIncludeSystemObjects();
}

void Catalog::OnCopy()
{
	POSITION pos = pSTARTPOS;
	FileName fn;
	String sFiles;
	while ( (fn = GetNextSelectedFile(pos)) != FileName())
	{
		if ("drive" != fn.sExt && "directory" != fn.sExt) 
			sFiles += fn.sFullPathQuoted() + "|";
	}
	
	zClipboard clip(this);
	clip.clear();
	clip.add(sFiles.sVal(), iFmtCopy);
}

void Catalog::GetFromClipboard(vector<FileName>& arFiles)
{
	int iFmtCopy = RegisterClipboardFormat("IlwisCopy");
	if (IsClipboardFormatAvailable(iFmtCopy))
	{
		zClipboard clip(AfxGetMainWnd());
		char *sText = clip.getText(iFmtCopy);
		if (0 == sText)
			return;
		String sFiles = sText;
		delete [] sText;
		String sFile;
		while( sFiles != "" && (sFile = sFiles.sHead("|")) != "" )
		{
			FileName fnFrom(sFile);
			arFiles.push_back(fnFrom);
			sFiles = sFiles.sTail("|");
		}
	}
}

void Catalog::OnPaste()
{
	vector<FileName> afn;
	GetFromClipboard(afn);
	if (afn.size() == 0)
		return;

	String sFiles;
	CatalogDocument *doc = GetDocument();
	FileName fnTo(doc->sGetPathName());
	for(vector<FileName>::iterator cur = afn.begin(); cur != afn.end(); ++cur)
	{
		String sPath = (*cur).sPath().toLower();
		sFiles += (*cur).sFullPathQuoted() + " ";
	}
	
	if ( sFiles == "")
		return;

	String sCmd;
	if (fnTo.sExt != "")
		sCmd = String("copy %S %S", sFiles, fnTo.sFullPathQuoted());
	else
		sCmd = String("copy %S %S", sFiles, Directory(fnTo).sFullPathQuoted());
	
	char* strc = sCmd.sVal();
	CWaitCursor cwr;		
	IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)strc);
}

void Catalog::OnUpdatePaste(CCmdUI* pCmdUI)
{
	bool fPossible = IsClipboardFormatAvailable(iFmtCopy)?true:false;
	pCmdUI->Enable(fPossible);
}

void Catalog::OnUpdateEdit(CCmdUI* pCmdUI)
{
	CListCtrl& lc = GetListCtrl();
	int iNr = lc.GetSelectedCount();
	if ( iNr > 0 )
	{
		POSITION pos = lc.GetFirstSelectedItemPosition();	
		int id = lc.GetNextSelectedItem(pos);
		FileName fn = vfn[viIndex[id]];
		if ( fn.sExt == "drive" || fn.sExt == "directory")
			pCmdUI->Enable(FALSE);
		else
		{
//			if ( pCmdUI->m_nID == ID_CAT_DEL && GetFileAttributes(fn.sFullPath().c_str()) & FILE_ATTRIBUTE_READONLY)
//				pCmdUI->Enable(FALSE);
//			else
				pCmdUI->Enable();
		}
	}
	else
		pCmdUI->Enable(FALSE);
}

void Catalog::OnShow()
{
	CListCtrl& lc = GetListCtrl();
	int iNr = lc.GetSelectedCount();
	if ( iNr <= 0 ) return;

	POSITION pos = lc.GetFirstSelectedItemPosition();
	if (pos == NULL)	return;
	int id = lc.GetNextSelectedItem(pos);

	vector<FileName> vsfn(iNr);
	for (int i = 0; i < iNr && id >=0; ++i) 
	{
		vsfn[i] = vfn[viIndex[id]];
		id = lc.GetNextSelectedItem(pos);
	}
	String sFile;
	for (int i = 0; i < iNr; ++i) 
	{
		FileName &fn = vsfn[i];
		if ("drive" == fn.sExt || "directory" == fn.sExt) 
		{
				String sCmd("opendir %S", fn.sFile);
				char* str = sCmd.sVal();
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			}
		else 
		{
			String sCmd("open %S", fn.sFullPathQuoted());
			char* str = sCmd.sVal();
			IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
		}
	}
}

void Catalog::OnOpen()
{
		String sCmd("open");
		char* str = sCmd.sVal();
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
}

void Catalog::DeleteItems()
{
	CListCtrl& lc = GetListCtrl();
	int iNr = lc.GetSelectedCount();
	if ( iNr <= 0 ) return;


	POSITION pos = lc.GetFirstSelectedItemPosition();
	if (pos == NULL)	return;
	int id = lc.GetNextSelectedItem(pos);

	vector<FileName> vsfn(iNr);
	for (int i = 0; i < iNr && id >=0; ++i) 
	{
		vsfn[i] = vfn[viIndex[id]];
		id = lc.GetNextSelectedItem(pos);
	}
	String sFiles;
	for (int i = 0; i < iNr; ++i) 
	{
		FileName &fn = vsfn[i];
		if ("drive" != fn.sExt && "directory" != fn.sExt) 
		{
			sFiles += fn.sFullPathQuoted() + " ";
		}
	}
	CWaitCursor cr;
	Tranquilizer trq;
	CommandHandler::DeleteObjects(this, sFiles, &trq);
}

void Catalog::OnDelete()
{
	DeleteItems(); //virtual function any descendent may delete in his own way
}

FileName Catalog::GetNextSelectedFile(POSITION& pos)
{
	CListCtrl& lc = GetListCtrl();
	if ( pos == pSTARTPOS)
	{
		int iNr = lc.GetSelectedCount();
		if (0 == iNr)
			return FileName();
		pos = lc.GetFirstSelectedItemPosition();
	}
	int id = lc.GetNextSelectedItem(pos);
	if (id >= 0) 
		return vfn[viIndex[id]];

	return FileName();
}

void Catalog::OnOpenPixelInfo()
{
	CListCtrl& lc = GetListCtrl();
	POSITION pos = lc.GetFirstSelectedItemPosition();
	FileName fn;
	String sFilesToOpen;
	while (pos) 
	{
		int id = lc.GetNextSelectedItem(pos);
		if (id >= 0) 
		{
			id = viIndex[id];
			fn = vfn[id];
			if ( fn.sExt == ".csy" || fn.sExt == ".mpr" || fn.sExt == ".mpp" ||
				 fn.sExt == ".mpa" || fn.sExt == ".mps")
			{
				sFilesToOpen += String((sFilesToOpen.length() > 0) ? " ":"") + fn.sRelativeQuoted(); // Bas Retsios: 15 February 2001, changed to Quoted to preserve long pathnames. sFullPathQuoted() would be an option, but for the time being it is useless because (it seems) the PixelInfo List control can only remember items from one catalog at a time
			}
		}
	}
	IlwWinApp()->Execute(String("pixelinfo %S", sFilesToOpen));
}

void Catalog::OnEditObject()
{
	POSITION pos = pSTARTPOS;
	FileName fn;
	while ( (fn = GetNextSelectedFile(pos)) != FileName())
	{
		if (!(GetFileAttributes(fn.sFullPath().c_str()) & FILE_ATTRIBUTE_READONLY) 
			&& !ObjectInfo::fSystemObject(fn)
			&& !ObjectInfo::fVirtual(fn))
			IlwWinApp()->Execute(String("edit %S", fn.sFullPathQuoted()));
		else
		{
			if (MessageBox(String(TR("Can not edit : %S ").c_str(), fn.sFile).c_str(), "", MB_ICONWARNING | MB_OKCANCEL | MB_TOPMOST) == IDCANCEL)
				break;
		}
	}
}

// This function is called for menu item: "Edit Object"
// Check if at least one object is editable:
// Editable= Ilwis object that is not R/O, no system object, no dependent object 
void Catalog::OnUpdateEditObject(CCmdUI* pCmdUI)
{
	POSITION pos = pSTARTPOS;
	FileName fn;
	DWORD dw;
	while ( (fn = GetNextSelectedFile(pos)) != FileName())
	{
		dw = GetFileAttributes(fn.sFullPath().c_str());
		if ( IlwisObject::iotObjectType( fn) != IlwisObject::iotANY &&   // it is an ILWIS object
			((GetFileAttributes(fn.sFullPath().c_str()) & FILE_ATTRIBUTE_READONLY) != FILE_ATTRIBUTE_READONLY) && // it is writable
			!ObjectInfo::fSystemObject(fn) &&  // it is not a system object
			!ObjectInfo::fVirtual(fn) )       // it is not a dependent object
		{
			pCmdUI->Enable(TRUE);
			return;
		}
		
	}
	pCmdUI->Enable(FALSE);
}

void Catalog::OnCreateObjectCollection() {
	FileName fn;
	POSITION pos = pSTARTPOS;
	String files;
	while ( (fn = GetNextSelectedFile(pos)) != FileName())
	{
		if ( files != "")
			files += ",";
		files += fn.sFile + fn.sExt;
	}

	IlwWinApp()->Execute(String("create ioc %S", files));

}

void Catalog::OnCopyTo()
{
	POSITION pos = pSTARTPOS;
	FileName fn;
	String sTo;
	bool fOnlyDirectory = false;
	list<FileName> lfnFiles;
	while ( (fn = GetNextSelectedFile(pos)) != FileName())
	{
		lfnFiles.push_back(fn);
	}
	if (lfnFiles.size() < 1 ) return;
	String sFiles;
	fn = (*(lfnFiles.begin())).sFullPathQuoted();	
	if ( lfnFiles.size() > 1 )
	{
		CopyObjectForm frm(this, lfnFiles, sTo, true);
		if ( frm.fOkClicked() )
			for(list<FileName>::iterator cur = lfnFiles.begin(); cur != lfnFiles.end(); ++cur)
				sFiles &= (*cur).sFullPathQuoted() + " ";
	}		
	else
	{
		CopyObjectForm frm(this, lfnFiles, sTo, false);
		if ( frm.fOkClicked() )		
			sFiles = fn.sFullPathQuoted();
	}		

	if (sFiles != "")
		IlwWinApp()->Execute(String("copy %S %S", sFiles, sTo));
}

void Catalog::OnProp()
{
	POSITION pos = pSTARTPOS;
	FileName fn;
	while ( (fn = GetNextSelectedFile(pos)) != FileName())
	{
			IlwWinApp()->Execute(String("prop %S", fn.sFullPathQuoted()));
	}
}

// This function is called for menu items: Copy and Properties
// Check if at least for one object the properties can be displayed 
// or the copy can be executed
void Catalog::OnPropertiesUpdateUI(CCmdUI* pCmdUI)
{
	CListCtrl& lc = GetListCtrl();
	int iNr = lc.GetSelectedCount();
	if (iNr > 0)
	{
		POSITION pos = pSTARTPOS;
		FileName fn;
		while ( (fn = GetNextSelectedFile(pos)) != FileName())
		{
			if ( IlwisObject::iotObjectType( fn) != IlwisObject::iotANY )
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
}

BOOL Catalog::PreTranslateMessage(MSG* pMsg)
{
  return CListView::PreTranslateMessage(pMsg);
}

String Catalog::sGetColumnName(int iCol)
{
	CListCtrl& lc = GetListCtrl();
	CHeaderCtrl *hdr = lc.GetHeaderCtrl();
	HDITEM hdi;
	TCHAR  lpBuffer[256];
	lpBuffer[0] = 0;

	hdi.mask = HDI_TEXT;
	hdi.pszText = lpBuffer;
	hdi.cchTextMax = 256;

	if (hdr->GetItem(iCol, &hdi))
		return String(lpBuffer);
	else
		return String();
}

class CopyObjectQuestionForm : public FormBaseDialog
{
	public:
		enum chChoice{ chYES, chYESTOALL, chNO, chCANCEL};

		CopyObjectQuestionForm(CWnd *par, const String& sTitle, const String& sQuestion,  chChoice &iC) :
			FormBaseDialog(par, sTitle, 
				fbsSHOWALWAYS|fbsMODAL|fbsNOCANCELBUTTON|fbsNOOKBUTTON | fbsBUTTONSUNDER|fbsNOBEVEL),
				iChoice(iC)
		{
			HICON hic = IlwWinApp()->LoadStandardIcon(IDI_QUESTION);
			StaticIcon *ic = new StaticIcon(root, hic, true);
			FieldGroup *fg = new FieldGroup(root);
			new StaticText(fg, sQuestion.sHead("\n"));
			new StaticText(fg, sQuestion.sTail("\n"));
			new FieldBlank(fg);
			PushButton *pb1 = new PushButton(root, TR("Yes"), (NotifyProc)&CopyObjectQuestionForm::Yes); 
			pb1->SetIndependentPos();
			PushButton *pb2 = new PushButton(root, TR("Yes to All"), (NotifyProc)&CopyObjectQuestionForm::YesToAll); 
			pb2->Align(pb1, AL_AFTER);
			pb2->SetIndependentPos();
			pb1 = new PushButton(root, TR("No"), (NotifyProc)&CopyObjectQuestionForm::No); 
			pb1->Align(pb2, AL_AFTER);
			pb1->SetIndependentPos();
			pb2 = new PushButton(root, TR("Cancel"), (NotifyProc)&CopyObjectQuestionForm::Cancel);
			pb2->Align(pb1, AL_AFTER);
			fg->Align(ic, AL_AFTER);
			create();
		}
		int Yes(Event *)
		{
			iChoice = chYES;
			FormBaseDialog::OnOK();
			return 1;
		}
		int No(Event *)
		{
			iChoice = chNO;
			FormBaseDialog::OnOK();
			return 1;
		}
		int YesToAll(Event  *)
		{
			iChoice = chYESTOALL;
			FormBaseDialog::OnOK();
			return 1;
		}
		int Cancel(Event *)
		{
			iChoice = chCANCEL;
			FormBaseDialog::OnOK();
			return 1;
		}
		int exec()
		{	return 1;}
	private:
		chChoice& iChoice;
};

void Catalog::CopyObjects(vector<FileName>& arFiles, const FileName& fnWhere)
{
	Directory dirDest(fnWhere);
	ObjectCopierUI coM_PIr(this, arFiles, dirDest);
	coM_PIr.Copy();
}

FileFilter& Catalog::GetFilter()
{
	return fil;
}

void Catalog::OnCatViewOptions()
{
	CatalogPropertySheet sheet(this);
	SimpleQueryPropPage page1(sheet, fil.GetNameExt());
	VisibleColumnsPropPage page2(this);
	sheet.AddPage(&page1);
	sheet.AddPage(&page2);
	MainWindow* mw = dynamic_cast<MainWindow*>(GetTopLevelFrame());
	QueryPropPage page3(sheet, mw->mapQueries());
	sheet.AddPage(&page3);
	int iRet = sheet.DoModal();
	if (iRet == IDOK)
	{
		Menu menu(mw);
		vector<String> defaults, owner;
		map<String, String> &queries = mw->mapQueries();

		int iSz = queries.size();
		int iStartOwnerQueries = ID_START_CATALOGQUERIES;
		String sTop = String("%s,%s", sMen(ID_VIEW) ,sMen(ID_CATALOG_QUERIES));		
		for(map<String, String>::iterator cur = queries.begin(); cur!=queries.end(); ++cur)
		{
			if ( (*cur).first.sHead(":") == "DEF")
			{
				defaults.push_back((*cur).first);
				iStartOwnerQueries++;
			}
			else
			{
				owner.push_back((*cur).first);
				String sItem = sTop  + "," + (*cur).first.c_str();
				menu.DeleteMenuItem(sItem);
			}
		}
		for(vector<String>::iterator cur2 = owner.begin(); cur2 != owner.end(); ++cur2)
		{
			String sItem = *cur2;
			sItem = sTop + "," + sItem;
			menu.Add(sItem, iStartOwnerQueries++);
		}
	}
}

class KeepCatalogSelection
{
public:
	KeepCatalogSelection(Catalog* catalog)
	: cat(catalog)
	{
		CListCtrl& lvCtrl = cat->GetListCtrl();
		if (lvCtrl.GetItemCount() == 0) {
			iCurInd = -1;
			return;
		}
		int iCurSel = lvCtrl.GetNextItem(-1, LVNI_FOCUSED);
		if (iCurSel < 0)
			return;
		iCurInd = cat->viIndex[iCurSel];
		if (iCurInd >= 0)
			fnCur = cat->vfn[iCurInd];
	}
	~KeepCatalogSelection()
	{
		if (iCurInd >= 0) {
			CListCtrl& lvCtrl = cat->GetListCtrl();
			iCurInd = find(cat->vfn.begin(), cat->vfn.end(), fnCur) - cat->vfn.begin();
			if (iCurInd >= 0) {
				int iRes = find(cat->viIndex.begin(), cat->viIndex.end(), iCurInd) - cat->viIndex.begin();
				int iCurSel = lvCtrl.GetNextItem(-1, LVNI_FOCUSED);
				lvCtrl.SetItemState(iCurSel, 0, 0x000F);
				if (iRes >= 0) {
					lvCtrl.SetItemState(iRes, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
					lvCtrl.EnsureVisible(iRes, FALSE);
				}
			}
		}
	}
private:
	Catalog* cat;
	int iCurInd;
	FileName fnCur;
};

void Catalog::OnSelBut(UINT id)
{
	KeepCatalogSelection kcs(this);
	vector<NameExt>& arr = fil.GetNameExt();
	if ( id < ID_MWD_STARTSELBUT + IlwisObject::iotANY + 2)
		arr[id - ID_MWD_STARTSELBUT ].fShow = ! arr[id - ID_MWD_STARTSELBUT ].fShow;
	if ( id == ID_MWD_SELECTALL )
	{
		fShowAll = !fShowAll;
		for(vector<NameExt>::iterator cur = arr.begin(); cur != arr.end(); ++cur)
			(*cur).fShow = fShowAll;
	}
	FillCatalog();
}

void Catalog::OnSelButUI(CCmdUI* pCmdUI)
{
	vector<NameExt>& arr = fil.GetNameExt();
	if ( pCmdUI->m_nID < ID_MWD_STARTSELBUT + IlwisObject::iotANY + 2)
	{
		int iIndex = pCmdUI->m_nID - ID_MWD_STARTSELBUT;
		pCmdUI->SetCheck(arr[iIndex].fShow);
	}
}

void Catalog::OnSortCol(UINT id)
{
	if (iSortColumn == id - ID_CAT_SORTCOL_START)
		fSortAscending = !fSortAscending;
	else
		fSortAscending = false; // name is the inverse of what you would expect, so default should be false	
	if (id - ID_CAT_SORTCOL_START == Catalog::ctNONE)  // Manual Sort, automatically assigned ID (1053)
		SortColumn(-1);
	else
		SortColumn(id - ID_CAT_SORTCOL_START);
}

void Catalog::OnSortColUI(CCmdUI* pCmdUI)
{
	int id = pCmdUI->m_nID;
	bool fActiveSortOption = (iSortedColumn() == id - ID_CAT_SORTCOL_START);
	if (id == ID_CAT_SORTCOL_START + Catalog::ctNONE)
	{
		pCmdUI->Enable(fIsManualSortAvail());
		fActiveSortOption = iSortedColumn() == -1;
	}
	
	pCmdUI->SetRadio(fActiveSortOption);
}

void Catalog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CListCtrl& lvCtrl = GetListCtrl();
	int iCurSel = lvCtrl.GetNextItem(-1, LVNI_FOCUSED);
	switch (nChar) {
		case VK_RETURN: 
			Execute(iCurSel);
			return;
	}
	CListView::OnKeyDown(nChar, nRepCnt, nFlags);
	iCurSel = lvCtrl.GetNextItem(-1, LVNI_FOCUSED);
	ShowStatusLineInfo(iCurSel);
}

void Catalog::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (isalnum(nChar)) 
	{
		CListCtrl& lvCtrl = GetListCtrl();
		int iCurSel = lvCtrl.GetNextItem(-1, LVNI_FOCUSED);
		nChar = tolower(nChar);
		int i = iCurSel + 1;
		while (i != iCurSel) 
		{
			if (i >= viIndex.size()) 
			{
				if (iCurSel < 1)
					break;
				i = 0;
			}
			FileName& fn = vfnDisplay[viIndex[i]];
			if (tolower(fn.sFile[0]) == nChar) {
				lvCtrl.SetItemState(iCurSel, 0, 0x000F);
				lvCtrl.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
				lvCtrl.EnsureVisible(i, FALSE);
				ShowStatusLineInfo(i);
				break;
			}
			i++;
		}
	}
	else	
		CListView::OnChar(nChar, nRepCnt, nFlags);
}

String Catalog::sGetExternalFileExtensions()
{
	return sExternalExtensions;
}

void Catalog::SetExternalFileExtensions(const String& sExt)
{
	sExternalExtensions = sExt;
}

void Catalog::OnSelectAll()
{
	LV_ITEM lvi;

	lvi.mask		= LVIF_STATE;
	lvi.iItem		= -1;
	lvi.iSubItem	= 0;
	lvi.state		= LVIS_SELECTED;
	lvi.stateMask	= LVIS_SELECTED;

	int iNoSelect=0;
	for(vector<FileName>::iterator cur = vfn.begin(); cur != vfn.end(); ++cur)
		if ( (*cur).sExt == "directory" || (*cur).sExt == "drive" )
			++iNoSelect;
	for(int i=0; i< vfn.size() - iNoSelect; ++i)
		GetListCtrl().SetItemState(i, &lvi);
}

void Catalog::OnUpdateSelectAll(CCmdUI* pCmdUI)
{
	CatalogFrameWindow * wnd = dynamic_cast<CatalogFrameWindow *>(GetParentFrame());
	CMDIChildWnd *mdic = wnd->GetMDIFrame()->MDIGetActive();
	if ( !mdic )
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void Catalog::OnShowMapTables()
{
	KeepCatalogSelection kcs(this);
	vector<NameExt>& arr = fil.GetNameExt();
	for(vector<NameExt>::iterator cur = arr.begin(); cur != arr.end(); ++cur)
		(*cur).fShow = false;
	for (int i = IlwisObject::iotRASMAP; i <= IlwisObject::iotTABLE; ++i)
		arr[i].fShow = true;
	FillCatalog();
}

void Catalog::OnShowAllData()
{
	KeepCatalogSelection kcs(this);
	vector<NameExt>& arr = fil.GetNameExt();
	for(vector<NameExt>::iterator cur = arr.begin(); cur != arr.end(); ++cur)
		(*cur).fShow = false;
	for (int i = IlwisObject::iotRASMAP; i <= IlwisObject::iotANNOTATIONTEXT; ++i)
		arr[i].fShow = true;
	FillCatalog();
}

void Catalog::OnShowAllObjects()
{
	KeepCatalogSelection kcs(this);
	vector<NameExt>& arr = fil.GetNameExt();
	for(vector<NameExt>::iterator cur = arr.begin(); cur != arr.end(); ++cur)
		(*cur).fShow = true;
	FillCatalog();
}

void Catalog::OnShowBaseMaps() {
	useBasemaps = !useBasemaps;
	FillCatalog();

}

void Catalog::OnUpdateCatalogQueries(CCmdUI* pCmdUI)
{
	MainWindow* mw = dynamic_cast<MainWindow*>(GetTopLevelFrame());
	if ( mw == 0 ) return;
	map<String, String> mpQueries = mw->mapQueries();
	map<String, String>::iterator where = mpQueries.begin();
	int id=0;
	for(; id< mpQueries.size(); ++id)	
	{
		if ( (*where).first == sCurrentQuery )
			break;
		++where;
	}
	if ( where != mpQueries.end())
	{
		if( pCmdUI->m_nID == ID_START_CATALOGQUERIES + id)
		{
			pCmdUI->SetRadio(TRUE);
			return;
		}
	}
	pCmdUI->SetRadio(FALSE)	;
}

void Catalog::OnCatalogQueries(UINT iID)
{
	String sName;
	CString str;
	MainWindow* mw = dynamic_cast<MainWindow*>(GetTopLevelFrame());
	if ( mw == 0 ) return;		
	CMenu* men = mw->GetMenu();
	men->GetMenuString(iID, str, MF_BYCOMMAND);

	if (iID < mw->iBeginOwnerQueries() )
		sName = String("DEF:%s", str);
	else
	{
		if ( iID >= mw->iBeginOwnerQueries() && iID < ID_END_CATALOGQUERIES)
			sName = str;
	}
	
	map<String, String> mpQueries = mw->mapQueries();
	sCurrentQuery = sName;
	String sQueryCommand = mpQueries[sName];
	String sCom("dir %S", sQueryCommand);
	IlwWinApp()->Execute(sCom);
}

void Catalog::SetCurrentQuery(const String& str)
{
	sCurrentQuery = str;
}

long Catalog::iGetObjectSize(const FileName& fn, const char* sSection, const char *sKey)
{
	CFileStatus status;
	status.m_size = 0;
	CFile::GetStatus( fn.sFullPath().c_str(), status);
	if ( sSection != 0 )
	{
		FileName fnData;
		ObjectInfo::ReadElement(sSection, sKey, fn, fnData);
		if ( fnData != FileName() && fnData.fExist() )
			CFile::GetStatus( fnData.sFullPath().c_str(), status);
	}
	return status.m_size;
}

long Catalog::iObjectSize(const FileName& fn)
{
	CFileStatus status;
	double  rVersion = rUNDEF;

	long int iSizeObj = iGetObjectSize(fn);
	
	switch( IlwisObject::iotObjectType( fn ) )
	{
		case IlwisObject::iotRASMAP:
			iSizeObj += iGetObjectSize(fn, "MapStore", "Data"); break;
		case IlwisObject::iotTABLE:
			iSizeObj += iGetObjectSize(fn, "TableStore", "Data"); break;
		case IlwisObject::iotPOINTMAP:
			iSizeObj += iGetObjectSize(fn, "TableStore", "Data"); break;
		case IlwisObject::iotSEGMENTMAP:
			ObjectInfo::ReadElement("Ilwis", "Version", fn, rVersion);
			if ( rVersion >= 3.0)
			{
				iSizeObj += iGetObjectSize(fn, "TableStore", "Data"); 
				iSizeObj += iGetObjectSize(fn, "ForeignFormat", "Filename");
			}	
			else
			{
				iSizeObj += iGetObjectSize(fn, "SegmentMapStore", "DataSeg");
				iSizeObj += iGetObjectSize(fn, "SegmentMapStore", "DataSegCode");
				iSizeObj += iGetObjectSize(fn, "SegmentMapStore", "DataCrd");
			}
			break;
		case IlwisObject::iotPOLYGONMAP:
			ObjectInfo::ReadElement("Ilwis", "Version", fn, rVersion);
			if ( rVersion >= 3.0)
			{
				iSizeObj += iGetObjectSize(fn, "top:TableStore", "Data");
				iSizeObj += iGetObjectSize(fn, "TableStore", "Data");
				iSizeObj += iGetObjectSize(fn, "ForeignFormat", "Filename");
			}
			else
			{
				iSizeObj += iGetObjectSize(fn, "SegmentMapStore", "DataSeg");
				iSizeObj += iGetObjectSize(fn, "SegmentMapStore", "DataCrd");
				iSizeObj += iGetObjectSize(fn, "PolygonMapStore", "DataPol");
				iSizeObj += iGetObjectSize(fn, "PolygonMapStore", "DataPolCode");
				iSizeObj += iGetObjectSize(fn, "PolygonMapStore", "DataTop");
			}
			break;
		case IlwisObject::iotDOMAIN:
				iSizeObj += iGetObjectSize(fn, "TableStore", "data"); break; // only true for domainssort
		case IlwisObject::iotREPRESENTATION:
				iSizeObj += iGetObjectSize(fn, "TableStore", "data"); break; // only true for rprclass
		case IlwisObject::iotGEOREF:
				iSizeObj += iGetObjectSize(fn, "TableStore", "data"); break; 
		case IlwisObject::iotCOORDSYSTEM:
				iSizeObj += iGetObjectSize(fn, "TableStore", "data"); break; 
		case IlwisObject::iotHISTRAS:
		case IlwisObject::iotHISTPOL:
		case IlwisObject::iotHISTSEG:
				iSizeObj += iGetObjectSize(fn, "TableStore", "data"); break; 
		case IlwisObject::iotSCRIPT:
				iSizeObj += iGetObjectSize(fn, "Script", "ScriptFile"); break;
		case IlwisObject::iotFUNCTION:
				iSizeObj += iGetObjectSize(fn, "FuncUser", "FuncDeffile"); break;
	}	
	return iSizeObj;
}

void Catalog::OnPyramids()
{
	CListCtrl& lc = GetListCtrl();
	int iNr = lc.GetSelectedCount();
	String sCommand = "CreatePyramidLayers ";
	if ( iNr > 0 ) // only selected rasters
	{

		POSITION pos = pSTARTPOS;
		FileName fn;
		while ( (fn = GetNextSelectedFile(pos)) != FileName())
		{
			if ( IlwisObject::iotObjectType( fn) == IlwisObject::iotRASMAP ||
				   IlwisObject::iotObjectType( fn) == IlwisObject::iotMAPLIST )
			{
				sCommand += fn.sRelativeQuoted() + " ";
			}
		}
	}
	else // do all rasters, maplists
	{
		vector<FileName> vfn, vfnDisp;
		CatalogDocument *doc = GetDocument();
		doc->GetFileNames(vfn, vfnDisp);
		for(vector<FileName>::iterator iter = vfn.begin(); iter != vfn.end(); ++iter)
		{
			FileName fn(*iter);
			if ( IlwisObject::iotObjectType( fn) == IlwisObject::iotRASMAP )
			{
				sCommand += fn.sRelativeQuoted() + " ";
			}
		}
	}
	if ( sCommand != "CreatePyramidLayers ") // no empty lists
		IlwWinApp()->Execute(sCommand);
}

void Catalog::OnUpdatePyramids(CCmdUI* pCmdUI)
{
	CListCtrl& lc = GetListCtrl();
	int iNr = lc.GetSelectedCount();
	if ( iNr > 0 )
	{
		POSITION pos = pSTARTPOS;
		FileName fn;
		while ( (fn = GetNextSelectedFile(pos)) != FileName())
		{
			if ( IlwisObject::iotObjectType( fn) == IlwisObject::iotRASMAP ||
				IlwisObject::iotObjectType( fn) == IlwisObject::iotMAPLIST )
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
		pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(TRUE);
}
