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
// GraphWindow.cpp: implementation of the GraphWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\TableWindow\GraphWindow.h"
#include "Client\TableWindow\GraphLayerTreeView.h"
#include "Headers\constant.h"
#include "Headers\Hs\Graph.hs"
#include "Headers\Htp\Ilwis.htp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(GraphWindow, DataWindow)

BEGIN_MESSAGE_MAP(GraphWindow, DataWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_EXIT, OnClose)  
	ON_COMMAND(ID_GRPHMAN, OnGraphManagement)
	ON_UPDATE_COMMAND_UI(ID_GRPHMAN, OnUpdateGraphManagement)
	ON_COMMAND(ID_BUTTONBAR, OnButtonBar)
	ON_UPDATE_COMMAND_UI(ID_BUTTONBAR, OnUpdateButtonBar)
  ON_NOTIFY(TBN_DROPDOWN,100,OnButtonBarMenuDropDown)
END_MESSAGE_MAP()


GraphWindow::GraphWindow()
{
  help = "ilwis\\graph_window_functionality.htm";
	sHelpKeywords = "Graph Window";
}

GraphWindow::~GraphWindow()
{
}


#define sMen(ID) ILWSF("men",ID).scVal()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int GraphWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
  add(ID_GRPH_NEW);
	add(ID_FILE_OPEN);
	add(ID_FILE_SAVE);
	add(ID_FILE_SAVE_AS);
  addBreak;
  add(ID_FILE_PRINT);
  add(ID_GRPH_OPENTABLE)
	addBreak;
  add(ID_EXIT);
  addMenu(ID_MEN_FILE);

  menPopup.CreateMenu();
    menSub.CreateMenu();
		addSub(ID_GRPH_ADD_COLUMN);
		addSub(ID_GRPH_ADD_FORMULA);
		addSub(ID_GRPH_ADD_LSF);
		addSub(ID_GRPH_ADD_SVM);
  addSubMenu(ID_GRPH_ADD);
	add(ID_REMOVELAYER);
  add(ID_COPY);
  addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
  add(ID_GRPHMAN);
  add(ID_BUTTONBAR);
  add(ID_STATUSLINE);
  addMenu(ID_MEN_VIEW);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	AddHelpMenu();
	SetAcceleratorTable();

	EnableDocking(CBRS_ALIGN_ANY);

	bbGraph.Create(this, "graph.but", "", 100);
	DockControlBar(&bbGraph, AFX_IDW_DOCKBAR_TOP);
	CToolBarCtrl& tbc = bbGraph.GetToolBarCtrl();
  tbc.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	TBBUTTONINFO bi;
  bi.cbSize = sizeof(TBBUTTONINFO);
  bi.dwMask = TBIF_STYLE;
	if (tbc.GetButtonInfo(ID_GRPH_ADD, &bi)) 
  {
    bi.fsStyle |= TBSTYLE_DROPDOWN;
    tbc.SetButtonInfo(ID_GRPH_ADD, &bi);    
  }

	gbGraphTree.Create(this, 125, CSize(200,200));
  gbGraphTree.SetWindowText(SGPTitleGraphManagement.scVal());
	gbGraphTree.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
  gbGraphTree.view = new GraphLayerTreeView;
	gbGraphTree.view->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW|TVS_HASLINES,
			CRect(0,0,0,0), &gbGraphTree, 100, 0);
	CView* pFirstView = dynamic_cast<CView*>(GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE));
	pFirstView->GetDocument()->AddView(gbGraphTree.view);
	DockControlBar(&gbGraphTree, AFX_IDW_DOCKBAR_LEFT);

	return 0;
}

void GraphWindow::OnGraphManagement()
{
	if (gbGraphTree.IsWindowVisible())
		ShowControlBar(&gbGraphTree,FALSE,FALSE);
	else
		ShowControlBar(&gbGraphTree,TRUE,FALSE);
}

void GraphWindow::OnUpdateGraphManagement(CCmdUI* pCmdUI)
{
	bool fCheck = gbGraphTree.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void GraphWindow::OnButtonBar()
{
	if (bbGraph.IsWindowVisible())
		ShowControlBar(&bbGraph,FALSE,FALSE);
	else
		ShowControlBar(&bbGraph,TRUE,FALSE);
}

void GraphWindow::OnUpdateButtonBar(CCmdUI* pCmdUI)
{
	bool fCheck = bbGraph.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void GraphWindow::OnButtonBarMenuDropDown(NMHDR* nmhdr, LRESULT* result)
{
  NMTOOLBAR* nmtb = (LPNMTOOLBAR)nmhdr;
  if (ID_GRPH_ADD != nmtb->iItem)
    return;
	CToolBarCtrl& tbc = bbGraph.GetToolBarCtrl();
  CRect rect;
  tbc.GetRect(ID_GRPH_ADD, &rect);
	bbGraph.ClientToScreen(&rect);
	CMenu menPopup;
	menPopup.CreatePopupMenu();
	add(ID_GRPH_ADD_COLUMN);
	menPopup.SetDefaultItem(ID_GRPH_ADD_COLUMN);
	add(ID_GRPH_ADD_FORMULA);
	add(ID_GRPH_ADD_LSF);
	add(ID_GRPH_ADD_SVM);
  menPopup.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, rect.left, rect.bottom, this);
  *result = 0;
}
