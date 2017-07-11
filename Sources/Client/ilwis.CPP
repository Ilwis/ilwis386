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
/* IlwisWinApp 
main application source file
$Header: /ILWIS 3.0/Main/ilwis.CPP 329   10-06-05 15:09 Retsios $
*/
#pragma warning( disable : 4786 )

#define ILWIS_C
#include "Client\Base\ilwis30.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\MainWindow\ACTION.H"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\FormElements\fldcs.h"
#include "Headers\constant.h"
#include "Client\Help\HelpFinder.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\Editors\Digitizer\DIGITIZR.H"
#include "Client\MainWindow\mainwind.h"
#include "Headers\messages.h"
//#include "Engine\Drawers\DrawerContext.h"
#include "Headers\constant.h"
#include "Client\FormElements\formbase.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Base\BaseDocTemplate.h"
#include "Client\Base\IlwisDocTemplate.h"
#include "Client\Base\IlwisMDIDocTemplate.h"
#include "Client\Base\ForeignDocTemplate.h"
#include "Client\Base\IlwisExpressionDocTemplate.h"
#include "Client\Base\datawind.h"
#include "Client\Base\DataFormWindow.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Client\Editors\Domain\classItem.h"
#include "Client\Base\BaseView.h"
#include "Client\Base\BaseView2.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\Domain\DomDoc.h"
#include "Client\Editors\Domain\DomainSortDoc.h"
#include "Client\Forms\generalformview.h"
#include "Client\Editors\Domain\DomainWindow.h"
#include "Client\Editors\Domain\DomainView.h"
#include "Client\Editors\Domain\DomainBoolView.h"
#include "Client\Editors\Domain\DomainValueFormView.h"
#include "Client\Editors\Domain\DomainValueIntFormView.h"
#include "Client\Editors\Domain\DomainUniqueIDFormView.h"
#include "Client\Editors\Domain\DomainTimeView.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Representation\Rprgrad.h"
#include "Client\Editors\Utils\colorScroll.h"
#include "Client\Editors\Utils\ColorCB.h"
#include "Client\Editors\Utils\ColorGrid.h"
#include "Client\Editors\Utils\colorIntensity.h"
#include "Client\Editors\Representation\ColorGridBar.h"
#include "Client\Editors\Representation\ColorIntensityBar.h"
#include "Client\Editors\Representation\RepresentationDoc.h"
#include "Client\Editors\Representation\RepresentationView.h"
#include "Client\Editors\Representation\RepresentationValueView.h"
#include "Client\Editors\Representation\RepresentationWindow.h"
#include "Client\Editors\Representation\RepresentationClassWindow.h"
#include "Client\Editors\Representation\RprClassLB.h"
#include "Client\Editors\Representation\RepresentationClassView.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Client\Editors\CoordSystem\CSyDoc.h"
#include "Client\Editors\CoordSystem\CsyView.h"
#include "Client\Editors\CoordSystem\CsyBoundsOnlyView.h"
#include "Client\Editors\CoordSystem\CsyViaLatLonView.h"
#include "Client\Editors\CoordSystem\CsyLatLonView.h"
#include "Client\Editors\CoordSystem\CsyProjectionView.h"
#include "Client\Editors\CoordSystem\CsyFormulaView.h"
#include "Engine\Scripting\Script.h"
#include "Engine\Function\FUNCUSER.H"
#include "Client\Editors\TextDoc.h"
#include "Client\Editors\FuncUserDoc.h"
#include "Client\Editors\ScriptDoc.h"
#include "Client\Editors\TextView.h"
#include "Client\Editors\TextWindow.h"
#include "Client\Editors\FuncUserView.h"
#include "Client\Editors\ScriptWindow.h"
#include "Client\Editors\FunctionWindow.h"
#include "Engine\SpatialReference\GRMRROT.H"
#include "Client\Editors\Georef\GeoRefDoc.h"
#include "Client\Editors\Georef\GeoRefGenerDoc.h"
#include "Client\Editors\Georef\GrCornersEdit.h"
#include "Client\Editors\Georef\grSimpleEdit.h"
#include "Client\Editors\Georef\GrGenerEdit.h"
#include "Client\Editors\Georef\GrFactorEdit.h"
#include "Client\Editors\Georef\GrSubmapEdit.h"
#include "Client\Editors\Georef\grsubcoordedit.h"
#include "Client\Editors\Georef\grsubcornersedit.h"
#include "Client\Editors\Georef\GrMirRotEdit.h"
//#include "Engine\Drawers\SelectionRectangle.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Forms\AboutForm.h"
//#include "Client\Mapwindow\MapCompositionInPlaceFrame.h"
#include "Client\TableWindow\TableCommandHandler.h"
#include "Client\TableWindow\RecordBar.h"
#include "Client\TableWindow\TableWindow.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\HistogramDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Table\tbl2dim.h"
#include "Client\TableWindow\Table2Window.h"
#include "Client\TableWindow\Table2Doc.h"
#include "Client\TableWindow\Table2PaneView.h"
#include "Client\TableWindow\HistogramWindow.h"
#include "Client\TableWindow\GraphWindow.h"
#include "Client\TableWindow\CartesianGraphDoc.h"
#include "Client\TableWindow\RoseDiagramDoc.h"
#include "Client\TableWindow\GraphPaneView.h"
#include "Client\Base\WinThread.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Mapwindow\PixelInfoWindow.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Client\Mapwindow\PixelInfoView.h"
#include "Client\MainWindow\Catalog\CatalogFrameWindow.h"
#include <afxdisp.h>
#include "Client\MainWindow\ACTION.H"
#include "Client\MainWindow\ASSOCSEL.H"
#include "Client\MainWindow\ACTPAIR.H"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Stereoscopy\StereoPair.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\Catalog\CollectionCatalog.h"
#include "Client\MainWindow\Catalog\ObjectCollectionWindow.h"
#include "Client\MainWindow\Catalog\MapListDoc.h"
#include "Client\MainWindow\Catalog\MapListWindow.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Client\MainWindow\Catalog\ForeignCollectionDoc.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Client\MainWindow\Catalog\DataBaseCatalog.h"
#include "Client\MainWindow\Catalog\DataBaseCollectionDoc.h"
#include "Client\MainWindow\Catalog\DataBaseCollectionWindow.h"
#include "Client\MainWindow\Catalog\PostgresDataBaseCatalog.h"
#include "Client\MainWindow\Catalog\ForeignCatalog.h"
#include "Engine\Matrix\Matrxobj.h"
#include "Client\Editors\MatrixDoc.h"
#include "Client\Editors\MatrixView.h"
#include "Client\Editors\MatrixWindow.h"
#include "Engine\Function\FILTER.H"
#include "Client\Editors\FilterDoc.h"
#include "Client\Editors\LinearFilterDoc.h"
#include "Client\Editors\LinearFilterView.h"
#include "Client\Editors\LinearFilterWindow.h"
#include "Headers\Htp\Ilwis.htp"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"
#include "Client\Editors\Layout\LayoutWindow.h"
#include "Client\Editors\Layout\LayoutView.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Client\Editors\SMCE\SMCEWindow.h"
#include "Client\Editors\SMCE\CriteriaTreeView.h"
#include "Client\Editors\SMCE\CriteriaTreeDoc.h"
#include "Client\Editors\Map\BaseMapEditor.h"
#include "Client\Base\DdeServerThread.h"
#include "Client\Base\IlwisDocManager.h"
#include "Client\MainWindow\AboutDlg.h"
#include "Headers\version.h"
#include "Engine\COMInterface\ComServer.h"
#include "Engine\COMInterface\ComRegistration.h"
#include "Engine\COMInterface\ICommandLine.h"
#include "Engine\COMInterface\COMCmdLineThread.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Applications\objvirt.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Base\System\Engine.h"
#include "MainWindow\Catalog\WMSCatalog.h"
#include "Engine\DataExchange\WMSCollection.h"
#include "MainWindow\Catalog\WMSCollectionDoc.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Help\HelpSearch.h"
#include <gdiplus.h>
#include "Headers\Hs\Mainwind.hs"

IlwisWinApp TheIlwisApp;
const CLSID CLSID_ILWIS_CommandLine = {0x524BCEF4,0x4370,0x4021,{0x92,0xD3,0xCE,0xC1,0x62,0x57,0x7C,0x08}};
const String ILWIS::Version::IlwisVersion="3.8.5.0";
//const String ILWIS::Version::IlwisShortName="Ilwis38";

BEGIN_MESSAGE_MAP( IlwisWinApp, CWinApp )
	//{{AFX_MSG_MAP( IlwisWinApp )
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_COMMAND(ID_TEST, OnTest)
	ON_COMMAND(ID_CLOSEALL, OnCloseAll)
	ON_COMMAND(ID_MINIMIZEALL, OnMinimizeAll)
	ON_COMMAND(ID_RESTOREALL, OnRestoreAll)
	ON_THREAD_MESSAGE(ILW_ADDDATAWINDOW, OnAddDataWindow)
	ON_THREAD_MESSAGE(ILW_REMOVEDATAWINDOW, OnRemoveDataWindow)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_HLPCONTENTS, OnHelpContents)
	ON_COMMAND(ID_HLPOPERATIONS, OnHelpOperations)
	ON_COMMAND(ID_HLPMENUCMDS, OnHelpMenuCmds)
	ON_COMMAND(ID_HLPSEARCH, OnHelpSearch)
	ON_COMMAND(ID_HLPINDEX, OnHelpIndex)
	ON_COMMAND(ID_HLPGLOSSARY, OnHelpGlossary)
	ON_COMMAND(ID_HLPHOWTO, OnHelpHowTo)
	ON_COMMAND(ID_HLPMAPTABCALC, OnHelpMapTabCalc)
	ON_COMMAND(ID_HLPILWEXPRES, OnHelpIlwExpressions)
	ON_COMMAND(ID_HLPSEBS, OnHelpSebs)
	ON_COMMAND(ID_HLPSCRIPTSYNTAX, OnHelpScriptSyntax)
	ON_COMMAND(ID_FILE_DIGSETUP, OnDigitizerSetup)
	ON_UPDATE_COMMAND_UI(ID_FILE_DIGSETUP, OnUpdateDigitizerSetup)
	ON_COMMAND(ID_FILE_DIGREF, OnMapReferencing)
	ON_UPDATE_COMMAND_UI(ID_FILE_DIGREF, OnUpdateMapReferencing)
	ON_COMMAND_RANGE(ID_OPERATION0, ID_OPERATION0+499, OnOperationCmd)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// MiscStatus:
// OLEMISC_RECOMPOSEONRESIZE =  1
// OLEMISC_CANLINKBYOLE1     = 32  (default)
static const TCHAR szMiscStatusRecompose[] = _T("CLSID\\%1\\MiscStatus\0") _T("33");

static const LPCTSTR rgServerMapComposition[] =
{
	szMiscStatusRecompose,
	NULL
};

bool IlwisWinApp::fShowHelpNumbers = false;



