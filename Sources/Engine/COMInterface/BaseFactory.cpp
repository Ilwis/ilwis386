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
#include "Engine\COMInterface\BaseFactory.h"

extern HANDLE g_hEvent;

//-------------------------------------------------------------------------------------------------------
ULONG __stdcall IlwisFactory::AddRef()
{
	return InterlockedIncrement(&m_iRefCount);
}

ULONG __stdcall IlwisFactory::Release()
{
	unsigned iRefCount = 0;
	iRefCount = InterlockedDecrement(&m_iRefCount);
	if ( iRefCount == 0 )
		delete this;

	return iRefCount;
}

HRESULT __stdcall IlwisFactory::QueryInterface(REFIID riid, void **ppObj)
{
	if ( riid == IID_IUnknown)            *ppObj = (IUnknown *)(this);
	else if ( riid == IID_IClassFactory)  *ppObj = (IlwisFactory *)(this);
	else
	{
		*ppObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

HRESULT __stdcall IlwisFactory::CreateInstance(IUnknown *pUnknowOuter, REFIID iid, void **ppv)
{
	if ( pUnknowOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	// create the new object; this factory only can create cmdline objects
	ILWIS_CommandLine *pObject = new ILWIS_CommandLine;
	if ( pObject == NULL )
		return E_OUTOFMEMORY;

	// QI will increase the reference count
	return pObject->QueryInterface(iid, ppv);
}

// LockServer handles the lifetime of the server when multiple objects
// are created. For each new object the function is called to increase
// the reference count. For each object that stops the reverse happens.
HRESULT __stdcall IlwisFactory::LockServer(BOOL fLock)
{
	if (fLock)
		CoAddRefServerProcess();
	else
		if(CoReleaseServerProcess() == 0)
			SetEvent(g_hEvent);
		
	return S_OK;
}
