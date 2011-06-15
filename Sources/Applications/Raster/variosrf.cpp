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
/* MapVariogramSurfacePnt
   Copyright Ilwis System Development ITC
   june 1998, by Jan Hendrikse
	Last change:  WK   29 Jun 99    3:55 pm
*/
#include "Headers\toolspch.h"
#include "Applications\Raster\variosrf.h"
#include "Engine\SpatialReference\Grdiff.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\Algorithm\Realmat.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"

#define EPS10 1.0e-10
#define EPS20 1.0e-20

IlwisObjectPtr * createMapVariogramSurfacePnt(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapVariogramSurfacePnt::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapVariogramSurfacePnt(fn, (MapPtr &)ptr);
}

const char* MapVariogramSurfacePnt::sSyntax() {
  return "MapVariogramSurfacePnt(pntmap[,lagspacing [,nrlags]])\n";
  //"MapVariogramSurfaceRas(pntmap[,lagspacing [,nrlags]])\n";
}

static void PointMapEmpty(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrPointMapEmpty), errMapVariogramSurfacePnt), fn);
}
static void LagSpacingNotPos(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrLagSpacingNotPos), errMapVariogramSurfacePnt +1), fn);
}
static void LagSpacingTooLarge(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrLagSpacingTooLarge), errMapVariogramSurfacePnt +2), fn);
}
static void NrLagsNotPos(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrNrLagsNotPos), errMapVariogramSurfacePnt +3), fn);
}
static void NrLagsTooLargePnt(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrNrLagsTooLargePnt), errMapVariogramSurfacePnt +4), fn);
}
static void MostLagsTooLong(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrMostLagsTooLong), errMapVariogramSurfacePnt +5), fn);
}

double MapVariogramSurfacePnt::rDefaultLagSpacing(const PointMap& pmap)
{
  double w = pmap->cb().width();
  double h = pmap->cb().height();
  double m = min(w,h);
  double rArea = w * h;
  double iNrP = pmap->iFeatures();
  if (iNrP == 0)
    return rUNDEF;
  else
    return sqrt(rArea/iNrP)/2; //default Lag spacing equals
                         // sqroot(inverse of avg point_density) / 2
}

ValueRange MapVariogramSurfacePnt::vrDefault(const PointMap& pmap)
{
  RangeReal rr = pmap->rrMinMax(BaseMapPtr::mmmNOCALCULATE);
  if (!rr.fValid())
    rr = pmap->dvrs().rrMinMax();
  double rLowestOut = 0.0;
  double rHighestOut = (rr.rHi() - rr.rLo()) * (rr.rHi() - rr.rLo()) / 2.0;
  double rStep = pmap->dvrs().rStep()/100.0;
  return ValueRange(rLowestOut, rHighestOut, rStep);
}

MapVariogramSurfacePnt* MapVariogramSurfacePnt::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 3 || iParms < 1)
    throw ErrorExpression(sExpr, sSyntax()); 
  PointMap pmap(as[0], fn.sPath());
  double iNrP = pmap->iFeatures();
  double rSpacing; 
  if (iNrP == 0)
    PointMapEmpty(fn); 
  else
    rSpacing = rDefaultLagSpacing(pmap);
  double w = pmap->cb().width();
  double h = pmap->cb().height();
  double m = min(w,h);
  double rDiagonal = sqrt(w*w + h*h);
  if (iParms > 1) {
    rSpacing = as[1].rVal();
    if ( rSpacing < EPS10 )
      LagSpacingNotPos(fn);
    else if (rSpacing > rDiagonal) //diagonal of point map
      LagSpacingTooLarge(fn);
  }

  long iLags = (long)m/rSpacing/2.0; // if user doesnt supply iLags
  if (iLags > 10) iLags = 10;      // maximum default of iLags
  if (iParms > 2) {
    //double rL = double(as[2]);
    //if (floor(rL) != rL)
    //  NrLagsNotInteger(fn);
    iLags = as[2].iVal();
    if (iLags < 1)
      NrLagsNotPos(fn);
    else if (iLags > 45)
      NrLagsTooLargePnt(fn);
    else if (iLags * rSpacing > 2.0 * rDiagonal)
      MostLagsTooLong(fn);
  }
  return new MapVariogramSurfacePnt(fn, p, pmap, rSpacing, iLags);
}

