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
// TableWindow.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Engine\Base\system\engine.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "engine\base\system\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Engine\Base\System\commandhandler.h"
#include "Client\TableWindow\TableCommandHandler.h"
#include "Client\TableWindow\TableDoc.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Utils\GeneralBar.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Client\TableWindow\TableSummaryPaneView.h"
#include "Engine\Table\Rec.h"
#include "Client\TableWindow\RecordView.h"
#include "Engine\Table\tblview.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\TableWindow\TableWindow.h"
#include "Headers\Hs\Table.hs"
#include "Client\FormElements\fldval.h"
#include "Client\Forms\SimpleCalcResultsForm.h"
#include "Client\FormElements\syscolor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define ID_GOTORECORDFIRST ID_GOTORECORD0
#define ID_GOTORECORDLAST ID_GOTORECORD1
#define ID_GOTORECORDPREV ID_GOTORECORD2
#define ID_GOTORECORDNEXT ID_GOTORECORD3

IMPLEMENT_DYNCREATE(TableWindow, DataWindow)


BEGIN_MESSAGE_MAP(TableWindow, DataWindow)
	//{{AFX_MSG_MAP(TableWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_EXIT, OnClose)  
	ON_MESSAGE(ILWM_EXECUTE, OnExecute)
	ON_MESSAGE(ILWM_CMDHANDLERUI, OnCommandUI)
	ON_COMMAND(ID_FILE_PROP, OnFileProp)
	ON_COMMAND(ID_ADDRECORD, OnAddRecord)
  ON_UPDATE_COMMAND_UI(ID_ADDRECORD, OnUpdateAddRecord)
	ON_COMMAND(ID_TBLSTATS, OnTblStats)
	ON_COMMAND(ID_AGGREGATE, OnAggregate)
	ON_COMMAND(ID_JOIN, OnJoin)
	ON_COMMAND(ID_LEASTSQUARES, OnLeastSquares)
	ON_COMMAND(ID_CUMULATIVE, OnCumulative)
	ON_COMMAND(ID_SEMIVARIOGRAM, OnSemiVariogram)
	ON_COMMAND(ID_TIMECOLUMN, OnTimeColumn)
	ON_COMMAND(ID_COLSLICING, OnSlicing)
	ON_COMMAND(ID_COMMANDLINE, OnCommandLine)
	ON_COMMAND(ID_SHOWADDINFO, OnAdditionalInfo)
	ON_COMMAND(ID_SHOWRECORDVIEW, OnShowRecordView)
	ON_COMMAND(ID_COLUMNS, OnColMan)
	ON_COMMAND(ID_CONFMATRIX, OnConfusionMatrix)
  ON_UPDATE_COMMAND_UI(ID_SHOWADDINFO, OnUpdateAdditionalInfo)
	ON_UPDATE_COMMAND_UI(ID_COMMANDLINE, OnUpdateCommandLine)
	ON_UPDATE_COMMAND_UI(ID_SHOWRECORDVIEW, OnUpdateShowRecordView)
  ON_COMMAND(ID_STATPANE, OnStatPane)
  ON_UPDATE_COMMAND_UI(ID_STATPANE, OnUpdateStatPane)
  ON_COMMAND(ID_TBLBUTTONBAR, OnButtonBar)
  ON_UPDATE_COMMAND_UI(ID_TBLBUTTONBAR, OnUpdateButtonBar)
  ON_UPDATE_COMMAND_UI(ID_GOTORECORDFIRST, OnUpdateRecFirst)
  ON_UPDATE_COMMAND_UI(ID_GOTORECORDLAST, OnUpdateRecLast)
  ON_UPDATE_COMMAND_UI(ID_GOTORECORDPREV, OnUpdateRecPrev)
  ON_UPDATE_COMMAND_UI(ID_GOTORECORDNEXT, OnUpdateRecNext)
	ON_COMMAND(ID_GOTORECORD, OnGotoRecord)
	ON_COMMAND(ID_GOTORECORDPREV, OnGotoRecordPrev)
	ON_COMMAND(ID_GOTORECORDNEXT, OnGotoRecordNext)
	ON_COMMAND(ID_GOTORECORDFIRST, OnGotoRecordFirst)
	ON_COMMAND(ID_GOTORECORDLAST, OnGotoRecordLast)

	ON_WM_ACTIVATE()
	ON_MESSAGE(MESSAGE_SELECT_ROW,OnSelectFeatures)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// TableWindow

