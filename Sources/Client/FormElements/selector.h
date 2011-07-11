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
/* BaseSelector
   by Wim Koolhoven, june 1994
  (c) Ilwis System Development ITC
	Last change:  WK   11 Aug 98   12:10 pm
*/
#ifndef SELECTOR_H
#define SELECTOR_H

class BaseSelector: public FormEntry
{
public:
  _export ~BaseSelector();
  void _export show(int);
  void SetFocus() { lb->SetFocus(); } // overriden
  virtual void StoreData()=0;
  void setDirty() { lb->Invalidate(); }
  void Disable() { lb->EnableWindow(FALSE); }
protected:
  _export BaseSelector(FormEntry* par);
  void _export create();
  virtual String sName(int id)=0;
  virtual void _export DrawItem(Event* ev);
  OwnerDrawListBox* lb;
  DWORD style;
  ItemDrawer idw;
};

class StringArrayLister: public BaseSelector
{
public:
  _export StringArrayLister(FormEntry* fe, const Array<String>& array);
  virtual void StoreData();
  String sGetText();
  void _export resetContent(const Array<String>& str);
	void _export AddString(const String& s, int iWhere=iUNDEF);
  int _export iGetSingleSelection();
  String _export sGetSelectedString();
  void _export Remove(int index);
  void _export Clear();
  virtual String _export sName(int id);
private:  
   int OnKeyUp( Event* ev);	
	virtual void _export DrawItem(Event* ev);
  void create();
  Array<String> as;
};

#endif // SELECTOR_H




