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
#pragma warning( disable : 4786 )

#include "Headers\stdafx.h"
#include "Client\Headers\AppFormsPCH.h"
#include "Client\ilwis.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <atldbcli.h>
#include <atldbsch.h>
#include "Client\Forms\IMPORT.H"
#include "Engine\Base\File\Directory.h"
#include "Client\Forms\FormGeneralPreferences.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Engine\Base\System\commandhandler.h"
#include "Client\MainWindow\mainwind.h"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Client\FormElements\formbase.h"
#include "Client\Forms\copyobj.h"
#include "Client\Forms\expobj.h"
#include "Client\FormElements\selector.h"
#include "Client\Forms\ImportAsciiTableWizard.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
//#include "ApplicationsUI\frmpolap.h"
#include "Client\MainWindow\Catalog\CatalogFrameWindow.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Client\FormElements\ReportListView.h"
#include "Client\Forms\repwin.h"
#include "Client\Base\Menu.h"
#include "Engine\Scripting\Script.h"
#include "Client\Editors\Digitizer\DIGITIZR.H"
#include "Client\MainWindow\ACTION.H"
#include "Client\Forms\GeneralImportForm.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\MainWindow\DirectoryDocument.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\Forms\SimpleCalcResultsForm.h"
#include "Client\Editors\Utils\SBDestination.h"
#include "Client\Forms\ProgressListWindow.h"
#include "Headers\version.h"

BEGIN_MESSAGE_MAP( MainWindow, CMDIFrameWnd )
	//{{AFX_MSG_MAP( MainWindow )
	ON_WM_CREATE()
	ON_MESSAGE(WM_COPYDATA, OnCopyData)
	ON_MESSAGE(ID_ILWISREP, OnIlwisReportMsg)
	ON_MESSAGE(ILWM_EXECUTE, OnExecute)
	ON_MESSAGE(ILWM_CMDHANDLERUI, OnCommandUI)
	ON_MESSAGE(ILW_READCATALOG, OnReadCatalog)
	ON_MESSAGE(ILWM_CLOSECOLLECTION, OnCloseCollection)
	ON_MESSAGE(ILWM_OPENCATALOG, OnOpenCatalog)
	ON_MESSAGE(ILWM_SETCOMMANDLINE, OnSetCommandLine)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()	
	ON_WM_SETFOCUS()	
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_STATUSLINE, OnStatusLine)
	ON_UPDATE_COMMAND_UI(ID_STATUSLINE, OnUpdateStatusLine)
	ON_COMMAND(ID_CAT_NEW, OnNewCatalog)
	ON_COMMAND(ID_COMMANDLINE, OnCommandLine)
	ON_UPDATE_COMMAND_UI(ID_COMMANDLINE, OnUpdateCommandLine)
	ON_COMMAND(ID_OPERLIST, OnOperationList)
	ON_UPDATE_COMMAND_UI(ID_OPERLIST, OnUpdateOperationList)
	ON_COMMAND(ID_SELBAR, OnSelBar)
	ON_UPDATE_COMMAND_UI(ID_SELBAR, OnUpdateSelBar)
	ON_COMMAND(ID_MAINWINDBAR, OnStandardBar)
	ON_UPDATE_COMMAND_UI(ID_MAINWINDBAR, OnUpdateStandardBar)

	ON_COMMAND_RANGE(ID_CREATERASMAP, ID_CREATEILO, OnCreateObject)
	ON_COMMAND(ID_GRAPH, OnGraph)
	ON_COMMAND(ID_ROSE, OnRoseDiagram)
	ON_COMMAND(ID_RESTOREALL, OnRestoreAll)
	ON_COMMAND(ID_MINIMIZEALL, OnMinimizeAll)
	ON_COMMAND(ID_CLOSE_ALL, OnCloseCatalogs)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_EXPORT, OnExport)
	ON_COMMAND(ID_FILE_IMPORT_MAP, OnImportMap)
	ON_COMMAND(ID_FILE_IMPORT_TABLE, OnImportTable)
	ON_COMMAND(ID_FILE_IMPORT_GDAL, OnImportGdal)
	ON_COMMAND(ID_FILE_IMPORT_GEOGATEWAY, OnImportGeoGateway)
	ON_COMMAND(ID_FILE_IMPORT_GENRASTER, OnImportGenRaster)
	ON_COMMAND(ID_CAT_REDRAW, OnCatRedraw)
	ON_COMMAND(ID_OPENMAP, OnOpenMap)
	ON_COMMAND(ID_FILE_OPENASTBL, OnOpenAsTable)		 
	ON_COMMAND(ID_FILE_PREFERENCES, OnPreferences)
	ON_COMMAND(ID_IMPORT, OnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


#define sMen(ID) ILWSF("men",ID).c_str()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

MainWindow::MainWindow()
	: iStartOwnerQueries(0)
	, iNrReportWindows(0)
	, fAllowCatalogUpdates(true)
	, frmSimpleCalc(0)
	, plwProgress(0)
{
  htpTopic = htpContentsMainWindow;
    mpQueries[ TR("DEF:All")] = "";

	mpQueries[TR("DEF:Maps with domain image")] = "map.domain.type=image";
	mpQueries[TR("DEF:Maps with domain value")] = "map.domain.type=value";
	mpQueries[TR("DEF:Maps with domain class or id")] = "map.domain.type=class or map.domain.type=identifier";
	mpQueries[TR("DEF:Maps with georef corners")] = "map.georef.type=georefcorners";
	mpQueries[TR("DEF:Maps with georef tiepoints")] = "map.georef.type=georefctp";
	mpQueries[TR("DEF:All objects made today")] = "time > now-1";


	IlwisSettings settings(String("%S\\Queries", MainWindow::sBaseRegKey()));
	int iNr = settings.iValue("NrOfQueries", 0);
	for ( int i=0; i<iNr; ++i)
	{
		String sQuery = settings.sValue(String("query%d", i));
		mpQueries[sQuery.sHead("|") ] = sQuery.sTail("|");
	}

	//SetWindowName("Main Widow");	// ensure loading and saving settings
	string s( VERSION_STRING);
	Create(ILWIS_VERSION_NAME, VERSION_STRING);  

  CWinApp* app = AfxGetApp();
  HICON hIconLogo = app->LoadIcon("LogoIcon");
  SetIcon(hIconLogo, TRUE);
}

MainWindow::~MainWindow()
{
	if (0 != frmSimpleCalc && IsWindow(frmSimpleCalc->m_hWnd)) 
		delete frmSimpleCalc;
	if ( 0 != plwProgress && IsWindow(plwProgress->m_hWnd))
		plwProgress->DestroyWindow();
}

BOOL MainWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	// Now includes registration check
	if(!(CMDIFrameWnd::PreCreateWindow(cs)))
		return FALSE;
	cs.style |= FWS_ADDTOTITLE;
 	cs.dwExStyle |= WS_EX_WINDOWEDGE;
	return TRUE;
}

