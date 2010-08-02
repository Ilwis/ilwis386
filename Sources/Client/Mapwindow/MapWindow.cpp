/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\Mapwindow\MapStatusBar.h"
#include "Client\Mapwindow\OverviewMapPaneView.h"
#include "Headers\Hs\Mapwind.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(MapWindow, DataWindow)

BEGIN_MESSAGE_MAP(MapWindow, DataWindow)
	//{{AFX_MSG_MAP(MapWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_EXIT, OnClose)
	ON_COMMAND(ID_LARGER, OnLarger)
	ON_COMMAND(ID_SMALLER, OnSmaller)
	ON_UPDATE_COMMAND_UI(ID_LARGER, OnUpdateLarger)
	ON_UPDATE_COMMAND_UI(ID_SMALLER, OnUpdateSmaller)
	ON_WM_GETMINMAXINFO()
	ON_WM_INITMENU()
	ON_MESSAGE(ILW_UPDATE, OnUpdate)
	ON_COMMAND(ID_LAYERMANAGE, OnLayerManagement)
	ON_COMMAND(ID_OVERVIEW, OnOverviewWindow)
	ON_COMMAND(ID_SCALECONTROL, OnScaleControl)
	ON_UPDATE_COMMAND_UI(ID_LAYERMANAGE, OnUpdateLayerManagement)
	ON_UPDATE_COMMAND_UI(ID_OVERVIEW, OnUpdateOverviewWindow)
	ON_UPDATE_COMMAND_UI(ID_SCALECONTROL, OnUpdateScaleControl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const int iMINSIZE = 50;

MapWindow::MapWindow()
: pFirstView(0)
{
  htpTopic = htpContentsMapWindow;
	sHelpKeywords = "Map Window";
}

MapWindow::~MapWindow()
{
}

#define sMen(ID) ILWSF("men",ID).scVal()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int MapWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
    addSub(ID_CREATEPNTMAP);
    addSub(ID_CREATESEGMAP);
    addSub(ID_CREATERASMAP);
    addSub(ID_CREATESMS);
    addSub(ID_CREATEGRF);
    addSub(ID_CREATECSY);
    addSub(ID_CREATEANNTEXT);
		addSub(ID_CREATESUBMAP)
  addSubMenu(ID_FILE_CREATE);
  add(ID_SAVEVIEW);
  add(ID_SAVEVIEWAS);
	add(ID_CREATE_LAYOUT);
  addBreak;
	menPropLayer.CreateMenu();
  menPopup.AppendMenu(MF_POPUP, (UINT)menPropLayer.GetSafeHmenu(), sMen(ID_PROPLAYER)); 	
//  add(ID_OPENGENMAP);
  add(ID_OPENPIXELINFO);
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
    menSub.CreateMenu();
    addSub(ID_PIXELEDIT);
    addSub(ID_POINTEDIT);
    addSub(ID_SEGEDIT);
    addSub(ID_POLEDIT);
  addSubMenu(ID_EDITOR);
	addBreak;
	menRprLayer.CreateMenu();
  menPopup.AppendMenu(MF_POPUP, (UINT)menRprLayer.GetSafeHmenu(), sMen(ID_RPRLAYER)); 	
	menDomLayer.CreateMenu();
  menPopup.AppendMenu(MF_POPUP, (UINT)menDomLayer.GetSafeHmenu(), sMen(ID_DOMLAYER)); 	
  add(ID_EDITGRF);
  add(ID_EDITCSY);
	hMenEdit = menPopup.GetSafeHmenu();
  addMenu(ID_MEN_EDIT);
  
  menPopup.CreateMenu();
	add(ID_ADDLAYER);
	add(ID_ADD_GRID);
	add(ID_ADD_GRATICULE);
	add(ID_REMOVELAYER);
	menDataLayer.CreateMenu();
  menPopup.AppendMenu(MF_POPUP, (UINT)menDataLayer.GetSafeHmenu(), sMen(ID_LAYEROPTIONS)); 	
	addBreak;
  add(ID_BGCOLOR);
		menSub.CreateMenu();
		addSub(ID_MAPDBLCLKRECORD);
		addSub(ID_MAPDBLCLKRPR);
		addSub(ID_MAPDBLCLKACTION);
  addSubMenu(ID_MAPDBLCLK);

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

	ltb.Create(this, 124, CSize(200,200));
  ltb.SetWindowText(SMWTitleLayerManagement.scVal());
	ltb.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
  ltb.view = new LayerTreeView;
	ltb.view->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
			CRect(0,0,0,0), &ltb, 100, 0);

  // half of the requested height will be assigned
