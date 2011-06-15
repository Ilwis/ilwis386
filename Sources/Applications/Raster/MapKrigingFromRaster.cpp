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
/* MapKrigingFromRaster
   october 1999, Jan Hendrikse
   Copyright Ilwis System Development ITC
	Last change:  JH   13 Jan 100    6:22 pm
*/
#include "Headers\toolspch.h"
#include "Applications\Raster\MapKrigingFromRaster.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Engine\Base\Algorithm\Octantmatrix.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"
#define EPS10 1.0e-10

IlwisObjectPtr * createMapKrigingFromRaster(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapKrigingFromRaster::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapKrigingFromRaster(fn, (MapPtr &)ptr);
}

const char* MapKrigingFromRaster::sSyntax() {
  return  "MapKrigingFromRaster(map,semivariogram[,radius[,units(m|p)\n"
          "[,errormap[,MinNrSamples[,MaxNrSamples]]]]])";
}

static void SameNameErrMapError(const FileName& fn)
{
  throw ErrorObject(WhatError(String(SMAPErrNameErrMapAlreadyUsed), errMapKrigingFromRaster), fn);
}
static void NrParamKrFromRas(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrNrParamKrFromRas), errMapKrigingFromRaster +17), fn);
}

static void RadiusNotPos(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrRadiusNotPos), errMapKrigingFromRaster +8), fn);
}
static void PixSizeNotDefined(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrPixSizeNotDefined), errMapKrigingFromRaster +7), fn);
}
static void RadiusNotLargeEnough(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrRadiusNotLargeEnough), errMapKrigingFromRaster +6), fn);
}
static void RadiusTooLarge(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrRadiusTooLargeRas), errMapKrigingFromRaster +1), fn);
}
static void RadiusUnitsWrong(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrRadiusUnitsWrong), errMapKrigingFromRaster +5), fn);
}
static void MinNrKrigingSamples(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrMinNrKrigingSamples), errMapKrigingFromRaster +2), fn);
}
static void MaxNrKrigingSamples(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrMaxNrKrigingSamples), errMapKrigingFromRaster +3), fn);
} 
static void IllegalNameErrMap(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrIllegalNameErrMap), errMapKrigingFromRaster + 4), fn);
}
static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

bool MapKrigingFromRaster::fValueRangeChangeable() const
{
  return true;
}

bool MapKrigingFromRaster::fDomainChangeable() const
{
  return true;
}

bool MapKrigingFromRaster::fGeoRefChangeable() const
{
  return false;
}

ValueRange MapKrigingFromRaster::vrDefault(const Map& mp)
{
  RangeReal rr = mp->rrMinMax(BaseMapPtr::mmmNOCALCULATE);
  if (!rr.fValid())
    rr = mp->dvrs().rrMinMax();
	double rRangeWidth = abs(rr.rHi() - rr.rLo());
  double rLowestOut = rr.rLo() - rRangeWidth;
  double rHighestOut = rr.rHi() + rRangeWidth;
  double rStep = mp->dvrs().rStep()/10.0;
  return ValueRange(rLowestOut, rHighestOut, rStep);
}

double MapKrigingFromRaster::rDefaultRadius(const Map& mp)
{
  double rPSize = mp->gr()->rPixSize();
	int h = mp->iLines();
  int w = mp->iCols();
  int iRad = min(10, min(h, w)); // default of search Radius (max = 10 pixels)
  return iRad * rPSize;;
}

