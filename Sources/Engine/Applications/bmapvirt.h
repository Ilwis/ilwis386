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
/* $Log: /ILWIS 3.0/BasicDataStructures/bmapvirt.h $
 * 
 * 3     6-11-00 15:58 Koolhoven
 * header comment
 * 
 * 2     6-11-00 15:58 Koolhoven
 * cs() and tblAtt() no longer return a reference
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/25 17:50:13  Wim
// Added (empty) DeleteInternals() to BaseMapVirtual
// to call in the UnFreeze() routine.
//
/* BaseMapVirtual
   Copyright Ilwis System Development ITC
   april 1997, by Jelle Wind
	Last change:  WK   25 Aug 97    3:19 pm
*/

#ifndef ILWBASEMAPVIRT_H
#define ILWBASEMAPVIRT_H
#include "Engine\Map\basemap.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\objdepen.h"
#include "Engine\Base\DataObjects\Tranq.h"


class BaseMapVirtual : public IlwisObjectVirtual
// used as base class for MapVirtual, PointMapVirtual, SegmentMapVirtual, PointMapVirtual
{
protected:  
  _export BaseMapVirtual(const FileName&, BaseMapPtr& p);
  _export BaseMapVirtual(const FileName& fn, BaseMapPtr& p, const CoordSystem&,
                const CoordBounds& cb, const DomainValueRangeStruct&);
  // inline functions taken over from BaseMapPtr for convenience:
  Domain dm() const { return bptr.dm(); }
  const ValueRange& vr() const  { return bptr.vr(); }
  const DomainValueRangeStruct& dvrs() const { return bptr.dvrs(); }
  StoreType st() const          { return bptr.st(); }
  CoordSystem cs() const { return bptr.cs(); }
  const CoordBounds& cb() const { return bptr.cb(); }
  void SetCoordBounds(const CoordBounds& cb) // dangerous function!
  { bptr.SetCoordBounds(cb); }
  void SetCoordSystem(const CoordSystem& cs) // dangerous function!
  { bptr.SetCoordSystem(cs); }
  void SetDomainValueRangeStruct(const DomainValueRangeStruct& dvs)
  { bptr.SetDomainValueRangeStruct(dvs); }
  void SetValueRange(const ValueRange& vr) // dangerous function!
  { bptr.SetValueRange(vr); }
  Table tblAtt()
  { return bptr.tblAtt(); }
  void SetAttributeTable(const Table& tbl)
  { bptr.SetAttributeTable(tbl); }
  void SetNoAttributeTable()
  { bptr.SetNoAttributeTable(); }
  bool fTblAtt()     { return bptr.fTblAtt(); }
  bool fTblAttSelf() { return bptr.fTblAttSelf(); }
  bool fUseReals() const     { return bptr.fUseReals(); }
  bool fRealValues() const   { return bptr.fRealValues(); }
  bool fRawAvailable() const { return bptr.fRawAvailable(); }
  bool fRawIsValue() const   { return bptr.fRawIsValue(); }
  bool fValues() const       { return bptr.fValues(); }
  void SetCB(const CoordBounds& cb) { bptr.cbOuter = cb; }
  void _export DeleteInternals();
  /// members:
  BaseMapPtr& bptr;
};
#endif





