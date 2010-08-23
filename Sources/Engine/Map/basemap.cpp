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
/* $Log: /ILWIS 3.0/BasicDataStructures/basemap.cpp $
 * 
 * 36    11/26/01 12:33 Willem
 * Getting the object structure for dependent data of an object is now
 * handled by IlwisObject instead of the individual objects
 * 
 * 35    13-08-01 16:57 Koolhoven
 * readability: replaced "== false" by a "!"
 * 
 * 34    29-03-01 11:39a Martin
 * fDataReadOnly function returns correct value
 * 
 * 33    7-03-01 16:46 Koolhoven
 * tblAtt() also stores the Table internally when the table comes from the
 * Domain, this prevents removed tables in record editing
 * 
 * 32    7-03-01 4:13p Martin
 * fDataReadOnly implemented. It checks for foreignformat files if their
 * format supports editing
 * 
 * 31    26/02/01 17:14 Willem
 * The CoordSystem is now not read from the ODF for raster maps; GeoRef
 * will provide the correct CoordSystem
 * 
 * 30    23-01-01 2:20p Martin
 * copier will now break the dependency of the target object, not of the
 * source object
 * 
 * 29    30/11/00 12:14 Willem
 * SetDomainValueRangeStruct() function now checks if the dmOld is a
 * DomainNone before clearing the attribute table
 * 
 * 28    23-11-00 10:12a Martin
 * for internal domains the data files are now required files in
 * getobjectstructure
 * 
 * 27    23-11-00 9:31a Martin
 * made a typo in the getobjectstructure for the section of dependencies
 * 
 * 26    17-11-00 15:26 Koolhoven
 * sTblAtt() now calls  ObjectInfo::fnAttributeTable() instead of reading
 * itself in the odf
 * 
 * 25    6-11-00 15:32 Koolhoven
 * tblAtt now returns simply a Table, not by reference anymore
 * 
 * 24    11/01/00 12:11p Martin
 * added errormap to the objectsructure members
 * 
 * 23    30-10-00 9:38a Martin
 * added handling for internal domains in the GetObjectStructure. Should
 * not be here but ilwis has no other option
 * 
 * 22    27-10-00 4:19p Martin
 * list of objdependencies added in objectstructure list
 * 
 * 21    25-10-00 4:33p Martin
 * changed the getobjectstructure function
 * 
 * 20    11-09-00 10:05a Martin
 * changed the structure of ObjectStructure object to include odf entry
 * information
 * 
 * 19    8-09-00 3:21p Martin
 * added function to set the fChanged member of all the 'members of an
 * object.
 * added function to retrieve the 'structure' of an object (filenames)
 * 
 * 18    10/08/00 9:50 Willem
 * The constructor of BaseMap now calls the BaseMap::pGet() function to
 * retrieve the proper BaseMapPtr. This is needed for polygonmaps that
 * contain segmentmaps with the same name
 * 
 * 17    4/26/00 5:27p Hendrikse
 * improved GetDataFiles using now the Basemap(fn)  constructor
 * 
 * 16    4/26/00 3:31p Hendrikse
 * adapted GetDataFiles(_) to ensure that while copying a KrigingMap the
 * associated ErrorMap is copied as well. 
 * This will then work for all maps producing an errormap
 * 
 * 15    21-02-00 4:35p Martin
 * Added a function to quickly add a whole set of values (AddValues)
 * 
 * 14    15-02-00 8:55a Martin
 * Added changes for use of foreign formats
 * 
 * 13    8-02-00 12:34 Wind
 * bug when reading in proximity
 * 
 * 12    17-01-00 11:30 Wind
 * added proximity to iValue(const Coord& ..) etc.
 * 
 * 11    12-01-00 17:13 Wind
 * added proximity and erormap functions
 * 
 * 10    10-01-00 2:05p Martin
 * added function toaccess to cbOuter member
 * 
 * 9     5-01-00 18:00 Wind
 * prevented SegmentMap of PolygonMap to destruy some entries in the ODF
 * 
 * 8     29-10-99 12:59 Wind
 * case sensitive stuff
 * 
 * 7     22-10-99 12:56 Wind
 * thread save access (not yet finished)
 * 
 * 6     10/01/99 1:37p Wind
 * support for map syntax maplist:bandnr
 * 
 * 5     9/29/99 11:01a Wind
 * added support for FileName::sSectionPostFix
 * 
 * 4     22-09-99 15:51 Koolhoven
 * SetNoAttributeTable() debugged
 * 
 * 3     9/08/99 10:10a Wind
 * adapted to use of quoted file names for attribute tables
 * 
 * 2     3/11/99 12:15p Martin
 * Added support for Case insesitive 
// Revision 1.8  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.7  1998/03/06 16:40:59  janh
// Initialization of _fAttTable was not done.
//
// Revision 1.6  1997/09/04 10:41:38  Wim
// Call DeleteCalc() in SetValueRangeStruct() and SetValueRange()
//
// Revision 1.5  1997-08-20 18:41:40+02  Wim
// Added Updated():
// reset minmax and perc1 interval
//
// Revision 1.4  1997-08-19 16:50:41+02  Wim
// Added BaseMap::pGetRasMap(), pGetSegMap(), pGetPolMap(), pGetPntMap()
// because all basemapptr's are in the same list.
// This caused problems with polygonmapstore which has a segmentmap
// inside, with the same filename.
// Due to the static casts you got some "funny" results.
//
// Revision 1.3  1997-08-19 14:53:51+02  Wim
// Some protections agains using invalid dm()
//
/* BaseMapPtr
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  J    21 Oct 99   10:03 am
*/
#include "Headers\toolspch.h"
#include "Engine\Map\Feature.h"
#include "Engine\Map\basemap.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dminfo.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Base\System\mutex.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Headers\Hs\DAT.hs"

