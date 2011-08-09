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
/*$Log: /ILWIS 3.0/GeoReference/GrParallProj.cpp $
 * 
 * 5     28-09-04 14:16 Hendrikse
 * Extended functionality to support sub-pixel precision in the tiepoints
 * and fiducials
 * 
 * 4     4-04-01 15:56 Koolhoven
 * added check on validity of mapDTM, if not valid throw an error
 * 
 * 3     10-01-01 18:23 Hendrikse
 * improved text of additional info (numerical precision)
 * 
 * 2     11-12-00 18:20 Hendrikse
 * extra addInfo about projection direction angles
 * 
 * 1     8-12-00 17:31 Hendrikse
/* GeoRefDIRLINnar
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:13 pm
*/

#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\GrParallProj.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#include "Headers\Hs\Georef.hs"
#define EPS10 1.e-10
#define EPS15 1.e-15
#define SIGN(A) (A>0?1:-1)

static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

static void GeoRefCornersError(const FileName& fn, IlwisError err)
{
  throw ErrorMapWithoutGeoRefCorners(fn, err);
}

GeoRefParallelProjective::GeoRefParallelProjective(const FileName& fn)
: GeoRefCTP(fn, true)
{
  ReadElement("GeoRefParallelProjective", "DTM", mapDTM);
  init();
  ReadElement("GeoRefParallelProjective", "Height Accuracy", rHeightAccuracy);
  long iTempMax;
  ReadElement("GeoRefParallelProjective", "Max Height Iterations", iTempMax);
  iMaxHeightCount = iTempMax;
  Compute();    
}

GeoRefParallelProjective::GeoRefParallelProjective(const FileName& fn, const Map& mp, RowCol rc, bool fSubPixelPrecise)
: GeoRefCTP(fn,mp->cs(),rc,true,fSubPixelPrecise), mapDTM(mp)
{
  init();
}

void GeoRefParallelProjective::init()
{
	if (!mapDTM.fValid())
		throw ErrorObject(TR("DTM is not valid"));

	mapDTM->KeepOpen(true);
  GeoRef grDTM = mapDTM->gr();
  if (grDTM->fGeoRefNone())
    GeoRefNoneError(mapDTM->fnObj, errGeoRef3D);
  if (!grDTM->fNorthOriented())    // if the DTM mp has not a GeoRefCorners
    GeoRefCornersError(mapDTM->fnObj, errGeoRef3D + 1);
  if (cs() != grDTM->cs())
    SetCoordSystem(grDTM->cs());
	iMaxHeightCount = 15;
    // default Maximum nr of iteration Counts, before stopping terrain height search:
  MinMax mm(RowCol(0.0,0.0),mapDTM->rcSize());
  mm.MaxRow() -= 1;
  mm.MaxCol() -= 1;
  cbDTM = CalcCoordBounds(mm);  // find xy edges of DTM mp
  rrMMDTM = mapDTM->rrMinMax(BaseMapPtr::mmmCALCULATE);  // find minmax height of DTM
  //rHeightAccuracy = 10;    
	if (mapDTM.fValid() && mapDTM->vr().fValid()) // protect against vr() == 0
		rHeightAccuracy = mapDTM->vr()->rStep();    
  // default terrain Height Accuracy in meters
}

void GeoRefParallelProjective::Store()
{
	SetAdditionalInfoFlag(fValid()); // set flag - Wim 18/5/99
  if (fValid()) {// write only AdditInfo for valid GeoRef JanH 14/5/99
    SetAdditionalInfo(sFormula());
  }
  GeoRefCTP::Store();
  WriteElement("GeoRef", "Type", "GeoRefParallelProjective");
  WriteElement("GeoRefParallelProjective", "DTM", mapDTM);
  WriteElement("GeoRefParallelProjective", "Height Accuracy", rHeightAccuracy);
  WriteElement("GeoRefParallelProjective", "Max Height Iterations", iMaxHeightCount);
  ObjectDependency objdep;
  objdep.Add(mapDTM);
  objdep.Store(this);
}

