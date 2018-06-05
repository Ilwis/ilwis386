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
/* calcstck.h
   Jelle Wind
   october 1995
	Last change:  JEL   7 Jul 97    4:53 pm
*/
#ifndef ILW_CALCH
#define ILW_CALCH
#include "Engine\Domain\dm.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Table\Col.h"
#include "Engine\Table\tbl.h"
#include "Engine\Base\DataObjects\Stack.h"

class DATEXPORT ValueRange;

//! Enum type for objects on the calculator stack
enum StackObjectType { sotRealVal, sotLongVal, sotLongRaw, sotStringVal, sotMap, sotTable, sotColumn, sotCoordVal };

//! Class for intermediate and final results on the calculator stack, stores data in buffers
class CALCEXPORT StackObject
{
public:
  StackObject() {}
  StackObject(StackObjectType sotp, const DomainValueRangeStruct& dvs, int iBufSize);
  StackObject(const Column& col);
  StackObject(const Map& mp);
  StackObject(const StackObject& so);
  ~StackObject();
  static StackObjectType sotype(const DomainValueRangeStruct& dvrs);
  void Resize(StringBuf& buf)
  { buf.Size(sBuf->iSize()); }
  void Resize(RealBuf& buf)
  { buf.Size(rBuf->iSize()); }
  void Resize(LongBuf& buf)
  { buf.Size(iBuf->iSize()); }
  void Resize(CoordBuf& buf)
  {  buf.Size(cBuf->iSize()); }
  void PutVal(const StringBuf&);
  void PutVal(const RealBuf&);
  void PutVal(const LongBuf&);
  void PutVal(const CoordBuf&);
  void PutRaw(const LongBuf&);
  void GetVal(StringBuf&, int iWid=0, int iDec = -1) const;
  void GetVal(RealBuf&) const;
  void GetVal(LongBuf&) const;
  void GetVal(CoordBuf&) const;
  void GetRaw(LongBuf&) const;
  Column colGet() const
  { assert(sot == sotColumn);
    return col; }
  Map mapGet() const
  { assert(sot == sotMap);
    return map; }
  int iSize() const { return iSiz; }
  StackObjectType sot;
  DomainValueRangeStruct dvs;
  union {
    LongBuf *iBuf;
    RealBuf *rBuf;
    StringBuf *sBuf;
    CoordBuf *cBuf;
  };
  Map map;
  Column col;
  int iSiz;
};


#endif




