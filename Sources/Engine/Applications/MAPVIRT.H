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
/* MapVirtual
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL   9 Jun 97   12:30 pm
*/

#ifndef ILWMAPVIRT_H
#define ILWMAPVIRT_H
//#include "Engine\Base\objdepen.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\bmapvirt.h"
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\Applications\ModuleMap.h"
//#include "Engine\Base\DataObjects\Tranq.h"

class _export MapVirtual: public BaseMapVirtual
{
  friend class ObjectDependency;
  friend class Map;
  friend class MapPtr;
protected:
  MapVirtual(const FileName&, MapPtr& p);
  MapVirtual(const FileName&, MapPtr& p, const GeoRef&, const RowCol& rcSize,
             const DomainValueRangeStruct&,
             const MapFormat mf = mfLine);
  static MapVirtual  *create(const FileName&, MapPtr& p);
  static MapVirtual  *create(const FileName&, MapPtr& p, const String& sExpression);
  ~MapVirtual();
  virtual void  Freeze();
  virtual void  UnFreeze();
  virtual bool fFreezing();
  virtual String sExpression() const;
  virtual String  sType() const;
  virtual void Store();
  virtual bool  fDomainChangeable() const;
  virtual bool  fValueRangeChangeable() const;
  virtual bool  fGeoRefChangeable() const;
  virtual DomainValueRangeStruct  dvrsDefault(const String& sExpression) const;
  virtual bool  fMergeDomainForCalc(Domain& dm, const String& sExpr);
  void  CreateMapStore();
  bool  fMapInObjDep(const Map&) const;
  bool  fMapInObjDep(const FileName& fn) const;
  // get data:
  virtual long  iComputePixelRaw(RowCol) const;
  virtual long  iComputePixelVal(RowCol) const;
  virtual double  rComputePixelVal(RowCol) const;
  virtual void ComputeLineRaw(long Line, ByteBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineRaw(long Line, IntBuf& , long iFrom, long iNum) const;
  virtual void ComputeLineRaw(long Line, LongBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineVal(long Line, LongBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineVal(long Line, RealBuf&, long iFrom, long iNum) const;
  void  ReadGeoRefFromFileAndSet();
  void ReadDomainValueRangeFromFileAndSet();
  RowCol rcSize() const { return _rcSize; }
  long iLines() const { return _rcSize.Row; }
  long iCols() const { return _rcSize.Col; }
  MapFormat mf() const { return ptr.mf(); }
  static IlwisObjectPtr * getMapVirtual(const FileName& fn, MapPtr& p, const String& sType);
  static MapVirtual *dottedExpressionMaps(const FileName& fn, MapPtr& p, const String& sExpression, vector<void *> extraParms );
  GeoRef gr() const { return ptr.gr(); }
  MapPtr& ptr; // the MapPtr of which this is a member
  MapStore*& pms; // the apStore for the calculated result
  RowCol& _rcSize; // _rcSize from ptr (for convenience)

};

#endif // ILWMAPVIRT_H





