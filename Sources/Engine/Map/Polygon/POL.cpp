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
#include "Engine\Map\Feature.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Applications\POLVIRT.H"
#include "Engine\Map\Polygon\POL14.H"
#include "Engine\Base\System\Engine.h"
#include "Engine\Applications\POLVIRT.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\DataExchange\PolygonMapStoreForeign.h"
#include "Engine\DataExchange\PolygonMapStoreFormat20.h"
#include "Engine\Map\Polygon\PolygonStore37.h"
#include "Engine\Map\Polygon\PolygonStore30.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\Algorithm\Clipline.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Table\TBLHSTPL.H"
#include "Headers\Hs\polygon.hs"


// for the time being the interface works with the 1.4 datastructures
// this will soon change to a more structured tableinterface

FileName PolygonMap::fnFromExpression(const String& sExpr)
{
	char *p = sExpr.strrchrQuoted('.');
	// Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
	char *q = sExpr.strrchrQuoted('\\');
	// *p is definitely not an extension's dot if p<q
	if (p<q)
		p = 0;
	// Now we're (only) sure that if p!=0 it points to the last dot in the filename
	if ((p == 0) || (0 == _strcmpi(p, ".mpa")))  // no attrib map
		return FileName(sExpr, ".mpa", true);
	return FileName();
}

PolygonMap::PolygonMap()
{
}

PolygonMap::PolygonMap(const FileName& fn)
: BaseMap() 
{
	FileName fnPol(fn, ".mpa", false);
	MutexFileName mut(fnPol);
	SetPointer(PolygonMapPtr::create(fnPol));
}

PolygonMap::PolygonMap(const FileName& fnPolMap, LayerInfo li) :
BaseMap()
{
	SetPointer(new PolygonMapPtr(fnPolMap, li));
}


PolygonMap::PolygonMap(const FileName& fn, const String& sExpression)
{ 
	FileName fnPol(fn, ".mpa", false);
	MutexFileName mut(fnPol);
	SetPointer(PolygonMapPtr::create(fnPol,sExpression));
	if (fValid())
		ptr()->Store();
}

PolygonMap::PolygonMap(const String& sExpression)
{ 
	FileName fnPol = fnFromExpression(sExpression); 
	MutexFileName mut(fnPol);
	if (fnPol.fValid())
		SetPointer(PolygonMapPtr::create(fnPol));
	else 
		SetPointer(PolygonMapPtr::create(FileName(),sExpression));
}

PolygonMap::PolygonMap(const String& sExpression, const String& sPath)
: BaseMap()
{
	// check if it's a potential attribute map
	String sExpr =  sExpression;
	char *p = sExpr.strrchrQuoted('.');
	// Bas Retsios, 8 February 2001: *p is not necessarily an extension's dot
	char *pMax = (sExpr.length() > 0) ? (const_cast<char*>(&(sExpr)[sExpr.length() - 1])) : 0; // last valid char in sExpr
	char *q = sExpr.strrchrQuoted('\\');
	// *p is definitely not an extension's dot if p<q
	if (p<q || p>=pMax)
		p = 0;
	// Now we're (only) sure that if p!=0 it points to the last dot in the filename
	if (p) 
		*p = 0;
	String sFile = sExpr.sVal();
	FileName fn(sFile, ".mpa", true);
	if (0 == strchr(sFile.c_str(), ':')) // no path set
		fn.Dir(sPath); 
	MutexFileName mut(fn);
	if (p && (0 != _strcmpi(p+1, "mpa"))) {
		if (isalpha(*(p+1)) || ((p<(pMax-1)) && *(p+1) == '\'' && isalpha(*(p+2)))) // attrib column (right side of || checks on quoted column)
			//    SetPointer(new PolygonMapAttribute(FileName(), PolygonMap(fn), p+1));
			SetPointer(PolygonMapPtr::create(FileName(), String("PolygonMapAttribute(%S,%s)", fn.sFullNameQuoted(), p+1)));
		else {
			fn.sFile = "";
			fn.sExt = "";
			SetPointer(PolygonMapPtr::create(fn, sExpression));
		}  
	}    
	else  
		SetPointer(PolygonMapPtr::create(fn));
}

