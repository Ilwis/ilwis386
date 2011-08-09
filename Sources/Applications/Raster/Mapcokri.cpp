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
/* MapCoKriginG
   april 1998, Jan Hendrikse
   Copyright Ilwis System Development ITC
	Last change:  WK   30 Jun 99    5:27 pm
*/
#include "Headers\toolspch.h"
#include "Applications\Raster\MAPCOKRI.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"
#define EPS10 1.0e-10

IlwisObjectPtr * createMapCoKriging(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapCoKriging::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapCoKriging(fn, (MapPtr &)ptr);
}

const char* MapCoKriging::sSyntax() {
  return "MapCoKriging(pntmapA,pntmapB,georef,semivarA,semivarB,crossvarAB,limdist[,errormap[,minNrpts[,"
      "maxNrpts[,no|average|firstval[,tolerance[,plane|sphere]]]]]])\n";
 }

static void SameNameErrMapError(const FileName& fn)
{
  throw ErrorObject(WhatError(String(TR("Name of Error Map already used")), errMapCoKriging), fn);
}
//static void TooManyKrigingKSPoints(const FileName& fn) {
//  throw ErrorObject(WhatError(String(TR("%S cannot interpolate with more than 200 points"), "CoKriging"), errMapCoKriging), fn);
//}
static void PointMapEmpty(const FileName& fn) {
  throw ErrorObject(WhatError(String(TR("Point map needs to contain at least 2 points")), errMapCoKriging +1), fn);
}
static void TooManyKrigingOPoints(const FileName& fn) {
  throw ErrorObject(WhatError(String(TR("%S cannot interpolate with more than 25000 points").c_str(), String("CoKriging")), errMapCoKriging +1), fn);
}
static void MinNrKrigingPnts(const FileName& fn) {
  throw ErrorObject(WhatError(String(TR("Minimum number must be at least 1")), errMapCoKriging +6), fn);
}
static void MaxNrKrigingPnts(const FileName& fn) {
  throw ErrorObject(WhatError(String(TR("Maximum number may not be less than minimum")), errMapCoKriging +7), fn);
}
static void LimitingDistanceOutsideRange(const FileName& fn) {
  throw ErrorObject(WhatError(String(TR("Limiting distance must be positive and < 2 x greatest point pair distance")), errMapCoKriging +8), fn);
}
static void TolerancePos(const FileName& fn) {
  throw ErrorObject(WhatError(String(TR("Tolerance must be greater than 0")), errMapCoKriging +9), fn);
}
static void NrParamCoKriging(const FileName& fn) {
  throw ErrorObject(WhatError(String(TR("CoKriging needs 7 to 12 parameters")), errMapCoKriging +10), fn);
}
static void NoOverlap(const FileName& fn) {
  throw ErrorObject(WhatError(String(TR("Predictand and Covariable Maps don't overlap")), errMapCoKriging +11), fn);
}
static void IllegalNameErrMap(const FileName& fn) {
  throw ErrorObject(WhatError(String(TR("Illegal Name for Error Map")), errMapCoKriging + 12), fn);
}

bool MapCoKriging::fValueRangeChangeable() const
{
  return true;
}

bool MapCoKriging::fDomainChangeable() const
{
  return true;
}

ValueRange MapCoKriging::vrDefault(const PointMap& pmap)
{
  RangeReal rr = pmap->rrMinMax(BaseMapPtr::mmmNOCALCULATE);
  if (!rr.fValid())
    rr = pmap->dvrs().rrMinMax();
	double rRangeWidth = abs(rr.rHi() - rr.rLo());
  double rLowestOut = rr.rLo() - rRangeWidth;
  double rHighestOut = rr.rHi() + rRangeWidth;
  double rStep = pmap->dvrs().rStep()/10.0;
  return ValueRange(rLowestOut, rHighestOut, rStep);
}

