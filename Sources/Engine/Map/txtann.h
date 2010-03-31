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
/* $Log: /ILWIS 3.0/BasicDataStructures/txtann.h $
 * 
 * 4     27-10-00 2:04p Martin
 * added getobjectstructure function
 * 
 * 3     5-08-99 16:33 Koolhoven
 * Header comment
 * 
 * 2     5-08-99 16:32 Koolhoven
 * removed superfluouse lines, replaced zDisplay with CDC
// Revision 1.1  1998/09/16 17:22:46  Wim
// 22beta2
//
/* AnnotationText
   Copyright Ilwis System Development ITC
   may 1998, by Wim Koolhoven
	Last change:  WK   10 Aug 98   11:35 am
*/

#ifndef ILWTXTANN_H
#define ILWTXTANN_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Table\tbl.h"
#include "Engine\Table\Col.h"

class _export AnnotationText;
class _export AnnotationTextPtr;
class Positioner;

class _export AnnotationTextPtr: public IlwisObjectPtr
{
  friend class AnnotationText;
  friend class _export FormCreateAnnotationText;
//  friend class AnnotationTextEditor;
//  friend class AnnotationTextDrawer;
  enum eCodeName { eCODE, eNAME, eBOTH };
  AnnotationTextPtr(const FileName& fn);
  AnnotationTextPtr(const FileName& fn, int); // create
  AnnotationTextPtr(const FileName& fn, const PointMap&, eCodeName);
  AnnotationTextPtr(const FileName& fn, const SegmentMap&, eCodeName);
  AnnotationTextPtr(const FileName& fn, const PolygonMap&, eCodeName);
public:
  virtual void Store();
  virtual ~AnnotationTextPtr();
  virtual String sType() const;
  virtual void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  long iSize() const
    { if (tbl.fValid()) return tbl->iRecs();
      else return 0; }
  long iAdd(const Coord&, const String&);
  long iAdd(double rRow, double rCol, const String&);
  void Delete(long iRec);
  void FillRec(long iRec);
  void FromRec(long iRec);
  void CreateColumns();
  void UseXY(const GeoRef&);
  void UseRowCol(const GeoRef&);
  void CreateColumnFontName();
  void CreateColumnFontSize();
  void CreateColumnFontBold();
  void CreateColumnFontItalic();
  void CreateColumnFontUnderline();
  void CreateColumnJustification();
  void CreateColumnColor();
  void CreateColumnTransparent();
  void CreateColumnRotation();
	void GetObjectStructure(ObjectStructure& os);
  // implemented in antxtdrw.c:
  void draw(CDC* dsp, Positioner* psn, long iRec);
//private:
  double rWorkingScale;
  bool fUseXY; // and thus not RowCol
  String sFontName;
  double rFontSize; // in points
  bool fBold, fItalic, fUnderline, fTransparent;//, fUpright;
  int iJustification;
  Color color;
  double rRotation; // in degrees
  Table tbl;
  Column colX, colY, colRow, colCol, colText,
         colFontName, colFontSize,
         colFontBold, colFontItalic, colFontUnderline, //colFontUpright,
         colJustification, colColor, colTransparent, colRotation;
};

class _export AnnotationText: public IlwisObject
{
  static IlwisObjectPtrList listTxtAnn;
public:
  AnnotationText();
  AnnotationText(const AnnotationText&);
  AnnotationText(const FileName& fn);
  void operator = (const AnnotationText& ta)
    { SetPointer(ta.pointer()); }
  inline AnnotationTextPtr* ptr() const;
  AnnotationTextPtr* operator -> () const { return ptr(); }
  static AnnotationTextPtr* pGet(const FileName& fn);
};

inline AnnotationTextPtr* AnnotationText::ptr() const
  { return static_cast<AnnotationTextPtr*>(pointer()); }


#endif // ILWTXTANN_H






