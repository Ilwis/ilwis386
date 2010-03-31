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
/* MapKrigingUniversal
   april 1998, Jan Hendrikse
   Copyright Ilwis System Development ITC
	Last change:  JH    3 Jul 99    4:40 pm
*/
#include "Headers\toolspch.h"
#include "Applications\Raster\Mapkrig.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"
#define EPS10 1.0e-10

//IlwisObjectPtr * createMapKrigingSimple(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
//	if ( sExpr != "")
//		return (IlwisObjectPtr *)MapKrigingSimple::create(fn, (MapPtr &)ptr, sExpr);
//	else
//		return (IlwisObjectPtr *)new MapKrigingSimple(fn, (MapPtr &)ptr);
//}
//
//IlwisObjectPtr * createMapKrigingOrdinary(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
//	if ( sExpr != "")
//		return (IlwisObjectPtr *)MapKrigingOrdinary::create(fn, (MapPtr &)ptr, sExpr);
//	else
//		return (IlwisObjectPtr *)new MapKrigingOrdinary(fn, (MapPtr &)ptr);
//}
//
//IlwisObjectPtr * createMapKrigingUniversal(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
//	if ( sExpr != "")
//		return (IlwisObjectPtr *)MapKrigingUniversal::create(fn, (MapPtr &)ptr, sExpr);
//	else
//		return (IlwisObjectPtr *)new MapKrigingUniversal(fn, (MapPtr &)ptr);
//}

IlwisObjectPtr * createMapKriging(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapKriging::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapKriging(fn, (MapPtr &)ptr);
}

const char* MapKriging::sSyntax() {
	return	"MapKrigingSimple(pntmap,georef,semivariogram[,plane|sphere][,"
				"errormap[,no|average|firstval[,tolerance]]])\n"
			"MapKrigingOrdinary(pntmap,georef,semivariogram,limdist[,plane|sphere][,"
				"errormap[,minNrpts[,maxNrpts[,no|average|firstval[,tolerance]]]]])\n"
			"MapKrigingAnisotropic(pntmap,georef,semivariogram,limdist[,angle,ratio[,"
				"errormap[,minNrpts[,maxNrpts[,no|average|firstval[,tolerance]]]]]])\n"
			"MapKrigingUniversal(pntmap,georef,semivariogram,limdist,degree[,angle,ratio[,"
				"errormap[,minNrpts[,maxNrpts[,no|average|firstval[,tolerance]]]]]])";
}

static void SameNameErrMapError(const FileName& fn)
{
  throw ErrorObject(WhatError(String(SMAPErrNameErrMapAlreadyUsed), errMapKriging), fn);
}
static void PointMapEmpty(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrPointMapEmpty), errMapKriging +3), fn);
}
static void TooManyKrigingSPoints(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrTooManyKSPoints_S.scVal(), String("Simple Kriging")), errMapKriging +4), fn);
}
static void TooManyKrigingOPoints(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrTooManyKOPoints_S.scVal(), String("Kriging")), errMapKriging +5), fn);
}
static void MinNrKrigingPnts(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrMinNrKrigingPnts), errMapKriging +6), fn);
}
static void MaxNrKrigingPnts(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrMaxNrKrigingPnts), errMapKriging +7), fn);
}
static void LimitingDistanceOutsideRange(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrLimitingDistance), errMapKriging +8), fn);
}
static void TrendDegree_012(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrTrendDegree_012), errMapKriging +9), fn);
}
static void AzimFrom_360to360(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrAzimFrom_360to360), errMapKriging +10), fn);
}
static void RatioFrom1To10(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrRatioFrom1To10), errMapKriging +11), fn);
}
static void MinNrPnts3orMore(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrMinNrPnts3orMore), errMapKriging +12), fn);
}
static void MinNrPnts6orMore(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrMinNrPnts6orMore), errMapKriging +13), fn);
}
static void TolerancePos(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrTolerancePos), errMapKriging +14), fn);
}
static void NrParamSimpleKr(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrNrParamSimpleKr), errMapKriging +15), fn);
}
static void NrParamOrdinaKr(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrNrParamOrdinaKr), errMapKriging +16), fn);
}
static void NrParamAnisotKr(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrNrParamAnisotKr), errMapKriging +17), fn);
}
static void NrParamUniverKr(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrNrParamUniverKr), errMapKriging +18), fn);
}
static void IllegalNameErrMap(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrIllegalNameErrMap), errMapKriging + 19), fn);
}

bool MapKriging::fValueRangeChangeable() const
{
  return true;
}

bool MapKriging::fDomainChangeable() const
{
  return true;
}

ValueRange MapKriging::vrDefault(const PointMap& pmap)
{
	RangeReal rr = pmap->rrMinMax(false);
	if (!rr.fValid())
		rr = pmap->dvrs().rrMinMax();
	double rRangeWidth = abs(rr.rHi() - rr.rLo());
	double rLowestOut = rr.rLo() - rRangeWidth;
	double rHighestOut = rr.rHi() + rRangeWidth;
	double rStep = pmap->dvrs().rStep()/10.0;
	return ValueRange(rLowestOut, rHighestOut, rStep);
}

class MapKrigingCreate
{
	// constructor parameters
	FileName fn;
	MapPtr& p;
	String sExpr;

	// Fields (MapKriging constructor parameters)
	PointMap pm;
	double rLimDst;
	GeoRef gr;
	SemiVariogram svm;
	MapKriging::KrigingMethod km;
	long iDegr;
	FileName fnErrMp;
	long iMin;
	long iMax;
	MapKriging::DuplicRemoveMethod iAvg;
	double rTol;
	double rAzim;
	double rRati;
	bool fSphericDist;

	// helper variables
	long iParms;          // total #parameters
	long iCurrentParm; // parameter counter
	Array<String> as;  // parameter array

	void ParseKrigingSimple();
	void ParseKrigingOrdinary();
	void ParseKrigingUniversal();
	void ParseKrigingAnisotropic();

	void CheckSpherical();
	void MakeErrorMapName();
	void GetDuplicatePointsMethod();
	void GetTolerance();
	void GetLimitingDistance();
	void GetMinimumPoints();
	void GetMaximumPoints();
	void GetDegree();
	void GetAzimuth();
	void GetRatio();
	
public:
	MapKrigingCreate(const FileName& fn, MapPtr& p, const String& sExpr);
	MapKriging* create();
};