GeoRefParallelProjective::~GeoRefParallelProjective()
{
}

String GeoRefParallelProjective::sType() const
{
  return "GeoReference ParallelProjective";
}

String GeoRefParallelProjective::sFormula() const
{
  String s = "ORIENTATION DATA: ";
	s &= String("\r\nDTM map: %S ",mapDTM->sName());
  s &= String("\r\nwith MinMax Terrain Heights: %.2f m, %.2f m ",
                rrMMDTM.rLo(),rrMMDTM.rHi());
	int iRecs = tblCTP->iRecs();
  s &= String("\r\n\r\nNr of Ground Control Points: %i", iRecs);  
  s &= String("\r\nNr of Active Ground Control points: %i",iActive);  
  s &= String("\r\nAverage Coords of active control points: \r\n(X,Y,Z)= %.2f, %.2f, %.2f ",
                rAvgX, rAvgY, rAvgZ);  
  s &= String("\r\nTrend-plane equation: \r\nZ = ");
  s &= String("%.2f ", rTrendPlaneCoeff[0] + rAvgZ);
  s &= String(" + %.6f * (X - %.2f) ", rTrendPlaneCoeff[1], rAvgX);
  s &= String("\r\n       + %.6f * (Y - %.2f) ", rTrendPlaneCoeff[2], rAvgY);
  s &= String("\r\n\r\nNon-Coplanarity (RMS) in Ground Control: %.2f m", rNonCoplanarity); 
  s &= String("\r\nZ confidence Range in Ground Control: %.2f m", rZConfidRange); 
  s &= String("\r\nComputed from 2 outliers among active points:"); 
  s &= String("\r\nVertical dist of Point %i from trend-plane: %.2f m",iOutlier1, rOut1);
  s &= String("\r\nVertical dist of Point %i from trend-plane: %.2f m",iOutlier2, rOut2);
	s &= "\r\n\r\nORIENTATION RESULTS: ";
  s &= String("\r\nDirect Linear Transformation (Ground to Picture):");
  s &= String("\r\nRow = \r\n%.4g * X + %.4g * Y + %.4g * Z + %.4g",
        rCoeff[0], rCoeff[1], rCoeff[2], rCoeff[3]);   
  s &= String("\r\nCol = \r\n%.4g * X + %.4g * Y + %.4g * Z + %.4g",
        rCoeff[4], rCoeff[5], rCoeff[6], rCoeff[7]);  
	double alpha = abs(rKu) < EPS10 ? 90 : atan(1/rKu)*180/M_PI;
	double beta = abs(rKv) < EPS10 ? 90 : atan(1/rKv)*180/M_PI;
  s &= String("\r\n\r\nProjection angle with terrain X-axis: %.2f", alpha);
  s &= String("\r\nProjection angle with terrain Y-axis: %.2f", beta);	  
	s &= String("\r\n\r\nApproximate PixelSize (Terrain-meters/RowCol): %.2f ", rAvgScale);
	s &= String("\r\nAverage Row of active ctrl points in image: %.1f", rAvgRow);
  s &= String("\r\nAverage Col of active ctrl points in image: %.1f", rAvgCol);
	if(fSubPixelPrecision)
		s &= String("\r\nTiepoints have RowCol positions with sub-pixel precision");
  return s;
}

double GeoRefParallelProjective::rPixSize() const
{
  if (!fValid())
    return rUNDEF;
  return abs(rAvgScale);
}

