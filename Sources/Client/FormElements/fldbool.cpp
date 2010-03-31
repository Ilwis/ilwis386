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
/* FieldBool
   by Wim Koolhoven, june 1994
   (c) Ilwis System Development ITC
	Last change:  MS   23 Sep 97    3:01 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldbool.h"

FieldBoolSimple::FieldBoolSimple(FormEntry* p, long* iVal, DomainBool* dbool)
  : FieldOneSelect(p, iVal)
{
  db = dbool;
}

FieldBoolSimple::~FieldBoolSimple()
{
}

void FieldBoolSimple::create()
{
  FieldOneSelect::create();
  ose->ResetContent();
  for (int i = 0; i <= 2; ++i) {
		int iRaw = 2-i;
    ose->AddString(db->sValueByRaw(iRaw,0).sVal());
    ose->SetItemData(i,iRaw);
  }
  ose->SetFont(frm()->fnt);  // Yes on !
  SetVal(*val);
}

void FieldBoolSimple::StoreData()
{
  int id = ose->GetCurSel();
  *val = 2 - id;
}

void FieldBoolSimple::SetVal(long iVal)
{
	ose->SetCurSel(2-iVal);
}

bool FieldBoolSimple::fDrawIcon()
{
    return false;
}


FieldBool::FieldBool(FormEntry* p, const String& sQuestion,
             long* iVal, DomainBool* db)
   : FieldGroup(p)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  fbs = new FieldBoolSimple(this, iVal, db);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

