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
/* $Log: /ILWIS 3.0/FormElements/fldprj.h $
 * 
 * 3     6/21/99 3:33p Visser
 * //->/*
 * 
 * 2     21-06-99 3:26p Martin
 * added export
// Revision 1.3  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.2  1997/09/12 15:29:07  Wim
// Constructors for FieldProjection and FieldEllipsoid no longer inline
//
/* FieldItem, FieldProjection, FieldDatum, FieldEllipsoid
   by Wim Koolhoven, march 1996
  (c) Ilwis System Development ITC
	Last change:  WK   12 Sep 97    5:24 pm
*/
#ifndef FLDPRJ_H
#define FLDPRJ_H
//#include "Client\FormElements\odlb&cb.h"
//#include "dat.h"
#include "Engine\SpatialReference\prj.h"

class FieldItem: public FormEntry
{
public:
  _export FieldItem(FormEntry*, String *psName);
  _export ~FieldItem();             
  void SetFocus() { lbObject->SetFocus(); } // overriden
  void StoreData();	         	// store result in destination
  void show(int);
  void create();                        // create entry
  String _export sValue();
protected:
  int DblClkObject(Event*);
  String *sName;
  OwnerDrawListBox* lbObject;
};

class  FieldProjection: public FieldItem
{
public:
  _export FieldProjection(FormEntry* parent, String *psName);
  void create();                        // create entry
};    

class  FieldEllipsoid: public FieldItem
{
public:
  _export FieldEllipsoid(FormEntry* parent, String *psName);
  void create();                        // create entry
};    

class  FieldDatum: public FieldItem
{
public:
  _export FieldDatum(FormEntry* parent, String *psName);
  void create();                        // create entry
};    

class FieldDatumArea: public FieldItem
{
public:
  _export FieldDatumArea(FormEntry* parent, String *psName);
  bool _export fInit(const String& sDatum);
};    

  
#endif // FLDPRJ_H





