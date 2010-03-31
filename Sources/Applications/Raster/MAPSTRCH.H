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
/* MapStretch
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
   Last change:  JEL  23 Jun 97    2:01 pm
*/

#ifndef ILWMAPSTRETCH_H
#define ILWMAPSTRETCH_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Table\TBLHIST.H"

class StretchConvRR {
public:
  StretchConvRR()
  { rVal = rUNDEF; rStretch = rUNDEF; }
  StretchConvRR(double rV, double rS = rUNDEF)
  { rVal = rV; rStretch = rS; }
  bool operator ==(const StretchConvRR& scrr)
  { return scrr.rVal == rVal; }
  double rVal, rStretch;
};

class StretchConvLL {
public:
  StretchConvLL()
  { iVal = iUNDEF; iStretch = iUNDEF; }
  StretchConvLL(long iV, long iS = iUNDEF)
  { iVal = iV; iStretch = iS; }
  bool operator ==(const StretchConvLL& scll)
  { return scll.iVal == iVal; }
  long iVal, iStretch;
};

IlwisObjectPtr * createMapStretch(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT MapStretch: public MapFromMap
{
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapFromMap;
public:  
  static const char* sSyntax();  
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  virtual void SetValueRange(const ValueRange& vr);
  static MapStretch* create(const FileName& fn, MapPtr&, const String& sExpression);
  MapStretch(const FileName& fn, MapPtr&);
protected:
  virtual void Store();
  MapStretch(const FileName& fn, MapPtr&, const Map& map, double rPerc, const RangeReal& rrFrm, 
             const DomainValueRangeStruct& dvrs, long iInterval);
  ~MapStretch();
  virtual String sExpression() const;
  virtual long iComputePixelRaw(RowCol) const;
  virtual double rComputePixelVal(RowCol) const;
  virtual void ComputeLineRaw(long Line, ByteBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineRaw(long Line, IntBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineRaw(long Line, LongBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineVal(long Line, LongBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineVal(long Line, RealBuf&, long iFrom, long iNum) const;
  void Init();
  void InitCalc();
  RangeReal rrFrom, rrTo;
  bool fLinear, fInitCalc, fUseRealValues;
  double rFact, rOff, rPerc;
  long iIntervals;
  HashTable<StretchConvRR> htrr;
  HashTable<StretchConvLL> htll;
  TableHistogramInfo histinf;
};

#endif




