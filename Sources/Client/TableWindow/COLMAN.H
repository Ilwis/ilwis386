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
	Last change:  WK   11 Jul 97    6:45 pm
*/  

#ifndef ILWCOLMAN_H
#define ILWCOLMAN_H
#include "Client\FormElements\selector.h"

class _export ColumnSelector;
class _export ColumnManageForm;


class _export ColumnSelector: public BaseSelector
{
public:
  void StoreData();
  ColumnSelector(FormEntry* par, ColumnManageForm*, TablePtr*);
  ~ColumnSelector();
  int idSelect();
  void Fill();
  Column col(int id);
protected:
  void create();
  long DrawItem(DRAWITEMSTRUCT* dis);
private:
  int mouseButtonDown(Event* Evt);
  int mouseButtonUp(Event* Evt);
  bool fDrag;
  int idDrag;
  zCursor *cur, curArrow;
  String sName(int id);
  TablePtr* tbl;
  ColumnManageForm* cmf;
};

class _export ColumnManageForm: public FormWithDest
{
public:
  ColumnManageForm(TableDoc*, TableView*);
  int Select(Event*);  
  int OnKeyDown( Event*);
  int Prop(Event*);
private:
  int Add(Event*);
  int Remove(Event*);
  int MakeUpToDate(Event*);
  ColumnSelector* cs;
  TableDoc* tbldoc;
  TableView* tvw;
  PushButton *pbAdd, *pbProp, *pbRemove, *pbMakeUpToDate;
  StaticText *stTypeName, *stDescr, *stRemark;
};




#endif // ILWCOLMAN_H