MapCoKriging* MapCoKriging::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 13 || iParms < 7)
		NrParamCoKriging(fn); 
  PointMap pmA(as[0], fn.sPath());
  PointMap pmB(as[1], fn.sPath());
  long iNrP = pmA->iFeatures();
  if (iNrP < 2) 
    PointMapEmpty(fn); 
  iNrP = pmB->iFeatures();
  if (iNrP < 2) 
    PointMapEmpty(fn);
  CoordSystem csA = pmA->cs();
  CoordSystem csB = pmB->cs();
  String sCsyA = csA->sName();
  String sCsyB = csB->sName();
  GeoRef gr(as[2]);
  CoordSystem csG = gr->cs();
  String sGeoRefCsy = csG->sName();
  if (!pmB->cs()->fConvertFrom(pmA->cs()))
      IncompatibleCoordSystemsError(sCsyA, sCsyB, pmA->sTypeName(), errMapCoKriging+2);
  if (!gr->cs()->fConvertFrom(pmA->cs()))
      IncompatibleCoordSystemsError(sGeoRefCsy, sCsyA, csG->sTypeName(), errMapCoKriging+3);
  if (!gr->cs()->fConvertFrom(pmB->cs()))
      IncompatibleCoordSystemsError(sGeoRefCsy, sCsyB, csG->sTypeName(), errMapCoKriging+4);
  bool fTrsfCrdsA = gr->cs() != pmA->cs();
  bool fTrsfCrdsB = gr->cs() != pmB->cs();
  Coord cMinA, cMinB, cMaxA, cMaxB; 
  pmA->Bounds(cMinA, cMaxA);
  pmB->Bounds(cMinB, cMaxB);
  if (fTrsfCrdsA) { //transform map-bounds of A to common coordsys csG of Kriging georef  
      cMinA = gr->cs()->cConv(pmA->cs(), cMinA);
      cMaxA = gr->cs()->cConv(pmA->cs(), cMaxA);
  }
  if (fTrsfCrdsB) { //transform map-bounds of B to common coordsys csG of Kriging georef
      cMinB = gr->cs()->cConv(pmB->cs(), cMinB);
      cMaxB = gr->cs()->cConv(pmB->cs(), cMaxB);
  }
  CoordBounds cbA = CoordBounds(cMinA, cMaxA);
  CoordBounds cbB = CoordBounds(cMinB, cMaxB);
  if (!cbA.fContains(cbB))
    NoOverlap(fn); 
  double rDiffAX = cMaxA.x - cMinA.x;
  double rDiffAY = cMaxA.y - cMinA.y;//largest possible predictand lag:
  double rMaxDiff = sqrt(rDiffAX * rDiffAX + rDiffAY * rDiffAY); 
	if (0 != pmA->cs()->pcsLatLon()) //for search radius limitation
		rMaxDiff *= 111111.1; //make degrees quasi-metric (1 degr)
	SemiVariogram svA(as[3]);
  SemiVariogram svB(as[4]);
  SemiVariogram svAB(as[5]);
  double rLimDst = 1.0;
  String sErrMp;
	FileName fnErrMp;
  long iMin = 1;   // default sample size limits
  long iMax = 16;
  DuplicRemoveMethod iAvg = drmAVERAGE;
  double rTol = 0.1;
  rLimDst = as[6].rVal();
  if (rLimDst <= 0 || rLimDst > 2 * rMaxDiff)  
      LimitingDistanceOutsideRange(fn);
  if (iParms > 7) {
    sErrMp = as[7];
    if (sErrMp != "" && (sErrMp != "0")) 
		{
			fnErrMp = fn;// ErrorMap name is made using KrigMap name
      fnErrMp.sFile &= "_Error";
      fnErrMp.sExt = ".mpr";
      //if (fn == fnErrMap)
      //  SameNameErrMapError(fn);
      if (fnErrMp.fExist())
        FileAlreadyExistError(fnErrMp);
      if (!fnErrMp.fValid())  
        IllegalNameErrMap(fnErrMp);
		}
	}
  if (iParms > 8){
    iMin = (long)as[8].rVal();
    if (iMin < 1)  
      MinNrKrigingPnts(fn);
  }    
  if (iParms > 9){
    iMax = (long)as[9].rVal();
    if (iMax < iMin)  
      MaxNrKrigingPnts(fn);
  }    
  if (iParms > 10){
    if (fCIStrEqual("no", as[10])) 
      iAvg = drmNO;
    else if (fCIStrEqual("average", as[10])) 
      iAvg = drmAVERAGE;
    else if (fCIStrEqual("firstval", as[10]))
      iAvg = drmFIRSTVAL;
    else
      ExpressionError(sExpr, sSyntax());  
  }    
  if (iParms > 11){
    rTol = as[11].rVal();
    if (rTol <= 0)  
      TolerancePos(fn);
  }  
  bool fSphericDist = false; //default 
	if (iParms == 13) { 
		if (fCIStrEqual("sphere", as[12]))
     fSphericDist = true;
		else if (fCIStrEqual("plane", as[12]))
     fSphericDist = false;
		else
      ExpressionError(sExpr, sSyntax());
	}
  return new MapCoKriging(fn, p, pmA, pmB, gr, svA, svB, svAB, rLimDst, 
													fnErrMp, iMin, iMax, iAvg, rTol, fSphericDist);
}

MapCoKriging::MapCoKriging(const FileName& fn, MapPtr& ptr)
: MapFromPointMap(fn, ptr)
{
  svmA = SemiVariogram(fnObj, "Variogram");
  svmB = SemiVariogram(fnObj, "Covariogram");
  svmAB = SemiVariogram(fnObj, "CrossVariogram");
  ReadElement("MapCoKriging", "CoVariableMap", pmpB);
  ReadElement("MapCoKriging", "LimitingDistance", rLimDist);
  ReadElement("MapCoKriging", "ErrorVarianceMap", ptr.fnErrorMap());
  ReadElement("MapCoKriging", "MinNrofPoints", iMinNrPoints);
  ReadElement("MapCoKriging", "MaxNrofPoints", iMaxNrPoints);
  String sDuplicRemoval;
  ReadElement("MapCoKriging", "DuplicRemoval",sDuplicRemoval);
  if (fCIStrEqual("no", sDuplicRemoval))
    duplicRemoval = drmNO;
  else if (fCIStrEqual("average", sDuplicRemoval)) 
    duplicRemoval = drmAVERAGE;
  else if (fCIStrEqual("firstval", sDuplicRemoval))
    duplicRemoval = drmFIRSTVAL;
  else  
    duplicRemoval = drmAVERAGE;
  ReadElement("MapCoKriging", "Tolerance", rTolerance);
  Init();
}  

MapCoKriging::MapCoKriging(const FileName& fn, MapPtr& ptr,
                       const PointMap& pmA, const PointMap& pmB,
                       const GeoRef& gr,
                       const SemiVariogram& svA,
                       const SemiVariogram& svB,
                       const SemiVariogram& svAB,
                       double rLimDst,
                       const FileName& fnErrM,
                       long iMin, long iMax,
                       DuplicRemoveMethod drm,
                       double rToler,
                       const bool fSpheric)