TableWindow::TableWindow()
: recBar(0),
	frmSimpleCalc(0)
{
  help = "ilwis\\table_window.htm";
	sHelpKeywords = "Table Window";
	commHandler.SetOwner(this);
  recBar = new RecordBar();
//	causes strange errors!!!!  SetWindowName("TableWindow");	// ensure loading and saving settings
}

TableWindow::~TableWindow()
{
	if (0 != recBar)
	  delete recBar;
	delete frmSimpleCalc;
	frmSimpleCalc = 0;
}



/////////////////////////////////////////////////////////////////////////////
// TableWindow message handlers

#define sMen(ID) ILWSF("men",ID).c_str()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int TableWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();

  menPopup.CreateMenu();
    menSub.CreateMenu();
    addSub(ID_GRAPH);
	  addSub(ID_ROSE);
  addSubMenu(ID_FILE_CREATE);
	add(ID_FILE_SAVE_AS);
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
  add(ID_ADDRECORD);
  add(ID_SELECTFEATURES_BYCOLUMN);
  addMenu(ID_MEN_EDIT);

  menPopup.CreateMenu();
  add(ID_COLUMNS);
  add(ID_ADDCOLUMN);
  add(ID_GOTOCOLUMN);
  add(ID_SORT);
  add(ID_UPDATEALLCOLS);
  addBreak;
  add(ID_TBLSTATS);
  add(ID_JOIN);
  add(ID_AGGREGATE);
  add(ID_CUMULATIVE);
  add(ID_LEASTSQUARES);
  add(ID_SEMIVARIOGRAM);
  add(ID_COLSLICING);
  add(ID_TIMECOLUMN)
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
	add(ID_SHOWADDINFO);
  add(ID_CONFMATRIX);
  addBreak;
  add(ID_VIEW_SELECTED_ONLY);
  menSub.CreateMenu();
	addSub(ID_VIEW_ALL_RECORDS);
  addSubMenu(ID_VIEW_MAP_RECORDS_ONLY);

  addBreak;
  add(ID_COMMANDLINE);
  add(ID_STATPANE);
  add(ID_TBLBUTTONBAR);
  add(ID_STATUSLINE);
  addMenu(ID_MEN_VIEW);

  menPopup.CreateMenu();
  add(ID_HLPKEY);
	add(ID_HELP_RELATED_TOPICS);
	addBreak
  add(ID_HLPMAPTABCALC)
	addBreak
  add(ID_HLPCONTENTS);
  add(ID_HLPINDEX);
  add(ID_HLPSEARCH);
	addBreak
	add(ID_ABOUT)
  addMenu(ID_MEN_HELP);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	SetAcceleratorTable();

	EnableDocking(CBRS_ALIGN_ANY);

	bBar.Create(this, "table.but", "", ID_TBLBUTTONBAR);
	DockControlBar(&bBar, AFX_IDW_DOCKBAR_TOP);

	commBar.Create(this, ID_COMMANDLINE, CommandCombo::cbTable);
	commHandler.setCommands(IlwWinApp()->getCommands()->getCommands());
	DockControlBar(&commBar);

	return 0;
}

BOOL TableWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	wndSplitter.CreateStatic(this, 2, 1, WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL);
	wndSplitter.CreateView(0, 0, pContext->m_pNewViewClass, CSize(400, 500), pContext);
	wndSplitter.CreateView(1, 0, RUNTIME_CLASS(TableSummaryPaneView), CSize(400,30), pContext);
	iPrevStatHeight = 30;
  tpv = (TablePaneView*)wndSplitter.GetPane(0,0);
	SetActiveView(tpv);
	return TRUE;
}

LONG TableWindow::OnExecute(UINT wParam, LONG lParam)
{
	char* str = (char*)(void*)lParam;
	commBar.SetCommandLine(str);
	commBar.AddToHistory(CString(str));
	return commHandler.Execute(str, this);
	//return IlwWinApp()->Execute(str);
}