int MainWindow::OnCreate(LPCREATESTRUCT lpc)
{
	Menu men(this);

	String sTop = sMen(ID_MEN_FILE);
	String sCreate = sTop + "," + sMen(ID_FILE_CREATE);

	men.Add(sCreate, ID_CREATEPNTMAP);
	men.Add(sCreate, ID_CREATESEGMAP);
	men.Add(sCreate, ID_CREATERASMAP);
	men.Add(sCreate, ID_CREATETBL);
	men.Add(sCreate, 0, MF_SEPARATOR);
	men.Add(sCreate, ID_CREATEMPL);
	men.Add(sCreate, ID_CREATEIOC);
	//men.Add(sCreate, ID_CREATEILO); 
	men.Add(sCreate, 0, MF_SEPARATOR);
	men.Add(sCreate, ID_GRAPH); 
	men.Add(sCreate, ID_ROSE); 
	men.Add(sCreate, 0, MF_SEPARATOR);
	men.Add(sCreate, ID_CREATEDOM);
	men.Add(sCreate, ID_CREATERPR);
	men.Add(sCreate, ID_CREATEGRF);
	men.Add(sCreate, ID_CREATECSY);
	men.Add(sCreate, 0, MF_SEPARATOR);
	men.Add(sCreate, ID_CREATESMS);
	men.Add(sCreate, ID_CREATE2DIMTBL);
	men.Add(sCreate, ID_CREATEFIL);
	men.Add(sCreate, ID_CREATEFUN);
	men.Add(sCreate, ID_CREATEISL);

	men.Add(sTop,    ID_FILE_OPEN);
	men.Add(sTop,    ID_FILE_OPENASTBL);
	men.Add(sTop,    ID_OPENPIXELINFO);
	men.Add(sTop,    0,                 MF_SEPARATOR);
	men.Add(sTop,    ID_CAT_PYRAMIDS);
	men.Add(sTop,    0,                 MF_SEPARATOR);	
	men.Add(sTop,    ID_FILE_PREFERENCES);
	men.Add(sTop,    0,                 MF_SEPARATOR);
	//men.Add(sTop,    ID_FILE_DIGSETUP);
	//men.Add(sTop,    ID_FILE_DIGREF);
	men.Add(sTop,    0,                 MF_SEPARATOR);

	men.Add(sTop,ID_IMPORT);
	//String sImport = sTop + "," + sMen(ID_IMPORT);
	//men.Add(sImport, ID_FILE_IMPORT_MAP);
	//men.Add(sImport, ID_FILE_IMPORT_TABLE);
	//men.Add(sImport, ID_FILE_IMPORT_GDAL);
	//men.Add(sImport, ID_FILE_IMPORT_GEOGATEWAY);
	//men.Add(sImport, ID_FILE_IMPORT_GENRASTER);
	//men.Add(sImport, ID_FILE_IMPORT_CONNECT_DATABASE);

	men.Add(sTop,    ID_EXPORT);
	men.Add(sTop,    0,                 MF_SEPARATOR);
	men.Add(sTop,    ID_EXIT);

	sTop = sMen(ID_MEN_EDIT);

	men.Add(sTop,			ID_EDIT_COPY);
	men.Add(sTop,			ID_CAT_COPY);
	men.Add(sTop,			ID_EDIT_PASTE);
	men.Add(sTop,			ID_CAT_DEL);
	men.Add(sTop,			   0,             MF_SEPARATOR);
	men.Add(sTop,     ID_EDIT_SELECT_ALL);
	men.Add(sTop,			   0,             MF_SEPARATOR);
	men.Add(sTop,			ID_EDIT_OBJ);
	men.Add(sTop,			ID_CAT_PROP);

  int id = ID_OPERATION0;

	sTop = sMen(ID_MEN_OPERATIONS);

  const ActionList* actList = IlwWinApp()->acl();
  for (SLIterCP<Action> iter(actList); iter.fValid(); ++iter, ++id) 
	{
    Action* act = iter();
    if ("-" == act->sMenOpt() || "" == act->sMenOpt()) continue;
		String sMenuItem;
		if ( act->sMidOpt() != "" )
			sMenuItem = String("%S,%S,%S,%S", sTop, act->sMenOpt(), act->sMidOpt(), act->sSubOpt());
		else
			sMenuItem = String("%S,%S,%S", sTop, act->sMenOpt(), act->sSubOpt());
		men.AppendMenuItem(sMenuItem, id);
	}

	sTop = sMen(ID_VIEW);
	
	men.Add(sTop,		ID_VIEW_LIST);
	men.Add(sTop,		ID_VIEW_DETAILS);
	men.Add(sTop,				   0,             MF_SEPARATOR);
	men.Add(sTop,		ID_CATVIEW_OPTIONS);
	String sShow = sTop + "," + sMen(ID_CAT_SHOWMENU);
	men.Add(sShow, ID_CAT_SHOW_MAPTBL);
	men.Add(sShow, ID_CAT_SHOW_DATA);
	men.Add(sShow, ID_CAT_SHOW_ALL);
	men.Add(sShow, ID_CAT_SHOW_BASEMAPS);

	String sSort = sTop + "," + sMen(ID_CAT_SORT);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctNAME); 
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctTYPE);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctMODIFIED);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctCALCULATED);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctDEPENDENT);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctUPTODATE);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctREADONLY);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctDOMAIN);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctDOMAINTYPE);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctCOORDSYSTEM);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctCOORDSYSTEMTYPE);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctGEOREF);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctGEOREFTYPE);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctGSIZE);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctATTRIBUTETABLE);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctDESCRIPTION);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctEXPRESSION);
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctOBJECTSIZE);	
	men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctNONE);
	//men.Add(sSort,  ID_CAT_SORTCOL_START + Catalog::ctOBJECTSIZE);

