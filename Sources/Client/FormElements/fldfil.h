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
/* FieldFilter
   by Wim Koolhoven, april 1996
   Copyright Ilwis System Development ITC
	Last change:  WK   10 Feb 97    1:56 pm
*/
   
#ifndef FIELDFIL_H
#define FIELDFIL_H

#ifndef OBJLIST_H
#include "Client\FormElements\objlist.h"
#endif
              
class FieldFilter: public FieldDataType
{
public:
  _export FieldFilter(FormEntry* fe, const String& sQuestion, 
              String* sFilter, long types=0x1F)
  : FieldDataType(fe, sQuestion, sFilter, new FilterLister(types), true) {}
};

class FieldFilterC: public FieldDataTypeC
{
public:
  _export FieldFilterC(FormEntry* fe, const String& sQuestion, 
               String* sFilter, long types=filLINEAR)
  : FieldDataTypeC(fe, sQuestion, sFilter, new FilterLister(types), true,
     (NotifyProc)&FieldFilterC::CreateFilter),
  filTypes(types) { sNewName = *sFilter; }
private:  
  long filTypes;
  String sNewName;
  int _export CreateFilter(void *);
};

class FormCreateFilter: public FormWithDest
{
public:
  _export FormCreateFilter(CWnd* wPar, String* sFil);
  _export ~FormCreateFilter();
private:    
  int exec();
  int CallBackName(Event *);
	int CallBackRowColSize(Event*);
  FieldDataTypeCreate* fdm;
	FieldInt *fiRows;
	FieldInt *fiCols;
  String* sFilter;
  String sNewName;
  String sDescr;
  int iRgVal;
  int iRows, iCols;
  bool fReal, fNameOK, fRowColOk;
  StaticText* stRemark;
  CWnd* wParent;
};  

#endif // FIELDFIL_H