MapKrigingCreate::MapKrigingCreate(const FileName& _fn, MapPtr& _p, const String& _sExpr)
		: fn(_fn), p(_p), sExpr(_sExpr)
{
	// Setup defaults
	km = MapKriging::kmSIMPLE;
	rLimDst = 1.0;
	iDegr = 0L;  // default trend
	iMin = 2L;   // default sample size limits
	iMax = 16L;
	iAvg = MapKriging::drmAVERAGE;
	rTol = 0.1;
	rAzim = 0.0;
	rRati = 1.0;
	fSphericDist = false; //default and only usable for kmSIMPLE and kmORDINARY
	
	iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms > 12 || iParms < 3)
       	ExpressionError(sExpr, MapKriging::sSyntax());
	
	pm= PointMap(as[0], fn.sPath());
	long iNrP = pm->iFeatures();
	if (iNrP < 2) 
		PointMapEmpty(fn);
	
	gr = GeoRef(as[1]);
	svm = SemiVariogram(as[2]);
}

void MapKrigingCreate::CheckSpherical()
{
	if (iParms <= iCurrentParm)
		return;
	
	if (fCIStrEqual("sphere", as[iCurrentParm]))
		fSphericDist = true;
	else if (fCIStrEqual("plane", as[iCurrentParm]))
		fSphericDist = false;
	else
		return;
	
	++iCurrentParm;
}

void MapKrigingCreate::MakeErrorMapName()
{
	if (iParms <= iCurrentParm)
		return;
	
	String sErrMp = as[iCurrentParm];
	if (sErrMp.length() > 0 && sErrMp != "0")
	{
		fnErrMp = fn;                  // ErrorMap name is made using KrigMap name
		fnErrMp.sFile &= "_Error";
		fnErrMp.sExt = ".mpr";
		if (fnErrMp.fExist())
			FileAlreadyExistError(fnErrMp);
		if (!fnErrMp.fValid())  
			IllegalNameErrMap(fnErrMp);
	}  
	++iCurrentParm;
}

void MapKrigingCreate::GetDuplicatePointsMethod()
{
	if (iParms <= iCurrentParm)
		return;
	
	if (fCIStrEqual("no", as[iCurrentParm]))
		iAvg = MapKriging::drmNO;
	else if (fCIStrEqual("average", as[iCurrentParm]))
		iAvg = MapKriging::drmAVERAGE;
	else if (fCIStrEqual("firstval", as[iCurrentParm]))
		iAvg = MapKriging::drmFIRSTVAL;
	else
		ExpressionError(sExpr, MapKriging::sSyntax());
	
	++iCurrentParm;
}	 

void MapKrigingCreate::GetTolerance()
{
	if (iParms <= iCurrentParm)
		return;
	
	rTol = as[iCurrentParm].rVal();
	if (rTol <= 0)	
		TolerancePos(fn);
	
	++iCurrentParm;
}

void MapKrigingCreate::GetLimitingDistance()
{
	if (iParms <= iCurrentParm)
		return;

	Coord crdMin, crdMax;
	pm->Bounds(crdMin, crdMax);
	double rMaxDiff = rDist(crdMin, crdMax);
	
	if (0 != pm->cs()->pcsLatLon()) //for search radius limitation
		rMaxDiff *= 111111.1; //make degrees quasi-metric (1 degr)
		
	rLimDst = as[iCurrentParm].rVal();
	if (rLimDst <= 0 || rLimDst > 2 * rMaxDiff)  
		LimitingDistanceOutsideRange(fn);
	
	++iCurrentParm;
}

void MapKrigingCreate::GetMinimumPoints()
{
	if (iParms <= iCurrentParm)
		return;
	
	iMin = as[iCurrentParm].iVal();
	if (iMin < 1)  
		MinNrKrigingPnts(fn);
	if (iMin < 3 && iDegr == 1) 
		MinNrPnts3orMore(fn);
	if (iMin < 6 && iDegr == 2) 
		MinNrPnts6orMore(fn);
	
	++iCurrentParm;
}

void MapKrigingCreate::GetMaximumPoints()
{
	if (iParms <= iCurrentParm)
		return;
	
	iMax = (long)as[iCurrentParm].rVal();
	if (iMax < iMin)
		MaxNrKrigingPnts(fn);
	
	++iCurrentParm;
}

void MapKrigingCreate::GetAzimuth()
{
	if (iParms <= iCurrentParm)
		return;
	
	if (as[iCurrentParm] == "")
		rAzim = 0;
	else  
		rAzim = as[iCurrentParm].rVal() * M_PI / 180.0;
	if (abs(rAzim) > 2 * M_PI)
		AzimFrom_360to360(fn);

	++iCurrentParm;
}

void MapKrigingCreate::GetRatio()
{
	if (iParms <= iCurrentParm)
		return;
	
	rRati = as[iCurrentParm].rVal();
	if (rRati < 0.99 || rRati > 10)
		RatioFrom1To10(fn);
	
	++iCurrentParm;
}

void MapKrigingCreate::GetDegree()
{
	if (iParms <= iCurrentParm)
		return;
	
	iDegr = as[iCurrentParm].iVal();
	if (iDegr < 0L || iDegr > 2L)
		TrendDegree_012(fn);
	else  // default iMin depends on degree
		iMin = (long)( 1 + 3 * iDegr/2.0 + iDegr*iDegr/2.0);
	
	++iCurrentParm;
}

void MapKrigingCreate::ParseKrigingSimple()
{
	km = MapKriging::kmSIMPLE;
	rLimDst = 0;   // overrule default
	
	if (iParms > 7)
		NrParamSimpleKr(fn);  

	CheckSpherical();

	MakeErrorMapName();

	GetDuplicatePointsMethod();

	GetTolerance();
}

void MapKrigingCreate::ParseKrigingOrdinary()
{
	km = MapKriging::kmORDINARY;
	if (iParms < 4 || iParms > 10) 
		NrParamOrdinaKr(fn);  

	GetLimitingDistance();	

	CheckSpherical();

	MakeErrorMapName();

	GetMinimumPoints();

	GetMaximumPoints();

	GetDuplicatePointsMethod();
	
	GetTolerance();
}

