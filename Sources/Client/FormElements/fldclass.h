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
/* Interface for FieldClass
   by Jelle Wind, sept.1996
   (c) Ilwis System Development ITC
	Last change:  MS   11 Sep 97   12:42 pm
*/
#ifndef FIELDCLASS_H
#define FIELDCLASS_H
#include "Engine\Domain\dmsort.h"

class FieldClassSimple: public FieldOneSelect
{
public:
  _export FieldClassSimple(FormEntry* parent, long* iClass, DomainSort* ds);
  _export ~FieldClassSimple();
  void _export SetVal(long iClass);
  void StoreData();	
	void DrawItem(Event* dis);
protected:
  void create();
private:
//  int SelChanged(zEvent*);
  bool DrawObject(long value, DRAWITEMSTRUCT* dis);
  bool fDrawIcon();
  DomainSort* ds;
  Representation rpr;
};


class _export FieldClass: public FieldGroup
{
public:
  FieldClass(FormEntry* parent, const String& sQuestion, long* iClass, DomainSort* ds);
  void SetVal(long iClass) { fcs->SetVal(iClass); }
  void StoreData() { fcs->StoreData(); }
private:
  FieldClassSimple* fcs;  
};

#endif FIELDCLASS_H




