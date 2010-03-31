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
/* recitsel.c
// RecItemSelector
// by Wim Koolhoven, june 1994
// (c) Computer Department ITC
	Last change:  WK   22 Apr 97    4:15 pm
*/

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\RECITEM.H"
#include "Client\Mapwindow\RECITSEL.H"


RecItemSelector::RecItemSelector(FormEntry* par,RecItem* ri)
  : BaseSelector(par), riBase(ri)
{
  psn->iMinWidth = 150;
}

void RecItemSelector::create()
{
  RecItemArray ria;
  riBase->AddSelfAlwaysToArray(ria);
  BaseSelector::create();
  lb->ResetContent();
  for (int i = 0; i < ria.iSize(); ++i) {
    String s = ria[i]->sName();
    int ind = lb->AddString(s.sVal());
    lb->SetItemData(ind,(LPARAM)ria[i]);
    if (ria[i]->fShowRec())
      lb->SetSel(ind, true);
  }
}

void RecItemSelector::StoreData()
{
  int iNr = lb->GetCount();
  for (int id = 0; id < iNr; ++id) {
    RecItem* ri = (RecItem*) lb->GetItemData(id);
    bool fSel = lb->GetSel(id) > 0;
    ri->ShowRec(fSel);
  }
}

String RecItemSelector::sName(int id)
{
  if (id < 0)
    return "?";
  RecItem* ri = (RecItem*) lb->GetItemData(id);
  String s;
//  s = ri->sType();
//  s &= " ";
  s = ri->sName();
  return s;
}


