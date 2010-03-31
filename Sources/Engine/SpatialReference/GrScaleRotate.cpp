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
/* GRSCALEROTATE_H
   made for Stereoscopy
   Copyright Ilwis System Development ITC
   june 2001, by Jan Hendrikse, Wim Koolhoven, Bas Retsios
*/
#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\GrScaleRotate.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\objdepen.h"
#define EPS8 1.e-8
#define EPS4 1.e-4

const char* GeoRefScaleRotate::sSyntax()
{
  return "GeoRefScaleRotate(PivotRow, PivotCol, Angle, Scale)";
}

/*
GeoRefScaleRotate* GeoRefScaleRotate::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(7);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  double rPivoRow = as[0].rVal();
	double rPivoCol = as[1].rVal(); 
  double rAngl = as[2].rVal();
	double rScal= as[3].rVal();
	return new GeoRefScaleRotate(fn, rPivoRow, rPivoCol, rAngl, rScal);
} */

GeoRefScaleRotate::GeoRefScaleRotate(const FileName& fn)
: GeoRefPtr(fn)
{
	rAngle = 0;
  rScale = 1;
  rcPivot = RowCol(0,0);
  ReadElement("GeoRefScaleRotate", "Pivot", rcPivot);
  ReadElement("GeoRefScaleRotate", "Angle", rAngle);
  ReadElement("GeoRefScaleRotate", "Scale", rScale);
	ReadElement("GeoRefScaleRotate", "SourceGeoRef", grSourceMap);
	ReadElement("GeoRefScaleRotate", "TargetPivotOffSet", rcOffSet);
}

GeoRefScaleRotate::GeoRefScaleRotate(const FileName& fn, const GeoRef& grf, 
										const double rPivotRow_in, const double rPivotCol_in,
										const double rAngle_in, const double rScale_in ,  
										const double rPivotRowTarget, const double rPivotColTarget)
: GeoRefPtr(fn, grf->cs(), RowCol()) ,grSourceMap(grf)
{
//	rCol = cos(rAngle) / rScale * c.x 
//           + -sin(rAngle) * rScale * c.y + rPivotCol;
//  rRow = -sin(rAngle) * rScale * c.x 
//           + cos(rAngle) / rScale * c.y + rPivotRow;
		// Copy the input params to the members: needed for sName !!!
		rcPivot.Row = rPivotRow_in;
		rcPivot.Col = rPivotCol_in;
		rAngle = rAngle_in;
		rScale = rScale_in;
		rcOffSet.Row = rPivotRowTarget;
		rcOffSet.Col = rPivotColTarget;
		Updated();
}

void GeoRefScaleRotate::Store()
{
  GeoRefPtr::Store();
  WriteElement("GeoRef", "Type", "GeoRefScaleRotate");
  WriteElement("GeoRefScaleRotate", "Pivot", rcPivot);
  WriteElement("GeoRefScaleRotate", "Angle", rAngle);
	WriteElement("GeoRefScaleRotate", "Scale", rScale);
	WriteElement("GeoRefScaleRotate", "SourceGeoRef", grSourceMap);
	WriteElement("GeoRefScaleRotate", "TargetPivotOffSet", rcOffSet);
  ObjectDependency objdep;
  objdep.Add(grSourceMap);
  objdep.Store(this);
}

void GeoRefScaleRotate::Coord2RowCol(const Coord& c, double& rRow, double& rCol) const
{
	double rRowTemp;
	double rColTemp;
	grSourceMap->Coord2RowCol(c, rRowTemp, rColTemp);
	rRowTemp -= rcPivot.Row;
	rColTemp -= rcPivot.Col;
  rRow = cos(rAngle) * rScale * rRowTemp 
           + sin(rAngle) * rScale * rColTemp + rcOffSet.Row;
  rCol =  - sin(rAngle) * rScale * rRowTemp 
           + cos(rAngle) * rScale * rColTemp + rcOffSet.Col;
}

void GeoRefScaleRotate::RowCol2Coord(double rRow, double rCol, Coord& c) const
{
	rRow -= rcOffSet.Row; // in target georef
	rCol -= rcOffSet.Col;
	double rRowTemp = cos(rAngle) / rScale * rRow - sin(rAngle) / rScale * rCol;
	double rColTemp = sin(rAngle) / rScale * rRow + cos(rAngle) / rScale * rCol;
	rRowTemp += rcPivot.Row;
	rColTemp += rcPivot.Col;
  grSourceMap->RowCol2Coord(rRowTemp, rColTemp, c);
}


// Getter/Setter functions
void GeoRefScaleRotate::SetRowCol(RowCol rcSize)
{
	GeoRefPtr::SetRowCol(rcSize);
}

String GeoRefScaleRotate::sType() const
{
  return "GeoReference Scale_Rotate";
}

String GeoRefScaleRotate::sTypeName() const
{
  return sType();
}

RowCol GeoRefScaleRotate::rGetPivotRowCol()
{
	return rcPivot;
}

double GeoRefScaleRotate::rGetRotAngle()
{
	return rAngle;
}

double GeoRefScaleRotate::rGetScaleFactor()
{
	return rScale;
}

void GeoRefScaleRotate:: SetPivotRowCol(const RowCol rcPiv)
{
	rcPivot.Row = rcPiv.Row;
	rcPivot.Col = rcPiv.Col;
}

void GeoRefScaleRotate::SetRotAngle(const double rAng)
{
	rAngle = rAng;
}

void GeoRefScaleRotate::SetScaleFactor(const double rSc)
{
	rScale = rSc;
}

bool GeoRefScaleRotate::fEqual(const IlwisObjectPtr& ptr) const
{
  if (!GeoRefPtr::fEqual(ptr))
    return false;
  const GeoRefScaleRotate* grscrot = dynamic_cast<const GeoRefScaleRotate*>(&ptr);
  if (0 == grscrot)
    return false;
  return (grSourceMap == grscrot->grSourceMap) && (rcPivot == grscrot->rcPivot)
		&& (rcOffSet == grscrot->rcOffSet) && (rAngle == grscrot->rAngle) 
		&& (rScale == grscrot->rScale) && (rcSize() == grscrot->rcSize());
}

bool GeoRefScaleRotate::fLinear() const
{
  return grSourceMap->fLinear();
}

bool GeoRefScaleRotate::fDependent() const
{
  return true;
}

bool GeoRefScaleRotate::fNorthOriented() const
{
  return grSourceMap->fNorthOriented() && (rAngle == 0);
}

double GeoRefScaleRotate::rPixSize() const
{
  return grSourceMap->rPixSize() * rScale;
}

bool GeoRefScaleRotate::fGeoRefNone() const
{
   return grSourceMap->fGeoRefNone();
}