IlwisObjectPtrList BaseMap::listMap;

BaseMap::BaseMap()
: IlwisObject(listMap)
{}

BaseMap::BaseMap(const BaseMap& mp)
: IlwisObject(listMap, mp.pointer())
{}


BaseMap::BaseMap(const FileName& filename)
: IlwisObject(listMap, FileName(filename, ".mpr", false))
{
	FileName fn(filename, ".mpr", false);
	BaseMapPtr* p = BaseMap::pGet(fn);
	if (p)
	{ // if already open return it
		SetPointer(p);
		return;
	}
	MutexFileName mut(fn);
	p = BaseMap::pGet(fn);
	if (p)  // if already open return it
		SetPointer(p);
	else
		SetPointer(BaseMapPtr::create(fn));
}

BaseMap::BaseMap(IlwisObjectPtr* ptr)
: IlwisObject(listMap, ptr)
{}

BaseMapPtr* BaseMap::pGet(const FileName& fn)
{
  if (fCIStrEqual(".mpr", fn.sExt))
    return pGetRasMap(fn);
  else if (fCIStrEqual(".mpa", fn.sExt))
    return pGetPolMap(fn);
  else if (fCIStrEqual(".mps", fn.sExt))
    return pGetSegMap(fn);
  else if (fCIStrEqual(".mpp", fn.sExt))
    return pGetPntMap(fn);
  else // you never know
    return static_cast<BaseMapPtr*>(listMap.pGet(fn));
}

SegmentMapPtr* BaseMap::pGetSegMap(const FileName& fn)
{
  if (!fn.fValid())
    return 0;
  ILWISSingleLock(&listMap.csAccess, TRUE, SOURCE_LOCATION);
  for (DLIterCP<IlwisObjectPtr> iter(&listMap); iter.fValid(); ++iter) {
    SegmentMapPtr* sm = dynamic_cast<SegmentMapPtr*>(iter());
    if (sm && (fn == sm->fnObj))
      return sm;
  }
  return 0;
}

PolygonMapPtr* BaseMap::pGetPolMap(const FileName& fn)
{
  if (!fn.fValid())
    return 0;
  ILWISSingleLock(&listMap.csAccess, TRUE, SOURCE_LOCATION);
  for (DLIterCP<IlwisObjectPtr> iter(&listMap); iter.fValid(); ++iter) {
    PolygonMapPtr* pm = dynamic_cast<PolygonMapPtr*>(iter());
    if (pm && (fn == pm->fnObj))
      return pm;
  }
  return 0;
}