void MapKrigingCreate::ParseKrigingUniversal()
{
	km = MapKriging::kmUNIVERSAL;
	if (iParms < 5 || iParms > 12)	
		NrParamUniverKr(fn); 

	GetLimitingDistance();

	GetDegree();
	
	GetAzimuth();

	GetRatio();	

	MakeErrorMapName();

	GetMinimumPoints();

	GetMaximumPoints();

	GetDuplicatePointsMethod();

	GetTolerance();
}
	
void MapKrigingCreate::ParseKrigingAnisotropic()
{
	km = MapKriging::kmANISOTROPIC;
	if (iParms < 5 || iParms > 11) 
		NrParamAnisotKr(fn); 

	GetLimitingDistance();

	GetAzimuth();	

	GetRatio();
	
	MakeErrorMapName();

	GetMinimumPoints();

	GetMaximumPoints();

	GetDuplicatePointsMethod();

	GetTolerance();
}
	
MapKriging* MapKrigingCreate::create()
{
	String sKriging = IlwisObjectPtr::sParseFunc(sExpr);
	iCurrentParm = 3; // 1st three parms already treated
	
	if (fCIStrEqual(sKriging, "MapKrigingSimple"))
		ParseKrigingSimple();
	else if (fCIStrEqual(sKriging, "MapKrigingOrdinary"))
		ParseKrigingOrdinary();
	else if (fCIStrEqual(sKriging,"MapKrigingAnisotropic") )
		ParseKrigingAnisotropic();
	else if (fCIStrEqual(sKriging,"MapKrigingUniversal") )
		ParseKrigingUniversal();
	else
		ExpressionError(sExpr, MapKriging::sSyntax());
	
	return new MapKriging(fn, p, pm, gr, svm, rLimDst, iDegr,
		rAzim, rRati, fnErrMp, iMin, iMax, 
		iAvg, rTol, km, fSphericDist);
}

MapKriging* MapKriging::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	return MapKrigingCreate(fn, p, sExpr).create();
}

MapKriging::MapKriging(const FileName& fn, MapPtr& ptr)
: MapFromPointMap(fn, ptr)
{
  svm = SemiVariogram(fnObj, "MapKriging");
  double rAz;
  krigMeth = kmORDINARY;
  String sKrigingMethod;
  ReadElement("MapKriging", "KrigingMethod",sKrigingMethod);
  if (fCIStrEqual("simple", sKrigingMethod))
    krigMeth = kmSIMPLE;
  else if (fCIStrEqual("ordinary", sKrigingMethod)) 
    krigMeth = kmORDINARY;
  else if (fCIStrEqual("universal", sKrigingMethod))
    krigMeth = kmUNIVERSAL;
	else if (fCIStrEqual("anisotropic", sKrigingMethod))
    krigMeth = kmANISOTROPIC;
  ReadElement("MapKriging", "LimitingDistance", rLimDist);
  if (rLimDist == 0) 
    krigMeth = kmSIMPLE;
  if (krigMeth == kmANISOTROPIC) 
	{
		iDegree = 0;
		ReadElement("MapKriging", "Azimuth", rAz);
		rAzimuth = rAz * M_PI / 180.0;
		ReadElement("MapKriging", "Ratio", rRatio);
	}
	else if (krigMeth == kmUNIVERSAL) 
	{
    ReadElement("MapKriging", "Degree", iDegree);
    ReadElement("MapKriging", "Azimuth", rAz);
    rAzimuth = rAz * M_PI / 180.0;
    ReadElement("MapKriging", "Ratio", rRatio);
  }
  else 
	{
    iDegree = 0;
    rAzimuth = 0;
    rRatio = 1;
  }
  ReadElement("MapKriging", "ErrorVarianceMap", ptr.fnErrorMap());
  ReadElement("MapKriging", "MinNrofPoints", iMinNrPoints);
  ReadElement("MapKriging", "MaxNrofPoints", iMaxNrPoints);
  String sDuplicRemoval;
  ReadElement("MapKriging", "DuplicRemoval",sDuplicRemoval);
  if (fCIStrEqual("no", sDuplicRemoval))
    duplicRemoval = drmNO;
  else if (fCIStrEqual("average", sDuplicRemoval))
    duplicRemoval = drmAVERAGE;
  else if (fCIStrEqual("firstval", sDuplicRemoval))
    duplicRemoval = drmFIRSTVAL;
  else  
    duplicRemoval = drmAVERAGE;
  ReadElement("MapKriging", "Tolerance", rTolerance);
  Init();

}  

MapKriging::MapKriging(const FileName& fn, MapPtr& ptr,
                       const PointMap& pmp, const GeoRef& gr,
                       const SemiVariogram& sv,
                       double rLimDst,
                       long iDegr,
                       double rAzim,
                       double rRati,
                       const FileName& fnErrM,
                       long iMin, long iMax,
                       DuplicRemoveMethod drm,
                       double rToler,
                       KrigingMethod km,
	                     const bool fSpheric)
: MapFromPointMap(fn, ptr, pmp, gr),
  svm(sv), rLimDist(rLimDst), iDegree(iDegr),
  rAzimuth(rAzim), rRatio(rRati),
  //fnErrorMap(fnErrM),
  iMinNrPoints(iMin), iMaxNrPoints(iMax),
  duplicRemoval(drm), rTolerance(rToler),
  krigMeth(km)
{
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, errMapKriging+1);
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapKriging+2);
  if (!cs()->fConvertFrom(pmp->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapKriging+19);
  // setting default range of output Kriging raster map (3 X wider than input range):
  DomainValueRangeStruct dvrs(vrDefault(pmp));
  SetDomainValueRangeStruct(dvrs);
  fNeedFreeze = true;
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.SetErrorMap(fnErrM);
	if (fSpheric)
		m_distMeth = Distance::distSPHERE;
	else
		m_distMeth = Distance::distPLANE;
  Init();
	Store(); // stores this Kriging map on disk before mpKrigingError (see next lines) looks for it 
	String sExpr("MapComputedElsewhere(%S)",ptr.sNameQuoted());
	if (ptr.fErrorMap()) {// open Error map
		Map mpComputedElsewhere(ptr.fnErrorMap(), sExpr);
		String sDescr(SMAPTextKrigingErrorMap_S.scVal(), ptr.sName());
    mpComputedElsewhere->sDescription = sDescr;
		mpComputedElsewhere->Store();
  }
}

MapKriging::~MapKriging()
{
}

