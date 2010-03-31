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
/* Field2DimTable
   by Wim Koolhoven, march 1997
   Copyright Ilwis System Development ITC
	Last change:  WK    7 Mar 97   10:41 am
*/
   
#ifndef FIELD2DIMTBL_H
#define FIELD2DIMTBL_H

class _export FieldTable2DimCreate: public FieldDataTypeCreate
{
public:
  FieldTable2DimCreate(FormEntry* parent, const String& sQuestion,
                       Parm *prm)
    : FieldDataTypeCreate(parent, sQuestion, prm, ".TA2",
                    true)
    {}

  FieldTable2DimCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".TA2",
                    true)
    {}
};


class _export FormCreateTable2Dim: public FormWithDest
{
public:
  FormCreateTable2Dim(CWnd* wPar, String* sTable2Dim, const String& sDom);
  ~FormCreateTable2Dim();
private:    
  int CallBackName(Event *);
  int exec();
  String* sTable2Dim;
  String sNewName;
  String sDescr;
  String sDom1, sDom2, sDomFill;
  ValueRange vr;
  FieldDataTypeCreate* fdt;  
  StaticText* stRemark;
  CWnd* wParent;
};

#endif // FIELD2DIMTBL_H