MapVariogramSurfacePnt::MapVariogramSurfacePnt(const FileName& fn, MapPtr& p)
: MapFromPointMap(fn, p)
{
  ReadElement("MapVariogramSurfacePnt", "LagSpacing", rLagSpacing);
  ReadElement("MapVariogramSurfacePnt", "NrLags", iNrLags);
  GeoRef grNew;
  grNew.SetPointer(new GeoRefDifferential(FileName(), rLagSpacing, iNrLags));
  ptr.SetGeoRef(grNew);
  ptr.SetSize(grNew->rcSize());
  Init();
}

MapVariogramSurfacePnt::MapVariogramSurfacePnt(const FileName& fn, MapPtr& p, 
            const PointMap& pmap, const double rSpacing, const long iLags)
: MapFromPointMap(fn, p, pmap, GeoRef(FileName("none"))), 
  rLagSpacing(rSpacing), iNrLags(iLags)
{   
  	if (!dvrs().fValues()) {
  	  String sFileName = fnObj.sPath();
  	  String sDom = dm()->sName(true, sFileName);
  	  String sTN = sTypeName();
      ValueDomainError(sDom, sTN, errMapVariogramSurfacePnt +6);
    }  
    
    GeoRef grNew;
  	grNew.SetPointer(new GeoRefDifferential(FileName(), rLagSpacing, iNrLags));
    
  	ptr.SetGeoRef(grNew);
  	ptr.SetSize(grNew->rcSize());
  	// setting default range of output variogrsurface:
    DomainValueRangeStruct dvrs(vrDefault(pmap));
    SetDomainValueRangeStruct(dvrs);
//    SetValueRange(vrDefault(pmap));
  	if (!fnObj.fValid())
    	objtime = objdep.tmNewest();
  	Init();
}

MapVariogramSurfacePnt::~MapVariogramSurfacePnt()
{
}

void MapVariogramSurfacePnt::Store()
{
  MapFromPointMap::Store();
  WriteElement("MapFromPointMap", "Type", "MapVariogramSurfacePnt");
  WriteElement("MapVariogramSurfacePnt", "LagSpacing", rLagSpacing);
  WriteElement("MapVariogramSurfacePnt", "NrLags", iNrLags);
}

String MapVariogramSurfacePnt::sAddInfo() const
{
  String s = String("Average over all data\r\n (mean of all (%i) sampled observations)",iNrValidPnts);
  s &= String("\r\n = %.3f (in data units)", rDataMean);
   s &= "\r\nVariance over all data\r\n (mean of all squared deviations from the average)";
  s &= String("\r\n = %.3f (in squared data units)", rDataVariance);
	s &= "\r\nUnbiased Variance Estimate:";
	s &= String("\r\n = %.3f ", rDataVariance * iNrValidPnts / (iNrValidPnts - 1));
  return s;
}

void MapVariogramSurfacePnt::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "MapVariogramSurfacePnt";
  htpFreeze = "ilwisapp\\variogram_surface_algorithm.htm";
}

