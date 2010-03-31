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
/* Win Tab driver for ilwis
// by Wim Koolhoven
// (c) ILWIS System Development ITC
	Last change:  WK    6 Mar 97   12:09 pm
*/

#include "Headers\toolspch.h"
#include "Client\Editors\Digitizer\WINTAB.H"

typedef UINT (API *WTInf)(UINT,UINT,LPVOID);
typedef BOOL (API *WTOverl)(HCTX,BOOL);
typedef HCTX (API *WTOp)(HWND, LPLOGCONTEXT, BOOL);
typedef BOOL (API *WTClos)(HCTX);
typedef BOOL (API *WTPack)(HCTX, UINT, LPVOID);

extern "C" {

static HINSTANCE hWinTab=0;
static UINT (API *lpWTInfo)(UINT,UINT,LPVOID)=0;
static BOOL (API *lpWTOverlap)(HCTX,BOOL)=0;
static HCTX (API *lpWTOpen)(HWND, LPLOGCONTEXT, BOOL)=0;
static BOOL (API *lpWTClose)(HCTX)=0;
static BOOL (API *lpWTPacket)(HCTX, UINT, LPVOID)=0;

static void LoadWinTab()
{
	if ( hWinTab ) return;
  hWinTab = LoadLibrary("wintab32.dll");
  if (hWinTab <= NULL)
    hWinTab = 0;
}

//extern "C" int APIENTRY
//DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
//{
//
//
//  if (dwReason == DLL_PROCESS_DETACH)
//	{
//     if ( hWinTab )
//     {
//				FreeLibrary(hWinTab);
//		 }
//	}
//	return 1;   // ok
//}


UINT API WTInfo(UINT wCat, UINT nIndex, LPVOID lp)
{
 	if (0 == lpWTInfo) 
	{         
		if (0 == hWinTab)    
			LoadWinTab();      
		if (0 != hWinTab)    
			 lpWTInfo = (WTInf)GetProcAddress(hWinTab, (LPCSTR)MAKELONG(ORD_WTInfo,0));
    if (0 == lpWTInfo)         
      return 0;          
  }  
  return (*lpWTInfo)(wCat,nIndex,lp);
}

BOOL API WTOverlap(HCTX hctx, BOOL f)
{
  if (0 == lpWTOverlap) 
	{         
    if (0 == hWinTab)    
      LoadWinTab();      
    if (0 != hWinTab)    
      lpWTOverlap = (WTOverl)GetProcAddress(hWinTab, (LPCSTR)MAKELONG(ORD_WTOverlap,0));
    if (0 == lpWTOverlap)         
      return 0;          
  }  
  return (*lpWTOverlap)(hctx,f);
}

HCTX API WTOpen(HWND hwnd, LPLOGCONTEXT lplc, BOOL f)
{
  if (0 == lpWTOpen) 
	{         
    if (0 == hWinTab)    
      LoadWinTab();      
    if (0 != hWinTab)    
      lpWTOpen = (WTOp)GetProcAddress(hWinTab, (LPCSTR)MAKELONG(ORD_WTOpen,0));
    if (0 == lpWTOpen)         
      return 0;          
  }  
  return (*lpWTOpen)(hwnd,lplc,f);
}

BOOL API WTClose(HCTX hctx)
{
  if (0 == lpWTClose) 
	{         
    if (0 == hWinTab)    
      LoadWinTab();      
    if (0 != hWinTab)    
      lpWTClose = (WTClos)GetProcAddress(hWinTab, (LPCSTR)MAKELONG(ORD_WTClose,0));
    if (0 == lpWTClose)         
      return 0;          
  }  
  return (*lpWTClose)(hctx);
}

BOOL API WTPacket(HCTX hctx, UINT w, LPVOID lp)
{
  if (0 == lpWTPacket) {         
    if (0 == hWinTab)    
      LoadWinTab();      
    if (0 != hWinTab)    
      lpWTPacket = (WTPack)GetProcAddress(hWinTab, (LPCSTR)MAKELONG(ORD_WTPacket,0));
    if (0 == lpWTPacket)         
      return 0;          
  }  
  return (*lpWTPacket)(hctx,w,lp);
}

}