PolygonMap::PolygonMap(const FileName& fn, const CoordSystem& cs,
					   const CoordBounds& cb, const DomainValueRangeStruct& dvs)
{
	FileName fnPol = fn;
	fnPol.sExt = ".mpa";
	MutexFileName mut(fnPol);
	SetPointer(new PolygonMapPtr(fnPol, cs, cb, dvs));
}


PolygonMap::PolygonMap(const PolygonMap& pm)
: BaseMap(pm.pointer())
{
}

void PolygonMapPtr::BreakDependency()
{
	//  OpenPolygonMapVirtual();
	if (!fCalculated())
		Calc();
	if (!fCalculated())
		return; 
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	delete pmv;
	pmv = 0;
	fChanged = true;
	//  _fDataReadOnly = false;
	WriteElement("PolygonMap", "Type", "PolygonMapStore");
	Store();
}

void PolygonMap::Export(const FileName& fn) const
{
	if (!ptr())
		return;
	ptr()->Export(fn);
}

PolygonMapPtr::PolygonMapPtr(const FileName& fn, const CoordSystem& cs, 
							 const CoordBounds& cb, const DomainValueRangeStruct& dvs)
							 : BaseMapPtr(fn,cs,cb,dvs), pms(0), pmv(0), fTopoMap(true)
{
	_iPol = _iPolDeleted = 0;
	setVersionBinary(ILWIS::Version::bvPOLYGONFORMAT37);
	pms = new PolygonMapStoreFormat37(fn, *this,true);
}

PolygonMapPtr::PolygonMapPtr(const FileName& fnSegmentMap, LayerInfo inf) :
BaseMapPtr(),
pmv(0),
pms(0),
fTopoMap(true)
{
	_iPolDeleted = 0;
	SetCoordSystem(inf.csy);
	SetDomainValueRangeStruct(inf.dvrsMap);
	Table tbl;
	tbl= inf.tblattr;
	SetAttributeTable(tbl);
	SetFileName(inf.fnObj);
	cbOuter = inf.cbMap;
	setVersionBinary(ILWIS::Version::bvFORMATFOREIGN);
	pms = new PolygonMapStoreForeign(fnSegmentMap, *this, inf);

	_iPol = inf.iShapes;
}

PolygonMapPtr::PolygonMapPtr(const FileName& fn, bool fCreate)
: BaseMapPtr(fn, fCreate), pms(0), pmv(0), fTopoMap(true)
{
	_iPol = _iPolDeleted = 0;
	String s;
	DoNotUpdate();
	DoNotStore(true);   
	setVersionBinary(ILWIS::Version::bvPOLYGONFORMAT37);


	if (0 != ReadElement("PolygonMapStore", (char*)0, s))
	{
		ILWIS::Version::BinaryVersion oldFormat = ILWIS::Version::bvUNKNOWN;
		ReadElement("PolygonMapStore", "Format", (int &)oldFormat);
		if ( oldFormat == shUNDEF) // old format
			ReadElement("SegmentMapStore", "Format", (int &)oldFormat);
		if ( fCreate ) oldFormat = ILWIS::Version::bvPOLYGONFORMAT37;
		getEngine()->getVersion()->fSupportsBinaryVersion(oldFormat);
		if ( oldFormat == ILWIS::Version::bvFORMAT30)
			pms = new PolygonMapStoreFormat30(fn, *this); 
		else if ( oldFormat == ILWIS::Version::bvPOLYGONFORMAT37)
			pms	= pms = new PolygonMapStoreFormat37(fn, *this, fCreate);
		else if ( oldFormat == ILWIS::Version::bvFORMAT20)
			pms = new PolygonMapStoreFormat20(fn, *this); 
		else if ( oldFormat == ILWIS::Version::bvFORMATFOREIGN )
		{
			setVersionBinary(ILWIS::Version::bvFORMATFOREIGN);
			ParmList pm;
			ForeignFormat *ff = ForeignFormat::Create(fn, pm);
			if ( ff )
			{
				LayerInfo inf = ff->GetLayerInfo(this, ForeignFormat::mtPolygonMap, false);
				pms = new PolygonMapStoreForeign(fn, *this, inf);
				delete ff;
			}
			else
				throw ErrorObject(String(TR("Could not open %S").c_str(), fn.sRelative()));
		}else 
			pms = new PolygonMapStoreFormat20(fn, *this);
	}

	if ( ReadElement("PolygonMap", "Topological", fTopoMap) == 0)
		fTopoMap = true;

	DoNotStore(false);
	DoNotUpdate();

}

