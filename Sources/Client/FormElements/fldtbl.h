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
/* FieldTable
   by Wim Koolhoven, november 1995
   Copyright Ilwis System Development ITC
	Last change:  WK    7 May 97    3:44 pm
*/
   
#ifndef FIELDTBL_H
#define FIELDTBL_H

#include "Client\FormElements\flddom.h"

class FieldTable: public FieldDataType
{
public:
  _export FieldTable(FormEntry* parent, const String& sQuestion,
           Parm *prm)
    : FieldDataType(parent, sQuestion, prm, ".TBT",
                    true)
    { setHelpItem(htpUiTbl); }

  _export FieldTable(FormEntry* parent, const String& sQuestion,
           String *sName, String sExtraExt=".TBT")
    : FieldDataType(parent, sQuestion, sName, sExtraExt,
                    true)
    { setHelpItem(htpUiTbl); }
  void _export SetDomain(const Domain& dm);
};

class FieldTableCreate: public FieldDataTypeCreate
{
public:
  _export FieldTableCreate(FormEntry* parent, const String& sQuestion,
                       Parm *prm)
    : FieldDataTypeCreate(parent, sQuestion, prm, ".TBT",
                    false)
    { setHelpItem(htpUiTbl); }

  _export FieldTableCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".TBT",
                    false)
    { setHelpItem(htpUiTbl); }
};


class FieldTableC: public FieldDataTypeC
{
public:
  _export FieldTableC(FormEntry* fe, const String& sQuestion, 
               String* sTable);
  _export FieldTableC(FormEntry* fe, const String& sQuestion, 
               String* sTable, const Domain& dm);      // list only this domain
  void SetDomain(const Domain& dm);
private:  
  String sNewName;
  String sDom;
  int CreateTable(Event*);
};

class FormCreateTable: public FormWithDest
{
public:
  _export FormCreateTable(CWnd* wPar, String* sTable, const String& sDom);
  _export ~FormCreateTable();
private:    
  int CallBackName(Event*);
  int CallBackDomain(Event*);
  int exec();
  String* sTable;
  String sNewName;
  String sDescr;
  String sDomain;
  long iRecs;
  FieldDataTypeCreate* fdt;  
  FieldDomainC* fdc;
  FieldInt* fiRecs;
  StaticText* stRemark;
  CWnd* wParent;
};

#endif // FIELDTBL_H




