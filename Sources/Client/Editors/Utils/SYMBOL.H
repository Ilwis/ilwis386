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
/*symbol.h
// Interface for Symbol
// by Wim Koolhoven, july 1994
// (c) Computer Department ITC
	Last change:  MS   11 Sep 97    2:38 pm
*/
#ifndef SYMBOL_H
#define SYMBOL_H

enum SymbolType { smbCircle, smbSquare, smbDiamond,
                  smbDeltaUp, smbDeltaDown,
		  smbPlus, smbMinus, smbCross, smbIcon };
		  
class _export Symbol
{
public:
  Symbol();
  Symbol(const Representation&, long iRaw);
  ~Symbol();
  void drawSmb(CDC*, HPALETTE, zPoint) const;
  zPoint pntText(CDC* cdc, zPoint) const;
  int iSize, iWidth;
  Color col, fillCol;
  SymbolType smb;
  HICON hIcon;
};

class _export FieldSymbolSimple: public FieldOneSelect
{
public:
  FieldSymbolSimple(FormEntry* parent, long*, HICON* hIcon);
  ~FieldSymbolSimple();
  bool fDrawIcon();
protected:  
  int SelChanged(Event*);
  void create();
private:
  bool DrawObject(long value, DRAWITEMSTRUCT* dis);
  HICON* hicon;
};

class _export FieldSymbol: public FieldGroup
{
public:
  FieldSymbol(FormEntry* parent, const String& sQuestion,
	      long* symbol, HICON* hIcon);
  void SetCallBack(NotifyProc np)
    { fss->SetCallBack(np); }
private:
  FieldSymbolSimple* fss;        	      
};

#endif // SYMBOL_H
