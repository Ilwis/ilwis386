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
/* MapMovingSurface
Copyright Ilwis System Development ITC
december 1995, by Dick Visser
Last change:  WK   21 Jul 97    5:57 pm
*/
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\Raster\MAPMVSRF.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\Algorithm\Realmat.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"
#include "Engine\SpatialReference\Cslatlon.h"

IlwisObjectPtr * createMapMovingSurface(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapMovingSurface::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapMovingSurface(fn, (MapPtr &)ptr);
}

#define EPS10 1.0e-10

static const char * sSurfaceType[] = { "Plane", "Linear2", "Parabolic2", "2", "3", "4", "5", "6", 0 };
static const char * sWeightFunc[] = { "InvDist", "Linear", 0 };

String wpsmetadataMapMovingSurface() {
	WPSMetaData metadata("MapMovingSurface");
	return metadata.toString();
}

ApplicationMetadata metadataMapMovingSurface(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapMovingSurface();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression = MapMovingSurface::sSyntax();

	return md;
}

const char* MapMovingSurface::sSyntax() {
	return "MapMovingSurface(pntmap,georef,surfacetype,weightfunc[,plane|sphere])";
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
		errMapMovingSurface), where) {}
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

class DATEXPORT ErrorSurface: public ErrorObject
{
public:
	ErrorSurface(const String& sSurfaceType, const WhereError& where)
		: ErrorObject(WhatError(String(SMAPErrInvalidSurfaceType_S.scVal(), sSurfaceType), errMapMovingSurface+1), where) {}
};

static void SurfaceError(const String& sSurface, const FileName& fn)
{
	throw ErrorSurface(sSurface, fn);
}

MapMovingSurface* MapMovingSurface::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	//if (!IlwisObjectPtr::fParseParm(sExpr, as))
	if (iParms > 7 || iParms < 1)
		ExpressionError(sExpr, sSyntax());
	PointMap pmp(as[0], fn.sPath());
	GeoRef gr(as[1], fn.sPath());
	SurfaceType sft = (SurfaceType)iFind(as[2], sSurfaceType);
	if (sft == shUNDEF)
		SurfaceError(as[2], fn);
	double rLimD;
	double rWeightExp ;
	int offset = 0;
	WeightFuncType wft;
	String sWeightFnc;
	if ( iParms <= 5) { // oldform
		String sWeightFnc;
		sWeightFnc = IlwisObjectPtr::sParseFunc(as[3]);
		WeightFuncType wft = (WeightFuncType)iFind(sWeightFnc, sWeightFunc);
		if (wft == shUNDEF)
			WeightFuncError(sWeightFnc, fn);
		Array<String> asd(2);
		if (!IlwisObjectPtr::fParseParm(as[3], asd))
			WeightFuncExprError(as[3], sWeightFnc);
		double rWeightExp = asd[0].rVal();
		if (rWeightExp == rUNDEF)
			WeightFuncExprError(as[3], sWeightFnc);
		double rLimD = asd[1].rVal();
	} else { // newform
		wft = wfEXACT;
		sWeightFnc = as[2];
		if ( sWeightFnc.toLower() != "invdist")
			wft = wfNOTEXACT;
		rWeightExp = as[3].rVal();
		rLimD = as[4].rVal();
		offset = 2;
	}
	if (rLimD == rUNDEF || rLimD < EPS10)
		WeightFuncExprError(as[3 + offset], sWeightFnc);
	bool fSphericDist = false; //default
	if ((iParms == 5 + offset) && fCIStrEqual("sphere", as[4 + offset]))
		fSphericDist = true;
	return new MapMovingSurface(fn, p, pmp, gr, sft, wft, rWeightExp, rLimD,fSphericDist);
}

MapMovingSurface::MapMovingSurface(const FileName& fn, MapPtr& p)
: MapFromPointMap(fn, p)
{
	fNeedFreeze = true;
	String sSurf;
	ReadElement("MapMovingSurface", "SurfaceType", sSurf);
	sft = (SurfaceType)iFind(sSurf, sSurfaceType);
	if (sft == shUNDEF) {
		ErrorSurface(sSurf, sTypeName()).Show();
		sft = sfPLANE;
	}
	String sWeightFnc;
	ReadElement("MapMovingSurface", "WeightFunction", sWeightFnc);
	wft = (WeightFuncType)iFind(sWeightFnc, sWeightFunc);
	if (wft == shUNDEF) {
		ErrorWeightFunc(sWeightFnc, sTypeName()).Show();
		wft = wfEXACT;
	}
	ReadElement("MapMovingSurface", "WeightExponent", rWeightExp);
	ReadElement("MapMovingSurface", "LimitingDistance", rLimDist);
	objdep.Add(gr().ptr());
	Init();
}

MapMovingSurface::MapMovingSurface(const FileName& fn, MapPtr& p, const PointMap& pmp, const GeoRef& gr,
								   SurfaceType sftp, WeightFuncType wftp,
								   double rWeightExponent, double rLimDst, bool fSpheric)
								   : MapFromPointMap(fn, p, pmp, gr), sft(sftp), wft(wftp), rWeightExp(rWeightExponent), rLimDist(rLimDst)
{
	if (!dvrs().fValues())
		ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapMovingSurface+2);
	if (gr->fGeoRefNone())
		throw ErrorGeoRefNone(gr->fnObj, errMapMovingSurface+3);
	if (!cs()->fConvertFrom(pmp->cs()))
		IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapMovingSurface+4);

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