//	men.Add(sTop,    ID_CATALOG_QUERIES);
	SetQueryMenu(men);

	men.Add(sTop,	0, MF_SEPARATOR);
	men.Add(sTop, ID_MAINWINDBAR);
	men.Add(sTop, ID_SELBAR);
	men.Add(sTop,	ID_OPERLIST);
	men.Add(sTop,	ID_COMMANDLINE);
	men.Add(sTop,	ID_STATUSLINE);

	sTop = sMen(ID_MEN_WINDOW);
	men.Add(sTop, ID_CAT_NEW);
	men.Add(sTop, ID_CAT_REDRAW);
	men.Add(sTop,	0, MF_SEPARATOR);
	String sCat = sTop + "," + sMen(ID_ARRANGE_CAT);
	men.Add(sCat, ID_WINDOW_CASCADE);
	men.Add(sCat, ID_WINDOW_TILE_HORZ);
	men.Add(sCat, ID_WINDOW_TILE_VERT);
	men.Add(sTop, ID_CLOSE_ALL);
	men.Add(sTop,	0, MF_SEPARATOR);
	String sData = sTop + "," + sMen(ID_WINDOW_DATA);
	men.Add(sData, ID_CLOSEALL);
	men.Add(sData, ID_MINIMIZEALL);
	men.Add(sData, ID_RESTOREALL);

	AddHelpMenu();
	SetAcceleratorTable();

  if (-1 == CMDIFrameWnd::OnCreate(lpc))
    return -1;

	status.Create(this);//, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, 1960);
	unsigned int ind[spANY] = { 0, 0, 0, 0, 0, 0, 0 };
	status.SetIndicators(ind, spANY);

	EnableDocking(CBRS_ALIGN_ANY);

	buttonBar.Create(this, "mainwnd.but", "Main Window", ID_MAINWINDBAR);
	DockControlBar(&buttonBar, AFX_IDW_DOCKBAR_TOP);

	selBar.Create(this, "selection.but", "Selections", ID_SELBAR);
//	RecalcLayout();
//	CRect rct;
//	buttonBar.GetWindowRect(&rct);
//	rct.OffsetRect(1,0);
	DockControlBar(&selBar, AFX_IDW_DOCKBAR_TOP);

	commBar.Create(this, ID_COMMANDBAR, CommandCombo::cbMain);
	DockControlBar(&commBar);

	avBar.Create(this);
	DockControlBar(&avBar, AFX_IDW_DOCKBAR_LEFT);

  return 0;
}

