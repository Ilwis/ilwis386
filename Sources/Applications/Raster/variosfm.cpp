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
/* MapVariogramSurfaceRas
   Copyright Ilwis System Development ITC
   november 1998, by Jan Hendrikse
	Last change:  WK   29 Jun 99    3:54 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "engine\base\system\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\Raster\variosfm.h"
#include "Engine\SpatialReference\Grdiff.H"
#include "Engine\Base\Algorithm\Realmat.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"

#define EPS10 1.0e-10
#define EPS20 1.0e-20

IlwisObjectPtr * createMapVariogramSurfaceRas(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapVariogramSurfaceRas::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapVariogramSurfaceRas(fn, (MapPtr &)ptr);
}

String wpsmetadataMapVariogramSurfaceRas() {
	WPSMetaData metadata("MapVariogramSurfaceRas");
	return metadata.toString();
}

ApplicationMetadata metadataMapVariogramSurfaceRas(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapVariogramSurfaceRas();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotMAPLIST;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapVariogramSurfaceRas::sSyntax();

	return md;
}


const char* MapVariogramSurfaceRas::sSyntax() {
  return "MapVariogramSurfaceRas(map[,nrlags])\n";
}

static void NrLagsNotPos(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrNrLagsNotPos), errMapVariogramSurfaceRas), fn);
}
static void NrLagsTooLargeRas(const FileName& fn) {
  throw ErrorObject(WhatError(String(SMAPErrNrLagsTooLargeRas), errMapVariogramSurfaceRas +1), fn);
}
static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

ValueRange MapVariogramSurfaceRas::vrDefault(const Map& mp)
{
  RangeReal rr = mp->rrMinMax(BaseMapPtr::mmmNOCALCULATE);  // default range of output variogrsurface
  if (!rr.fValid())
    rr = mp->dvrs().rrMinMax();
  double rLowestOut = 0.0;
  double rHighestOut = (rr.rHi() - rr.rLo())*(rr.rHi() - rr.rLo()) / 2.0;
  double rStp = mp->dvrs().rStep()/100.0;
  return ValueRange(rLowestOut, rHighestOut, rStp);
}

MapVariogramSurfaceRas* MapVariogramSurfaceRas::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 2 || iParms < 1)
    throw ErrorExpression(sExpr, sSyntax()); 
  Map map(as[0], fn.sPath());
  if (map->gr()->fGeoRefNone())
    GeoRefNoneError(map->fnObj, errMapVariogramSurfaceRas +3);
  long h = map->iLines(); //->gr()->rcSize().Row;
  long w = map->iCols();
  //double rPixSize = map->rPixelSize();
  long iLags = min(10, min(h, w));      // default of iLags (max = 10)
  if (iParms > 1) {
    long iL = as[1].iVal();
    if (iL < 1)
      NrLagsNotPos(fn);
    else if (iL > 45)
      NrLagsTooLargeRas(fn);
    iLags = min(iL, min(h, w));// never more lags than pixels on a row or col
  }

  return new MapVariogramSurfaceRas(fn, p, map, iLags);
}

MapVariogramSurfaceRas::MapVariogramSurfaceRas(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  ReadElement("MapVariogramSurfaceRas", "NrLags", iNrLags);
  ReadElement("MapVariogramSurfaceRas", "LagSpacing", rLagSpacing);
  GeoRef grNew; 
  //double rLagSpacing = map->gr()->rPixSize();
  grNew.SetPointer(new GeoRefDifferential(FileName(), rLagSpacing, iNrLags));
  ptr.SetGeoRef(grNew);
  ptr.SetSize(grNew->rcSize());
  Init();
}

MapVariogramSurfaceRas::MapVariogramSurfaceRas(const FileName& fn, MapPtr& p, 
            const Map& mp,  const long iLags)
: MapFromMap(fn, p, mp), 
  rLagSpacing(mp->gr()->rPixSize()),
  iNrLags(iLags)
{
  GeoRef grNew; 
  grNew.SetPointer(new GeoRefDifferential(FileName(), rLagSpacing, iNrLags));
  ptr.SetGeoRef(grNew);
  ptr.SetSize(grNew->rcSize());
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapVariogramSurfaceRas +4);
  SetValueRange(vrDefault(mp));
  Init();
}

MapVariogramSurfaceRas::~MapVariogramSurfaceRas()
{
}

void MapVariogramSurfaceRas::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapVariogramSurfaceRas");
  WriteElement("MapVariogramSurfaceRas", "NrLags", iNrLags);
  WriteElement("MapVariogramSurfaceRas", "LagSpacing", rLagSpacing);
}

String MapVariogramSurfaceRas::sAddInfo() const
{
  String s = String("Average over all data\r\n (mean of all (%i) valid input pixel values) ",iNrValidInPixels);
  s &= String("\r\n = %.3f (in data units)", rDataMean);
   s &= "\r\nVariance over all data\r\n (mean of all squared deviations from the average)";
  s &= String("\r\n = %.3f (in squared data units)", rDataVariance);
	s &= "\r\nUnbiased Variance Estimate:";
	s &= String("\r\n = %.3f ", rDataVariance * iNrValidInPixels / (iNrValidInPixels - 1));
  return s;
}

void MapVariogramSurfaceRas::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "MapVariogramSurfaceRas";
  htpFreeze = "ilwisapp\\variogram_surface_algorithm.htm";
}

void MapVariogramSurfaceRas::ShiftArrayBufIn(Array<RealBuf>& arBuf, long iNextRow, long iFr, long iNr)
{
  if(iNextRow < iNrLags)       //  incomplete window filling only
    mp->GetLineVal(iNextRow, arBuf[iNextRow], iFr, iNr);
  else {                       //  shifting-up all window rows 
    for (long iR = 0; iR < iNrLags - 1; iR++)
      Swap(arBuf[iR], arBuf[iR + 1]);
    mp->GetLineVal(iNextRow, arBuf[iNrLags - 1], iFr, iNr); // and read new one
  }  
}
  
bool MapVariogramSurfaceRas::fFreezing()
{ 
  long iNrInRows = mp->iLines();
  long iNrInCols = mp->iCols();
  long iNrOutRows = ptr.iLines();
  long iNrOutCols = ptr.iCols(); 
  RealBuf rBufOut(iNrOutCols);

  Array<RealBuf> arBufIn;  // these buffers contain the rows that intersect the moving window
  arBufIn.Resize(iNrLags);
  for (long iL = 0L; iL < iNrLags; iL++)
    arBufIn[iL].Size(iNrInCols); 
  
  RealMatrix rmVarioSurf(iNrOutRows, iNrOutCols);
  LongMatrix imFrequency(iNrOutRows, iNrOutCols);
  for (long iR = 0L; iR < iNrOutRows; iR++) {
    for (long iC = 0L; iC < iNrOutCols; iC++) {
      rmVarioSurf(iR,iC) = 0.0;
      imFrequency(iR,iC) = 0L;
    }
  }
  // A window (upper half of a square with size iNrOutRows by iNrOutCols)
  // moves over input map; While moving 
  // its bottom line central pixel is at position iRow, iCol, 
  // Inside this window a pixel 'Other' at position iWinRow, iWinCol;
  // is visited to subtract rCenter - rOther (the 2 pixelvals)
  // This difference is squared and averaged for each lag vector
  // contributing to the output pixel at position iSurfRow, iSurfCol
  trq.SetText("Calculating");
	rDataMean = 0.0; 
  rDataVariance = 0.0;
	iNrValidInPixels = 0;
  double rDataSquared = 0.0;
  double rDifZ, rDifZSquared; // intermediate values 
  for (long iRow = 0L; iRow < iNrInRows; iRow++)  { 
    if (trq.fUpdate(iRow, iNrInRows))
          return false;
    long iMinWinRow = max(iRow - iNrLags + 1, 0);//don't go beyond Row==0
    //long iMaxWinCol = min(iCol + iNrLags, iNrInCols) - 1; 
    ShiftArrayBufIn(arBufIn, iRow, 0, iNrInCols);
    for (long iCol = 0L; iCol < iNrInCols; iCol++) {
      if (trq.fAborted())
        return false;
      long iMinWinCol = max(iCol - iNrLags + 1, 0);//don't go beyond Col==0
      long iMaxWinCol = min(iCol + iNrLags, iNrInCols) - 1;//nor beyond Co==iNrCols-1  
      double rCenter = mp->rValue(RowCol(iRow,iCol));
       //value of window's central pixel
      if (rCenter == rUNDEF)    continue; 
			rDataSquared += rCenter * rCenter;
			rDataMean += rCenter;
			iNrValidInPixels++;
      for (long iWinRow = iMinWinRow; iWinRow <= iRow; iWinRow++) {
        if (iWinRow == iRow) iMaxWinCol = iCol - 1;
        ///mp->GetLineVal(iWinRow, rBufIn);
        for (long iWinCol = iMinWinCol; iWinCol <= iMaxWinCol ; iWinCol++) { 
          //rOther = mp->rValue(RowCol(iWinRow,iWinCol));
          double rOther = arBufIn[iWinRow - iMinWinRow][iWinCol];
          if (rOther == rUNDEF)    continue; 
          rDifZ = rCenter - rOther;
          rDifZSquared = rDifZ * rDifZ / 2.0;  //contrbutes to semivar
          long iRowDiff = iWinRow - iRow; // Row position of rcOther relative to rcCenter
          long iColDiff = iWinCol - iCol; // Col position  ,,   ,,      ,,     ,,   ,,
          long iSurfRow = iNrLags - 1 + iRowDiff; // Shift of VarioSurf origin
          long iSurfCol = iNrLags - 1 + iColDiff; // to upper left RowCol
          rmVarioSurf(iSurfRow, iSurfCol) += rDifZSquared;
          imFrequency(iSurfRow, iSurfCol)++;// for pixel in one quadrant
          rmVarioSurf(2 * iNrLags - 2 - iSurfRow, 
                      2 * iNrLags - 2 - iSurfCol) += rDifZSquared;         
          imFrequency(2 * iNrLags - 2 - iSurfRow, 
                      2 * iNrLags - 2 - iSurfCol)++;// for pixel in oppos quadrant
        }
      }
    }
  }
  // Computed semivariogram values are stored in output raster:
  for (long iR = 0; iR < iNrOutRows; iR++)  { // fill pixels row by row
    if (trq.fUpdate(iR, iNrOutRows))
      return false;
    for (long iC = 0; iC < iNrOutCols; iC++)  { // fill pixels col by col
      if (trq.fAborted())
        return false;
      if (imFrequency(iR,iC) == 0)
        rBufOut[iC] = rUNDEF;
      else
        rBufOut[iC] = rmVarioSurf(iR,iC)/ imFrequency(iR,iC);
    }
    pms->PutLineVal(iR,rBufOut);
  }
	if (iNrValidInPixels) {
    rDataMean = rDataMean / iNrValidInPixels;
    rDataVariance = rDataSquared / iNrValidInPixels - rDataMean * rDataMean;
  }  
  else {
    rDataMean = rUNDEF;
    rDataVariance = rUNDEF;
  }
	ptr.SetAdditionalInfoFlag(true);
  ptr.SetAdditionalInfo(sAddInfo());
  return true;
}

String MapVariogramSurfaceRas::sExpression() const
{
  return String("MapVariogramSurfaceRas(%S, %li)", mp->sName(true, fnObj.sPath()),
                                                  iNrLags);
}

bool MapVariogramSurfaceRas::fDomainChangeable() const
{
  return true;
}

bool MapVariogramSurfaceRas::fValueRangeChangeable() const
{
  return true;
}

bool MapVariogramSurfaceRas::fGeoRefChangeable() const
{
  return false;
}
