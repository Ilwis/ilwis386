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
/* GeoRefNone
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   19 Jul 96    4:47 pm
*/
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\GRNONE.H"

const char* GeoRefNone::sSyntax()
{
  return "GeoRefNone(rows,cols)";
}

GeoRefNone* GeoRefNone::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  long iRows = as[0].iVal();
  long iCols = as[1].iVal();
  return new GeoRefNone(fn, RowCol(iRows, iCols));
}

void GeoRefNone::Store()
{
  GeoRefPtr::Store();
  WriteElement("GeoRef", "Type", "GeoRefNone");
}

void GeoRefNone::Coord2RowCol
  (const Coord& c, double& rRow, double& rCol) const
{
  rCol = c.x; 
  rRow = -c.y; 
}

void GeoRefNone::RowCol2Coord
  (double rRow, double rCol, Coord& c) const
{
  c.x = rCol;
  c.y = -rRow; 
}

//double GeoRefNone::rPixSize() const
//{ return 1; }

bool GeoRefNone::fEqual(const IlwisObjectPtr& ptr) const
{
  const GeoRefPtr* grp = dynamic_cast<const GeoRefPtr*>(&ptr);
  if (0 == grp)
    return false;
  if (rcSize() != grp->rcSize())
    return false;
  const GeoRefNone* grn = dynamic_cast<const GeoRefNone*>(grp);
  return (0 == grn);
}

bool GeoRefNone::fGeoRefNone() const
{
   return true;
}