void PolygonMapPtr::Load()
{
	_iPol = _iPolDeleted = 0;
	String s;
	setVersionBinary(ILWIS::Version::bvPOLYGONFORMAT37);
	if (0 != ReadElement("PolygonMapStore", (char*)0, s))
	{
		ILWIS::Version::BinaryVersion oldFormat = ILWIS::Version::bvUNKNOWN;
		ReadElement("SegmentMapStore", "Format", (int &)oldFormat);
		if ( oldFormat == ILWIS::Version::bvFORMAT30)
			pms = new PolygonMapStoreFormat30(fnObj, *this); 
		else if ( oldFormat == ILWIS::Version::bvPOLYGONFORMAT37)
			pms = new PolygonMapStoreFormat37(fnObj, *this);
		else if ( oldFormat == ILWIS::Version::bvFORMAT20)
			pms = new PolygonMapStoreFormat20(fnObj, *this); 
		else if ( oldFormat == ILWIS::Version::bvFORMATFOREIGN )
		{
			ParmList pm;
			setVersionBinary(ILWIS::Version::bvFORMATFOREIGN);
			ForeignFormat *ff = ForeignFormat::Create(fnObj, pm);
			LayerInfo inf = ff->GetLayerInfo(this, ForeignFormat::mtPolygonMap, false);
			pms = new PolygonMapStoreForeign(fnObj, *this, inf);
		}
	}
	if ( ReadElement("PolygonMap", "Topological", fTopoMap) == 0)
		fTopoMap = true;	
}

PolygonMapPtr* PolygonMapPtr::create(const FileName& fn)
{
	if (!File::fExist(fn))
		NotFoundError(fn);
	MutexFileName mut(fn);
	PolygonMapPtr* p = BaseMap::pGetPolMap(fn);
	if (p) // if already open return it
		return p;
	p = new PolygonMapPtr(fn);
	return p;
}

PolygonMapPtr* PolygonMapPtr::create(const FileName& fn,
									 const String& sExpression)
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
		if ((p == 0) || (0 == _strcmpi(p, ".mpa"))) { // no attrib map    
			FileName fnMap(sExpression, ".mpa", true);
			if (File::fExist(fnMap)) {
				MutexFileName mut(fnMap);
				// see if map is already open (if it is in the list of open base maps)
				PolygonMapPtr* p = BaseMap::pGetPolMap(fnMap);
				if (p) // if already open return it
					return p;
				// open map and return it
				return PolygonMapPtr::create(fnMap);
			}
		}  
	}
	bool fPrevExist = File::fExist(fn);
	PolygonMapPtr* p = new PolygonMapPtr(fn, true);
	p->pmv = PolygonMapVirtual::create(fn, *p, sExpression);
	if ((0 != p->pmv) && fPrevExist) {
		PolygonMapStore::UnStore(fn);
		//    unlink(fn.sFullName(true)); // delete previous object def file
		ObjectInfo::WriteElement("ObjectDependency", (char*)0, fn, (char*)0);
		ObjectInfo::WriteElement("PolygonMapVirtual", (char*)0, fn, (char*)0);
		TableHistogramPol::Erase(fn); // delete previous histogram
	}  
	if (0 != p->pmv) {
		p->SetDomainChangeable(p->pmv->fDomainChangeable());
		p->SetValueRangeChangeable(p->pmv->fValueRangeChangeable());
		p->SetExpressionChangeable(p->pmv->fExpressionChangeable());
	}
	return p;
}

PolygonMapPtr::~PolygonMapPtr()
{
	if (fErase) {
		FileName fnHis = FileName(fnObj, ".hsa");
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
	if (0 != pmv) {
		delete pmv;
		pmv = 0;
	}  
}

void PolygonMapPtr::Store()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	bool fDep = fDependent();
	if ((0 != pmv) && (sDescription == ""))
		sDescription = pmv->sExpression();
	BaseMapPtr::Store();
	WriteElement("BaseMap", "Type", "PolygonMap");
	WriteElement("ILWIS", "Version", "3.7");
	if (0 != pms)
		pms->Store();
	else
		PolygonMapStore::UnStore(fnObj);
	if (fDep)
		WriteElement("PolygonMap", "Type", "PolygonMapVirtual");
	if (0 != pmv)
		pmv->Store();
	String s;
	if (0 == ReadElement("PolygonMap", "Type", s))
		WriteElement("PolygonMap", "Type", "PolygonMapStore");
	WriteElement("PolygonMap", "Topological", fTopoMap);
}