void MainWindow::SetQueryMenu(Menu& menu)
{
	vector<String> defaults, owner;

	size_t iSz = mpQueries.size();

	for(map<String, String>::iterator cur = mpQueries.begin(); cur!=mpQueries.end(); ++cur)
	{
		if ( (*cur).first.sHead(":") == "DEF")
		{
			defaults.push_back((*cur).first);
		}
		else
			owner.push_back((*cur).first);
	}
	iStartOwnerQueries = ID_START_CATALOGQUERIES;
	String sTop = String("%s,%s", sMen(ID_VIEW) ,sMen(ID_CATALOG_QUERIES));
	String sItem;
	for(vector<String>::iterator cur2 = defaults.begin(); cur2 != defaults.end(); ++cur2)
	{
		sItem = sTop + "," + (*cur2).sTail(":").c_str();
		menu.AppendMenuItem(sItem, iStartOwnerQueries++);
	}
	if ( owner.size() > 0 )
	{
		int iID = iStartOwnerQueries;
		menu.AppendSeparatorAfter(sItem);
		for(vector<String>::iterator cur2 = owner.begin(); cur2 != owner.end(); ++cur2)
		{
			sItem = sTop  + "," + (*cur2).c_str();
			menu.AppendMenuItem(sItem, iID++);
		}
	}
}

LONG MainWindow::OnIlwisStart(const char* str)
{
	if (0 == str)
		return FALSE;
	if (0 == *str) {
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
		commBar.Activate();
		return TRUE;
	}

	String sMsg = str;
	sMsg = sMsg.sTrimSpaces();
	if ( sMsg == "")

		if ("quit" == sMsg || "exit" == sMsg) {
			PostMessage(WM_CLOSE);
			return TRUE;
		}
		CWinApp* app = AfxGetApp();
		Script::Exec(sMsg);
		//  CDocument* doc = app->OpenDocumentFile(sMsg.c_str());
		//  if (doc)
		//    return TRUE;
		//  else
		//    return FALSE;

		commBar.Activate();

		return TRUE;
}

LONG MainWindow::OnCopyData(UINT, LONG p)
{
  if (0 == p)
    return FALSE;
  COPYDATASTRUCT* cds = (COPYDATASTRUCT*)p;
  switch (cds->dwData) {
    case ID_ILWISSTART:
      return OnIlwisStart((LPTSTR)cds->lpData);
    default:
      return FALSE;
  }
}

LRESULT MainWindow::OnSetCommandLine(WPARAM wPar, LPARAM) {
	String *s = (String *)wPar;
	SetCommandLine(*s, true);
	return 0;
}

LONG MainWindow::OnIlwisReportMsg(UINT iLP, LONG lp)
{
	ISTRUE(fINotEqual, lp, 0L);
	void* vp = (void*)(lp);
	long iRet = 0;
	UINT iRepCode = LOWORD(iLP);
	unsigned short iProgressID = HIWORD(iLP);
	if ( plwProgress == 0 && iProgressID > 0)
	{
		plwProgress = new ProgressListWindow();
//		plwProgress->ShowWindow(TRUE);
	}		
	
	switch( iRepCode )
	{
		case ILWREP_START:
			{
				Tranquilizer* tr = reinterpret_cast<Tranquilizer*>(vp);
				if ( iProgressID != 0) // not zero for use with the new tranq setup
				{
					plwProgress->Start(tr);
					iRet = (LPARAM) plwProgress;
				}
				else
				{
					++iNrReportWindows;					
					iRet = (LPARAM) new ReportWindow(tr);
				}					
			}
			break;
		case ILWREP_STOP:
			{
				if ( iProgressID != 0 )
					plwProgress->Stop(iProgressID);
				else
				{
					--iNrReportWindows;
					ReportWindow* rw = reinterpret_cast<ReportWindow*>(vp);
					rw->Stop();
					delete rw;
					rw = 0;					
				}					
			}
			break;
	}
	return iRet;
}

void MainWindow::OnClose() 
{
	if (IlwWinApp()->fWindowsOpen() || 
		  IlwWinApp()->iOpeningDoc > 0 ||
			iNrReportWindows > 0)
	{
		int iRet = MessageBox(TR("Are your sure( Data still in use)?").c_str(), TR("Close ILWIS").c_str(),
	       MB_YESNO | MB_ICONEXCLAMATION | MB_TOPMOST);
		if (IDYES != iRet)
			return;
	}

//	SaveBarState("MainWindow");
	IlwisSettings settings(sBaseRegKey());
	SaveStateOfMDIChildren(settings);
	SaveState(settings);

	IlwWinApp()->DeleteDirDoc();
	IlwWinApp()->OnCloseAll();
	CMDIFrameWnd::OnClose();
}