//	gbOverview.Create(this, 125, CSize(150,300));
  //gbOverview.m_szFloat = CSize(150,150);
//	gbOverview.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
 // gbOverview.view = new OverviewMapPaneView;
  //gbOverview.view->Create(NULL,NULL,AFX_WS_DEFAULT_VIEW,CRect(0,0,0,0),&gbOverview,100.0);
 // gbOverview.SetWindowText(SMWTitleOverviewWindow.scVal());

	if (0 == pFirstView) 
		pFirstView = dynamic_cast<CView*>(GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE));
	pFirstView->GetDocument()->AddView(ltb.view);
	//pFirstView->GetDocument()->AddView(gbOverview.view);

	DockControlBar(&ltb,AFX_IDW_DOCKBAR_LEFT);
  RecalcLayout();
  ltb.GetWindowRect(&rect);
	rect.OffsetRect(0,1);
	//DockControlBar(&gbOverview,AFX_IDW_DOCKBAR_LEFT,rect);

	return 0;
}

BOOL MapWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if (!DataWindow::OnCreateClient(lpcs, pContext))
	  return FALSE;
  //wndSplitter.Create(this, 2, 2, CSize(50,50), pContext);
	return TRUE;
}

void MapWindow::OnLarger()
{
  MapPaneView* mpv = dynamic_cast<MapPaneView*>(GetActiveView());
	if (mpv)
		mpv->OnZoomIn();
	zRect rect;
	GetWindowRect(rect);
	CWindowDC wdc(this);
	int iScreenWidth = wdc.GetDeviceCaps(HORZRES);
	if (rect.width() > 0.45 * iScreenWidth)
		return;
	int iScreenHeight = wdc.GetDeviceCaps(VERTRES);
	if (rect.height() > 0.45 * iScreenHeight)
		return;
  rect.right() += rect.width();
  rect.bottom() += rect.height();
	MoveWindow(&rect);
}

void MapWindow::OnSmaller()
{
  MINMAXINFO mmi;
	OnGetMinMaxInfo(&mmi);
	zRect rect;
	GetWindowRect(rect);
	if (rect.width() < mmi.ptMinTrackSize.x)
		return;
	if (rect.height() < mmi.ptMinTrackSize.y)
		return;
  rect.right() -= rect.width() / 2;
  rect.bottom() -= rect.height() / 2;
	MoveWindow(&rect);
  MapPaneView* mpv = dynamic_cast<MapPaneView*>(GetActiveView());
	if (mpv)
		mpv->OnZoomOut();
}

void MapWindow::OnUpdateLarger(CCmdUI* pCmdUI)
{
	zRect rect;
	GetWindowRect(rect);
	CWindowDC wdc(this);
	int iScreenWidth = wdc.GetDeviceCaps(HORZRES);
	int iScreenHeight = wdc.GetDeviceCaps(VERTRES);
	BOOL fEnable = (rect.width() < 0.45 * iScreenWidth)
	            && (rect.height() < 0.45 * iScreenHeight);
	pCmdUI->Enable(fEnable);
}

void MapWindow::OnUpdateSmaller(CCmdUI* pCmdUI)
{
  MINMAXINFO mmi;
	OnGetMinMaxInfo(&mmi);
	zRect rect;
	GetWindowRect(rect);
	BOOL fEnable = (rect.width() > mmi.ptMinTrackSize.x)
		          && (rect.height() > mmi.ptMinTrackSize.y);
	pCmdUI->Enable(fEnable);
}

