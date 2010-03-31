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
/* MapPatchReal
   Implementation for ILWIS 2.0
   12 Apr 95, by Willem Nieuwenhuis
   Copyright ILWIS Department, ITC
	Last change:  MS   15 Aug 97   11:10 am
*/

#include "Engine\Map\Raster\MPPTREAL.H"
#include "Engine\Domain\Dmvalue.h"

String MapPatchReal::sType() const
{
	return "Real";
}

StoreType MapPatchReal::st() const
{
	return stREAL;
}

double MapPatchReal::rValue(RowCol rc) const {
  double rVal = rUNDEF;
  if (!fInside(rc)) return rVal;   // double undefined
  long iPatch;
  int iPRow, iPCol;
  RowCol2Patch(rc, iPatch, iPRow, iPCol);
  file->Seek(sizeof(double) * (iPatch * PATCH_UNITS + iPRow * PATCH_SIDE + iPCol) );
  file->Read(sizeof(double), &rVal);
  return rVal;
}

void MapPatchReal::PutVal(RowCol rc, double rVal) {
  if (!fInside(rc)) return;
  long iPatch;
  int iPRow, iPCol;
  RowCol2Patch(rc, iPatch, iPRow, iPCol);
  file->Seek(sizeof(double) * (iPatch * PATCH_UNITS + iPRow * PATCH_SIDE + iPCol) );
  file->Write(sizeof(double), &rVal);
  fChanged = true;
}

void MapPatchReal::PutVal(RowCol rc, const String& s) {
  if (dvs.fRealValues()) {
    double r = dvs.rValue(s);
    PutVal(rc, r);
  }
}

void MapPatchReal::PutRaw(RowCol rc, long iVal) {
  double r = doubleConv(iVal);
  PutVal(rc, r);
}

void MapPatchReal::GetPatchRaw(RowCol rc, BytePatch& patch) const {
  RowCol rcPatchOrg = rcPatchStart(rc);
  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      patch(iL, iC) = 0;
}

void MapPatchReal::GetPatchRaw(RowCol rc, IntPatch& patch) const {
  RowCol rcPatchOrg = rcPatchStart(rc);
  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      patch(iL, iC) = shUNDEF;
}

void MapPatchReal::GetPatchRaw(RowCol rc, LongPatch& patch) const {
  RowCol rcPatchOrg = rcPatchStart(rc);
  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      patch(iL, iC) = iUNDEF;
}

void MapPatchReal::GetPatchVal(RowCol rc, LongPatch& patch) const {
  RealPatch rpt;
  GetPatchVal(rc, rpt);
  RowCol rcPatchOrg = rcPatchStart(rc);
  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      patch(iL, iC) = longConv(rpt(iL, iC));
}

void MapPatchReal::GetPatchVal(RowCol rc, RealPatch& patch) const {
  long iPatch;
  int iPRow, iPCol;
  RowCol2Patch(rc, iPatch, iPRow, iPCol);
  file->Seek(sizeof(double) * iPatch * PATCH_UNITS);
  file->Read(sizeof(double) * PATCH_UNITS, patch.buf());
}


void MapPatchReal::PutPatchRaw(RowCol, const BytePatch&) {
}

void MapPatchReal::PutPatchRaw(RowCol, const IntPatch&) {
}

void MapPatchReal::PutPatchRaw(RowCol, const LongPatch&) {
}

void MapPatchReal::PutPatchVal(RowCol rc, const LongPatch& patch) {
  RealPatch rpt;
   RowCol rcPatchOrg = rcPatchStart(rc);
   for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      rpt(iL, iC) = doubleConv(patch(iL, iC));
  PutPatchVal(rc, rpt);
}

void MapPatchReal::PutPatchVal(RowCol rc, const RealPatch& patch) {
  long iPatch;
  int iPRow, iPCol;
  RowCol2Patch(rc, iPatch, iPRow, iPCol);
  RealPatch rpt;
  RangeReal rr = vr()->rrMinMax();
  RowCol rcPatchOrg = rcPatchStart(rc);
  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      rpt(iL, iC) = rr.fContains(patch(iL, iC)) ? patch(iL, iC) : rUNDEF;
  file->Seek(sizeof(double) * iPatch * PATCH_UNITS);
  file->Write(sizeof(double) * PATCH_UNITS, rpt.buf());
  fChanged = true;
}




