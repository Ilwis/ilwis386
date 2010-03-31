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
/*
   ui/exprerr.c
   Source for ExpressionErrorBox
   juan 1997, by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    3 Sep 97    7:53 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\exprerr.h"
#include "Client\ilwis.h"

void _export ShowExpressionError(const String& sTitle,
                     const String& sWhat,
                     const String& sExpr1, const String& sExpr2,
                     const String& sWhere)
{
  ExpressionErrorBox(AfxGetMainWnd(), sTitle, sWhat, sExpr1, sExpr2, sWhere);
}

ExpressionErrorBox::ExpressionErrorBox(CWnd* wPar, const String& sTitle,
                     const String& sWhat,
                     const String& sExpr1, const String& sExpr2,
                     const String& sWhere)
: FormBaseDialog(wPar, sTitle,
   fbsSHOWALWAYS|fbsMODAL|fbsNOCANCELBUTTON|fbsBUTTONSUNDER|fbsNOBEVEL)
{
  Array<String> as;
  Split(sWhat, as, "\n");
	HICON hic = IlwWinApp()->LoadStandardIcon(IDI_HAND);
  StaticIcon *ic = new StaticIcon(root, hic, true);
  StaticText *st;
	FieldGroup *fg = new FieldGroup(root);
  for (unsigned int i=0; i < as.iSize(); ++i)
	{
    st = new StaticText(fg, as[i]);
	}

  String s;
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = 100;
	logFont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS | FF_ROMAN | FF_MODERN | FF_SCRIPT;
	lstrcpy(logFont.lfFaceName, "Courier New");
	fnt.CreatePointFontIndirect(&logFont);	
	
  s = sExpr1.sLeft(80);
  st = new StaticText(fg, s);
  st->Font(&fnt, false);
  s = sExpr2.sLeft(80);
	if ( s != "" )
	{
		st = new StaticText(fg, s);
		st->Font(&fnt, false);
	}
  as.Resize(0);
  Split(sWhere, as, "\n");
  for (unsigned int i=0; i < as.iSize(); ++i)
	{
		String ss = as[i];
    st = new StaticText(root, ss);
	}
	fg->Align(ic, AL_AFTER);
  create();
}

ExpressionErrorBox::~ExpressionErrorBox()
{
}

int ExpressionErrorBox::exec()
{
  return 0;
}





