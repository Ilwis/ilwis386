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
// LayoutWindow.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\Editors\Layout\LayoutWindow.h"
#include "Client\Editors\Layout\LayoutListView.h"
#include "Headers\Hs\Layout.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LayoutWindow

IMPLEMENT_DYNCREATE(LayoutWindow, DataWindow)

BEGIN_MESSAGE_MAP(LayoutWindow, DataWindow)
	//{{AFX_MSG_MAP(LayoutWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_EXIT, OnClose)
	ON_COMMAND(ID_TOOLBAR_STANDARD, OnStandardBar)
	ON_COMMAND(ID_TOOLBAR_LAYOUT, OnLayoutBar)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_STANDARD, OnUpdateStandardBar)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_LAYOUT, OnUpdateLayoutBar)
	ON_COMMAND(ID_LAYOUT_MANAGEMENT, OnLayerManagement)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_MANAGEMENT, OnUpdateLayerManagement)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



LayoutWindow::LayoutWindow()
{
  htpTopic = htpLayoutWindow;
	sHelpKeywords = "Layout editor";
}

LayoutWindow::~LayoutWindow()
{
}

#define sMen(ID) ILWSF("men",ID).scVal()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int LayoutWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	EnableDocking(CBRS_ALIGN_ANY);

  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
	add(ID_FILE_NEW);
	add(ID_FILE_OPEN);
	add(ID_FILE_SAVE);
	add(ID_FILE_SAVE_AS);
  addBreak;
	add(ID_FILE_PAGE_SETUP);
	add(ID_FILE_PRINT);
	add(ID_MAKE_BITMAP);
	add(ID_EXIT);
  addMenu(ID_MEN_FILE);

  menPopup.CreateMenu();
	add(ID_EDIT_CUT);
	add(ID_EDIT_COPY);
	add(ID_EDIT_PASTE);
	add(ID_EDIT_CLEAR);
  addBreak;
	add(ID_ITEM_EDIT);
	add(ID_ENTIREMAP);
  addBreak;
	add(ID_PAGEBORDER);
	addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
	add(ID_ADD_MAPVIEW);
	add(ID_ADD_TEXT);
	add(ID_ADD_LEGEND);
	add(ID_ADD_SCALEBAR);
	add(ID_ADD_SCALETEXT);
	add(ID_ADD_NORTHARROW);
	add(ID_ADD_MAPBORDER);
	add(ID_ADD_PICTURE);
	add(ID_ADD_BOX);
	addMenu(ID_MEN_INSERT);

  menPopup.CreateMenu();
	add(ID_ENTIRE_PAGE);
	add(ID_NORMAL);
  add(ID_ZOOMIN);
  add(ID_ZOOMOUT);
	add(ID_PANAREA);
  add(ID_REDRAW);
  addBreak;
	add(ID_KEEPSCALE);
  add(ID_TOOLBAR_STANDARD);
	add(ID_TOOLBAR_LAYOUT);
  add(ID_LAYOUT_MANAGEMENT);
  add(ID_STATUSLINE);
  addMenu(ID_MEN_VIEW);
  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	AddHelpMenu();
	SetAcceleratorTable();

	bbStandard.Create(this, "ilostd.but", SLOTitleStandardBar, 100);
	DockControlBar(&bbStandard, AFX_IDW_DOCKBAR_TOP);

	bbLayout.Create(this, "layout.but", SLOTitleLayoutBar, 101);
	DockControlBar(&bbLayout, AFX_IDW_DOCKBAR_TOP);

	llb.Create(this);
	CView* pFirstView = 0;
	if (0 == pFirstView) 
		pFirstView = dynamic_cast<CView*>(GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE));
	pFirstView->GetDocument()->AddView(llb.llv);
	DockControlBar(&llb,AFX_IDW_DOCKBAR_TOP);

	unsigned int ind[4] = { 0, 0, 0, 0 };
	status->SetIndicators(ind, 4);
	status->SetPaneInfo(0, 0, SBPS_STRETCH|SBPS_NOBORDERS, 100);
	CClientDC dcScreen(NULL);
	HFONT hFont = (HFONT)status->SendMessage(WM_GETFONT);
	HGDIOBJ hOldFont = NULL;
	if (hFont != NULL)
		hOldFont = dcScreen.SelectObject(hFont);
	int iWidth = dcScreen.GetTextExtent("x = 9999.9 mm").cx;
	status->SetPaneInfo(1, 0, SBPS_NORMAL, iWidth);
	status->SetPaneInfo(2, 0, SBPS_NORMAL, iWidth);
	iWidth = dcScreen.GetTextExtent("9999.9 x 9999.9 mm").cx;
	status->SetPaneInfo(3, 0, SBPS_NORMAL, iWidth);
	if (hOldFont != NULL)
		dcScreen.SelectObject(hOldFont);

	return 0;
}

void LayoutWindow::OnStandardBar()
{
	if (bbStandard.IsWindowVisible())
		ShowControlBar(&bbStandard,FALSE,FALSE);
	else
		ShowControlBar(&bbStandard,TRUE,FALSE);
}

void LayoutWindow::OnUpdateStandardBar(CCmdUI* pCmdUI)
{
	bool fCheck = bbStandard.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void LayoutWindow::OnLayoutBar()
{
	if (bbLayout.IsWindowVisible())
		ShowControlBar(&bbLayout,FALSE,FALSE);
	else
		ShowControlBar(&bbLayout,TRUE,FALSE);
}

void LayoutWindow::OnUpdateLayoutBar(CCmdUI* pCmdUI)
{
	bool fCheck = bbLayout.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void LayoutWindow::OnLayerManagement()
{
	if (llb.IsWindowVisible())
		ShowControlBar(&llb,FALSE,FALSE);
	else
		ShowControlBar(&llb,TRUE,FALSE);
}

void LayoutWindow::OnUpdateLayerManagement(CCmdUI* pCmdUI)
{
	bool fCheck = llb.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