void MapWindow::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	DataWindow::OnGetMinMaxInfo(lpMMI);
/*
	if (IsZoomed()) { // maximized
		DataWindow::OnGetMinMaxInfo(lpMMI);
		return;
	}
	CRect rectWindow, rectClient;
	GetWindowRect(rectWindow);
	// restore window also calls GetMinmaxInfo - for Win NT
	if (rectWindow.top == -32000) {
		DataWindow::OnGetMinMaxInfo(lpMMI);
		return;
	}
	GetClientRect(rectClient);
	// restore window also calls GetMinmaxInfo - for Win95 
	if (rectClient.Height() == 0) {
		DataWindow::OnGetMinMaxInfo(lpMMI);
		return;
	}
  MapPaneView* mpv = dynamic_cast<MapPaneView*>(GetActiveView());
	if (mpv) {
  	mpv->GetClientRect(rectClient);
		mpv->GetMinMaxInfo(lpMMI);
  	lpMMI->ptMinTrackSize.x += rectWindow.Width() - rectClient.right;
	  lpMMI->ptMinTrackSize.y += rectWindow.Height() - rectClient.bottom;
  	lpMMI->ptMaxTrackSize.x += rectWindow.Width() - rectClient.right;
	  lpMMI->ptMaxTrackSize.y += rectWindow.Height() - rectClient.bottom;
  }
	else {
	 	lpMMI->ptMinTrackSize.x = rectWindow.Width() - rectClient.right;
		lpMMI->ptMinTrackSize.y = rectWindow.Height() - rectClient.bottom;
		lpMMI->ptMinTrackSize.x += iMINSIZE;
		lpMMI->ptMinTrackSize.y += iMINSIZE;
	}
*/
}

void MapWindow::OnInitMenu(CMenu* pMenu) 
{
	DataWindow::OnInitMenu(pMenu);
	
  MapPaneView* mpv = dynamic_cast<MapPaneView*>(pFirstView);
	if (0 == mpv) 
		return;
  MapCompositionDoc* mcd = mpv->GetDocument();  
	if (0 == mcd) 
		return;
  mcd->menLayers(menDataLayer, ID_LAYFIRST);
  mcd->menLayers(menEditLayer, ID_EDITLAYER);
  mcd->menLayers(menPropLayer, ID_PROPLAYER);
  mcd->menLayers(menRprLayer, ID_RPRLAYER);
  mcd->menLayers(menDomLayer, ID_DOMLAYER);
  DrawMenuBar();
}

BOOL MapWindow::PreTranslateMessage(MSG* pMsg)
{
	return DataWindow::PreTranslateMessage(pMsg);
}

void MapWindow::UpdateMenu(HMENU hmFile, HMENU hmEdit)
{
	CMenu* men = GetMenu();
	men->RemoveMenu(0, MF_BYPOSITION);
	men->RemoveMenu(0, MF_BYPOSITION);
	men->InsertMenu(0, MF_BYPOSITION|MF_POPUP, (UINT)hmFile, sMen(ID_MEN_FILE));
	men->InsertMenu(1, MF_BYPOSITION|MF_POPUP, (UINT)hmEdit, sMen(ID_MEN_EDIT));
	DrawMenuBar();
}

void MapWindow::UpdateMenu()
{
	CMenu* men = GetMenu();
	men->RemoveMenu(0, MF_BYPOSITION);
	men->RemoveMenu(0, MF_BYPOSITION);
	men->InsertMenu(0, MF_BYPOSITION|MF_POPUP, (UINT)hMenFile, sMen(ID_MEN_FILE));
	men->InsertMenu(1, MF_BYPOSITION|MF_POPUP, (UINT)hMenEdit, sMen(ID_MEN_EDIT));
	DrawMenuBar();
}

LRESULT MapWindow::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	CView* view = pFirstView;
	if (view)
		return view->SendMessage(ILW_UPDATE, wParam, lParam);
	return 0;
}

void MapWindow::OnLayerManagement()
{
	if (ltb.IsWindowVisible())
		ShowControlBar(&ltb,FALSE,FALSE);
	else
		ShowControlBar(&ltb,TRUE,FALSE);
}

void MapWindow::OnUpdateLayerManagement(CCmdUI* pCmdUI)
{
	bool fCheck = ltb.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void MapWindow::OnOverviewWindow()
{
	if (gbOverview.IsWindowVisible())
		ShowControlBar(&gbOverview,FALSE,FALSE);
	else
		ShowControlBar(&gbOverview,TRUE,FALSE);
}

void MapWindow::OnUpdateOverviewWindow(CCmdUI* pCmdUI)
{
	//bool fCheck = gbOverview.IsWindowVisible() != 0;
	//pCmdUI->SetCheck(fCheck);
}

BOOL MapWindow::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (DataWindow::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	if (0 == pFirstView) 
		pFirstView = dynamic_cast<CView*>(GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE));
	if (0 != pFirstView && pFirstView != GetActiveView())
		return pFirstView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	else
		return FALSE;
}