MapKrigingFromRaster* MapKrigingFromRaster::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 7 || iParms < 2)
    NrParamKrFromRas(fn);  
  Map mp(as[0], fn.sPath());
  if (mp->gr()->fGeoRefNone())
    GeoRefNoneError(mp->fnObj, errMapKrigingFromRaster + 4);
	double rPSize = mp->gr()->rPixSize();
	if (rPSize == rUNDEF || rPSize < EPS10)
		PixSizeNotDefined(fn);
  SemiVariogram sv(as[1]);
	FileName fnErrMp;
  int h = mp->iLines();
  int w = mp->iCols();
	RadiusUnits ru = ruMETERS;      // default units
  int iRad = 10;// = min(10, min(h, w)); // default of search Radius (max = 10 pixels)
	double rRad = rDefaultRadius(mp); //iRad * rPSize;  // same default in meters
	if (iParms > 2) {
		rRad = as[2].rVal();
		if (rRad < EPS10)
			RadiusNotPos(fn);
	}
	if (iParms == 3) {
      // without unit specification, rRad in meters
		iRad = (int)(rRad / rPSize); // radius converted from meters to pixels for size check
  }
  if (iParms > 3) { // with meters or pixels as units  
		rRad = as[2].rVal(); 
		if (fCIStrEqual("m", as[3])) {//||fCIStrEqual("meter", as[4]))		
		  iRad = (int)(rRad / rPSize); // radius converted from meters to pixels for size check
		}
    else if (fCIStrEqual("p", as[3])) {
      ru = ruPIXELS;
			iRad = (int)rRad;
		}
		else 
			RadiusUnitsWrong(fn);
	}
	if (iRad < 1)							// size check inn pixel units
    RadiusNotLargeEnough(fn); 
  else if (iRad > 40)
    RadiusTooLarge(fn); // never a radius longer than 40 pixels	

  if (iParms > 4) {
		String sErrMap = as[4];
    if ((sErrMap != "") && (sErrMap != "0"))
		{
			fnErrMp = fn;// ErrorMap name is made using KrigMap name
      fnErrMp.sFile &= "_Error"; //
      fnErrMp.sExt = ".mpr";
			if (fnErrMp.fExist())
        FileAlreadyExistError(fnErrMp);
      if (!fnErrMp.fValid())  
        IllegalNameErrMap(fnErrMp);  
    }  
  }
  int iMin = 6L;   // default sample size limits
  int iMax = 16L;
  if (iParms > 5) {
    iMin = as[5].iVal();
    if (iMin < 1)
      MinNrKrigingSamples(fn);
  }
  if (iParms > 6) {
    iMax = as[6].iVal();
    if (iMax < iMin || iMax > 100)
      MaxNrKrigingSamples(fn);
  }
  //else
  //  ExpressionError(sExpr, sSyntax());
  return new MapKrigingFromRaster(fn, p, mp, sv, rRad, ru, fnErrMp, iMin, iMax);
}

MapKrigingFromRaster::MapKrigingFromRaster(const FileName& fn, MapPtr& ptr)
: MapFromMap(fn, ptr)
{
  svm = SemiVariogram(fnObj, "MapKrigingFromRaster");
	ReadElement("MapKrigingFromRaster", "Radius", rRadius);
	String sRadiusUnits;
	ReadElement("MapKrigingFromRaster", "RadiusUnits", sRadiusUnits);
	if (fCIStrEqual("p", sRadiusUnits)) 
	  ruUnits = ruPIXELS; 
	else 
    ruUnits = ruMETERS; 
	//ReadElement("MapKrigingFromRaster", "RadiusUnits", ruUnits);
	ReadElement("MapKrigingFromRaster", "ErrorVarianceMap", ptr.fnErrorMap());
  ReadElement("MapKrigingFromRaster", "MinNrSamples", iMinNrSamples);
  ReadElement("MapKrigingFromRaster", "MaxNrSamples", iMaxNrSamples);
  Init();
}  

MapKrigingFromRaster::MapKrigingFromRaster(const FileName& fn, MapPtr& ptr,
                       const Map& mp,
                       const SemiVariogram& sv,
											 const double rRad,
											 RadiusUnits ru, 
											 const FileName& fnErrM,
                       const int iMin,
                       const int iMax)
: MapFromMap(fn, ptr, mp),
  svm(sv),
	//fnErrorMap(fnErrMap),
  rRadius(rRad),
	ruUnits(ru),
  iMinNrSamples(iMin),
  iMaxNrSamples(iMax)
{
  if (gr()->fGeoRefNone())
    throw ErrorGeoRefNone(gr()->fnObj, errMapKrigingFromRaster+1);
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapKrigingFromRaster+2);
  DomainValueRangeStruct dvrs(vrDefault(mp));
  SetDomainValueRangeStruct(dvrs);
  fNeedFreeze = true;
  objdep.Add(gr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.SetErrorMap(fnErrM);
  Init();
	Store(); // stores this Kriging map on disk before mpKrigingError (see next lines) looks for it 
	String sExpr("MapComputedElsewhere(%S)",ptr.sNameQuoted());
	if (ptr.fErrorMap()) {// open Error map:
		Map mpComputedElsewhere(ptr.fnErrorMap(), sExpr);
		String sDescr(SMAPTextKrigingErrorMap_S.scVal(), ptr.sName());
    mpComputedElsewhere->sDescription = sDescr;
		mpComputedElsewhere->Store();
  }
}

