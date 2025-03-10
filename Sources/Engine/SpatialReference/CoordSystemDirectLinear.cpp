/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
/* CoordSystemDirectLinear
   Copyright Ilwis System Development ITC
   january 1997, by Jan Hendrikse
	Last change:  WK   11 Jun 98    2:13 pm
*/

#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\CoordSystemDirectLinear.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\Coordsys.hs"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

#define EPS10 1.e-10
#define EPS15 1.e-15

static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

static void GeoRefCornersError(const FileName& fn, IlwisError err)
{
  throw ErrorMapWithoutGeoRefCorners(fn, err);
}


CoordSystemDirectLinear::CoordSystemDirectLinear(const FileName& fn)
: CoordSystemCTP(fn, true)
{
  long iTempMax;
	bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	bool fPrev = *fDoNotShowError;
	*fDoNotShowError = true;
  ReadElement("CoordSystemDirectLinear", "DTM", mapDTM);
	*fDoNotShowError = fPrev;
	init();
  ReadElement("CoordSystemDirectLinear", "Height Accuracy", rHeightAccuracy);
  ReadElement("CoordSystemDirectLinear", "Max Height Iterations", iTempMax);
  iMaxHeightCount = iTempMax;
  GeoRef grDTM = mapDTM->gr();
  if (csOther != grDTM->cs())
    SetCoordSystemOther(grDTM->cs());
  if (!grDTM->fNorthOriented())    // if the DTM map has not a GeoRefCorners
    GeoRefCornersError(mapDTM->fnObj, errGeoRef3D + 1);
  int i = 0;
  Compute(); 
}

CoordSystemDirectLinear::CoordSystemDirectLinear(const FileName& fn, const Map& mp, const FileName& fnRefMap)
: CoordSystemCTP(fn, mp->cs() , fnRefMap, true), mapDTM(mp)
{
	init(); 
	GeoRef grDTM = mapDTM->gr();
  if (grDTM->fGeoRefNone())
    GeoRefNoneError(mapDTM->fnObj, errGeoRef3D);
  if (!grDTM->fNorthOriented())    // if the DTM map has not a GeoRefCorners
    GeoRefCornersError(mapDTM->fnObj, errGeoRef3D + 1);
  if (csOther != grDTM->cs())
    SetCoordSystemOther(grDTM->cs());
}

void CoordSystemDirectLinear::Store()
{
  SetAdditionalInfoFlag(fValid()); // set flag - Wim 18/5/99
  if (fValid()) {// write only AdditInfo for valid GeoRef JanH 14/5/99
    SetAdditionalInfo(sFormula());
  }
  CoordSystemCTP::Store();
  WriteElement("CoordSystem", "Type", "DirectLinear");
  WriteElement("CoordSystemDirectLinear", "DTM", mapDTM);
  WriteElement("CoordSystemDirectLinear", "Height Accuracy", rHeightAccuracy);
  WriteElement("CoordSystemDirectLinear", "Max Height Iterations", iMaxHeightCount);
  ObjectDependency objdep;
  objdep.Add(mapDTM);
  objdep.Store(this);
}

CoordSystemDirectLinear::~CoordSystemDirectLinear()
{
}

void CoordSystemDirectLinear::init()
{
	if (!mapDTM.fValid())
		throw ErrorObject(TR("DTM is not valid"));
  mapDTM->KeepOpen(true);
	
  rHeightAccuracy = 7.5;     // default terrain Height Accuracy in meters
                           // before stopping terrain height search
	MinMax mm(RowCol(0.0,0.0),mapDTM->rcSize());
  mm.MaxRow() -= 1;
  mm.MaxCol() -= 1;
  cbDTM = CalcCoordBounds(mm);  // find xy edges of DTM mp
  rrMMDTM = mapDTM->rrMinMax(BaseMapPtr::mmmCALCULATE);  // find minmax height of DTM
  //rHeightAccuracy = 10;    
	if (mapDTM.fValid() && mapDTM->vr().fValid()) // protect against vr() == 0
		rHeightAccuracy = mapDTM->vr()->rStep();    
  
  iMaxHeightCount = 15;                         
}

String CoordSystemDirectLinear::sType() const
{
  return "CoordSystem DirectLinear";
}

