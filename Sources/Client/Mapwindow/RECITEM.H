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
/* RecItem, RecItemMap, RecItemPolMap, 
   RecItemTable, RecItemColumn,
   RecItemInt, RecItemCoord
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   10 Feb 97    3:17 pm
*/

#ifndef RECITEM_H
#define RECITEM_H

class RecItem;
class RecItemMap;
class RecItemTable;
class RecItemColumn;
class RecItemInt;
class RecItemCoord;
class RecItemCoordSystem;
class RecItemGeoRef;
class RecItemAnimation;
class RecItemRasterMap;
class RecItemCollection;
class RecItemCollectionLayer;
class ObjectCollection;


namespace ILWIS {
	class NewDrawer;
	class DrawerTool;
	//class ComplexDrawer;
}

typedef Array<RecItem*> RecItemArray;

class _export RecItem
{
public:
  enum AddType{atNORMAL, atANIMATION, atCOLLECTION, atCOLLECTIONLAYER};
  virtual ~RecItem();
  virtual String sName() ;
  virtual int Configure(CWnd*);
  virtual int Changed();  // even if overridden, call this one
  virtual const String& sValue(int iWidth = 0);  // text to display
  virtual void SetValue(const char*);
  virtual void PutVal(const String& s) {};
  virtual long iValue();
  virtual const CoordWithCoordSystem& crdValue();
  virtual FileName fnObj() ;
  int AddSelfToArray(RecItemArray&);
  int AddSelfAlwaysToArray(RecItemArray&);
  RecItemMap* AddMap(BaseMap);
  RecItemRasterMap* AddRasterMap(Map);
  RecItemCoordSystem* AddCoordSystem(const CoordSystem&);
  RecItemTable* AddTable(Table, bool fAllowEdit);
  RecItemTable* AddTable(Table tbl);
  RecItemGeoRef* AddGeoRef(const GeoRef&);
  RecItemAnimation *AddAnimation(const IlwisObject& obj, ILWIS::ComplexDrawer *drawr=0);
  RecItemCollection *AddCollection(const ObjectCollection& obj, ILWIS::ComplexDrawer *drawr=0);
  RecItemCollectionLayer *RecItem::AddCollectionLayer(const IlwisObject& obj,  ILWIS::ComplexDrawer *drawr=0);
//  RecItemTable* AddTable(TableView, bool fAllowEdit = false);
  bool fShowRec() { return fShow; }
  void ShowRec(bool fSR) { fShow = fSR; }
  bool fAllowEdit() ;
  void AllowEdit(bool fEdit) ;
  virtual void setAssociatedDrawerTool(ILWIS::DrawerTool *drawr, const String& targetName);
  ILWIS::DrawerTool *getAssociatedTool() const { return associatedDrawerTool; }
protected:
  void updateView(const IlwisObject& obj);
  CoordWithCoordSystem cwcs;
  RecItem(RecItem* parent);
  SListP<RecItem> children;
  ILWIS::DrawerTool *associatedDrawerTool;
  RecItem* riParent;
  unsigned fShow	 :1;
  unsigned fShowChildren :1;
  unsigned fValid	 :1;
  unsigned fResValid	 :1;
  unsigned fStrValid	 :1;
  unsigned fEditable	 :1;
//unsigned fContinuous	 :1;  // used by RecItemCoord
  String sVal;
};
#ifdef RECITEM_C
static SList<RecItem> DummyRecItemSList;
#pragma option -Jgx
#endif

class _export RecItemMap: public RecItem
{
public:
  RecItemMap(RecItem* parent, BaseMap);
  virtual int Configure(CWnd*);
  virtual String sName() ;
  virtual long iValue();   // pixel value
  virtual const String& sValue(int iWidth = 0); // meaning
  virtual FileName fnObj() ;
  virtual void PutVal(const String& s);
  virtual void setAssociatedDrawerTool(ILWIS::DrawerTool *drawr, const String& targetName);
protected:
  virtual const BaseMap& map()  { return _map; }
protected:
  BaseMap _map;
  long iVal;
};

class _export RecItemRasterMap: public RecItemMap
{
public:
  RecItemRasterMap(RecItem* parent, Map);
};

class _export RecItemAnimation: public RecItemMap
{
public:
	RecItemAnimation(RecItem* parent, const MapList& mapl, ILWIS::ComplexDrawer *drawr);
    RecItemAnimation(RecItem* parent, const ObjectCollection& col, ILWIS::ComplexDrawer *drawr);
    virtual FileName fnObj() ;
  //virtual void setAssociatedDrawerTool(ILWIS::DrawerTool *drawr, const String& targetName);
protected:
  virtual String sName() ;
  const BaseMap& map() ;
private:
	ILWIS::ComplexDrawer *animationDrawer;
  MapList mpl;
  ObjectCollection collection;
};

