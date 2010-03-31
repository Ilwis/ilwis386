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

#include "Headers\stdafx.h"
#include "Client\Help\CITS.H"

CItsFile::CItsFile()
{
    m_pITStorage   = NULL;
    m_pStorage     = NULL;
    m_pwTmpFile     = NULL;
}

CItsFile::~CItsFile()
{
    if (m_pStorage)
        m_pStorage->Release();
    if (m_pITStorage)
        m_pITStorage->Release();
    if (m_pwTmpFile)
        LocalFree(m_pwTmpFile);
}

static const GUID CLSID_ITStorage = { 0x5d02926a, 0x212e, 0x11d0, { 0x9d, 0xf9, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xec } };
static const GUID IID_ITStorage = { 0x88cc31de, 0x27ab, 0x11d0, { 0x9d, 0xf9, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xec} };

HRESULT CItsFile::OpenITS(PCSTR pszFile) 
{
    HRESULT hr = CoCreateInstance(CLSID_ITStorage, NULL, CLSCTX_INPROC_SERVER,
        IID_ITStorage, (void **) &m_pITStorage);
    if (!SUCCEEDED(hr)) 
        return hr;

    hr = m_pITStorage->StgOpenStorage(FileNameToWCHAR(pszFile), NULL, 
        STGM_READ | STGM_SHARE_DENY_WRITE, NULL, 0, &m_pStorage);
    return hr;
}
    
WCHAR* CItsFile::FileNameToWCHAR(PCSTR pszFile)
{            
    if (!m_pwTmpFile)
        m_pwTmpFile = (WCHAR*) LocalAlloc(LMEM_FIXED, MAX_PATH * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, pszFile, -1, m_pwTmpFile, MAX_PATH * sizeof(WCHAR));
    return m_pwTmpFile;
}

//////////////////////// CStringSubFile ////////////////////////////////////////////

CStringSubFile::CStringSubFile(CItsFile* pif)
{
    m_pif = pif;
    m_pStream = NULL;
}

CStringSubFile::~CStringSubFile()
{
    if (m_pStream)
        m_pStream->Release();
}

bool CStringSubFile::GetString(DWORD offset, char* pszDst, int cbDst)
{
    ASSERT(offset);
    ASSERT(m_pif->GetStorage());   // if NULL, CHM file hasn't been opened

    if (!m_pStream) {
        if (FAILED(m_pif->OpenStream("#STRINGS", &m_pStream, m_pif->GetStorage(), STGM_READ))) {
            *pszDst = 0;    
            return false;    
        }    
    }

    /* Now we get to the dicey part. Strings are stored in a CHM file as
    normal zero-terminated strings. Strings are stored in pages of 4K
    -- no string will cross the 4K boundary. ITSS caches a decompression
    block,  which means reading the string subfile will probably toss
    any current decompression block on medium to large sized files, and
    cause a 64K-128K file read. That happens anyway in medium to large
    sized files where HTML files are referenced that are outside of the
    current decompression range, so it's not as bad as it sounds.
    Frequent access of the #STRINGS subfile interspersed with access of
    HTML files should be cached. The occassional read is not that much
    of a performance hit. In the following code we read a chunk at a
    time and then look for the null-terminating byte. If the subfile was
    cached in 4K blocks, then once the appropriate block was read, the
    string would always  be available. */

    LARGE_INTEGER liOffset;
    liOffset.LowPart = (int) offset;
    liOffset.HighPart = 0;
    if (FAILED(m_pStream->Seek(liOffset, STREAM_SEEK_SET, NULL))) {
        *pszDst = 0;    
        return false;    // probably means offset is out of range
    }

    ULONG cbRead;
    int cbTotalRead = 0;
    const int CB_READ_BLOCK = 64;
    while (cbTotalRead + CB_READ_BLOCK < cbDst &&
            SUCCEEDED(m_pStream->Read(pszDst, CB_READ_BLOCK, &cbRead))) {
        if (!cbRead) {
            *pszDst = 0;
            return false;   // bad #STRINGS file
        }    
        for (ULONG iPos = 0; iPos < cbRead; iPos++) {
            if (!pszDst[iPos]) 
                return TRUE;    // we got our zero-terminated string
        }
        pszDst += cbRead;
    }    
    *pszDst = 0;
    return false;   // file reading error, most likely
}