IlwisWinApp::IlwisWinApp() 
: CWinApp(ILWIS_VERSION_NAME)
, smILWIS(1,1,ILWIS_VERSION_NAME)
, smMainWindow(1,1,String("%sMW",ILWIS_VERSION_NAME).c_str())
, lockMWCreated(&smMainWindow)
, lockILWIS(&smILWIS)
, dirdoc(0)
, dtPixelInfo(0)
, dtDirectory(0)
, dtMapWindow(0)
, dtTableWindow(0)
, dtDomWindow(0)
, dtRprWindow(0)
, dtLayout(0)
, dtGraph(0)
, dtRoseDiagram(0)
, dtForeign(0)
, dtSMCE(0)
, digitizer(0)
, iOpeningDoc(0)
, actList(0)
, actPairList(0)
, thrDdeServer(0)
, fRemoveUserRegistry(false)
,fOldTranquilizers(false),
helpFinder(0)
{
	if (GetPrivateProfileInt("statusline", "numbers", 0, "ilwis.ini"))
		fShowHelpNumbers = true;
	m_fHideLogo = false;
}


IlwisWinApp::~IlwisWinApp()
{
	delete digitizer;
	delete actList;
	delete actPairList;
	delete commandUI;
	ILWISSingleLock sl(&IlwWinApp()->csHelp, TRUE,SOURCE_LOCATION);
	delete helpFinder;
	WEP_Color(); // clean some static allocations
	if ( fRemoveUserRegistry )
	{
		IlwisSettings settings("");
		settings.DeleteKey();
	}		
	Context()->RemoveThreadLocalVars();
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif 
}

void IlwisWinApp::SendCommandLine(CWnd* wnd) const
{
	if (0 == wnd)
		return;
	COPYDATASTRUCT cds;
	cds.dwData = ID_ILWISSTART;
	cds.lpData = (void*)m_sCommand.c_str();
	cds.cbData = (DWORD)strlen(m_lpCmdLine) + 1;
	if (wnd)
		wnd->SendMessage(WM_COPYDATA, 0, (LPARAM)&cds);
}

UINT StartComServer(LPVOID p)
{
	IlwWinApp()->Context()->InitThreadLocalVars();
	UINT iRes = RunComServer(p);
	IlwWinApp()->Context()->RemoveThreadLocalVars();
	return iRes;
}

BOOL IlwisWinApp::InitApplication()
{
	if (!CWinApp::InitApplication())
		return FALSE;
	m_sCommand = "";
		
	getEngine();
	ilwapp = getEngine()->getContext();

	String sParms(m_lpCmdLine);
	short ipos2 = sParms.iPos(String("-D"));
	short ipos = sParms.iPos(String("-C"));

	if ( ipos2 != shUNDEF) {
		if ( (ipos2 < ipos && ipos!=shUNDEF) || ipos == shUNDEF)
		getEngine()->setDebugMode(true);
		lockILWIS.setIdentity(SOURCE_LOCATION);
	}
	if ( ipos == shUNDEF) {
		ipos = sParms.iPos(String("-CR"));
	} else
		getEngine()->setStayResident(false);


	if ( ipos != shUNDEF){
		m_sCommand = sParms.substr(ipos + getEngine()->fStayResident() ? 4 : 3);
		sParms = " -NoLogo " + sParms;
		// -C mode will not give messageboxes anymore
		getEngine()->getContext()->SetThreadLocalVar(IlwisAppContext::tlvSERVERMODE, new bool(true));
	}
	else
		plInitParms.Fill(sParms);

	commandUI = new CommandHandlerUI(NULL);
	commandUI->addModules();


	// Check for COM server command line parameters
	bool fRun = true;
	m_fEmbedded = false;


	// We are already done if ILWIS is already running, just activate
	// the running version
	if (0 == lockILWIS.Lock(0) && getEngine()->fStayResident())
	{
		//log.LogLine("na : " + m_sCommand);
		// ILWIS already running	  ,
		WaitForSingleObject(smMainWindow, 5000); // wait until the main window is created
		CWnd* wnd = CWnd::FindWindow(ILWIS_VERSION_NAME, 0);
		if (wnd)
			SendCommandLine(wnd);
		return FALSE;
	} else
		lockMWCreated.Lock();

	// erase startup parms from string
	m_fHideLogo = sParms.find("-NoLogo") != -1;
	if (m_fHideLogo)
		sParms.replace(sParms.find("-NoLogo"), 7, "");
	if (m_fEmbedded)
		sParms.replace(sParms.find("-Embedding"), 9, "");
	if (m_fHideLogo || m_fEmbedded)
		strcpy(m_lpCmdLine, sParms.c_str());

	return TRUE;
}

BOOL IlwisWinApp::InitInstance()
{
	try
	{
		::OleInitialize(NULL);
		setlocale(LC_TIME, ""); // initialize localization time defaults
		RegisterClasses();
		RegisterDocTemplates();
		InitImageLists();
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		//ParmList *pl;
		//Context()->InitThreadLocalVars();
		//delete pl;

		IlwisSettings ilwregset("", IlwisSettings::pkuUSER);
		SetRegistryKey(ilwregset.sSoftwareSubKey().c_str());
		free((void*)m_pszProfileName);
		m_pszProfileName = _tcsdup("");

		LoadSettings();

		if (!m_fHideLogo && ! m_fEmbedded)
			OpenLogoWindow();

		actList = new ActionList;
		actPairList = new ActionPairList;

		MainWindow* mw = new MainWindow;
		m_pMainWnd = mw;
		if ( m_sCommand == "" || m_sCommand == "visible") {
			mw->SendMessage(WM_SETREDRAW, FALSE, 0);
			mw->InitialUpdate(0, TRUE);
			mw->SendMessage(WM_SETREDRAW, TRUE, 0);
		}else
			mw->ShowWindow(SW_HIDE);

		mw->UpdateWindow();

		commandUI->SetOwner(mw);

		IlwisSettings trqSetting("DefaultSettings");
		fOldTranquilizers = trqSetting.fValue("OldTranquilizers", false);


		EnableDigitizer(); // if configuration is available
		CatalogDocument *doc;
		String sKey = MainWindow::sBaseRegKey();
		IlwisSettings settingsdocs(sKey);
		int iNr = settingsdocs.iValue("NrDocs");
		for ( int iDoc = iNr - 1; iDoc >= 0; --iDoc)
		{
			String sDocName("Doc%d", iDoc);
			String sV("!REGKEY:%S", sDocName);
			IlwisSettings settings(String("%S\\%S", MainWindow::sBaseRegKey(), sDocName));
			String sName = settings.sValue("DocName");
			if (sName.length() > 0) {
				IlwisObject::iotIlwisObjectType iotType = IlwisObject::iotObjectType(FileName(sName));
				try
				{
					switch(iotType)
					{
					case IlwisObject::iotOBJECTCOLLECTION :
						doc = dynamic_cast<CatalogDocument*>(m_pDocManager->OpenDocumentFile(sName.c_str()));
						break;
					case  IlwisObject::iotMAPLIST:
						doc = dynamic_cast<CatalogDocument*>(dtMapList->OpenDocumentFile(sV.c_str()));
						break;
					default:
						doc = dynamic_cast<CatalogDocument*>(OpenNewDirectory(sV.c_str())); 
					}
					if (!doc) // catalog could not be opened; skip it.
					{
						settings.SetValue("DocName", "");
					} else {
						doc->SetRegistryName(sDocName);
						if (dirdoc == NULL)
							SetCatalogDocument(doc);
					}
				}
				catch (ErrorObject& err) 
				{
					err.Show();
					settings.SetValue("DocName", "");
				}
			}
		}

		if ( iNr <=  0 )
			dirdoc = dynamic_cast<CatalogDocument*>(OpenNewDirectory(sGetCurDir().c_str()));
		Context()->ComHandler()->SetOwner(mw);
		thrDdeServer = new DdeServerThread();
		thrDdeServer->CreateThread(0, 0);

		CloseLogoWindow();
		lockMWCreated.Unlock();
		commandUI->initModules();

		if ( m_sCommand != "") {
			if ( m_sCommand.sHead(" ") != "open") {
				Script::Exec(m_sCommand);
				if ( getEngine()->fStayResident() == false) {
					Execute("exit");
					return FALSE;
				}
			} else {
				String command = m_sCommand + " -exitOnClose";
				getEngine()->Execute(command);
			}
		}
		helpFinder = new ILWIS::HelpFinder();
		if ( m_sCommand == "") {
			//::MessageBox(0,"Start index", "help", MB_OK);
			helpFinder->indexHTMLFiles();
		}
		return TRUE;
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}
	catch(CException* err)
	{
		MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
		err->Delete();
	}
	return FALSE;
}

void IlwisWinApp::LoadSettings(const String& sSettingName)
{
	IlwisSettings prop(sSettingName);

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = 90;
	logFont.lfWeight = FW_NORMAL;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS | FF_ROMAN | FF_MODERN | FF_SCRIPT;
	lstrcpy(logFont.lfFaceName, "MS Sans Serif");

	CFont *fnt = new CFont();
	if ( prop.fKeyExists("FormFont"))
		prop.GetFont("FormFont", fnt);
	else // default;
		fnt->CreatePointFontIndirect(&logFont);

	StandardFonts[sfFORM] = fnt;

	fnt = new CFont();
	if ( prop.fKeyExists("WindowsFont") )
		prop.GetFont("WindowsFont", fnt);
	else //default'
	{
		logFont.lfHeight = 80;
		fnt->CreatePointFontIndirect(&logFont);
	}
	StandardFonts[sfWindowMedium] = fnt;

	fnt = new CFont();
	if ( prop.fKeyExists("TableFont") )
		prop.GetFont("TableFont", fnt);
	else // default
	{
		logFont.lfHeight = 100;
		logFont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS | FF_ROMAN | FF_MODERN | FF_SCRIPT;
		lstrcpy(logFont.lfFaceName, "Courier New");
		fnt->CreatePointFontIndirect(&logFont);
	}
	StandardFonts[sfTABLE] = fnt;

	fnt = new CFont();
	if ( prop.fKeyExists("GraphFont") )
		prop.GetFont("GraphFont", fnt);
	else // default
	{
		logFont.lfHeight = 100;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS | FF_ROMAN | FF_MODERN | FF_SCRIPT;
		lstrcpy(logFont.lfFaceName, "Arial");
		fnt->CreatePointFontIndirect(&logFont);
	}
	StandardFonts[sfGRAPH] = fnt;

	if ( prop.fKeyExists("SearchPaths") )
	{
		String sPath;
		String sKey("SearchPaths\\SearchPath0");
		int i=0;
		while( prop.fKeyExists(sKey) )
		{
			prop.sValue(sKey, sPath);
			if ( _access(sPath.c_str(), 0) != -1)
			{
				AddSearchPath( String("%S\\",sPath));
				sKey = String("SearchPaths\\SearchPath%d", i);
			}
		}
	}
	getEngine()->getContext()->AddBasemapPath(FileName(getEngine()->getContext()->sStdDir() + "\\Basemaps\\"));
	if ( Context()->fnGetSearchPath(0) == FileName())
		Context()->AddSearchPath(String("%S\\",getEngine()->getContext()->sStdDir()));

	IlwisSettings settings("DefaultSettings");
	String sPath = settings.sValue("DefaultStartUpDir", "");
	if ("" != sPath) 
		SetCurDir(sPath);
}

void IlwisWinApp::SaveSettings(const String& sSettingName)
{
	IlwisSettings prop(sSettingName);
	prop.SetFont("FormFont", StandardFonts[sfFORM]);
	prop.SetFont("WindowsFont", StandardFonts[sfWindowMedium]);
	prop.SetFont("TableFont", StandardFonts[sfTABLE]);
	prop.SetFont("GraphFont", StandardFonts[sfGRAPH]);
	IlwisSettings prop2(String("%S\\SearchPaths", sSettingName));
	int i = 0;
	while(Context()->fnGetSearchPath(i) != FileName() ) {
		prop2.SetValue(String("SearchPaths%d", i), Context()->fnGetSearchPath(i).sPath());
		++i;
	}

}

