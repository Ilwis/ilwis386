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
/* $Log: /ILWIS 3.0/BasicDataStructures/objvirt.h $
 * 
 * 5     16-08-01 19:01 Koolhoven
 * implemented ReadElement() of Time
 * 
 * 4     8/10/01 20:44 Hendrikse
 * added 3 ReadElement inlines used in MapVisibility
 * 
 * 3     21-12-00 10:20a Martin
 * comment error
 * 
 * 2     21-12-00 10:17a Martin
 * added an access function for the objectdependencies
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/18 16:22:42  Wim
// Added fDefOnlyPossible()
//
/* IlwisObjectVirtual
   Copyright Ilwis System Development ITC
   april 1997, by Jelle Wind
	Last change:  WK   18 Aug 97    6:22 pm
*/

#ifndef ILWOBJECTVIRT_H
#define ILWOBJECTVIRT_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\File\objinfo.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Base\DataObjects\Tranq.h"

class BaseMapPtr;
class DATEXPORT PointMapPtr;

class IlwisObjectVirtual
{
public:
    bool              fDefOnlyPossible() const
                        { return !fNeedFreeze; }
		const ObjectDependency& odGetObjectDependencies() const;
    Tranquilizer      trq;

protected:
    _export IlwisObjectVirtual(const FileName&, IlwisObjectPtr& p, ObjectDependency& objdp, bool fCreate=false);
    virtual _export ~IlwisObjectVirtual();

    virtual void _export InitFreeze();
    virtual void _export ExitFreeze();
    virtual void _export Store();
    virtual String       sExpression() const;
    // inline functions taken over from IlwisObjectPtr for convenience:
    String               sName(bool fExt = false, const String& sDirRelative = "") const
                            { return optr.sName(fExt, sDirRelative); }
    String               sTypeName() const
                            { return optr.sTypeName(); }
    virtual void _export InitName(const FileName& fn);
    virtual bool _export fDomainChangeable() const;
    virtual bool _export fValueRangeChangeable() const;
    virtual bool _export fGeoRefChangeable() const;
    virtual bool _export fExpressionChangeable() const;
    // inline functions for convenience:
    int               ReadElement(const char* sSection, const char* sEntry, String& sValue) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, sValue); }
    int               ReadElement(const char* sSection, const char* sEntry, bool& fValue) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, fValue); }
    int               ReadElement(const char* sSection, const char* sEntry, int& iValue) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, iValue); }
    int               ReadElement(const char* sSection, const char* sEntry, long& iValue) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, iValue); }
    int               ReadElement(const char* sSection, const char* sEntry, double& rValue) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rValue); }
    int               ReadElement(const char* sSection, const char* sEntry, RangeInt& rng) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rng); }
    int               ReadElement(const char* sSection, const char* sEntry, RangeReal& rng) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rng); }
    int               ReadElement(const char* sSection, const char* sEntry, FileName& fn) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, fn); }
    int               ReadElement(const char* sSection, const char* sEntry, ObjectTime& tim) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, tim); }
    int               ReadElement(const char* sSection, const char* sEntry, Domain& dm) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, dm); }
    int               ReadElement(const char* sSection, const char* sEntry,Map& mp) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mp); }
    int               ReadElement(const char* sSection, const char* sEntry, MapList& ml) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, ml); }
    int               ReadElement(const char* sSection, const char* sEntry, SegmentMap& mp) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mp); }
    int               ReadElement(const char* sSection, const char* sEntry, PolygonMap& mp) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mp); }
    int               ReadElement(const char* sSection, const char* sEntry,PointMap& mp) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, mp); }
    int               ReadElement(const char* sSection, const char* sEntry, Table& tbl) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, tbl); }
    int               ReadElement(const char* sSection, const char* sEntry, Representation& rpr) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rpr); }
    int               ReadElement(const char* sSection, const char* sEntry, GeoRef& gr) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, gr); }
    int               ReadElement(const char* sSection, const char* sEntry, CoordSystem& cs) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, cs); }
    int               ReadElement(const char* sSection, const char* sEntry, ValueRange& vr) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, vr); }
		int              ReadElement(const char* sSection, const char* sEntry, Coord& crd) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, crd); }
    int              ReadElement(const char* sSection, const char* sEntry, LatLon& ll) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, ll); }
    int              ReadElement(const char* sSection, const char* sEntry, RowCol& rc) const
                        { return ObjectInfo::ReadElement(sSection, sEntry, fnObj, rc); }
    bool              WriteElement(const char* sSection, const char* sEntry, const String& sValue)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, sValue); }
    bool              WriteElement(const char* sSection, const char* sEntry, const char *sValue)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, sValue); }
    bool              WriteElement(const char* sSection, const char* sEntry, bool fValue)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, fValue); }
    bool              WriteElement(const char* sSection, const char* sEntry, short iValue)
                        { return WriteElement(sSection, sEntry, (long)iValue); }
    bool              WriteElement(const char* sSection, const char* sEntry, int iValue)
                        { return WriteElement(sSection, sEntry, (long)iValue); }
    bool              WriteElement(const char* sSection, const char* sEntry, long iValue)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, iValue); }
    bool              WriteElement(const char* sSection, const char* sEntry, double rValue)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, rValue); }
    bool              WriteElement(const char* sSection, const char* sEntry, const FileName& fn)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, fn); }
    bool              WriteElement(const char* sSection, const char* sEntry, const IlwisObject& obj)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, obj); }
    bool              WriteElement(const char* sSection, const char* sEntry, const RangeInt& rng)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, rng); }
    bool              WriteElement(const char* sSection, const char* sEntry, const RangeReal& rng)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, rng); }
    bool              WriteElement(const char* sSection, const char* sEntry, const Coord& crd)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, crd); }
    bool              WriteElement(const char* sSection, const char* sEntry, const LatLon& ll)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, ll); }
    bool              WriteElement(const char* sSection, const char* sEntry, const RowCol& rc)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, rc); }
    bool              WriteElement(const char* sSection, const char* sEntry, const MinMax& mm)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, mm); }
    bool              WriteElement(const char* sSection, const char* sEntry, const CoordBounds& cb)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, cb); }
    bool              WriteElement(const char* sSection, const char* sEntry,  const ValueRange& vr)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, vr); }
    bool              WriteElement(const char* sSection, const char* sEntry, Color col)
                        { return ObjectInfo::WriteElement(sSection, sEntry, fnObj, col); }
    /// members:
    const             FileName& fnObj;
    IlwisObjectPtr&   optr;
    ObjectDependency& objdep;
    ObjectTime&             objtime;
    bool              fNeedFreeze;
    String         htpFreeze;
    String            sFreezeTitle;
};
#endif





