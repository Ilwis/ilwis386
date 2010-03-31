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
/* $Log: /ILWIS 3.0/PointMap/PNT.cpp $
 * 
 * 37    19-05-05 17:57 Retsios
 * [bug=6424] Properly open the maps in case the supplied attribute column
 * contains spaces. Added a safeguard to p to reduce the chance of reading
 * characters outside the string bounds.
 * 
 * 36    15-10-03 13:53 Retsios
 * Added function to fill the pointmap from a buffer (performance reason)
 * 
 * 35    10-01-03 11:03 Hendrikse
 * added  SetCoordSystem(const CoordSystem&); to be used by property sheet
 * when user changes csy of pointmap. It calls
 * pointmapstore::SetCoordSystem(const CoordSystem&); to ensure that the
 * coordinate column also gets the new csy as domain
 * 
 * 34    12/06/01 14:49 Willem
 * ForeignFormat pointer is now deleted after use
 * 
 * 33    12/05/01 10:27 Willem
 * The histogram of all maptypes is now also copied or deleted with the
 * map it belongs to
 * 
 * 32    8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 31    22-03-01 11:23a Martin
 * extra guard against not created foreign formats
 * 
 * 30    5/03/01 10:34 Willem
 * Removed superfluous member _iPoints from PointMapPtr: iPnt() now gets
 * the number of points from the PointMapStore or PointMapVirtual
 * 
 * 29    26/02/01 12:58 Willem
 * iPnt() function now queries either the mapstore of the MapVirtual for
 * the number of points.
 * 
 * 28    20-02-01 2:43p Martin
 * unneeded foreign file was added in the getobjectstructure. Ilwisobject
 * already does this
 * 
 * 27    2/08/01 12:05 Retsios
 * Solved a few more potential "dot in pathname detected as extension's
 * dot" bugs
 * 
 * 26    2/01/01 17:33 Retsios
 * Now PointMap::PointMap(const String& sExpression, const String& sPath)
 * correctly handles dots that are part of the path and not the filename
 * (bug 3884)
 * 
 * 25    21-12-00 10:22a Martin
 * added access function for the virtualobject
 * 
 * 24    21-11-00 15:39 Hendrikse
 * in Open...MapVirtual() load ObjectDependcency after the creation of the
 * ...MapVirtual. This prevents a.o.a problems with internal tables and
 * columns (...MapAttribute)
 * 
 * 23    27-10-00 10:20a Martin
 * added foreignformat files to the getobjectstructure
 * 
 * 22    11-09-00 11:33a Martin
 * added function for objectstructure and DoNotUpdate
 * 
 * 21    8-09-00 4:40p Martin
 * added function GetObjectStructure and DoNotUpdate
 * 
 * 20    10-08-00 15:14 Koolhoven
 * PointMap(const FileName& fn) when fn.sCol has a value now returns an
 * attribute map
 * 
 * 19    20-06-00 8:26a Martin
 * sets outbounds of map
 * 
 * 18    27-04-00 12:46 Koolhoven
 * Added function UndoAllChanges()
 * and internal help funciton Load()
 * 
 * 17    9-03-00 8:38a Martin
 * changed the usage of tblptr to table in LayerInfo
 * 
 * 16    3-03-00 4:33p Martin
 * temporary removed delete for ForeignFormat *ff, caused crash (???)
 * 
 * 15    28-02-00 10:23a Martin
 * Wrong test to see if a pointmap is foreign or not with existsting mpp
 * files, now corrected.
 * 
 * 14    28-02-00 8:49a Martin
 * changes to do with multiple shapes in one layer. The LayerInfo object
 * is now passed as parm
 * 
 * 13    21-02-00 4:38p Martin
 * Pntmap works now with PointMapStoreForeignFormat for non ilwis formats
 * 
 * 12    15-02-00 8:55a Martin
 * Added changes for use of foreign formats
 * 
 * 11    17-01-00 11:29 Wind
 * added proximity to iValue(const Coord& ..) etc.
 * 
 * 10    12-01-00 17:12 Wind
 * removed proximity (moved to basemap)
 * 
 * 9     8-12-99 16:16 Wind
 * delete table if definition failed
 * 
 * 8     25-10-99 13:14 Wind
 * making thread save (2); not yet finished
 * 
 * 7     22-10-99 12:54 Wind
 * thread save access (not yet finished)
 * 
 * 6     9/24/99 10:37a Wind
 * replaced calls to static funcs ObjectInfo::ReadElement and WriteElement
 * by calls to member functions
 * 
 * 5     9/08/99 1:00p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 4     9/08/99 10:21a Wind
 * adpated to use of quoted file names
 * 
 * 3     3/23/99 9:05a Martin
 * //->/*
 * 
 * 2     3/23/99 9:05a Martin
 * Case Problem solved
// Revision 1.12  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.11  1997/09/29 13:07:55  Wim
// Update nr of points according to number of records in table of pointmapstroe
// in constructor of PointMapPtr
//
// Revision 1.10  1997-09-12 17:16:20+02  Wim
// Default proximity is now 1e20 instead of 0.
//
// Revision 1.9  1997-09-11 20:40:41+02  Wim
// Allow to break dependencies even when source objects are gone
//
// Revision 1.8  1997-09-09 14:02:07+02  Wim
// In Store() make sure the map remains dependent even when
// the virtual map has not been loaded into memory
//
// Revision 1.7  1997-09-04 12:45:58+02  Wim
// Delete Histogram in DeleteCalc() function
//
/* PointMap
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   29 Sep 97    3:06 pm
*/

