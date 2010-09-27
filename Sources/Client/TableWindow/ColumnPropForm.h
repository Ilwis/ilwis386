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
/* ColumnManageForm, ColumnSelector
   by Wim Koolhoven, may 1996
  (c) Ilwis System Development ITC
	Last change:  WK   14 Jul 97    9:02 am
*/  

class ColumnPropForm: public FormWithDest
{
public:
  ColumnPropForm(CWnd* parent, ColumnView& cv, int* iPos, bool fNew=false);
private:
  int exec();
  int BreakLink(Event*);
  int MakeUpToDate(Event*);
  int CallBackDomainChange(Event*);
//  int CallBackStepChange(Event*);
  int ValueRangeCallBack(Event*);
  int SetDefaults(Event*);
  int CreateDomain(Event*);
  virtual FormEntry* feDefaultFocus();
  void AdditionalInfoButton();
  int AdditionalInfo(Event*);
  FieldString* fsExpr, *fsDesc;
  FieldDomainC* fd;
  FieldValueRange* fvr;
  FieldInt *fiDec;
//  FieldReal* frStep;
  StaticText* stMinMax;
  ColumnView& _cv;
  bool fReadOnly;
  String sExpr, sDomain;
  ValueRange vr;
  ILWIS::Time begin;
  ILWIS::Time end;
  ILWIS::Duration duration;
  FieldTime *ft1;
  FieldTime *ft2;
  FieldTime *ftStep;


  bool fSetDescFromExpr;
  Column col; 
  StaticText* stRemark;
  bool fWasReadOnly;
};
