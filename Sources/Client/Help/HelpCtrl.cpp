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
// HelpCtrl.cpp: implementation of the HelpCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\stdafx.h"
#include "Client\Help\HHCTRL.H"
#include "Client\Help\helpctrl.h"
#include "Headers\htmlhelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HelpCtrl::HelpCtrl()
{
	fFirst = true;
}

HelpCtrl::~HelpCtrl()
{
}


void HelpCtrl::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
                                   LPCTSTR lpszTargetFrameName, 
                                   CByteArray& baPostedData,
                                   LPCTSTR lpszHeaders, BOOL* pfCancel)
{
	if (fFirst) {
		fFirst = false;
		*pfCancel = FALSE;
		return;
	}
	int iLen = strlen(lpszURL);
	if (0 == strnicmp("Javascript", lpszURL, 10))
	{
		char* s0 = new char[iLen];
		strcpy(s0, "ilwis.chm::");
		char* str = strstr(s0, "::") + 2;
		const char* s = lpszURL + 11;	 // minus "Javascript:"
		for (;;) 
		{
			if ('_' == *s) {	 // replace double underscore with slash
				++s;
				if ('_' == *s) { 
					++s;
					*str++ = '/';
				}
				else
					*str++ = '_';
			}
			else if ('.' == *s) // .Click()
				break;
			else
				*str++ = *s++;
		}
		strcpy(str, ".htm");
		::HtmlHelp(::GetDesktopWindow(), s0, HH_DISPLAY_TOPIC, NULL);
		delete s0;
	}
	else if (0 == strnicmp("its:", lpszURL, 4)) 
	{
		char* s = new char[iLen+1];
		strcpy(s, lpszURL+4); // minus "its:"
		strcat(s, ">sec");
		::HtmlHelp(::GetDesktopWindow(), s, HH_DISPLAY_TOPIC, NULL);
		delete s;
	}
	*pfCancel = TRUE;
}
