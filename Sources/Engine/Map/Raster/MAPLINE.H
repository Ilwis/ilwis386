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
   Interface for ILWIS 2.0
   11 Apr 95, by Willem Nieuwenhuis
   Copyright ILWIS Department, ITC
	Last change:  JEL  16 Apr 97    9:23 am
*/
   
#ifndef ILWMAPLINE_H
#define ILWMAPLINE_H
#include "Engine\Map\Raster\MAPSTORE.H"

class DATEXPORT MapLine : public MapStoreBase {
  friend class MapStore;
  // Get/Put a Patch
  void GetPatchRaw(RowCol, BytePatch&) const;
  void GetPatchRaw(RowCol, IntPatch&) const;
  void GetPatchRaw(RowCol, LongPatch&) const;
  void GetPatchVal(RowCol, LongPatch&) const;
  void GetPatchVal(RowCol, RealPatch&) const;
  void PutPatchRaw(RowCol, const BytePatch&);
  void PutPatchRaw(RowCol, const IntPatch&);
  void PutPatchRaw(RowCol, const LongPatch&);
  void PutPatchVal(RowCol, const LongPatch&);
  void PutPatchVal(RowCol, const RealPatch&);
protected:  
  MapLine(const FileName& fnDat, MapStore& p, const RowCol& rcSize, 
          const DomainValueRangeStruct& dvs, long iStartOff, long iRowLen, bool fPixelInterLeaved, bool fSwapBytes);
  MapLine(const FileName& fnDat, MapStore& p) : MapStoreBase(fnDat, p) {}
  long iStartOffset, iRowLength;
  bool fPixelInterLeaved, fSwapBytes;
  long iNrBands;
};

inline void swapbytes(short *iVal)
{
	unsigned short iS = (unsigned short)(*iVal);
	*iVal = (iS << 8) | (iS >> 8);
}

inline void swapbytes(long *iVal)
{
	unsigned long iV = (unsigned long)(*iVal);
	*iVal = ((iV << 24) & 0xff000000L) |
	        ((iV <<  8) & 0x00ff0000L) |
	        ((iV >>  8) & 0x0000ff00L) |
	        ((iV >> 24) & 0x000000ffL); 
}

inline void swapbytes(double *prVal)
{
	long *pc = (long *)prVal;
	swapbytes(pc);
	swapbytes(pc + 1); // 1 = 1 long so 4 bytes gap!!
}


#endif