#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\Col.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\DataExchange\PointMapStoreForeign.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "PointApplications\PNTATTRB.H"
#include "Engine\Table\TBLHSTPT.H"
#include "Engine\Base\System\mutex.h"



//----------------------------------------------------------
FileName PointMap::fnFromExpression(const String& sExpr)
{
  char *p = sExpr.strrchrQuoted('.');
  // Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
  char *q = sExpr.strrchrQuoted('\\');
  // *p is definitely not an extension's dot if p<q
  if (p<q)
  	p = 0;
  // Now we're (only) sure that if p!=0 it points to the last dot in the filename
  if ((p == 0) || (0 == _strcmpi(p, ".mpp")))  // no attrib map
    return FileName(sExpr, ".mpp", true);
  return FileName();
}

PointMap::PointMap()
{
}

PointMap::PointMap(const FileName& fn)
: BaseMap() //FileName(fn, ".mpp"))
{
	if ("" == fn.sCol) {
		FileName fnPnt(fn, ".mpp", false);
		MutexFileName mut(fnPnt);
		SetPointer(PointMapPtr::create(fnPnt));
	}
	else {
		MutexFileName mut(fn);
		String sExpression = fn.sFullName();
    SetPointer(PointMapPtr::create(FileName(),sExpression));
	}
}

PointMap::PointMap(const FileName& fn, const CoordSystem& cs,
                   const CoordBounds& cb, const DomainValueRangeStruct& dvs)
: BaseMap()
{
  FileName fnPnt = fn;
  fnPnt.sExt = ".mpp";
  MutexFileName mut(fnPnt);
  SetPointer(new PointMapPtr(fnPnt, cs, cb, dvs, 0));
}

PointMap::PointMap(const FileName& fnPointMap, LayerInfo li) :
	BaseMap()
{
	SetPointer(new PointMapPtr(fnPointMap, li));
}


PointMap::PointMap(const String& sExpression)
: BaseMap()
{
  FileName fnPnt = fnFromExpression(sExpression); 
  MutexFileName mut(fnPnt);
  if (fnPnt.fValid())
    SetPointer(PointMapPtr::create(fnPnt));
  else 
    SetPointer(PointMapPtr::create(FileName(),sExpression));
  if (fValid())
    ptr()->Store();
}

