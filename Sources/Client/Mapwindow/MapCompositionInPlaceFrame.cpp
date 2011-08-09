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
// MapCompositionInPlaceFrame.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\Mapwindow\MapCompositionInPlaceFrame.h"
#include "Client\Mapwindow\LayerTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MapCompositionInPlaceFrame

IMPLEMENT_DYNCREATE(MapCompositionInPlaceFrame, COleIPFrameWnd)

BEGIN_MESSAGE_MAP(MapCompositionInPlaceFrame, COleIPFrameWnd)
	//{{AFX_MSG_MAP(MapCompositionInPlaceFrame)
	ON_WM_CREATE()
	ON_WM_INITMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MapCompositionInPlaceFrame::MapCompositionInPlaceFrame()
{
}

MapCompositionInPlaceFrame::~MapCompositionInPlaceFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// MapCompositionInPlaceFrame message handlers

#define sMen(ID) ILWSF("men",ID).c_str()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();


int MapCompositionInPlaceFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleIPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// CResizeBar implements in-place resizing.
	if (!m_wndResizeBar.Create(this))
	{
		TRACE0("Failed to create resize bar\n");
		return -1;      // fail to create
	}

	// By default, it is a good idea to register a drop-target that does
	//  nothing with your frame window.  This prevents drops from
	//  "falling through" to a container that supports drag-drop.
	m_dropTarget.Register(this);

	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
  add(ID_COPY );
	add(ID_COPYSCALEBARLINK)
  addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
	add(ID_ADDLAYER);
	add(ID_ADD_GRID);
	add(ID_ADD_GRATICULE);
	menDataLayer.CreateMenu();
  menPopup.AppendMenu(MF_POPUP, (UINT)menDataLayer.GetSafeHmenu(), sMen(ID_LAYEROPTIONS)); 	
	addBreak;
  add(ID_BGCOLOR);
  addMenu(ID_MEN_LAYERS);

	men.AppendMenu(MF_SEPARATOR); 
  menPopup.CreateMenu();
  add(ID_ENTIREMAP);
  add(ID_ZOOMIN);
  add(ID_ZOOMOUT);
  add(ID_SCALE1);
  addBreak;
  add(ID_REDRAW);
  add(ID_MEASUREDIST);
  addBreak;
    menSub.CreateMenu();
    addSub(ID_EXTPERC);
    addSub(ID_EXTCOORD);
  addSubMenu(ID_EXTEND);
  add(ID_COORDSYSTEM);
  addBreak;
  add(ID_LAYERMANAGE);
  add(ID_BUTTONBAR);
  addMenu(ID_MEN_OPTIONS);

	men.AppendMenu(MF_SEPARATOR);
  menPopup.CreateMenu();
  add(ID_HLPKEY);
  add(ID_HLPCONTENTS);
  add(ID_HLPINDEX);
  add(ID_HLPSEARCH);
	addBreak
	add(ID_ABOUT)
  addMenu(ID_MEN_HELP);

	return 0;
}

BOOL MapCompositionInPlaceFrame::OnCreateControlBars(CWnd* pWndFrame, CWnd* pWndDoc)
{
	CFrameWnd* fw = dynamic_cast<CFrameWnd*>(pWndFrame);
	if (0 == fw)
		return FALSE;

	fw->EnableDocking(CBRS_ALIGN_ANY);
	bbMapWindow.Create(fw, "map.but", "", 100);
	bbMapWindow.SetOwner(this);
	fw->DockControlBar(&bbMapWindow, AFX_IDW_DOCKBAR_TOP);

	barScale.Create(fw);
	barScale.SetOwner(this);
	fw->DockControlBar(&bbMapWindow, AFX_IDW_DOCKBAR_TOP);

	ltb.Create(fw);
	ltb.SetOwner(this);
	fw->DockControlBar(&ltb,AFX_IDW_DOCKBAR_LEFT);
	CView*pFirstView = dynamic_cast<CView*>(GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE));
	pFirstView->GetDocument()->AddView(ltb.ltv);
	ltb.ltv->OnInitialUpdate();

	return TRUE;
}

HMENU MapCompositionInPlaceFrame::GetInPlaceMenu()
{
	return men.GetSafeHmenu();
}

void MapCompositionInPlaceFrame::OnInitMenu(CMenu* pMenu) 
{
  MapPaneView* mpv = dynamic_cast<MapPaneView*>(GetActiveView());
	if (0 == mpv) 
		return;
  MapCompositionDoc* mcd = mpv->GetDocument();  
	if (0 == mcd) 
		return;
  mcd->menLayers(menDataLayer, ID_LAYFIRST);
//mcd->menLayers(menEditLayer, ID_EDITLAYER);
//mcd->menLayers(menPropLayer, ID_PROPLAYER);
//mcd->menLayers(menRprLayer, ID_RPRLAYER);
//mcd->menLayers(menDomLayer, ID_DOMLAYER);
  DrawMenuBar();
}