void MapKriging::Store()
{
  MapFromPointMap::Store();
  String sKrigingMethod;
  switch (krigMeth) {
    case kmSIMPLE: sKrigingMethod = "simple"; break; 
    case kmORDINARY: sKrigingMethod = "ordinary"; break; 
		case kmANISOTROPIC: sKrigingMethod = "anisotropic"; break; 
		case kmUNIVERSAL: sKrigingMethod = "universal"; break; 
  } 
  WriteElement("MapKriging", "KrigingMethod", sKrigingMethod);
  WriteElement("MapFromPointMap", "Type", "MapKriging");
  //else if (rLimDist > 0) 
  //  WriteElement("MapFromPointMap", "Type", "MapKrigingOrdinary");
  //else  
  //  WriteElement("MapFromPointMap", "Type", "MapKrigingUniversal");
  svm.Store(fnObj, "MapKriging");
  WriteElement("MapKriging", "LimitingDistance", rLimDist);
  WriteElement("MapKriging", "Degree", iDegree);
  double rAz = rAzimuth * 180 / M_PI;
  WriteElement("MapKriging", "Azimuth", rAz);
  WriteElement("MapKriging", "Ratio", rRatio);

  if (ptr.fnErrorMap().sFile.length() != 0)
    WriteElement("MapKriging", "ErrorVarianceMap", ptr.fnErrorMap());
  WriteElement("MapKriging", "MinNrofPoints", iMinNrPoints);
  WriteElement("MapKriging", "MaxNrofPoints", iMaxNrPoints);
  String sDuplicRemoval;
  switch (duplicRemoval) {
    case drmNO: sDuplicRemoval = "no"; break;
    case drmAVERAGE: sDuplicRemoval = "average"; break;
    case drmFIRSTVAL: sDuplicRemoval = "firstval"; break;
  }
  WriteElement("MapKriging", "DuplicRemoval", sDuplicRemoval);
  WriteElement("MapKriging", "Tolerance", rTolerance);
  //WriteElement("MapKriging", "NrofValidPoints", iNrValidPnts);
}

void MapKriging::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "MapKriging";
  htpFreeze = htpMapKrigingT;
	if (krigMeth ==  kmANISOTROPIC)
    htpFreeze = htpMapAnisotropicKrigingT;
  if (krigMeth ==  kmUNIVERSAL)
    if (0 == iDegree)
      htpFreeze = htpMapAnisotropicKrigingT;
    else
      htpFreeze = htpMapUniversalKrigingT;
}

double MapKriging::rDist2Ellip(const Coord& crd1, const Coord& crd2, 
                   const double rAzimuth, const double rRatio)
{
  // distance generated by affine matrix A = DR using d2 = XtRtDRX
  // where D scaling matrix magnifying y coordinates with factor r
  // where R rotation matrix with angle Pi/2 - rAzimuth
  // AtA = RtDtDR =  //dilates distances along secondary axis
   // |c -s | | 1 0 | | 1 0 || c  s |  =  |(cc + ss*rr)  (cs-cs*rr)  | 
   // |s  c | | 0 r | | 0 r ||-s  c |     | (cs-cs*rr)   (ss + cc*rr)|
  // Xt = [x  y] is the distance vector whose length XtX is scaled
  double x = crd1.x - crd2.x;
  double y = crd1.y - crd2.y;
  if (rRatio < EPS10)
    return rUNDEF;   
  double s = sin(M_PI_2 - rAzimuth);
  double c = cos(M_PI_2 - rAzimuth);
  double cs = c*s;
  double cc = c*c;
  double ss = s*s;
  double rr = rRatio * rRatio;
  double xx = x*x;
  double yy = y*y;
  double xy = x*y;
  double d2 = (cc + ss*rr)*xx + 2*(cs-cs*rr)*xy + (ss + cc*rr)*yy;
  return d2;
}
/*
Coord MapKriging::crdEllipTransform(const Coord& crd, 
                                    const double rAngle,
                                    const double rRatio)
{ 
  if (rRatio < EPS10)
    return crdUNDEF;
  double s = sin(rAngle);
  double c = cos(rAngle);
  Coord cTransformed;
  cTransformed.x = (s * crd.x + c * crd.y) / rRatio;
  cTransformed.y = -c * crd.x + s * crd.y;
  return cTransformed;
}         */                    

bool MapKriging::fKrigingSimple(Array<Array<double> > & aaH2, Map & mpErreur)
{
  trq.SetText(SMAPTextCalculating);
  RealMatrix matKrige(iNrValidPnts + 1);
  int i,j;
  for (i= 0; i < iNrValidPnts; i++)
    for (j = i + 1; j < iNrValidPnts; j++) {
      double rCell = aaH2[i][j];  //svm.rCalc(sqrt(aaH2[i][j]));
      matKrige(i,j) = rCell;
      matKrige(j,i) = rCell;
    }
  for (j = 0; j < iNrValidPnts; j++) {
    matKrige(iNrValidPnts, j) = 1;
    matKrige(j,iNrValidPnts) = 1;
  }
  for (i= 0; i <= iNrValidPnts; i++)  
    matKrige(i, i) = 0;    
  //RealMatrix matKrigeInv = matKrige;
  ///bool fKrigSingular = matKrige.fSingular();   
  //if (!fKrigSingular) 
    matKrige.Invert();
  //else
    //throw ErrorObject(SPRJ, 2211);
  CVector vecD(iNrValidPnts + 1); // right-hand side of Kriging System
  RealBuf rBufOut(iMCol);
  RealBuf rBufOutError(iMCol); 
	Distance dis = Distance(pmp->cs(), m_distMeth);

  trq.SetText(SMAPTextSimpleKriging);
  for (long iRow = 0; iRow < iMRow; iRow++)  { // estimate ('predict') NonVisited pixels row by row
    if (trq.fUpdate(iRow, iMRow))
      return false;
    for (long iCol = 0; iCol < iMCol; iCol++)  { // estimate NonVisited pixels col by col
      if (trq.fAborted())
        return false;
      Coord  crdNonVis = gr()->cConv(RowCol(iRow, iCol)); // coord of NonVisited pix
      // look in point list
      for (i = 0; i < iNrValidPnts; i++)  {
        vecD(i) = svm.rCalc(dis.rDistance(cPoints[i],crdNonVis));
      }
      vecD(iNrValidPnts) = 1;
      CVector vecW(iNrValidPnts + 1); // solution vector
      for (i= 0; i <= iNrValidPnts; i++)
        vecW(i) = 
        (matKrige.GetRowVec(i) * vecD);//.GetColVec(0);// W = C_inv * D
      // iNrPoints weights and Lagr param

      double rV, rWeightedSum = 0; 
      for (i = 0; i < iNrValidPnts; i++) {
        rWeightedSum += vecW(i) * rAttrib[i];
      }  
      rV = rWeightedSum;
      double rVerror = 0;
      for (i = 0; i <= iNrValidPnts; i++) 
        rVerror += vecW(i) * vecD(i);
      rBufOut[iCol] = rV;
      rBufOutError[iCol] = sqrt(abs(rVerror));
    }
    pms->PutLineVal(iRow,rBufOut);
    if (mpErreur.fValid())
      mpErreur->PutLineVal(iRow,rBufOutError);
  } 
	if (mpErreur.fValid()) {
		mpErreur->Updated();
		mpErreur->Store();
	}
  return true;
}  // end KrigingSimple