PointMap::PointMap(const String& sExpression, const String& sPath)
: BaseMap()
{
 // check if it's a potential attribute map
  String sExpr =  sExpression;
  char *p = sExpr.strrchrQuoted('.');
	// Bas Retsios, 1 February 2001: *p is not necessarily an extension's dot
	char *pMax = (sExpr.length() > 0) ? (const_cast<char*>(&(sExpr)[sExpr.length() - 1])) : 0; // last valid char in sExpr
	char *q = sExpr.strrchrQuoted('\\');
	// *p is definitely not an extension's dot if p<q
	if (p<q || p>=pMax)
		p = 0;
	// Now we're (only) sure that p points to the last dot in the filename
  if (p)
    *p = 0;
  const String sFile = sExpr.scVal();
  FileName fn(sFile, ".mpp", true);
  if (0 == strchr(sFile.scVal(), ':')) // no path set
    fn.Dir(sPath); 
  MutexFileName mut(fn);
  if (p && (0 != _strcmpi(p+1, "mpp"))) {
    if (isalpha(*(p+1)) || ((p<(pMax-1)) && *(p+1) == '\'' && isalpha(*(p+2)))) // attrib column (right side of || checks on quoted column)
//      SetPointer(new PointMapAttribute(FileName(), PointMap(fn), p+1));
      SetPointer(PointMapPtr::create(FileName(), String("PointMapAttribute(%S,%s)", fn.sFullNameQuoted(), p+1)));
    else {
      fn.sFile = "";
      fn.sExt = "";
      SetPointer(PointMapPtr::create(fn, sExpression));
    }  
  }    
  else  
    SetPointer(PointMapPtr::create(fn));
}

PointMap::PointMap(const FileName& fn, const String& sExpression)
: BaseMap()
{
  FileName fnPnt(fn, ".mpp", false);
  MutexFileName mut(fnPnt);
  SetPointer(PointMapPtr::create(fnPnt, sExpression));
}

PointMap::PointMap(const PointMap& mp)
: BaseMap(mp.pointer())
{}

void PointMap::Export(const FileName& fn) const
{
	if (!ptr())
		return;
	ptr()->Export(fn);
}

PointMapPtr::PointMapPtr(const FileName& fn, bool fCreate)
  : BaseMapPtr(fn, fCreate), pms(0), pmv(0)
{
	if (fCreate) 
		return;
	Load();
}

PointMapPtr::PointMapPtr(const FileName& fnPointMap, LayerInfo inf) :
	BaseMapPtr(),
	pmv(0),
	pms(0)
{
	if ( inf.iShapes == 0)
	{
		fErase=true;
		return;
	}
	SetCoordSystem(inf.csy);
	SetDomainValueRangeStruct(inf.dvrsMap);
	Table tbl;
	tbl= inf.tblattr;
	cbOuter = inf.cbMap;
	SetAttributeTable(tbl);
	SetFileName(inf.fnObj);

	pms = new PointMapStoreForeign(fnPointMap, *this, inf);
}

PointMapPtr::PointMapPtr(const FileName& fn, const CoordSystem& cs, 
              const CoordBounds& cb, const DomainValueRangeStruct& dvs, long iPoints)
  : BaseMapPtr(fn, cs, cb, dvs), pms(0), pmv(0)
{
	pms = new PointMapStore(fnObj, *this, iPoints);
}

PointMapPtr::~PointMapPtr()
{
  if (fErase) {
    FileName fnHis = FileName(fnObj, ".hsp");
    if (File::fExist(fnHis)) {
      try { 
        Table his(fnHis);
        his->fErase = true;
      }
      catch (const ErrorObject&) {
      }  
    }  
  } 
  if (0 != pms) {
    delete pms;
    pms = 0;
  }
  if (0 != pmv)
    delete pmv;
  if (fErase) {
    WriteElement("PointMapStore", (char*)0, (char*)0);
    WriteElement("Table", (char*)0, (char*)0);
  } 
} 

