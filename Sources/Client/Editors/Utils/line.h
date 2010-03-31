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
/* $Log: /ILWIS 3.0/UserInterfaceObjects/line.h $
 * 
 * 12    4-03-03 16:27 Retsios
 * Added function to resize the symbol of a line.
 * 
 * 11    19-02-03 13:51 Retsios
 * Added a Resize function - for the time being this is only needed for
 * the legend and representation of classes, where the original effect is
 * too "thick" compared to the layout.
 * 
 * 10    20-03-01 20:18 Koolhoven
 * added class FieldLineTypeSimple as friend to class Line
 * 
 * 9     23-02-01 18:30 Koolhoven
 * FieldLineType::sGetText() added
 * 
 * 8     4-05-00 10:00 Koolhoven
 * Added drawRectangle()
 * replaced zDisplay* with CDC*
 * 
 * 7     3-05-00 19:30 Koolhoven
 * Added option to ask color in FieldLine also
 * 
 * 6     4-04-00 17:46 Koolhoven
 * Added Read() and Write() functions to load/store in ODF
 * 
 * 5     4-04-00 10:06 Koolhoven
 * Adapted FieldLine to show correctly when used under a CheckBox
 * 
 * 4     3-04-00 14:46 Koolhoven
 * Added some include statements to allow include without too much
 * requirements on user
 * 
 * 3     6/02/99 3:59p Martin
 * 
 * 2     5/26/99 12:56p Martin
 * 
 * 1     5/26/99 11:41a Martin
// Revision 1.3  1998/09/18 15:16:46  Wim
// Dummy's removed
//
// Revision 1.2  1998-09-18 14:48:58+01  Wim
// removed hdcLast and rPixPerMM from class Line
//
// Revision 1.1  1998-09-16 18:33:58+01  Wim
// 22beta2
//
/* line.h
   Interface for Line
   by Wim Koolhoven, march 1998
   (c) ILWIS System Development, ITC
	Last change:  WK   18 Sep 98    4:16 pm
*/
#ifndef LINE_H
#define LINE_H

#ifndef EXTSYMBOL_H
#include "Client\Editors\Utils\Smbext.h"
#endif



enum LineDspType { ldtNone,
                   ldtSingle, ldtDouble, ldtTriple,
                   ldtDot, ldtDash, ldtDashDot, ldtDashDotDot,
                   ldtBlocked, ldtSymbol
                 };

/*
   None: no line
     - optional symbols (smb, rDist)
     - optional support line (clrFill, rWidth)
   Single, fSupportLine:
     - solid line (clr)
     - background line (clrFill, rWidth)
     - optional symbols (smb, rDist)
   Single, !fSupportLine:
     - solid line (clr, rWidth)
     - optional symbols (smb, rDist)
   Double, Triple:
     - double/triple line (clr)
     - Total width (rWidth)
     - background filling (clrFill)
   Dot, Dash, DashDot, DashDotDot:
     - simple line of specified type (clr)
     - optional support line (clrFill, rWidth)
   Blocked:
     - double line (clr)
     - Total width (rWidth)
     - blocks in two colors (clr, clrFill, rDist)

   The support line is drawn first (brush)
   then the line itself (pen)
   and the symbols on top of them.
*/

class _export Line;
class _export FieldLine;