void IlwisWinApp::DisableDigitizer()
{
	delete digitizer;
	digitizer = 0;
}

void IlwisWinApp::EnableDigitizer(bool fConfigure)
{
	if ( fConfigure )
	{
		if ( digitizer == 0 )
			digitizer = new Digitizer(m_pMainWnd);
		digitizer->Configure(0);
	}
	if ( IlwisSettings("Digitizer", IlwisSettings::pkuMACHINE, false, IlwisSettings::omREADONLY).fKeyExists() )
	{
		if ( digitizer == 0) 
			digitizer = new Digitizer(m_pMainWnd);
	}
}

inline void delfnt(const pair<IlwisWinApp::sfFontType, CFont*>& pr) 
{
	CFont* fnt = pr.second;
	delete fnt;
}

int IlwisWinApp::ExitInstance()
{
	TRACE0("TheApp::ExitInstance\n");

	if (thrDdeServer)
	{
		HANDLE hThread = *thrDdeServer;
		thrDdeServer->PostThreadMessage(WM_QUIT, 0, 0);
		// Bas Retsios, 3 Jan 2001
		// Hang here until DDE thread has finished cleaning up (static variables that are used 
		// there are being cleaned up here .. unfortunately we have to wait before cleaning them up)
		// Wait until DDE thread terminates or 4 seconds (to avoid waiting forever), whichever comes 
		// first. 4 seconds should be sufficient. However, if the timeout is reached, something bad 
		// happened and the cleanup of the dde thread was only partial.
		WaitForSingleObject(hThread, INFINITE); 
		// Note!!! if you need to debug the DDE part, replace temporarily '4000' with 'INFINITE' (time goes on for the debugger while you're looking at the screen).

		// Now do the same for COM command line thread, no need for cleanup, so maximum 1 second delay is used
		delete chCOMServerHandler;
	}

	SaveSettings();

	// remove the default fonts
	for_each(StandardFonts.begin(), StandardFonts.end(), delfnt);
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	::OleUninitialize();

	return 0;
}

void IlwisWinApp::OnAbout()
{

	new AboutForm(m_pMainWnd);
}

LRESULT IlwisWinApp::Execute(const String& sCmd)
{
	return Context()->Execute(sCmd);
}