: MapFromPointMap(fn, ptr, pmA, gr),
  pmpB(pmB),
  svmA(svA), svmB(svB), svmAB(svAB), 
  rLimDist(rLimDst), 
  //fnErrorMap(fnErrM),
  iMinNrPoints(iMin), iMaxNrPoints(iMax),
  duplicRemoval(drm), rTolerance(rToler)
{
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, errMapCoKriging+2);
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapCoKriging+3);
  if (!cs()->fConvertFrom(pmp->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapMovingAverage+3);
  DomainValueRangeStruct dvrs(vrDefault(pmp));
  SetDomainValueRangeStruct(dvrs);
  fNeedFreeze = true;
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.SetErrorMap(fnErrM);
  Init();
	Store(); // stores this Kriging map on disk before mpKrigingError (see next lines) looks for it 
	String sExpr("MapComputedElsewhere(%S)",ptr.sNameQuoted());
	if (ptr.fErrorMap()) {// create Error map
		Map mpComputedElsewhere(ptr.fnErrorMap(), sExpr);
		String sDescr(TR("Kriging Error Map of %S").c_str(), ptr.sName());
    mpComputedElsewhere->sDescription = sDescr;
		mpComputedElsewhere->Store();
		if (fSpheric)
			m_distMeth = Distance::distSPHERE;
		else
			m_distMeth = Distance::distPLANE;
  }
}

MapCoKriging::~MapCoKriging()
{
}

void MapCoKriging::Store()
{
  MapFromPointMap::Store();
  //String sKrigingMethod;
  //WriteElement("MapCoKriging", "KrigingMethod", sKrigingMethod);
  WriteElement("MapFromPointMap", "Type", "MapCoKriging");
  WriteElement("MapCoKriging", "CoVariableMap", pmpB);
  svmA.Store(fnObj, "Variogram");
  svmB.Store(fnObj, "Covariogram");
  svmAB.Store(fnObj, "CrossVariogram");
  WriteElement("MapCoKriging", "LimitingDistance", rLimDist);
  if (ptr.fnErrorMap().sFile.length() != 0)
    WriteElement("MapCoKriging", "ErrorVarianceMap", ptr.fnErrorMap());
  WriteElement("MapCoKriging", "MinNrofPoints", iMinNrPoints);
  WriteElement("MapCoKriging", "MaxNrofPoints", iMaxNrPoints);
  String sDuplicRemoval;
  switch (duplicRemoval) {
    case drmNO: sDuplicRemoval = "no"; break;
    case drmAVERAGE: sDuplicRemoval = "average"; break;
    case drmFIRSTVAL: sDuplicRemoval = "firstval"; break;
  }
  WriteElement("MapCoKriging", "DuplicRemoval", sDuplicRemoval);
  WriteElement("MapCoKriging", "Tolerance", rTolerance);
  //WriteElement("MapCoKriging", "NrofValidPoints", iNrValidPnts);
}

void MapCoKriging::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "MapCoKriging";
  htpFreeze = "ilwisapp\\cokriging_algorithm.htm";
}

