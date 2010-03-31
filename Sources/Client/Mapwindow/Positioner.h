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
// Positioner.h: interface for the Positioner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSITIONER_H__8A84267E_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
#define AFX_POSITIONER_H__8A84267E_E359_11D2_B73E_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace ILWIS;

class ILWIS::Segment;
class ILWIS::Polygon;
class SimpleMapPaneView;

//! Positioner handles the translation between RowCols and Windows coordinates during drawing operations
class _export Positioner
{
public:
	//! translation from Windows coordinates to RowCols
  virtual RowCol rcPos(zPoint)=0; // Windows coordinates -> internal RowCol
  MinMax mmSize() { return mm; }
	//! translation from Windows coordinates to RowCols
  MinMax mmRect(zRect); 	  // Windows coordinates -> internal RowCol
  zRect rectSize();
	//! translation from RowCols to Windows Coordinates
  virtual zPoint pntPos(RowCol);  // internal RowCol -> Windows coordinates
	//! translation from RowCols to Windows Coordinates
  virtual zPoint pntPos(double rRow, double rCol)=0;  // internal RowCol -> Windows coordinates
	//! translation from Coord with help of the georef to RowCols to Windows Coordinates
  zPoint pntPos(Coord);
  long iSegPos(const ILWIS::Segment*, Array<zPoint>&, const CoordSystem& cs);
  void iPolPos(const ILWIS::Polygon*, vector<Array<zPoint>>&, const CoordSystem& cs);
  const GeoRef& georef() const { return gr; }
	double rSc() const { return rScale; }
protected:
  Positioner(const GeoRef& grf): gr(grf) {}
  long scale(long,bool fInv=false); // Windows coordinates -> internal RowCol
  long scale(int,bool fInv=false); // Windows coordinates -> internal RowCol
  long scale(short,bool fInv=false); // Windows coordinates -> internal RowCol
  long scale(double,bool fInv=false); // Windows coordinates -> internal RowCol
  void SequenceToPoints(const CoordinateSequence * seq, Array<zPoint>& p, const CoordSystem& cs);
  double rScale;
  long iXpos, iYpos;
  double rCorr;
  MinMax mm;
//private:
  GeoRef gr;  
};

class _export DefaultPositioner: public Positioner
{
public:
  DefaultPositioner(SimpleMapPaneView*,MinMax,const GeoRef&);
  RowCol rcPos(zPoint);   // Windows coordinates -> internal RowCol
  zPoint pntPos(RowCol);  // internal RowCol -> Windows coordinates
  zPoint pntPos(double rRow, double rCol);  // internal RowCol -> Windows coordinates
};

/*
class _export PrinterPositioner: public Positioner
{
public:
  PrinterPositioner(zDisplay* pd, MinMax, const GeoRef&);
  RowCol rcPos(zPoint);   // Windows coordinates -> internal RowCol
  zPoint pntPos(RowCol rc) { return Positioner::pntPos(rc); }
  zPoint pntPos(double rRow, double rCol);  // internal RowCol -> Windows coordinates
  virtual void getBitmapScaling(zRect& rectPrinter, zRect& rectBitmap);
  double rScale() const;
protected:
  double rScaleX, rScaleY;
  int iPixPerInchX, iPixPerInchY;
};

class _export PrintOnScalePositioner: public PrinterPositioner
{
public:
  PrintOnScalePositioner(zDisplay* pd, MinMax, const GeoRef&, double rScale);  
  virtual void getBitmapScaling(zRect& rectPrinter, zRect& rectBitmap);
  int iNrPages() const;
  int iNextPage();  // returns 0 when finished
private:
  int iMaxPageRow, iMaxPageCol;
  int iPageRow, iPageCol;
  long iDiffX, iDiffY;
};
*/
class _export MetafilePositioner: public Positioner
{
public:
  MetafilePositioner(MinMax, const GeoRef&, double rScale);
  MetafilePositioner(MinMax, const GeoRef&);
  RowCol rcPos(zPoint);   // Windows coordinates -> internal RowCol
  zPoint pntPos(double rRow, double rCol);  // internal RowCol -> Windows coordinates
};

class _export BitmapPositioner: public Positioner
{
public:
  BitmapPositioner(double rScale, MinMax, const GeoRef&);
  RowCol rcPos(zPoint);   // Windows coordinates -> internal RowCol
  zPoint pntPos(RowCol);  // internal RowCol -> Windows coordinates
  zPoint pntPos(double rRow, double rCol);  // internal RowCol -> Windows coordinates
};



#endif // !defined(AFX_POSITIONER_H__8A84267E_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
