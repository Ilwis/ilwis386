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
/* PointMapVirtual
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL   7 May 97    9:50 pm
*/

#ifndef ILWPOINTMAPVIRT_H
#define ILWPOINTMAPVIRT_H
#include "Engine\Map\Point\PNT.H"
#include "Engine\Applications\bmapvirt.h"

class PointMapStore;

typedef PointMapVirtual* (*CreateFunction)(const FileName&, PointMapPtr& p, const String& sExpression);

class _export PointMapVirtual : public BaseMapVirtual
{
  friend class ObjectDependency;
  friend class PointMap;
  friend class PointMapPtr;
public:
	static void LoadPlugins();
protected:
  PointMapVirtual(const FileName&, PointMapPtr& p);
  PointMapVirtual(const FileName& fn, PointMapPtr& p, const CoordSystem&,
                const CoordBounds& cb, const DomainValueRangeStruct&);
  static PointMapVirtual* create(const FileName&, PointMapPtr& p);
  static PointMapVirtual* create(const FileName&, PointMapPtr& p, const String& sExpression);
  ~PointMapVirtual();
  virtual void Freeze();
  virtual void UnFreeze();
  virtual bool fFreezing();
  virtual String sExpression() const;
  virtual String sType() const;
  virtual void Store();
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  virtual void CreatePointMapStore();
  virtual long iRaw(long iRec) const;
  virtual long iValue(long iRec) const;
  virtual double rValue(long iRec) const;
  virtual String sValue(long iRec, short iWidth=-1, short iDec=-1) const;
  virtual Coord cValue(long iRec) const;
  // inline functions taken over from PointMapPtr for convenience:
  virtual long iPnt() const { return _iPoints; }
  PointMapPtr& ptr; // the PointMapPtr of which this is a member
  PointMapStore*& pms; // the PointMapStore for the calculated result
  long _iPoints; // _iPoints from ptr (for convenience)
};

#endif