void PolygonMapPtr::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvsNew)
{
	BaseMapPtr::SetDomainValueRangeStruct(dvsNew);

}

void PolygonMapPtr::SetValueRange(const ValueRange& vrNew)
{
	BaseMapPtr::SetValueRange(vrNew);
}

vector<String> PolygonMapPtr::vsValue(const Coord& crd, short iWidth, short iDec, double rPrx) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	vector<ILWIS::Polygon *> v = pol(crd); // ignore proximity
	vector<String> values;
	for(int i = 0; i < v.size(); ++i) {
		ILWIS::Polygon *p = v[i];
		if (!p->fValid())
			values.push_back(sUNDEF);
		else if ( dvrs().fValues())
			values.push_back(dvrs().sValue(p->rValue(), iWidth, iDec));
		else if ( dvrs().fRawAvailable())
			values.push_back(dvrs().sValueByRaw(p->iValue(), iWidth, iDec));
		else
			values.push_back(dvrs().sValue(p->iValue(), iWidth, iDec));
	}
	return values;
}

vector<long> PolygonMapPtr::viRaw(const Coord& crd, double rPrx) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	vector<ILWIS::Polygon *> v = pol(crd); // ignore proximity
	vector<long> values;
	for(int i = 0; i < v.size(); ++i) {
		ILWIS::Polygon *p = v[i];
		if (!p->fValid())
			values.push_back(iUNDEF);
		else
			values.push_back(p->iValue());
	}
	return values;
}

vector<long> PolygonMapPtr::viValue(const Coord& crd, double rPrx) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	vector<ILWIS::Polygon *> v = pol(crd); // ignore proximity
	vector<long> values;
	for(int i = 0; i < v.size(); ++i) {
		ILWIS::Polygon *p = v[i];
		if (!p->fValid())
			values.push_back(iUNDEF);
		else
			values.push_back(dvrs().iValue(p->iValue()));
	}
	return values;
}

vector<double> PolygonMapPtr::vrValue(const Coord& crd, double rPrx) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	vector<ILWIS::Polygon *> v = pol(crd); // ignore proximity
	vector<double> values;
	for(int i = 0; i < v.size(); ++i) {
		ILWIS::Polygon *p = v[i];
		if (!p->fValid())
			values.push_back(iUNDEF);
		else
			values.push_back(dvrs().iValue(p->rValue()));
	}
	return values;
}

vector<ILWIS::Polygon *> PolygonMapPtr::pol(const Coord& crd) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pms)
		return pms->pol(crd);
	return vector<ILWIS::Polygon *>();
}

Geometry *PolygonMapPtr::getFeatureById(const String& id) const {
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if ( pms) {
		pms->getFeatureById(id);
	}
	return NULL;
}

Geometry *PolygonMapPtr::getFeature(const String& s) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	ILWIS::Polygon *pol;
	int index = 0;
	while( (pol = (*pms)[index++]) != NULL)
		if (pol->sValue(dvrs()) == s)
			return pol;
	return NULL;
}

long PolygonMapPtr::iFeatures() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	return _iPol;
}

long PolygonMapPtr::iPolDeleted() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	return _iPolDeleted;
}

Geometry *PolygonMapPtr::getFeature(long id) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pms)
		return pms->pol(id);
	return NULL;
}

Geometry *PolygonMapPtr::getTransformedFeature(long iRec, const CoordSystem& csy) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pms)
		return pms->getTransformedFeature(iRec, csy);
	return NULL;
}


ILWIS::Polygon *PolygonMapPtr::polFirst() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pms)
		return pms->polFirst();
	return NULL;
}

ILWIS::Polygon *PolygonMapPtr::polLast() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pms)
		return pms->polLast();
	return NULL;
}

