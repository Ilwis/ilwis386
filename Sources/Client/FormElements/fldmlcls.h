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
/* Interface for FieldMultiClass
   by Wim Koolhoven, august 198
   (c) Ilwis System Development ITC
	Last change:  WK   24 Aug 98    6:27 pm
*/
#ifndef FIELDMULTICLASS_H
#define FIELDMULTICLASS_H
#include "Client\FormElements\selector.h"
#include "Engine\Domain\dmclass.h"

class  GeneralClassSelector;
class  MultiClassSelector;
class  ClassSquenceSelector;
class  FieldMultiClass;

class _export GeneralClassSelector: public BaseSelector
{
public:
  String sName(int id);
  void SetSel(int id, bool fSel);
  void SetSel(const String&, bool fSel);
  bool fSel(int id);
  int iGetSelected(IntBuf& buf);
  void create();
protected:
  GeneralClassSelector(FormEntry* par);
  void DrawItem(Event* dis);
  void StoreData() {}
};

class MultiClassSelector: public GeneralClassSelector
{
public:
  _export MultiClassSelector(FormEntry* par, DomainClass*);
  void create();
private:
  DomainClass* dc;
};

class ClassSequenceSelector: public GeneralClassSelector
{
public:
  _export ClassSequenceSelector(FormEntry* par, DomainClass*, Array<int>& arr);
  _export ~ClassSequenceSelector();
  void create();
  FormEntry* CheckData();
  String sName(int id);
  void StoreData();
  int idActive();
  int Add(long iRaw);
  int Add(const String&);
  void Del(int id);
  int iClasses() const;
  String sGetText();
private:
  DomainClass* dc;
  int mouseButtonDown(Event *);
  int mouseButtonUp(Event *);
  int mouseMove(Event *);
  bool fDrag;
  int idDrag;
  zCursor *cur, curArrow;
  Array<int>& arr;
};

class FieldMultiClass: public FieldGroup
{
public:
  _export FieldMultiClass(FormEntry* par, const String& sQuestion, DomainClass*, Array<int>& arr);
  void create();
  String sGetText();
private:
  int Add(Event*);
  int Del(Event*);
  MultiClassSelector* mcs;
  ClassSequenceSelector* cacs;
};



#endif // FIELDMULTICLASS_H




