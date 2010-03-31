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
// Copyright (C) 1998-1999 KeyWorks Software. All rights reserved.

// This module contains the interface and class that can be used for reading
// files (streams) out of a .CHM or .ITS file. 


#ifndef __CITS_H__
#define __CITS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DEFINE_GUID(CLSID_ITStorage, 0x5d02926a, 0x212e, 0x11d0, 0x9d, 0xf9, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xec);
DEFINE_GUID(IID_ITStorage, 0x88cc31de, 0x27ab, 0x11d0, 0x9d, 0xf9, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xec);

typedef struct _ITS_Control_Data
{
    UINT cdwControlData;     // Number of DWords to follow.
    UINT adwControlData[1];  // Actually this will be adwControlData[cdwControlData]

} ITS_Control_Data, *PITS_Control_Data;  

typedef enum ECompactionLev { 
    COMPACT_DATA = 0, 
    COMPACT_DATA_AND_PATH
};

DECLARE_INTERFACE_(IITStorage, IUnknown)
{
    STDMETHOD(StgCreateDocfile) (const WCHAR* pwcsName, DWORD grfMode, DWORD reserved, IStorage** ppstgOpen) PURE;

    STDMETHOD(StgCreateDocfileOnILockBytes) (ILockBytes * plkbyt, DWORD grfMode, 
                            DWORD reserved, IStorage ** ppstgOpen) PURE;

    STDMETHOD(StgIsStorageFile) (const WCHAR * pwcsName) PURE;

    STDMETHOD(StgIsStorageILockBytes) (ILockBytes * plkbyt) PURE;

    STDMETHOD(StgOpenStorage)(const WCHAR * pwcsName, IStorage * pstgPriority, 
                              DWORD grfMode, SNB snbExclude, DWORD reserved, 
                              IStorage ** ppstgOpen
                             ) PURE;

    STDMETHOD(StgOpenStorageOnILockBytes)
                  (ILockBytes * plkbyt, IStorage * pStgPriority, DWORD grfMode, 
                   SNB snbExclude, DWORD reserved, IStorage ** ppstgOpen
                  ) PURE;

    STDMETHOD(StgSetTimes)(WCHAR const * lpszName,  FILETIME const * pctime, 
                           FILETIME const * patime, FILETIME const * pmtime
                          ) PURE;

    STDMETHOD(SetControlData)(PITS_Control_Data pControlData) PURE;

    STDMETHOD(DefaultControlData)(PITS_Control_Data *ppControlData) PURE;
		
    STDMETHOD(Compact)(const WCHAR* pwcsName, ECompactionLev iLev) PURE;
};

class CItsFile
{
public:
    CItsFile();
    ~CItsFile();

    IStorage* GetStorage(void) { return m_pStorage; }   // this is the root storage

    HRESULT   OpenITS(PCSTR pszFile);

    HRESULT OpenStorage(PCSTR pszName, IStorage** ppStorage, DWORD dwAccess = STGM_READWRITE) {
                return m_pStorage->OpenStorage(FileNameToWCHAR(pszName), NULL, dwAccess, 0, 0, ppStorage); }
    HRESULT OpenStream(PCSTR pszFile, IStream** ppStream, IStorage* pStorage, DWORD dwAccess = STGM_READWRITE) {
                return pStorage->OpenStream(FileNameToWCHAR(pszFile), NULL, dwAccess, 0, ppStream); }

	WCHAR* FileNameToWCHAR(PCSTR psz);

protected:
    IITStorage*   m_pITStorage;
    IStorage*     m_pStorage;
    WCHAR*        m_pwTmpFile;
};

// Use this class to extract strings from the #STRINGS subfile

class CStringSubFile
{
public:
    CStringSubFile(CItsFile* pif);
    ~CStringSubFile();

    bool GetString(DWORD offset, char* pszDst, int cbDst);

protected:
    IStream*      m_pStream;     // pointer to #STRINGS stream
    CItsFile*     m_pif;
};

#endif  // __CITS_H__
