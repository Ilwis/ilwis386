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
/* Interface for FieldMapList, ...
   by Wim Koolhoven, october 1994
   (c) Ilwis System Development ITC
	Last change:  WK   12 Aug 98   11:28 am
*/
#ifndef FIELDLIST_H
#define FIELDLIST_H
#include "Client\FormElements\selector.h"

class _export GeneralMapSelector;
class _export MultiMapSelector;
class _export MapSquenceSelector;
class _export FieldMultiMap;

class _export FieldMapList: public FieldDataTypeC
{
public:
  FieldMapList(FormEntry* parent, const String& sQuestion,
	   Parm *prm, bool fCreate = false);
  FieldMapList(FormEntry* parent, const String& sQuestion,
	   String *sName, bool fCreate = false);
private:
  void init();
  int CreateMapList(void*);
  String sNewName;
};

class _export MapListSelector: public BaseSelector
{
public:
  MapListSelector(FormEntry* par);
  ~MapListSelector();
  void FillArray(Array<FileName>&);
  void StoreData() {}
protected:
  void create();
  virtual String sName(int id);
};

class APPEXPORT Mask;

class _export MaskSelector: public BaseSelector
{
public:
  MaskSelector(FormEntry* par, Mask* msk);
  ~MaskSelector();
  void StoreData();
protected:
  void create();
  virtual String sName(int id);
  Mask* mask;
};

class _export FormCreateMapList: public FormWithDest
{
public:
  FormCreateMapList(CWnd* wPar, String* sML);
private:
  int exec();
  int CallBackName(Event *);
  String* sMapList;  
  String sNewName;
  String sDescr;
  FieldDataTypeCreate* fml;
  FieldMultiMap* fmm;
//  MapListSelector* mls;
  StaticText* stRemark;
  Array<Map> arr;
};


class _export GeneralMapSelector: public BaseSelector
{
public:
  String sName(int id);
  void SetSel(int id, bool fSel);
  void SetSel(const String&, bool fSel);
  bool fSel(int id);
  int iGetSelected(IntBuf& buf);
  void create();
protected:
  GeneralMapSelector(FormEntry* par);
  void DrawItem(Event* dis);
  void RecalcHScrollSize(); // call this everytime the size of the horizontal scrollbar should be re-estimated
  void StoreData(); // empty implementation!
};

class _export MultiMapSelector: public GeneralMapSelector
{
public:
  MultiMapSelector(FormEntry* par, const GeoRef&, const Domain&);
  void create();
private:
  bool fOK(const FileName& fn);
  GeoRef gr;
  Domain dm;
};

class _export MapSequenceSelector: public GeneralMapSelector
{
public:
	MapSequenceSelector(FormEntry* par, Array<Map>& arr);
	~MapSequenceSelector();
	void create();
	FormEntry* CheckData();
	void StoreData();
	int idActive();
	int Add(const String&);
	int Add(const Map&);
	void Del(int id);
	int Del(const Map&);
	int iMaps() const;
	Map map(int id) ;
	String sGetText();
	
private:
	bool fCheckDomains(const Map& mp, String& sErr) const;
	bool fCheckGeoRefs(const Map& mp, String& sErr) const;
	
	int mouseButtonDown(Event*);
	int mouseButtonUp(Event*);
	int mouseMove(Event*);
	
	bool fDrag;
	int idDrag;
	zCursor *cur, curArrow;
	Array<Map>& arr;
};

class _export FieldMultiMap: public FieldGroup
{
public:
  FieldMultiMap(FormEntry* par, const String& sQuestion, Array<Map>& arr);
  void create();
  String sGetText();
private:
  int Add(Event*);
  int Del(Event*);
  MultiMapSelector* mcs;
  MapSequenceSelector* cacs;
};



#endif // FIELDLIST_H




