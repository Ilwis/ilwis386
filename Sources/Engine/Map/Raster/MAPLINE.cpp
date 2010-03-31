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
/* MapLine
   Implementation for ILWIS 2.0
   Tue  23-05-95, by Willem Nieuwenhuis
   Copyright ILWIS Department, ITC
	Last change:  JEL  16 Apr 97   12:49 pm
*/

#include "Engine\Map\Raster\MAPLINE.H"
#include "Engine\Domain\Dmvalue.h"
/*
MapLine::MapLine(const FileName& fn)
: MapStore(fn) 
{
  iStartOffset = iReadElement("MapStore", "StartOffset");
  if (iStartOffset == iUNDEF)
    iStartOffset = 0;
  iRowLength = iReadElement("MapStore", "RowLength");
  if (iRowLength == iUNDEF)
    iRowLength = iCols();
}
*/

MapLine::MapLine(const FileName& fnDat, MapStore& p, const RowCol& rcSize, const DomainValueRangeStruct& dvrs, long iStartOff, long iRowL, bool fPixIntL, bool fSwapB)
: MapStoreBase(fnDat, p, rcSize, dvrs)
{
  iStartOffset = iStartOff;
  iRowLength = iRowL;
  fPixelInterLeaved = fPixIntL;
  fSwapBytes = fSwapB;
  iNrBands = iRowLength / rcSize.Col;
}


// Compose a patch from lines
#define GetPatchRawFromLine(TypeBuf, TypePatch) \
void MapLine::GetPatchRaw(RowCol rc, TypePatch& ptp) const { \
  long iL = (rc.Row / PATCH_SIDE) * PATCH_SIDE;         \
  long iC = (rc.Col / PATCH_SIDE) * PATCH_SIDE;         \
  short iLNr = (iLines() - iL) > PATCH_SIDE ? PATCH_SIDE : (iLines() - iL); \
  short iCNr = (iCols() - iC) > PATCH_SIDE ? PATCH_SIDE : (iCols() - iC);   \
  TypeBuf bufLine(iCNr);                                \
  short iRow=0, iCol;                                   \
  do {                                                  \
    GetLineRaw(iL, bufLine, iC, iCNr);                     \
    for (iCol=0; iCol<iCNr; iCol++)                     \
      ptp(iRow, iCol) = bufLine[iCol];                  \
    iLNr--;                                             \
    iRow++;                                             \
    iL++;                                               \
  }                                                     \
  while (iLNr > 0);                                     \
}                                                       \

GetPatchRawFromLine(ByteBuf, BytePatch)
GetPatchRawFromLine(IntBuf,  IntPatch)
GetPatchRawFromLine(LongBuf, LongPatch)


#define GetPatchValFromLine(TypeBuf, TypePatch) \
void MapLine::GetPatchVal(RowCol rc, TypePatch& ptp) const { \
  long iL = (rc.Row / PATCH_SIDE) * PATCH_SIDE;         \
  long iC = (rc.Col / PATCH_SIDE) * PATCH_SIDE;         \
  short iLNr = (iLines() - iL) > PATCH_SIDE ? PATCH_SIDE : (iLines() - iL); \
  short iCNr = (iCols() - iC) > PATCH_SIDE ? PATCH_SIDE : (iCols() - iC);   \
  TypeBuf bufLine(iCNr);                                \
  short iRow=0, iCol;                                   \
  do {                                                  \
    GetLineVal(iL, bufLine, iC, iCNr);                     \
    for (iCol=0; iCol<iCNr; iCol++)                     \
      ptp(iRow, iCol) = bufLine[iCol];                  \
    iLNr--;                                             \
    iRow++;                                             \
    iL++;                                               \
  }                                                     \
  while (iLNr > 0);                                     \
}                                                       \


GetPatchValFromLine(LongBuf, LongPatch)
GetPatchValFromLine(RealBuf, RealPatch)

#define PutPatchRawToLine(TypeBuf, TypePatch) \
void MapLine::PutPatchRaw(RowCol rc, const TypePatch& ptp) { \
  long iL = (rc.Row / PATCH_SIDE) * PATCH_SIDE;         \
  long iC = (rc.Col / PATCH_SIDE) * PATCH_SIDE;         \
  short iLNr = (iLines() - iL) > PATCH_SIDE ? PATCH_SIDE : (iLines() - iL); \
  short iCNr = (iCols() - iC) > PATCH_SIDE ? PATCH_SIDE : (iCols() - iC);   \
  TypeBuf bufLine(iCNr);                                \
  short iRow=0, iCol;                                   \
  do {                                                  \
    for (iCol=0; iCol<iCNr; iCol++)                     \
      bufLine[iCol] = ptp(iRow, iCol);                  \
    iLNr--;                                             \
    iRow++;                                             \
    PutLineRaw(iL, bufLine, iC, iCNr);                     \
    iL++;                                               \
  }                                                     \
  while (iLNr > 0);                                     \
}                                                       \

PutPatchRawToLine(ByteBuf, BytePatch)
PutPatchRawToLine(IntBuf,  IntPatch)
PutPatchRawToLine(LongBuf, LongPatch)


#define PutPatchValToLine(TypeBuf, TypePatch) \
void MapLine::PutPatchVal(RowCol rc, const TypePatch& ptp) { \
  long iL = (rc.Row / PATCH_SIDE) * PATCH_SIDE;         \
  long iC = (rc.Col / PATCH_SIDE) * PATCH_SIDE;         \
  short iLNr = (iLines() - iL) > PATCH_SIDE ? PATCH_SIDE : (iLines() - iL); \
  short iCNr = (iCols() - iC) > PATCH_SIDE ? PATCH_SIDE : (iCols() - iC);   \
  TypeBuf bufLine(iCNr);                                \
  short iRow=0, iCol;                                   \
  do {                                                  \
    for (iCol=0; iCol<iCNr; iCol++)                     \
      bufLine[iCol] = ptp(iRow, iCol);                  \
    iLNr--;                                             \
    iRow++;                                             \
    PutLineVal(iL, bufLine, iC, iCNr);                     \
    iL++;                                               \
  }                                                     \
  while (iLNr > 0);                                     \
}                                                       \

PutPatchValToLine(LongBuf, LongPatch)
PutPatchValToLine(RealBuf, RealPatch)
