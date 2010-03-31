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
/* FieldMapInMapList
   by Wim Koolhoven, January 1998
   (c) ILWIS System Development ITC
*/

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldmapml.h"

FieldMapInMapList::FieldMapInMapList(FormEntry* parent, const String& sQuestion, const MapList& maplist, long* iMapNr)
: FormEntry(parent, 0, true), ml(maplist), iMap(iMapNr)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldOneSelect(this, iMapNr);
  if (st)
    fld->Align(st, AL_AFTER);
  fld->psn->iMinWidth = FLDNAMEOUTPUTWIDTH ;
}

void FieldMapInMapList::SetVal(int iMapNr)
{
  iMapNr -= ml->iLower();
  fld->ose->SetCurSel(iMapNr);
}

void FieldMapInMapList::DrawItem(Event *dis)
{ 
   fld->DrawItem(dis); 
}

void FieldMapInMapList::create()
{
  CreateChildren();
  String sMap;
  int iMax = ml->iUpper();
  for (int i = ml->iLower(); i <= iMax; ++i) {
//    sMap = ml[i]->sName(true);
    sMap = ml->sDisplayName(i);
		sMap &= ".mpr";
    fld->ose->AddString(sMap.scVal());
  }
  long iMapNr = *iMap - ml->iLower();
  fld->ose->SetCurSel(iMapNr);
  fld->ose->setNotify(this, (NotifyProc)&FieldMapInMapList::DrawItem, WM_DRAWITEM);
}

void FieldMapInMapList::StoreData()
{
  CheckData();
  fld->StoreData();
  *iMap += ml->iLower();
}

void FieldMapInMapList::SetFocus()
{
  fld->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}