void PointMapPtr::Load()
{
	if (sDescription.length() == 0) // downward compatibility with 2.02
		ReadElement("PointMapVirtual", "Expression", sDescription);
	String s;
	if (0 != ReadElement("TableStore", (char*)0, s)) 
	{
		ReadElement("TableStore", "Type", s);
		if ( s != "TableForeign")
			pms = new PointMapStore(fnObj, *this);
		else
		{
			ParmList pm;
			ForeignFormat *ff = ForeignFormat::Create(fnObj, pm);
			LayerInfo li = ff->GetLayerInfo(this, ForeignFormat::mtPointMap, false);
			if ( ff )
			{
				pms = new PointMapStoreForeign(fnObj, *this, li);
			}
			delete ff;
		}
		if (cbOuter.fUndef())
		{
			long iNr = iFeatures();
			for (long i = 1; i <= iNr; ++i)
			{
				Coord c = cValue(i);
				if (c.fUndef()) 
					continue;
				cbOuter += c;
			}
		}
	}  
}

void PointMapPtr::BreakDependency()
{
	if (!fCalculated())
		Calc();
	if (!fCalculated())
		return; 
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	delete pmv;
	pmv = 0;
	fChanged = true;
	WriteElement("PointMap", "Type", "PointMapStore");
	Store();
}

void PointMapPtr::Store()
{
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	bool fDep = fDependent();
	if ((0 != pmv) && (sDescription == ""))
		sDescription = pmv->sExpression();
	BaseMapPtr::Store();
	WriteElement("BaseMap", "Type", "PointMap");
	WriteElement("PointMap", "Points", iFeatures());
	if (0 != pms)
		pms->Store();
	else
		PointMapStore::UnStore(fnObj);
	if (fDep)
		WriteElement("PointMap", "Type", "PointMapVirtual");
	if (0 != pmv)
		pmv->Store();
	String s;
	if (0 == ReadElement("PointMap", "Type", s))
		WriteElement("PointMap", "Type", "PointMapStore");
}

String PointMapPtr::sType() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE,SOURCE_LOCATION);
  if (0 != pmv)
    return pmv->sType();
  else if (fDependent())
    return "Dependent Point Map";
  else
    return "Point Map";
}

PointMapPtr* PointMapPtr::create(const FileName& fn)
{
  if (!File::fExist(fn))
    NotFoundError(fn);
  MutexFileName mut(fn);
  PointMapPtr* p = BaseMap::pGetPntMap(fn);
  if (p) // if already open return it
    return p;
  p = new PointMapPtr(fn);
  return p;
}

PointMapPtr* PointMapPtr::create(const FileName& fn, const String& sExpression)
{
  if (fn.sFile.length() == 0) { // no file name
    // check if sExpression is an existing point map on disk
    // check if it's a potential attribute map
    char *p = sExpression.strrchrQuoted('.');
    // Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
    char *q = sExpression.strrchrQuoted('\\');
    // *p is definitely not an extension's dot if p<q
    if (p<q)
    	p = 0;
    // Now we're (only) sure that if p!=0 it points to the last dot in the filename
    if ((p == 0) || (0 == _strcmpi(p, ".mpp"))) { // no attrib map
      FileName fnMap(sExpression, ".mpp", true);
      if (File::fExist(fnMap)) {
        // see if map is already open (if it is in the list of open base maps)
        MutexFileName mut(fnMap);
        PointMapPtr* p = BaseMap::pGetPntMap(fnMap);
        if (p) // if already open return it
          return p;
        // open map and return it
        return PointMapPtr::create(fnMap);
      }
    }  
  }
  bool fPrevExist = File::fExist(fn);
  PointMapPtr* p = new PointMapPtr(fn, true);
  bool fPrevErase = p->fErase;
  p->fErase = true;
  p->pmv = PointMapVirtual::create(fn, *p, sExpression);
  p->fErase = fPrevErase;
  if ((0 != p->pmv) && fPrevExist) {
    PointMapStore::UnStore(fn);
//  unlink(fn.sFullName(true)); // delete previous object def file
    ObjectInfo::WriteElement("ObjectDependency", (char*)0, fn, (char*)0);
    ObjectInfo::WriteElement("PointMapVirtual", (char*)0, fn, (char*)0);
    TableHistogramPnt::Erase(fn); // delete previous histogram
  }  
  if (0 != p->pmv) {
    p->SetDomainChangeable(p->pmv->fDomainChangeable());
    p->SetValueRangeChangeable(p->pmv->fValueRangeChangeable());
    p->SetExpressionChangeable(p->pmv->fExpressionChangeable());
  }
  return p;
}

