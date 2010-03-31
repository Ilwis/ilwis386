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
/* MapCrossVariogramSurfaceRas
   Copyright Ilwis System Development ITC
   november 1998, by Jan Hendrikse
	Last change:  WK   29 Jun 99    3:54 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Applications\Raster\varcross.h"
#include "Engine\SpatialReference\Grdiff.H"
#include "Engine\Base\Algorithm\Realmat.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"
#define EPS10 1.0e-10
#define EPS20 1.0e-20


const char* MapCrossVariogramSurfaceRas::sSyntax() {
  return "MapCrossVariogramSurfaceRas(mapA,mapB[,nrlags])\n";
}

static void NrLagsNotPos(const FileName& fn) {
  throw ErrorObject(SMAPErrNrLagsNotPos);
}
static void NrLagsTooLargeRas(const FileName& fn) {
  throw ErrorObject(SMAPErrNrLagsTooLargeRas);
}
static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

ValueRange MapCrossVariogramSurfaceRas::vrDefault(const Map& map)
{
  RangeReal rr = map->rrMinMax(false);  // default range of output variogrsurface
  if (!rr.fValid())
    rr = map->dvrs().rrMinMax();
  double rLowestOut = 0.0;
  double rHighestOut = sqr(rr.rHi() - rr.rLo()) / 2.0;
  double rStp = map->dvrs().rStep()/100.0;
  return ValueRange(rLowestOut, rHighestOut, rStp);
}

MapCrossVariogramSurfaceRas* MapCrossVariogramSurfaceRas::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 3 || iParms < 1)
    throw ErrorExpression(sExpr, sSyntax()); 
  Map mapA(as[0], fn.sPath());
  Map mapB(as[1], fn.sPath());
  if (mapA->gr()->fGeoRefNone())
    GeoRefNoneError(mapA->fnObj, errMapCrossVariogramSurfaceRas +3);
  if (mapB->gr()->fGeoRefNone())
    GeoRefNoneError(mapB->fnObj, errMapCrossVariogramSurfaceRas +3);
  bool fIncompGeoRef = false;
  if (mapA->gr()->fGeoRefNone() && mapB->gr()->fGeoRefNone())
    fIncompGeoRef = mapA->rcSize() != mapB->rcSize();
  else
    fIncompGeoRef = mapA->gr() != mapB->gr();
  if (fIncompGeoRef)
    throw ErrorIncompatibleGeorefs(mapA->gr()->sName(true, fn.sPath()),
                          mapB->gr()->sName(true, fn.sPath()), fn, errMapCross);
  long h = mapA->iLines(); //->gr()->rcSize().Row;
  long w = mapA->iCols();
  //double rPixSize = map->rPixelSize();
  long iLags = min(10, min(h, w));      // default of iLags (max = 10)
  if (iParms > 2) {
    long iL = (long)(as[2].rVal());
    if (iL < 1)
      NrLagsNotPos(fn);
    else if (iL > 4000)
      NrLagsTooLargeRas(fn);
    iLags = min(iL, min(h, w));// never more lags than pixels on a row or col
  }

  return new MapCrossVariogramSurfaceRas(fn, p, mapA, mapB, iLags);
}

MapCrossVariogramSurfaceRas::MapCrossVariogramSurfaceRas(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  ReadElement("MapCrossVariogramSurfaceRas", "CoVariableMap", mpB);
  ReadElement("MapCrossVariogramSurfaceRas", "NrLags", iNrLags);
  ReadElement("MapCrossVariogramSurfaceRas", "LagSpacing", rLagSpacing);
  GeoRef grNew; 
  //double rLagSpacing = map->gr()->rPixSize();
  grNew.SetPointer(new GeoRefDifferential(FileName(), rLagSpacing, iNrLags));
  ptr.SetGeoRef(grNew);
  ptr.SetSize(grNew->rcSize());
  Init();
}

MapCrossVariogramSurfaceRas::MapCrossVariogramSurfaceRas(const FileName& fn, MapPtr& p, 
            const Map& mapA,  const Map& mapB, const long iLags)
: MapFromMap(fn, p, mapA), 
  mpB(mapB),
  rLagSpacing(mapA->gr()->rPixSize()),
  iNrLags(iLags)
{
  GeoRef grNew; 
  grNew.SetPointer(new GeoRefDifferential(FileName(), rLagSpacing, iNrLags));
  ptr.SetGeoRef(grNew);
  ptr.SetSize(grNew->rcSize());
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapCrossVariogramSurfaceRas +4);
  SetValueRange(vrDefault(mapA));
  Init();
}

MapCrossVariogramSurfaceRas::~MapCrossVariogramSurfaceRas()
{
}

void MapCrossVariogramSurfaceRas::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapCrossVariogramSurfaceRas");
  WriteElement("MapCrossVariogramSurfaceRas", "CoVariableMap", mpB);
  WriteElement("MapCrossVariogramSurfaceRas", "NrLags", iNrLags);
  WriteElement("MapCrossVariogramSurfaceRas", "LagSpacing", rLagSpacing);
}

void MapCrossVariogramSurfaceRas::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "MapCrossVariogramSurfaceRas";
  //htpFreeze = htpMapCrossVariogramSurfaceRasT;
}

void MapCrossVariogramSurfaceRas::FillAndShiftArrayBufInA(Array<RealBuf>& arBuf, long iNextRow, long iFr, long iNr)
{
  if(iNextRow < iNrLags)       //  incomplete window filling only
    mp->GetLineVal(iNextRow, arBuf[iNextRow], iFr, iNr);
  else {                       //  shifting-up all window rows 
    for (long iR = 0; iR < iNrLags - 1; iR++)
      Swap(arBuf[iR], arBuf[iR + 1]);
    mp->GetLineVal(iNextRow, arBuf[iNrLags - 1], iFr, iNr); // and read new one
  }  
}

void MapCrossVariogramSurfaceRas::FillAndShiftArrayBufInB(Array<RealBuf>& arBuf, long iNextRow, long iFr, long iNr)
{
  if(iNextRow < iNrLags)       //  incomplete window filling only
    mpB->GetLineVal(iNextRow, arBuf[iNextRow], iFr, iNr);
  else {                       //  shifting-up all window rows 
    for (long iR = 0; iR < iNrLags - 1; iR++)
      Swap(arBuf[iR], arBuf[iR + 1]);
    mpB->GetLineVal(iNextRow, arBuf[iNrLags - 1], iFr, iNr); // and read new one
  }  
}
  
bool MapCrossVariogramSurfaceRas::fFreezing()
{ 
  long iNrInRows = mp->iLines();
  long iNrInCols = mp->iCols();
  long iNrOutRows = ptr.iLines();
  long iNrOutCols = ptr.iCols(); 
  RealBuf rBufOut(iNrOutCols);

  Array<RealBuf> arBufInA;  // these buffers contain the rows that intersect the moving window
  Array<RealBuf> arBufInB;
  arBufInA.Resize(iNrLags);
  arBufInB.Resize(iNrLags);
  for (long iL = 0L; iL < iNrLags; iL++) {
    arBufInA[iL].Size(iNrInCols); 
    arBufInB[iL].Size(iNrInCols);
  }
  
  RealMatrix rmCrossVarioSurf(iNrOutRows, iNrOutCols);
  LongMatrix imFrequency(iNrOutRows, iNrOutCols);
  for (long iR = 0L; iR < iNrOutRows; iR++) {
    for (long iC = 0L; iC < iNrOutCols; iC++) {
      rmCrossVarioSurf(iR,iC) = 0.0;
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
  double rDifZ_A, rDifZ_B, rDifZCrossed; // intermediate values 
  for (long iRow = 0L; iRow < iNrInRows; iRow++)  { 
    if (trq.fUpdate(iRow, iNrInRows))
          return false;
    long iMinWinRow = max(iRow - iNrLags + 1, 0);//don't go beyond Row==0
    //long iMaxWinCol = min(iCol + iNrLags, iNrInCols) - 1; 
    FillAndShiftArrayBufInA(arBufInA, iRow, 0, iNrInCols);
    FillAndShiftArrayBufInB(arBufInB, iRow, 0, iNrInCols);
    for (long iCol = 0L; iCol < iNrInCols; iCol++) {
      if (trq.fAborted())
        return false;
      long iMinWinCol = max(iCol - iNrLags + 1, 0);//don't go beyond Col==0
      long iMaxWinCol = min(iCol + iNrLags, iNrInCols) - 1;//nor beyond Co==iNrCols-1  
      double rCenterA = mp->rValue(RowCol(iRow,iCol));
       //A-value of window's central pixel
      if (rCenterA == rUNDEF)    continue; 
      double rCenterB = mpB->rValue(RowCol(iRow,iCol));
       //B-value of window's central pixel
      if (rCenterB == rUNDEF)    continue; 
      for (long iWinRow = iMinWinRow; iWinRow <= iRow; iWinRow++) {
        if (iWinRow == iRow) iMaxWinCol = iCol - 1;
        ///mp->GetLineVal(iWinRow, rBufIn);
        for (long iWinCol = iMinWinCol; iWinCol <= iMaxWinCol ; iWinCol++) { 
          //rOther = mp->rValue(RowCol(iWinRow,iWinCol));
          double rOtherA = arBufInA[iWinRow - iMinWinRow][iWinCol];
          if (rOtherA == rUNDEF)    continue; 
          double rOtherB = arBufInB[iWinRow - iMinWinRow][iWinCol];
          if (rOtherB == rUNDEF)    continue; 
          rDifZ_A = rCenterA - rOtherA;
          rDifZ_B = rCenterB - rOtherB;
          rDifZCrossed = rDifZ_A * rDifZ_B / 2.0;  //contrbutes to semivar
          long iRowDiff = iWinRow - iRow; // Row position of rcOther relative to rcCenter
          long iColDiff = iWinCol - iCol; // Col position  ,,   ,,      ,,     ,,   ,,
          long iSurfRow = iNrLags - 1 + iRowDiff; // Shift of VarioSurf origin
          long iSurfCol = iNrLags - 1 + iColDiff; // to upper left RowCol
          rmCrossVarioSurf(iSurfRow, iSurfCol) += rDifZCrossed;
          imFrequency(iSurfRow, iSurfCol)++;// for pixel in one quadrant
          rmCrossVarioSurf(2 * iNrLags - 2 - iSurfRow, 
                      2 * iNrLags - 2 - iSurfCol) += rDifZCrossed;         
          imFrequency(2 * iNrLags - 2 - iSurfRow, 
                      2 * iNrLags - 2 - iSurfCol)++;// for pixel in oppos quadrant
        }
      }
    }
  }
  // Computed semivariogram values are stored in output raster:
  for (int iR = 0; iR < iNrOutRows; iR++)  { // fill pixels row by row
    if (trq.fUpdate(iR, iNrOutRows))
      return false;
    for (long iC = 0; iC < iNrOutCols; iC++)  { // fill pixels col by col
      if (trq.fAborted())
        return false;
      if (imFrequency(iR,iC) == 0)
        rBufOut[iC] = rUNDEF;
      else
        rBufOut[iC] = rmCrossVarioSurf(iR,iC)/ imFrequency(iR,iC);
      if (iR == iNrLags - 1) // make central variogr surface pixel always 0
        rBufOut[iNrLags - 1] = 0.0;
    }
    pms->PutLineVal(iR,rBufOut);
  } 
  return true;
}

String MapCrossVariogramSurfaceRas::sExpression() const
{
  return String("MapCrossVariogramSurfaceRas(%S, %S, %li)", mp->sName(true, fnObj.sPath()),
  													mpB->sName(true, fnObj.sPath()),
                                                  iNrLags);
}

bool MapCrossVariogramSurfaceRas::fDomainChangeable() const
{
  return true;
}

bool MapCrossVariogramSurfaceRas::fValueRangeChangeable() const
{
  return true;
}

bool MapCrossVariogramSurfaceRas::fGeoRefChangeable() const
{
  return false;
}
