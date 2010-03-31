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
/* MapMovingAverage
   Copyright Ilwis System Development ITC
   december 1995, by Dick Visser
	Last change:  WK   11 Sep 97    6:58 pm
*/
#include "Applications\Raster\MAPMVAVG.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"
#include "Engine\SpatialReference\Cslatlon.h"

IlwisObjectPtr * createMapMovingAverage(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapMovingAverage::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapMovingAverage(fn, (MapPtr &)ptr);
}

#define EPS10 1.0e-10
#define EPS20 1.0e-20

static const char * sWeightFunc[] = { "InvDist", "Linear", 0 };

const char* MapMovingAverage::sSyntax() {
  return "MapMovingAverage(pntmap,georef,weightfunc[,plane|sphere])";
}

static int iFind(const String& s, const char* sArr[])
{
  int i = 0;
  while (sArr[i]) {
    if (_strnicmp(sArr[i], s.scVal(), s.length()) == 0)
      return i;
    i++;
  }
  return shUNDEF;
}

class DATEXPORT ErrorWeightFunc: public ErrorObject
{
public:
  ErrorWeightFunc(const String& sWeightFunc, const WhereError& where)
  : ErrorObject(WhatError(String(SMAPErrInvalidWeightFunction.scVal(), sWeightFunc),
                errMapMovingAverage), where) {}
};

static void WeightFuncError(const String& sWeightFunc, const FileName& fn)
{
  throw ErrorWeightFunc(sWeightFunc, fn);
}

class DATEXPORT ErrorWeightFuncExpr: public ErrorExpression
{
public:
  ErrorWeightFuncExpr(const String& sExpr, const char* sWeightFunc)
  : ErrorExpression(sExpr, String("%s(exp,limdist)", sWeightFunc).scVal()) {}
};

static void WeightFuncExprError(const String& sExpr, const String& sWeightFunc)
{
  throw ErrorWeightFuncExpr(sExpr, sWeightFunc.scVal());
}

MapMovingAverage* MapMovingAverage::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
	short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  //if (!IlwisObjectPtr::fParseParm(sExpr, as))
	if (iParms > 4 || iParms < 1)
    ExpressionError(sExpr, sSyntax());
  PointMap pmp(as[0], fn.sPath());
  GeoRef gr(as[1], fn.sPath());
  String sWeightFnc;
  sWeightFnc = IlwisObjectPtr::sParseFunc(as[2]);
  WeightFuncType wft = (WeightFuncType)iFind(sWeightFnc, sWeightFunc);
  if (wft == shUNDEF)
    WeightFuncError(sWeightFnc, fn);
  Array<String> asd(2);
  if (!IlwisObjectPtr::fParseParm(as[2], asd))
    WeightFuncExprError(as[2], sWeightFnc);
  double rWeightExp = asd[0].rVal();
  if (rWeightExp == rUNDEF)
    WeightFuncExprError(as[2], sWeightFnc);
  double rLimD = asd[1].rVal();
  if (rLimD == rUNDEF || rLimD < EPS10)
    WeightFuncExprError(as[2], sWeightFnc);
	bool fSphericDist = false; //default
	if ((iParms == 4) && fCIStrEqual("sphere", as[3]))
		fSphericDist = true;
  return new MapMovingAverage(fn, p, pmp, gr, wft, rWeightExp, rLimD, fSphericDist);
}

MapMovingAverage::MapMovingAverage(const FileName& fn, MapPtr& p)
: MapFromPointMap(fn, p)
{
  String sWeightFnc;
  ReadElement("MapMovingAverage", "WeightFunction", sWeightFnc);
  wft = (WeightFuncType)iFind(sWeightFnc, sWeightFunc);
  if (wft == shUNDEF) {
    ErrorWeightFunc(sWeightFnc, fn).Show();
    wft = wfEXACT;
  }
  ReadElement("MapMovingAverage", "WeightExponent", rWeightExp);
  ReadElement("MapMovingAverage", "LimitingDistance", rLimDist);
  objdep.Add(gr().ptr());
  Init();
}

MapMovingAverage::MapMovingAverage(const FileName& fn, MapPtr& p, const PointMap& pmp, const GeoRef& gr,
                                   WeightFuncType wftp, double rWeightExponent, double rLimDst, const bool fSpheric)
: MapFromPointMap(fn, p, pmp, gr), wft(wftp), rWeightExp(rWeightExponent), rLimDist(rLimDst)
{
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, errMapMovingAverage+2);
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapMovingAverage+1);
  if (!cs()->fConvertFrom(pmp->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapMovingAverage+3);
  fNeedFreeze = true;
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	if (fSpheric)
		m_distMeth = Distance::distSPHERE;
	else
		m_distMeth = Distance::distPLANE;
  Init();
}