class _export RecItemCollectionLayer: public RecItemMap
{
public:
    RecItemCollectionLayer(RecItem* parent, const ObjectCollection& col, ILWIS::ComplexDrawer *drawr);
    virtual FileName fnObj() ;
  //virtual void setAssociatedDrawerTool(ILWIS::DrawerTool *drawr, const String& targetName);
protected:
  virtual String sName() ;
  const BaseMap& map() ;
private:
  ILWIS::ComplexDrawer *collectionDrawer;
  ObjectCollection collection;
};

class _export RecItemCollection: public RecItem
{
public:
    RecItemCollection(RecItem* parent, const ObjectCollection& col, ILWIS::ComplexDrawer *drawr);
    virtual FileName fnObj() ;
  //virtual void setAssociatedDrawerTool(ILWIS::DrawerTool *drawr, const String& targetName);
protected:
  virtual String sName() ;
private:
    ObjectCollection collection;
};

class _export RecItemTable: public RecItem
{
public:
  RecItemTable(RecItem* parent, Table);
//  RecItemTable(RecItem* parent, TableView*);
  virtual int Configure(CWnd*);
  virtual String sName() ;
  virtual long iValue();   // record number
  virtual const String& sValue(int iWidth = 0); // key value
  virtual void SetValue(const char*);
  RecItemColumn* AddColumn(Column, bool fAllowEdit);
  RecItemColumn* AddColumn(Column col);
  virtual FileName fnObj() ;
protected:
  const Table& tbl() const { return _tbl; }
  long iRec;
private:
  Table _tbl;  
};

class _export RecItemExtra: public RecItem
{
public:
  RecItemExtra(RecItem* parent, const BaseMap& mp);
  virtual int Configure(CWnd*);
  virtual String sName() ;
  virtual const String& sValue(int iWidth = 0);
  virtual const CoordWithCoordSystem& crdValue();
  virtual FileName fnObj() ;
  void setAssociatedDrawerTool(ILWIS::DrawerTool *drawr, const String& targetName);
protected:
 private:
  BaseMap bmp;
  IlwisObject::iotIlwisObjectType type;
};

class _export RecItemColumn: public RecItem
{
public:
  RecItemColumn(RecItem* parent, Column);
  virtual int Configure(CWnd*);
  virtual String sName() ;
  virtual long iValue();
  virtual const String& sValue(int iWidth = 0);
  virtual void SetValue(const char*);
  virtual const CoordWithCoordSystem& crdValue();
  virtual FileName fnObj() ;
  virtual void PutVal(const String& s);
  void setAssociatedDrawerTool(ILWIS::DrawerTool *drawr, const String& targetName);
protected:
  const Column& col() const { return _col; }
  long iRec;
  long iVal;
private:
  Column _col;  
};

class _export RecItemInt: public RecItem    // used in TableRecord
{
public:
  RecItemInt(long);
  virtual int Configure(CWnd*);
  virtual String sName() ;
  virtual const String& sValue(int iWidth = 0);
  virtual long iValue();
  virtual void SetValue(const char*);
  void SetValue(long);
protected:
  String sNam;
  long iVal;
};

class _export RecItemCoord: public RecItem  // used in PixelInfo
{
public:
  RecItemCoord(RecItem* parent, const CoordWithCoordSystem&);
  virtual int Configure(CWnd*);
  virtual String sName() ;
  virtual const String& sValue(int iWidth = 0);
  virtual const CoordWithCoordSystem& crdValue();
  virtual void SetValue(const char*);
  void SetValue(const CoordWithCoordSystem&);
  void setAssociatedDrawerTool(ILWIS::DrawerTool *drawr, const String& targetName);

//bool fContinual() { return fContinuous; }
//void Continual(bool fCont) { fContinuous = fCont; }
protected:
  String sNam;
};


class _export RecItemCoordSystem: public RecItemCoord
{
public:
  RecItemCoordSystem(RecItem* parent, const CoordSystem&);
  virtual const CoordWithCoordSystem& crdValue();
  virtual FileName fnObj() ;
};

class RecItemGeoRef: public RecItem
{
public:
  RecItemGeoRef(RecItem* parent, const GeoRef&);
  virtual String sName() ;
  virtual const String& sValue(int iWidth = 0);
  virtual FileName fnObj() ;
  RowCol rcValue();
private:
  GeoRef grf;
};


#endif // RECITEM_H
