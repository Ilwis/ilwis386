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
/* Form entries for Columns
   by Wim Koolhoven
   (c) Ilwis System Development
	Last change:  WK   15 Oct 98    6:47 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "Engine\Table\tblinfo.h"
#include "Engine\Table\tblview.h"
#include "Client\FormElements\objlist.h"

/*
FieldColumn::FieldColumn(FormEntry* p, const String& sQuestion,
              Table* t, Parm *prm)
  : FormEntry(p, prm, true), tbl(t->ptr()), dmTypes(0)
{
  _sName = prm->sVal();
  _psName = 0;
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldOneSelect(this, 0);
  if (st)
    fld->Align(st, AL_AFTER);
  setHelpItem(htpUiColumn);
}
*/
FieldColumn::FieldColumn(FormEntry* p, const String& sQuestion,
              const Table& t, String *psName, long types)
  : FormEntry(p, 0, true),
    tbl(t.ptr()), table(t), dmTypes(types)
{
  _psName = psName;
  _sName = *psName;
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldOneSelect(this, 0);
	fld->psn->iMinWidth *= 1.5;
  if (st)
    fld->Align(st, AL_AFTER);
  setHelpItem(htpUiColumn);
};


FieldColumn::FieldColumn(FormEntry* p, const String& sQuestion,
              TableView* tv, String *psName, long types)
  : FormEntry(p,0, true),
    tbl(reinterpret_cast<TablePtr*>(tv)), dmTypes(types)
{
  _psName = psName;
  _sName = *psName;
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldOneSelect(this, 0);
	fld->psn->iMinWidth *= 1.5;
  if (st)
    fld->Align(st, AL_AFTER);
  setHelpItem(htpUiColumn);
};


void FieldColumn::create()
{
  CreateChildren();
  String sCol;
  if (tbl != NULL)
    FillWithColumns(tbl);
  sCol = _sName;
  sCol &= ".clm";
  fld->ose->SelectString(-1, sCol.sVal());
}

FormEntry* FieldColumn::CheckData()
{
  if (fShow()) {
    int id = fld->ose->GetCurSel();
  String sCol = _sName.sUnQuote() + ".clm"; // strip quotes from column name
  CString s = sCol.sVal();
  if (id >= 0)
      fld->ose->GetLBText(id, s);
    else
      fld->ose->GetWindowText(s);
    if (s == "")
      return this;
    _sName = String(s.Left(s.GetLength()-4)).sQuote();  // strip ".clm" and add quotes
  }
  return FormEntry::CheckData();
}

void FieldColumn::StoreData()
{
  CheckData();

  if (fShow()) {
    if (_psName != 0)
      *_psName = _sName;
    else
      _prm->Replace(_sName);
  }
  FormEntry::StoreData(); // do not use FieldOneSelect::StoreData!
}

String FieldColumn::sGetText()
{
    if ( !fShow() ) return "";
    String s1;
    if ( st ) s1=st->sGetText();
    return String("%S:\t%S", s1, sName());
}

void FieldColumn::FillWithColumns(Table * t)
{
  FillWithColumns(t->ptr());
}

void FieldColumn::FillWithColumns(TablePtr *t)
{
  String sCol;
  
  tbl = t;
  if (0 == dynamic_cast<TableView*>(t))
    table.SetPointer(t);
	if (0 == fld->ose)
		return;
  fld->ose->ResetContent();
  if (tbl) {
    for (int i = 0; i < tbl->iCols(); ++i) {
      Column col = tbl->col(i);
      if (!col.fValid())
        continue;
      if (dmTypes) {
        Domain dm = col->dm();
        bool fAdd = false;
        if ((dmTypes & dmCLASS) && 0 != dm->pdc()) fAdd = true;
        else if ((dmTypes & dmIDENT) && 0 != dm->pdid()) fAdd = true;
        else if ((dmTypes & dmTIME) && 0 != dm->pdtime()) fAdd = true;
        else if ((dmTypes & dmVALUE) && 0 != dm->pdv() && 0 == dm->pdbool()) fAdd = true;
        else if ((dmTypes & dmGROUP) && 0 != dm->pdgrp()) fAdd = true;
        else if ((dmTypes & dmIMAGE) && 0 != dm->pdi()) fAdd = true;
        else if ((dmTypes & dmPICT)  && 0 != dm->pdp()) fAdd = true;
        else if ((dmTypes & dmCOLOR) && 0 != dm->pdcol()) fAdd = true;
        else if ((dmTypes & dmBOOL)  && 0 != dm->pdbool()) fAdd = true;
        else if ((dmTypes & dmBIT)   && 0 != dm->pdbit()) fAdd = true;
        else if ((dmTypes & dmSTRING)&& 0 != dm->pds()) fAdd = true;
				else if ((dmTypes & dmCOORD) && 0 != dm->pdcrd()) fAdd = true;
        else if ((dmTypes & dmUNIQUEID)&& 0 != dm->pdUniqueID()) fAdd = true;				
        if (!fAdd)
          continue;
      }
      sCol = col->sName();
      sCol &= ".clm";
      fld->ose->AddString(sCol.sVal());
    }
  }
  sCol = _sName;
	if ( sCol != "")
	{
		sCol &= ".clm";
		fld->ose->SelectString(-1, sCol.scVal());
	}
	else
		fld->ose->SetCurSel(0);
}

