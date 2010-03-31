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
/*/ $Log: /ILWIS 3.0/RasterStorage/MAPPATCH.H $
 * 
 * 3     6/05/01 4:30p Martin
 * removed comment bug
 * 
 * 2     6/01/01 12:10p Martin
 * added an extra parameter to the GetLine... functions to retrieve a
 * specific line from an pyramid layer. Added a function to create pyramid
 * layers (empty at the moment)
// Revision 1.3  1998/09/16 17:24:28  Wim
// 22beta2
//
// Revision 1.2  1997/08/15 09:08:43  martin
// A function has been added that computes the Origin of a Patch
//
/* MapPatch
   Interface for ILWIS 2.0
   11 Apr 95, by Willem Nieuwenhuis
   Copyright ILWIS Department, ITC
	Last change:  MS   15 Aug 97   10:08 am
*/
   
#ifndef ILWMAPPATCH_H
#define ILWMAPPATCH_H
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"

class DATEXPORT MapPtr;
                                
class DATEXPORT MapPatch : public MapStoreBase {
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapStore;
  ArrayLarge<long> aiRedir;
  // Get/Put a line
	void GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum, int iPyrLayer = 0) const;
	void GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum, int iPyrLayer = 0) const;
	void GetLineRaw(long iLine, LongBuf&, long iFrom, long iNum, int iPyrLayer = 0) const;
	void GetLineVal(long iLine, LongBuf&, long iFrom, long iNum, int iPyrLayer = 0) const;
	void GetLineVal(long iLine, RealBuf&, long iFrom, long iNum, int iPyrLayer = 0) const;
  void PutLineRaw(long iLine, const ByteBuf&, long iFrom, long iNum);
  void PutLineRaw(long iLine, const IntBuf&,  long iFrom, long iNum);
  void PutLineRaw(long iLine, const LongBuf&, long iFrom, long iNum);
  void PutLineVal(long iLine, const LongBuf&, long iFrom, long iNum);
  void PutLineVal(long iLine, const RealBuf&, long iFrom, long iNum);
protected:
  RowCol rcPatchStart(const RowCol& rc) const;
  void RowCol2Patch(const RowCol&, long& iPatch, int& iPRow, int& iPCol) const;
  RowCol Patch2RowCol(const long iPatch) const;  // get upper left RowCol of patch iPatch
  void BuildRedirTable();
//  MapPatch(const FileName& fn) : MapStore(fn) { BuildRedirTable(); }
  MapPatch(const FileName& fnDat, MapStore& p, const RowCol& rcSize, const DomainValueRangeStruct& dvs) 
    : MapStoreBase(fnDat, p, rcSize, dvs) { BuildRedirTable(); }
  MapPatch(const FileName& fnDat, MapStore& p) 
    : MapStoreBase(fnDat, p) {}
//  virtual void Store(); 
	void CreatePyramidLayers();	
};

#endif



