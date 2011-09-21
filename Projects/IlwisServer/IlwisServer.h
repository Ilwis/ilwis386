// IlwisServer.h : main header file for the IlwisServer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// IlwisServerApp:
// See IlwisServer.cpp for the implementation of this class
//

class IlwisServerApp : public CWinApp
{
public:
	IlwisServerApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

private:
	BOOL InitApplication();
	String m_sCommand;
	CSemaphore smILWIS;
	ILWISSingleLock lockILWIS;
	DECLARE_MESSAGE_MAP()
};

extern IlwisServerApp theApp;