MapMovingAverage::~MapMovingAverage()
{
}

void MapMovingAverage::Store()
{
  MapFromPointMap::Store();
  WriteElement("MapFromPointMap", "Type", "MapMovingAverage");
  WriteElement("MapMovingAverage", "WeightFunction", sWeightFunc[wft]);
  WriteElement("MapMovingAverage", "WeightExponent", rWeightExp);
  WriteElement("MapMovingAverage", "LimitingDistance", rLimDist);
}

void MapMovingAverage::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "MapMovingAverage";
  htpFreeze = htpMapMovingAverageT;
}

double MapMovingAverage::rInvDist(double rDis)
{ 
	double rLimit = rLimDist;
  if ((rDis < EPS20) || (rDis > rLimit))
    return 0;
  double rX = rLimit / rDis ; // reduced distance inverted
  return pow(abs(rX), rWeightExp) - 1; // w = (1/d)^n - 1
} 

double MapMovingAverage::rLinDecr(double rDis)
{
	double rLimit = rLimDist;
  if (rDis < EPS10)
    return 1;
  if (rDis > rLimit)
    return 0;
  double rX = rDis / rLimit; // reduced distance
  return 1 - pow(abs(rX), rWeightExp); // w = 1 - d^n
} 

bool MapMovingAverage::fFreezing()
{
  iNrPoints = pmp ->iFeatures();
   // determine size of map in row's and col's
  RowCol rcMrc = gr()->rcSize();
  long iMRow = rcMrc.Row;
  long iMCol = rcMrc.Col;
  RealBuf rBufOut(iMCol);
  bool fTransformCoords = cs() != pmp->cs();
	Distance dis = Distance(pmp->cs(), m_distMeth);
	double rLimD = rLimDist; //local substit
  double rMinDist = rLimDist * 1.0e-10; // minimal distance taken into account
  double rLimDist2 = rLimD * rLimD;
  trq.SetText(SMAPTextCalculating);
  trq.SetTitle(SMAPTextMapMovingAverage);
  iNrValidPnts = 0;   // valid point counter
  cPoints.Resize(iNrPoints);
  rAttrib.Resize(iNrPoints);
  // collect valid points from pointmap
  // for possible conversion of crds and validity check
  // and count them again (iNrValidPnts):
  for (long i = 0; i < iNrPoints; i++)  {   
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
  for (long iRc = 0; iRc < iMRow; iRc++)  {
    if (trq.fUpdate(iRc, iMRow))
      return false;
    for (long iCc = 0; iCc < iMCol; iCc++)  {
      if (trq.fAborted())
        return false;
      Coord  crdRC = gr()->cConv(RowCol(iRc, iCc));
      double rSumW   = 0.0;     // Sum Weight's
      double rSumWtA = 0.0;     // Sum of Weight times atrribute
      double rW;
			double rDis2;
      // look in point list
      for (long iPc = 0; iPc < iNrValidPnts; iPc++)  {
				rDis2 = dis.rDistance2(crdRC, cPoints[iPc]);
				
        if (rDis2 < rMinDist && wft == wfEXACT ) {
          rSumWtA = rAttrib[iPc];  // only 1 point exactly in center of estimated
          rSumW = 1;               // pixel contributes to its value  
					break;                   // no more points, even if they
        }													// coincide with this first-found point 
        else if (rDis2 < rLimDist2)  { // point inside lim circle
          if(wft == wfEXACT)
            rW = rInvDist(sqrt(rDis2)); 
          else
            rW = rLinDecr(sqrt(rDis2));
          rSumW   += rW;
          rSumWtA += rW * rAttrib[iPc]; 
        }
      }
      double rV;
      if (rSumW == 0.0)
        rV = rUNDEF;
      else
        rV = rSumWtA / rSumW;  // apply normalized weights
        rBufOut[iCc] = rV;
    }
    pms->PutLineVal(iRc,rBufOut);
  }

  return true;
}

String MapMovingAverage::sExpression() const
{
	String sDistanceMethod;;
	if (m_distMeth == Distance::distSPHERE)
		sDistanceMethod = String("sphere");
	else
		sDistanceMethod = String("plane");
  return String("MapMovingAverage(%S,%S,%s(%f,%f),%S)", pmp->sNameQuoted(true, fnObj.sPath()),
                 gr()->sNameQuoted(true, fnObj.sPath()),
                 sWeightFunc[wft], rWeightExp, rLimDist,
								 sDistanceMethod);
}

bool MapMovingAverage::fDomainChangeable() const
{
  return true;
}

bool MapMovingAverage::fValueRangeChangeable() const
{
  return true;
}




