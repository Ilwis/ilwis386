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
/* MapPatchByte
   Implementation for ILWIS 2.0
   12 Apr 95, by Willem Nieuwenhuis
   Copyright ILWIS Department, ITC
	Last change:  MS   15 Aug 97   10:03 am
*/

#include "Engine\Map\Raster\MPPTBYT.H"
#include "Engine\Domain\Dmvalue.h"

String MapPatchByte::sType() const
{
	return "Byte";
}

StoreType MapPatchByte::st() const
{
	return stBYTE;
}

long MapPatchByte::iRaw(RowCol rc) const {
  byte b;
  if (!fInside(rc)) return iUNDEF;   // byte undefined
  long iPatch;
  int iPRow, iPCol;
  RowCol2Patch(rc, iPatch, iPRow, iPCol);
  file->Seek(iPatch * PATCH_UNITS + iPRow * PATCH_SIDE + iPCol);
  file->Read(1, &b);
  return b;
}

void MapPatchByte::PutRaw(RowCol rc, long iVal) {
  if (!fInside(rc)) return;
  long iPatch;
  int iPRow, iPCol;
  RowCol2Patch(rc, iPatch, iPRow, iPCol);
  file->Seek(iPatch * PATCH_UNITS + iPRow * PATCH_SIDE + iPCol);
  byte b = byteConv(iVal);
  file->Write(1, &b);
  fChanged = true;
}

void MapPatchByte::GetPatchRaw(RowCol rc, BytePatch& patch) const {
  long iPatch;
  int iPRow, iPCol;
  RowCol2Patch(rc, iPatch, iPRow, iPCol);
  file->Seek(iPatch * PATCH_UNITS);
  file->Read(PATCH_UNITS, patch.buf());
}

void MapPatchByte::GetPatchRaw(RowCol rc, IntPatch& patch) const {
  BytePatch bp;
  GetPatchRaw(rc, bp);
  RowCol rcPatchOrg = rcPatchStart(rc);
  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
    for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      patch(iL, iC) = bp(iL, iC);
}

void MapPatchByte::GetPatchRaw(RowCol rc, LongPatch& patch) const
{
  BytePatch bp;
  GetPatchRaw(rc, bp);
  RowCol rcPatchOrg = rcPatchStart(rc);
  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
    for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      patch(iL, iC) = bp(iL, iC);
}


void MapPatchByte::GetPatchVal(RowCol rc, LongPatch& patch) const
{

  RowCol rcPatchOrg = rcPatchStart(rc);
  if (!dvs.fValues()) {
  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
    for (int iC=0; iC < rcPatchOrg.Col ; iC++)
        patch(iL, iC) = iUNDEF;
    return;
  }  
  BytePatch bp;
  GetPatchRaw(rc, bp);
  if (dvs.fRawIsValue())
      for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
            patch(iL, iC) = bp(iL, iC);
  else    
      for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
            patch(iL, iC) = vr()->iValue(bp(iL, iC));
}

void MapPatchByte::GetPatchVal(RowCol rc, RealPatch& patch) const
{
  RowCol rcPatchOrg = rcPatchStart(rc);
  if (!dvs.fValues())
  {
    for (int iL=0; iL < rcPatchOrg.Row ; iL++)
         for (int iC=0; iC < rcPatchOrg.Col ; iC++)
            patch(iL, iC) = rUNDEF;
    return;
  }
  BytePatch bp;
  GetPatchRaw(rc, bp);
  if (dvs.fRawIsValue())
    for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
            patch(iL, iC) = bp(iL, iC);
  else   
    for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
            patch(iL, iC) = vr()->rValue(bp(iL, iC));
}


void MapPatchByte::PutPatchRaw(RowCol rc, const BytePatch& patch) {
  long iPatch;
  int iPRow, iPCol;
  RowCol2Patch(rc, iPatch, iPRow, iPCol);
  file->Seek(iPatch * PATCH_UNITS);
  file->Write(PATCH_UNITS, patch.buf());
  fChanged = true;
}

void MapPatchByte::PutPatchRaw(RowCol rc, const IntPatch& patch)
{
  BytePatch bp;
  RowCol rcPatchOrg = rcPatchStart(rc);

  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      bp(iL, iC) = byteConv(patch(iL, iC));
  PutPatchRaw(rc, bp);
}

void MapPatchByte::PutPatchRaw(RowCol rc, const LongPatch& patch) {
  BytePatch bp;
  RowCol rcPatchOrg = rcPatchStart(rc);

  for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
      bp(iL, iC) = byteConv(patch(iL, iC));
  PutPatchRaw(rc, bp);
}

void MapPatchByte::PutPatchVal(RowCol rc, const LongPatch& patch)
{
  if (!dvs.fValues())
    return;
   BytePatch bp;
   RowCol rcPatchOrg = rcPatchStart(rc);

  if (dvs.fRawIsValue())
  {
    for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
            bp(iL, iC) = byteConv(patch(iL, iC));
  }
  else
  {
     for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
            bp(iL, iC) = byteConv(vr()->iRaw(patch(iL, iC)));
  }
  PutPatchRaw(rc, bp);
}

void MapPatchByte::PutPatchVal(RowCol rc, const RealPatch& patch) {
  if (!dvs.fValues())
    return;
   BytePatch bp;
   RowCol rcPatchOrg = rcPatchStart(rc);

  if (dvs.fRawIsValue())
  {
    for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
            bp(iL, iC) = byteConv(patch(iL, iC));
  }
  else
  {
    for (int iL=0; iL < rcPatchOrg.Row ; iL++)
        for (int iC=0; iC < rcPatchOrg.Col ; iC++)
                bp(iL, iC) = byteConv(vr()->iRaw(patch(iL, iC)));
  }
  PutPatchRaw(rc, bp);
}




