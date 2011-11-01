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
// SmceMapWindow.cpp: implementation of the SmceMapWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Client\Editors\SMCE\SmceMapWindow.h"
#include "Client\Mapwindow\MapStatusBar.h"
#include "Headers\Hs\Mapwind.hs"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\OverviewMapPaneView.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"

#include "Client\FormElements\flddom.h"
#include "Client\FormElements\fentdms.h"
//#include "ApplicationsUI\frmpolap.h"
//#include "ApplicationsUI\frmmapap.h"

#include "Client\ilwis.h" // for IlwWinApp

IMPLEMENT_DYNCREATE(SmceMapWindow, MapWindow)

BEGIN_MESSAGE_MAP(SmceMapWindow, MapWindow)
	//{{AFX_MSG_MAP(MapWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_CT_SEGMENT_TO_POLYGON, OnSegmentToPolygon)
	ON_COMMAND(ID_CT_POLYGON_TO_RASTER, OnPolygonToRaster)
//	ON_UPDATE_COMMAND_UI(ID_CT_SEGMENT_TO_POLYGON, OnUpdateSegmentToPolygon)
//	ON_UPDATE_COMMAND_UI(ID_CT_POLYGON_TO_RASTER, OnUpdatePolygonToRaster)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define sMen(ID) ILWSF("men",ID).c_str()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

void SmceMapWindow::SetGeoRef(GeoRef& gr)
{
	m_grf = gr;
}

afx_msg int SmceMapWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	MapStatusBar* msb = new MapStatusBar;
	status = msb;
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	msb->Create(this);

	EnableDocking(CBRS_ALIGN_ANY);

	CMenu men;
	CMenu menPopup;
	CMenu menSub;
	men.CreateMenu();

	menPopup.CreateMenu();
//  add(ID_FILE_OPEN);
		menSub.CreateMenu();
		addSub(ID_CREATESEGMAP);
		addSub(ID_CREATEPNTMAP);
		addSub(ID_CT_SEGMENT_TO_POLYGON);
		addSub(ID_CT_POLYGON_TO_RASTER);
//			addSub(ID_CREATERASMAP);
//			addSub(ID_CREATESMS);
//			addSub(ID_CREATEGRF);
//			addSub(ID_CREATECSY);
//			addSub(ID_CREATEANNTEXT);
//			addSub(ID_CREATESUBMAP)

	addSubMenu(ID_FILE_CREATE);
	add(ID_SAVEVIEW);
	add(ID_SAVEVIEWAS);
	add(ID_CREATE_LAYOUT);
	addBreak;
	menPropLayer.CreateMenu();
	menPopup.AppendMenu(MF_POPUP, (UINT)menPropLayer.GetSafeHmenu(), sMen(ID_PROPLAYER)); 	
//  add(ID_OPENGENMAP);
	//add(ID_OPENPIXELINFO);
	addBreak;
	add(ID_EXIT);
	hMenFile = menPopup.GetSafeHmenu();
	addMenu(ID_MEN_FILE);

	menPopup.CreateMenu();
	add(ID_COPY );
	add(ID_PASTE);
	addBreak;
	menEditLayer.CreateMenu();
	menPopup.AppendMenu(MF_POPUP, (UINT)menEditLayer.GetSafeHmenu(), sMen(ID_EDITLAYER)); 	
//			menSub.CreateMenu();
//			addSub(ID_PIXELEDIT);
//			addSub(ID_POINTEDIT);
//			addSub(ID_SEGEDIT);
//			addSub(ID_POLEDIT);
//		addSubMenu(ID_EDITOR);
//		addBreak;
	menRprLayer.CreateMenu();
//		menPopup.AppendMenu(MF_POPUP, (UINT)menRprLayer.GetSafeHmenu(), sMen(ID_RPRLAYER)); 	
	menDomLayer.CreateMenu();
// 		menPopup.AppendMenu(MF_POPUP, (UINT)menDomLayer.GetSafeHmenu(), sMen(ID_DOMLAYER)); 	
//		add(ID_EDITGRF);
//		add(ID_EDITCSY);
	hMenEdit = menPopup.GetSafeHmenu();
	addMenu(ID_MEN_EDIT);

	menPopup.CreateMenu();
	add(ID_ADDLAYER);
//		add(ID_ADD_GRID);
//		add(ID_ADD_GRATICULE);
	add(ID_REMOVELAYER);
	menDataLayer.CreateMenu();
	menPopup.AppendMenu(MF_POPUP, (UINT)menDataLayer.GetSafeHmenu(), sMen(ID_LAYEROPTIONS)); 	
	addBreak;
	add(ID_BGCOLOR);
