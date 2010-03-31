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
// HistogramWindow.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\TableWindow\TableCommandHandler.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\Editors\Utils\GeneralBar.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\TableWindow\HistogramGraphView.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Client\TableWindow\TableSummaryPaneView.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Base\datawind.h"
#include "Client\TableWindow\RecordBar.h"
#include "Client\TableWindow\TableWindow.h"
#include "Client\TableWindow\HistogramWindow.h"
#include "Client\TableWindow\GraphPaneView.h"
#include "Client\MainWindow\AddInfoView.h"
#include "Client\TableWindow\HistogramDoc.h"
#include "Client\TableWindow\GraphLayerTreeView.h"
#include "Headers\Hs\Table.hs"
#include "Headers\Hs\Graph.hs"
#include "Headers\Htp\Ilwis.htp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HistogramWindow

IMPLEMENT_DYNCREATE(HistogramWindow, TableWindow)

BEGIN_MESSAGE_MAP(HistogramWindow, TableWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_GRPHMAN, OnGraphManagement)
	ON_UPDATE_COMMAND_UI(ID_GRPHMAN, OnUpdateGraphManagement)
END_MESSAGE_MAP()

#define ID_GOTORECORDFIRST ID_GOTORECORD0
#define ID_GOTORECORDLAST ID_GOTORECORD1
#define ID_GOTORECORDPREV ID_GOTORECORD2
#define ID_GOTORECORDNEXT ID_GOTORECORD3


HistogramWindow::HistogramWindow()
{
	htpTopic = htpHistogramObject;
	sHelpKeywords = "Histogram";
}

HistogramWindow::~HistogramWindow()
{
}

#define sMen(ID) ILWSF("men",ID).scVal()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int HistogramWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
  HistogramDoc* hd = dynamic_cast<HistogramDoc*>(hgv->CView::GetDocument());
  if (0 == hd)
    return -1;
  
  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
  add(ID_FILE_PRINT);
  addBreak;
  add(ID_FILE_PROP);
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
    menSub.CreateMenu();
		addSub(ID_GRPH_ADD_COLUMN);
		addSub(ID_GRPH_ADD_FORMULA);
		addSub(ID_GRPH_ADD_LSF);
		addSub(ID_GRPH_ADD_SVM);
  addSubMenu(ID_GRPH_ADD);
  addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
  add(ID_COLUMNS);
  add(ID_ADDCOLUMN);
  add(ID_GOTOCOLUMN);
  add(ID_SORT);
  add(ID_UPDATEALLCOLS);
  addMenu(ID_MEN_COLUMNS);

  menPopup.CreateMenu();
  add(ID_GOTORECORD);
  add(ID_GOTORECORDFIRST);
  add(ID_GOTORECORDPREV);
  add(ID_GOTORECORDNEXT);
  add(ID_GOTORECORDLAST);
  addMenu(ID_MEN_RECORDS);

  menPopup.CreateMenu();
  add(ID_SHOWRECORDVIEW);
  addBreak;
  add(ID_COMMANDLINE);
  add(ID_GRPHMAN);
  add(ID_STATPANE);
  add(ID_TBLBUTTONBAR);
  add(ID_STATUSLINE);
  addMenu(ID_MEN_VIEW);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	AddHelpMenu();
	SetAcceleratorTable();

	EnableDocking(CBRS_ALIGN_ANY);

	bBar.Create(this, "histogram.but", "", ID_TBLBUTTONBAR);
	DockControlBar(&bBar, AFX_IDW_DOCKBAR_TOP);

	commBar.Create(this, ID_COMMANDLINE, CommandCombo::cbTable);
	DockControlBar(&commBar);

  hd->RemoveView(hgv);
  hd->hgd.AddView(hgv);
	gbGraphTree.Create(this, 125, CSize(200,200));
  gbGraphTree.SetWindowText(SGPTitleGraphManagement.scVal());
	gbGraphTree.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
  gbGraphTree.view = new GraphLayerTreeView;
	gbGraphTree.view->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW|TVS_HASLINES,
			CRect(0,0,0,0), &gbGraphTree, 100, 0);
  hd->hgd.AddView(gbGraphTree.view);
	DockControlBar(&gbGraphTree, AFX_IDW_DOCKBAR_LEFT);
	ShowControlBar(&gbGraphTree,FALSE,FALSE);

  return 0;
}

BOOL HistogramWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
 	wndHor1.CreateStatic(this, 2, 1, WS_CHILD|WS_VISIBLE);
  wndVer1.CreateStatic(&wndHor1, 1, 2, WS_CHILD|WS_VISIBLE|WS_BORDER, wndHor1.IdFromRowCol(0, 0));
  wndVer1.CreateView(0,0,RUNTIME_CLASS(HistogramGraphView),CSize(400,400), pContext);
	wndSplitter.CreateStatic(&wndVer1, 2, 1, WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL, wndVer1.IdFromRowCol(0, 1));
	wndSplitter.CreateView(0, 0, pContext->m_pNewViewClass, CSize(400, 500), pContext);
	wndSplitter.CreateView(1, 0, RUNTIME_CLASS(TableSummaryPaneView), CSize(400,30), pContext);
  wndHor1.CreateView(1,0,RUNTIME_CLASS(AddInfoView),CSize(400,30), pContext);
	iPrevStatHeight = 30;
  tpv = (TablePaneView*)wndSplitter.GetPane(0,0);
  hgv = (HistogramGraphView*)wndVer1.GetPane(0,0);
	SetActiveView(tpv);
	return TRUE;
}

void HistogramWindow::InitSettings()
{
  TableWindow::InitSettings();

  CView* vwAddInfo = (CView*)wndSplitter.GetPane(1,0);
	IlwisDocument* doc = dynamic_cast<IlwisDocument*>(vwAddInfo->GetDocument());
  int iRowHeight = tpv->iHeight();
  int iIdealAddInfoHeight;
  if (doc->obj()->fAdditionalInfo())
    iIdealAddInfoHeight = 7 * iRowHeight;
  else
    iIdealAddInfoHeight = 0;
  wndSplitter.iIdealStatPaneHeight = iIdealAddInfoHeight;
  int iCurrH1, iCurrH2, iDummy;
  wndHor1.GetRowInfo(0, iCurrH1, iDummy);
  wndHor1.GetRowInfo(1, iCurrH2, iDummy);
  int iHeight = iCurrH1 + iCurrH2 - iIdealAddInfoHeight;
  if (iHeight < iIdealAddInfoHeight)
    iHeight = iIdealAddInfoHeight;
  wndHor1.SetRowInfo(0, iHeight, 0);
  wndHor1.SetRowInfo(1, iIdealAddInfoHeight, 0);
  wndHor1.RecalcLayout();    
}

void HistogramWindow::OnGraphManagement()
{
	if (gbGraphTree.IsWindowVisible())
		ShowControlBar(&gbGraphTree,FALSE,FALSE);
	else
		ShowControlBar(&gbGraphTree,TRUE,FALSE);
}

void HistogramWindow::OnUpdateGraphManagement(CCmdUI* pCmdUI)
{
	bool fCheck = gbGraphTree.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