void MainWindow::SaveStateOfMDIChildren(IlwisSettings& settings)
{
	int iNr=0;
	CMDIChildWnd* wndfirst = MDIGetActive();
	if ( wndfirst )
	{
		CMDIChildWnd* wndcur = wndfirst; 
		do 
		{
			CDocument *doc = wndcur->GetActiveDocument();
			CatalogDocument *catdoc = dynamic_cast<CatalogDocument *>(doc);
			if ( catdoc)
				catdoc->SaveSettings(settings, iNr++);
			MDINext();
			wndcur = MDIGetActive();
		} while(wndcur != wndfirst );
	}
	settings.SetValue("NrDocs", iNr);

}

void MainWindow::OnDestroy() 
{
	CMDIFrameWnd::OnDestroy();
	PostQuitMessage(0);
}

LONG MainWindow::OnExecute(UINT, LONG lParam)
{				 
	try {
		char* str = (char*)(void*)lParam;  // avoid calling String(const char* sFormat..)
		IlwWinApp()->Execute(str); 
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}
	return 0;
}

LONG MainWindow::OnCommandUI(UINT, LONG lParam)
{				 
	try {

		char* str = (char *)(void*)lParam;  // avoid calling String(const char* sFormat..) b 
		String s(str);
		delete [] str;
		return IlwWinApp()->ExecuteUI(s);
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}
	return -1;
}

void MainWindow::OnCommandLine()
{
	if (commBar.IsWindowVisible())
		ShowControlBar(&commBar,FALSE,FALSE);
	else
		ShowControlBar(&commBar,TRUE,FALSE);
}

void MainWindow::OnUpdateCommandLine(CCmdUI* pCmdUI)
{
	bool fCheck = commBar.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void MainWindow::SetCommandLine(const String& s, bool fAddToHistory )
{
	if ( s.sHead(" ").toLower() != "showexpressionerror")
		commBar.SetCommandLine(s.c_str(), fAddToHistory);
}

void MainWindow::GetMessageString(UINT nID, CString& sMessage) const
{
	if (nID >= ID_OPERATION0 && nID <= ID_OPERATION0+500) {
		IlwisWinApp* iwa = IlwWinApp();
		const ActionList& actList = *iwa->acl();
		Action* act = actList[nID-ID_OPERATION0];
		String sDsc = act->sDescription();
		sMessage = sDsc.c_str();
	}
	else {
		string s;
		if (IlwisWinApp::fHelpNumbers())
			s = String("%i: %S", nID, ILWSF("dsc", nID));
		else
			s = ILWSF("dsc", nID);
		sMessage = s.c_str();
	}
}

void MainWindow::OnOperationList()
{
	if (avBar.IsWindowVisible())
		ShowControlBar(&avBar,FALSE,FALSE);
	else
		ShowControlBar(&avBar,TRUE,FALSE);
}

void MainWindow::OnUpdateOperationList(CCmdUI* pCmdUI)
{
	bool fCheck = avBar.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void MainWindow::OnSelBar()
{
	OnBarCheck(ID_SELBAR);
}

void MainWindow::OnUpdateSelBar(CCmdUI* pCmdUI)
{
	OnUpdateControlBarMenu(pCmdUI);
}

void MainWindow::OnStandardBar()
{
	OnBarCheck(ID_MAINWINDBAR);
}

void MainWindow::OnUpdateStandardBar(CCmdUI* pCmdUI)
{
	OnUpdateControlBarMenu(pCmdUI);
}

void MainWindow::AddHelpMenu()
{
	CMenu* men = GetMenu();
	CMenu menPopup;
 	menPopup.CreateMenu();
 	add(ID_HLPKEY);
	add(ID_HELP_RELATED_TOPICS);
	addBreak
 	add(ID_HLPCONTENTS);
 	add(ID_HLPINDEX);
 	add(ID_HLPSEARCH);
	addBreak
 	add(ID_HLPMAPTABCALC);
 	add(ID_HLPOPERATIONS);
 	add(ID_HLPILWEXPRES);
	add(ID_HLPSEBS);
	addBreak
 	add(ID_HLPGLOSSARY);
	add(ID_HLPHOWTO);
	addBreak
	add(ID_ABOUT)
 	men->AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID_MEN_HELP));
 	menPopup.Detach();
}

void MainWindow::InitialUpdate(CDocument* pDoc, BOOL bMakeVisible)
{
	// if the frame does not have an active view, set to first pane
	CView* pView = NULL;
	if (GetActiveView() == NULL)
	{
		CWnd* pWnd = GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
		{
			pView = (CView*)pWnd;
			SetActiveView(pView, FALSE);
		}
	}
	zDisplay zz(this);
	int p1= zz.pixPerInchX();
	int p2= zz.pixPerInchY();
	int p3 = GetSystemMetrics(SM_CXFULLSCREEN);
	if (bMakeVisible)
	{
		// send initial update to all views (and other controls) in the frame
		SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

		InitSettings();

		// finally, activate the frame
		// (send the default show command unless the main desktop window)
		int nCmdShow = -1;      // default
		CWinApp* pApp = AfxGetApp();
		if (pApp != NULL && pApp->m_pMainWnd == this)
		{
			nCmdShow = pApp->m_nCmdShow; // use the parameter from WinMain
			pApp->m_nCmdShow = -1; // set to default after first time
		}
		ActivateFrame(nCmdShow);
		if (pView != NULL)
			SetActiveView(pView);
//			pView->OnActivateView(TRUE, pView, pView);
	}

	// update frame counts and frame title (may already have been visible)
	if (pDoc != NULL)
		pDoc->UpdateFrameCounts();
	OnUpdateFrameTitle(TRUE);
}