LONG TableWindow::OnCommandUI(UINT wParam, LONG lParam)
{
	char* str = (char*)(void*)lParam;
	commBar.SetCommandLine(str);
	commBar.AddToHistory(CString(str));
	
	return 1;
}

void TableWindow::OnTblStats()
{
	OnExecute(1, (LPARAM)"clmstatistics");
}

void TableWindow::OnAggregate()
{
	OnExecute(1, (LPARAM)"clmaggregate");
}

void TableWindow::OnJoin()
{
	OnExecute(1, (LPARAM)"clmjoin");
}

void TableWindow::OnLeastSquares()
{
	OnExecute(1, (LPARAM)"clmleastsquares");
}

void TableWindow::OnCumulative()
{
	OnExecute(1, (LPARAM)"clmcumulative");
}

void TableWindow::OnSemiVariogram()
{
	OnExecute(1, (LPARAM)"clmsemivariogram");
}

void TableWindow::OnSlicing()
{
	OnExecute(1, (LPARAM)"clmslicing");
}

void TableWindow::OnTimeColumn() {
	OnExecute(1, (LPARAM)"timefromcolumns");
}

void TableWindow::OnCommandLine()
{
	if (commBar.IsWindowVisible())
		ShowControlBar(&commBar,FALSE,FALSE);
	else
		ShowControlBar(&commBar,TRUE,FALSE);
}

