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
/* GeoRefDifferential
   made for MapVariogramSurface
   Copyright Ilwis System Development ITC
   november 1998, by Jan Hendrikse, Wim Koolhoven
	Last change:  WK    7 Dec 98   11:58 am
*/
#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grdiff.H"

GeoRefDifferential* GeoRefDifferential::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  double rSpacing = as[0].rVal();
  long iLags = as[1].iVal();
  return new GeoRefDifferential(fn, rSpacing, iLags);
}


GeoRefDifferential::GeoRefDifferential(const FileName& fn, double rPixSiz, long iNrLgs)
: GeoRefSmpl(fn, CoordSystem(), RowCol(2*iNrLgs-1,2*iNrLgs-1))
{
   assert(rPixSiz > 0.001);
   assert(iNrLgs >= 1);
   rPixelSize = rPixSiz;
   iLags = iNrLgs;
   double a11, a12, a21, a22, b1, b2;
   a11 = 1 / rPixelSize;
   a22 = -a11;
   a21 = a12 = 0;
   b1 = b2 = iLags - 0.5;  // offset of csy origin
   SetSmpl(a11, a12, a21, a22, b1, b2);
   CoordSystem csNew;
   csNew.SetPointer(new CoordSystemDifferential());
   SetCoordSystem(csNew);
}

String GeoRefDifferential::sType() const
{
  return "GeoReference Differential";
}

String GeoRefDifferential::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  long iLags = (rcSize().Row + 1)/2;  
  return String("GeoRefDifferential(%g,%li)", rPixSize(), iLags);
}

String GeoRefDifferential::sTypeName() const  
{
  return sType();
}

bool GeoRefDifferential::fNorthOriented() const
{
  return true;
}