MapKrigingFromRaster::~MapKrigingFromRaster()
{
}

void MapKrigingFromRaster::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapKrigingFromRaster");
  svm.Store(fnObj, "MapKrigingFromRaster");
	if (ptr.fnErrorMap().sFile.length() != 0)
    WriteElement("MapKrigingFromRaster", "ErrorVarianceMap", ptr.fnErrorMap());
  WriteElement("MapKrigingFromRaster", "Radius", rRadius);
	String sRadiusUnits;
	switch (ruUnits) {
		case ruPIXELS: sRadiusUnits = "p"; break;
		case ruMETERS: sRadiusUnits = "m"; break;
  } 
  WriteElement("MapKrigingFromRaster", "RadiusUnits", sRadiusUnits);
	WriteElement("MapKrigingFromRaster", "MinNrSamples", iMinNrSamples);
  WriteElement("MapKrigingFromRaster", "MaxNrSamples", iMaxNrSamples);
}

void MapKrigingFromRaster::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "MapKrigingFromRaster";
  //htpFreeze = "ilwisapp\\kriging_from_raster_algorithm.htm";
}

class RowColLess {
public:
	RowColLess(const DoubleOctantMatrix &om) : omDist(om) {}
	bool operator()(const RowCol& a, const RowCol& b) const
		{ return omDist(a.Col,a.Row) < omDist(b.Col,b.Row);}
private:
	const DoubleOctantMatrix& omDist;
};