long PointMapPtr::iFeatures() const
{ 
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE,SOURCE_LOCATION);
	if (0 != pms)
		return pms->iPnt();
	if (pmv != 0)
		return pmv->iPnt();
	
	return 0;
}

vector<long> PointMapPtr::viRaw(const Coord& c, double rPrx) const
{ 
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE,SOURCE_LOCATION);
  vector<long> v;
  v.push_back(iRaw(iRec(c, rPrx))); 
  return v;
}

vector<long> PointMapPtr::viValue(const Coord& c, double rPrx) const
{ 
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE,SOURCE_LOCATION);
  vector<long> v;
  v.push_back(iValue(iRec(c, rPrx))); 
  return v;
}  

vector<double> PointMapPtr::vrValue(const Coord& c, double rPrx) const
{ 
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  vector<double> v;
  v.push_back(rValue(iRec(c, rPrx))); 
  return v;
}

vector<String> PointMapPtr::vsValue(const Coord& c, short iWidth, short iDec, double rPrx) const
{ 
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  vector<String> v;
  v.push_back(sValue(iRec(c, rPrx), iWidth, iDec)); 
  return v;
}

Coord PointMapPtr::cValue(const String& sValue) const
{ 
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  return cValue(iRec(sValue)); 
}

bool PointMapPtr::fPntInMask(long iRec, const Mask& mask) const
{
  if (fValues()) {
    String sVal = sValue(iRec,0);
    return mask.fInMask(sVal);
  }
  else
    return mask.fInMask(iRaw(iRec));
}

void PointMapPtr::Bounds(Coord& crdMin, Coord& crdMax) const
{
  if (iFeatures()<=0) {
    crdMin = crdUNDEF;
    crdMax = crdUNDEF;
    return;
  }
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  Coord crd;
  crdMin = Coord(DBL_MAX, DBL_MAX);
  crdMax = Coord(-DBL_MAX, -DBL_MAX);
  for (long i=1; i<=iFeatures(); i++) {
    crd = cValue(i);
    if (crd.x  == rUNDEF || crd.y == rUNDEF)
      continue;
    if (crd.x < crdMin.x)
      crdMin.x = crd.x;
    if (crd.y < crdMin.y)
      crdMin.y = crd.y;
    if (crd.x > crdMax.x)
      crdMax.x = crd.x;
    if (crd.y > crdMax.y)
      crdMax.y = crd.y;
  }
}


void PointMapPtr::CalcMinMax()
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (fnObj.sFile.length() == 0) {
    _rrMinMax = dvrs().rrMinMax();
    _rrPerc1 = dvrs().rrMinMax();
    _riMinMax = dvrs().riMinMax();
    _riPerc1 = dvrs().riMinMax();
    return;
  }
  if (dm()->pdbit() || (dm()->fnObj.sFile == "bool")/*not a nice check*/) {
    _rrMinMax = RangeReal(0,1);
    _riMinMax = RangeInt(0,1);
    return;
  }
  Table his;
  FileName fnHis(fnObj, ".hsp", true);
  if (File::fExist(fnHis))
    his = Table(fnHis);
  else
    his = Table(fnHis, String("TableHistogramPnt(%S)", sNameQuoted()));
  his->Calc(); // sets 0 and 1 perc interval
}

void PointMapPtr::Rename(const FileName& fnNew)
{
}

long PointMapPtr::iRaw(long iRec) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->iRaw(iRec);
  else if (0 != pmv)
    return pmv->iRaw(iRec);
  return iUNDEF;
}

