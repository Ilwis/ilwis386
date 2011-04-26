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
/* MapDensify
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
   Last change:  JEL  21 Jun 97    8:52 pm
*/

#ifndef ILWMAPDENSIFY_H
#define ILWMAPDENSIFY_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFMAP.H"

IlwisObjectPtr * createMapDensify(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapDensify(ApplicationQueryData *query);

class DATEXPORT MapDensify: public MapFromMap
{
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapFromMap;
public:  
  static const char* sSyntax();  
  static MapDensify* create(const FileName& fn, MapPtr& p, const String& sExpression);
  MapDensify(const FileName& fn, MapPtr& p);
protected:
  virtual void Store();
  MapDensify(const FileName& fn, MapPtr& p, const Map& mp, double rEnl, int iMethod);
  ~MapDensify();
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
  void ComputeNearest(long iLine, RealBuf& buf, long iFrom, long iNum);
  void ComputeBiLin(long iLine, RealBuf& buf, long iFrom, long iNum);
  void ComputeCubic(long iLine, RealBuf& buf, long iFrom, long iNum);
  void ReadBufLines(long iLine, long iFrom, long iNum);
  void ReadLineVal(long iLine, RealBuf& buf, long iFrom, long iNum);

  double rBiLin(const double& a, const double& b, const double& dx);
  double rCubic(const double& a, const double& b, const double& c, const double& d, const double& dx);
  void Init();
  RealBuf* bufListValReal;
  short iLinesStored;
  long iLastInpLine, iLastInpFrom, iLastInpNum;
  double rEnlFac;
  RealArray rDL, rDC;
  LongArray iInpLine, iInpFirstCol, iInpLastCol;
  int iMethod;
  RangeReal rrMinMax;
};

#endif