void IlwisWinApp::RegisterDocTemplates()
{
	m_pDocManager = new IlwisDocManager;

	BaseDocTemplate* idt;

	// Domain
	idt = new IlwisDocTemplate(
		".dom", 
		"DomainGroup",
		"ILWIS Domain",
		RUNTIME_CLASS(DomainGroupDoc),
		RUNTIME_CLASS(DomainWindow),
		RUNTIME_CLASS(DomainGroupView));
	AddDocTemplate(idt);

	dtDomUnique = new IlwisDocTemplate(
		".dom", 
		"DomainUniqueID",
		"ILWIS Domain",
		RUNTIME_CLASS(DomainDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(DomainUniqueIDFormView));
	AddDocTemplate(dtDomUnique);

	dtDomWindow = new IlwisDocTemplate(
		".dom", 
		"DomainClass,DomainIdentifier,DomainSort",
		"ILWIS Domain",
		RUNTIME_CLASS(DomainSortDoc),
		RUNTIME_CLASS(DomainWindow),
		RUNTIME_CLASS(DomainSortView));
	AddDocTemplate(dtDomWindow);

	idt = new IlwisDocTemplate(
		".dom", 
		"DomainBool",
		"ILWIS Domain",
		RUNTIME_CLASS(DomainDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(DomainBoolView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".dom", 
		"DomainValueInt",
		"ILWIS Domain",
		RUNTIME_CLASS(DomainDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(DomainValueIntFormView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".dom", 
		"DomainTime",
		"ILWIS Domain",
		RUNTIME_CLASS(DomainDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(DomainTimeView));
	AddDocTemplate(idt);


	idt = new IlwisDocTemplate(
		".dom", 
		"DomainValue",
		"ILWIS Domains",
		RUNTIME_CLASS(DomainDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(DomainValueFormView));
	AddDocTemplate(idt);

	// catch all other domain types: only typename, description and optionally system object
	idt = new IlwisDocTemplate(
		".dom", 
		"",
		"ILWIS Domains",
		RUNTIME_CLASS(DomainDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(GeneralFormView));
	AddDocTemplate(idt);

	dtFunctionWindow = new IlwisDocTemplate(
		".fun", 
		"FuncUser",
		"ILWIS Functions",
		RUNTIME_CLASS(FuncUserDoc),
		RUNTIME_CLASS(FunctionWindow),
		RUNTIME_CLASS(FuncUserView));
	AddDocTemplate(dtFunctionWindow);

	dtScriptWindow = new IlwisDocTemplate(
		".isl", 
		"Script",
		"ILWIS Scripts",
		RUNTIME_CLASS(ScriptDoc),
		RUNTIME_CLASS(ScriptWindow),
		RUNTIME_CLASS(TextView));
	AddDocTemplate(dtScriptWindow);

	// GeoReferences
	idt = new IlwisDocTemplate(
		".grf", 
		"GeoRefCorners",
		"ILWIS Georeferences",
		RUNTIME_CLASS(GeoRefDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(GeoRefCornersView));
	AddDocTemplate(idt); 

	idt = new IlwisDocTemplate(
		".grf", 
		"GeoRefSmpl",
		"ILWIS Georeferences",
		RUNTIME_CLASS(GeoRefDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(GeoRefSimpleView));
	AddDocTemplate(idt); 

	idt = new IlwisDocTemplate(
		".grf", 
		"GeoRefFactor",
		"ILWIS Georeferences",
		RUNTIME_CLASS(GeoRefGeneratedDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(GeoRefFactorView));
	AddDocTemplate(idt); 

	idt = new IlwisDocTemplate(
		".grf", 
		"GeoRefSubMapCoords",
		"ILWIS Georeferences",
		RUNTIME_CLASS(GeoRefGeneratedDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(GeoRefSubMapCoordView));
	AddDocTemplate(idt); 

	idt = new IlwisDocTemplate(
		".grf", 
		"GeoRefSubMapCorners",
		"ILWIS Georeferences",
		RUNTIME_CLASS(GeoRefGeneratedDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(GeoRefSubMapCornersView));
	AddDocTemplate(idt); 

	idt = new IlwisDocTemplate(
		".grf", 
		"GeoRefSubMap",
		"ILWIS Georeferences",
		RUNTIME_CLASS(GeoRefGeneratedDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(GeoRefSubmapView));
	AddDocTemplate(idt); 

	idt = new IlwisDocTemplate(
		".grf", 
		"GeoRefMirrorRotate",
		"ILWIS Georeferences",
		RUNTIME_CLASS(GeoRefGeneratedDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(GeoRefMirrorRotateView));
	AddDocTemplate(idt); 

	// Representation
	dtRprWindow = new IlwisDocTemplate(
		".rpr", 
		"RepresentationClass",
		"ILWIS Representations",
		RUNTIME_CLASS(RepresentationClassDoc),
		RUNTIME_CLASS(RepresentationClassWindow),
		RUNTIME_CLASS(RepresentationClassView));
	AddDocTemplate(dtRprWindow); 	

	idt = new IlwisDocTemplate(
		".rpr", 
		"RepresentationValue,RepresentationGradual",
		"ILWIS Representations",
		RUNTIME_CLASS(RepresentationValueDoc),
		RUNTIME_CLASS(RepresentationWindow),
		RUNTIME_CLASS(RepresentationValueView));
	AddDocTemplate(idt);

	// MapList

	dtMapList = new IlwisMDIDocTemplate(
		".mpl", 
		"MapList", 
		"ILWIS Maplists",
		RUNTIME_CLASS(MapListDoc),
		RUNTIME_CLASS(MapListWindow),
		RUNTIME_CLASS(Catalog));
	AddDocTemplate(dtMapList);

	dtAnimation = new IlwisDocTemplate(
		".mpl,ioc", 
		"MapList,ObjectCollection", 
		"ILWIS Animation",
		RUNTIME_CLASS(MapCompositionDoc),
		RUNTIME_CLASS(MapWindow),
		RUNTIME_CLASS(MapPaneView));
	AddDocTemplate(dtAnimation);

	idt = new IlwisDocTemplate(
		".mpl", 
		"MapList", 
		"Color Composite",
		RUNTIME_CLASS(MapCompositionDoc),
		RUNTIME_CLASS(MapWindow),
		RUNTIME_CLASS(MapPaneView));
	AddDocTemplate(idt);

	dtObjectCollection = new IlwisMDIDocTemplate(
		".ioc", 
		"ObjectCollection", 
		"ILWIS ObjectCollections",
		RUNTIME_CLASS(ObjectCollectionDoc),
		RUNTIME_CLASS(ObjectCollectionWindow),
		RUNTIME_CLASS(CollectionCatalog));
	AddDocTemplate(dtObjectCollection);

	idt = new IlwisDocTemplate(
		".ioc", 
		"ObjectCollection", 
		"ILWIS CollectionLayer",
		RUNTIME_CLASS(MapCompositionDoc),
		RUNTIME_CLASS(MapWindow),
		RUNTIME_CLASS(MapPaneView));
	AddDocTemplate(idt);

	dtObjectCollection = new ForeignMultiDocTemplate(
		"DataBaseCollection", 
		"ILWIS DataBase Collections",
		RUNTIME_CLASS(DataBaseCollectionDoc),
		RUNTIME_CLASS(DataBaseCollectionWindow),
		RUNTIME_CLASS(DataBaseCatalog));
	AddDocTemplate(dtObjectCollection);	

	dtObjectCollection = new ForeignMultiDocTemplate(
		"DataBaseCollection", 
		"ILWIS Postgres DataBase Collections",
		RUNTIME_CLASS(DataBaseCollectionDoc),
		RUNTIME_CLASS(DataBaseCollectionWindow),
		RUNTIME_CLASS(PostgresDataBaseCatalog));
	AddDocTemplate(dtObjectCollection);	

	dtObjectCollection = new ForeignMultiDocTemplate(
		"WMSCollection", 
		"ILWIS WMSCollections",
		RUNTIME_CLASS(WMSCollectionDoc),
		RUNTIME_CLASS(ObjectCollectionWindow),
		RUNTIME_CLASS(WMSCatalog));
	AddDocTemplate(dtObjectCollection);

	dtObjectCollection = new ForeignMultiDocTemplate(
		"Foreign Collection", 
		"ILWIS ForeignCollections",
		RUNTIME_CLASS(ForeignCollectionDoc),
		RUNTIME_CLASS(ObjectCollectionWindow),
		RUNTIME_CLASS(ForeignCatalog));
	AddDocTemplate(dtObjectCollection);

	// MapWindow
	dtMapWindow = new IlwisDocTemplate(
		".mpr.mps.mpa.mpp.mpl.mpv", 
		"Map,SegmentMap,PolygonMap,PointMap,MapList,MapView",
		"ILWIS Map View",
		RUNTIME_CLASS(MapCompositionDoc),
		RUNTIME_CLASS(MapWindow),
		RUNTIME_CLASS(MapPaneView));
	AddDocTemplate(dtMapWindow);
	/*  
	idt->SetServerInfo(1, 1,
	RUNTIME_CLASS(MapCompositionInPlaceFrame),
	RUNTIME_CLASS(MapPaneView));
	AddDocTemplate(dtMapWindow);
	otsMapComposition.ConnectTemplate(MapCompositionDoc::clsid, idt, FALSE);
	*/	
	idt = new IlwisDocTemplate(
		".grf", 
		"GeoRefCTP,GeoRefOrthoPhoto,GeoRefDirectLinear,GeoRef3D,GeoRefParallelProjective",
		"ILWIS Georeferences",
		RUNTIME_CLASS(MapCompositionDoc),
		RUNTIME_CLASS(MapWindow),
		RUNTIME_CLASS(MapPaneView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".sms", 
		"SampleSet",
		"ILWIS SampleSet",
		RUNTIME_CLASS(MapCompositionDoc),
		RUNTIME_CLASS(MapWindow),
		RUNTIME_CLASS(MapPaneView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".csy", 
		"TiePoints,OrthoPhoto,DirectLinear",
		"ILWIS Coordinate systems",
		RUNTIME_CLASS(MapCompositionDoc),
		RUNTIME_CLASS(MapWindow),
		RUNTIME_CLASS(MapPaneView));
	AddDocTemplate(idt);

	// CoordinateSystemBoundsOnly
	idt = new IlwisDocTemplate(
		".csy", 
		"BoundsOnly",
		"ILWIS Coordinate systems",
		RUNTIME_CLASS(CoordSysDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(CoordSysBoundsOnlyView));
	AddDocTemplate(idt); 

	//CoordinateSystemLatLon	
	idt = new IlwisDocTemplate(
		".csy", 
		"LatLon",
		"ILWIS Coordinate systems",
		RUNTIME_CLASS(CoordSysDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(CoordSysLatLonView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".csy", 
		"Formula",
		"ILWIS Coordinate systems",
		RUNTIME_CLASS(CoordSysDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(CoordSysFormulaView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".csy", 
		"Projection,CoordSystem",
		"ILWIS Coordinate systems",
		RUNTIME_CLASS(CoordSysDoc),
		RUNTIME_CLASS(DataFormWindow),
		RUNTIME_CLASS(CoordSysProjectionView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".atx", 
		"AnnotationText",
		"ILWIS Tables",
		RUNTIME_CLASS(TableDoc),
		RUNTIME_CLASS(TableWindow),
		RUNTIME_CLASS(TablePaneView));
	AddDocTemplate(idt);
	dtTableWindow = (IlwisDocTemplate *)idt;

	idt = new IlwisDocTemplate(
		".tbt", 
		"Table",
		"ILWIS Tables",
		RUNTIME_CLASS(TableDoc),
		RUNTIME_CLASS(TableWindow),
		RUNTIME_CLASS(TablePaneView));
	AddDocTemplate(idt);
	dtTableWindow = (IlwisDocTemplate *)idt;	

	idt = new IlwisDocTemplate(
		".his.hsa.hss.hsp", 
		"Table",
		"ILWIS Histograms",
		RUNTIME_CLASS(HistogramDoc),
		RUNTIME_CLASS(HistogramWindow),
		RUNTIME_CLASS(TablePaneView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".ta2", 
		"Table2Dim",
		"ILWIS 2 Dimensional Tables",
		RUNTIME_CLASS(Table2Doc),
		RUNTIME_CLASS(Table2Window),
		RUNTIME_CLASS(Table2PaneView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".fil", 
		"FilterLinear",
		"ILWIS Filters",
		RUNTIME_CLASS(LinearFilterDoc),
		RUNTIME_CLASS(LinearFilterWindow),
		RUNTIME_CLASS(LinearFilterView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".mat", 
		"Matrix",
		"ILWIS Matrices",
		RUNTIME_CLASS(MatrixDoc),
		RUNTIME_CLASS(MatrixWindow),
		RUNTIME_CLASS(MatrixView));
	AddDocTemplate(idt);

	//idt = new IlwisDocTemplate(
	//	".mpr", 
	//	"Foreign Object",
	//	"Openstreetmap",
	//	RUNTIME_CLASS(MapCompositionDoc),
	//	RUNTIME_CLASS(MapWindow),
	//	RUNTIME_CLASS(MapPaneView));
	//AddDocTemplate(idt);

	idt = new IlwisExpressionDocTemplate(
		"Matrix",
		RUNTIME_CLASS(MatrixDoc),
		RUNTIME_CLASS(MatrixWindow),
		RUNTIME_CLASS(MatrixView));
	AddDocTemplate(idt);

	idt = new IlwisDocTemplate(
		".mat", 
		"Matrix",
		"ILWIS Matrices",
		RUNTIME_CLASS(MatrixDoc),
		RUNTIME_CLASS(MatrixWindow),
		RUNTIME_CLASS(MatrixView));
	AddDocTemplate(idt);

	idt = new IlwisExpressionDocTemplate(
		"Graph",
		RUNTIME_CLASS(CartesianGraphDoc),
		RUNTIME_CLASS(GraphWindow),
		RUNTIME_CLASS(GraphPaneView));
	AddDocTemplate(idt);

	dtGraph = new IlwisDocTemplate(
		".grh", 
		"StandardGraph",
		"ILWIS Graph",
		RUNTIME_CLASS(CartesianGraphDoc),
		RUNTIME_CLASS(GraphWindow),
		RUNTIME_CLASS(GraphPaneView));
	AddDocTemplate(dtGraph);

	dtRoseDiagram = new IlwisDocTemplate(
		".grh", 
		"RoseDiagram",
		"ILWIS Graph",
		RUNTIME_CLASS(RoseDiagramDoc),
		RUNTIME_CLASS(GraphWindow),
		RUNTIME_CLASS(GraphPaneView));
	AddDocTemplate(dtRoseDiagram);

	dtLayout = new IlwisDocTemplate(
		".ilo", 
		"Layout",
		"ILWIS Layout",
		RUNTIME_CLASS(LayoutDoc),
		RUNTIME_CLASS(LayoutWindow),
		RUNTIME_CLASS(LayoutView));
	AddDocTemplate(dtLayout);

	dtSMCE = new IlwisDocTemplate(
		".smc", 
		"CriteriaTree",
		"ILWIS Criteria Tree",
		RUNTIME_CLASS(CriteriaTreeDoc),
		RUNTIME_CLASS(SMCEWindow),
		RUNTIME_CLASS(CriteriaTreeView));
	AddDocTemplate(dtSMCE);

	dtPixelInfo = new IlwisDocTemplate(
		"", "",
		"ILWIS PixelInfo",
		RUNTIME_CLASS(PixelInfoDoc),
		RUNTIME_CLASS(MapWindow),
		RUNTIME_CLASS(PixelInfoView));
	AddDocTemplate(dtPixelInfo);

	dtForeign = new ForeignMultiDocTemplate(); // catch all for non - ilwis extensions, 
	AddDocTemplate(dtForeign);

	dtDirectory = new CMultiDocTemplate(1,
		RUNTIME_CLASS(DirectoryDocument),
		RUNTIME_CLASS(CatalogFrameWindow),
		RUNTIME_CLASS(Catalog));
	AddDocTemplate(dtDirectory);
	// Comment: for the time being the code below causes problems in Windows NT
	// when a non-admin is logged in
	/*
	COleTemplateServer::RegisterAll();
	otsMapComposition.UpdateRegistry(OAT_INPLACE_SERVER, NULL, (LPCTSTR*)rgServerMapComposition);
	COleObjectFactory::UpdateRegistryAll();
	*/
}

void IlwisWinApp::SetCommandLine(String s, bool fAddToHistory)
{
	MainWindow* mw = dynamic_cast<MainWindow*>(m_pMainWnd);
	if (mw)
		mw->SetCommandLine(s, fAddToHistory);
}

void IlwisWinApp::RegisterClasses()
{
	// MainWindow
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_DBLCLKS ;
	wc.lpfnWndProc = AfxWndProc;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	wc.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	wc.lpszClassName = ILWIS_VERSION_NAME;
	AfxRegisterClass(&wc);

	// FormViewBase
	wc.style = 0;
	wc.hbrBackground = HBRUSH(COLOR_BTNFACE + 1);
	wc.hCursor = 0;
	wc.hIcon = LoadIcon("LogoIcon");
	wc.lpszClassName = "IlwisView";
	AfxRegisterClass(&wc);

	GetClassInfo(NULL,"STATIC",&wc);
	wc.style |= CS_SAVEBITS;
	// wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "InfoLine";
	AfxRegisterClass(&wc);
}

void IlwisWinApp::InitImage(string sExt, const char* sIcon, int iNr)
{
	HICON hIcon;
	mapExt[sExt] = iNr;                                        
	hIcon = (HICON)LoadImage(m_hInstance, sIcon, IMAGE_ICON,   
		16, 16, LR_DEFAULTCOLOR);       
	ilSmall.Replace(iNr, hIcon);                               
	hIcon = (HICON)LoadImage(m_hInstance, sIcon, IMAGE_ICON,   
		32, 32, LR_DEFAULTCOLOR);       
	ilLarge.Replace(iNr, hIcon);                               
}

void IlwisWinApp::InitImageLists()
{
	ilSmall.Create(16,16,TRUE,4,4);
	ilLarge.Create(32,32,TRUE,4,4);
	ilSmall.SetImageCount(145);
	ilLarge.SetImageCount(145);

	int i = 0;
	InitImage("ilwis", "LogoIcon", i++);

	InitImage(".dom", "DomIcon", i++);
	InitImage(".rpr", "RprIcon", i++);
	InitImage(".grf", "GrfIcon", i++);
	InitImage(".csy", "CsyIcon", i++);

	InitImage(".mpr", "MapIcon", i++);
	InitImage(".mpa", "PolIcon", i++);
	InitImage(".mps", "SegIcon", i++);
	InitImage(".mpp", "PntIcon", i++);
	InitImage(".mpv", "MpvIcon", i++);
	mapExt["MapView"] = mapExt[".mpv"];
	InitImage("MapBorder", "MapBorderIcon", i++);
	InitImage(".mpl", "MplIcon", i++);
	InitImage(".ilo", "IloIcon", i++);
	InitImage(".ioc", "OBJECTCOLLECTIONICON", i++);

	InitImage(".tbt", "TbtIcon", i++);
	InitImage(".ta2", "Ta2Icon", i++);
	InitImage(".his", "HisIcon", i++);
	InitImage(".hsa", "HsaIcon", i++);
	InitImage(".hss", "HssIcon", i++);
	InitImage(".hsp", "HspIcon", i++);

	InitImage(".sms", "SmsIcon", i++);
	InitImage(".mat", "MatIcon", i++);
	InitImage(".fil", "FltIcon", i++);
	InitImage(".fun", "FunIcon", i++);
	InitImage(".isl", "IslIcon", i++);
	mapExt["Script"] = mapExt[".isl"];
	InitImage(".atx", "AtxIcon", i++);
	InitImage(".grh", "GraphObjectIcon", i++);
	InitImage(".stp", "StereoPairIcon", i++);
	InitImage(".smc", "SMCEIcon", i++);
	InitImage("Graph", "GraphIcon", i++);
	InitImage("Axis", "AxisIcon", i++);

	InitImage("directory", "DirIcon", i++);
	InitImage("opendirectory", "OpenDirIcon", i++);
	InitImage("drive", "DriveIcon", i++);
	InitImage("driveremote", "DriveRemote", i++);
	InitImage("column", "ColumnIcon", i++);
	InitImage(".col", "ColumnIcon", i++);

	InitImage(".grid", "GridIcon", i++);
	InitImage(".grat", "GratIcon", i++);
	InitImage("info", "InfoIcon", i++);
	InitImage("prop", "PropIcon", i++);
	InitImage("legend", "LegendIcon", i++);
	mapExt["Legend"] = mapExt["legend"];
	InitImage("ScaleBar", "ScaleBarIcon", i++);
	mapExt["ScaleText"] = mapExt["ScaleBar"];
	InitImage("NorthArrow", "NorthArrowIcon", i++);
	InitImage("Text", "TextIcon", i++);
	InitImage("Picture", "PictureIcon", i++);
	InitImage("Bitmap", "BitmapIcon", i++);
	InitImage("Box", "BoxIcon", i++);

	InitImage("ExeIcoL", "Exe16IcoL", i++);
	InitImage("ExeMap16IcoL", "ExeMapIconL", i++);
	InitImage("ExePol16IcoL", "ExePolIconL", i++);
	InitImage("ExeSeg16IcoL", "ExeSegIconL", i++);
	InitImage("ExePnt16IcoL", "ExePntIconL", i++);
	InitImage("ExeTbl16IcoL", "ExeTblIconL", i++);
	InitImage("ExeMat16IcoL", "ExeMatIconL", i++);
	InitImage("ExeGenMap16Ico", "ExeGenIcon", i++);
	InitImage("DspMap16Ico", "DspMapIcon", i++);

	InitImage("OverlaySystem", "OvlSystemIcon", i++);
	InitImage("OverlayInfo", "OvlInfoIcon", i++);
	InitImage("OverlayUseAs", "OvlUseAs", i++);
	InitImage("History", "History", i++);
	InitImage(".history", "HISTORY", i++);
	InitImage("TabRasMap", "MapTabIcon", i++);
	InitImage("TabPolMap", "PolTabIcon", i++);
	InitImage("TabSegMap", "SegTabIcon", i++);
	InitImage("TabPntMap", "PntTabIcon", i++);
	InitImage("ForeignFile", "ForeignIcon", i++);
	InitImage("CDRomDrive", "cddrive", i++);
	InitImage("NetworkDrive", "drivenet", i++);
	InitImage("FloppyDrive", "floppyicon", i++);	
	InitImage("CalculationMultiple", "CalculationMulti16Icon", i++);		
	InitImage("Calculationsingle", "CalculationSingle16Icon", i++);			
	InitImage("WMF", "WMF16ICO", i++);			
	InitImage("Transparent", "Transparent", i++);			
	InitImage("Valuerange", "ValueRange", i++);			
	InitImage("Colors", "Colors", i++);			
	InitImage("SingleColor", "SingleColor", i++);			
	InitImage("MultipleColors", "MultipleColors", i++);
	InitImage("Active", "Active", i++);
	InitImage("Mask", "Mask", i++);
	InitImage("Set", "Set", i++);
	InitImage("MapPane", "MapPane", i++);
	InitImage("LineStyle", "LineStyle", i++);
	InitImage("3D", "ThreeD", i++);
	InitImage("Animation","Animation", i++);
	InitImage("Run","Run", i++);
	InitImage("Stop","Stop", i++);
	InitImage("Begin","Begin", i++);
	InitImage("End","End", i++);
	InitImage("Pause","Pause", i++);
	InitImage("Extrusion","Extrusion",i++);
	InitImage("Slicing","Slicing",i++);
	InitImage("SelectArea","SelectArea",i++);
	InitImage("TimeSelection","TimeSelection",i++);
	InitImage(".TimeSelection","TimeSelection",i++);
	InitImage("Select","Select",i++);
	InitImage("SelectAoi","SelectAOI",i++);
	InitImage("Average","average",i++);
	InitImage("Median","median",i++);
	InitImage("Min","min",i++);
	InitImage("Max","max",i++);
	InitImage("Save","save",i++);
	InitImage("EditMap","EditMap",i++);
	InitImage("OneFramePlus","OneFramePlus",i++);
	InitImage("OneFrameMinus","OneFrameMinus",i++);
	InitImage("GNC", "GNC", i++);
	InitImage(".GNC", "GNC", i++);
	InitImage("ESA", "ESA", i++);	
	InitImage("amesd", "amesd", i++);	
	InitImage("Setcol", "SETCOLL", i++);	
	InitImage("Stack", "Stack", i++);	
	InitImage("Measurer", "Measurer", i++);	
	InitImage("AllLayers", "AllLayers", i++);	
	InitImage("Circle", "Circle", i++);	
	InitImage("CrossSection", "CrossSection", i++);	
	InitImage("Track", "Track", i++);	
	InitImage("Scale", "Scale", i++);	
	InitImage("Annotation", "Annotation", i++);	
	InitImage("Position", "Position", i++);	
	InitImage("Appearance", "Appearance", i++);	
	InitImage("Direction", "direction", i++);	
	InitImage(".ivg", "Symbol", i++);	
	InitImage("AutoClose", "AutoClose", i++);
	InitImage("Snap", "snap", i++);
	InitImage("Undefined", "undefined", i++);
	InitImage("integer", "integer", i++);
	InitImage(".integer", "integer", i++);
	InitImage("float", "float", i++);
	InitImage(".float", "float", i++);
	InitImage(".bool", "boolean", i++);
	InitImage(".choice", "choice", i++);
	InitImage("ColorComposite", "ColorComposite", i++);
	InitImage("Border", "Border", i++);
	InitImage("Openstreemap","Openstreetmap", i++);
	InitImage("SpaceTimeCube","SpaceTimeCube", i++);
	InitImage(".ivh","Hatch",i++);
	InitImage("earth", "earth", i++);
	InitImage("ethiopia", "ethiopia", i++);
	InitImage("histo16", "HISICON16", i++);
	InitImage("ellipse", "ellipse", i++);
	InitImage("curve", "Curve", i++);
	InitImage("line", "Line", i++);
	InitImage("whitespace", "whitespace", i++);
	InitImage("false", "CHECKBOXEMPTYICO", i++);
	InitImage("true", "CHECKBOXCHECKEDICO", i++);
}

int IlwisWinApp::iImage(const string& sExt)
{
	return mapExt[sExt];
}

void IlwisWinApp::DirChanged()
{
	if (0 == dirdoc)
		return;
	CWaitCursor wc;
	dirdoc->OnOpenDocument(0);
	dirdoc->UpdateAllViews(0);
}

CatalogDocument* IlwisWinApp::dirDoc()
{
	return dirdoc;
}

void IlwisWinApp::SetCatalogDocument(CatalogDocument *doc)
{
	dirdoc = dynamic_cast<DirectoryDocument*>(doc);
}


CDocument* IlwisWinApp::OpenDirectory(LPCTSTR lpszFileName)
{
	if (0 == dirdoc)
		return 0;
	dirdoc->OnOpenDocument(lpszFileName);
	SetCurDir(lpszFileName);
	dirdoc->UpdateAllViews(0);
	return dirdoc;
}

CDocument* IlwisWinApp::OpenADOTable(const FileName& fn, const ParmList& pm)
{
	return NULL;
}

CDocument* IlwisWinApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	return OpenDocumentFile(lpszFileName, IlwisDocument::otNORMAL);
}

CDocument* IlwisWinApp::OpenDocumentFile(LPCTSTR lpszFileName, const String& sCom)
{
	String sCommand("%s %S", lpszFileName, sCom);
	return dtForeign->OpenDocumentFile(sCommand.c_str());
}

/*****************************************************************************
*
* GetLongPathName
*
* only the necessary part was taken from NewAPIs.h, starting at the following line
*
*  Copyright (c) 1997-1999, Microsoft Corporation
*
*  Wrapper module that "stubs" APIs that were not implemented
*  on Windows 95 or Windows NT versions less than 4.0 SP 3.
*
*  By using this header, your code will run on older platforms.
*
*  To enable a particular wrapper, define the corresponding symbol.
*
*  Function                Symbol
*
*  GetDiskFreeSpaceEx      WANT_GETDISKFREESPACEEX_WRAPPER
*  GetLongPathName         WANT_GETLONGPATHNAME_WRAPPER
*  GetFileAttributesEx     WANT_GETFILEATTRIBUTESEX_WRAPPER
*  IsDebuggerPresent       WANT_ISDEBUGGERPRESENT_WRAPPER
*
*  Exactly one source file must include the line
*
*  #define COMPILE_NEWAPIS_STUBS
*
*  before including this file.
*
*
*****************************************************************************/

#include <shlobj.h>

#undef GetLongPathName
#define GetLongPathName _GetLongPathName

extern DWORD (CALLBACK *GetLongPathName)(LPCTSTR, LPTSTR, DWORD);

/*
* The version to use if we are forced to emulate.
*/
static DWORD WINAPI
Emulate_GetLongPathName(LPCTSTR ptszShort, LPTSTR ptszLong, DWORD ctchBuf)
{
	LPSHELLFOLDER psfDesk;
	HRESULT hr;
	LPITEMIDLIST pidl;
	TCHAR tsz[MAX_PATH];            /* Scratch TCHAR buffer */
	DWORD dwRc;
	LPMALLOC pMalloc;

	/*
	*  The file had better exist.  GetFileAttributes() will
	*  not only tell us, but it'll even call SetLastError()
	*  for us.
	*/
	if (GetFileAttributes(ptszShort) == 0xFFFFFFFF) {
		return 0;
	}

	/*
	*  First convert from relative path to absolute path.
	*  This uses the scratch TCHAR buffer.
	*/
	dwRc = GetFullPathName(ptszShort, MAX_PATH, tsz, NULL);
	if (dwRc == 0) {
		/*
		*  Failed; GFPN already did SetLastError().
		*/
	} else if (dwRc >= MAX_PATH) {
		/*
		*  Resulting path would be too long.
		*/
		SetLastError(ERROR_BUFFER_OVERFLOW);
		dwRc = 0;
	} else {
		/*
		*  Just right.
		*/
		hr = SHGetDesktopFolder(&psfDesk);
		if (SUCCEEDED(hr)) {
			ULONG cwchEaten;

#ifdef UNICODE
#ifdef __cplusplus
			hr = psfDesk->ParseDisplayName(NULL, NULL, tsz,
				&cwchEaten, &pidl, NULL);
#else
			hr = psfDesk->lpVtbl->ParseDisplayName(psfDesk, NULL, NULL, tsz,
				&cwchEaten, &pidl, NULL);
#endif
#else
			WCHAR wsz[MAX_PATH];        /* Scratch WCHAR buffer */

			/*
			*  ParseDisplayName requires UNICODE, so we use
			*  the scratch WCHAR buffer during the conversion.
			*/
			dwRc = MultiByteToWideChar(
				AreFileApisANSI() ? CP_ACP : CP_OEMCP,
				0, tsz, -1, wsz, MAX_PATH);
			if (dwRc == 0) {
				/*
				*  Couldn't convert to UNICODE.  MB2WC uses
				*  ERROR_INSUFFICIENT_BUFFER, which we convert
				*  to ERROR_BUFFER_OVERFLOW.  Any other error
				*  we leave alone.
				*/
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					SetLastError(ERROR_BUFFER_OVERFLOW);
				}
				dwRc = 0;
			} else {
#ifdef __cplusplus
				hr = psfDesk->ParseDisplayName(NULL, NULL, wsz,
					&cwchEaten, &pidl, NULL);
#else
				hr = psfDesk->lpVtbl->ParseDisplayName(psfDesk, NULL, NULL,
					wsz, &cwchEaten, &pidl, NULL);
#endif
#endif

				if (FAILED(hr)) {
					/*
					*  Weird.  Convert the result back to a Win32
					*  error code if we can.  Otherwise, use the
					*  generic "duh" error code ERROR_INVALID_DATA.
					*/
					if (HRESULT_FACILITY(hr) == FACILITY_WIN32) {
						SetLastError(HRESULT_CODE(hr));
					} else {
						SetLastError(ERROR_INVALID_DATA);
					}
					dwRc = 0;
				} else {
					/*
					*  Convert the pidl back to a filename in the
					*  TCHAR scratch buffer.
					*/
					dwRc = SHGetPathFromIDList(pidl, tsz);
					if (dwRc == 0 && tsz[0]) {
						/*
						*  Bizarre failure.
						*/
						SetLastError(ERROR_INVALID_DATA);
					} else {
						/*
						*  Copy the result back to the user's buffer.
						*/
						dwRc = lstrlen(tsz);
						if (dwRc + 1 > ctchBuf) {
							/*
							*  On buffer overflow, return necessary
							*  size including terminating null (+1).
							*/
							SetLastError(ERROR_INSUFFICIENT_BUFFER);
							dwRc = dwRc + 1;
						} else {
							/*
							*  On buffer okay, return actual size not
							*  including terminating null.
							*/
							lstrcpyn(ptszLong, tsz, ctchBuf);
						}
					}

					/*
					*  Free the pidl.
					*/
					if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
#ifdef __cplusplus
						pMalloc->Free(pidl);
						pMalloc->Release();
#else
						pMalloc->lpVtbl->Free(pMalloc, pidl);
						pMalloc->lpVtbl->Release(pMalloc);
#endif
					}
				}
#ifndef UNICODE
			}
#endif
			/*
			*  Release the desktop folder now that we no longer
			*  need it.
			*/
#ifdef __cplusplus
			psfDesk->Release();
#else
			psfDesk->lpVtbl->Release(psfDesk);
#endif
		}
	}
	return dwRc;
}

/*
* The stub that probes to decide which version to use.
*/
static DWORD WINAPI
Probe_GetLongPathName(LPCTSTR ptszShort, LPTSTR ptszLong, DWORD ctchBuf)
{
	HINSTANCE hinst;
	FARPROC fp;
	DWORD dwRc;
	BOOL (CALLBACK *RealGetLongPathName)(LPCTSTR, LPTSTR, DWORD);

	hinst = GetModuleHandle(TEXT("KERNEL32"));
#ifdef UNICODE
	fp = GetProcAddress(hinst, "GetLongPathNameW");
#else
	fp = GetProcAddress(hinst, "GetLongPathNameA");
#endif

	if (fp) {
		*(FARPROC *)&RealGetLongPathName = fp;
		dwRc = RealGetLongPathName(ptszShort, ptszLong, ctchBuf);
		if (dwRc || GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) {
			GetLongPathName = (DWORD (CALLBACK *)(LPCTSTR, LPTSTR, DWORD))RealGetLongPathName;
		} else {
			GetLongPathName = Emulate_GetLongPathName;
			dwRc = GetLongPathName(ptszShort, ptszLong, ctchBuf);
		}
	} else {
		GetLongPathName = Emulate_GetLongPathName;
		dwRc = GetLongPathName(ptszShort, ptszLong, ctchBuf);
	}

	return dwRc;

}

DWORD (CALLBACK *GetLongPathName)(LPCTSTR, LPTSTR, DWORD) =
Probe_GetLongPathName;

/*****************************************************************************
*
* END OF GetLongPathName FROM NewAPIs.h
*
*****************************************************************************/


CDocument *IlwisWinApp::OpenDocumentFile(const String& sFO, ParmList& pm)
{
	bool fCollection = false;
	// if it is a collection we must handle everyhting in the mainthread (MDI)	
	// so we must determine this
	ForeignFormat *ff	= NULL;
	if ( !pm.fExist("collection")) 
	{
		String sMethod = pm.sGet("method");
		if ( sMethod != "")
		{
			ff = ForeignFormat::Create(sMethod);
			if ( ff ) {
				fCollection = ff->fIsCollection(sFO);
			}
		}			
	}	
	if ( fCollection ) 
	{
		FileName fnFO(sFO);
		Directory dirOut(fnFO.sPath());
		FileName fnIOC(fnFO);
		if ( pm.fExist("output")) {
			fnIOC = FileName(pm.sGet("output"));	
		}
		fnIOC.Dir(dirOut.sFullPath());
		if (!pm.fExist("refresh"))
			fnIOC = FileName::fnUnique(FileName(fnIOC.sFullPathQuoted(), ".ioc"));

		ForeignCollection fc;
		if ( ff && (ff->fMatchType(fnFO.sFullPath(), "ILWIS DataBase Collections") || ff->fMatchType(fnFO.sFullPath(), "ILWIS Postgres DataBase Collections")))
		{
			String loc = pm.sGet(0);
			String databaseName = loc.sHead("\\").sUnQuote();
			if ( databaseName != "" && pm.fExist("database") == false) {
				pm.Add(new Parm("database",databaseName));
			}
			fc = DataBaseCollection(fnIOC, pm);
		}			
		else {
			if ( pm.fExist("input") == false && pm.sGet(0) != "")
				pm.Add(new Parm("input", pm.sGet(0)));
			fc= ForeignCollection(fnIOC, pm);
		}

		fc->Create(pm);
	}		
	else // start a new wintread, and open there the file
	{
		WinThread *thr = new WinThread(m_pDocManager, sFO, pm);
		if ( thr )
			thr->CreateThread(0,0);
	}
	delete ff;	
	return NULL;
}

CDocument* IlwisWinApp::OpenDocumentFile(LPCTSTR lpszFileName, IlwisDocument::OpenType ot)
{
	const int iMaxLen = MAX_PATH + 1;
	char pLongFileName[iMaxLen];
	if (0 == GetLongPathName(lpszFileName, pLongFileName, iMaxLen))
		strcpy(pLongFileName, lpszFileName); // when not found just copy the string

	FileName fn(pLongFileName);
	String sFileName = pLongFileName;
	sFileName = sFileName.sUnQuote();
	bool fExisting = _access(sFileName.c_str(), 0) == 0; // check needed for inline matrices like MatrixVarCov
	IlwisObject::iotIlwisObjectType iotType = IlwisObject::iotObjectType(fn);
	if (iotType == IlwisObject::iotMAPLIST)
	{
		MapList ml(fn);
		bool fCalculated = ml->fCalculated();
		// prevent dead lock
		ml.SetPointer(0);
		if (fCalculated) {
			if ( ot == IlwisDocument::otANIMATION || ot == IlwisDocument::otCOLORCOMP) {
				WinThread *thr = new WinThread(m_pDocManager, pLongFileName, ot);
				if ( thr)
					thr->CreateThread(0,0);
			} else
				m_pDocManager->OpenDocumentFile(fn.sFullName().c_str());
		}
		else
			AfxBeginThread(CommandHandler::CalcObjectAndShow, (LPVOID)(new String(fn.sFullName(true))));
	}
	else if (iotType == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc(fn);
		if ( oc.fValid() == false)
			throw ErrorObject(TR("Invalid object collection"));
		bool fCalculated = oc->fCalculated();
		// prevent dead lock
		oc.SetPointer(0);
		if (fCalculated) {
			if ( ot == IlwisDocument::otANIMATION  || ot == IlwisDocument::otCOLLECTION) {
				//((IlwisDocManager *)m_pDocManager)->OpenDocumentAsAnimation(fn.sFullName().c_str());
				WinThread *thr = new WinThread(m_pDocManager, pLongFileName, ot);
				if ( thr)
					thr->CreateThread(0,0);

			}
			else
				m_pDocManager->OpenDocumentFile(fn.sFullName().c_str());
		}
		else
			AfxBeginThread(CommandHandler::CalcObjectAndShow, (LPVOID)(new String(fn.sFullName(true))));
	}
	else if	(iotType == IlwisObject::iotANY && fExisting) // catch all will go the foreignformat
	{
		m_pDocManager->OpenDocumentFile(fn.sFullName().c_str());
	}
	else
	{
		WinThread* thr = 0;
		if (fn.sSectionPostFix.length() > 0) 
		{
			if (fn.sExt.length() == 0)
				fn.sExt = ".mpl";
			thr = new WinThread(dtMapWindow, fn.sFullNameQuoted().c_str(), ot);
		}
		else 
			thr = new WinThread(m_pDocManager, pLongFileName, ot);
		if (thr) 
			thr->CreateThread(0, 0);
	}
	return 0;
}

CDocument* IlwisWinApp::OpenPixelInfo(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtPixelInfo, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenNewDirectory(LPCTSTR lpszFileName)
{
	return dtDirectory->OpenDocumentFile(lpszFileName);
}

CDocument* IlwisWinApp::OpenDocumentAsMap(LPCTSTR lpszFileName, bool fShow)
{
	if (fShow) {
		WinThread* thr = new WinThread(dtMapWindow, lpszFileName);
		if (thr) 
			thr->CreateThread(0, 0);
		return 0;
	}
	else {
		CDocument* doc = 0;
		if (0 != lpszFileName) 
			dtMapWindow->MatchDocType(lpszFileName, doc);
		if (doc)
			return doc;
		doc = new MapCompositionDoc;
		dtMapWindow->AddDocument(doc);
		BOOL bRes;
		if (0 == lpszFileName) 
			bRes = doc->OnNewDocument();
		else
			bRes = doc->OnOpenDocument(lpszFileName);
		if (!bRes) {
			delete doc;
			return 0;
		}
		return doc;
	}
}

CDocument* IlwisWinApp::OpenDocumentAsLayout(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtLayout, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenDocumentAsGraph(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtGraph, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenDocumentAsRoseDiagram(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtRoseDiagram, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenDocumentAsSlideShow(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtMapWindow, lpszFileName, IlwisDocument::otSLIDESHOW);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenDocumentAsTable(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtTableWindow, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenDocumentAsDomain(LPCTSTR lpszFileName)
{
	IlwisDocTemplate* idt = 0;
	String sType;
	ObjectInfo::ReadElement("Domain", "Type", lpszFileName, sType);
	if ("DomainUniqueID" == sType)
		idt = dtDomUnique;
	else 
		idt = dtDomWindow;

	WinThread* thr = new WinThread(idt, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenDocumentAsRpr(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtRprWindow, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenDocumentScript(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtScriptWindow, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenDocumentFunction(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtFunctionWindow, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CDocument* IlwisWinApp::OpenDocumentAsSMCE(LPCTSTR lpszFileName)
{
	WinThread* thr = new WinThread(dtSMCE, lpszFileName);
	if (thr) 
		thr->CreateThread(0, 0);
	return 0;
}

CFont* IlwisWinApp::GetFont(sfFontType sf)
{
	map<sfFontType, CFont*>::iterator fnt = StandardFonts.find(sf);

	if (fnt == StandardFonts.end())
		throw ErrorObject("XXRequesting Undefined Font");
	return fnt->second;
}

class TestForm : public FormWithDest
{
public:
	TestForm(CWnd *parent, String sTitle)
		: FormWithDest(parent, sTitle)
	{
		new StaticText(root, "Hallo");
		new FieldColor(root, "&Color", &clr);
		create();
	}
	String sDummy;
	Color clr;
};

void IlwisWinApp::OnTest()
{
	int iWidth = GetSystemMetrics(SM_CXMENUCHECK);
	int iHeight = GetSystemMetrics(SM_CYMENUCHECK);
	String sMsg("width = %i\nheight = %i", iWidth, iHeight);
	m_pActiveWnd->MessageBox(sMsg.sVal());
	//TestForm fwd(GetMainWnd(), "Test");
	//MessageBox(0, fwd.sDummy.sVal(), "Test", MB_OK);
}

void IlwisWinApp::OnAddDataWindow(WPARAM wParam, LPARAM lp)
{
	HWND hnd = (HWND) wParam;
	if ( lp == 0) {
		lhWindows.push_back(hnd);
	} else {
		lhNonDataWindows.push_back(hnd);
	}
}

void IlwisWinApp::OnRemoveDataWindow(WPARAM wParam, LPARAM lp)
{
	HWND hnd = (HWND) wParam;
	if ( lp == 0)
		lhWindows.remove(hnd);
	else
		lhNonDataWindows.remove(hnd);
}

inline void SendCloseMsg(HWND hWnd) 
{ ::SendMessage(hWnd, WM_CLOSE, 0, 0); }

void IlwisWinApp::OnCloseAll()
{
	for_each(lhWindows.begin(), lhWindows.end(), SendCloseMsg);
}

inline void SendMinimizeMsg(HWND hWnd) 
{ ::SendMessage(hWnd, WM_SYSCOMMAND,SC_MINIMIZE,0); }

void IlwisWinApp::OnMinimizeAll()
{
	for_each(lhWindows.begin(), lhWindows.end(), SendMinimizeMsg);
}

inline void SendRestoreMsg(HWND hWnd) 
{ ::SendMessage(hWnd, WM_SYSCOMMAND,SC_RESTORE,0); }

void IlwisWinApp::OnRestoreAll()
{
	for_each(lhWindows.begin(), lhWindows.end(), SendRestoreMsg);
}

class SendUpdateCoordMessage
{
	int cm;
	CoordWithCoordSystem* cwcs;
public:
	SendUpdateCoordMessage(int crdm, CoordWithCoordSystem* cwcsys)
		: cm(crdm), cwcs(cwcsys) {}
	void operator()(HWND hWnd)
	{ 
		if (IsWindowVisible(hWnd))
			SendMessage(hWnd,ILW_UPDATE,(WPARAM)cm,(LPARAM)(void*)cwcs); 
	}
};

void IlwisWinApp::SendUpdateCoordMessages(int cm, CoordWithCoordSystem* cwcs)
{
	for_each(lhWindows.begin(), lhWindows.end(), SendUpdateCoordMessage(cm,cwcs));
	for_each(lhNonDataWindows.begin(), lhNonDataWindows.end(), SendUpdateCoordMessage(cm,cwcs));
	if (cm & cmMOUSECLICK || cm >= cmDIGICLICK1)
		thrDdeServer->PostThreadMessage(ILW_UPDATE,(WPARAM)cm,(LPARAM)(void*)cwcs);
}

class SendUpdateAnimMessage
{
	FileName fnAnim;
	int currentIndex;

public:
	SendUpdateAnimMessage(const FileName& _fnAnim, int index)
		: fnAnim(_fnAnim), currentIndex(index) {}
	void operator()(HWND hWnd)
	{ 
		if (IsWindowVisible(hWnd))
			SendMessage(hWnd,ILWM_UPDATE_ANIM,(WPARAM)&fnAnim,(LPARAM)currentIndex); 
	}
};



void IlwisWinApp::SendUpdateAnimMessages(const FileName& fnAnim, int index)
{
	for_each(lhWindows.begin(), lhWindows.end(), SendUpdateAnimMessage(fnAnim,index));
	for_each(lhNonDataWindows.begin(), lhNonDataWindows.end(), SendUpdateAnimMessage(fnAnim,index));
}

void IlwisWinApp::RemoveSearchPath(const FileName& fn)
{
	Context()->RemoveSearchPath(fn);
}			

void IlwisWinApp::AddSearchPath(const FileName& fn)
{
	Context()->AddSearchPath(fn);
}

FileName IlwisWinApp::fnGetSearchPath(int iIndex)
{
	return Context()->fnGetSearchPath(iIndex);
}

void IlwisWinApp::ShowPopupMenu(CWnd* w, CPoint p, const FileName& fn, AssociationSelector** as, int iDfltOption)
{
	AssociationSelector assSel(w, p, *actList, fn, as, iDfltOption);
}

void IlwisWinApp::OnHelpContents()
{
	showHelp("__chmvf__.html");
}

void IlwisWinApp::OnHelpOperations()
{
	showHelp("ilwisapp/ilwis_operations.htm");
}

void IlwisWinApp::OnHelpMenuCmds()
{
	showHelp("ilwismen/menu_commands.htm");
}

void IlwisWinApp::OnHelpSearch()
{
	new HelpSearch(GetMainWnd());
}

void IlwisWinApp::OnHelpIndex()
{
	showHelp("ilwis/index.htm");

}

void IlwisWinApp::OnHelpGlossary()
{
	showHelp("ilwis/glossary.htm");
}

void IlwisWinApp::OnHelpHowTo()
{
	showHelp("ilwis/how_to.htm");
}

void IlwisWinApp::OnHelpMapTabCalc()
{
	showHelp("ilwis/calc.htm");
}

void IlwisWinApp::OnHelpIlwExpressions()
{
	showHelp("ilwis/appendices_construction_of_expressions.htm");
}

void IlwisWinApp::OnHelpSebs()
{
	String sDir = getEngine()->getContext()->sIlwDir();
	String sHelpFile (sDir + "Extensions\\SEBS\\SEBS4ILWIS_help.chm");
	::ShellExecute(GetDesktopWindow(),"open",sHelpFile.c_str(),0,0,SW_SHOW);
}

void IlwisWinApp::OnHelpScriptSyntax()
{
	//String sHelpFile (ChmFinder::sFindChmFile("ilwis.chm"));

	//::HtmlHelp(GetDesktopWindow(), sHelpFile.sVal(), HH_HELP_CONTEXT, htpIlwisScriptSyntax);
	//::HtmlHelp(0, sHelpFile.sVal(), HH_DISPLAY_TOC, 0);
}

void IlwisWinApp::DeleteDirDoc() 
{ 
	if ( ((CMDIFrameWnd *)GetMainWnd())->MDIGetActive())
	{
		delete dirdoc; 
		dirdoc = 0; 
	}
}

void IlwisWinApp::AddToCatalogHistory(const String& sDir)
{
	MainWindow *mw = dynamic_cast<MainWindow *>(m_pMainWnd);
	if ( mw)
		mw->AddToHistory(sDir);
}


void IlwisWinApp::OnDigitizerSetup()
{
	EnableDigitizer(true);
}

void IlwisWinApp::OnMapReferencing()
{
	ISTRUE(fINotEqual, digitizer, (Digitizer *)0);
	digitizer->Reference(0);
}

void IlwisWinApp::OnUpdateDigitizerSetup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(digitizer->fSetupAllowed());
	bool fSetup = false;
	if (digitizer) 
		fSetup = digitizer->fSetup;
	pCmdUI->SetCheck(fSetup?1:0);
}

void IlwisWinApp::OnUpdateMapReferencing(CCmdUI* pCmdUI)
{
	bool fSetup = false;
	bool fReferenced = false;

	if (digitizer) {
		fSetup = digitizer->fSetup;
		fReferenced = digitizer->fSetup && digitizer->fMapRef;
	}
	pCmdUI->Enable(fSetup && digitizer->fMapReferenceAllowed());
	pCmdUI->SetCheck(fReferenced?1:0);
}

void IlwisWinApp::OnOperationCmd(UINT nID)
{
	Action* act = (*actList)[nID-ID_OPERATION0];
	String sCmd = act->sAction();

	char* str = sCmd.sVal();
	GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
}

void IlwisWinApp::SetNextWindowRect(CPoint ptTopLeft, CSize sz)
{
	dquNextDataWindowPos.push_front(CRect(ptTopLeft, sz));	
}

void IlwisWinApp::GetNextWindowRect(CPoint& ptTopLeft, CSize& sz)
{
	if ( dquNextDataWindowPos.empty() ) return;
	CRect rct = dquNextDataWindowPos.back();
	dquNextDataWindowPos.pop_back();
	ptTopLeft = rct.TopLeft();
	sz = rct.Size();
}

// the win16 function MoveTo() was removed from windows.h
inline void MoveTo(HDC hdc, int x, int y)
{
	MoveToEx(hdc, x, y, 0);
}

// KBase Q124947

static HPALETTE CreateDIBPalette (LPBITMAPINFO lpbmi, LPINT lpiNumColors)
{
	LPBITMAPINFOHEADER lpbi;
	LPLOGPALETTE     lpPal;
	HANDLE           hLogPal;
	HPALETTE         hPal = NULL;
	int              i;

	lpbi = (LPBITMAPINFOHEADER)lpbmi;
	if (lpbi->biBitCount <= 8)
		*lpiNumColors = (1 << lpbi->biBitCount);
	else
		*lpiNumColors = 0;  // No palette needed for 24 BPP DIB

	if (*lpiNumColors)
	{
		hLogPal = GlobalAlloc (GHND, sizeof (LOGPALETTE) +
			sizeof (PALETTEENTRY) * (*lpiNumColors));
		lpPal = (LPLOGPALETTE) GlobalLock (hLogPal);
		lpPal->palVersion    = 0x300;
		lpPal->palNumEntries = *lpiNumColors;

		for (i = 0;  i < *lpiNumColors;  i++)
		{
			lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
			lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
			lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
			lpPal->palPalEntry[i].peFlags = 0;
		}
		hPal = CreatePalette (lpPal);
		GlobalUnlock (hLogPal);
		GlobalFree   (hLogPal);
	}
	return hPal;
}

static HBITMAP LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,
								  HPALETTE FAR* lphPalette)
{
	HRSRC  hRsrc;
	HGLOBAL hGlobal;
	HBITMAP hBitmapFinal = NULL;
	LPBITMAPINFOHEADER  lpbi;
	HDC hdc;
	int iNumColors;

	if (hRsrc = FindResource(hInstance, lpString, RT_BITMAP))
	{
		hGlobal = LoadResource(hInstance, hRsrc);
		lpbi = (LPBITMAPINFOHEADER)LockResource(hGlobal);
		hdc = GetDC(NULL);
		*lphPalette = CreateDIBPalette((LPBITMAPINFO)lpbi, &iNumColors);
		if (*lphPalette)
		{
			SelectPalette(hdc,*lphPalette,FALSE);
			RealizePalette(hdc);
		}

		hBitmapFinal = CreateDIBitmap(hdc,
			(LPBITMAPINFOHEADER)lpbi,
			(LONG)CBM_INIT,
			(LPSTR)lpbi + lpbi->biSize + iNumColors * sizeof(RGBQUAD),
			(LPBITMAPINFO)lpbi,
			DIB_RGB_COLORS );

		ReleaseDC(NULL,hdc);
		UnlockResource(hGlobal);
		FreeResource(hGlobal);
	}
	return (hBitmapFinal);
}

void IlwisWinApp::OpenLogoWindow()
{
	WNDCLASS wnd;
	wnd.style = CS_BYTEALIGNCLIENT|CS_SAVEBITS|CS_OWNDC;
	wnd.lpfnWndProc = DefWindowProc;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hInstance = m_hInstance;
	wnd.hIcon = NULL;
	wnd.hCursor = NULL;
	wnd.hbrBackground = NULL;
	wnd.lpszMenuName = NULL;
	wnd.lpszClassName = "IlwisLogo";
	RegisterClass(&wnd);

	HPALETTE hPal;
	HBITMAP hBmp = LoadResourceBitmap(m_hInstance, "Start", &hPal);
	BITMAP bm;
	int iX, iY, iWidth, iHeight;

	GetObject(hBmp,sizeof(bm),&bm);
	int iExtraX = 8; //GetSystemMetrics(SM_CXDLGFRAME);
	int iExtraY = 8; //GetSystemMetrics(SM_CYDLGFRAME);
	iWidth = bm.bmWidth + 2 * iExtraX;
	iHeight = bm.bmHeight + 2 * iExtraY;
	iX = (GetSystemMetrics(SM_CXSCREEN) - iWidth) / 2;
	iY = (GetSystemMetrics(SM_CYSCREEN) - iHeight) / 2;

	// WS_EX_TOOLWINDOW ensures that no taskbar button is created
	hWndLogo = CreateWindowEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
		"IlwisLogo","",
		WS_POPUP|WS_VISIBLE/*|WS_DLGFRAME*/,
		iX,iY,iWidth,iHeight,
		NULL,NULL,m_hInstance,NULL);
	HDC hdc = GetDC(hWndLogo);

	HPEN hPenBlack, hPenWhite, hPenLight, hPenGray, hPenDark, hPenOld;
	HBRUSH hBrush, hBrushOld;
	hPenWhite = CreatePen(PS_SOLID, 1, RGB(255,255,255));
	hPenBlack = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	hPenLight = CreatePen(PS_SOLID, 1, RGB(192,192,192));
	hPenGray  = CreatePen(PS_SOLID, 1, RGB(160,160,164));
	hPenDark  = CreatePen(PS_SOLID, 1, RGB(128,128,128));
	hBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	hPenOld = (HPEN)SelectObject(hdc,hPenBlack);
	hBrushOld = (HBRUSH)SelectObject(hdc,hBrush);
	//  Rectangle(hdc,0,0,iWidth,iHeight);
	int i;

	HDC hdcMemory = CreateCompatibleDC(hdc);
	SelectPalette(hdc,hPal,FALSE);
	RealizePalette(hdc);
	SelectPalette(hdcMemory,hPal,FALSE);
	RealizePalette(hdcMemory);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMemory, hBmp);

	BitBlt(hdc,iExtraX,iExtraY,bm.bmWidth,bm.bmHeight,hdcMemory,0,0,SRCCOPY);
	SelectObject(hdcMemory,hBmpOld);
	DeleteDC(hdcMemory);

	Rectangle(hdc,0,0,iWidth,iHeight);
	Rectangle(hdc,iExtraX-1,iExtraY-1,iWidth-iExtraX+1,iHeight-iExtraY+1);

	iWidth -= 1;
	iHeight-= 1;

	for (i = 1; i < 3; ++i) {
		SelectObject(hdc,hPenWhite);
		MoveTo(hdc,i,iHeight-i);
		LineTo(hdc,i,i);
		LineTo(hdc,iWidth-i+1,i);
		MoveTo(hdc,iWidth-i,i);
		SelectObject(hdc,hPenDark);
		LineTo(hdc,iWidth-i,iHeight-i);
		LineTo(hdc,i,iHeight-i);
	}

	SelectObject(hdc,hPenGray);
	for (i = 3; i < 5; ++i) {
		MoveTo(hdc,i,iHeight-i);
		LineTo(hdc,i,i);
		LineTo(hdc,iWidth-i,i);
		LineTo(hdc,iWidth-i,iHeight-i);
		LineTo(hdc,i,iHeight-i);
	}

	i = 6;    
	for (i = 5; i < 7; ++i) {
		SelectObject(hdc,hPenDark);
		MoveTo(hdc,i,iHeight-i);
		LineTo(hdc,i,i);
		LineTo(hdc,iWidth-i+1,i);
		MoveTo(hdc,iWidth-i,i);
		SelectObject(hdc,hPenLight);
		MoveTo(hdc,iWidth-i,i+1);
		LineTo(hdc,iWidth-i,iHeight-i);
		LineTo(hdc,i,iHeight-i);
	}

	SelectObject(hdc,hPenOld);
	SelectObject(hdc,hBrushOld);
	DeleteObject(hPenWhite);
	DeleteObject(hPenBlack);
	DeleteObject(hPenGray);
	DeleteObject(hPenLight);
	DeleteObject(hPenDark);
	DeleteObject(hBrush);

	SetBkMode(hdc, TRANSPARENT);
	HFONT hFnt = CreateFont(-24, 0, 0,0, FW_BOLD, 0, 0, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif");
	HFONT hFntOld = (HFONT)SelectObject(hdc, hFnt);

	char s1[] = VERSION_SPECIALBUILD;
	SetTextColor(hdc, RGB(49,99,173));
	TextOut(hdc, iExtraX+134, iExtraY+82, s1, (int)strlen(s1));

	HFONT hFntUser = CreateFont(-6, 0, 0,0, FW_THIN, 0, 0, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
		VARIABLE_PITCH | FF_SWISS, "MS Sans Serif");
	SelectObject(hdc, hFntUser);
	DeleteObject(hFnt);

	SetTextColor(hdc, RGB(7,0,31));
	String s2;

#ifdef BETA_VERSION
	// Set the Beta text
	Rectangle(hdc, iExtraX + 2, iExtraY+220, 130, 265);
	hFntOld = (HFONT)SelectObject(hdc, hFnt);
	s2 = String(" This is a Beta version,");
	TextOut(hdc, iExtraX +2, iExtraY+225, s2.sVal(), s2.length());
	s2 = String(" no support available");
	TextOut(hdc, iExtraX + 2, iExtraY+240, s2.sVal(), s2.length());
#endif // BETA_VERSION

	SelectObject(hdc, hFntOld); 
	DeleteObject(hFntUser);

	ReleaseDC(hWndLogo,hdc);
	DeleteObject(hBmp);
}

void IlwisWinApp::CloseLogoWindow()
{
	DestroyWindow(hWndLogo);
	MainWindow *mw = dynamic_cast<MainWindow *> (GetMainWnd());

	if ( mw )
	{
		CMDIChildWnd *mdi = mw->MDIGetActive();
		if ( mdi )
			mdi->Invalidate();
	}

	hWndLogo = 0;
}

CDocument *IlwisWinApp::OpenDocumentFile(const URL& url, ParmList& pm)
{
	bool fCollection = false;
	// if it is a collection we must handle everyhting in the mainthread (MDI)	
	// so we must determine this
	ForeignFormat *ff	= NULL;
	if ( !pm.fExist("collection")) 
	{
		String sMethod = pm.sGet("method");
		if ( sMethod != "")
		{
			ff = ForeignFormat::Create(sMethod);
			if ( ff ){
				fCollection = ff->fIsCollection(url.sVal());
			}
		}			
	}	

	if ( fCollection ) 
	{
		pm.Add(new Parm("url", url.sVal()));
		ForeignCollection fc;
		FileName fnIOC = ff->createCollectionName(url.sVal(), pm);
		if ( ff && (ff->fMatchType(url.sVal(), "ILWIS DataBase Collections") || ff->fMatchType(url.sVal(), "ILWIS Postgres DataBase Collections"))){
			SplitUrl(url, pm);
			fc = DataBaseCollection(fnIOC, pm);
		}  else if (ff && (ff->fMatchType(url.sVal(), "ILWIS ForeignCollections"))) {
			SplitUrl(url, pm);
			fc = ForeignCollection(fnIOC, pm);
		}
		else{
			fc = WMSCollection(fnIOC, pm);
		}

		fc->Create(pm);
		m_pDocManager->OpenDocumentFile(fnIOC.sFullName().c_str());
	}		
	delete ff;	
	return NULL;
}

void IlwisWinApp::SplitUrl(const URL& url, ParmList& pm) {
		Array<String> parts;
		Split(url.getPath(),parts,"/");
		String dbName = parts[parts.size() - 1];
		String host = url.getHost();
		pm.Add(new Parm("database", dbName));
		pm.Add(new Parm("host", host));
		pm.Add(new Parm("port", (double)url.getPort()));
}

void IlwisWinApp::SetTranquilizerStyle(bool fOld)
{
	fOldTranquilizers = fOld;
}

void  IlwisWinApp::SetCurDir(const String& sDir) {
	Context()->SetCurDir(sDir);
}

String IlwisWinApp::sGetCurDir() {
	return Context()->sCurDir();
}

LRESULT IlwisWinApp::ExecuteUI(const String& sCmd, CWnd *parent) {
	return commandUI->Execute(sCmd, parent);
}

vector<Action *> IlwisWinApp::CreateActions() {
	return commandUI->CreateActions();
}

void IlwisWinApp::getDocumentList(list<CDocument *>& opendocs)
{
   ASSERT(opendocs.size() == 0);

   POSITION pos = GetFirstDocTemplatePosition();

   while (pos)
   {
      CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate(pos);
      POSITION pos2 = pTemplate->GetFirstDocPosition();
      while (pos2)
      {
         CDocument * pDocument;
         if ((pDocument=pTemplate->GetNextDoc(pos2)) != NULL)
            opendocs.push_back(pDocument);
      }
   }
}

void IlwisWinApp::showHelp(const String& helpfile) {
	String help = helpfile;
	if ( help == "")
		return;
	String file;
	String ilwDir = getEngine()->getContext()->sIlwDir();
	if (ilwDir.length() > 0 && ilwDir[ilwDir.length() - 1] != '\\')
		ilwDir += '\\';
	string defaultFile = String("%Shelp\\eng\\%S",ilwDir,help);
	if ( helpfile[1] != ':') {
		String language = getEngine()->getContext()->getLanguageExtension();
		language = language.substr(1,language.size() - 1);
		if ( help.toLower().find("extensions") == 0) {
			file = String("%S\\%S",getEngine()->getContext()->sIlwDir(),help);
		}
		else {
			file = String("%Shelp\\%S\\%S",ilwDir,language,help);
		}
	} else
		file = helpfile;
	FileName fn1 = FileName(file);
	if ( !FileName(file).fExist())
		file = defaultFile;
	if ( FileName(file).fExist())
		ShellExecute(GetMainWnd()->m_hWnd,"open",file.c_str(),0,0,SW_SHOW);
	else
		throw ErrorObject(String("Help file %S%S doenst exist",fn1.sFile, fn1.sExt));
}

void IlwisWinApp::SendUpdateTableSelection(const vector<long>& raws, const FileName& fnObj, long self){
	for(list<HWND>::const_iterator cur = lhWindows.begin(); cur != lhWindows.end(); ++cur) {
		RowSelectInfo * inf = new RowSelectInfo();
		inf->fn = fnObj;
		inf->raws = raws;
		inf->sender = self;
		SendMessage((*cur), MESSAGE_SELECT_ROW,(WPARAM)inf, 0);
	}
}