void TableWindow::OnUpdateCommandLine(CCmdUI* pCmdUI)
{
	bool fCheck = commBar.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

bool TableWindow::fExecute(const String& sCmd)
{
  return commHandler.Execute(sCmd);
}

#define SPLITBORDER 3

void TableWindow::OnStatPane()
{
  int iCurrStatHeight, iCurrTableHeight, dummy;
  wndSplitter.GetRowInfo(1, iCurrStatHeight, dummy);
  wndSplitter.GetRowInfo(0, iCurrTableHeight, dummy);
  if (iCurrStatHeight == 0) { 
    // stat pane is off, so reduce to table pane height with previous stat pane height
    if (iCurrTableHeight > 2*iPrevStatHeight) 
      wndSplitter.SetRowInfo(0, max(0, iCurrTableHeight - iPrevStatHeight - 2 /*correction*/), 0);
    else // divide in equal sizes
      wndSplitter.SetRowInfo(0, (iCurrTableHeight + iCurrStatHeight) / 2, 0);
  }
  else  { // extend of table pane so that stat pane is not visible anymore
    wndSplitter.SetRowInfo(0, iCurrTableHeight + iCurrStatHeight + SPLITBORDER, 0);
    iPrevStatHeight = iCurrStatHeight <= 0 ? 120 : iCurrStatHeight;
  }
  wndSplitter.RecalcLayout();
}

void TableWindow::OnUpdateStatPane(CCmdUI* pCmdUI)
{
  int iCurrStatHeight, dummy;
  wndSplitter.GetRowInfo(1, iCurrStatHeight, dummy);
  bool fCheck = iCurrStatHeight != 0;
	pCmdUI->SetCheck(fCheck);
}

TableView* TableWindow::tvw() const
{
  return tpv->tvw();
}

void TableWindow::InitSettings()
{
  DataWindow::InitSettings();
  TableView* tv = tvw();
  int iRowHeight = tpv->iHeight();
  CDC dc;
  dc.CreateCompatibleDC(0);
  tpv->InitColPix(&dc);

  bool fStatPaneVisible;

  if (0 == ObjectInfo::ReadElement("TableView", "StatPaneVisible", tv->fnTbl(), fStatPaneVisible)) {
    if (!fCIStrEqual(tv->fnTbl().sExt, ".tbt") && !fCIStrEqual(tv->fnTbl().sExt, ".his"))
      fStatPaneVisible = false;
    else {
      // check if there are value columns (that are not boolean)
      bool fValues = false;
      for (int i=0; i < tv->iCols(); i++)
        if (tv->cv(i).fValid() && tv->cv(i)->fValues() && (0 == tv->cv(i)->dm()->pdbool())) {
          fValues = true;
          break;
        }
      fStatPaneVisible = fValues;
    }
  }

	// control bars
	IlwisSettings settings3("DefaultSettings");
	bool fShowAsTableCL = settings3.fValue("ShowAsTableCL", false);
  if ( !fShowAsTableCL &&	!fCIStrEqual(tv->fnTbl().sExt, ".tbt")) 
 		ShowControlBar(&commBar,FALSE,FALSE); // hide commandline
	bool fRecordView;
  ObjectInfo::ReadElement("TableView", "RecordView", tvw()->fnTbl(), fRecordView);
	if (fRecordView) {
		CreateRecordBar();
		ShowControlBar(recBar,TRUE,FALSE);
	}
	int iRecBarWidth = 0;
	if (0 != recBar && 0 != recBar->GetSafeHwnd()) {
		if (recBar->IsVisible() && !recBar->IsFloating()) {
			CRect rectRecBar;
			recBar->GetClientRect(rectRecBar);
			if (recBar->IsVertDocked())
				iRecBarWidth = rectRecBar.Width() + 4; 
		}
	}
	int iButWidth;
  if (0 != ObjectInfo::ReadElement("TableView", "ButtonWidth", tvw()->fnTbl(), iButWidth))
		tpv->SetButWidth(iButWidth);
	
  int iRecs = max(15, tv->iNettoRecs());
  if (0 != tv->dm()->pdnone())
    iRecs++;
  // take two third of screen as default table size
  int iWidth = GetSystemMetrics(SM_CXSCREEN) * 2 / 3;
  int iHeight = GetSystemMetrics(SM_CYSCREEN) / 2;

  int iCharWidth = tpv->iChrWidth() + 1;
   // sum all column width
  int iColWidth = tpv->iButWidth() * iCharWidth + 5 + iRecBarWidth;
  for (int i=0; i < tv->iCols(); i++)
    iColWidth += tv->cv(i).iWidth * iCharWidth + 5;
  // make smaller if space not needed
  if (iColWidth < iWidth)
    iWidth = max(iColWidth, 250); // use a minimum width
  if (iRecs * iRowHeight < iHeight)
    iHeight = (iRecs + 1) * iRowHeight;
  
  int iIdealStatPaneHeight = 5 * iRowHeight + 2 /* ask 2 more than you will get */;
  int iCurrStatHeight, iCurrTableHeight, iCurrTableWidth, dummy;
  wndSplitter.iIdealStatPaneHeight = iIdealStatPaneHeight;
  
  CRect rectWin;
  GetWindowRect(&rectWin);

  // set width 
  wndSplitter.GetColumnInfo(0, iCurrTableWidth, dummy);
  rectWin.right += iWidth - iCurrTableWidth + 3;
	int iIdealWidth = rectWin.Width();
	MoveWindow(&rectWin);
  wndSplitter.RecalcLayout();

  GetWindowRect(&rectWin);
  // set height
  wndSplitter.GetRowInfo(1, iCurrStatHeight, dummy);
  wndSplitter.GetRowInfo(0, iCurrTableHeight, dummy);

  int iIdealHeight = rectWin.Height() + iHeight - iCurrTableHeight + (fStatPaneVisible ? iIdealStatPaneHeight: 0) - iCurrStatHeight;

	IlwisSettings settings("DefaultSettings\\DataObjectPos");
	int iMaxW = dc.GetDeviceCaps(HORZRES);
	int iMaxH = dc.GetDeviceCaps(VERTRES);

	CPoint ptTopLeft;
	CSize sz;
	ptTopLeft.x = settings.iValue("X1");
	ptTopLeft.y = settings.iValue("Y1");
	sz.cx = settings.iValue("XSize1");
	sz.cy = settings.iValue("YSize1");

	CPoint ptTopLeftNext = ptTopLeft;
	CSize szNext = sz;
	IlwWinApp()->GetNextWindowRect(ptTopLeftNext, szNext); 
	if (ptTopLeftNext.x >= 0)
		ptTopLeft.x = ptTopLeftNext.x;
	if (ptTopLeftNext.y >= 0)
		ptTopLeft.y = ptTopLeftNext.y;
	if (szNext.cx > 0)
		sz.cx = szNext.cx;
	if (szNext.cy > 0)
		sz.cy = szNext.cy;

	if (ptTopLeft.x < 0)
		ptTopLeft.x = rectWin.left;
	if (ptTopLeft.y < 0)
		ptTopLeft.y = rectWin.top;
	if (sz.cx < 100)
		sz.cx = iIdealWidth;
  if (sz.cy < 100)
		sz.cy = iIdealHeight + 1;

	MoveWindow(ptTopLeft.x, ptTopLeft.y, sz.cx, sz.cy);
  wndSplitter.RecalcLayout();

  // divide height between two panes
  wndSplitter.GetRowInfo(1, iCurrStatHeight, dummy);
  wndSplitter.GetRowInfo(0, iCurrTableHeight, dummy);
  if (fStatPaneVisible) {
    if (iCurrTableHeight > iIdealStatPaneHeight) 
      wndSplitter.SetRowInfo(0, max(0, iCurrTableHeight + iCurrStatHeight - iIdealStatPaneHeight), 0);
    else  // divide in equal sizes
      wndSplitter.SetRowInfo(0, (iCurrStatHeight + iCurrTableHeight) / 2, 0);
    wndSplitter.GetRowInfo(1, iPrevStatHeight, dummy);
  }
  else {
    wndSplitter.SetRowInfo(0, iCurrTableHeight + iCurrStatHeight + 1, 0);
    iPrevStatHeight = iIdealStatPaneHeight;
  }

  wndSplitter.RecalcLayout();

	String sTitle = String(TR("Toolbar for table %S").c_str(), tv->fnTbl().sShortName());
	bBar.SetWindowText(sTitle.c_str());
	
}

void TableWindow::SaveSettings()
{
	// fill lgb
	POSITION pos = m_listControlBars.GetHeadPosition();
	list<CControlBar*> lcb;
	while (pos != NULL) 
	{
		CControlBar* pBar =	(CControlBar*)m_listControlBars.GetNext(pos);
		lcb.push_back(pBar);
  } 
	// remove additional info bar
	list<CControlBar*>::iterator iter;
	for (iter = lcb.begin(); iter != lcb.end(); ++iter) 
	{
		CControlBar* pBar = *iter;
		if (!IsWindow(pBar->m_hWnd))
			continue;
		if (pBar->GetDlgCtrlID() == ID_ADDINFOBAR) {
			delete pBar;
			continue;
		}
  }    
  FrameWindow::SaveSettings();
  int iStatHeight, dummy;
  wndSplitter.GetRowInfo(1, iStatHeight, dummy);
  ObjectInfo::WriteElement("TableView", "StatPaneVisible", tvw()->fnTbl(), iStatHeight > 0);
	bool fRecordViewVisible = (0 != recBar) && (0 != recBar && 0 != recBar->GetSafeHwnd()) && recBar->IsWindowVisible() != 0;
  ObjectInfo::WriteElement("TableView", "RecordView", tvw()->fnTbl(), fRecordViewVisible);
//	CDockState ds;
//	GetDockState(ds);
//  ObjectInfo::WriteElement("TableView", "DockingState",tvw()->fnTbl(), ds);
//  ObjectInfo::WriteElement("TableView", "DockState",tvw()->fnTbl(), (const char*)0); // remove old information
  ObjectInfo::WriteElement("TableView", "ButtonWidth", tvw()->fnTbl(), tpv->iButWidth());
}

IMPLEMENT_DYNCREATE(TableSplitterWindow, CSplitterWnd)

BEGIN_MESSAGE_MAP(TableSplitterWindow, CSplitterWnd)
	//{{AFX_MSG_MAP(TableSplitterWindow)
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void TableSplitterWindow::OnSize(UINT nType, int cx, int cy)
{
  if (m_pRowInfo == NULL) {
    CSplitterWnd::OnSize(nType, cx, cy);
    return;
  }
  int iPrevStatHeight, dummy;
  GetRowInfo(1, iPrevStatHeight, dummy);
  CSplitterWnd::OnSize(nType, cx, cy);
  int iTableHeight, iStatHeight;
  GetRowInfo(0, iTableHeight, dummy);
  GetRowInfo(1, iStatHeight, dummy);
  if (iTableHeight + iStatHeight <= 2 * iIdealStatPaneHeight && 
      iTableHeight <= iStatHeight)
    // divide in equal sizes
    SetRowInfo(0, max(0, (iTableHeight + iStatHeight) / 2), 0);
  else
    SetRowInfo(0, max(0, iTableHeight + iStatHeight - iPrevStatHeight), 0);
  RecalcLayout();
}

void TableSplitterWindow::OnPaint()
{
	CSplitterWnd::OnPaint(); 
	// if we do not draw our selves in the case of no statpane
	// below the scrollbar two lines will not be drawn
	CWindowDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	Color clrBtnHilite = SysColor(COLOR_BTNHILIGHT);
	Color clrBtnFace = SysColor(COLOR_BTNFACE);
	Color clrBtnShadow = SysColor(COLOR_BTNSHADOW);
	Color clrWindowFrame = SysColor(COLOR_WINDOWFRAME);
	dc.Draw3dRect(rect, clrBtnShadow, clrBtnHilite);
	rect.InflateRect(-1, -1);
	dc.Draw3dRect(rect, clrWindowFrame, clrBtnFace);
}

void TableWindow::OnButtonBar()
{
	if (bBar.IsWindowVisible())
		ShowControlBar(&bBar,FALSE,FALSE);
	else
		ShowControlBar(&bBar,TRUE,FALSE);
}

void TableWindow::OnUpdateButtonBar(CCmdUI* pCmdUI)
{
	bool fCheck = bBar.IsVisible() != 0;
  pCmdUI->SetCheck(fCheck);
}

void TableWindow::OnUpdateRecFirst(CCmdUI* pCmdUI)
{
	int iCurRec = tpv->iCurrRec();
	pCmdUI->Enable(iCurRec == iUNDEF || iCurRec != 1);
}

void TableWindow::OnUpdateRecLast(CCmdUI* pCmdUI)
{
	int iCurRec = tpv->iCurrRec();
	pCmdUI->Enable(iCurRec == iUNDEF || iCurRec != tvw()->iNettoRecs());
}

void TableWindow::OnUpdateRecNext(CCmdUI* pCmdUI)
{
  int iCurRec = tpv->iCurrRec();
	pCmdUI->Enable(iCurRec != iUNDEF && iCurRec != tvw()->iNettoRecs());
}

void TableWindow::OnUpdateRecPrev(CCmdUI* pCmdUI)
{
  int iCurRec = tpv->iCurrRec();
	pCmdUI->Enable(iCurRec != iUNDEF && iCurRec != 1);
}

void TableWindow::OnFileProp()
{
  fExecute("prop");
}

void TableWindow::OnAddRecord()
{ 
	fExecute("addrecord 1");
}

void TableWindow::OnUpdateAddRecord(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(tvw()->dm()->pdnone() != 0);
}

void TableWindow::CreateRecordBar()
{
	// if already created return
	if (0 != recBar->GetSafeHwnd()) 
		return;
	recBar->Create(this, ID_RECORDBAR);
  GetActiveView()->GetDocument()->AddView(recBar->view);
  String sTitle(TR("Record view for table %S").c_str(), tvw()->fnTbl().sShortName());
  recBar->SetWindowText(sTitle.c_str());
	Ilwis::Record rec = tvw()->rec(1);
	recBar->view->SetRecord(rec,tvw());
  FloatControlBar(recBar,CPoint(100,100));
	ShowControlBar(recBar,FALSE,FALSE);
}

void TableWindow::OnShowRecordView()
{
	CreateRecordBar();
	if (recBar->IsWindowVisible())
		ShowControlBar(recBar,FALSE,FALSE);
	else
		ShowControlBar(recBar,TRUE,FALSE);
}

void TableWindow::OnUpdateShowRecordView(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(0 != recBar && 0 != recBar->GetSafeHwnd() && recBar->IsVisible());
}

void TableWindow::OnAdditionalInfo()
{
  fExecute("addinfo");  
}

void TableWindow::OnColMan()
{
  fExecute("colman");  
}

void TableWindow::OnUpdateAdditionalInfo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(tvw()->fAdditionalInfo());
}