long PointMapPtr::iValue(long iRec) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->iValue(iRec);
  else if (0 != pmv)
    return pmv->iValue(iRec);
  return iUNDEF;
}

double PointMapPtr::rValue(long iRec) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->rValue(iRec);
  else if (0 != pmv)
    return pmv->rValue(iRec);
  return rUNDEF;
}

String PointMapPtr::sValue(long iRec, short iWidth, short iDec) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->sValue(iRec, iWidth, iDec);
  else if (0 != pmv)
    return pmv->sValue(iRec, iWidth, iDec);
  return sUNDEF;
}

Coord PointMapPtr::cValue(long iRec) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->cValue(iRec);
  else if (0 != pmv)
    return pmv->cValue(iRec);
  return crdUNDEF;
}

Geometry *PointMapPtr::getFeature(long i) const {

  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->getFeature(i);
  return NULL;
}

long PointMapPtr::iRec(const String& s) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->iRec(s);
//if (0 != pmv)
//  return pmv->iRec(s);
  return iUNDEF;
}

long PointMapPtr::iRec(long iRaw) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->iRec(iRaw);
//if (0 != pmv)
//  return pmv->iRec(iRaw);
  return iUNDEF;
}

long PointMapPtr::iRec(const Coord& c, double rPrx) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->iRec(c, rPrx);
//if (0 != pmv)
//  return pmv->iRec(c);
  return iUNDEF;
}

void PointMapPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->GetDataFiles(afnDat, asSection, asEntry);
}

void PointMapPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
  ObjectDependency::Read(fnObj, afnObjDep);
}

void PointMapPtr::KeepOpen(bool f)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  BaseMapPtr::KeepOpen(f);
  if (0 != pms)
    pms->KeepOpen(f);
}

void PointMapPtr::Export(const FileName& fn) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pmv)
    pmv->Freeze();
  if (0 != pms)
    pms->Export(fn);
}

void PointMapPtr::PutRaw(long iRec, long iRaw)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutRaw(iRec, iRaw);
  Updated();
}

void PointMapPtr::PutVal(long iRec, double rVal)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutVal(iRec, rVal);
}

void PointMapPtr::PutVal(long iRec, const String& sVal)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutVal(iRec, sVal);
}

void PointMapPtr::PutVal(long iRec, const Coord& c)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutVal(iRec, c);
}

void PointMapPtr::Delete(long iRec)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->Delete(iRec);
}

long PointMapPtr::iAdd(long iRecs)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->iAdd(iRecs);
  return iUNDEF;
}

long PointMapPtr::iAddRaw(const Coord& c, long iRaw)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->iAddRaw(c, iRaw);
  return iUNDEF;
}

long PointMapPtr::iAddVal(const Coord& c, const String& s)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    return pms->iAddVal(c, s);
  return iUNDEF;
}

long PointMapPtr::iAddVal(const Coord& c, double rValue)
{
  if (0 != pms)
    return pms->iAddVal(c, rValue);
  return iUNDEF;
}

void PointMapPtr::PutBufRaw(const CoordBuf& cb, const LongBuf& lb)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (0 != pms)
    pms->PutBufRaw(cb, lb);
}

bool PointMapPtr::fConvertTo(const DomainValueRangeStruct& dvs, const Column& col)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  if (col.fValid() && col->dvrs() == dvs)
    return false;
  else if (dvrs() == dvs)
    return false;
  bool f = false;
  if (0 != pms)
    f = pms->fConvertTo(dvs, col);
  if (col.fValid())
    SetDomainValueRangeStruct(col->dvrs());
  else
    SetDomainValueRangeStruct(dvs);
  return f;
}

void PointMapPtr::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvrs)
{
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
    BaseMapPtr::SetDomainValueRangeStruct(dvrs);
    //if ( pms )
    //    pms->colVal->SetDomainValueRangeStruct(dvrs);
}