PointMapPtr*   BaseMap::pGetPntMap(const FileName& fn)
{
  if (!fn.fValid())
    return 0;
  ILWISSingleLock(&listMap.csAccess, TRUE, SOURCE_LOCATION);
  for (DLIterCP<IlwisObjectPtr> iter(&listMap); iter.fValid(); ++iter) {
    PointMapPtr* pm = dynamic_cast<PointMapPtr*>(iter());
    if (pm && (fn == pm->fnObj))
      return pm;
  }
  return 0;
}

MapPtr*  BaseMap::pGetRasMap(const FileName& fn)
{
  if (!fn.fValid())
    return 0;
  ILWISSingleLock(&listMap.csAccess, TRUE, SOURCE_LOCATION);
  for (DLIterCP<IlwisObjectPtr> iter(&listMap); iter.fValid(); ++iter) {
    MapPtr* map = dynamic_cast<MapPtr*>(iter());
    if (map && (fn == map->fnObj))
      return map;
  }
  return 0;
}


BaseMapPtr::BaseMapPtr()
: IlwisObjectPtr(), 
  rProx(rUNDEF), 
	dvs(DomainValueRangeStruct(Domain("none")))
{
  _fAttTable = false;
  fKeepOpen = false;
}

BaseMapPtr::~BaseMapPtr()
{
  if (fErase && dm().fValid()) {
    if (dm()->fnObj == fnObj) {
      dm()->fErase = true;
      try {
        Representation rpr = dm()->rpr();
        if (rpr.fValid())
          rpr->fErase = true;
      }
      catch (const ErrorObject&) {
      }
    }
  }
}

String BaseMapPtr::sType() const
{
  return SDATMsgUnspecifiedMap;
}

BaseMapPtr* BaseMapPtr::create(const FileName& fn)
{
// err: invalid basemap type              ::create(fn)
  if (!File::fExist(fn))
    NotFoundError(fn);
//    throw ErrorNotFound(fn);
  if (fn.sSectionPostFix.length() > 0) {
    char *p = const_cast<char*>(&fn.sSectionPostFix[1]);
    long iBandNr = String(p).iVal();
    long iOffsetForBands;
    FileName fnMpl = fn;
    fnMpl.sSectionPostFix = "";
    ObjectInfo::ReadElement("MapList", "Offset", fnMpl, iOffsetForBands);
    FileName fnMap;
    ObjectInfo::ReadElement("MapList", String("Map%li", iBandNr-1+iOffsetForBands).scVal(), fnMpl, fnMap);
    if (fnMap.sSectionPostFix.length() > 0) {
      fnMpl.sSectionPostFix = fnMap.sSectionPostFix;
      fnMpl.sExt = ".mpl";
      return new MapPtr(fnMpl, false, iBandNr);
    }
    else 
      return MapPtr::create(fnMap);
  }

  String sType;
  ObjectInfo::ReadElement("BaseMap", "Type", fn, sType);
  if (fCIStrEqual("Map" , sType))
    return MapPtr::create(fn);
  if (fCIStrEqual("SegmentMap" , sType))
    return SegmentMapPtr::create(fn);
  if (fCIStrEqual("PolygonMap" , sType))
    return PolygonMapPtr::create(fn);
  if (fCIStrEqual("PointMap" , sType))
    return PointMapPtr::create(fn);
//  throw ErrorInvalidType(fn, "BaseMap", sType);
  InvalidTypeError(fn, "BaseMap", sType);
  return 0;
}