void TableWindow::GotoRecord(long iRec)
{
  zRect rect = tpv->rectSelect();
	if (!rect.IsRectEmpty())
  	tpv->InvalidateRect(&rect);
  long iCurrStart = tpv->iFirstVisibleRow();
	int iVisibleRows = tpv->iLastVisibleRow() - iCurrStart + 1;
	long iRowInTable = tvw()->iRow(iRec);
  long iNewStart = iRowInTable<= iVisibleRows ? 1 : max(1, iRowInTable - iVisibleRows / 2); // - iDiff / 2;
  tpv->vertPixMove(iNewStart - iCurrStart);
	MinMax mmSelect;
	mmSelect.MinCol() = -1;
	mmSelect.MaxCol() = tvw()->iCols();
	mmSelect.MinRow() = mmSelect.MaxRow() = iRowInTable;
	tpv->setSelection(mmSelect);
  rect = tpv->rectSelect();
	if (!rect.IsRectEmpty())
  	tpv->InvalidateRect(&rect);
		// also move tField !!!
  Ilwis::Record rec = tvw()->rec(iRec);
	// ensure that Record Bar is properly initialized
	CreateRecordBar();
  recBar->view->SetRecord(rec,tvw());
	// ensure that Record Bar is visible
	ShowControlBar(recBar,TRUE,FALSE);
}