bool MapKrigingFromRaster::fFreezing()
{  
	double rDist;
	double rPixelSize = gr()->rPixSize();
	int iRadius;// for algorithm we need search-radius in pixel units
	if (ruUnits == ruMETERS)
	  iRadius = (int)(rRadius / rPixelSize); 
	else
		iRadius = (int)(rRadius); 
	int iWindowSide = 2 * iRadius + 1;
  DoubleOctantMatrix omSemiVars(2 * iRadius);
  DoubleOctantMatrix omDistances(2 * iRadius);
  
	///*********>>> FILL THE 2 OCTANTMATRICES FOR USE IN fKrigingFromRaster()
  for (int a = 0; a < iWindowSide; ++a) {// VarioSurf is isotropic for the time being
    for (int b = 0; b <= a; ++b) { // The georef is assumed having square pixels
									// i.e. the RowCol2Coord is a conformal coord transformation
      rDist = rPixelSize * sqrt((double)(a*a + b*b));  
			omDistances(a,b) = rDist;
      omSemiVars(a,b) = svm.rCalc(rDist);
			//rDist = omSemiVars(a,b); // for testing only
    }
  }  
  //********** >>>> PLACE THE ROWCOLS OF THE SEARCH-WINDOW IN A VECTOR OF ROWCOLS vecRowCol
	//********** >>>> AND SORT THEM
	vector<RowCol> vecRowCol(iWindowSide * iWindowSide);// stores all relative RowCols of the search window 
																											// (same for all windows indep of their position) 
  int iDRow, iDCol;
	int iNummer = 0;
	for (iDCol = -iRadius; iDCol <= iRadius; ++iDCol) {
    for (iDRow = -iRadius; iDRow <= iRadius; ++iDRow) {
			vecRowCol[iNummer].Col = iDCol;
			vecRowCol[iNummer].Row = iDRow;
			//assert(iNummer<iWindowSide*iWindowSide);// for testing only
			iNummer++;
		}
	}
	vector<RowCol> vecRowColLimited(iMaxNrSamples); // stores all RowCols of each search window; 
																									// this changes with the position of the window
	RowColLess rcl(omDistances);
	//RowCol rcTest = vecRowCol[0];// for testing only
	sort(vecRowCol.begin(),vecRowCol.end(), rcl); // sort the vector containing all window RowCols
	//rcTest = vecRowCol[0];// for testing only
	trq.SetText("Calculating");  

  //************ DEFINE MAP AND SEARCH WINDOW DIMENSIONS 

  trq.SetText(SMAPTextCalculating);
 
  RowCol rcMrc = gr()->rcSize();
  int iMaxRow = rcMrc.Row;
  int iMaxCol = rcMrc.Col;
	int iKrigingOrder;
 
  RealMatrix matKrige;
  CVector vecD;
  bool fKrigSingular;
  RealBuf rBufOut(iMaxCol);
  RealBuf rBufOutError(iMaxCol);

	//************* DEFINE ERROR MAP AND ITS PROPERTIES
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
  trq.SetText(SMAPTextKriging_From_Raster);  
  int iR, iC; // the pixels with Defined value ('Visited') used for the estimation
	double rR;  // search radius in map scale (meters)
	if (ruUnits == ruMETERS)
		rR = rRadius;
	else
		rR = rRadius * rPixelSize;
	//***** GO THROUGH ALL OUTPUT ROWCOLS  AND IF THE CORRESP INPUT ROWCOL IS UNDEFINED
	//***** THEN PLACE A SQUARE WINDOW AND SEARCH USABLE INPUT INSIDE iRADIUS AND 'KRIGE' 
	//***** USING A LIMITED NR (iMaxNrSamples) OF INPUT SAMPLES:
	
	for (int iRow = 0; iRow < iMaxRow; iRow++)  {  // estimate ('predict') NonVisited pixels iRow ,iCol
    if (trq.fUpdate(iRow, iMaxRow))
      return false;
    int iMinWinRow = max(iRow - iRadius, 0);//don't go beyond iRow==0
    int iMaxWinRow = min(iRow + iRadius, iMaxRow); 
    for (int iCol = 0; iCol < iMaxCol; iCol++)  {  // estimate NonVisited pixels col by col
      if (trq.fAborted())
        return false;
      double rV = rUNDEF;
      double rVerror = rUNDEF;
      double rWindowCenter = mp->rValue(RowCol(iRow,iCol));
       //value of window's central pixel
      if (rWindowCenter != rUNDEF) { // no estimate needed  
        rV = rWindowCenter;
        rVerror = 0;
      }  
			else {
	      int iMinWinCol = max(iCol - iRadius, 0);//don't go beyond iCol==0
				int iMaxWinCol = min(iCol + iRadius, iMaxCol);//nor beyond iCol==iNrCols-1  

		//*************** RETRIEVE ROWCOLS IN  SEARCH-WINDOW FROM SORTED VECTOR vecRowCol  ************
		//*************** and PUT VALID AND 'RADIUS'-LIMITED INPUT ROWCOLS IN vecRowColLimited ********
		//*************** and SORT THE icountInput VALID-ONES		
				int iCountInput = 0;
				for (int k = 0; k < iWindowSide*iWindowSide; ++k) {
					iDRow = vecRowCol[k].Row;
					iDCol = vecRowCol[k].Col;
					RowCol rcAbs;
					rcAbs.Row = iDRow + iRow; // shift to absolute rc position
					rcAbs.Col = iDCol + iCol;
					if (mp->rValue(rcAbs) == rUNDEF) 
						continue;
					if (omDistances(iDRow,iDCol) > rR) 
						break; 
					vecRowColLimited[iCountInput] = vecRowCol[k];
					iCountInput++;
					if (iCountInput == iMaxNrSamples)
						break;
				}
				if (iCountInput < iMinNrSamples ) {
					rBufOut[iCol] = rBufOutError[iCol] = rUNDEF;
          continue; // estimates rV,rVerror remain rUNDEF if not sufficient input
				}
				vector<RowCol> vecRowColUsed(iCountInput); // stores all RowCols really used
				for (int i= 0; i < iCountInput; i++) 
					vecRowColUsed[i] = vecRowColLimited[i]; 
				sort(vecRowColUsed.begin(),vecRowColUsed.end(), rcl);//sort useful RowCols

				//*************** TAKE  SORTED AND USABLE INPUT RowCols 
				//*************** PUT THE SEMIVAR VALUES PER VALID Rowcol PAIR IN THE KRIGING EQUATIONS
				iKrigingOrder = iCountInput + 1;
				matKrige = RealMatrix(iKrigingOrder);
				vecD = CVector(iKrigingOrder);
				//double rDisTemp, rSemivTemp;
				for (int i= 0; i < iCountInput; i++) {
				  for (int j = i + 1; j < iCountInput; j++) {
				    iDCol = vecRowColUsed[i].Col - vecRowColUsed[j].Col;
					  iDRow = vecRowColUsed[i].Row - vecRowColUsed[j].Row;	
						matKrige(i,j) = matKrige(j,i) = omSemiVars(iDCol, iDRow);
		      }
					vecD(i) = omSemiVars(vecRowColUsed[i].Col, vecRowColUsed[i].Row);
				  matKrige(i, iCountInput) = 1;  // column of 1's
					matKrige(iCountInput, i) = 1;  // row of 1's
					matKrige(i, i) = 0; // diagonal
				}
				matKrige(iCountInput, iCountInput) = 0;//lower right el
				fKrigSingular = matKrige.fSingular();
				if (!fKrigSingular) 
				  matKrige.Invert(); 
				if (fKrigSingular) 
					continue;
				vecD(iCountInput) = 1;
      
			  if (!fKrigSingular) {
					CVector vecW(iKrigingOrder); 
					double rW;
	        for (int i = 0; i < iKrigingOrder; i++) {
	          rW = 0;
		        for (int j = 0; j < iKrigingOrder; j++)
	            rW += matKrige(i,j) * vecD(j);
	          vecW(i) = rW;  // solution of Kriging weights
	        }        // iLimitedPnts weights and Lagr param are now solved
	        double rWeightedSum = 0;
	        //for (iR = iMinWinRow; iR < iMaxWinRow; iR++) 
	        //  for (iC = iMinWinCol; iC < iMaxWinCol; iC++) 
	        //    rWeightedSum += vecW(i) * mp->rValue(RowCol(iR,iC));	
					
					for (int i= 0; i < iCountInput; i++) {	// compute the estimate by wighted mean
					  iC = iCol + vecRowColUsed[i].Col; // shift to absol RowCol position
					  iR = iRow + vecRowColUsed[i].Row;
						if (iR > iMaxWinRow || iC > iMaxWinCol)
							continue;
						if (iR < 0 || iC < 0)
							continue;
					  rWeightedSum += vecW(i) * mp->rValue(RowCol(iR,iC));
					}
					rV = rWeightedSum;
					if (iCountInput > 1) // case of more points than mathem minimum
					{
						rVerror = 0;
						for (int i = 0; i < iKrigingOrder; i++)
						  rVerror += vecW(i) * vecD(i);
						rVerror = sqrt(abs(rVerror));
				  }// else (if nr of points in circle at minimum 1
           // leave rVerror (the error stand deviation) undefined
				}
				// rV = rUNDEF and Verror = rUNDEF remain for this RowCol;
      } // end if estimation needed and matKrige Nonsingular 
      
      rBufOut[iCol] = rV;
      rBufOutError[iCol] = rVerror;
    }
    pms->PutLineVal(iRow,rBufOut);
    if (mpError.fValid())
      mpError->PutLineVal(iRow,rBufOutError);
  } 
  return true;
}  

String MapKrigingFromRaster::sExpression() const
{	
	String sRadiusUnits;
	double rRadiusInMetersOrPixels = rRadius;
  switch (ruUnits) {
    case ruPIXELS: sRadiusUnits = "p"; break;
    case ruMETERS: sRadiusUnits = "m"; break;
	}
	String sErrorMap = "1";
	if (ptr.fErrorMap()) {
		if (!fCIStrEqual(String("%S_Error", fnObj.sFile), ptr.fnErrorMap().sFile))
			sErrorMap = ptr.fnErrorMap().sShortName(false);
	}
	else
		sErrorMap = "0";
  return String("MapKrigingFromRaster(%S,%S,%.2f,%S,%S,%i,%i)",
                 mp->sName(false, fnObj.sPath()),
                 svm.sExpression(),
							   rRadiusInMetersOrPixels,
								 sRadiusUnits,
								 sErrorMap,
                 iMinNrSamples,
                 iMaxNrSamples);
}
