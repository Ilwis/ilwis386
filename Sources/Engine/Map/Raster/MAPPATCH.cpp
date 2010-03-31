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
/* MapPatch
   Implementation for ILWIS 2.0
   11 Apr 95, by Willem Nieuwenhuis
   Copyright ILWIS Department, ITC
	Last change:  MS   15 Aug 97   10:00 am
*/

#include "Engine\Map\Raster\MAPPATCH.H"
#include "Engine\Domain\Dmvalue.h"

static byte PatchIndex[16] = {  0,  1,  4,  5, 16, 17, 20, 21,
                               64, 65, 68, 69, 80, 81, 84, 85};
static byte RowIndex[16] = { 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3};
static byte ColIndex[16] = { 0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3};
/*
void MapPatch::Store() {
  MapStore::Store();
  WriteElement("MapStore", "Structure", "Patch");
  WriteElement("MapStore", "Data", FileName(file->sName()));
}
*/
long iToIndex(long iRC) {
  long iNdx = iRC / PATCH_SIDE;
  if (iNdx <= 15 ) return (long)PatchIndex[iNdx];
  long factor = 1;
  long iD = iNdx;
  long iR, iRet = 0;
  do {
    iR = iD % 16;
    iD = iD >> 4;
    iRet += (long)PatchIndex[iR] * factor;
    factor *= 256;
  }
  while (iD>0);
  return iRet;
}

// This routine has a limit of a maximum row and col of 4.194.303.
// (The max patch number will then be 1.073.741.823).
// This limit is due to the use of long integer calculation only.
// The number returned in iPatch is the position of the patch in the file
void MapPatch::RowCol2Patch(const RowCol& rc, long& iPatch, int& iPRow, int& iPCol) const {
  iPatch = aiRedir[iToIndex(rc.Col) + iToIndex(rc.Row) * 2];
  iPRow = rc.Row % PATCH_SIDE;
  iPCol = rc.Col % PATCH_SIDE;
}

// Calculate RowCol addresses from patch number. The input iPatch is the position of a patch
// in a file not the actual patch number. This can be retrieved from the redirection table.
RowCol MapPatch::Patch2RowCol(const long iPatch) const {
  unsigned long i;
  for (i=0; i<aiRedir.iSize(); i++)
    if (iPatch == aiRedir[i]) 
      break;   // at break i is actual patch number
  long factor=7;
  RowCol rc(0L,0L);
  long iR, iD = i; // iPatch; Jelle,24-4-97
  do {
    iR = iD % 16;
    iD = iD >> 4;
    rc.Row += (long)RowIndex[iR] << factor;
    rc.Col += (long)ColIndex[iR] << factor;
    factor += 2;
  }
  while (iD);
  return rc;
}

RowCol MapPatch::rcPatchStart(const RowCol& rc) const
{
  long StartXPatch = rc.Col - rc.Col % PATCH_SIDE;
  long StartYPatch = rc.Row - rc.Row % PATCH_SIDE;
  long iLE = StartYPatch <= iLines() - PATCH_SIDE ? PATCH_SIDE : iLines() % PATCH_SIDE;
  long iCE = StartXPatch <= iCols() - PATCH_SIDE  ? PATCH_SIDE : iCols()  % PATCH_SIDE;

  return RowCol(iLE, iCE);
}


// Build redirection table to store patches in a file without gaps in the file
void MapPatch::BuildRedirTable() {
  long iMaxPatch = 1 + iToIndex(iCols()) + iToIndex(iLines()) * 2;
  aiRedir.Resize(iMaxPatch);
  long iX, iY, iPatch;
  for (long i=0; i<iMaxPatch; i++)
    aiRedir[i] = 0;
  for (iY=0; iY < iLines(); iY+=PATCH_SIDE)
    for (iX=0; iX < iCols(); iX+=PATCH_SIDE) {
      iPatch = iToIndex(iX) + iToIndex(iY) * 2;
      aiRedir[iPatch] = 1;  // valid patch
    }
  long iLast = 0;
  for (long i=0; i<iMaxPatch; i++)
    if (1 == aiRedir[i]) {
      aiRedir[i] = iLast;
      iLast++;
    }
}

// Compose a line from a patch
#define GetLineRawFromPatch(TypeBuf, TypePatch) \
void MapPatch::GetLineRaw(long iLine, TypeBuf& buf, long iFrom, long iNum, int iPyrLayer) const {\
  long iNr = (iNum==0) ? iCols() - iFrom : min(iCols() - iFrom, iNum);  \
  assert(iNr <= buf.iSize());                   \
  TypePatch patch;                             \
  long iCol = iFrom;                           \
  short iR = iLine % PATCH_SIDE;               \
  short iC = iFrom % PATCH_SIDE;               \
  short iCpNr = (iNr<PATCH_SIDE-iC ? iNr : PATCH_SIDE - iC);         \
  do {                                         \
    GetPatchRaw(RowCol(iLine, iCol), patch);      \
    int i, j;                                  \
    for (i=iC, j=iCol-iFrom; i<iCpNr; i++, j++)          \
      buf[j] = patch(iR, i);                   \
    iCol += PATCH_SIDE;                        \
    iNr  -= iCpNr;                             \
    iC = 0;                                    \
    iCpNr = iNr > PATCH_SIDE ? PATCH_SIDE : iNr; \
  } while (iNr > 0);                           \
}