bool MapVariogramSurfacePnt::fFreezing()
{
  iNrPoints = pmp ->iFeatures();
   // mapvariogrsurface has size derived from mapfpnt
   // determine its size in row's and col's base on usersupplied georef
  RowCol rcMrc = gr()->rcSize();
  long iNrOfRows = rcMrc.Row;
  long iNrOfCols = rcMrc.Col;
  long iMidRow = (rcMrc.Row-1)/2;
  long iMidCol = (rcMrc.Col-1)/2;
  RealBuf rBufOut(iNrOfCols);
  double rPixelSize = gr()->rPixSize();
  
  trq.SetTitle("MapVariogramSurfacePnt");
  iNrValidPnts = 0;   // valid point counter
  cPoints.Resize(iNrPoints);
  rAttrib.Resize(iNrPoints);
	rDataMean = 0.0; 
  rDataVariance = 0.0;
  double rDataSquared = 0.0;
  // collect valid points from pointmap
  // for validity check
  // and count them again (iNrValidPnts):
  for (long i = 0; i < iNrPoints; i++)  {   
    double rAtr = pmp->rValue(i);
    Coord cVal = pmp->cValue(i); 
    if (cVal == crdUNDEF || rAtr == rUNDEF)
       continue;
    cPoints[iNrValidPnts] = cVal; 
    rAttrib[iNrValidPnts] = rAtr;
		rDataSquared += rAtr * rAtr;
    rDataMean += rAtr;
    iNrValidPnts++;
  }  
	if (iNrValidPnts) {
    rDataMean = rDataMean / iNrValidPnts;
    rDataVariance = rDataSquared / iNrValidPnts - rDataMean * rDataMean;
  }  
  else {
    rDataMean = rUNDEF;
    rDataVariance = rUNDEF;
  }
  Coord crdMin, crdMax;
  pmp->Bounds(crdMin, crdMax);
  long j, iDifX, iDifY;
  double rDifZ, rDifZSquared ;
  RealMatrix rmVarioSurf(iNrOfRows, iNrOfCols);
  LongMatrix imFrequency(iNrOfRows, iNrOfCols);
  for (long i = 0; i < iNrOfRows; i++) {
    for (j = 0; j < iNrOfCols; j++) {
      rmVarioSurf(i,j) = 0.0;
      imFrequency(i,j) = 0L;
    }
  }
  // Input Pointmap is subdivided in adjacent square cells having as size 
  // the output rPixelsize and covering the total map
  // They are numbered with integers iDifX, iDifY  to which
  // lag vectors are rounded to obtain integer x and y components.
  // The difference of each lagvector begin and end point value rDifZ is found
  // This difference is squared, halved and added to the correct lag vector class,
  // contributing to the output pixel at positions 
  // +/- iDifY+iMidRow, +/-iDifX+iMidCol
  trq.SetText("Calculating Squared Differences");
  for (long i = 0; i < iNrValidPnts; i++)  { 
    if (trq.fUpdate(i, iNrValidPnts))
      return false;
    for (j = i + 1; j < iNrValidPnts; j++) { 
      if (trq.fAborted())
        return false;
      
      rDifZ = rAttrib[i] - rAttrib[j];    

      double rDifX = (cPoints[i].x - cPoints[j].x) + rPixelSize/2;
      double rDifY = (- cPoints[i].y + cPoints[j].y) + rPixelSize/2;
      iDifX = (long)floor(rDifX / rPixelSize);
      if (2*abs(iDifX) > iNrOfCols-1) continue;
      iDifY = (long)floor(rDifY / rPixelSize);
      if (2*abs(iDifY) > iNrOfRows-1) continue;
      rDifZSquared  = rDifZ * rDifZ / 2.0;   // cotribution to semivariance
      rmVarioSurf(iDifY+iMidRow,iDifX+iMidCol) += rDifZSquared ;  
      // for pixel in one quadrant
      rmVarioSurf(-iDifY+iMidRow,-iDifX+iMidCol) += rDifZSquared ;  
      // for pixel in opposite quadrant
      imFrequency(iDifY+iMidRow,iDifX+iMidCol)++;  
      // for pixel in one quadrant
      imFrequency(-iDifY+iMidRow,-iDifX+iMidCol)++;  
      // for pixel in opposite quadrant
    }
  }
  // Computed semivariogram values are stored in output raster:
  trq.SetText("Adding Sums of Squares to Surface");
  for (long iRow = 0; iRow < iNrOfRows; iRow++)  { // fill pixels row by row
    if (trq.fUpdate(iRow, iNrOfRows))
      return false;
    for (long iCol = 0; iCol < iNrOfCols; iCol++)  { // fill pixels col by col
      if (trq.fAborted())
        return false;
      if (imFrequency(iRow,iCol) == 0)
        rBufOut[iCol] = rUNDEF;
      else
      {
        double rmV = rmVarioSurf(iRow,iCol);
        double imV = imFrequency(iRow,iCol);
        rBufOut[iCol] = rmV / (double)imV;
      }  
    }
    pms->PutLineVal(iRow,rBufOut);
  } 
	ptr.SetAdditionalInfoFlag(true);
  ptr.SetAdditionalInfo(sAddInfo());
  return true;
}

String MapVariogramSurfacePnt::sExpression() const
{
  return String("MapVariogramSurfacePnt(%S, %.2f, %li)", pmp->sName(true, fnObj.sPath()),
                                                  rLagSpacing, iNrLags);
}

bool MapVariogramSurfacePnt::fDomainChangeable() const
{
  return true;
}

bool MapVariogramSurfacePnt::fValueRangeChangeable() const
{
  return true;
}  

bool MapVariogramSurfacePnt::fGeoRefChangeable() const
{
  return false;
}