CoordBounds GeoRefParallelProjective::CalcCoordBounds(MinMax& mm)
{
  Coord c1, c2, c3, c4;
  mapDTM->gr()->RowCol2Coord(mm.MinRow(), mm.MinCol(), c1);
  mapDTM->gr()->RowCol2Coord(mm.MinRow(), mm.MaxCol(), c2);
  mapDTM->gr()->RowCol2Coord(mm.MaxRow(), mm.MinCol(), c3);
  mapDTM->gr()->RowCol2Coord(mm.MaxRow(), mm.MaxCol(), c4);
  Coord cMin, cMax;
  cMin.x = min(min(c1.x,c2.x),min(c3.x,c4.x));
  cMax.x = max(max(c1.x,c2.x),max(c3.x,c4.x));
  cMin.y = min(min(c1.y,c2.y),min(c3.y,c4.y));
  cMax.y = max(max(c1.y,c2.y),max(c3.y,c4.y));
  CoordBounds cb(cMin, cMax);
  return cb;
}
 
void GeoRefParallelProjective::Coord2RowCol(const Coord& crd, double& rRow, double& rCol) const
{
  if (!fValid() || crd.fUndef()) {
    rRow = rUNDEF;
    rCol = rUNDEF;
    return;
  }  
  Coord c = crd;
  if (!cbDTM.fContains(c))  {
    rRow = rUNDEF;
    rCol = rUNDEF;               // crd outside DTM
    return;
  }
  double rZ = mapDTM->rValue(c);
  if (rZ == rUNDEF || abs(rZ) > 1e6 || rZ > rrMMDTM.rHi() || rZ < rrMMDTM.rLo()){
    rRow = rUNDEF;
    rCol = rUNDEF;
    return;
  }
        
  c.x = crd.x - rAvgX;  // make reduced input ground coords
  c.y = crd.y - rAvgY;  // for use in dirlin equations
  double z = rZ - rAvgZ;
  
  rRow = rAvgRow + (rCoeff[0] * c.x + rCoeff[1] * c.y +     
                         rCoeff[2] * z + rCoeff[3]);  // change image RowCols      
  rCol = rAvgCol + (rCoeff[4] * c.x + rCoeff[5] * c.y +      // from reduced back
                         rCoeff[6] * z + rCoeff[7]);  // to true RowCols           
}

