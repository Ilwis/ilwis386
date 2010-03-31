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
/* ObjectInfo
   Copyright Ilwis System Development ITC
   may 1996, Jelle Wind
	Last change:  WK   31 Aug 98   10:40 am
*/
#ifndef ILWISOBJINFO_H
#define ILWISOBJINFO_H

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


#include "Engine\Base\DataObjects\Color.h"
//#include "Engine\Map\Raster\Map.h"

class Filter;
class Classifier;
class SampleSet;
class Tranquilizer;
class RealMatrix;
class RangeInt;
class RangeReal;
class Domain;
class Georef;
class CoordSystem;
class Representation;
class Column;
class GeoRef;
class Map;
class SegmentMap;
class PolygonMap;
class PointMap;
class Table;
class MapList;
class IlwisObject;
class ValueRange;
class CSize;
class CRect;

class ObjectInfo
{
public:
  static String _export sDescr(const FileName& fn);
  static String sExpression(const FileName& fn);
  static String sCalcExpression(const FileName& fn);
	static FileName _export fnAttributeTable(const FileName& fn);
  static bool fVirtual(const FileName& fn) // obsolete
  { return fDependent(fn); }
  static bool _export fDependent(const FileName& fn);
  static bool _export fDependenciesForCopy(const FileName& fn);
  static bool fUpToDate(const FileName& fn); // calls ObjectDependency::GetNewestDependency()
  static bool fUpToDate(const FileName& fn, const String& sCol); // calls ObjectDependency::GetNewestDependency()
	static bool _export fInCollection(const FileName& fn);
	static bool _export fIsDataObject(const FileName& fn);
  // in use functions
  static bool _export fUsedInOtherObjects(const FileName& fn, Array<String>* as = 0, bool fStrict = true, Tranquilizer* trqPar = 0);
  static bool fDomainUsedInOtherObjects(const FileName& fn, Array<String>* as, bool fStrict, Tranquilizer* trq);
  static bool fUses(const FileName& fn, const Domain& dm);
  static bool fUses(const FileName& fn, const GeoRef& gr);
  static bool fUses(const FileName& fn, const CoordSystem& cs);
  static bool fUses(const FileName& fn, const Representation& rpr);
  static bool fUses(const FileName& fn, const FileName& fnCheck);
  static bool fUses(const FileName& fn, const Column& col);
  static bool fUses(const FileName& fn, const String& sSection, const FileName& fnCheck);
  static bool fUses(const FileName& fn, const String& sSection, const Column& col);
  // miscellaneous
  static void GetDataFiles(const FileName& fn, Array<FileName>& afnDat);
  static void _export Add(Array<FileName>& afn, const FileName& fn, const String& sPath);
  static bool _export fTable(const FileName& fn);
  static bool _export fDomain(const FileName& fn, bool fAllowDomPicture = true);
  static bool _export fRepresentation(const FileName& fn);
  static bool _export fGeoRef(const FileName& fn);
  static bool _export fSystemObject(const FileName& fn);
	static bool _export fIsPartOfDataBase(const FileName& fn, const FileName& fnDB=FileName());
	static bool _export fUseAs(const FileName& fn);
  // file should know if it is part of a collection or not
  static void _export WriteAdditionOfFileToCollection(const FileName& filename, const FileName& fnCollection);
  static void _export WriteRemovalOfFileFromCollection(const FileName& filename, const FileName& fnCollection);
	static _export void RemoveFileFromLinkedCollections(const FileName& filename)	;
  // read from object def file
  static int _export ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, String& sValue);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, int& iValue);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, long& iValue);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, double& rValue);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, bool& fValue);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, RealArray& arr);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, RealMatrix& mat);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, IntMatrix& mat);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, Coord& crd);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, LatLon& ll);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, RowCol& rc);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, MinMax& mm);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, CoordBounds& cb);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, CSize& sz);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, CRect& rect);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, RangeInt& rng);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, RangeReal& rng);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, Time& tim);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, ValueRange& vr);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& filename, Color& col);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, FileName& fn);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, Domain& dm);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, GeoRef& gr);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, CoordSystem& cs);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, Representation& rpr);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, Map& map);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, SegmentMap& map);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, PolygonMap& map);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, PointMap& map);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, Table& tbl);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, MapList& mpl);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& fnObj, Filter& flt);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                          const FileName& fnObj, Classifier& clf);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                   const FileName& fnObj, SampleSet& sms);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                   const FileName& fnObj, char* pc, int iLen);
  static int _export ReadElement(const char* sSection, const char* sEntry,
                   const FileName& fnObj, CObject* cobj);
  // write to object def file                
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                         const FileName& filename, const char* sValue);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                         const FileName& filename, const String& sValue);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                         const FileName& filename, double rValue);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                         const FileName& filename, long iValue);
  static bool WriteElement(const char* sSection, const char* sEntry,
                         const FileName& filename, int iValue)
		{ return WriteElement(sSection, sEntry, filename, (long)iValue); }
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                         const FileName& filename, bool fValue);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, const FileName& fn);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                  const FileName& fnObj, const IlwisObject& obj);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                           const FileName& fnObj, const RangeInt& rng);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                           const FileName& fnObj, const RangeReal& rng);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                           const FileName& filename, const Time& tim);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const Domain& dm);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const Representation& rpr);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const GeoRef& gr);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const RealArray& arr);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const RealMatrix& mat);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const IntMatrix& mat);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const Coord& crd);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const LatLon& ll);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const RowCol& rc);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const MinMax& mm);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const CSize& sz);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const CRect& rect);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const CoordBounds& cb);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, const ValueRange& vr);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                          const FileName& filename, Color col);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                   const FileName& fnObj, const char* pc, int iLen);
  static bool _export WriteElement(const char* sSection, const char* sEntry,
                   const FileName& fnObj, CObject& cobj); // can't be const CObject&, due to Serialize 
  static char _export cLastDataFileExtChar(const FileName& fnObj, const String& sExt);
  static long iTotalFileSize(const Array<FileName>& afn);
	static void _export RemoveSection(const FileName& fn, const String& sSection);
	static void _export RemoveKey(const FileName& fn, const String& sSection, const String& sKey);
private:                  
  static bool fUsedInOtherObjects(Tranquilizer* trq, const FileName& fn, 
                                  const Array<String>& asExtMask, Array<String>* as);
};

#endif





