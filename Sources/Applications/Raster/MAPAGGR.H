/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
/* MapAggregate
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
   Last change:  JEL   7 May 97    9:24 am
*/

#ifndef ILWMAPAGGREGATE_H
#define ILWMAPAGGREGATE_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFMAP.H"

class DATEXPORT AggregateFunction;

IlwisObjectPtr * createMapAggregate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapAggregate(ApplicationQueryData *query);

class DATEXPORT MapAggregate: public MapFromMap
{
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapFromMap;
public:  
  static const char* sSyntax();
  static const char* sSyntax2();
  static MapAggregate* create(const FileName& fn, MapPtr& p, const String& sExpression);
  MapAggregate(const FileName& fn, MapPtr& p);
protected:
  virtual void Store();
  MapAggregate(const FileName& fn, MapPtr& p, const Map& mp, long iGrpFac, bool fGrpFac, 
               long iRowOff, long iColOff,
               AggregateFunction* agf, const String& sGrf);
  ~MapAggregate();
  virtual bool fFreezing();
  virtual String sExpression() const;
  virtual long iComputePixelRaw(RowCol) const;
  virtual double rComputePixelVal(RowCol) const;
  virtual void ComputeLineRaw(long Line, ByteBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineRaw(long Line, IntBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineRaw(long Line, LongBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineVal(long Line, LongBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineVal(long Line, RealBuf&, long iFrom, long iNum) const;
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
private:
  void ReadBufLinesRaw(long iLine, long iFrom, long iNum) const;
  void ReadBufLinesReal(long iLine, long iFrom, long iNum) const;
  void ReadLineVal(long iLine, RealBuf& buf, long iFrom, long iNum) const;
  void ReadLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const;
  void Init();
  RealBuf* bufListValReal;
  LongBuf* bufListValRaw;
  long mutable iLastInpLine, iLastInpFrom, iLastInpNum;
  long iGrpFac;
  AggregateFunction * agf;
  long iRowOffset, iColOffset;
  bool fGrpPixels;
};

#endif