void GeoRefParallelProjective::RowCol2Coord(double rRow, double rCol, Coord& crd) const
{
  if (!fValid()) {
//    throw ErrorObject("Invalid GeoRef", 12345);
    crd = crdUNDEF;
    return;
  }    
  if (rRow == rUNDEF || rCol == rUNDEF) {
    crd = crdUNDEF;
    return;
  }
	rRow -= rAvgRow;
	rCol -= rAvgCol;
	double rZrelative = rrMMDTM.rLo() - rAvgZ;// reative w.r.t. centroid
	Coord cGrnd;  // approx relat ground position in UV coord (planimetric)
	Coord cGround;  // approx ground position in UV coord (planimetric)
	cGrnd =	const_cast<GeoRefParallelProjective *>
			(this)->crdInverseOfParallelProjective(rRow, rCol, rZrelative);
																// pierce-point at lowest DTM level level
  double rDu = cGrnd.x + rAvgX; //
	double rDv = cGrnd.y + rAvgY; // naming similar to direct linear parameters
	cGround = Coord(rDu, rDv);

	if (abs(rKu) < EPS10 && abs(rKv) < EPS10) {    // lightray is vertical, no iteration needed
		if (cbDTM.fContains(cGround)) {
      crd = cGround;
      return;
    }
    else                         // sensor not above DTM area
      crd = crdUNDEF;
		return;
	}
  double rKvOverKu, rKuOverKv;
  if (abs(rKu) >= EPS10)
     rKvOverKu = rKv / rKu;   // direction coeff of lightray projected on UV plane
  if (abs(rKv) >= EPS10)
     rKuOverKv = rKu / rKv;   // inverse dir coeff of lightray

	double rBoxMinX = cbDTM.cMin.x;
  double rBoxMaxX = cbDTM.cMax.x;
  double rBoxMinY = cbDTM.cMin.y;
  double rBoxMaxY = cbDTM.cMax.y;
  bool fcGroundShifted = true;
  // Treatment of special cases: light ray perpendicular to U- or V- axis.

	if (abs(rKu) < EPS10)  {
    if ((rDu < rBoxMinX) || (rDu > rBoxMaxX)){  
      crd = crdUNDEF;
      return;
    }
  }
  if (abs(rKv) < EPS10) {
    if ((rDv < rBoxMinY) || (rDv > rBoxMaxY)){
      crd = crdUNDEF;
      return;
    }
  }
  if (abs(rKu) < EPS10 && abs(rKv) < EPS10)
    return;     // lightray is vertical and inside dtm, no iteration needed
  if (rDu < rBoxMinX) {
    cGround.x = rBoxMinX;
    cGround.y = rKvOverKu * (rBoxMinX - rDu)  + rDv; // oblique shift to DTM MinX edge
  }
  else if (rDu > rBoxMaxX) {
    cGround.x = rBoxMaxX;
    cGround.y = rKvOverKu * (rBoxMaxX - rDu)  + rDv; // oblique shift to DTM MaxX edge
  }
  if (rDv < rBoxMinY) {
    cGround.y = rBoxMinY;
    cGround.x = rKuOverKv * (rBoxMinY - rDv)  + rDu; // oblique shift to DTM MinY edge
  }
  else if (rDv > rBoxMaxY) {
    cGround.y = rBoxMaxY;
    cGround.x = rKuOverKv * (rBoxMaxY - rDv)  + rDu; // oblique shift to DTM MaxY edge
  }
  else
    fcGroundShifted = false;
  if ( fcGroundShifted ) {
    if (!cbDTM.fContains(cGround))  { // after shift lightray start point
      crd = crdUNDEF;              // not yet on DTM bounding rectangle (even not in corner)
      return;
    }
  }
/*
  Coord cPrev, cNext, cTrue;
  cNext = cGround;
  double rHeightPrev;
  //rRow -= rAvgRow;
  //rCol -= rAvgCol;
  crd.x = 0; // terrain crds relative to centroid
  crd.y = 0;
	double rHeight = rrMMDTM.rHi() - rAvgZ; //  ,,      ,,

  double rPixS2 = rPixSize() * rPixSize();
  double rDistZ = 2 * rHeightAccuracy;
  double rDistXY2 = 20 * rPixS2;
  
  int iCount = 0;
  while (((rDistZ > rHeightAccuracy)   // not yet close enough vertically
           || (rDistXY2 > 10 * rPixS2))   // or not yet close enough horizontally
           && ( iCount < iMaxHeightCount)) // and still few iterations
  {
    cPrev = cNext;
    rHeightPrev = rHeight;
  	cNext = 
	const_cast<GeoRefParallelProjective *>(this)->crdInverseOfParallelProjective(rRow, rCol, rHeight);
    cTrue.x = cNext.x + rAvgX;  // shift back to true terrain position
    cTrue.y = cNext.y + rAvgY;  // to find the height fom the DTM
    rHeight = mapDTM->rValue(cTrue) - rAvgZ;
    rDistXY2 = rDist2(cPrev, cNext);
    rDistZ = abs(rHeight - rHeightPrev);
    iCount++;
  }
  if ( iCount >= iMaxHeightCount)
    crd = crdUNDEF;
  //else
    // if (!cbDTM.fContains(cNext))  {
   // crd = crdUNDEF;             // solution outside "DTM cylinder"
   // return;
  //}
  else
  {
    crd.x += cNext.x + rAvgX; // final relatve terraincrd + shift back to absolute
    crd.y += cNext.y + rAvgY;
  }
  return;

  if (abs(rKu) < EPS10 && abs(rKv) < EPS10) {    // lightray is vertical, no iteration needed
    cGround.x = rPCu;            // ground coordinate in Nadir point
    cGround.y = rPCv;            // perpenpicularly below Projection Center PC
    if (cbDTM.fContains(cGround)) {
      crd = cGround;
      return;
    }
    else                         // camer not above DTM area
      crd = crdUNDEF;
    return; 
  }  
  */

  double rW[2];  // 2 successive estimations of terrain height    
  double rH[2], rD[2];   // 2 succ computations of DTM heigth and vertical move resp   
  rW[0] =  rEstimTerrainHeight; 
  // rW[0] = rPrevTerrainHeight; (in case of segment digitizing in monoplot
  
  int iCount = 0; 
  double rN; // difference of 2 succesive vertical moves
  bool fFirst = true; // true if iteration just started
  int i = 0;
  rD[0] = 1000;  // initialize vertical shifts
  rD[1] = 1000;
  
    while ((abs(rD[i]) > rHeightAccuracy) && ( iCount < iMaxHeightCount))  
		{ 
      cGround.x = rKu * rW[i] + rDu;  //  first terrainpoint U, V guess
      cGround.y = rKv * rW[i] + rDv;  //  where the light ray pierces the W = W[0] level plane
 
      if (!cbDTM.fContains(cGround))  { // after 2 shifts lightray piercing point
        crd = crdUNDEF;              // not yet on DTM bounding rectangle (even not in corner)
        return;
      }  
      rH[i] = mapDTM->rValue(cGround);
      if (rH[i] == rUNDEF) {
        crd = crdUNDEF;
        return;
      }  
      rD[i] = rW[i] - rH[i];
      if (abs(rD[i]) < rHeightAccuracy) {
        crd = cGround;
        return;                // Ground coord found with sufficient accuracy
      }
      else  {
        i = 1 - i;    // 1 "toggles" between 0 and 1
      }  
      if (fFirst) {
        rN = 0;
        fFirst = false;
        rW[1] = rH[0];
      }
      else
        rN = rD[1] - rD[0];  //  compare lightray heights at 2 successive places
      if ( rN /(rW[1] - rW[0]) < 0.5)        //  lightray parallel to terrain slope if rN == 0
        rW[i] = rH[1-i];                     //  rectangular spiral converging to endpoint ("stepwise")
      else
        rW[i] = (rD[1] * rW[0] - rD[0] * rW[1]) / rN; // triangular spiral converging faster and safer
      iCount++;           
    } // end while iterating towards accurate ground coordinates   */
}

