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


void PointMapPtr::removeFeature(const String& id, const vector<int>& selectedCoords) {
	if ( 0 != pms)
		pms->removeFeature(id, selectedCoords);
}