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
/* $Log: /ILWIS 3.0/GeoReference/GRMRROT.cpp $
 * 
 * 8     13-12-02 18:38 Hendrikse
 * implemented virtual bool fGeoRefNone() const;
 * 
 * 7     13-08-01 17:05 Koolhoven
 * GeoRefSubMap, Factor and MirrorRotate now have a fDependent() function
 * which always returns true. Ensuring that they will have a Dependeny tab
 * in the property form
 * 
 * 6     26-02-01 9:10a Martin
 * added correct GetObjectStructure functions for these two
 * 
 * 5     9/29/99 10:21a Wind
 * added case insensitive string comparison
 * 
 * 4     20-08-99 12:08p Martin
 * 
 * 3     20-08-99 11:39a Martin
 * //-> /*
/* 
 * 2     19/08/99 17:28 Willem
 * Extended the interface with getter/setter functions
// Revision 1.3  1998/09/16 17:24:43  Wim
// 22beta2
//
// Revision 1.2  1997/08/25 08:41:02  Wim
// Added ObjectDependency in Store() to be sure that Copy() works
// properly
//
/* GeoRefMirrorRotate
   Copyright Ilwis System Development ITC
   november 1995, by Wim Koolhoven
	Last change:  WK   25 Aug 97   10:33 am
*/
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\GRMRROT.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\objdepen.h"

GeoRefMirrorRotate::GeoRefMirrorRotate(const FileName& fn)
: GeoRefPtr(fn)
{
  ReadElement("GeoRefMirrorRotate", "GeoRef", gr);
  if (!gr.fValid())
    gr = GeoRef(rcSize());
  String smr;  
  ReadElement("GeoRefMirrorRotate", "MirrorRotate", smr);
  mr = mrType(smr);
  fSwapCols = mr==mrVERT  || mr==mrDIAG || mr==mrR270 || mr==mrR180 ;
  fSwapRows = mr==mrHOR   || mr==mrDIAG || mr==mrR180 || mr==mrR90 ;
  fRotate = mr==mrTRANS || mr==mrDIAG || mr==mrR270 || mr==mrR90 ;
}

GeoRefMirrorRotate::GeoRefMirrorRotate(const FileName& fn, const GeoRef& grf, MirrorRotateType mirrrot)
: GeoRefPtr(fn, grf->cs(), RowCol()),
  gr(grf), mr(mirrrot)
{
  fSwapCols = mr==mrVERT  || mr==mrDIAG || mr==mrR270 || mr==mrR180 ;
  fSwapRows = mr==mrHOR   || mr==mrDIAG || mr==mrR180 || mr==mrR90 ;
  fRotate = mr==mrTRANS || mr==mrDIAG || mr==mrR270 || mr==mrR90 ;
  RowCol rc = grf->rcSize();
  if (fRotate) {
    long iTmp = rc.Row;
    rc.Row = rc.Col;
    rc.Col = iTmp;
  }
  SetRowCol(rc);
}

MirrorRotateType GeoRefMirrorRotate::mrtMethod() const
{
	return mr;
}

void GeoRefMirrorRotate::SetMirrorRotateMethod(MirrorRotateType mrt)
{
	mr = mrt;
}

String GeoRefMirrorRotate::sType() const
{
  return "GeoReference MirrorRotate";
}

void GeoRefMirrorRotate::Store()
{
  GeoRefPtr::Store();
  WriteElement("GeoRef", "Type", "GeoRefMirrorRotate");
  WriteElement("GeoRefMirrorRotate", "GeoRef", gr);
  String smr = sMirrorRotateType(mr);
  WriteElement("GeoRefMirrorRotate", "MirrorRotate", smr);
  ObjectDependency objdep;
  objdep.Add(gr);
  objdep.Store(this);
}

void GeoRefMirrorRotate::GetObjectStructure(ObjectStructure& os)
{
	GeoRefPtr::GetObjectStructure(os);
	os.AddFile(fnObj, "GeoRefMirrorRotate", "GeoRef");
}

void GeoRefMirrorRotate::Coord2RowCol
  (const Coord& c, double& rRow, double& rCol) const
{
  gr->Coord2RowCol(c, rRow, rCol);
  //if (fSwapRows) {
  //  if (fRotate)
  //    rRow = rcSize().Col - rRow;
  //  else
  //    rRow = rcSize().Row - rRow;
  //}  
  //if (fSwapCols) {
  //  if (fRotate)
  //    rCol = rcSize().Row - rCol;
  //  else
  //    rCol = rcSize().Col - rCol;
  //}  
  //if (fRotate) {
  //  double rTmp = rRow;
  //  rRow = rCol;
  //  rCol = rTmp;
  //}  
}

void GeoRefMirrorRotate::RowCol2Coord
  (double rRow, double rCol, Coord& c) const
{
  
 /* if (fRotate) {
    double rTmp = rRow;
    rRow = rCol;
    rCol = rTmp;
  }  
  if (fSwapRows) {
    if (fRotate)
      rRow = rcSize().Col - rRow;
    else
      rRow = rcSize().Row - rRow;
  }  
  if (fSwapCols) {
    if (fRotate)
      rCol = rcSize().Row - rCol;
    else
      rCol = rcSize().Col - rCol;
  }  */
  gr->RowCol2Coord(rRow, rCol, c);
}

double GeoRefMirrorRotate::rPixSize() const
{
  return gr->rPixSize();
}

String GeoRefMirrorRotate::sMirrorRotateType(MirrorRotateType mr)
{
  switch (mr) {
    case mrHOR:   return "MirrHor"; 
    case mrVERT:  return "MirrVert";
    case mrTRANS: return "Transpose";
    case mrDIAG:  return "MirrDiag";
    case mrR90:   return "Rotate90";
    case mrR180:  return "Rotate180";
    case mrR270:  return "Rotate270";
    default :     return sUNDEF;
  }
}

MirrorRotateType GeoRefMirrorRotate::mrType(const String& sMirrRot)
{
  if (fCIStrEqual(sMirrRot, "MirrHor"))
    return mrHOR;
  if (fCIStrEqual(sMirrRot, "MirrVert"))
    return mrVERT;
  if (fCIStrEqual(sMirrRot, "Transpose"))
    return mrTRANS;
  if (fCIStrEqual(sMirrRot, "MirrDiag"))
    return mrDIAG;
  if (fCIStrEqual(sMirrRot, "Rotate90"))
    return mrR90;
  if (fCIStrEqual(sMirrRot, "Rotate180"))
    return mrR180;
  if (fCIStrEqual(sMirrRot, "Rotate270"))
    return mrR270;
  return mrNORM;
}  


bool GeoRefMirrorRotate::fEqual(const IlwisObjectPtr& ptr) const
{
  if (!GeoRefPtr::fEqual(ptr))
    return false;
  const GeoRefMirrorRotate* grmr = dynamic_cast<const GeoRefMirrorRotate*>(&ptr);
  if (0 == grmr)
    return false;
  return (mr == grmr->mr) && (gr == grmr->gr);
}

bool GeoRefMirrorRotate::fLinear() const
{
  return gr->fLinear();
}

bool GeoRefMirrorRotate::fDependent() const
{
  return true;
}

bool GeoRefMirrorRotate::fGeoRefNone() const
{
   return gr->fGeoRefNone();
}