void MainWindow::OnNewCatalog()
{
	CMDIChildWnd *wnd = MDIGetActive();
	String sPath;
	if (0 != wnd)
	{
		CatalogDocument *doc = dynamic_cast<CatalogDocument *> (wnd->GetActiveDocument());
		if (doc)
			sPath = doc->GetPathName();
	}
	else
	{
		IlwisSettings settings("DefaultSettings");
		sPath = settings.sValue("DefaultStartUpDir", "");
	}

	if ("" == sPath)
		sPath = IlwWinApp()->sGetCurDir();

	CSBDestination sb(m_hWnd, "");
	sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
	sb.SetInitialSelection(sPath.c_str());
	if (!sb.SelectFolder())
		return;
	sPath = sb.GetSelectedFolder();

	String sCmd("opendir %S", sPath);
	IlwWinApp()->Execute(sCmd);	
}

void MainWindow::OnStatusLine()
{
	if (status.IsWindowVisible())
		ShowControlBar(&status,FALSE,FALSE);
	else
		ShowControlBar(&status,TRUE,FALSE);
}

void MainWindow::OnUpdateStatusLine(CCmdUI* pCmdUI)
{
	bool fCheck = status.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

int MainWindow::iBeginOwnerQueries()
{
	return iStartOwnerQueries;
}

void MainWindow::OnCreateObject(UINT iID)
{
	String sType;
	switch(iID)
	{
		case ID_CREATEPNTMAP:
			sType = "pnt"; break; 
		case ID_CREATESEGMAP:
			sType = "seg"; break; 
		case ID_CREATERASMAP:
			sType = "map"; break; 
		case ID_CREATEMPL:
			sType = "mpl"; break; 
		case ID_CREATETBL:
			sType = "tbl"; break; 
		case ID_CREATE2DIMTBL:
			sType = "tb2"; break; 
		case ID_CREATEDOM:
			sType = "dom"; break; 
		case ID_CREATERPR:
			sType = "rpr"; break; 
		case ID_CREATEGRF:
			sType = "grf"; break; 
		case ID_CREATECSY:
			sType = "csy"; break; 
		case ID_CREATESMS:
			sType = "sms"; break; 
		case ID_CREATEFIL:
			sType = "fil"; break; 
		case ID_CREATEFUN:
			sType = "fun"; break; 
		case ID_CREATEISL:
			sType = "isl"; break; 
		case ID_CREATEILO:
			sType = "lay"; break;
		case ID_CREATEIOC:
			sType = "ioc"; break;
	}
	IlwWinApp()->Execute(String("create %S", sType));
}

void MainWindow::OnGraph()
{
	IlwWinApp()->Execute("graph");
}

void MainWindow::OnRoseDiagram()
{
	IlwWinApp()->Execute("rosediagram");
}

void MainWindow::OnRestoreAll()
{
	IlwWinApp()->OnRestoreAll();
}

void MainWindow::OnMinimizeAll()
{
	IlwWinApp()->OnMinimizeAll();
}

void MainWindow::UpdateCatalogs(const FileName& fnPath)
{
	if (!fAllowCatalogUpdates)
		return;

	CWnd *wndcur = NULL;
	CMDIChildWnd *mdicur, *mdifirst;
	mdicur = mdifirst = MDIGetActive();
	if (0 == mdicur)
		return;
	
	do 
	{
		CDocument* doc = mdicur->GetActiveDocument();
		CatalogDocument* catdoc = dynamic_cast<CatalogDocument*>(doc);
		if (0 != catdoc)
		{
			bool fUpdate = true;
			if (fnPath != FileName())
			{
				String sDir = catdoc->sGetPathName();
				FileName fnCur(sDir);
				if (fnCur.sPath().toLower() != fnPath.sPath().toLower())
					fUpdate = false;
			}
			if (fUpdate)
				catdoc->UpdateAllViews(0);
		}
		wndcur = mdicur->GetNextWindow();
		while (wndcur && wndcur != mdifirst)
		{
			mdicur = dynamic_cast<CMDIChildWnd*>(wndcur);
			if (mdicur)
				break;
			else
				wndcur = wndcur->GetNextWindow();
		}
	} while (wndcur && mdicur != mdifirst);
}

void MainWindow::OnSize( UINT nType, int cx, int cy )
{
	CMDIFrameWnd::OnSize(nType, cx, cy);
	if ( status.GetSafeHwnd() )
	{
		CRect rect;

		status.GetClientRect(&rect);
		int widths[spANY] = { 400, rect.right - 100, rect.right-80, rect.right-60, rect.right-40, rect.right-20, -1 };

		status.GetStatusBarCtrl().SetParts(spANY, widths);
	}
}

void MainWindow::OnExit()
{
	IlwWinApp()->Execute("exit");
}

void MainWindow::LoadState(IlwisSettings& ilwSettings)
{
	String sBarKey = String("%S\\ToolBars\\Bars", ilwSettings.sIlwisSubKey());
	LoadBarState(sBarKey.c_str());

	WINDOWPLACEMENT wpl;
	wpl.length = sizeof(WINDOWPLACEMENT);
 	GetWindowPlacement(&wpl);
	WINDOWPLACEMENT wplDefault = wpl;
	wpl = ilwSettings.GetWindowPlacement("WindowPosition", wplDefault);
	
	avBar.LoadSettings(ilwSettings);
	String sDataObjectPos("DefaultSettings\\DataObjectPos");
	IlwisSettings settings(sDataObjectPos);

	MinMax tabPos;
	int iMaxW = 2000; //dc.GetDeviceCaps(HORZRES);
	int iMaxH = 2000; //dc.GetDeviceCaps(VERTRES);
	tabPos.rcMin.Col = settings.iValue("X3");
	tabPos.rcMin.Row = settings.iValue("Y3");
	tabPos.rcMax.Col = settings.iValue("XSize3");
	tabPos.rcMax.Row = settings.iValue("YSize3");
	if ( tabPos.rcMin.Col > 0 && tabPos.rcMin.Col < iMaxW)
		wpl.rcNormalPosition.left = tabPos.rcMin.Col;
	if ( tabPos.rcMin.Row > 0 && tabPos.rcMin.Row < iMaxH)
		wpl.rcNormalPosition.top = tabPos.rcMin.Row;
	if ( tabPos.rcMax.Col > 100 && tabPos.rcMax.Col < iMaxW)
		wpl.rcNormalPosition.right = wpl.rcNormalPosition.left + tabPos.rcMax.Col;
  if ( tabPos.rcMax.Row > 100 && tabPos.rcMax.Row < iMaxH)
		wpl.rcNormalPosition.bottom = wpl.rcNormalPosition.top + tabPos.rcMax.Row;
	
	SetWindowPlacement(&wpl);
//	if (wpl.showCmd != SW_SHOWMAXIMIZED && wpl.showCmd != SW_SHOWMINIMIZED)
//		CheckMove();
}

void MainWindow::SaveState(IlwisSettings& ilwSettings)
{
	String sBarKey = String("%S\\ToolBars\\Bars", ilwSettings.sIlwisSubKey());
	SaveBarState(sBarKey.c_str());

  avBar.SaveSettings(ilwSettings);

	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);

 	ilwSettings.SetWindowPlacement("WindowPosition", wpl);
}

