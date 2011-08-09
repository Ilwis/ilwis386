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
// res.c
// Resources for ILWIS 2.0
// october 1994, by Wim Koolhoven
// (c) ILWIS Department ITC
#define RESEXPORT _export

#include "Headers\toolspch.h"
#include "Client\Base\Res.h"
#include "Client\ilwis.h"


//static HINSTANCE IlwWinApp()->m_hInstance;

/*BOOL WINAPI DllMain(HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
  IlwWinApp()->m_hInstance = (HINSTANCE)hModule;

  return TRUE;
}*/

zIcon::zIcon() :
 hi(0)
{
}

zIcon::zIcon(StockIcon sico)
{
  hi = LoadIcon(NULL,MAKEINTRESOURCE(sico));
//  if (hi==NULL)
//    throw WindowsError(GetLastError());
  fRes = 0;
}

zIcon::zIcon(const char* s)
{
  hi = LoadIcon(  IlwWinApp()->m_hInstance, s);
//  if (hi==NULL)
//    throw WindowsError(GetLastError());
    
  fRes = 1;
}

bool zIcon::fLoad(const String& s)
{
  hi = LoadIcon(IlwWinApp()->m_hInstance, s.c_str());
	if (hi == NULL)
		return false;

	return true;
}

zIcon::~zIcon()
{
  if (fRes && hi) DestroyIcon(hi);
}

zCursor::zCursor(StockCursor sc)
{
  hc = LoadCursor(NULL,MAKEINTRESOURCE(sc));
  if (hc==NULL)
    throw WindowsError(GetLastError());
  fRes = 0;
}

zCursor::zCursor()
{
  hc = LoadCursor(NULL,MAKEINTRESOURCE(Arrow));
  if (hc==NULL)
    throw WindowsError(GetLastError());
  fRes = 0;
}

zCursor::zCursor(const char* s)
{
  hc = LoadCursor(IlwWinApp()->m_hInstance, s);
  fRes = 1;
}

zCursor::~zCursor()
{
  if (fRes && hc) DestroyCursor(hc);
}

zCursor::zCursor(const zCursor& cur)
{
  fRes = 0;
  hc = cur;
}

zCursor& zCursor::operator = (const zCursor& cur)
{
  fRes = 0;
  hc = cur;
  return *this;
}


void LoadIlwisButtonBitmap(const char* sName, CBitmap& bmp)
{
	HBITMAP hb = ::LoadBitmap(IlwWinApp()->m_hInstance, sName);
  if (hb==NULL)
    throw WindowsError(GetLastError());

	bmp.Attach(hb);
}

void LoadIlwisButtonBitmap(UINT uID, CBitmap& bmp)
{
	HBITMAP hb = ::LoadBitmap(IlwWinApp()->m_hInstance, MAKEINTRESOURCE(uID));
  if (hb==NULL)
    throw WindowsError(GetLastError());

	bmp.Attach(hb);
}

void UnloadIlwisButtonBitmap(CBitmap& bmp)
{
	HBITMAP hb = (HBITMAP)bmp.Detach();
	if (hb)
		DeleteObject(hb);
}

void UnloadIlwisButtonBitmap(HBITMAP hb)
{
	if (hb)
		DeleteObject(hb);
}