GetLineRawFromPatch(ByteBuf, BytePatch)
GetLineRawFromPatch(IntBuf,  IntPatch)
GetLineRawFromPatch(LongBuf, LongPatch)

#define GetLineValFromPatch(TypeBuf, TypePatch) \
void MapPatch::GetLineVal(long iLine, TypeBuf& buf, long iFrom, long iNum, int iPyrLayer) const {\
  long iNr = (iNum==0) ? iCols() - iFrom : min(iCols() - iFrom, iNum);  \
  assert(iNr <= buf.iSize());                   \
  TypePatch patch;                             \
  long iCol = iFrom;                           \
  short iR = iLine % PATCH_SIDE;               \
  short iC = iFrom % PATCH_SIDE;               \
  short iCpNr = (iNr<PATCH_SIDE-iC ? iNr : PATCH_SIDE - iC);         \
  do {                                         \
    GetPatchVal(RowCol(iLine, iCol), patch);      \
    int i, j;                                  \
    for (i=iC, j=iCol-iFrom; i<iCpNr; i++, j++)          \
      buf[j] = patch(iR, i);                   \
    iCol += PATCH_SIDE;                        \
    iNr  -= iCpNr;                             \
    iC = 0;                                    \
    iCpNr = iNr > PATCH_SIDE ? PATCH_SIDE : iNr; \
  } while (iNr > 0);                           \
}

GetLineValFromPatch(LongBuf, LongPatch)
GetLineValFromPatch(RealBuf, RealPatch)


#define PutLineRawToPatch(TypeBuf, TypePatch) \
void MapPatch::PutLineRaw(long iLine, const TypeBuf& buf, long iFrom, long iNum) { \
  long iNr = (iNum==0) ? iCols() - iFrom : min(iCols() - iFrom, iNum);  \
  assert(iNr <= buf.iSize());                   \
  TypePatch patch;                           \
  long iCol = iFrom;                           \
  short iR = iLine % PATCH_SIDE;               \
  short iC = iFrom % PATCH_SIDE;               \
  short iCpNr = (iNr<PATCH_SIDE-iC ? iNr : PATCH_SIDE - iC);         \
  do {                                         \
    GetPatchRaw(RowCol(iLine, iCol) , patch);     \
    int i, j;                                  \
    for (i=iC, j=iCol-iFrom; i<iCpNr; i++, j++)          \
      patch(iR, i) = buf[j];                   \
    PutPatchRaw(RowCol(iLine, iCol), patch);      \
    iCol += PATCH_SIDE;                        \
    iNr  -= iCpNr;                             \
    iC = 0;                                    \
    iCpNr = iNr > PATCH_SIDE ? PATCH_SIDE : iNr; \
  } while (iNr > 0);                           \
}

PutLineRawToPatch(ByteBuf, BytePatch)
PutLineRawToPatch(IntBuf,  IntPatch)
PutLineRawToPatch(LongBuf, LongPatch)

#define PutLineValToPatch(TypeBuf, TypePatch) \
void MapPatch::PutLineVal(long iLine, const TypeBuf& buf, long iFrom, long iNum) { \
  long iNr = (iNum==0) ? iCols() - iFrom : min(iCols() - iFrom, iNum);  \
  assert(iNr <= buf.iSize());                   \
  TypePatch patch;                           \
  long iCol = iFrom;                           \
  short iR = iLine % PATCH_SIDE;               \
  short iC = iFrom % PATCH_SIDE;               \
  short iCpNr = (iNr<PATCH_SIDE-iC ? iNr : PATCH_SIDE - iC);         \
  do {                                         \
    GetPatchVal(RowCol(iLine, iCol) , patch);     \
    int i, j;                                  \
    for (i=iC, j=iCol-iFrom; i<iCpNr; i++, j++)          \
      patch(iR, i) = buf[j];                   \
    PutPatchVal(RowCol(iLine, iCol), patch);      \
    iCol += PATCH_SIDE;                        \
    iNr  -= iCpNr;                             \
    iC = 0;                                    \
    iCpNr = iNr > PATCH_SIDE ? PATCH_SIDE : iNr; \
  } while (iNr > 0);                           \
}

PutLineValToPatch(LongBuf, LongPatch)
PutLineValToPatch(RealBuf, RealPatch)

void MapPatch::CreatePyramidLayers()
{
}