BaseMapPtr::BaseMapPtr(const FileName& fn, bool fCreate)
: IlwisObjectPtr(fn, fCreate), _fAttTable(false), rProx(rUNDEF)
{
  if (fCreate) 
    return;
  if (!File::fExist(fn))
    NotFoundError(fn);
  Time tim = objtime;

	// CoordSystem for raster is determined by GeoRef so skip it here
  if (!fCIStrEqual(fn.sExt, ".mpr"))
	ReadElement("BaseMap", "CoordSystem", _csys);
  ReadElement("BaseMap", "CoordBounds", cbOuter);
  Domain dom;
	String sType;
	ReadElement("BaseMap", "Domain", sType);
	ReadElement("BaseMap", "Domain", dom);
  dvs.SetDomain(dom,true); 
  if (dvs.fValues()) {
    ValueRange vr;
    ReadElement("BaseMap", "Range", vr);
    if (vr.fValid())
      dvs.SetValueRange(vr);
  }  
  // attribute table is loaded on demand, see tblAtt()
  ReadElement("BaseMap", "AttributeTable", sAttTable);
  _fAttTable = sAttTable.length() != 0;
  if (dvs.fValues()) {
    if (dvs.fRealValues()) {
      ReadElement("BaseMap", "MinMax", _rrMinMax);
      if (!_rrMinMax.fValid())
        ReadElement("Map", "MinMax", _rrMinMax); // for compat with 2.02 and previous
      ReadElement("BaseMap", "Perc1", _rrPerc1);
      if (!_rrPerc1.fValid())
        ReadElement("Map", "Perc1", _rrPerc1); // for compat with 2.02 and previous
      _riMinMax = RangeInt(longConv(_rrMinMax.rLo()),
                           longConv(_rrMinMax.rHi()));
      _riPerc1 = RangeInt(longConv(_rrPerc1.rLo()),
                          longConv(_rrPerc1.rHi()));
    }
    else {
      ReadElement("BaseMap", "MinMax", _riMinMax);
      if (!_riMinMax.fValid())
        ReadElement("Map", "MinMax", _riMinMax); // for compat with 2.02 and previous
      ReadElement("BaseMap", "Perc1", _riPerc1);
      if (!_riPerc1.fValid())
        ReadElement("Map", "Perc1", _riPerc1); // for compat with 2.02 and previous
      _rrMinMax = RangeReal(doubleConv(_riMinMax.iLo()), doubleConv(_riMinMax.iHi()));
      _rrPerc1 = RangeReal(doubleConv(_riPerc1.iLo()), doubleConv(_riPerc1.iHi()));
    }
  }
  ReadElement("BaseMap", "ErrorMap", fnErrMap);
  ReadElement("BaseMap", "Proximity", rProx);
  if (rProx == rUNDEF)
    rProx = rProxDefault();
  fChanged = false;  // SetDomain sets it on true
  objtime = tim; // SetDomain sets it on current time
}  

BaseMapPtr::BaseMapPtr(const FileName& fn, const CoordSystem& cs,
                       const CoordBounds& cb, const DomainValueRangeStruct& _dvs)
: IlwisObjectPtr(fn, true, fn.sExt.scVal())
{
  _csys = cs;
  cbOuter = cb;
  dvs = _dvs;
  _fAttTable = false;
  rProx = rProxDefault();
}

double BaseMapPtr::rProxDefault() const
{ 
  return min(cbOuter.width(), cbOuter.height()) / 200;
}


void BaseMapPtr::Store()
{
  IlwisObjectPtr::Store();
  WriteElement("Ilwis", "Type", "BaseMap");
  WriteElement("BaseMap", "CoordSystem", cs());
  WriteElement("BaseMap", "CoordBounds", cbOuter);
  // the following is needed to prevent the segment map from a polygon map
  // to destroy the entries in the ODF that follow 
  if (!dm().fValid()) // is true for a segmentmap of a polygonmap
    return;
  WriteElement("BaseMap", "Domain", dm());
  WriteElement("BaseMap", "Range", vr());
  if (!_fAttTable || sAttTable.length() == 0)
    WriteElement("BaseMap", "AttributeTable", (const char*)0);
  else
    WriteElement("BaseMap", "AttributeTable", sAttTable);
  if (dvs.fValues()) {
    if (dvs.fRealValues()) {
      WriteElement("BaseMap", "MinMax", _rrMinMax);
      WriteElement("BaseMap", "Perc1", _rrPerc1);
    }
    else {
      WriteElement("BaseMap", "MinMax", _riMinMax);
      WriteElement("BaseMap", "Perc1", _riPerc1);
    }
  }
  else {
    WriteElement("Map", "MinMax", (char*)0);
    WriteElement("Map", "Perc1", (char*)0);
  }
  if (rProx != rUNDEF)
    WriteElement("BaseMap", "Proximity", rProx);
  else
    WriteElement("BaseMap", "Proximity", (const char*)0);
  if (fErrorMap())
    WriteElement("BaseMap", "ErrorMap", fnErrorMap());
  else
    WriteElement("BaseMap", "ErrorMap", (const char*)0);

  if (dm().fValid())
    if (dm()->fnObj == fnObj)
      dm()->Store();
}
/*
void BaseMapPtr::SetDomain(const Domain& dom)
{
  _dm = dom;
  const_cast<DomainValue*>(_dv) = dm()->pdv();
  const_cast<DomainValueReal*>(_dvr) = dm()->pdvr();
  const_cast<DomainClass*>(_dc) = dm()->pdc();
//  assert(dm()->pdnone() == 0);
  SetValueRange(0);
  fChanged = true;
}
*/
void BaseMapPtr::SetCoordSystem(const CoordSystem& cs)
{
  if (_csys.fValid() && (_csys->fnObj == cs->fnObj))
    return;
  _csys = cs;
  Updated();
}

