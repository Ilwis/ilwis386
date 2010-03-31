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
// COMCmdLineThread.cpp: implementation of the COMCmdLineThread class.
//                       implementation of the COMServerHandler class.
//
// The COM server handler and the COM CmdLine thread are handled in the
// "main" project and not in the RasterCOMServer project, because of the
// need to access IlwisWinApp, which in turn needs MFC.
// The RasterCOMServer project does not use MFC, because of the compile
// and link problems in combining ATL, MFC and ILWIS
//////////////////////////////////////////////////////////////////////

//#include "Client\Headers\AppFormsPCH.h"
#include "Headers\messages.h"
#include "Engine\Scripting\Script.h"
#include "Engine\COMInterface\COMCmdLineThread.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(COMCmdLineThread, CWinThread )
	//{{AFX_MSG_MAP(COMCmdLineThread)
	ON_THREAD_MESSAGE(ILWM_COMCMDLN_EXECUTE, OnExecute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


COMCmdLineThread::COMCmdLineThread()
{
	m_sDfltDir = getEngine()->sGetCurDir();
}

COMCmdLineThread::~COMCmdLineThread()
{
	getEngine()->RemoveThreadLocalVars();
}

BOOL COMCmdLineThread::InitInstance()
{
	// calling base class is not needed, always returns FALSE!
	getEngine()->InitThreadLocalVars();
	getEngine()->SetCurDir(m_sDfltDir);

	// The COM server catches all errors that force a messagebox itself
	bool *fNoErrorDisplay = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	*fNoErrorDisplay = true;
	fNoErrorDisplay = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWANYERROR));
	*fNoErrorDisplay = true;

	return TRUE;
}

int COMCmdLineThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

void COMCmdLineThread::OnExecute(WPARAM wParam, LPARAM lParam)
{
	String *ps = (String*)lParam;
	String sCmd(*ps);
	delete ps;

	// sTotCmd contains a single command to execute
	// A command is sent to the script command handler, except:
	// The commands 'import' and 'export' when no parameters are supplied. They're sent to the CommandHandler.
	bool fCommandResult = true;
	try
	{
		if (sCmd.length() > 0)
		{
			if (fCIStrEqual(sCmd,"import"))
			{
				// this should popup the import dialog box
				String sImpCmd("*import"); // The * is for not putting the command in the history
				const char *s = sImpCmd.scVal();		
				getEngine()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)s);
			}
			else if (fCIStrEqual(sCmd,"export"))
			{
				// this should popup the export dialog box
				String sExpCmd("*export"); // The * is for not putting the command in the history
				const char *s = sExpCmd.scVal();		
				getEngine()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)s);
			}
			else
				// Regular branch
				Script::Exec(sCmd);
		}
	}
	catch (...)
	{
		fCommandResult = false;
	}
	//return fCommandResult;
}

// --------- COMServerHandler ---------------
COMServerHandler::COMServerHandler()
{
	// the COMCmdLineThread thread handles the command line from COM clients
	m_thrCOMCmd = new COMCmdLineThread();
	m_thrCOMCmd->CreateThread(0, 0);
}

COMServerHandler::~COMServerHandler()
{
	HANDLE hThread = *m_thrCOMCmd;
	m_thrCOMCmd->PostThreadMessage(WM_QUIT, 0, 0);
	WaitForSingleObject(hThread, 1000); // wait a max of 1 second for the thread to stop
}

// sCmd contains a single command to execute
// The command is sent to the script command handler, except:
// The commands 'import' and 'export' when no parameters are supplied. They're sent to the CommandHandler.
bool COMServerHandler::ExecuteCmdLine(char *sCmd)
{
	if (m_thrCOMCmd)
	{
		String *ps = new String(sCmd);
		return (bool) m_thrCOMCmd->PostThreadMessage(ILWM_COMCMDLN_EXECUTE, 0, (LPARAM)(void *)ps);
	}
	else
		return false;
}

void COMServerHandler::StartFactories(StartComServerFunction *pf)
{
	AfxBeginThread(pf, (LPVOID)0);  // COM factory thread
}
