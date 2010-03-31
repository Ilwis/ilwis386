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
	Last change:  WK   11 Aug 98   10:46 am
*/

#ifndef FLDCOL_H
#define FLDCOL_H
#include "Engine\Table\tbl.h"

class FieldColumn: public FormEntry
  // a variation on FieldDataType
{
public:
/*  _export FieldColumn(FormEntry*, const String& sQuestion,
              const Table&, Parm *prm);*/
  _export FieldColumn(FormEntry*, const String& sQuestion,
              const Table&, String *psName, long types = 0);
  _export FieldColumn(FormEntry*, const String& sQuestion,
              TableView*, String *psName, long types = 0);
  void SetVal(const String& sVal)
    { fld->ose->SelectString(-1, sVal.scVal()); }
  virtual _export FormEntry* CheckData();       // validate entry value
  void _export FillWithColumns(Table* tbl);
  void _export FillWithColumns(TablePtr *tbl);
  virtual void _export FillWithColumns(const FileName& fnTbl=FileName());
  virtual void _export FillWithColumns(const FileName& fnTbl, const Domain& dmCol);
  virtual void _export FillWithColumns(const FileName& fnTbl, long types);  
  void _export SetSelected(int iIndex);
  void DrawItem(Event *dis) { fld->DrawItem(dis); }
  virtual void _export create();                // create entry
  virtual void _export StoreData();             // store result in destination
  String sName() { CheckData(); return _sName; }
  void _export SetFocus();                     // overriden
  void SetCallBack(NotifyProc np) 
		{ FormEntry::SetCallBack(np); fld->SetCallBack(np); }
  void SetCallBack(NotifyProc np, CallBackHandler* cb)
    { FormEntry::SetCallBack(np,cb); fld->SetCallBack(np,cb); }
  String _export sGetText();
protected:
  TablePtr* tbl;
  Table table;  // maintain a copy of table to avoid that it is removed from memory
  String _sName, *_psName;              // local storage and destination
  StaticTextSimple *st;
  FieldOneSelect *fld;
  long dmTypes;
};

#endif // FLDCOL_H





