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
// Table2Window.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\Base\ButtonBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "engine\base\system\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine\Table\tbl2dim.h"
#include "Client\TableWindow\Table2Doc.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\TableWindow\Table2Window.h"
#include "Client\TableWindow\Table2PaneView.h"
#include "Headers\Hs\Table.hs"
#include "Client\FormElements\fldval.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(Table2Window, DataWindow)


BEGIN_MESSAGE_MAP(Table2Window, DataWindow)
	//{{AFX_MSG_MAP(Table2Window)
	ON_WM_CREATE()
	ON_COMMAND(ID_EXIT, OnClose)  
  ON_UPDATE_COMMAND_UI(ID_TBL2BUTTONBAR, OnUpdateButtonBar)
  ON_COMMAND(ID_TBL2BUTTONBAR, OnButtonBar)
//	ON_MESSAGE(ILWM_EXECUTE, OnExecute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Table2Window

Table2Window::Table2Window()
{
	htpTopic = htp2DimTableObject;
	sHelpKeywords = "Two-dimensional tables";
}

Table2Window::~Table2Window()
{
}



/////////////////////////////////////////////////////////////////////////////
// Table2Window message handlers

#define sMen(ID) ILWSF("men",ID).scVal()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int Table2Window::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_ANY);

/*	BOOL fRet = commBar.Create(this, ID_COMMANDLINE, CommandCombo::cbTable);
	ISTRUE(fINotEqual, fRet, FALSE);
	DockControlBar(&commBar);*/

  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
  add(ID_FILE_PRINT);
  addBreak;
  add(ID_PROP);
	addBreak;
  add(ID_EXIT);
  addMenu(ID_MEN_FILE);

  menPopup.CreateMenu();
  add(ID_COPY );
  add(ID_PASTE);
  add(ID_CLEAR);
  add(ID_EDIT);
  addBreak;
  add(ID_SELALL);
  addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
  add(ID_TBL2BUTTONBAR);
  add(ID_STATUSLINE);
  addMenu(ID_MEN_VIEW);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	AddHelpMenu();
	SetAcceleratorTable();
	bBar.Create(this, "table2.but", "", ID_TBL2BUTTONBAR);
	DockControlBar(&bBar, AFX_IDW_DOCKBAR_TOP);
	return 0;
}

BOOL Table2Window::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return DataWindow::OnCreateClient(lpcs, pContext);
}

void Table2Window::InitSettings()
{
  DataWindow::InitSettings();
  Table2PaneView* tp = (Table2PaneView*)GetActiveView();
  int iRowHeight = tp->iHeight()+1;
	int iCharWidth = tp->iChrWidth() + 1;
  CDC dc;
  dc.CreateCompatibleDC(0);
  tp->InitColPix(&dc);

  CRect r;
  GetWindowRect(&r);
  CRect rc;
  GetClientRect(&rc);

	Table2Doc* td = (Table2Doc*)GetActiveDocument();
  int iButtonWidth = max(4, td->table2()->ds1()->iWidth());

  int iWidth = iCharWidth * (iButtonWidth - 2 + td->table2()->iMatrixCols() * max(td->table2()->dvrsVal().iWidth(), td->table2()->ds2()->iWidth()));
  int iHeight = iRowHeight * (td->table2()->iMatrixRows() + 5);
  int iMaxWidth = GetSystemMetrics(SM_CXSCREEN) * 2 / 3;
  int iMaxHeight = GetSystemMetrics(SM_CYSCREEN) / 2;
	iWidth = min(iWidth, iMaxWidth);
	iHeight = min(iHeight, iMaxHeight);
  SetWindowPos(0, r.left, r.top, r.Width() - rc.Width() + iWidth, r.Height() - rc.Height() + iHeight, SWP_SHOWWINDOW);  
	
}

void Table2Window::SaveSettings()
{
	DataWindow::SaveSettings();
}

void Table2Window::OnButtonBar()
{
	if (bBar.IsWindowVisible())
		ShowControlBar(&bBar,FALSE,FALSE);
	else
		ShowControlBar(&bBar,TRUE,FALSE);
}

void Table2Window::OnUpdateButtonBar(CCmdUI* pCmdUI)
{
	bool fCheck = bBar.IsVisible() != 0;
  pCmdUI->SetCheck(fCheck);
}
