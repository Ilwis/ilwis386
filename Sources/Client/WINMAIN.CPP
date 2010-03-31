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
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "Headers\toolspch.h"
#include "Headers\stdafx.h"

#ifdef AFX_CORE1_SEG
#pragma code_seg(AFX_CORE1_SEG)
#endif

/////////////////////////////////////////////////////////////////////////////
// Standard WinMain implementation
//  Can be replaced as long as 'AfxWinInit' is called first

#define AfxWinMain WinMain
int AFXAPI AfxWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	try
  {
		ASSERT(hPrevInstance == NULL);

		int nReturnCode = -1;
		CWinApp* pApp = AfxGetApp();

		// AFX internal initialization
		if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
			goto InitFailure;

		{
		int iBreakNr = -1;
		_CrtSetBreakAlloc(iBreakNr);
		}
		// App global initializations (rare)
		ASSERT_VALID(pApp);
		if (!pApp->InitApplication())
			goto InitFailure;
		ASSERT_VALID(pApp);

		// Perform specific initializations
		if (!pApp->InitInstance())
		{
			if (pApp->m_pMainWnd != NULL)
			{
				TRACE0("Warning: Destroying non-NULL m_pMainWnd\n");
				pApp->m_pMainWnd->DestroyWindow();
			}
			nReturnCode = pApp->ExitInstance();
			goto InitFailure;
		}
		ASSERT_VALID(pApp);

		nReturnCode = pApp->Run();
		ASSERT_VALID(pApp);

	InitFailure:
	#ifdef _DEBUG
		// Check for missing AfxLockTempMap calls
		if (AfxGetModuleThreadState()->m_nTempMapLock != 0)
		{
			TRACE1("Warning: Temp map lock count non-zero (%ld).\n",
				AfxGetModuleThreadState()->m_nTempMapLock);
		}
		AfxLockTempMaps();
		AfxUnlockTempMaps();
	#endif

		AfxWinTerm();
		return nReturnCode;
	}
	catch(ErrorObject& err)
  {
		err.Show();
	}
	catch(CException* err)
	{
    MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
	}	
  catch (...) 
	{
    MessageBox(0, "Unknown Error", "ILWIS Error", MB_SYSTEMMODAL|MB_ICONSTOP|MB_OK);
  }
	
	return  1; 
}


/////////////////////////////////////////////////////////////////////////////
// copied from appmodul.cpp
// if not available MFC thinks that the executable is a DLL


BOOL AFXAPI AfxInitialize(BOOL bDLL, DWORD dwVersion)
{
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	pModuleState->m_bDLL = (BYTE)bDLL;
	ASSERT(dwVersion <= _MFC_VER);
	UNUSED(dwVersion);  // not used in release build
#ifdef _AFXDLL
	pModuleState->m_dwVersion = dwVersion;
#endif
#ifdef _MBCS
	// set correct multi-byte code-page for Win32 apps
	if (!bDLL)
		_setmbcp(_MB_CP_ANSI);
#endif //_MBCS
	return TRUE;
}

// force initialization early
#pragma warning(disable: 4073)
#pragma init_seg(lib)

#ifndef _AFXDLL
void AFX_CDECL _AfxTermAppState()
{
	// terminate local data and critical sections
	AfxTermLocalData(NULL, TRUE);
	AfxCriticalTerm();

	// release the reference to thread local storage data
	AfxTlsRelease();
}
#endif

#ifndef _AFXDLL
char _afxInitAppState = (char)(AfxInitialize(FALSE, _MFC_VER), atexit(&_AfxTermAppState));
#else
char _afxInitAppState = (char)(AfxInitialize(FALSE, _MFC_VER));
#endif

/////////////////////////////////////////////////////////////////////////////