struct saTwoPointArr {    // used to sort in Kriging Ordinary
  Array<int>* iClosestPnts;  
  Array<double>* rDist2FromPix;
//  Array<double>* rClosestPntsX;
//  Array<double>* rClosestPntsY;
}; 

static bool LessFunc(long i1, long i2, void* p)
{
   saTwoPointArr* saT = static_cast<saTwoPointArr*>(p);
   double rTemp=saT->rDist2FromPix->ind(i1);
   return rTemp < (saT->rDist2FromPix->ind(i2)); 
}

static void SwapFunc(long i1, long i2, void* p)
{
  saTwoPointArr* saT = static_cast<saTwoPointArr*>(p);
  Array<int> &aIntTemp=*(saT->iClosestPnts);
  int iTemp = saT->iClosestPnts->ind(i1);
  aIntTemp[i1] = saT->iClosestPnts->ind(i2);
  aIntTemp[i2] = iTemp;

  Array<double> &aDoubleTemp=*(saT->rDist2FromPix);
  double rTemp = saT->rDist2FromPix->ind(i1);
  aDoubleTemp[i1] = saT->rDist2FromPix->ind(i2);
  aDoubleTemp[i2] = rTemp;
}

bool MapKriging::fKrigingUniversal(Array<Array<double> > & aaH2, Map & mpErreur)
{ //  Kriging with anisotropy and trend (0|1|2) assumed
  trq.SetText(SMAPTextCalculating);
  int i,j;
  Array<int> iClosestPnts(iNrValidPnts);  // temp array of pntnrs of pnts within limiting circle
  Array<int> iClosestPntsBefore(iNrValidPnts); // idem within previous limiting circle
  Array<double> rLimAttrib(iNrValidPnts);  // their respective attrib values
  Array<double> rDist2FromPix(iNrValidPnts); // their resp distance from NonVisited pixel (RowCol)
	Array<double> rD2FromPix(iNrValidPnts); // idem but not sorted according to distance
  Array<double> rPtX(iNrValidPnts); // their resp X-coord for use in Universal Krig
  Array<double> rPtY(iNrValidPnts); // their resp Y-coord for use in Universal Krig
  Array<bool> fInsideLim(iNrValidPnts);
  Array<bool> fInsideLimBefore(iNrValidPnts);
  for (i = 0; i < iNrValidPnts; i++) 
    fInsideLimBefore[i]       = false;
  bool fLimPntSetChanged = false;
  double rLim2 = rLimDist * rLimDist;
	RealMatrix matKrige;
  RealMatrix matKrigeLast;
  long iNrPrevLimitedPnts = 0; 
  bool fKrigSingular;
  RealBuf rBufOut(iMCol);
  RealBuf rBufOutError(iMCol);
	Distance dis = Distance(pmp->cs(), m_distMeth);
  
  trq.SetText(SMAPTextOrdinaryKriging);  
  if (rRatio != 1)
    trq.SetText(SMAPTextAnisotropicKriging); 
  if (iDegree != 0)
    trq.SetText(SMAPTextUniversalKriging);
  
  for (long iRow = 0; iRow < iMRow; iRow++)  {  // estimate ('predict') NonVisited pixels row by row
    if (trq.fUpdate(iRow, iMRow))
      return false;
    for (long iCol = 0; iCol < iMCol; iCol++)  {  // estimate NonVisited pixels col by col
      if (trq.fAborted())
        return false;
      Coord  crdNonVis = gr()->cConv(RowCol(iRow, iCol));//coord of NonVisited pixel
      // put limiting rectangle around NonVisited point:
      double rLD = abs(rLimDist) * rRatio;
      Coord coLess=(Coord((crdNonVis.x-rLD),crdNonVis.y-rLD));
      Coord coMore=(Coord((crdNonVis.x+rLD),crdNonVis.y+rLD));
      long iNrLimitedPnts = 0;// counts points within limiting distance and at most iMaxNrPoints 
      
			// Check if points are within limiting distance and if so
      // add them to the 2 arrays of saTwoPointArr at position n:
      long n = 0; 
      long iInCircle = 0;//counts only points inside lim circle
      for (i = 0; i < iNrValidPnts; i++) {
        Coord crdPi = cPoints[i];
        
        if (!(crdPi.fInside(coLess,coMore))) continue;// outside box: skip
        double rvDist2;
        if (rRatio != 1)
          rvDist2 = rDist2Ellip(crdNonVis,crdPi,rAzimuth,rRatio);
        else
          rvDist2 = dis.rDistance2(crdNonVis,crdPi);
        if (rvDist2>rLim2) continue;  // outside ellips: skip
        iClosestPnts[iInCircle] = i;
        rDist2FromPix[iInCircle] = rvDist2;
				rD2FromPix[i] = rvDist2;  //this array will not be sorted but sieved
																	//and contains only distances inside circle
        iInCircle++;
         // if n == iMaxNrPoints maximum nr is reached
      }
      iNrLimitedPnts = iInCircle;
      if (iInCircle > iMaxNrPoints) {
        saTwoPointArr saT;
        saT.iClosestPnts = &iClosestPnts;  
        saT.rDist2FromPix = &rDist2FromPix;
        QuickSort(0, iInCircle-1, LessFunc, SwapFunc, &saT);
        iNrLimitedPnts = iMaxNrPoints;
      }
      for (i = 0; i < iNrValidPnts; i++) 
        fInsideLim[i] = false;
      for (n = 0; n < iNrLimitedPnts; n++)  
        fInsideLim[iClosestPnts[n]] = true;
        
      double rV = rUNDEF;
      double rVerror = rUNDEF;
      if (iNrLimitedPnts >= iMinNrPoints) 
			{
        fLimPntSetChanged = (iNrPrevLimitedPnts != iNrLimitedPnts); 
        n = 0; 
        while (!fLimPntSetChanged && n < iNrValidPnts) {
          fLimPntSetChanged =
            (fInsideLimBefore[iClosestPnts[n]]
                != fInsideLim[iClosestPnts[n]]); 
          n++;
        }  
        
				// copy limited-point-set flags for later comparison:
				for (n = 0; n < iNrValidPnts; n++) 
          fInsideLimBefore[n] = fInsideLim[n];
        iNrPrevLimitedPnts = iNrLimitedPnts; ;  
          
      // Take mutual semivars from matrix aaH2,
      // and semivars to NonVisited point in vector vecD
      // Both aaH2 and vecD have variable filling < iNrValidPnts
      // depending on contents of Limiting circle while 
			// NonVisited point moves to estimate each RowCol output
      // But updated filling of aaH2 only needed if fLimPntSetChanged
      // recalculation of Kriging matrix inverse is only done in that case

        long iNrOfTerms = 1;   // number of trend polynomial terms
        if (iDegree != 0)      // if trend assumed then
          iNrOfTerms = 3 * iDegree; // 3 terms if linear; 6 terms if quadratic
				
				CVector vecD(iNrLimitedPnts + iNrOfTerms); // right-hand side of Kriging System
				matKrige = RealMatrix(iNrLimitedPnts + iNrOfTerms);
				long k = 0;   // row index of Krige matrix and right-hand vecD 
				long l = 0;   // col index of Krige matrix 
				
				if (fLimPntSetChanged) 
				{
						// update aaH2 and recalculate matKrige and matKrige_Inverse
					k = 0;
					for (i= 0; i < iNrValidPnts; i++) {  // sieving the active points
						if (!fInsideLim[i]) continue;
						l = k + 1;
						for (j = i + 1; j < iNrValidPnts; j++) {
							if (!fInsideLim[j] ) continue;
							matKrige(l,k) = matKrige(k,l) = aaH2[i][j];
							l++;
						}
						matKrige(k, iNrLimitedPnts) = 1;  // 1's in last column 
						matKrige(iNrLimitedPnts, k) = 1;  // 1's in last row 
						vecD(k) = svm.rCalc(sqrt(rD2FromPix[i])); // no sorting
						rLimAttrib[k] = rAttrib[i];  // select only the attr of lim points
						rPtX[k] = cPoints[i].x;
						rPtY[k] = cPoints[i].y;
						k++;
          } 
					for (k = 0; k <= iNrLimitedPnts; k++) 
            matKrige(k, k) = 0;
					if ( iDegree > 0 ) {
            for ( k = 0; k < iNrLimitedPnts; k++ )
            {
              matKrige(iNrLimitedPnts + 1, k) = matKrige(k,iNrLimitedPnts + 1) = rPtX[k];
              matKrige(iNrLimitedPnts + 2, k) = matKrige(k,iNrLimitedPnts + 2) = rPtY[k];
            }
            for ( k = iNrLimitedPnts; k < iNrLimitedPnts + iNrOfTerms; k++ )
              matKrige(iNrLimitedPnts, k) = matKrige(iNrLimitedPnts + 1, k)
              = matKrige(iNrLimitedPnts + 2, k) = 0;
          }
          if ( iDegree == 2 ) {
            for ( k = 0; k < iNrLimitedPnts; k++ ){
              matKrige(iNrLimitedPnts + 3, k) =
              matKrige(k, iNrLimitedPnts + 3) = rPtX[k] * rPtX[k];
              matKrige(iNrLimitedPnts + 4, k) =
              matKrige(k, iNrLimitedPnts + 4) = rPtX[k] * rPtY[k];
              matKrige(iNrLimitedPnts + 5, k) =
              matKrige(k, iNrLimitedPnts + 5) = rPtY[k] * rPtY[k];
            }
            for ( k = iNrLimitedPnts; k < iNrLimitedPnts + iNrOfTerms; k++ )
              for ( l = k; l < iNrLimitedPnts + iNrOfTerms; l++ )
								matKrige(k, l) = matKrige(l, k) = 0;
          }
          fKrigSingular = matKrige.fSingular();
          if (!fKrigSingular) 
            matKrige.Invert(); 
					matKrigeLast = matKrige;
        }
				else 
        { 
					k = 0;
					for (i= 0; i < iNrValidPnts; i++) {
						if (!fInsideLim[i]) continue;
					  vecD(k) = svm.rCalc(sqrt(rD2FromPix[i])); 
						k++;
					}
				}
        if (!fKrigSingular) {

          //for (i= 0; i < iNrLimitedPnts; i++) {   // semivars between NonVisited pix
          //  vecD(i) = svm.rCalc(vecDPart(i)); // and point cLimPoints[i]
          //}
          vecD(iNrLimitedPnts) = 1;
          if ( iDegree > 0 ) {
            vecD(iNrLimitedPnts + 1) = crdNonVis.x;
            vecD(iNrLimitedPnts + 2) = crdNonVis.y;
          }
          if ( iDegree == 2 ) {
            vecD(iNrLimitedPnts + 3) = crdNonVis.x * crdNonVis.x;
            vecD(iNrLimitedPnts + 4) = crdNonVis.x * crdNonVis.y;
            vecD(iNrLimitedPnts + 5) = crdNonVis.y * crdNonVis.y;
          }
          
					CVector vecW(iNrLimitedPnts + iNrOfTerms); // vecW includes  iLimitedP Kriging weights
                                      // and iNrOfT trend coefficients
          double rW;
          for (i = 0; i < iNrLimitedPnts + iNrOfTerms; i++) {
            rW = 0;
            for (j = 0; j < iNrLimitedPnts + iNrOfTerms; j++)
              rW += matKrigeLast(i,j) * vecD(j);
            vecW(i) = rW;
          //  vecW(i) = (matKrige.GetRowVec(i) * vecD);//.GetColVec(0);// W = C_inv * D
          }
          // iNrLimitedPnts weights and Lagr param are now solved
          double rWeightedSum = 0;
          for (i = 0; i < iNrLimitedPnts; i++) 
            rWeightedSum += vecW(i) * rLimAttrib[i];
          rV = rWeightedSum;
          if (iNrLimitedPnts > iNrOfTerms) // case of more points than mathem minimum
          {
            rVerror = 0;
            for (i = 0; i < iNrLimitedPnts + iNrOfTerms; i++)
              rVerror += vecW(i) * vecD(i);
            rVerror = sqrt(abs(rVerror));
          }
          // else (if nr of points in circle at minimum 1, 3 or 6
          // leave rVerror (the error stand deviation) undefined
        } // end if matKrige Nonsingular 
      }// end if more then iMinNrPoints available
      rBufOut[iCol] = rV;
      rBufOutError[iCol] = rVerror;
    }
    pms->PutLineVal(iRow,rBufOut);
    if (mpErreur.fValid())
      mpErreur->PutLineVal(iRow,rBufOutError);
  } 
	if (mpErreur.fValid()) {
		mpErreur->Updated();
		mpErreur->Store();
	}
  return true;
}  // end fKrigingUniversal

