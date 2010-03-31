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
/* GeoRefCorners
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:03 pm
*/
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Grcornrs.h"

const char* GeoRefCorners::sSyntax()
{
  return "GeoRefCorners(rows,cols,cornersofcorners,minx,miny,maxx,maxy)\ncornersofcorners=0,1";
}

GeoRefCorners* GeoRefCorners::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(7);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  long iRows = as[0].iVal();
  long iCols = as[1].iVal();
  bool fCoC = 0 != as[2].shVal();
  Coord crdMin, crdMax;
  crdMin.x = as[3].rVal();
  crdMin.y = as[4].rVal();
  crdMax.x = as[5].rVal();
  crdMax.y = as[6].rVal();
  return new GeoRefCorners(fn, RowCol(iRows, iCols), fCoC, crdMin, crdMax);
}

GeoRefCorners::GeoRefCorners(const FileName& fn)
: GeoRefSmpl(fn)
{
  ReadElement("GeoRefCorners", "CornersOfCorners", fCornersOfCorners);
  crdMin.x = rReadElement("GeoRefCorners", "MinX");
  crdMin.y = rReadElement("GeoRefCorners", "MinY");
  crdMax.x = rReadElement("GeoRefCorners", "MaxX");
  crdMax.y = rReadElement("GeoRefCorners", "MaxY");
  Compute();
}

String GeoRefCorners::sType() const
{
  return "GeoReference Corners";
}

String GeoRefCorners::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("GeoRefCorners(%li,%li,%i,%g,%g,%g,%g)", 
                 rcSize().Row, rcSize().Col, fCornersOfCorners,
                 crdMin.x, crdMin.y, crdMax.x, crdMax.y);
}

void GeoRefCorners::Store()
{
  GeoRefSmpl::Store();
  WriteElement("GeoRef", "Type", "GeoRefCorners");
  WriteElement("GeoRefCorners", "CornersOfCorners", fCornersOfCorners);
  WriteElement("GeoRefCorners", "MinX", crdMin.x);
  WriteElement("GeoRefCorners", "MinY", crdMin.y);
  WriteElement("GeoRefCorners", "MaxX", crdMax.x);
  WriteElement("GeoRefCorners", "MaxY", crdMax.y);
}

void GeoRefCorners::Compute()
{
  if (rcSize().Col <= 1 || rcSize().Row <= 1)
    return ; //Error(typeid(GeoRefCorners), -1);
  a12 = a21 = 0;
  double rX = crdMax.x - crdMin.x;
  double rY = crdMax.y - crdMin.y;
  if (abs(rX) < 1e-6 || abs(rY) < 1e-6)
    return ; //Error(typeid(GeoRefCorners), -1);
  if (fCornersOfCorners) {
    a11 = rcSize().Col / rX;
    a22 = - rcSize().Row / rY;
    b1 = - a11 * crdMin.x;
    b2 = - a22 * crdMax.y;
  }
  else {
    a11 = (rcSize().Col - 1) / rX;
    a22 = - (rcSize().Row - 1) / rY;
    b1 = 0.5 - a11 * crdMin.x;
    b2 = 0.5 - a22 * crdMax.y;
  }
}

#define swap(a,b) \
{ double r = a; a = b; b = r; }

void GeoRefCorners::Rotate(bool fSwapRows, bool fSwapCols, bool fRotate)
{
  GeoRefPtr::Rotate(fSwapRows, fSwapCols, fRotate);
  if (fSwapRows) 
    swap(crdMin.y, crdMax.y);
  if (fSwapCols) 
    swap(crdMin.x, crdMax.x);
  if (fRotate) {
    Coord crd = crdMin; 
    crdMin = crdMax; 
    crdMax = crd;
  }  
  Compute();
}

bool GeoRefCorners::fNorthOriented() const
{
  return true;
}