void FieldColumn::FillWithColumns(const FileName& fnTbl)
{
  String sCol;
//tbl = t;
	if (0 == fld->ose)
		return;
  fld->ose->ResetContent();
  if (fnTbl.fValid()) {
    TableInfo tblinf(fnTbl);
    for (int i = 0; i < tblinf.iCols(); ++i) {
      if (dmTypes) {
        DomainType dmtCol = tblinf.aci[i].dmt();
        bool fAdd = false;
        if ((dmTypes & dmCLASS) && (dmtCol == dmtCLASS || dmtCol == dmtGROUP))
           fAdd = true;
        else if ((dmTypes & dmIDENT) && (dmtCol == dmtID)) 
          fAdd = true;
        else if ((dmTypes & dmVALUE) && (dmtCol == dmtVALUE)) 
          fAdd = true;
        else if ((dmTypes & dmVALUE) && (dmtCol == dmtBOOL)) 
          fAdd = true;
        else if ((dmTypes & dmVALUE) && (dmtCol == dmtBIT)) 
          fAdd = true;
        else if ((dmTypes & dmGROUP) && (dmtCol == dmtGROUP)) 
          fAdd = true;
        else if ((dmTypes & dmIMAGE) && (dmtCol == dmtIMAGE)) 
          fAdd = true;
        else if ((dmTypes & dmPICT)  && (dmtCol == dmtPICTURE)) 
          fAdd = true;
        else if ((dmTypes & dmCOLOR) && (dmtCol == dmtCOLOR)) 
          fAdd = true;
        else if ((dmTypes & dmSTRING)&& (dmtCol == dmtSTRING))
          fAdd = true;
				else if (( dmTypes & dmCOORD) && ( dmtCol == dmtCOORD))
					fAdd = true;
				else if (( dmTypes & dmUNIQUEID) && ( dmtCol == dmtUNIQUEID))
					fAdd = true;				
        if (!fAdd)
          continue;
      }
      sCol = tblinf.aci[i].sName();
      sCol &= ".clm";
      fld->ose->AddString(sCol.sVal());
    }
  }
	if ( _sName != "")
	{
		sCol = _sName;
		sCol &= ".clm";
		if (CB_ERR == fld->ose->SelectString(-1, sCol.sVal()))
			fld->ose->SetCurSel(0);
	}
	else
		fld->ose->SetCurSel(0);
}

void FieldColumn::FillWithColumns(const FileName& fnTbl, long types)
{
	Table tbl(fnTbl);
	if ( tbl.fValid() )
	{
		dmTypes = types;
		FillWithColumns(fnTbl);
	}		
}

void FieldColumn::FillWithColumns(const FileName& fnTbl, const Domain& dmCol)
{
  String sCol;
//tbl = t;
	if (0 == fld->ose)
		return;
  fld->ose->ResetContent();
  if (fnTbl.fValid()) {
    TableInfo tblinf(fnTbl);
    for (int i = 0; i < tblinf.iCols(); ++i) 
			if (tblinf.aci[i].dm() == dmCol) {
        sCol = tblinf.aci[i].sName();
        sCol &= ".clm";
        fld->ose->AddString(sCol.sVal());
			}
  }
	if ( _sName != "")
	{
		sCol = _sName;
		sCol &= ".clm";
		if (CB_ERR == fld->ose->SelectString(-1, sCol.sVal()))
			fld->ose->SetCurSel(0);
	}
	else
		fld->ose->SetCurSel(0);
}

void FieldColumn::SetFocus()
{
  fld->SetFocus();
  _frm->ilwapp->setHelpItem(htp());
}

void FieldColumn::SetSelected(int iIndex)
{
	if ( iIndex < fld->ose->GetCount())
		fld->ose->SetCurSel(iIndex);
}