void BaseMapPtr::SetCoordBounds(const CoordBounds& cb)
{
  if (cbOuter == cb)
    return;
  cbOuter = cb;
  fChanged = true;
}

void BaseMapPtr::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvrs)
{
  if (dm().fValid()) {
    if (!dm()->fnObj.fValid() && !dvrs.dm()->fnObj.fValid())
      if (dm() == dvrs.dm())
        return;

    if (dm()->fnObj.fValid() && dvrs.dm()->fnObj.fValid())
      if (dm()->fnObj == dvrs.dm()->fnObj) {
        if (!vr().fValid() && !dvrs.vr().fValid())
          return;
        if (vr().fValid() && dvrs.vr().fValid())
          if (vr() == dvrs.vr())
            return;
      }
  }
    
  Domain dmOld = dm();
  dvs = dvrs;
  DeleteCalc();
  Updated();
  if (!dmOld->pdnone() && dm() != dmOld)
    SetAttributeTable(Table());
}  

void BaseMapPtr::SetValueRange(const ValueRange& vr)
{
  if (!dvs.fValues())
    return;
  if (!vr.fValid() && !dvs.vr().fValid())
    return;
  if (vr.fValid() && dvs.vr().fValid())
    if (vr == dvs.vr())
      return;
  dvs.SetValueRange(vr);
  DeleteCalc();
  Updated();
}

bool BaseMapPtr::fTblAtt() 
{ 
  if (_fAttTable)
    return true;
  if (!dm().fValid())
    return false;
  DomainSort* ds = dm()->pdsrt();
  if (0 == ds)
    return false;
  else
    return ds->fTblAtt();  
}

String BaseMapPtr::sTblAtt() const
{
	FileName fn = ObjectInfo::fnAttributeTable(fnObj);
	return fn.sFullPath();
}

Table BaseMapPtr::tblAtt()
{
	if (!_tblAtt.fValid())
	{
	  if (_fAttTable) 
      _tblAtt = tblAttLoad();
	  else if (dm().fValid()) 
		{
		  DomainSort* ds = dm()->pdsrt();
			if (ds)
				_tblAtt = ds->tblAtt();    
		}  
	}
  return _tblAtt;
}

Table BaseMapPtr::tblAttLoad()
{
  FileName fnTbl;
  Table tbl;
  try {
    ReadElement("BaseMap", "AttributeTable", tbl);
    if (tbl.fValid()) 
      if (dm() != tbl->dm()) {
         MessageBox(0, SDATErrInvalidAttrDomain.scVal(),SDATTitleLoadingAttrTable.scVal(), MB_OK | MB_ICONSTOP);
        tbl = Table();
      }
  }
  catch (const ErrorObject& err) {
    err.Show(SDATErrLoadAttrTableMap);
    tbl = Table();
  }
  if (tbl.fValid())
    sAttTable = tbl->sNameQuoted(true, fnObj.sPath());
  return tbl;
}
/*
Table BaseMapPtr::tblAttCopy(const FileName& fn) const
{
  // make a copy of attt table if it exists and has to go to other directory
  Table tblAttr = const_cast<BaseMapPtr*>(this)->tblAtt();
  if (fn.sPath() != fnObj.sPath() && tblAttr.fValid()) { 
    FileName fnTblCopy = tblAttr->fnObj;
    fnTblCopy.Dir(fn.sPath());
    if (fnObj.sFile == tblAttr->fnObj.sFile)
      fnTblCopy.sFile = fn.sFile;
    return tblAttr.Copy(fnTblCopy);
  }
  else
    return tblAttr;
}
*/

void BaseMapPtr::KeepOpen(bool f)
{
  fKeepOpen = f;
}