//			menSub.CreateMenu();
//			addSub(ID_MAPDBLCLKRECORD);
//			addSub(ID_MAPDBLCLKRPR);
//			addSub(ID_MAPDBLCLKACTION);
//		addSubMenu(ID_MAPDBLCLK);

	addMenu(ID_MEN_LAYERS);

	menPopup.CreateMenu();
	add(ID_ENTIREMAP);
	add(ID_NORMAL);
	add(ID_ZOOMIN);
	add(ID_ZOOMOUT);
	add(ID_PANAREA);
	add(ID_SCALE1);
	addBreak;
	add(ID_REDRAW);
	add(ID_MEASUREDIST);
	add(ID_SHOWHISTOGRAM);
	addBreak;
		menSub.CreateMenu();
		addSub(ID_EXTPERC);
		addSub(ID_EXTCOORD);
	addSubMenu(ID_EXTEND);
	add(ID_COORDSYSTEM);
	addBreak;
	add(ID_ADJUSTSIZE);
	add(ID_LAYERMANAGE);
	add(ID_OVERVIEW);
	add(ID_SCALECONTROL);
	add(ID_SHOWRECORDVIEW);
	add(ID_BUTTONBAR);
	add(ID_STATUSLINE);
	addMenu(ID_MEN_OPTIONS);

	menPopup.CreateMenu();
	add(ID_HLPKEY);
	add(ID_HELP_RELATED_TOPICS);
	addBreak
	add(ID_HLPCONTENTS);
	add(ID_HLPINDEX);
	add(ID_HLPSEARCH);
	addBreak
	add(ID_HELP_ILWIS_OBJECTS);
	addBreak
	add(ID_ABOUT)
	addMenu(ID_MEN_HELP);

	SetMenu(&men);
	menPopup.Detach();
	men.Detach();

	SetAcceleratorTable();

	bbDataWindow.Create(this, "map.but", "", 100);
	// prevent docking right/left because e.g. Tiepointeditors enhance buttonbar
	bbDataWindow.EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);

	DockControlBar(&bbDataWindow, AFX_IDW_DOCKBAR_TOP);

	barScale.Create(this);
	RecalcLayout();
	CRect rect;
	bbDataWindow.GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	DockControlBar(&barScale,AFX_IDW_DOCKBAR_TOP,rect);

	ltb.Create(this, 124, CSize(150,200));
	ltb.SetWindowText(TR("Layer Management").c_str());
	ltb.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	ltb.view = new LayerTreeView;
	ltb.view->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
			CRect(0,0,0,0), &ltb, 100, 0);

	// half of the requested height will be assigned
	gbOverview.Create(this, 125, CSize(150,300));
	gbOverview.m_szFloat = CSize(150,150);
	gbOverview.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	gbOverview.view = new OverviewMapPaneView;
	gbOverview.view->Create(NULL,NULL,AFX_WS_DEFAULT_VIEW,CRect(0,0,0,0),&gbOverview,100.0);
	gbOverview.SetWindowText(TR("Overview window").c_str());

	if (0 == pFirstView) 
		pFirstView = dynamic_cast<CView*>(GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE));
	pFirstView->GetDocument()->AddView(ltb.view);
	pFirstView->GetDocument()->AddView(gbOverview.view);

	DockControlBar(&ltb,AFX_IDW_DOCKBAR_LEFT);
	RecalcLayout();
	ltb.GetWindowRect(&rect);
	rect.OffsetRect(0,1);
	DockControlBar(&gbOverview,AFX_IDW_DOCKBAR_LEFT,rect);

	return 0;
}

void SmceMapWindow::OnSegmentToPolygon()
{
	String sMostRecentSegmentMap (fnGetMostRecentMap(".mps").sRelativeQuoted());
	if (sMostRecentSegmentMap.length() > 0)
		sMostRecentSegmentMap = " " + sMostRecentSegmentMap;
	String sMostRecentPointMap (fnGetMostRecentMap(".mpp").sRelativeQuoted());
	if (sMostRecentPointMap.length() > 0)
		sMostRecentPointMap = " " + sMostRecentPointMap;
	String sParms("%S%S", sMostRecentSegmentMap, sMostRecentPointMap);

	//new FormPolygonMapFromSegment(this, sParms.c_str());
	IlwWinApp()->ExecuteUI(sParms, this);
}

void SmceMapWindow::OnPolygonToRaster()
{
	String sMostRecentPolygonMap (fnGetMostRecentMap(".mpa").sRelativeQuoted());
	if (sMostRecentPolygonMap.length() > 0)
		sMostRecentPolygonMap = " " + sMostRecentPolygonMap;
	String sGeoRef;
	if (m_grf.fValid())
		sGeoRef = " " + m_grf->fnObj.sRelativeQuoted();
	String sParms("%S%S", sMostRecentPolygonMap, sGeoRef);

//	new FormRasPolygon(this, sParms.c_str());
	IlwWinApp()->ExecuteUI(sParms, this);
}

FileName SmceMapWindow::fnGetMostRecentMap(String sExt)
{
	String sDir = IlwWinApp()->sGetCurDir();
	FileName fnRet;
	FILETIME timeRet;
	bool fFound = false;
	CFileFind finder;
	String strPattern(sDir + "\\*" + sExt);
	BOOL bWorking = finder.FindFile(strPattern.c_str());
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		FILETIME time;
		finder.GetLastWriteTime(&time);
		FileName fn (finder.GetFilePath());
		if ((fn.sExt == sExt) && (shUNDEF == fn.sFile.iPos(String("_contours")))) // prevent ".mpa#" or other false positives
		{
			if (!fFound)
			{
				fnRet = fn;
				timeRet = time;
				fFound = true;
			}
			else
			{
				if ((timeRet.dwHighDateTime < time.dwHighDateTime) || ((timeRet.dwHighDateTime == time.dwHighDateTime) && (timeRet.dwLowDateTime < time.dwLowDateTime)))
				{
					fnRet = fn;
					timeRet = time;
				}
			}
		}
	}

	finder.Close();
		
	return fnRet;
}