MapMovingSurface::~MapMovingSurface()
{
}

void MapMovingSurface::Store()
{
	MapFromPointMap::Store();
	WriteElement("MapFromPointMap", "Type", "MapMovingSurface");
	WriteElement("MapMovingSurface", "SurfaceType", sSurfaceType[sft]);
	WriteElement("MapMovingSurface", "WeightFunction", sWeightFunc[wft]);
	WriteElement("MapMovingSurface", "WeightExponent", rWeightExp);
	WriteElement("MapMovingSurface", "LimitingDistance", rLimDist);
}

void MapMovingSurface::Init()
{
	fNeedFreeze = true;
	sFreezeTitle = "MapMovingSurface";
	htpFreeze = "ilwisapp\\moving_surface_algorithm.htm";
}

double MapMovingSurface::rInvDist(double rDis)
{ 
	double rLimit = rLimDist;
	if (rDis > rLimit)
		return 0;
	if  (rDis < EPS10) 
		return 10;   
	double rX = rLimit / rDis ; // reduced distance inverted
	return pow(abs(rX), rWeightExp) - 1; // w = (1/d)^n - 1
} 

double MapMovingSurface::rLinDecr(double rDis)
{
	double rLimit = rLimDist;
	if ((rDis < EPS10) || (rDis > rLimit))
		return 0;
	double rX = rDis / rLimit; // reduced distance
	return 1 - pow(abs(rX), rWeightExp); // w = 1 - d^n
} 

