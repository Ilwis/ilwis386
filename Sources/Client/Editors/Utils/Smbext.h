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
/*smbext.h
   Interface for ExtendedSymbol
   Based on symbol.h
   More functionality than currently available in the Representation.	
   by Wim Koolhoven, february 1997
   (c) Computer Department ITC
	Last change:  WK   18 Sep 98    4:16 pm
*/
#ifndef EXTSYMBOL_H
#define EXTSYMBOL_H

#ifndef SYMBOL_H
#include "Client\Editors\Utils\SYMBOL.H"
#endif

#ifndef FIELDCOLOR_H
#include "Client\FormElements\fldcolor.h"
#endif

class _export ExtendedSymbol
{
  friend class _export FieldExtendedSymbol;
  friend class _export PointMapRprEditForm;
  friend class _export Line;
public:
  ExtendedSymbol();
  ExtendedSymbol(const MapView& view, const char* sSection);
  ExtendedSymbol(const Representation& rpr, long iRaw);
  ExtendedSymbol(const RepresentationPtr* rpr, long iRaw);
  ~ExtendedSymbol();
  void Store(RepresentationClass*, long iRaw);
  void drawSmb(CDC*, HPALETTE, zPoint);
  zPoint pntText(CDC* cdc, zPoint);
  void WriteLayer(MapView& view, const char* sSection) const;
	void Read(const char* sSection, const char* sPrefix, const FileName& filename);
	void Write(const char* sSection, const char* sPrefix, const FileName& filename);
  int iSize;
  double rRotation;
  Color color() const { return col; }
  Color colorFill() const { return fillCol; }
  void SetColor(Color clr) { col = fillCol = clr; }
	void Resize(double rFactor);
private:
  double rPixelPerPoint(HDC hDC);
  enum { smSIMPLE, smFONT } smType;
  Color col;
  // Simple:
  SymbolType smb;
  int iWidth;
  Color fillCol;
  // Font:
  String sFaceName;
  short cSmb;
//HDC hdcDummy;  // remove !
//double rDummy; // remove !
};

class _export FieldSymbolTypeSimple: public FieldOneSelect
{
public:
  FieldSymbolTypeSimple(FormEntry* parent, long*, const String& sType);
  void create();
  int idSimple;
private:
  String sInitFont;  
};

class _export FieldSimpleSymbolSimple: public FieldOneSelect
{
public:
  FieldSimpleSymbolSimple(FormEntry* parent, long*);
  bool fDrawIcon();
protected:  
  void create();
private:
  bool DrawObject(long value, DRAWITEMSTRUCT* dis);
};

class _export FieldFontSymbolSimple: public FormEntry
{
public:
  FieldFontSymbolSimple(FormEntry* parent, short*);
  ~FieldFontSymbolSimple();
  void SetFont(CFont*); // old one is automatically deleted !
private:  
  void create();
  virtual void show(int sw);            // set show or hide
  CListBox* lb;
  void StoreData();
  CFont* fnt;
  short* cSmb;
};

class _export FieldExtendedSymbol: public FieldGroup
{
public:
  FieldExtendedSymbol(FormEntry* parent,
      ExtendedSymbol& symbol, bool fAskSize=true, bool fAskColor=true);
private:	              
  int FieldSymbolCallBack(void *);
  void StoreData();
  ExtendedSymbol& smb;
  FieldSymbolTypeSimple *fsts;
  FieldGroup *fgSimple, *fgFont;
  FieldFontSymbolSimple *ffss;
  FieldSimpleSymbolSimple *fsss;
  FieldFillColor *ffc;
  int TypeChanged(void *);
  long iType;
};


#endif // EXTSYMBOL_H
