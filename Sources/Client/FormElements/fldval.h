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
// $Log: FLDVAL.H $
// Revision 1.3  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.2  1997/08/07 15:00:26  Wim
// FieldVal now works on DomainValueRangeStruct instead of Domain.
// For value domains the value range is used.
//
/* FieldVal
   by Wim Koolhoven, may 1995
   Copyright Ilwis System Development ITC
	Last change:  WK    7 Aug 97    4:51 pm
*/
   
#ifndef FIELDVAL_H
#define FIELDVAL_H

class FieldVal: public FormEntry{
public:
  _export FieldVal(FormEntry* parent, const String& sQuestion,
	   const DomainValueRangeStruct& dvrs, String *sName, bool fEditor=false);
  void create();
  void StoreData();         
  void SetFocus() { fld->SetFocus(); }   
  void setHelpItem(const HelpTopic& htp)
    { fld->setHelpItem(htp); }
  void SetCallBack(NotifyProc np)
    { fld->SetCallBack(np); }
  void SetCallBack(NotifyProc np, CallBackHandler* cb)
    { fld->SetCallBack(np,cb); }
  void SetWidth(short iWidth);
private:
  void Fill();
  int NewDomainItem(Event*);
  Domain dm;
  StaticTextSimple *st;
  FormEntry *fld;
  bool fVal;
  long iVal;
  double rVal;
  String* sVal;
  bool fEditor;
};  

#endif // FIELDVAL_H