bool PointMapPtr::fDependent() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pmv)
    return true;
  String s;
  ReadElement("PointMap", "Type", s);
  return fCIStrEqual(s , "PointMapVirtual");
}

String PointMapPtr::sExpression() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != pmv)
    return pmv->sExpression();
  String s;
  s = IlwisObjectPtr::sExpression();
  if (s.length() == 0) // for downward compatibility with 2.02
    ReadElement("PointMapVirtual", "Expression", s);
  return s;
}

bool PointMapPtr::fCalculated() const
// returns true if a calculated result exists
{
  if (!fDependent())
    return IlwisObjectPtr::fCalculated();
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csCalc), TRUE, SOURCE_LOCATION);
  return 0 != pms;
} 

bool PointMapPtr::fDefOnlyPossible() const
// returns true if data can be retrieved without complete calculation (on the fly)
{
  if (!fDependent())
    return IlwisObjectPtr::fDefOnlyPossible();
  bool f;
  if (0 != ReadElement("PointMapVirtual", "DefOnlyPossible", f))
    return f;
  return false;
}

void PointMapPtr::Calc(bool fMakeUpToDate)
// calculates the result     
{
  ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
  OpenPointMapVirtual();
  if (fMakeUpToDate)
    if (!objdep.fUpdateAll())
      return;
  if (0 != pmv) 
    pmv->Freeze();
}


void PointMapPtr::DeleteCalc()
// deletes calculated  result     
{
  TableHistogramPnt::Erase(fnObj);
  if (0 == pms)
    return;
  ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
  OpenPointMapVirtual();
  if (0 != pmv)
    pmv->UnFreeze();
}

void PointMapPtr::OpenPointMapVirtual()
{
  if (0 != pmv) // already there
    return;
  if (!fDependent())
    return;
  try {
    pmv = PointMapVirtual::create(fnObj, *this);
    String s;
    // for downward compatibility with 2.02 :
    if (0 == ReadElement("IlwisObjectVirtual", (char*)0, s)) {
      SetDomainChangeable(pmv->fDomainChangeable());
      SetValueRangeChangeable(pmv->fValueRangeChangeable());
      SetExpressionChangeable(pmv->fExpressionChangeable());
    }
    objdep = ObjectDependency(fnObj);
  }
  catch (const ErrorObject& err) {
    err.Show();
    pmv = 0;
    objdep = ObjectDependency();
  }
}

void PointMapPtr::CreatePointMapStore()
{
  if (0 != pmv)
    pmv->CreatePointMapStore();
}

void PointMapPtr::UndoAllChanges()
{
	delete pms;
	Load();
}

void PointMapPtr::GetObjectStructure(ObjectStructure& os)
{
	BaseMapPtr::GetObjectStructure(os);
	os.AddFile(fnObj, "TableStore", "Data");
	FileName fnHist(fnObj, ".hsp");
	if (File::fExist(fnHist) && IlwisObject::iotObjectType( fnHist ) != IlwisObject::iotANY)
	{
		IlwisObject obj = IlwisObject::obj(fnHist);
		if ( obj.fValid())
			obj->GetObjectStructure(os);
	}					
}


void PointMapPtr::DoNotUpdate()
{
	BaseMapPtr::DoNotUpdate();
	if ( pms )
		pms->DoNotUpdate();
	
}

IlwisObjectVirtual *PointMapPtr::pGetVirtualObject() const
{
	return pmv;
}

void PointMapPtr::SetCoordSystem(const CoordSystem& cs) // dangerous function!
{
	BaseMapPtr::SetCoordSystem(cs);
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if ( pms )
		pms->SetCoordSystem(cs);
}

Feature* PointMapPtr::newFeature(geos::geom::Geometry *pnt)
{
  if (0 != pms)
    return pms->pntNew(pnt);
  return NULL;
}

vector<Geometry *> PointMapPtr::getFeatures(Coord crd, double rPrx) {
	if ( 0 != pms)
		return pms->getFeatures(crd, rPrx);
	return vector<Geometry *>();
}