void MainWindow::InitSettings()
{
	SendMessageToDescendants(ILWM_VIEWSETTINGS, LOADSETTINGS, 0, TRUE, TRUE);

	IlwisSettings settings(sBaseRegKey());
	LoadState(settings);
}


String MainWindow::sBaseRegKey()
{
	return "MainWindow";
}

void MainWindow::AddToHistory(const String& sDir)
{
	avBar.AddToHistory(sDir);
}

void MainWindow::OnCloseCatalogs()
{

	CMDIChildWnd* wndcur = MDIGetActive();
	if ( !wndcur ) return;
	while(wndcur != NULL )
	{
		wndcur->SendMessage(WM_CLOSE);
		MDINext();
		wndcur = MDIGetActive();
	} 
}

void MainWindow::OnExport()
{
	IlwWinApp()->Execute("export");
}

void MainWindow::OnImportMap()
{
	IlwWinApp()->Execute("import");
}

void MainWindow::OnImportTable()
{
	IlwWinApp()->Execute("import -tablewizard");
}

void MainWindow::OnImportGdal()
{
	IlwWinApp()->Execute("import -gdal");
}

void MainWindow::OnImportGeoGateway()
{
	IlwWinApp()->Execute("import -geogateway");
}

void MainWindow::OnImportGenRaster()
{
	IlwWinApp()->Execute("import -generalraster");
}

void MainWindow::OnCatRedraw()
{
	UpdateCatalogs();
	RefreshNavigator();
}

void MainWindow::OnOpenMap()
{
	IlwWinApp()->Execute("open");
}

void MainWindow::OnOpenAsTable()
{
	IlwWinApp()->Execute("showastbl");
}

BOOL MainWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if ((nHitTest == HTCLIENT) && (IlwWinApp()->iOpeningDoc > 0)) 
	{
    SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
		return TRUE;
	}

	return CMDIFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

void MainWindow::OnPreferences()
{
	FormGeneralPreferences frm;
}