void MapWindow::InitSettings()
{
  DataWindow::InitSettings();

	if (!IsZoomed()) 
	{	 
		RecalcLayout();

		CRect rect, rectView;
		GetWindowRect(&rect);
	
		MapPaneView* mpv = dynamic_cast<MapPaneView*>(pFirstView);
		CSize sz, szPref = mpv->GetDocument()->szPrefSize;
		if (szPref.cx > 0 && szPref.cy > 0) {
			mpv->GetClientRect(&rectView);
			rect.right += szPref.cx - rectView.Width();
			rect.bottom += szPref.cy - rectView.Height();
			sz.cx = rect.Width();
			sz.cy = rect.Height();
		 	MoveWindow(CRect(rect.TopLeft(), sz));
		}
	}
}

void MapWindow::OnScaleControl()
{
	if (barScale.IsWindowVisible())
		ShowControlBar(&barScale,FALSE,FALSE);
	else
		ShowControlBar(&barScale,TRUE,FALSE);
}

void MapWindow::OnUpdateScaleControl(CCmdUI* pCmdUI)
{
	bool fCheck = barScale.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void MapWindow::InitialUpdate(CDocument* pDoc, BOOL bMakeVisible)
{
	CWnd* pWnd = GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
	MapPaneView* mpv = dynamic_cast<MapPaneView*>(pWnd);
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(pDoc);
	IlwisSettings settings("DefaultSettings\\DataObjectPos");
	CPoint ptTopLeft;
	CSize sz;
	ptTopLeft.x = settings.iValue("X0");
	ptTopLeft.y = settings.iValue("Y0");
	sz.cx = settings.iValue("XSize0");
	sz.cy = settings.iValue("YSize0");
	
	CSize szPref = mcd->szPrefSize;
	CRect rect;
	GetWindowRect(&rect);
	
	CWindowDC wdc(this);
	const int iScreenWidth = wdc.GetDeviceCaps(HORZRES);
	const int iScreenHeight = wdc.GetDeviceCaps(VERTRES);
	
	if (ptTopLeft.x < 0) 
		ptTopLeft.x = rect.left;
	if (ptTopLeft.y < 0) 
		ptTopLeft.y = rect.top;
	if (sz.cx < 100)
		sz.cx = szPref.cx;
	if (sz.cx < 100)
		sz.cx = 0.6 * iScreenWidth;
	if (sz.cy < 100)
		sz.cy = szPref.cy;
	if (sz.cy < 100)
		sz.cy = 0.6 * iScreenHeight;
	
	FrameWindow::InitialUpdate(pDoc, bMakeVisible);
	CRect rectClient;
	mpv->GetClientRect(&rectClient);
	sz.cx += rect.Width() - rectClient.Width();
	sz.cy += rect.Height() - rectClient.Height();
	
	CPoint ptTL(iUNDEF, iUNDEF);
	CSize  szWH(iUNDEF, iUNDEF);
	IlwWinApp()->GetNextWindowRect(ptTL, szWH);

	// if GetNextWindowRect has non-specified values (iUNDEF)
	// leave the already specified ones in place
	if (ptTL.x != iUNDEF)
		ptTopLeft.x = ptTL.x;
	if (ptTL.y != iUNDEF)
		ptTopLeft.y = ptTL.y;
	if (szWH.cx != iUNDEF)
		sz.cx = szWH.cx;
	if (szWH.cy != iUNDEF)
		sz.cy = szWH.cy;

	rect = CRect(ptTopLeft, sz);
	MoveWindow(rect);
	
	mpv->UpdateFrame();
}

void MapWindow::GetMessageString(UINT nID, CString& sMessage) const
{
	// corrections for multiple items which should get same text
	if (nID > ID_LAYFIRST && nID < ID_LAYFIRST + 900)
		nID = ID_LAYFIRST;
	if (nID > ID_DOMLAYER && nID < ID_DOMLAYER + 900)
		nID = ID_DOMLAYER;
	if (nID > ID_RPRLAYER && nID < ID_RPRLAYER + 900)
		nID = ID_RPRLAYER;
	if (nID > ID_EDITLAYER && nID < ID_EDITLAYER + 900)
		nID = ID_EDITLAYER;
	if (nID > ID_PROPLAYER && nID < ID_PROPLAYER + 900)
		nID = ID_PROPLAYER;

	FrameWindow::GetMessageString(nID, sMessage);
}