struct PointStructure {
  Coord crdXY;
  bool fDup;
  double rAttrib;
};  

bool MapKriging::fFreezing()
{
  trq.SetText(SMAPTextCalculating);
  trq.fUpdate(0);
  bool fKriSimple = (krigMeth == kmSIMPLE);
// Changing of formula not allowed!
// when something goes wrong just throw an error
//  if (fKrigingSimple && iDuplicRemoval == drmNO)
//    iDuplicRemoval = drmAVERAGE;  // in simple Kriging, coinciding points not allowed
  iNrPoints = pmp ->iFeatures();
  //if (fKriSimple)  && (iNrPoints > 200)) // limitation removed
  //  TooManyKrigingSPoints(fnObj.sFile);  // for the time being
  // If iNrPoints  = 89
  // for simple Kriging all pnts enter into a 89 * 89 matrix 
  // with doubles (89*89*8 =63368) that must be inverted 
  // For ordin Kriging
  // the storage of the number of points is limited by
  // the storage capability of the array of arrays used
  // for matrix row storage. This can handle only 2730 = 63368/24 doubles
  // maximum, so to be on the safe side: 2500:
  // iMaxNrPoints < 81 inside the search circle allows the mat inversion
   
  //if (iNrPoints > 25000)								// limitation removed
  //  TooManyKrigingOPoints(fnObj.sFile);	// for the time being
  

  RowCol rcMrc = gr()->rcSize();
  iMRow = rcMrc.Row;
  iMCol = rcMrc.Col;
  bool fTransformCoords = cs() != pmp->cs();
  //double rMinDist = max(1.0e-10, rLimDist * 1.0e-10);
  iNrValidPnts = 0;   // valid point counter
  cPoints.Resize(iNrPoints);
  rAttrib.Resize(iNrPoints);
	Distance dis = Distance(pmp->cs(), m_distMeth);

  long i,j;
  // collect valid points from pointmap
  // for possible conversion of crds and validity check
  // and count them again (iNrValidPnts):
 
  for (i = 0; i < iNrPoints; i++)  {   
    double rAtr = pmp->rValue(i);
    Coord cVal = pmp->cValue(i); 
    if (cVal == crdUNDEF || rAtr == rUNDEF)
       continue;
    if (fTransformCoords) {
      cVal = cs()->cConv(pmp->cs(), cVal);
      if (cVal == crdUNDEF) continue;
    }
    cPoints[iNrValidPnts] = cVal; 
    rAttrib[iNrValidPnts] = rAtr;
    iNrValidPnts++;
  }  
  // cPoints.Resize(iNrValidPnts);
  // matrix of n(n-1)/2 squared ellipticdistances between pointmap validpoints
  // used for both simpleKriging (all pointpairs) and ordKriging ( pairs within LimDist) 
  //matH2 = RealMatrix(iNrValidPnts,iNrValidPnts);//local matrix to size matH2

    // ********************************************
    // * Treatment of (almost) coinciding points  *
    // ********************************************
  // iDuplicRemoval==0; no removal; it will cause UNDEF estimates 
  // iDuplicRemoval==1 remove duplic; take the average attrib values
  // iDuplicRemoval==2 remove duplic; take the first attrib value 
  trq.SetText(SMAPTextFindingDuplicates);
  { // begin of scope of aPtS
     Array<PointStructure> aPtS(iNrPoints);
     if (duplicRemoval != drmNO) {
      for (i = 0; i < iNrValidPnts; i++)  {   
        aPtS[i].crdXY = cPoints[i];  // store each point coord in temp struct
        aPtS[i].fDup  = false;       // including duplicate flag for removal
        aPtS[i].rAttrib = rAttrib[i];// and the resp attrib value
      }
      long iDuplicates, k;
      bool fAverageAttrib = (duplicRemoval==1); 
      // perhaps we should offer this as user choice ??
  
      double rD2, rTol2 = rTolerance * rTolerance; 
      // minimum distance of distinct points 
      double rSumOfAttrib; 
      k = 0;
      for (i = 0; i < iNrValidPnts; ++i) {
        if (trq.fUpdate(i, iNrValidPnts))
          return false;
        iDuplicates = 0;
        if (aPtS[i].fDup) continue;
        rSumOfAttrib = aPtS[i].rAttrib;
        for (j = i + 1; j < iNrValidPnts; ++j) {
          if (trq.fAborted())
            return false;
          //if (rRatio != 1)
          //  rD2 = rDist2Ellip(aPtS[i].crdXY, aPtS[j].crdXY, rAzimuth, rRatio);
          //else
            rD2 = dis.rDistance2(aPtS[i].crdXY, aPtS[j].crdXY);
          if (rD2 < rTol2) {  // Pntj coincides with Pnti
            aPtS[j].fDup = true;
            if (fAverageAttrib)  {
              rSumOfAttrib += aPtS[j].rAttrib;
              ++iDuplicates;
            }
          }         
        } 
        rAttrib[k] = rSumOfAttrib / (iDuplicates + 1);
        cPoints[k] = aPtS[i].crdXY;  
        ++k;
      }
      if (trq.fUpdate(iNrValidPnts, iNrValidPnts))
          return false;
      iNrValidPnts = k;
    } // end of iDuplicRemoval and possible averaging of attrib_values
  }

  trq.SetText(SMAPTextFindingPointPDist);
  if (iNrValidPnts < iMinNrPoints) {
    ///pms->FillWithUndef();
    String s(SMAPErrNotEnoughPoints_ii.scVal(), iNrValidPnts, iMinNrPoints);
    throw ErrorObject(WhatError(s, errMapKriging+20), fnObj);
  }  
  else  {
    Array<Array<double> > aaHH;
    aaHH.Resize(iNrValidPnts);
    for (i = 0; i < iNrValidPnts; ++i) 
      aaHH[i].Resize(iNrValidPnts);
    double rElemij;
    if (rRatio == 1) {
      for (i = 0; i < iNrValidPnts; ++i) {
        if (trq.fUpdate(i, iNrValidPnts))
          return false;
        for (j = i + 1; j < iNrValidPnts; ++j) {
          if (trq.fAborted())
            return false;
          double afs = sqrt(dis.rDistance2(cPoints[i], cPoints[j]));
          rElemij = svm.rCalc(afs);   aaHH[i][j] = rElemij;
          aaHH[j][i] = rElemij;
        }
      }  
    }
    else {
      for (i = 0; i < iNrValidPnts; ++i) {
        if (trq.fUpdate(i, iNrValidPnts))
          return false;
        for (j = i + 1; j < iNrValidPnts; ++j) {
          if (trq.fAborted())
            return false;
          double afst = sqrt(rDist2Ellip(cPoints[i], cPoints[j], rAzimuth, rRatio));
          rElemij = svm.rCalc(afst);
          aaHH[i][j] = rElemij;
          aaHH[j][i] = rElemij;
        }
      }
    }  
                     /*
  matH2 = RealMatrix(iNrValidPnts,iNrValidPnts);//local matrix to size matH2
  for (i = 0; i < iNrValidPnts; ++i) 
    for (j = i + 1; j < iNrValidPnts; ++j) 
      matH2(j,i) = matH2(i,j) =
       rDist2Ellip(cPoints[i], cPoints[j], rAzimuth, rRatio); */
		Map mpError;
		if (ptr.fErrorMap()){ // then open Error map:
			try {
				mpError = Map(ptr.fnErrorMap());
				mpError->OpenMapVirtual();
				mpError->CreateMapStore();
			}
			catch (const ErrorObject& err) {  // not found
				err.Show();
				String sExpr("MapComputedElsewhere(%S)",ptr.sNameQuoted());
				try {
					mpError = Map(ptr.fnErrorMap(), sExpr); //try to create Error map again
				}
				catch (const ErrorObject& err) {  // not found
	  			err.Show();
				}
			}
		}  
		if (mpError.fValid()) {
			Domain dom = Domain("Value");
			RangeReal rr = ptr.dvrs().rrMinMax();
			double rMin = 0;
			double rMax = rr.rHi() - rr.rLo();
			double rStep = ptr.dvrs().rStep()/10.0;          
			ValueRange vr = ValueRange(rMin, rMax, rStep);
			DomainValueRangeStruct dvs(dom, vr);
			mpError->SetDomainValueRangeStruct(dvs);
			String sDescr(SMAPTextKrigingErrorMap_S.scVal(), ptr.sName());
			mpError->sDescription = sDescr;
			mpError->CreateMapStore();
			mpError->KeepOpen(true);
		}
    if (fKriSimple)     
      return fKrigingSimple(aaHH, mpError); 
    else     
      return fKrigingUniversal(aaHH, mpError);
  }
}

