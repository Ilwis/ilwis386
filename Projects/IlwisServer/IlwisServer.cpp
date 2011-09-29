// IlwisServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Headers\toolspch.h"
#include "IlwisServer.h"
#include "MainFrm.h"
#include "Headers\version.h"
#include "Engine\Base\DataObjects\Version.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\Appcont.h"
#include "Engine\Scripting\Script.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\RootDrawer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// IlwisServerApp

BEGIN_MESSAGE_MAP(IlwisServerApp, CWinApp)
END_MESSAGE_MAP()


// IlwisServerApp construction

const String ILWIS::Version::IlwisVersion="3.8 beta - 1";

IlwisServerApp::IlwisServerApp() :
CWinApp(ILWIS_VERSION_NAME),
smILWIS(1,1,ILWIS_VERSION_NAME),
lockILWIS(&smILWIS)
{
}


// The one and only IlwisServerApp object

IlwisServerApp theApp;


BOOL IlwisServerApp::InitApplication()
{
	if (!CWinApp::InitApplication())
		return FALSE;

	getEngine();

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
	}



	if ( m_sCommand != "") {
		Script::Exec(m_sCommand);
		if ( getEngine()->fStayResident() == false) {
			getEngine()->Execute("exit");
			return FALSE;
		}
	}
	return TRUE;
}

BOOL IlwisServerApp::InitInstance()
{
	CWinApp::InitInstance();



	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);


	getEngine()->getContext()->InitThreadLocalVars();
	//delete pl;

	IlwisSettings ilwregset("", IlwisSettings::pkuUSER);
	SetRegistryKey(ilwregset.sSoftwareSubKey().c_str());
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup("");

	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	getEngine()->Execute("startserver");



	return TRUE;
}