void PolygonMapPtr::CalcMinMax()
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
	FileName fnHis(fnObj, ".hsa", true);
	if (File::fExist(fnHis))
		his = Table(fnHis);
	else
		his = Table(fnHis, String("TableHistogramPol(%S)", sNameQuoted()));
	his->Calc(); // sets 0 and 1 perc interval
	/*
	PolygonMap mp;
	mp.SetPointer(this);
	TableHistogramPol his(mp);
	if (!fCalculated())
	Calc();
	if (dvs.fValues()) {
	if (dvs.fRealValues()) {
	_rrMinMax = his.rrMinMax(0);
	_rrPerc1 = his.rrMinMax(1);
	_riMinMax = RangeInt(longConv(_rrMinMax.rLo()),
	longConv(_rrMinMax.rHi()));
	_riPerc1 = RangeInt(longConv(_rrPerc1.rLo()),
	longConv(_rrPerc1.rHi()));
	}
	else {
	_riMinMax = his.riMinMax(0);
	_riPerc1 = his.riMinMax(1);
	_rrMinMax = RangeReal(_riMinMax.iLo(),_riMinMax.iHi());
	_rrPerc1 = RangeReal(_riPerc1.iLo(),_riPerc1.iHi());
	}
	}
	Store();
	his.Store();
	fChanged = true;*/
}

String PolygonMapPtr::sType() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pmv)
		return pmv->sType();
	else if (fDependent())    
		return "Dependent ILWIS::Polygon Map";
	else
		return "ILWIS::Polygon Map";
}

void PolygonMapPtr::Rename(const FileName& fnNew)
{
}

void PolygonMapPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
	IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pms)
		pms->GetDataFiles(afnDat, asSection, asEntry);
}

void PolygonMapPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
	ObjectDependency::Read(fnObj, afnObjDep);
}

void PolygonMapPtr::KeepOpen(bool f)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	BaseMapPtr::KeepOpen(f);
	if (0 != pms)
		pms->KeepOpen(f);
}

void PolygonMapPtr::Export(const FileName& fn) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pmv)
		pmv->Freeze();
	if (0 != pms)
		pms->Export(fn);
}

bool PolygonMapPtr::fConvertTo(const DomainValueRangeStruct& dvs, const Column& col)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (col.fValid() && col->dvrs() == dvs)
		return false;
	else if (dvrs() == dvs)
		return false;
	bool f = false;
	if (col.fValid())
		SetDomainValueRangeStruct(col->dvrs());
	else
		SetDomainValueRangeStruct(dvs);
	return f;
}

bool PolygonMapPtr::fDependent() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pmv)
		return true;
	String s;
	ReadElement("PolygonMap", "Type", s);
	return fCIStrEqual(s , "PolygonMapVirtual");
}

String PolygonMapPtr::sExpression() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pmv)
		return pmv->sExpression();
	String s;
	s = IlwisObjectPtr::sExpression();
	if (s.length() == 0) // for downward compatibility with 2.02
		ReadElement("PolygonMapVirtual", "Expression", s);
	return s;
}

bool PolygonMapPtr::fCalculated() const
// returns true if a calculated result exists
{
	if (!fDependent())
		return IlwisObjectPtr::fCalculated();
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csCalc), TRUE);
	return 0 != pms;
} 
/*
bool PolygonMapPtr::fUpToDate() const
// returns true if an up to date calculated result exists
{
if (!fDependent())
return IlwisObjectPtr::fUpToDate();
if (0 == pms)
return false;
OpenPolygonMapVirtual();
if (0 == pmv)
return false;
if (!objdep.fUpToDate())
return false;
Time timDep = objdep.tmNewest();
return timDep <= pms->timStore;
}
*/
bool PolygonMapPtr::fDefOnlyPossible() const
// returns true if data can be retrieved without complete calculation (on the fly)
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (!fDependent())
		return IlwisObjectPtr::fDefOnlyPossible();
	bool f;
	if (0 != ReadElement("PolygonMapVirtual", "DefOnlyPossible", f))
		return f;
	return false;
}

void PolygonMapPtr::Calc(bool fMakeUpToDate)
// calculates the result     
{
	ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
	OpenPolygonMapVirtual();
	if (fMakeUpToDate)
		if (!objdep.fUpdateAll())
			return;
	if (0 != pmv) 
		pmv->Freeze();
}