class _export Line
{
  friend struct LinePosition;
  friend class FieldLine;
	friend class FieldLineTypeSimple;
public:
  Line(const Representation& rpr, long iRaw);
  Line(const RepresentationPtr* rpr, long iRaw);
  Line(Color color=Color(0,0,0), double rWidth=0, LineDspType typ=ldtSingle);
  Line(LineDspType, Color, Color clrFill);
  Line(const ExtendedSymbol&, double rDist);
  Line(const Line&);
  ~Line();
  void operator=(const Line&);
  void drawLine(CDC*, zPoint*, int iCount);
  void drawLine(CDC*, zPoint pFrom, zPoint pTo);
  void drawLine(HDC, zPoint*, int iCount);
  void drawLine(HDC, zPoint pFrom, zPoint pTo);
	void drawRectangle(CDC*, const CRect&);
  void Store(RepresentationClass*, long iRaw);
	void Read(const char* sSection, const char* sPrefix, const FileName& filename);
	void Write(const char* sSection, const char* sPrefix, const FileName& filename);
  Color& clrLine() { return clr; }
  Color& clrLineFill() { return clrFill; }
  static String sConv(LineDspType ldt);
  static LineDspType ldtConv(const String&);
  static int psConv(LineDspType); // PenStyle is a zApp enum
  bool fSymbolType() const { return ldtSymbol == ldt; }
	void Resize(double rFactor);
	void ResizeSymbol(double rFactor);
private:
  void init(const RepresentationPtr* rpr, long iRaw);
  inline int iSize(HDC, double rMM);     // convert from mm to pixels
  double rSize(HDC, double rMM);  // convert from mm to pixels
//  double rSizeX(HDC, double rMM); // convert from mm to pixels
//  double rSizeY(HDC, double rMM); // convert from mm to pixels
  void drawSimpleLine(HDC, zPoint*, int iCount);
  void drawDoubleLine(HDC, zPoint*, int iCount);
  void drawSupportLine(HDC, zPoint*, int iCount);
  void drawSymbolLine(HDC, zPoint*, int iCount);
  void drawBlockedLine(HDC, zPoint*, int iCount);
  void drawBlock(HDC, zPoint*, int iCount);
  void drawSymbol(HDC, zPoint);
  void getBoundary(HDC, const zPoint*, int iCount,
                   zPoint* pRight, int& iRight,
                   zPoint* pLeft, int& iLeft);
  void getBoundPoints(double rX, double rY,
                      zPoint p1, zPoint p2,
                      zPoint& p1r, zPoint& p2r,
                      zPoint& p1l, zPoint& p2l);
  static bool fCross(zPoint pA0, zPoint pA1, zPoint pB0, zPoint pB1, zPoint& pC);
  LineDspType ldt;
  double rWidth, rDist; // in mm
  Color clr, clrFill;
  bool fSupportLine;
  ExtendedSymbol* smb;
//HDC hdcDummy;  // remove !
//double rDummy; // remove !
};

class _export FieldLineTypeSimple: public FieldOneSelect
{
public:
  FieldLineTypeSimple(FormEntry* parent, LineDspType* ldt, bool fOnlySimple=false);
  void StoreData();
protected:
  void create();
  void DrawItem(Event *ev);
private:
  LineDspType* ldt;
  long iLdt;
  bool fOnlySimple;
};

class _export FieldLineType: public FormEntry
{
public:
  FieldLineType(FormEntry* parent, const String& sQuestion,
                LineDspType* ldt, bool fOnlySimple=true);
  void SetCallBack(NotifyProc np)
	{ FormEntry::SetCallBack(np);
		fld->SetCallBack(np); 
	}
  void SetCallBack(NotifyProc np, CallBackHandler* cb)
  { 
		FormEntry::SetCallBack(np, cb);
		fld->SetCallBack(np,cb); 
	}
  void create();             // overriden
  void StoreData();          // overriden
  void SetFocus();
  String sGetText();
private:
  StaticTextSimple *st;       // text before field
  FieldLineTypeSimple *fld;   // edit field
};

class _export FieldLine: public FieldGroup
{
public:
  FieldLine(FormEntry* parent, Line*, bool fAskColor=false);
  void StoreData();
  virtual void show(int sw);           
private:
  int TypeChanged(void*);
  int SupportChanged(void *);
  int SymbolButton(void*);
  FieldLineType* flt;
  CheckBox* cbSupport;
  FieldColor* fcBG;
  FieldReal* frWidth;
  FieldReal* frDist;
  FieldReal* frSymbDist;
  PushButton* pbSymbol;
  Line line;
  Line* pLine;
};

#endif // LINE_H