void TableWindow::OnGotoRecordFirst()
{
	long iRec = tvw()->iRec(1);
	long iRecs = tvw()->iRecs();
	if (iRec <= 0 || iRec > iRecs)
		return;
	GotoRecord(iRec);
}

void TableWindow::OnGotoRecordLast()
{
	long iRows = tpv->iRows();
	long iRecs = tvw()->iRecs();
	long iRec = tvw()->iRec(iRows);
	if (iRec <= 0 || iRec > iRecs)
		return;
	GotoRecord(iRec);
}

void TableWindow::OnGotoRecordPrev()
{
	long iRec = tvw()->iRec(tpv->sel().mm().MinRow()-1);
	long iRecs = tvw()->iRecs();
  if (iRec <= 0 || iRec > iRecs)
			return;
	GotoRecord(iRec); 
}

void TableWindow::OnGotoRecordNext()
{
	long iRec = tvw()->iRec(tpv->sel().mm().MinRow()+1);
	long iRecs = tvw()->iRecs();
	if (iRec <= 0 || iRec > iRecs)
		return;
	GotoRecord(iRec); 
}

void TableWindow::OnGotoRecord()
{
  class Form: public FormWithDest
  {
  public:
    Form(CWnd* parent, const Domain& dm, String* sRec)
    : FormWithDest(parent, TR("Goto Record"))
    {
      new FieldBlank(root);
      fv = new FieldVal(root, TR("&Record"), dm, sRec);
      SetMenHelpTopic("ilwismen\\goto_record.htm");
      create();
    }
    FormEntry* feDefaultFocus()
    { 
      return fv;
    }
  private:
    FieldVal* fv;  
  };
  long iRec;
  String sRec;
  iRec = tpv->iFirstVisibleRow();
  sRec = tvw()->sRow(iRec);
  Form frm(this, tvw()->dm(), &sRec);
  if (frm.fOkClicked()) {
		CreateRecordBar();
    Ilwis::Record rec = recBar->view->GetRecord();
	  if (rec.Goto(sRec))
		  GotoRecord(rec.iRec());
  }
}


void TableWindow::OnConfusionMatrix()
{
	  fExecute("confmat");  
}

SimpleCalcResultForm *TableWindow::frmSimpleCalcResults()
{

	if (0 != frmSimpleCalc && !IsWindow(frmSimpleCalc->m_hWnd)) // will happen if closed by user; the class has already been destructed
		frmSimpleCalc = 0;
	if (0 == frmSimpleCalc) 
		frmSimpleCalc = new SimpleCalcResultForm(this);
	return frmSimpleCalc;
}

void TableWindow::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	DataWindow::OnActivate(nState, pWndOther, bMinimized);
	if (WA_ACTIVE == nState)
		commBar.Activate();
}

LONG TableWindow::OnSelectFeatures(UINT wParam, LONG lParam)
{
	RowSelectInfo * inf = (RowSelectInfo *)wParam;
	tpv->selectFeatures(*inf);
	delete inf;

	return 1;
}