struct saTwoPointArr {    // used to sort in Kriging Ordinary
  Array<int>* iClosestPnts;  
  Array<double>* rDist2FromPix;
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

bool MapCoKriging::fCoKriging(Array<Array<double> > & aaAA,
                   Array<Array<double> > & aaBB, Array<Array<double> > & aaAB,
									 Map & mpErreur)
{ //  CoKriging with predictand A and covariable B in limiting circle
  trq.SetText(TR("Calculating"));
  int i,j;
  Array<int> iClosestPntsA(iNrValidPntsA);  // temp array of pntnrs of pnts A thin limiting circle
  Array<double> rLimAttribA(iNrValidPntsA);  // their respective attrib values
  Array<double> rDist2FromPixA(iNrValidPntsA); // their resp distance from NonVisited pixel (RowCol)
  Array<double> rD2FromPixA(iNrValidPntsA); // idem but not sorted according to distance
	Array<bool> fInsideLimA(iNrValidPntsA);      // pointnrs of points A in current lim circle
  Array<bool> fInsideLimBeforeA(iNrValidPntsA);// pointnrs of points A in previous lim circle
  for (i = 0; i < iNrValidPntsA; i++) 
    fInsideLimBeforeA[i]       = false;
  bool fLimPntSetChangedA = false;         // is set to true as soon as limit point set changes

  Array<int> iClosestPntsB(iNrValidPntsB);  // temp array of pntnrs of pnts B thin limiting circle
  Array<double> rLimAttribB(iNrValidPntsB);  // their respective attrib values
  Array<double> rDist2FromPixB(iNrValidPntsB); // their resp distance from NonVisited pixel (RowCol)
  Array<double> rD2FromPixB(iNrValidPntsB); // idem but not sorted according to distance
	Array<bool> fInsideLimB(iNrValidPntsB);      // pointnrs of points B in current lim circle
  Array<bool> fInsideLimBeforeB(iNrValidPntsB);// pointnrs of points B in previous lim circle
  for (i = 0; i < iNrValidPntsB; i++) 
    fInsideLimBeforeB[i]       = false;
  bool fLimPntSetChangedB = false;         // is set to true as soon as limit point set changes

  double rLim2 = rLimDist * rLimDist; 
  RealMatrix matCoKrigeLast;
  RealMatrix matCoKrige; 
  long iNrPrevLimitedPntsA = 0; 
  long iNrPrevLimitedPntsB = 0; 
  bool fKrigSingular;
  RealBuf rBufOut(iMCol);
  RealBuf rBufOutError(iMCol); 
	Distance dis = Distance(pmp->cs(), m_distMeth);
  
  trq.SetText(TR("CoKriging"));   
 
	for (long iRow = 0; iRow < iMRow; iRow++)  {  // estimate ('predict') NonVisited pixels row by row
    if (trq.fUpdate(iRow, iMRow))
      return false;
    for (long iCol = 0; iCol < iMCol; iCol++)  {  // estimate NonVisited pixels col by col
      if (trq.fAborted())
        return false;
      Coord  crdNonVis = gr()->cConv(RowCol(iRow, iCol));//coord of NonVisited pixel
      // put limiting rectangle around NonVisited point:
      double rLD = abs(rLimDist);
      Coord coLess=(Coord((crdNonVis.x-rLD),crdNonVis.y-rLD));
      Coord coMore=(Coord((crdNonVis.x+rLD),crdNonVis.y+rLD));
      long iNrLimitedPntsA = 0; // counts points A within limiting distance
      long iNrLimitedPntsB = 0; // counts points B within limiting distance

      // Check if points are within limiting distance and if so
      // add them to the 4 arrays of saTwoPointArr at position n:
      long n = 0;
      long iInCircle = 0;//counts only points of set A inside lim circle:
      for (i = 0; i < iNrValidPntsA; i++) {
        Coord crdPi = cPointsA[i];
        if (!(crdPi.fInside(coLess,coMore))) continue;// outside box: skip
        double  rvDist2 = dis.rDistance2(crdNonVis,crdPi);
        if (rvDist2>rLim2) continue;  // outside ellips: skip
        iClosestPntsA[iInCircle] = i;
        rDist2FromPixA[iInCircle] = rvDist2;
				rD2FromPixA[i] = rvDist2;
        iInCircle++;
      }
      iNrLimitedPntsA = iInCircle;
      if (iInCircle > iMaxNrPoints) { // sort out only the MaxNrP points A
        saTwoPointArr saT;                // that are closest to (iRow, iCol):
        saT.iClosestPnts = &iClosestPntsA;  
        saT.rDist2FromPix = &rDist2FromPixA;
        QuickSort(0, iInCircle-1, LessFunc, SwapFunc, &saT);
        iNrLimitedPntsA = iMaxNrPoints;
      }

      iInCircle = 0;  //counts only points of set B inside lim circle:
      for (i = 0; i < iNrValidPntsB; i++) {
        Coord crdPi = cPointsB[i];
        if (!(crdPi.fInside(coLess,coMore))) continue;// outside box: skip
        double  rvDist2 = dis.rDistance2(crdNonVis,crdPi);
        if (rvDist2>rLim2) continue;  // outside ellips: skip
        iClosestPntsB[iInCircle] = i;
        rDist2FromPixB[iInCircle] = rvDist2;
				rD2FromPixB[i] = rvDist2;
        iInCircle++;
      }
      iNrLimitedPntsB = iInCircle;
      if (iInCircle > iMaxNrPoints) { // sort out only the MaxNrP points A
        saTwoPointArr saT;                // that are closest to (iRow, iCol):
        saT.iClosestPnts = &iClosestPntsB;  
        saT.rDist2FromPix = &rDist2FromPixB;
        QuickSort(0, iInCircle-1, LessFunc, SwapFunc, &saT);
        iNrLimitedPntsB = iMaxNrPoints;
      }
      
      for (i = 0; i < iNrValidPntsA; i++) 
        fInsideLimA[i] = false;
      for (n = 0; n < iNrLimitedPntsA; n++)  //put numbers (indices) of closest
        fInsideLimA[iClosestPntsA[n]] = true; //pnts A in array fInsideLimA
      for (i = 0; i < iNrValidPntsB; i++) 
        fInsideLimB[i] = false;
      for (n = 0; n < iNrLimitedPntsB; n++)  //put numbers (indices) of closest
        fInsideLimB[iClosestPntsB[n]] = true; //pnts B in array fInsideLimB
        
      double rV = rUNDEF;
      double rVerror = rUNDEF;
      if (iNrLimitedPntsA >= iMinNrPoints && iNrLimitedPntsB >= iMinNrPoints ) 
       // Carry out Kriging prediction only if sets A and B are both large enough
      { 
        //find out a change in size of lim point set A:  
        fLimPntSetChangedA = (iNrPrevLimitedPntsA != iNrLimitedPntsA); 
        n = 0;
        //find out a change in contents of lim point set A: 
        while (!fLimPntSetChangedA && n < iNrValidPntsA) {
          fLimPntSetChangedA =
            (fInsideLimBeforeA[iClosestPntsA[n]]
                != fInsideLimA[iClosestPntsA[n]]); //|| fLimPntSetChanged;
          n++;
        }  
        //find out a change in size of lim point set B:  
        fLimPntSetChangedB = (iNrPrevLimitedPntsB != iNrLimitedPntsB); 
        n = 0;
        //find out a change in contents of lim point set B:
        while (!fLimPntSetChangedB && n < iNrValidPntsB) {
          fLimPntSetChangedB =
            (fInsideLimBeforeB[iClosestPntsB[n]]
                != fInsideLimB[iClosestPntsB[n]]); //|| fLimPntSetChanged;
          n++;
        }  
        // copy lim point sets for later comparison:
        for (n = 0; n < iNrValidPntsA; n++)
          fInsideLimBeforeA[n]//[iClosestPntsA[n]]
              = fInsideLimA[n];//[iClosestPntsA[n]];
        iNrPrevLimitedPntsA = iNrLimitedPntsA; ;  
        for (n = 0; n < iNrValidPntsB; n++)
          fInsideLimBeforeB[n]//[iClosestPntsB[n]]
              = fInsideLimB[n];//[iClosestPntsB[n]];
        iNrPrevLimitedPntsB = iNrLimitedPntsB; ; 
          
      // Put mutual semivar(distance) in  matrix matCoKrige,
      // and semivar(dist to NonVisited point) in vector vecD
      // Both matCoKrige and vecD have variable size: iNrValidPnts
      // depending on contents of Limiting circle while NonVisited point moves
      // But update filling of matCoKrige only needed if fLimPntSetChanged
      // recalculation of Kriging matrix inverse is only done in that case
      // Right hand side vecD changes witj each move of estimated RowCol
      
        //All selected points enter into Kriging system of order iAllLimitedPnts +1:
        long iAllLimitedPnts = iNrLimitedPntsA + iNrLimitedPntsB;
        CVector vecDPart(iAllLimitedPnts); //  right hand vector D with distances:
         
        for (n = 0; n < iNrLimitedPntsA; n++) 
          vecDPart(n) = sqrt(rDist2FromPixA[n]); //can argum be < 0 ??????????
        for (n = 0; n < iNrLimitedPntsB; n++) 
          vecDPart(iNrLimitedPntsA + n) = sqrt(rDist2FromPixB[n]); 

				CVector vecD(iAllLimitedPnts + 2); // right-hand side of Kriging System
        matCoKrige = RealMatrix(iAllLimitedPnts + 2);
				//matCoKrige = RealMatrix(iAllLimitedPnts);
        long k, l;   // row and col counters in partial matrix
        
        if (fLimPntSetChangedA || fLimPntSetChangedB) //even uitgeschakeld 19/5/99 janh
        {
				// update matCoKrige and recalculate 
        // matKrige and matKrige_Inverse
					k = 0;
          for (i = 0; i < iNrValidPntsA; i++) { //upper left mat G11
            if (!fInsideLimA[i]) continue;
						l = k + 1;
						for (j = i + 1; j < iNrValidPntsA; j++) {
							if (!fInsideLimA[j]) continue;
							// 2 points A,A within the limiting distance 
							// (upper and lower triangle of the partial matrix aaAA:
              matCoKrige(l,k) = matCoKrige(k,l) = aaAA[i][j];
							l++;
            }
						matCoKrige(k, iAllLimitedPnts) = 1;  // upper part mu1 column of 1's
            matCoKrige(k, iAllLimitedPnts + 1) = 0;  // upper part mu2 column of 0's 
						matCoKrige(iAllLimitedPnts, k) = 1;  // left part sec last row of 1's
            matCoKrige(iAllLimitedPnts + 1, k) = 0;  // left part row of 1's
						vecD(k) = svmA.rCalc(sqrt(rD2FromPixA[i])); // no sorting, semivars to A points
						rLimAttribA[k] = rAttribA[i];// select only the attr of lim A points
						k++;
          } 
          k = 0;
					for (i = 0; i < iNrValidPntsA; i++) { //upper right mat G12
						if (!fInsideLimA[i]) continue;
						l = iNrLimitedPntsA;
            for (j = 0; j < iNrValidPntsB; j++) {
							if (!fInsideLimB[j]) continue;
							// 2 points A,B within the limiting distance 
							// (upper and lower triangle of the partial matrix aaBB:
              matCoKrige(k, l) =				// upper right G12
              matCoKrige(l,k) = aaAB[i][j];  // and lower left G21                
							l++;
            }
						k++;
          } 
					k = iNrLimitedPntsA;
          for (i = 0; i < iNrValidPntsB; i++) { //lower right
						if (!fInsideLimB[i]) continue;
						l = k + 1;
            for (j = i + 1; j < iNrValidPntsB; j++) { // mat G22
							if (!fInsideLimB[j]) continue;
							// 2 points B,B within the limiting distance 
							// (upper and lower triangle of the partial matrix aaAB:
               matCoKrige(l, k) = matCoKrige(k, l) = aaBB[i][j];
							 l++;
            }
						matCoKrige(k, iAllLimitedPnts) = 0;  // lower part mu1 column of 0's
            matCoKrige(k, iAllLimitedPnts + 1) = 1;  // lower part mu2 column of 1's 
						matCoKrige(iAllLimitedPnts, k) = 0;  // right part sec last row of 1's
            matCoKrige(iAllLimitedPnts + 1, k) = 1;  // right part last row of 1's
						vecD(k) = svmAB.rCalc(sqrt(rD2FromPixB[i])); // no sorting, crossvars to B points
						rLimAttribB[k - iNrLimitedPntsA] = rAttribB[i];// select only the attr of lim A points
						k++;
          } 

          for (i= 0; i < iAllLimitedPnts; i++) {
            for (j = i + 1; j < iAllLimitedPnts; j++) {
              matCoKrige(i,j) = matCoKrige(j,i);
            }
						//if (i < iNrLimitedPntsA) {
            //  matCoKrige(i, iAllLimitedPnts) = 1;  // upper part mu1 column of 1's
            //  matCoKrige(i, iAllLimitedPnts + 1) = 0;  // upper part mu2 column of 0's 
            //}  
            //else 
						//{
            //  matCoKrige(i, iAllLimitedPnts) = 0;  // lower part mu1 column of 0's
            //  matCoKrige(i, iAllLimitedPnts + 1) = 1;  // lower part mu2 column of 1's 
            //}  
            //if (i < iNrLimitedPntsA) {
            //  matCoKrige(iAllLimitedPnts, i) = 1;  // left part sec last row of 1's
            //  matCoKrige(iAllLimitedPnts + 1, i) = 0;  // left part row of 1's
            //}  
            //else 
						//{  
            //  matCoKrige(iAllLimitedPnts, i) = 0;  // right part sec last row of 1's
            //  matCoKrige(iAllLimitedPnts + 1, i) = 1;  // right part last row of 1's
            //} 
          }
          for (i = 0; i <= iAllLimitedPnts + 1; i++) 
            matCoKrige(i, i) = 0;
          matCoKrige(iAllLimitedPnts + 1, iAllLimitedPnts) = matCoKrige(iAllLimitedPnts, iAllLimitedPnts + 1) = 0;
          fKrigSingular = matCoKrige.fSingular();
          if (!fKrigSingular) 
            matCoKrige.Invert(); 
					matCoKrigeLast = matCoKrige;
        }
				else // even for unchanged limpointset, the right side vecD will change for each rowcol
        { 
					k = 0;
					for (i= 0; i < iNrValidPntsA; i++) {
						if (!fInsideLimA[i]) continue;
					  vecD(k) = svmA.rCalc(sqrt(rD2FromPixA[i])); 
						k++;
					}
					for (i= 0; i < iNrValidPntsB; i++) {
						if (!fInsideLimB[i]) continue;
						vecD(k) = svmAB.rCalc(sqrt(rD2FromPixB[i])); // no sorting, crossvars to B points
						k++;
					}
				}
         
        if (!fKrigSingular) {

          vecD(iAllLimitedPnts) = 1;
          vecD(iAllLimitedPnts + 1) = 0;
          
          CVector vecW(iAllLimitedPnts + 2); // vecW includes  iAllLimitedP Kriging weights and 2 Lagr multipl                
					double rW;
          for (i = 0; i < iAllLimitedPnts + 2; i++) {
            rW = 0;
            for (j = 0; j < iAllLimitedPnts + 2; j++)
              rW += matCoKrigeLast(i,j) * vecD(j); //inverseKrigeMatrix times vecD
            vecW(i) = rW;
          //  vecW(i) = (matKrige.GetRowVec(i) * vecD);//.GetColVec(0);// W = C_inv * D
          }
          // iLimitedPnts weights for predictand and covariable and both Lagr param are now solved
          double rWeightedSum = 0;
          for (i = 0; i < iNrLimitedPntsA; i++)       // contribution of predictand A to the estimate
            rWeightedSum += vecW(i) * rLimAttribA[i]; 
          for (i = 0; i < iNrLimitedPntsB; i++)       // contribution of covariable B to the estimate
            rWeightedSum += vecW(iNrLimitedPntsA + i) * rLimAttribB[i];  
          rV = rWeightedSum;     // 
          if (iNrLimitedPntsA > 1) // case where nr of points at least 2 (mathem minimum) 
          {
            rVerror = 0;
            for (i = 0; i < iAllLimitedPnts + 1; i++) // variance computed from predictand and covariable values
              rVerror += vecW(i) * vecD(i);           // and from only the first Lagrange multiplier
            rVerror = sqrt(abs(rVerror));
          }

          // else (if nr of points in circle at is 1 or 0
          // leave rVerror (the error stand deviation) undefined
        } // end  'if matKrige Nonsingular' 
      }// end  'if more then iMinNrPoints available'
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
}  // end fCoKriging

struct PointStruc {
  Coord crdXY;
  bool fDup;
  double rAttrib;
};  

bool MapCoKriging::fFreezing()
{
  trq.SetText(TR("Calculating"));
  trq.fUpdate(0);
  PointMap pmpA = pmp;   // give clear alias to member pmp 
                  //(the pointmap  with predictands)
  iNrPointsA = pmpA ->iFeatures();
  if (iNrPointsA > 25000)
    TooManyKrigingOPoints(fnObj.sFile); 
  // If iNrPoints  = 89
  // for simple Kriging all pnts enter into a 89 * 89 matrix 
  // with doubles (89*89*8 =63368) that must be inverted 
  // For coKriging
  // the storage of the number of points is limited by
  // the storage capability of the 3 arrays of arrays used
  // for matrix row storage. This can handle only 2630 = 63368/24 doubles
  // maximum, so to be on the safe side: 2600:
  // iMaxNrPoints < 81 inside the search circle allows the mat inversion

  iNrPointsB = pmpB ->iFeatures();
  if (iNrPointsB > 25000)
    TooManyKrigingOPoints(fnObj.sFile);
  
  RowCol rcMrc = gr()->rcSize();
  iMRow = rcMrc.Row;
  iMCol = rcMrc.Col;
  fTransformCoordsA = cs() != pmpA->cs();
  fTransformCoordsB = cs() != pmpB->cs();
  //double rMinDist = max(1.0e-10, rLimDist * 1.0e-10);
  iNrValidPntsA = 0; // counts valid points found in A
  iNrValidPntsB = 0; //   ,,     ,,     ,,    ,,  ,, B
  cPointsA.Resize(iNrPointsA);
  rAttribA.Resize(iNrPointsA);
  cPointsB.Resize(iNrPointsB);
  rAttribB.Resize(iNrPointsB);
	Distance dis = Distance(pmp->cs(), m_distMeth);

  long i,j;
  // collect valid points from pointmapA
  // for possible conversion of crds and validity check
  // and count them again (iNrValidPntsA):
 
  for (i = 0; i < iNrPointsA; i++)  {   
    double rAtr = pmpA->rValue(i);
    Coord cVal = pmpA->cValue(i); 
    if (cVal == crdUNDEF || rAtr == rUNDEF)
       continue;
    if (fTransformCoordsA) {
      cVal = cs()->cConv(pmpA->cs(), cVal);
      if (cVal == crdUNDEF) continue;
    }
    cPointsA[iNrValidPntsA] = cVal; 
    rAttribA[iNrValidPntsA] = rAtr;
    iNrValidPntsA++;
  }  
  for (i = 0; i < iNrPointsB; i++)  {   
    double rAtr = pmpB->rValue(i);
    Coord cVal = pmpB->cValue(i); 
    if (cVal == crdUNDEF || rAtr == rUNDEF)
       continue;
    if (fTransformCoordsB) {
      cVal = cs()->cConv(pmpB->cs(), cVal);
      if (cVal == crdUNDEF) continue;
    }
    cPointsB[iNrValidPntsB] = cVal; 
    rAttribB[iNrValidPntsB] = rAtr;
    iNrValidPntsB++;
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
  trq.SetText(TR("Finding duplicate points  "));
  { // begin of scope of aPtSA and of aPtSB
    Array<PointStruc> aPtSA(iNrPointsA);
    Array<PointStruc> aPtSB(iNrPointsB);
    if (duplicRemoval != drmNO) {
      for (i = 0; i < iNrValidPntsA; i++)  {   
        aPtSA[i].crdXY = cPointsA[i];  // store each point coord in temp struct
        aPtSA[i].fDup  = false;       // including duplicate flag for removal
        aPtSA[i].rAttrib = rAttribA[i];// and the resp attrib value
      }
      long iDuplicates, k;
      bool fAverageAttrib = (duplicRemoval==1); 
      // perhaps we should offer this as user choice ??
  
      double rD2, rTol2 = rTolerance * rTolerance; 
      // minimum distance of distinct points 
      double rSumOfAttrib; 
      k = 0;
      for (i = 0; i < iNrValidPntsA; ++i) {
        iDuplicates = 0;
        if (aPtSA[i].fDup) continue;
        rSumOfAttrib = aPtSA[i].rAttrib;
        for (j = i + 1; j < iNrValidPntsA; ++j) {
          rD2 = dis.rDistance2(aPtSA[i].crdXY, aPtSA[j].crdXY);
          if (rD2 < rTol2) {  // Pntj coincides with Pnti
            aPtSA[j].fDup = true;
            if (fAverageAttrib)  {
              rSumOfAttrib += aPtSA[j].rAttrib;
              ++iDuplicates;
            }
          }         
        } 
        rAttribA[k]  = rSumOfAttrib / (iDuplicates + 1);
        cPointsA[k] = aPtSA[i].crdXY;  
        ++k;
      }
      if (trq.fUpdate(iNrValidPntsA, iNrValidPntsA))
        return false;
      iNrValidPntsA = k;
    } // end of iDuplicRemovalA and possible averaging of attrib_valuesA

    if (duplicRemoval != drmNO) {
      for (i = 0; i < iNrValidPntsB; i++)  {   
        aPtSB[i].crdXY = cPointsB[i];  // store each point coord in temp struct
        aPtSB[i].fDup  = false;       // including duplicate flag for removal
        aPtSB[i].rAttrib = rAttribB[i];// and the resp attrib value
      }
      long iDuplicates, k;
      bool fAverageAttrib = (duplicRemoval==1); 
      // perhaps we should offer this as user choice ??
  
      double rD2, rTol2 = rTolerance * rTolerance; 
      // minimum distance of distinct points 
      double rSumOfAttrib; 
      k = 0;
      for (i = 0; i < iNrValidPntsB; ++i) {
        iDuplicates = 0;
        if (aPtSB[i].fDup) continue;
        rSumOfAttrib = aPtSB[i].rAttrib;
        for (j = i + 1; j < iNrValidPntsB; ++j) {
          rD2 = dis.rDistance2(aPtSB[i].crdXY, aPtSB[j].crdXY);
          if (rD2 < rTol2) {  // Pntj coincides with Pnti
            aPtSB[j].fDup = true;
            if (fAverageAttrib)  {
              rSumOfAttrib += aPtSB[j].rAttrib;
              ++iDuplicates;
            }
          }         
        } 
        rAttribB[k] = rSumOfAttrib / (iDuplicates + 1);
        cPointsB[k] = aPtSB[i].crdXY;  
        ++k;
      }
      if (trq.fUpdate(iNrValidPntsB, iNrValidPntsB))
          return false;
      iNrValidPntsB = k;
    } // end of iDuplicRemovalB and possible averaging of attrib_valuesB
  }

  trq.SetText(TR("Finding point pair distances"));
  if (iNrValidPntsA < iMinNrPoints || iNrValidPntsB < iMinNrPoints) {
  ///  pms->FillWithUndef();
  ///if (iDim > iVPc) { 
    long iMin = min(iNrValidPntsA, iNrValidPntsB);
    String s(TR("Too few valid points (%li), at least %i needed.").c_str(), iMin, iMinNrPoints);
    throw ErrorObject(WhatError(s, errMapCoKriging+13), fnObj);
  }    
  else  {
    double r5LD2 = 5 * rLimDist * rLimDist; // pairs to be rejected 
                      // (lag is longer than diameter of limiting circle
    double rD2; // pointpair distance squared
    double rElemij;
    Array<Array<double> > aa_AA;// inter_predictand semivar_values (local Array of Arrays)
    aa_AA.Resize(iNrValidPntsA);  // upper left submatrix G11
    for (i = 0; i < iNrValidPntsA; ++i) 
      aa_AA[i].Resize(iNrValidPntsA);
    for (i = 0; i < iNrValidPntsA; ++i) 
      for (j = 0; j < iNrValidPntsA; ++j) 
        aa_AA[i][j] = rUNDEF;
    for (i = 0; i < iNrValidPntsA; ++i) {
      for (j = i + 1; j < iNrValidPntsA; ++j) {
        rD2 = dis.rDistance2(cPointsA[i], cPointsA[j]);
        if (rD2 > r5LD2) continue; // this pair is not needed
        rElemij = svmA.rCalc(sqrt(rD2));
        aa_AA[i][j] = rElemij;
        aa_AA[j][i] = rElemij; //lower triangular mat is superfluous
      }                       // matCoKrig is filled using upper one   
    }  
    Array<Array<double> > aa_BB;//inter_covariable semivar_values (local Array of Arrays)
    aa_BB.Resize(iNrValidPntsB);  // lower right submatrix G22
    for (i = 0; i < iNrValidPntsB; ++i) 
      aa_BB[i].Resize(iNrValidPntsB);
    for (i = 0; i < iNrValidPntsB; ++i) 
      for (j = 0; j < iNrValidPntsB; ++j) 
        aa_BB[i][j] = rUNDEF;  
    for (i = 0; i < iNrValidPntsB; ++i) {
      for (j = i + 1; j < iNrValidPntsB; ++j) {
        rD2 = dis.rDistance2(cPointsB[i], cPointsB[j]);
        if (rD2 > r5LD2) continue; // this pair is not needed
        rElemij = svmB.rCalc(sqrt(rD2));
        aa_BB[i][j] = rElemij;
        aa_BB[j][i] = rElemij;//lower triangular mat is superfluous
      }                       // matCoKrig is filled using upper one  
    }
    Array<Array<double> > aa_AB;//predictand_covariable crossvar_values (loc Array of Arrays)
    aa_AB.Resize(iNrValidPntsA);  // upper-right submatrix G12
    for (i = 0; i < iNrValidPntsA; ++i) 
      aa_AB[i].Resize(iNrValidPntsB);
    for (i = 0; i < iNrValidPntsA; ++i) 
      for (j = 0; j < iNrValidPntsB; ++j) 
        aa_AB[i][j] = rUNDEF;  
    for (i = 0; i < iNrValidPntsA; ++i) {
      for (j = 0; j < iNrValidPntsB; ++j) {
        rD2 = dis.rDistance2(cPointsA[i], cPointsB[j]);
        if (rD2 > r5LD2) continue; // this pair is not needed
        rElemij = svmAB.rCalc(sqrt(rD2));
        aa_AB[i][j] = rElemij;
        //aa_BA[j][i] = rElemij; // transpose not needed
      }                     // matCoKrig is filled using upper one  
    } 
		Map mpError;
		if (ptr.fErrorMap()){ // then create Error map:
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
			String sDescr(TR("Kriging Error Map of %S").c_str(), ptr.sName());
			mpError->sDescription = sDescr;
			mpError->CreateMapStore();
			mpError->KeepOpen(true);
		}
    return fCoKriging(aa_AA, aa_BB, aa_AB, mpError);
  }
  return true;
}

String MapCoKriging::sExpression() const
{
  String sDuplicRemoval;
  switch (duplicRemoval) {
    case drmNO: sDuplicRemoval = "no"; break;
    case drmAVERAGE:  sDuplicRemoval = "average"; break;
    case drmFIRSTVAL: sDuplicRemoval = "firstval"; break;
  }
	
	String sErrorMap = "1";
	if (ptr.fErrorMap()) {
		if (!fCIStrEqual(String("%S_Error", fnObj.sFile), ptr.fnErrorMap().sFile))
			sErrorMap = ptr.fnErrorMap().sShortName(false);
	}
	else
		sErrorMap = "0";
	
	String sDistanceMethod;
	if (m_distMeth == Distance::distSPHERE)
		sDistanceMethod = String("sphere");
	else
		sDistanceMethod = String("plane");

  return String("MapCoKriging(%S,%S,%S,%S,%S,%S,%.2f,%S,%li,%li,%S,%.4f,%S)",
                 pmp->sName(false, fnObj.sPath()),
                 pmpB->sName(false, fnObj.sPath()),
                 gr()->sName(false, fnObj.sPath()),
                 svmA.sExpression(),
                 svmB.sExpression(),
                 svmAB.sExpression(),
                 rLimDist,
                 sErrorMap,
                 iMinNrPoints,
                 iMaxNrPoints,
                 sDuplicRemoval,
                 rTolerance,
								 sDistanceMethod);
}
