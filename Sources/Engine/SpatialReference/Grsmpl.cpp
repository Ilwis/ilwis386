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
/* GeoRefSmpl
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:07 pm
*/
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsmpl.h"

const char* GeoRefSmpl::sSyntax()
{
  return "GeoRefSmpl(rows,cols,a11,a12.,a21,a22,b1,b2)";
}

GeoRefSmpl* GeoRefSmpl::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(8);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  long iRows = as[0].shVal();
  long iCols = as[1].shVal();
  double a11 = as[2].rVal();
  double a12 = as[3].rVal();
  double a21 = as[4].rVal();
  double a22 = as[5].rVal();
  double b1 = as[6].rVal();
  double b2 = as[7].rVal();
  return new GeoRefSmpl(fn, RowCol(iRows, iCols), a11, a12, a21, a22, b1, b2);
}

GeoRefSmpl::GeoRefSmpl(const FileName& fn)
: GeoRefPtr(fn)
{
// err: no coefficients                      ::GeoRefSmpl(fn)
  a11 = 1;
  a22 = -1;
  b1 = b2 = a12 = a21 = 0;
  a11 = rReadElement("GeoRefSmpl", "a11");
  a12 = rReadElement("GeoRefSmpl", "a12");
  a21 = rReadElement("GeoRefSmpl", "a21");
  a22 = rReadElement("GeoRefSmpl", "a22");
  b1  = rReadElement("GeoRefSmpl", "b1");
  b2  = rReadElement("GeoRefSmpl", "b2");
}

GeoRefSmpl::GeoRefSmpl(const FileName& fn, const CoordSystem& cs, RowCol rc,
             double a11, double a12, double a21, double a22,
             double b1, double b2)
: GeoRefPtr(fn, cs, rc)
{ SetSmpl(a11, a12, a21, a22, b1, b2); }
    
GeoRefSmpl::GeoRefSmpl(const FileName& fn, RowCol rc,
             double a11, double a12, double a21, double a22,
             double b1, double b2)
: GeoRefPtr(fn, rc)
{ SetSmpl(a11, a12, a21, a22, b1, b2); }

String GeoRefSmpl::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("GeoRefSmpl(%li,%li,%g,%g,%g,%g,%g,%g)", rcSize().Row, rcSize().Col, a11, a12, a21, a22, b1, b2);
}

void GeoRefSmpl::Store()
{
  GeoRefPtr::Store();
  WriteElement("GeoRef", "Type", "GeoRefSmpl");
	String s_a11 = String("%.12f",a11);
	String s_a12 = String("%.12f",a12);
	String s_a21 = String("%.12f",a21);
	String s_a22 = String("%.12f",a22);
	String s_b1 = String("%.12f",b1);
	String s_b2 = String("%.12f",b2);
  WriteElement("GeoRefSmpl", "a11", s_a11);
  WriteElement("GeoRefSmpl", "a12", s_a12);
  WriteElement("GeoRefSmpl", "a21", s_a21);
  WriteElement("GeoRefSmpl", "a22", s_a22);
  WriteElement("GeoRefSmpl", "b1", s_b1);
  WriteElement("GeoRefSmpl", "b2", s_b2);
}

double GeoRefSmpl::rDeterm() const
{ return a11 * a22 - a12 * a21; }

void GeoRefSmpl::Coord2RowCol
  (const Coord& c, double& rRow, double& rCol) const
{
  if (c.fUndef()) {
    rRow = rUNDEF;
    rCol = rUNDEF;
    return;
  }
  rCol = a11 * c.x + a12 * c.y + b1; // - 1;
  rRow = a21 * c.x + a22 * c.y + b2; // - 1;
/*
  if ( iCoeff == 2 ) {	      // Geografic Coordinates
    Coord crd = c;
    if (crd.x < 0)
      crd.x += 1296000; 	// 360 * 3600
    else if (crd.x > 1296000)
      crd.x -= 1296000;
    rC = a11 * crd.x + a12 * crd.y + b1; // - 1;
    rR = a21 * crd.x + a22 * crd.y + b2 ; //- 1;
    if ( abs(rC + rR) < abs(rCol + rRow) ) {
      rCol = rC;
      rRow = rR;
    }
  }
*/
}

void GeoRefSmpl::RowCol2Coord
  (double rRow, double rCol, Coord& c) const
{
  double rDet = rDeterm();
  c.x = ( a22 * (rCol - b1) - a12 * (rRow - b2)) / rDet;
  c.y = (-a21 * (rCol - b1) + a11 * (rRow - b2)) / rDet;
}

double GeoRefSmpl::rPixSize() const
{ return sqrt(1 / abs(rDeterm())); }

bool GeoRefSmpl::fEqual(const IlwisObjectPtr& ptr) const
{
  const GeoRefPtr* grp = dynamic_cast<const GeoRefPtr*>(&ptr);
  if (0 == grp)
    return false;
  if (rcSize() != grp->rcSize())
    return false;
  const GeoRefSmpl* grs = dynamic_cast<const GeoRefSmpl*>(grp);
  if (0 == grs)
    return grp->fEqual(*this);
  double rd1, rd2;
  rd1 = rDeterm();
  rd2 = grs->rDeterm();
  if (abs(rd1) < 1e-12) return false;
  if (abs(rd2) < 1e-12) return false;
  if (abs(rd1 / rd2 - 1) > 1e-3) return false;
  if (abs(rd1 - rd2) > 1e-6) return false;
  if (abs(a11 - grs->a11) > 1e-6) return false;
  if (abs(a12 - grs->a12) > 1e-6) return false;
  if (abs(a21 - grs->a21) > 1e-6) return false;
  if (abs(a22 - grs->a22) > 1e-6) return false;
  if (abs(b1  - grs->b1 ) > 1e-6) return false;
  if (abs(b2  - grs->b2 ) > 1e-6) return false;
  return true;
}

#define swap(a,b) \
{ double r = a; a = b; b = r; }

void GeoRefSmpl::Rotate(bool fSwapRows, bool fSwapCols, bool fRotate)
{
  GeoRefPtr::Rotate(fSwapRows, fSwapCols, fRotate);
  if (fSwapCols) {
    b1 = (fRotate ? _rc.Row : _rc.Col) - b1;
    a11 = -a11;
    a12 = -a12;
  }
  if (fSwapRows) {
    b2 = (fRotate ? _rc.Col : _rc.Row) - b2;
    a21 = -a21;
    a22 = -a22;
  }
  if (fRotate) {
    swap(a11, a21);
    swap(a12, a22);
    swap(b1, b2);
  }
}

bool GeoRefSmpl::fDependent() const
{
  return false;
}

bool GeoRefSmpl::fLinear() const
{
  return true;
}

bool GeoRefSmpl::fNorthOriented() const
{
  return (0 == a12) && (0 == a21);
}