String CoordSystemDirectLinear::sFormula() const
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
  s &= String("\r\nRow = \r\n(%.4g * X + %.4g * Y + %.4g * Z + %.2f) /",
        rCoeff[0], rCoeff[1], rCoeff[2], rCoeff[3]);  
  s &= String("\r\n (%.6f * X + %.6f * Y + %.6f * Z + 1)",
        rCoeff[8], rCoeff[9], rCoeff[10]);  
  s &= String("\r\nCol = \r\n(%.4g * X + %.4g * Y + %.4g * Z + %.2f) /",
        rCoeff[4], rCoeff[5], rCoeff[6], rCoeff[7]);  
  s &= String("\r\n (%.6f * X + %.6f * Y + %.6f * Z + 1)",
        rCoeff[8], rCoeff[9], rCoeff[10]);        
  s &= String("\r\n\r\nEstimated Camera Projection Center: \r\n (X,Y,Z)= %.2f, %.2f, %.2f ",
                rPCu, rPCv ,rPCw );
  Coord cNadir = Coord(rPCu,rPCv);
  double rNadirHeight =mapDTM->rValue(cNadir);
  double rFlyingHeight = rPCw - rNadirHeight ;
  double rInvFlyingDist = sqrt(rCoeff[8]*rCoeff[8]+rCoeff[9]*rCoeff[9]+rCoeff[10]*rCoeff[10]);
  double rFlyingDist; 
  if (rInvFlyingDist < EPS15 || abs(rCoeff[10]) < EPS15)
    rFlyingDist = rUNDEF;
  else  
    rFlyingDist = - rCoeff[10]/abs(rCoeff[10])/rInvFlyingDist;
  s &= String("\r\nEstimated Flying Height (distance to Nadir):  %.2f m",rFlyingHeight);
  s &= String("\r\nEstimated dist from Camera to Control points:  %.2f m",rFlyingDist);
  s &= String("\r\nCamera-axis Angles with X,Y,Z-axes (degrees):\r\n %.2f, %.2f, %.2f ",
                rAlpha*180/M_PI,rBeta*180/M_PI, rGamma*180/M_PI);
  s &= String("\r\n\r\nApprox 'Pixel'-Size (Terrain-meters/DigXYs): %.2f ", rAvgScale);
  s &= String("\r\nAverage X of active ctrl points in picture: %.1f", rAvgRow);
  s &= String("\r\nAverage Y of active ctrl points in picture: %.1f", rAvgCol);

	return s;
}

double CoordSystemDirectLinear::rPixSize() const
{
  if (!fValid())
    return rUNDEF;
  return rAvgScale;
}


Coord CoordSystemDirectLinear::cConvFromOther(const Coord& c) const
{
  if (!fValid() || c.fUndef()) 
    return crdUNDEF;
  Coord crd = c;
  if (!cbDTM.fContains(crd))  // crd outside DTM
    return crdUNDEF;
  double rZ = mapDTM->rValue(crd);
  if (rZ == rUNDEF || abs(rZ) > 1e6 || 
		         rZ > rrMMDTM.rHi() || rZ < rrMMDTM.rLo())
    return crdUNDEF;
        
  crd.x -=  rAvgX;  // make reduced input ground coords
  crd.y -=  rAvgY;  // for use in dirlin equations
  double z = rZ - rAvgZ;
  double denom = (rCoeff[8] * crd.x + rCoeff[9] * crd.y + rCoeff[10] * z + 1);
  if (abs(denom)< EPS10) 
    return crdUNDEF;
  double rRow = rAvgRow + (rCoeff[0] * crd.x + rCoeff[1] * crd.y +     
                         rCoeff[2] * z + rCoeff[3]) / denom;  // change image RowCols      
  double rCol = rAvgCol + (rCoeff[4] * crd.x + rCoeff[5] * crd.y +      // from reduced back
                         rCoeff[6] * z + rCoeff[7]) / denom;  // to true RowCols           

	Coord crdOut(rRow,rCol);
  return crdOut;         
}

CoordBounds CoordSystemDirectLinear::CalcCoordBounds(MinMax& mm)
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
  