bool MapMovingSurface::fFreezing()
{
	enum esft{Plane, Lin2, Parab2, o2, o3, o4, o5, o6 };
	long iNrPoints = pmp ->iFeatures();
	RowCol rcMrc = gr()->rcSize();
	long iMaxRow = rcMrc.Row;
	long iMaxCol = rcMrc.Col;
	RealBuf rBufOut(iMaxCol);
	Distance dis = Distance(pmp->cs(), m_distMeth);
	double rLimD = rLimDist; //local substit
	bool fTransformCoords = cs() != pmp->cs();
	double rLimDist2 = rLimD * rLimD;
	trq.SetText(SMAPTextCalculating);
	trq.SetTitle(SMAPTextMapMovingSurface);
	long iNrValidPnts = 0;   // valid point counter
	cPoints.Resize(iNrPoints);
	rAttrib.Resize(iNrPoints);
	long i;
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
	int iDim=0;    // dimension of matrices
	// determine dimension of matrices depending of surface type
	switch (sft) {
case Plane          :{ iDim =  3; break;}
case Lin2           :{ iDim =  4; break;}
case Parab2         :{ iDim =  5; break;}
case o2             :{ iDim =  6; break;}
case o3             :{ iDim = 10; break;}
case o4             :{ iDim = 15; break;}
case o5             :{ iDim = 21; break;}
case o6             :{ iDim = 28; break;}
	};
	CVector rT(iDim);
	CVector rR(iDim);
	RealBuf rX(iNrValidPnts);
	RealBuf rY(iNrValidPnts);
	RealBuf rAt(iNrValidPnts);
	RealBuf rDist(iNrValidPnts);
	bool fWeigtOverflow;

	if (iDim > iNrValidPnts) { 
		String s(SMAPErrNotEnoughPoints_ii.scVal(), iNrValidPnts, iDim);
		throw ErrorObject(WhatError(s, errMapTrendSurface+3), fnObj);
	}  
	for (long iRow = 0; iRow < iMaxRow; iRow++)  {
		if (trq.fUpdate(iRow, iMaxRow))
			return false;
		// start new Row, get coordinate of first point
		for (long iCol = 0; iCol < iMaxCol; iCol++)  {
			if (trq.fAborted())
				return false;
			Coord  crdRC = gr()->cConv(RowCol(iRow, iCol));


			long iLimitedPnts = 0;   // counter of points in limiting circle

			// check if points are within limiting distance and put them in buf's
			for (i = 0; i < iNrValidPnts; i++)  {
				Coord crdPnt_i = cPoints[i];

				double rAttr = rAttrib[i];
				if((crdPnt_i.x==rUNDEF)||(crdPnt_i.y==rUNDEF)||(rAttr==rUNDEF)) continue;
				double rDistan2 = dis.rDistance2(crdPnt_i, crdRC);
				if (rDistan2 > rLimDist2 - EPS10) continue;
				// Yes we have a point within the limiting distance so add it to list
				// Points within EPS10 distance from limiting circle yield an almost zero
				// weight and hence an almost zero row in the matrix rM, which makes the sys-
				// system quasi singular and thus unstable if no redundacy is left.
				rX[iLimitedPnts] = crdPnt_i.x - crdRC.x;// reduce coordinates
				rY[iLimitedPnts] = crdPnt_i.y - crdRC.y;// i.e. shift origin to crdRC
				rAt[iLimitedPnts]= rAttr;
				rDist[iLimitedPnts] = sqrt(rDistan2);
				iLimitedPnts++;
			}
			// if not enough valid points make element undef and goto next point
			if (iLimitedPnts<iDim) {
				rBufOut[iCol] = rUNDEF;
				continue;
			}

			RealMatrix rM(iLimitedPnts,iDim);
			CVector rA(iLimitedPnts);


			for ( i = 0; i < iLimitedPnts; i++)  {

				rA(i) = rAt[i];

				double rXi = rX[i];
				double rYi = rY[i];
				rM(i,0) = 1.0;                 // constant factor
				rM(i,1) = rXi;                 // X
				rM(i,2) = rYi;                 // Y
				if(sft == Plane)
					continue;
				if(sft == Lin2)  {
					rM(i,3) = rXi * rYi;       // X * Y
					continue;
				}
				if(sft == Parab2)  {
					rM(i,3) = rXi * rXi;       // X^2
					rM(i,4) = rYi * rYi;       // Y^2
					continue;
				}
				rM(i,3) = rXi * rXi;         // X^2
				rM(i,4) = rXi * rYi;         // X * Y
				rM(i,5) = rYi * rYi;         // Y^2
				if(sft == o2)
					continue;
				rM(i,6) = rM(i,3) * rXi;    // X^3
				rM(i,7) = rM(i,4) * rXi;    // X^2 * Y
				rM(i,8) = rM(i,4) * rYi;    // X   * Y^2
				rM(i,9) = rM(i,5) * rYi;    // X^3
				if(sft == o3)
					continue;
				rM(i,10) = rM(i,6) * rXi;    // X^4
				rM(i,11) = rM(i,7) * rXi;    // X^3 * Y
				rM(i,12) = rM(i,8) * rXi;    // X^2 * Y^2
				rM(i,13) = rM(i,8) * rYi;    // X   * Y^3
				rM(i,14) = rM(i,9) * rYi;    // Y^4
				if(sft == o4)
					continue;
				rM(i,15) = rM(i,10) * rXi;    // X^5
				rM(i,16) = rM(i,11) * rXi;    // X^4 * Y
				rM(i,17) = rM(i,12) * rXi;    // X^3 * Y^2
				rM(i,18) = rM(i,12) * rYi;    // X^2 * Y^3
				rM(i,19) = rM(i,13) * rYi;    // X^1 * Y^4
				rM(i,20) = rM(i,14) * rYi;    // Y^5
				if(sft == o5)
					continue;
				rM(i,21) = rM(i,15) * rXi;    // X^6
				rM(i,22) = rM(i,16) * rXi;    // X^5 * Y
				rM(i,23) = rM(i,17) * rXi;    // X^4 * Y^2
				rM(i,24) = rM(i,18) * rXi;    // X^3 * Y^3
				rM(i,25) = rM(i,18) * rYi;    // X^2 * Y^4
				rM(i,26) = rM(i,19) * rYi;    // X^1 * Y^5
				rM(i,27) = rM(i,20) * rYi;    // Y^6
			}
			///double rMinDist = rLimD * 1.0e-10;      // minimal distance taken into account
			// take weight factor into account
			for ( i = 0; i < iLimitedPnts; i++)  {
				double rD = rDist[i]/rLimD;
				//if (rD < rMinDist ) rD = rMinDist;
				double rWeight;
				if(wft == wfEXACT)
					rWeight = rInvDist(rD); 
				else
					rWeight = rLinDecr(rD);
				fWeigtOverflow = (abs(rWeight) > 1.0e20); 
				double rAi = rA(i);
				rA(i) = rAi * rWeight;
				//rA(i) *= rWeight;  // this crashes in realmatrix.Cvector class
				for (int k = 0 ; k < iDim; k++) {
					double rMik = rM(i,k);
					rM(i,k) = rMik * rWeight;
				}  
			}
			if (fWeigtOverflow) {
				rBufOut[iCol] = rAt[i];
				continue; 
			}  
			try {
				rR = LeastSquares(rM,rA);
			}

			catch (const ErrorObject & ) {
				rBufOut[iCol] = rUNDEF;
				continue;
			}
			rBufOut[iCol] = rR(0); //= rVal;

		}
		pms->PutLineVal(iRow,rBufOut);
	}
	return true;
}


String MapMovingSurface::sExpression() const
{
	String sDistanceMethod;;
	if (m_distMeth == Distance::distSPHERE)
		sDistanceMethod = String("sphere");
	else
		sDistanceMethod = String("plane");
	return String("MapMovingSurface(%S,%S,%s,%s(%f,%f),%S)", pmp->sNameQuoted(true, fnObj.sPath()),
		gr()->sNameQuoted(true, fnObj.sPath()), sSurfaceType[sft],
		sWeightFunc[wft], rWeightExp, rLimDist,
		sDistanceMethod);
}

bool MapMovingSurface::fDomainChangeable() const
{
	return true;
}

bool MapMovingSurface::fValueRangeChangeable() const
{
	return true;
}