bool GeoRefParallelProjective::fEqual(const IlwisObjectPtr& obj) const
{
  const GeoRefPtr* grp = dynamic_cast<const GeoRefPtr*>(&obj);
  if (grp == 0)
    return false;
  if (grp == this)
    return true;
  if (rcSize() != grp->rcSize())
    return false;
  const GeoRefParallelProjective* grc = dynamic_cast<const GeoRefParallelProjective*>(grp);
  if (0 == grc)
    return false;
  for (int i = 0; i <= 7; ++i)
    if (rCoeff[i] != grc->rCoeff[i])
      return false;
  if (tblCTP->sName() != grc->tblCTP->sName())
    return false;
  return true;
}

int GeoRefParallelProjective::Compute()
{
  _fValid = false;
  if (!mapDTM.fValid())
    return -5;
  rEstimTerrainHeight = (4 * rrMMDTM.rHi() + rrMMDTM.rLo()) / 5;  // First terrainHeightGuess
  rPrevTerrainHeight = 0;  // to be used for segment (stream) digitizing in monoplot
  int i, iNr;
  int iRecs = tblCTP->iRecs();
  if (iRecs == 0) return -1;
  int iRes = 0;
  Coord* crdXY = new Coord[iRecs];
  Coord* crdRowCol = new Coord[iRecs];
  double* crdZ = new double[iRecs];
  double rSumX = 0;
  double rSumY = 0;
  double rSumZ = 0;
  double rSumRow = 0;
  double rSumCol = 0;
  iNr = 0;
  for (i = 1; i <= iRecs; ++i) {
    if (fActive(i)) {
      bool fAct = true;
      double r, rY;
      Coord cXY;
      double rX = colX->rValue(i);
      if ((rX == rUNDEF || abs(rX) > 1e20 ||(rX < cbDTM.cMin.x || rX > cbDTM.cMax.x)))
        fAct = false;
      if (fAct) {
        rY = colY->rValue(i);
        if ((rY == rUNDEF || abs(rY) > 1e20||(rY < cbDTM.cMin.y || rY > cbDTM.cMax.y)))
          fAct = false;
      }
      if (fAct) {
        cXY = Coord(rX, rY);
        //r = mapDTM->rValue(cXY);
        //if (r == rUNDEF || abs(r) > 1e6 || r > rrMMDTM.rHi() || r < rrMMDTM.rLo())
        //  fAct = false;
      }  
      if (fAct) {
        r = colRow->rValue(i);
        if (r == rUNDEF || abs(r) > 1e6)
          fAct = false;
      }  
      if (fAct) {
        r = colCol->rValue(i);
        if (r == rUNDEF || abs(r) > 1e6)
          fAct = false;
      }  
      if (!fAct)
        SetActive(i, false);
      else {   
        rSumX += crdXY[iNr].x = colX->rValue(i);
        rSumY += crdXY[iNr].y = colY->rValue(i);
        r = colZ->rValue(i);
        if ( r == rUNDEF )
          crdZ[iNr] = mapDTM->rValue(cXY);
        else
          crdZ[iNr] = r;
        rSumZ += crdZ[iNr];
        crdRowCol[iNr].x = colRow->rValue(i) - 0.5;
        crdRowCol[iNr].y = colCol->rValue(i) - 0.5;
        rSumRow += crdRowCol[iNr].x;
        rSumCol += crdRowCol[iNr].y;
        iNr += 1;
      }  
    }  
  }
  iActive = iNr; // added to provide additional info 
  if (iNr == 0) 
    iRes = -1;
  else if (iNr < iMinNr()) 
    iRes = -2;

  if (0 == iRes) 
	{
    rAvgX = rSumX / iNr;
    rAvgY = rSumY / iNr;
    rAvgZ = rSumZ / iNr;
    rAvgRow = rSumRow / iNr;
    rAvgCol = rSumCol / iNr;
    for (i = 0; i < iNr; ++i) 
		{
      crdXY[i].x -= rAvgX;
      crdXY[i].y -= rAvgY;
      crdZ[i] -= rAvgZ;
      crdRowCol[i].x -= rAvgRow;
      crdRowCol[i].y -= rAvgCol;
    }
		iRes = iFindCoeffsOfParProj(iNr, crdRowCol, crdXY, crdZ, rCoeff);
		if (iRes == 0) {
			iRes = iEstimateAverageScale(iNr, crdRowCol, crdXY, rAvgScale);
		}
		if (iRes == 0) 
			iRes = iFindNonCoplanarity(iNr, crdXY, crdZ);
	}
	
  delete [] crdXY;
  delete [] crdRowCol;
  _fValid = iRes == 0;
  return iRes;
  //    -1    no valid control points found
  //    -2    less than iMinNr = 4 control points (not enough)
  //    -3    incorrect positions of control points  (e.g.some almost coincide)
  //    -4    incorrect DTM heights compared to camera proj center
  //    -5    no valid DTM mp found 
}


void GeoRefParallelProjective::Rotate(bool fSwapRows, bool fSwapCols, bool fRotate)
{
  GeoRefPtr::Rotate(fSwapRows, fSwapCols, fRotate);
}

void GeoRefParallelProjective::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  FileName fnDat(fnObj, ".gr#", true);
  ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
  if (asSection != 0) {
    (*asSection) &= "TableStore";
    (*asEntry) &= "Data";
  }
}

int GeoRefParallelProjective::iMinNr() const
{
  return 4;  
}

void GeoRefParallelProjective::GetObjectStructure(ObjectStructure& os)
{
	GeoRefCTP::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "GeoRefParallelProjective", "DTM");		
	}		

}	