Coord CoordSystemDirectLinear::cConvToOther(const Coord& crdIn) const
{
// use a non constant member function to be able to change members PhotoCrd,vec1stRow( etc.)
// which are auxil vectorial member variables
  double rRow = crdIn.x;
  double rCol = crdIn.y;
  Coord crd = crdUNDEF;
  ///RC2Crd(rRow, rCol, crdOut);
  ///return crdOut;

  if (!fValid()) {
//    throw ErrorObject("Invalid GeoRef", 12345);
    return crd;
  }    
  if (rRow == rUNDEF || rCol == rUNDEF) {
    return crdUNDEF;
  }    
 
  const_cast<CoordSystemDirectLinear *>(this)->iFindLightRayParameters(rRow, rCol);
  Coord cGround;  // approx ground position in UV coord (planimetric)
  
  if (abs(rKu) < EPS10 && abs(rKv) < EPS10) {    // lightray is vertical, no iteration needed
    cGround.x = rPCu;            // ground coordinate in Nadir point
    cGround.y = rPCv;            // perpenpicularly below Projection Center PC
    if (cbDTM.fContains(cGround))     
      return cGround;    
    else                         // camer not above DTM area
      return crdUNDEF;
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
  // Treatment of special cases: light ray perpendicular to U- or V- axis.(Karl16feb,Jan17feb)
  cGround.x = rDu;  //  first terrainpoint U, V guess
  cGround.y = rDv;  //  where the light ray pierces the W = 0 level plane  
  if (abs(rKu) < EPS10)  {
    if ((rDu < rBoxMinX) || (rDu > rBoxMaxX))  
      return crdUNDEF;
  }
  if (abs(rKv) < EPS10) {
    if ((rDv < rBoxMinY) || (rDv > rBoxMaxY))
			return crdUNDEF;
  }
  if (abs(rKu) < EPS10 && abs(rKv) < EPS10)
    return cGround;     // lightray is vertical and inside dtm, no iteration needed
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
    if (!cbDTM.fContains(cGround))   // after shift lightray start point
      return crdUNDEF;              // not yet on DTM bounding rectangle (even not in corner)
  }

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

      if (!cbDTM.fContains(cGround))   // after 2 shifts lightray piercing point
        return crdUNDEF;             // not yet on DTM bounding rectangle (even not in corner)
      
      rH[i] = mapDTM->rValue(cGround);
      if (rH[i] == rUNDEF) 
        return crdUNDEF;
       
      rD[i] = rW[i] - rH[i];
      if (abs(rD[i]) < rHeightAccuracy) 
        return cGround;                // Ground coord found with sufficient accuracy
      
      else  
        i = 1 - i;    // 1 "toggles" between 0 and 1
       
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
    } // end while iterating towards accurate ground coordinates   

  return cGround;
}

int CoordSystemDirectLinear::Compute()
{
 
  if (!mapDTM.fValid())
    return -5;
  rEstimTerrainHeight = (4 * rrMMDTM.rHi() + rrMMDTM.rLo()) / 5;  // First terrainHeightGuess
  rPrevTerrainHeight = 0;  // to be used for segment (stream) digitizing in monoplot
  int i, iNr;
  int iRecs = tblCTP->iRecs();
  if (iRecs == 0) return -1;
  int iRes = 0;
  Coord* crdRefXY = new Coord[iRecs];
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
      double rX = colRefX->rValue(i);
      if ((rX == rUNDEF || abs(rX) > 1e20 ||(rX < cbDTM.cMin.x || rX > cbDTM.cMax.x)))
        fAct = false;
      if (fAct) {
        rY = colRefY->rValue(i);
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
        r = colX->rValue(i);
        if (r == rUNDEF || abs(r) > 1e20)
          fAct = false;
      }  
      if (fAct) {
        r = colY->rValue(i);
        if (r == rUNDEF || abs(r) > 1e20)
          fAct = false;
      }  
      if (!fAct)
        SetActive(i, false);
      else {   
        rSumX += crdRefXY[iNr].x = colRefX->rValue(i);
        rSumY += crdRefXY[iNr].y = colRefY->rValue(i);
        r = colRefZ->rValue(i);
        if ( r == rUNDEF )
          crdZ[iNr] = mapDTM->rValue(cXY);
        else
          crdZ[iNr] = r;
        rSumZ += crdZ[iNr];
        crdRowCol[iNr].x = colX->rValue(i);
        crdRowCol[iNr].y = colY->rValue(i);
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
  if (0 == iRes) {
    rAvgX = rSumX / iNr;
    rAvgY = rSumY / iNr;
    rAvgZ = rSumZ / iNr;
    rAvgRow = rSumRow / iNr;
    rAvgCol = rSumCol / iNr;
    for (i = 0; i < iNr; ++i) {
      crdRefXY[i].x -= rAvgX;
      crdRefXY[i].y -= rAvgY;
      crdZ[i] -= rAvgZ;
      crdRowCol[i].x -= rAvgRow;
      crdRowCol[i].y -= rAvgCol;
    }

    iRes = iFindCoeffsOfDirLin(iNr, crdRowCol, crdRefXY, crdZ, rCoeff);
    if (iRes == 0) {
      iRes = iEstimateAverageScale(iNr, crdRowCol, crdRefXY, rAvgScale);
    }
	if (iRes == 0) {
      iRes = iFindProjectionCenter();
    }
    if (iRes == 0) {
      iRes = iFindNonCoplanarity(iNr, crdRefXY, crdZ);
    }  
  }
  delete [] crdRefXY;
  delete [] crdRowCol;
  _fValid = iRes == 0;
  return iRes;
  //    -1    no valid control points found
  //    -2    less than iMinNr = 6 control points (not enough)
  //    -3    incorrect positions of control points  (e.g.some almost coincide)
  //    -4    incorrect DTM heights compared to camera proj center
  //    -5    no valid DTM mp found 
}

int CoordSystemDirectLinear::iMinNr() const
{
  return 6;  
}

void CoordSystemDirectLinear::GetObjectStructure(ObjectStructure& os)
{
	CoordSystemCTP::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "CoordSystemDirectLinear", "DTM");
	}		
}	
