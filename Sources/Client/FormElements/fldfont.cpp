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
/* ui/fldfont.c
   FieldFont
   by Wim Koolhoven, june 1997
   Copyright Ilwis System Development 1997
	Last change:  MS   21 Nov 97    4:17 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldfont.h"
#include <commdlg.h>

FieldFont::FieldFont(FormEntry* fe, const String& sQuestion, LOGFONT* fntspec,
            CWnd* wnd, HDC hdc)
: FieldGroup(fe), pb(0), bw(wnd), fsStore(fntspec), fs(*fntspec), hDC(hdc)
{
  pb = new PushButton(this, sQuestion, (NotifyProc)&FieldFont::SelectFont, true);
}

int FieldFont::SelectFont(void* Ev)
{
//  bw->SetDialogHelpTopic("ilwismen\font.htm");
  CHOOSEFONT chf;
  chf.Flags = CF_INITTOLOGFONTSTRUCT;
  chf.iPointSize = fs.lfHeight;
  chf.lStructSize = sizeof(CHOOSEFONT);
  chf.hwndOwner = bw->m_hWnd;
  chf.lpLogFont = &fs;
  chf.hDC = hDC;
  chf.Flags |= CF_BOTH | CF_LIMITSIZE | CF_PRINTERFONTS | CF_SHOWHELP;
  chf.rgbColors = 0;
  chf.lCustData = 0;
  chf.lpfnHook = 0;
  chf.lpTemplateName = 0;
  chf.hInstance = 0;
  chf.lpszStyle = 0;
  chf.nFontType = 0;
  chf.nSizeMin = 6;
  chf.nSizeMax = 96;
  ChooseFont(&chf);
  return 1;
}

void FieldFont::StoreData()
{
  *fsStore = fs;
}