long BaseMapPtr::iRaw(const Coord& c, double rPrx) const
{
	vector<long> values = viRaw(c,rPrx);
	if ( values.size() > 0)
		return values.at(0);
	return iUNDEF;
}

vector<long> BaseMapPtr::viRaw(const Coord&, double rPrx) const
{
  return vector<long>();
}

long BaseMapPtr::iValue(const Coord& crd, double rPrx) const
{
  if (!dvs.fValues() || !dvs.fRawAvailable())
    return iUNDEF;
  long raw = iRaw(crd, rPrx);
  return dvs.iValue(raw);
}

vector<long> BaseMapPtr::viValue(const Coord& crd, double rPrx) const
{
  if (!dvs.fValues() || !dvs.fRawAvailable())
    return vector<long>();
  vector<long> raws = viRaw(crd, rPrx);
  for(int i=0; i<raws.size(); ++i)
	  raws[i] = dvs.iValue(raws[i]);
  return raws;
}

double BaseMapPtr::rValue(const Coord& crd, double rPrx) const
{
  if (!dvs.fValues() || !dvs.fRawAvailable())
    return rUNDEF;
  long raw = iRaw(crd, rPrx);
  return dvs.rValue(raw);
}

vector<double> BaseMapPtr::vrValue(const Coord& crd, double rPrx) const
{
  if (!dvs.fValues() || !dvs.fRawAvailable())
    return vector<double>();
  vector<long> raws = viRaw(crd, rPrx);
  vector<double> reals;
  for(int i=0; i<raws.size(); ++i)
	  reals.push_back(dvs.iValue(raws[i]));
  return reals;
}

String BaseMapPtr::sValue(const Coord& crd, short iWidth, short iDec, double rPrx) const
{
  if (dvs.fRawAvailable()) {
    long raw = iRaw(crd, rPrx);
    return dvs.sValueByRaw(raw, iWidth, iDec);
  }
  else {
    double val = rValue(crd);
    return dvs.sValue(val, iWidth, iDec);
  }
}

vector<String> BaseMapPtr::vsValue(const Coord& crd, short iWidth, short iDec, double rPrx) const
{
  vector<String> strings;
  if (dvs.fRawAvailable()) {
    vector<long> raws = viRaw(crd, rPrx);
	for(int i = 0; i < raws.size(); ++i)
		strings.push_back(dvs.sValueByRaw(raws[i], iWidth, iDec));
  }
  else {
	  vector<double> vals = vrValue(crd, rPrx);
	for(int i = 0; i < vals.size(); ++i)
		strings.push_back(dvs.sValueByRaw(vals[i], iWidth, iDec));
  }
  return strings;
}

void BaseMapPtr::SetAttributeTable(const Table& tbl)
{ 
  if (tbl.fValid()) {
    if (dm() != tbl->dm()) 
    {
        MessageBox(0, SDATErrInvalidAttrDomain.scVal(), SDATTitleLoadingAttrTable.scVal(), MB_OK | MB_ICONSTOP);
        return;
    }
  }
  _tblAtt = tbl;
  _fAttTable = _tblAtt.fValid();
  if (_fAttTable) {
    WriteElement("BaseMap", "AttributeTable", _tblAtt);
    sAttTable = tbl->sNameQuoted(true, fnObj.sPath());
  }  
  else {
    WriteElement("BaseMap", "AttributeTable", (const char*)0);
    sAttTable = "";
  } 
}

void BaseMapPtr::SetNoAttributeTable()
{ 
	Table tbl;
  SetAttributeTable(tbl);
}

RangeInt BaseMapPtr::riMinMax(bool fForce)
{
  if (!dvs.fValues())
    return RangeInt();
  if (dm().fValid() && (dm()->pdbit() || dm()->pdbool()))
    return RangeInt(0,1);
  if (fForce)
    if (!_riMinMax.fValid())  // not yet set
      CalcMinMax();
  return _riMinMax;
}

RangeReal BaseMapPtr::rrMinMax(bool fForce)
{
  if (!dvs.fValues())
    return RangeReal();
  if (dm().fValid() && (dm()->pdbit() || dm()->pdbool()))
    return RangeReal(0,1);
  if (fForce)
    if (!_rrMinMax.fValid())  // not yet set
      CalcMinMax();
  return _rrMinMax;
}

