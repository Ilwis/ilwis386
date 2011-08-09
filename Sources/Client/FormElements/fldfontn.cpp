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
/* FieldFontName
   by Wim Koolhoven, may 1998
   (c) Ilwis System Develpment ITC
	Last change:  WK   12 Jun 98    5:16 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldfontn.h"
#include "Headers\Hs\Userint.hs"


FieldFontName::FieldFontName(FormEntry* parent, const String& sQuestion,
                             String* sFontName, unsigned long iAttr)
: FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  ffns = new FieldFontNameSimple(this, sFontName, iAttr);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

FieldFontNameSimple::FieldFontNameSimple(FormEntry* parent, String* sFontName, unsigned long iAttr)
  : FieldOneSelect(parent,&iFont,true), sFont(sFontName), iAttributes(iAttr)
{
  iFont = 0;
  psn->iMinWidth = 2.0 * FLDNAMEWIDTH;
}

FieldFontNameSimple::~FieldFontNameSimple()
{
}

void FieldFontNameSimple::SetVal(const String& s)
{
	String str = s;
	int iRes = ose->FindStringExact(-1, str.c_str()); // select font
	if (iRes == CB_ERR)
	{
		str = String("%S.ttfont", s);   // try selecting font as truetype
		iRes = ose->FindStringExact(-1, str.c_str());
	}
	ose->SetCurSel(iRes);
}

int FAR PASCAL AddNewFont(const LOGFONT* lplf, const TEXTMETRIC* lptm,
                               unsigned long nFontType, LPARAM lp)
{
	bool fAcceptTrueType=true;
	bool fAcceptFixed=true;
	FieldFontNameSimple * fns = (FieldFontNameSimple*) lp;
	fAcceptTrueType = (fns->iAttributes & FieldLogFont::faTRUETYPE) == FieldLogFont::faTRUETYPE;
	fAcceptFixed = (fns->iAttributes & FieldLogFont::faFIXED) == FieldLogFont::faFIXED;
  
	if (fAcceptTrueType )
		if ( (TRUETYPE_FONTTYPE & nFontType) == 0)
			return 1;
	if ( fAcceptFixed )
	{
		bool fY = (lplf->lfPitchAndFamily & FIXED_PITCH) == FIXED_PITCH	;
		if ( !fY)
			return 1;
	}
  LOGFONT* lf = (LOGFONT*)lplf;
  OneSelectEdit* ose = fns->ose;
	String str;
	if ( TRUETYPE_FONTTYPE & nFontType )
		str = String("%s.ttfont", lf->lfFaceName);
	else
		str = String("%s", lf->lfFaceName);
  ose->AddString(str.sVal());
  return 1;
}                               

void FieldFontNameSimple::create()
{
  FieldOneSelect::create();
  HDC hdc = GetDC(NULL);
  EnumFontFamilies(hdc, NULL, &AddNewFont, (LPARAM)this);
  ReleaseDC(NULL, hdc);
//  ose->selection(*sFont);
  SetVal(*sFont);
}

void FieldFontNameSimple::StoreData()
{
  FieldOneSelect::StoreData();
  int id = ose->GetCurSel();
  if (id >=0)
  {
    CString sFnt;
    ose->GetLBText(id, sFnt);
		char* s = const_cast<char *>(strrchr(sFnt, '.'));
		if (s)
			*s = 0;
    *sFont = String(sFnt);
  }
}


FieldLogFont::FieldLogFont(FormEntry* parent, LOGFONT* logfont, unsigned long iAttributes)
: FieldGroup(parent)
, lf(logfont)
{
	sFont = lf->lfFaceName;			
	fBold = lf->lfWeight > FW_MEDIUM;
	fItalic = lf->lfItalic ? true : false;
	iHeight = abs(lf->lfHeight);
	new FieldFontName(this, TR("&Font Name"), &sFont, iAttributes & 0x07);
	new FieldInt(this, TR("Font &Size"), &iHeight, ValueRange(5,100), true); 
	iRotation = lf->lfOrientation / 10;
	if (iAttributes & faROTATION) 
		new FieldInt(this, TR("&Rotation"), &iRotation, ValueRange(0,359), true);
	new CheckBox(this, TR("&Bold"), &fBold);
	new CheckBox(this, TR("&Italic"), &fItalic);
}

void FieldLogFont::StoreData()
{
	FieldGroup::StoreData();
	lstrcpy(lf->lfFaceName, sFont.c_str());
	if (fBold)
		lf->lfWeight = FW_BOLD;
	else
		lf->lfWeight = FW_NORMAL;

	lf->lfItalic = fItalic;
	lf->lfHeight = -iHeight;

	lf->lfEscapement = iRotation * 10;
	lf->lfOrientation = iRotation * 10;
}