void PolygonMapPtr::DeleteCalc()
// deletes calculated  result     
{
	TableHistogramPol::Erase(fnObj);
	if (0 == pms)
		return;
	ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
	OpenPolygonMapVirtual();
	if (0 != pmv) 
		pmv->UnFreeze();
}

void PolygonMapPtr::OpenPolygonMapVirtual()
{
	if (0 != pmv) // already there
		return;
	if (!fDependent())
		return;
	try {
		pmv = PolygonMapVirtual::create(fnObj, *this);
		objdep = ObjectDependency(fnObj);
		String s;
		if (0 == ReadElement("IlwisObjectVirtual", (char*)0, s)) {
			// for downward compatibility with 2.02 :
			SetDomainChangeable(pmv->fDomainChangeable());
			SetValueRangeChangeable(pmv->fValueRangeChangeable());
			SetExpressionChangeable(pmv->fExpressionChangeable());
		}
	}
	catch (const ErrorObject& err) {
		err.Show();
		pmv = 0;
		objdep = ObjectDependency();
	}
}

void PolygonMapPtr::CreatePolygonMapStore()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != pmv)
		pmv->CreatePolygonMapStore();
}

void PolygonMapPtr::UndoAllChanges()
{
	delete pms;
	Load();
}

bool PolygonMapPtr::fTopologicalMap()
{
	return fTopoMap;
}

void PolygonMapPtr::TopologicalMap(bool fYes)
{
	fTopoMap = fYes;
}

void PolygonMapPtr::GetObjectStructure(ObjectStructure& os)
{
	BaseMapPtr::GetObjectStructure(os);
	double  rVersion = rReadElement("Ilwis", "Version");

	if ( rVersion < 3.0)					   
	{
		os.AddFile(fnObj, "SegmentMapStore", "DataSeg");				
		os.AddFile(fnObj, "SegmentMapStore", "DataCrd");								
		os.AddFile(fnObj, "PolygonMapStore", "DataPol");										
		os.AddFile(fnObj, "PolygonMapStore", "DataPolCode");												
		os.AddFile(fnObj, "PolygonMapStore", "DataTop");														
	}
	else if ( rVersion < 3.7)
	{
		os.AddFile(fnObj, "top:TableStore", "Data");
		os.AddFile(fnObj, "TableStore", "Data");	
	} else {
		if (!fUseAs() || (os.caGetCommandAction() == ObjectStructure::caDELETE))
			os.AddFile(fnObj, "PolygonMapStore", "DataPol");
	}
	FileName fnHist(fnObj, ".hsa");
	if (File::fExist(fnHist) && IlwisObject::iotObjectType( fnHist ) != IlwisObject::iotANY)
	{
		IlwisObject obj = IlwisObject::obj(fnHist);
		if ( obj.fValid())
			obj->GetObjectStructure(os);
	}					
}

void PolygonMapPtr::DoNotUpdate()
{
	BaseMapPtr::DoNotUpdate();

}

IlwisObjectVirtual *PolygonMapPtr::pGetVirtualObject() const
{
	return pmv;
}

Feature* PolygonMapPtr::newFeature(geos::geom::Geometry *p)
{
	if (0 != pms)
		return pms->newFeature(p);
	return NULL;
}

void PolygonMapPtr::addPolygon(ILWIS::Polygon *p)
{
	if (0 != pms)
		pms->addPolygon(p);
}

vector<Geometry *> PolygonMapPtr::getFeatures(Coord crd, double rPrx) {
	if (! crd.fUndef()) {

		ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
		if (0 != pms) {
			vector<ILWIS::Polygon *> pols = pms->pol(crd);
			vector<Geometry *> geoms;
			for(int i = 0;  i < pols.size(); ++i) {
				geoms.push_back(pols.at(i));
			}
			return geoms;
		}
	}
	return vector<Geometry *>();
}


bool PolygonMapPtr::removeFeature(const String& id, const vector<int>& selectedCoords) {
	if ( 0 != pms)
		return pms->removeFeature(id, selectedCoords);
	return false;
}

vector<Feature *> PolygonMapPtr::getFeatures(const CoordBounds& cb, bool complete) const {
	if ( pms) {
		return pms->getFeatures(cb, complete);
	}
	return vector<Feature *>();

}