// copied from FrameWindow
void MainWindow::SetAcceleratorTable()
{
	if (m_hAccelTable) 
		DestroyAcceleratorTable(m_hAccelTable);
	m_hAccelTable = 0;
	CMenu* men = GetMenu();
	vector<ACCEL> va;
	// add accelerators mentioned in menu
	AddAccel(men, va);

	// add standard accelerators
	ACCEL acc;
	acc.cmd = ID_EDIT_COPY; // Ctrl-Ins
	acc.key = VK_INSERT;
	acc.fVirt = FVIRTKEY | FCONTROL;
	va.push_back(acc);
	acc.cmd = ID_EDIT_PASTE;	// Shft-Ins
	acc.key = VK_INSERT;
	acc.fVirt = FVIRTKEY | FSHIFT;
	va.push_back(acc);
	acc.cmd = ID_EDIT_CUT;	// Shft-Del
	acc.key = VK_DELETE;
	acc.fVirt = FVIRTKEY | FSHIFT;
	va.push_back(acc);
//	acc.cmd = ID_CLEAR;	// Del
//	acc.key = VK_DELETE;
//	acc.fVirt = FVIRTKEY;
//	va.push_back(acc);

	acc.cmd = ID_HELP;	// F1
	acc.key = VK_F1;
	acc.fVirt = FVIRTKEY;
	va.push_back(acc);
	acc.cmd = ID_REDRAW;	// F5
	acc.key = VK_F5;
	acc.fVirt = FVIRTKEY;
	va.push_back(acc);
	acc.cmd = ID_HLPCONTENTS;	// F11
	acc.key = VK_F11;
	acc.fVirt = FVIRTKEY;
	va.push_back(acc);
	acc.key = VK_F1;
	acc.fVirt = FVIRTKEY | FSHIFT;
	va.push_back(acc);

	m_hAccelTable = CreateAcceleratorTable(&*va.begin(), va.size());
}

BOOL MainWindow::PreTranslateMessage(MSG* pMsg)
{
	CString str;
  if (pMsg->message == WM_KEYDOWN)
	{
		if (!fHandleAccelerator)
			return FALSE;
	}
  return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

SimpleCalcResultForm *MainWindow::frmSimpleCalcResults()
{
	if (0 != frmSimpleCalc && !IsWindow(frmSimpleCalc->m_hWnd)) // will happen if closed by user
		frmSimpleCalc = 0;
	if (0 == frmSimpleCalc) 
		frmSimpleCalc = new SimpleCalcResultForm(this);
	return frmSimpleCalc;
}


void MainWindow::RefreshNavigator()
{
	avBar.Refresh();
}

void MainWindow::SetStatusLine(const String& sTxt, MainWindow::spStatusPanes iIndex)
{
	status.SetPaneText(iIndex, sTxt.c_str())	;
}

void MainWindow::OnSetFocus(CWnd* pWndOld)
{
	if (::IsWindow(commBar.m_hWnd) && commBar.IsWindowEnabled() && commBar.IsWindowVisible())	
		commBar.Activate();
}

void MainWindow::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	if (WA_ACTIVE == nState || WA_CLICKACTIVE == nState)
	{
		if (::IsWindow(commBar.m_hWnd) && commBar.IsWindowEnabled() && commBar.IsWindowVisible())
			commBar.Activate();
	}		
}

LRESULT MainWindow::OnOpenCatalog(WPARAM iW, LPARAM iL)
{
	Directory *dir = (Directory *) iW;
	if ( dir )
		IlwWinApp()->OpenNewDirectory( dir->sFullPath().c_str() );

	return 1;
}

LRESULT MainWindow::OnReadCatalog(WPARAM iWP, LPARAM iLP)
{
	if (iWP == WP_STOPREADING)
		fAllowCatalogUpdates = false;
	else if (iWP == WP_RESUMEREADING)
		fAllowCatalogUpdates = true;

	FileName fn;
	if (0 != iLP)
	{
		FileName* pfn = reinterpret_cast<FileName*>(iLP);
		fn = *pfn;
		delete pfn; // this pointer is maded outside this function and must be deleted here
	}			
	if (fAllowCatalogUpdates)
	{
		UpdateCatalogs(fn);
	}

	return 1;
}

LRESULT MainWindow::OnCloseCollection(WPARAM iFileName, LPARAM iL)
{
	FileName fnCollection;
	if (iFileName != 0)
	{
		fnCollection = *(FileName *)iFileName;
		delete (FileName *)iFileName; // this pointer is made outside this function and must be deleted here
	}
	if (!fnCollection.fExist())
		return 1;
	CMDIChildWnd* wndfirst = MDIGetActive();
	if ( wndfirst )
	{
		CMDIChildWnd* wndcur = wndfirst; 
		do 
		{
			if (wndfirst == 0) // by removing it has become invalid, get a new 'first' window
				wndfirst = wndcur;
			
			CDocument *doc = wndcur->GetActiveDocument();
			ObjectCollectionDoc *ocDoc = dynamic_cast<ObjectCollectionDoc *>(doc);
			if ( ocDoc)
			{
				FileName fnDoc(ocDoc->obj()->fnObj );
				if ( fnDoc == fnCollection)
				{
					ocDoc->OnCloseDocument();
					wndfirst = 0; // has become invalid;
				}					
			}				
			MDINext();
			wndcur = MDIGetActive();
		} while(wndcur != wndfirst );
	}
	return 1;
}

void MainWindow::OnImport()   
{
	GeneralImportForm *frm = new GeneralImportForm(this);
}

#ifdef _DEBUG
void MainWindow::AssertValid() const
{
	// do nothing
}

void MainWindow::Dump(CDumpContext& dc) const
{
	// do nothing
}

#endif //_DEBUG