String MapKriging::sExpression() const
{
  String sDuplicRemoval;
  switch (duplicRemoval) {
    case drmNO: sDuplicRemoval = "no"; break;
    case drmAVERAGE:  sDuplicRemoval = "average"; break;
    case drmFIRSTVAL: sDuplicRemoval = "firstval"; break;
  }
  String sDistanceMethod;;
	if (m_distMeth == Distance::distSPHERE)
		sDistanceMethod = String("sphere");
	else
		sDistanceMethod = String("plane");
	String sErrorMap = "1";
	if (ptr.fErrorMap()) {
		if (!fCIStrEqual(String("%S_Error", fnObj.sFile), ptr.fnErrorMap().sFile))
			sErrorMap = ptr.fnErrorMap().sShortName(false);
	}
	else
		sErrorMap = "0";
  if (rLimDist == 0 || krigMeth == kmSIMPLE)
    return String("MapKrigingSimple(%S,%S,%S,%S,%S,%S,%.4f)",
                 pmp->sName(false, fnObj.sPath()),
                 gr()->sName(false, fnObj.sPath()),
								 svm.sExpression(),
								 sDistanceMethod,
                 sErrorMap,
                 sDuplicRemoval,
                 rTolerance);
  else if (rLimDist > 0 && krigMeth == kmORDINARY) 
    return String("MapKrigingOrdinary(%S,%S,%S,%.2f,%S,%S,%li,%li,%S,%.4f)",
                 pmp->sName(false, fnObj.sPath()),
                 gr()->sName(false, fnObj.sPath()),
                 svm.sExpression(),
                 rLimDist,
                 sDistanceMethod,
                 sErrorMap,
                 iMinNrPoints,
                 iMaxNrPoints,
                 sDuplicRemoval,
                 rTolerance);
  else if (iDegree == 0 && rRatio != 1)             
    return String("MapKrigingAnisotropic(%S,%S,%S,%.2f,%.2f,%.2f,%S,%li,%li,%S,%.4f)",
                 pmp->sName(false, fnObj.sPath()),
                 gr()->sName(false, fnObj.sPath()),
                 svm.sExpression(),
                 rLimDist,
                 rAzimuth * 180 / M_PI,
                 rRatio,
                 sErrorMap,
                 iMinNrPoints,
                 iMaxNrPoints,
                 sDuplicRemoval,
                 rTolerance);
	else               
    return String("MapKrigingUniversal(%S,%S,%S,%.2f,%li,%.2f,%.2f,%S,%li,%li,%S,%.4f)",
                 pmp->sName(false, fnObj.sPath()),
                 gr()->sName(false, fnObj.sPath()),
                 svm.sExpression(),
                 rLimDist,
                 iDegree,
                 rAzimuth * 180 / M_PI,
                 rRatio,
                 sErrorMap,
                 iMinNrPoints,
                 iMaxNrPoints,
                 sDuplicRemoval,
                 rTolerance);
}