RangeInt BaseMapPtr::riPerc1(bool fForce)
{
  if (!dvs.fValues())
    return RangeInt();
  if (st() > stINT)
    return riMinMax(fForce);
  if (fForce)
    if (!_riPerc1.fValid())  // not yet set
      CalcMinMax();
  return _riPerc1;
}

RangeReal BaseMapPtr::rrPerc1(bool fForce)
{
  if (!dvs.fValues())
    return RangeReal();
  if (st() > stINT)
    return rrMinMax(fForce);
  if (fForce)
    if (!_rrPerc1.fValid())  // not yet set
      CalcMinMax();
  return _rrPerc1;
}

void BaseMapPtr::SetMinMax(const RangeInt& ri)
{ 
  _riMinMax = ri;  
  WriteElement("BaseMap", "MinMax", _riMinMax);
}

void BaseMapPtr::SetPerc1(const RangeInt& ri)
{ 
  _riPerc1 = ri; 
  WriteElement("BaseMap", "Perc1", _riPerc1);
}
  
void BaseMapPtr::SetMinMax(const RangeReal& rr)
{ 
  _rrMinMax = rr; 
  WriteElement("BaseMap", "MinMax", _rrMinMax);
}

void BaseMapPtr::SetPerc1(const RangeReal& rr)
{ 
  _rrPerc1 = rr; 
  WriteElement("BaseMap", "Perc1", _rrPerc1);
}

void BaseMapPtr::CalcMinMax()
{
  
  // implemented in derived classes
}

DomainInfo BaseMapPtr::dminf() const
{
  return DomainInfo(fnObj, "BaseMap");
}

bool BaseMapPtr::fConvertTo(const DomainValueRangeStruct&, const Column&)
{
  return false;
}

bool BaseMapPtr::fConvertTo(const DomainValueRangeStruct& _dvrsTo)
{
  return fConvertTo(_dvrsTo, Column()); 
}

bool BaseMapPtr::fConvertTo(const Column& col)
{
  return fConvertTo(col->dvrs(), col); 
}

void BaseMapPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  if (dm().fValid() && (dm()->fnObj == fnObj))
    dm()->GetDataFiles(afnDat, asSection, asEntry);
	FileName fnErrormap;
	int i = ReadElement("BaseMap", "ErrorMap", fnErrormap);
	BaseMap mpError;
	if (i > 0 )
		mpError = BaseMap(fnErrormap);
  if (mpError.fValid())
      mpError->GetDataFiles(afnDat, asSection, asEntry);
}

void BaseMapPtr::Updated()
{
  IlwisObjectPtr::Updated();
  _riMinMax = RangeInt();
  _rrMinMax = RangeReal();
  _riPerc1 = RangeInt();
  _rrPerc1 = RangeReal();
}


CoordBounds BaseMapPtr::cbGetCoordBounds() const
{
	return cbOuter;
}

void BaseMapPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	// beeuh solving internal objects at this level is bad. But ilwis has
	// no other option. (obj(..) function can not retrieve internal objects )	
	if ( fnObj == dm()->fnObj )	
	{
		os.AddFile(fnObj, "dom:TableStore", "data");
		os.AddFile(fnObj, "rpr:TableStore", "data");				
	}		
		
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "BaseMap", "AttributeTable");
		if ( fnObj != dm()->fnObj )
			os.AddFile(fnObj, "BaseMap", "Domain", ".dom");
	
		os.AddFile(fnObj, "BaseMap", "CoordSystem");
		os.AddFile(fnObj, "BaseMap", "ErrorMap");
	}	
}

bool BaseMapPtr::fDataReadOnly() const
{
	bool fDataEditable = !IlwisObjectPtr::fDataReadOnly();
	if ( !fDataEditable )
		return true;
	String sExpr;
	ReadElement("ForeignFormat", "Expression", sExpr);
	if ( sExpr != "" )
	{
		Array<String> arParts;
		// get the expression
		Split(sExpr, arParts, "=(,");
		// which method is used
		String sForeignMethod = arParts[0];
		// create a dummy foreignformat (use its virtuals)
		ForeignFormat *ff = ForeignFormat::Create(sForeignMethod);
		if ( ff )
		{
			fDataEditable = ff->fIsSupported(arParts[1], ForeignFormat::cbEDIT );
			delete ff;
		}			
	}
	return !fDataEditable;
}

