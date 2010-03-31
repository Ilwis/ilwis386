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
/* Interface for FieldColor
   by Wim Koolhoven, june 1994
   (c) Ilwis System Develpment ITC
	Last change:  WK   11 Jun 98    6:28 pm
*/
#ifndef FIELDCOLOR_H
#define FIELDCOLOR_H

#include "Client\FormElements\selector.h"

Color _export clrStd(int iNr);
void _export WEP_Color();

class FieldColorSimple: public FieldOneSelect
{
public:
  _export FieldColorSimple(FormEntry* parent, Color *);
  _export ~FieldColorSimple();
  void _export SetVal(Color);
  int CreateColor(void *);
protected:
  void _export create();
  bool _export DrawObject(long value, DRAWITEMSTRUCT* dis);
  bool _export fDrawIcon();
  int SelChanged(Event *);
  int idCustom;
};


class FieldColor: public FieldGroup
{
public:
  _export FieldColor(FormEntry* parent, const String& sQuestion, Color*);
  void _export SetVal(Color clr) { fcs->SetVal(clr); }
  void create();
private:
  FieldColorSimple* fcs;  
  OwnButtonSimple *pbCreate;
  int CreateColor(void *);
};

class FieldFillColorSimple: public FieldColorSimple
{
public:
  _export FieldFillColorSimple(FormEntry* parent, Color* col)
  : FieldColorSimple(parent,col) {}
  void _export StoreData();
private:
  void _export create();
};

class FieldFillColor: public FieldGroup
{
public:
  _export FieldFillColor(FormEntry* parent, const String& sQuestion,
		 Color* col);
  void SetVal(Color clr) { fcs->SetVal(clr); }
private:
  FieldFillColorSimple* fcs;
};

class ColorSelector: public BaseSelector
{
public:  
  _export ColorSelector(FormEntry* parent, Color*, bool fTransparant=false);
  void StoreData();
  void _export SetVal(Color);
  int _export CustomColor(Event *);

protected:  
  void create();
  virtual String sName(int id);
  void DrawItem(Event* dis);
private:
  void DoubleClick(Event *);
  Color* clr;
  bool fTrans;
  int idCustom;
};

#endif // FIELDCOLOR_H




