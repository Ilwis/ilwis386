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
#include "Engine\COMInterface\ComCaller.h"
#include "Engine\COMInterface\CommandLine.h"
#include <comdef.h>

extern HANDLE g_hEvent;

ILWIS_CommandLine::ILWIS_CommandLine() :
	m_iRefCount(1)
{
	CoAddRefServerProcess();
}
	
ILWIS_CommandLine::~ILWIS_CommandLine()
{
	if (CoReleaseServerProcess() == 0)
		SetEvent(g_hEvent);
}

ULONG __stdcall ILWIS_CommandLine::AddRef()
{
	return InterlockedIncrement(&m_iRefCount);
}

ULONG __stdcall ILWIS_CommandLine::Release()
{
	long iRefCount = 0;
	iRefCount = InterlockedDecrement(&m_iRefCount);
	if ( iRefCount == 0 )
		delete this;

	return iRefCount;
}

HRESULT __stdcall ILWIS_CommandLine::QueryInterface(REFIID riid, void **ppObj)
{
	if ( riid == IID_IUnknown)           *ppObj = (IUnknown *)(this);
	else if ( riid == IID_IILWIS_CommandLine)  *ppObj = (IILWIS_CommandLine *)(this);
	else
	{
		*ppObj = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

HRESULT __stdcall ILWIS_CommandLine::Execute(BSTR sCmd)
{
	_bstr_t bs(sCmd);
	char *pc = (char*)bs;
	if (ComCaller((char*)bs))
		return S_OK;
	else
		return S_FALSE;
}

