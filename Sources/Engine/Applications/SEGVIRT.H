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
/* SegmentMapVirtual
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL   7 May 97    9:50 pm
*/

#ifndef ILWSEGVIRT_H
#define ILWSEGVIRT_H
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Applications\bmapvirt.h"
#include "Engine\Map\Segment\SEGSTORE.H"

class _export SegmentMapVirtual: public BaseMapVirtual
{
  friend class ObjectDependency;
  friend class SegmentMapPtr;
public:
   SegmentMapVirtual(const FileName&, SegmentMapPtr&);
   SegmentMapVirtual(const FileName& fn, SegmentMapPtr&, const CoordSystem&, 
                    const CoordBounds&, const DomainValueRangeStruct& dvs);
  static SegmentMapVirtual* create(const FileName&, SegmentMapPtr&);
  static SegmentMapVirtual* create(const FileName&, SegmentMapPtr&, const String& sExpression);
   ~SegmentMapVirtual();
  virtual void  Freeze();
  virtual void  UnFreeze();
  virtual bool fFreezing();
  virtual String sExpression() const;
  virtual String  sType() const;
  virtual void Store();
  virtual bool  fDomainChangeable() const;
  virtual bool  fValueRangeChangeable() const;
  void  CreateSegmentMapStore();
  long iSeg() const { return ptr._iSeg; }
protected:
  SegmentMapPtr& ptr;
  SegmentMapStore*& pms;
};

#endif




