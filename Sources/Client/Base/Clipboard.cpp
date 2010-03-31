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
#include "Client\Base\ZappToMFC.h"

zClipboard::zClipboard(CWnd *win) 
{
	if (win==0) return;
	w=win;
	res=0;
}

void zClipboard::add(char *s, UINT uFormat) 
{
	res=OpenClipboard(w->m_hWnd);
	if (res!=0) 
    {
		char *a;
		HANDLE h;
		h=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,strlen(s)+2);
		a=(char*)GlobalLock(h);
		strcpy(a,s);
		GlobalUnlock(h);
		SetClipboardData(uFormat,h);
		CloseClipboard();
	}
}

void zClipboard::clear() 
{
	res=OpenClipboard(w->m_hWnd);
	if (res!=0) EmptyClipboard();
	CloseClipboard();
}

char* zClipboard::getText(UINT uFormat)
{
	if (!IsClipboardFormatAvailable(uFormat)) return 0;
	if (!(OpenClipboard(*w))) return 0;

	HANDLE hClipTextMem;
	hClipTextMem = GetClipboardData(uFormat);
	char *retChar = new char[(unsigned int)GlobalSize(hClipTextMem)];
	LPSTR clipT =(char*) GlobalLock(hClipTextMem);
	lstrcpy((LPSTR) retChar, clipT);
	GlobalUnlock(hClipTextMem);
	CloseClipboard();
	return retChar;
}

bool zClipboard::isTextAvail() 
{
	return (IsClipboardFormatAvailable(CF_TEXT) != 0);
}
