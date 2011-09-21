// IlwisStub.h : main header file for the IlwisStub application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CIlwisStubApp:
// See IlwisStub.cpp for the implementation of this class
//

class CIlwisStubApp : public CWinApp
{
public:
	CIlwisStubApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	void copyUpdates(const std::string& root, const std::string& rootUpdate, const std::string& folder);
	void copyFile(const std::string& root, const std::string& rootUpdate,const std::string& f);

	DECLARE_MESSAGE_MAP()
};

extern CIlwisStubApp ilwisStub;