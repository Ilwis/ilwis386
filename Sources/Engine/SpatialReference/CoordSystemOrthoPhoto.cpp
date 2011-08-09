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
/* CoordSystemOrthoPhoto
   Copyright Ilwis System Development ITC
   january 1997, by Jan Hendrikse
	Last change:  WK   11 Jun 98    2:13 pm
*/

#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\SpatialReference\csctp.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\CoordSystemOrthoPhoto.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\SpatialReference\SolveOrthoPhoto.h"
#include "Engine\Base\Algorithm\Fpolynom.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\Coordsys.hs"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

#define EPS15 1.e-15

static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

static void GeoRefCornersError(const FileName& fn, IlwisError err)
{
  throw ErrorMapWithoutGeoRefCorners(fn, err);
}


CoordSystemOrthoPhoto::CoordSystemOrthoPhoto(const FileName& fn)
: CoordSystemCTP(fn, true)
{
  long iTemp1Max, iTemp2Max, iTempNrFiduc;//, iTempCornF;
  double rTempPrincD;
  init();
  ReadElement("CoordSystemOrthoPhoto", "UseCornerFiducials", fUseCornerFiducials);
	bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	bool fPrev = *fDoNotShowError;
	*fDoNotShowError = true;
  ReadElement("CoordSystemOrthoPhoto", "DTM", mapDTM);
	*fDoNotShowError = fPrev;
	//if (mapDTM->sName() == "" || 
	if (!mapDTM.fValid())
		throw ErrorObject(TR("DTM is not valid"));
	mapDTM->KeepOpen(true);
  ReadElement("CoordSystemOrthoPhoto", "Principal Distance (mm)", rTempPrincD);// in mm on photograph
  ReadElement("CoordSystemOrthoPhoto", "Height Accuracy", rHeightAccuracy);
  ReadElement("CoordSystemOrthoPhoto", "Max Orient Iterations", iTemp1Max);
  ReadElement("CoordSystemOrthoPhoto", "Max Height Iterations", iTemp2Max);
  ReadElement("CoordSystemOrthoPhoto", "Fiducial Marks", iTempNrFiduc);
  iMaxOrientCount = iTemp1Max;
  iMaxHeightCount = iTemp2Max;
  iNrFiduc = iTempNrFiduc;
  rPrincDistance = rTempPrincD/1000;  // in meters for computation
  //fUseCornerFiducials = iTempCornF;
  GeoRef grDTM = mapDTM->gr();
  if (csOther != grDTM->cs())
    SetCoordSystemOther(grDTM->cs());
  if (!grDTM->fNorthOriented())    // if the DTM map has not a GeoRefCorners
    GeoRefCornersError(mapDTM->fnObj, errGeoRef3D + 1);
  int i = 0;
  for (; i < iNrFiduc; ++i)
  {
    String sEntry("Photo Fiduc. Mark %i (mm)", i);
    ReadElement("CoordSystemOrthoPhoto", sEntry.c_str(), cPhotoFid[i]);
    cPhotoFid[i].x /= 1000;
    cPhotoFid[i].y /= 1000;
  }  
  for (i=0; i < iNrFiduc; ++i)
  {
    String sEntry("Scanned Fiduc. Mark %i", i);
    Coord crd;
    ReadElement("CoordSystemOrthoPhoto", sEntry.c_str(), crd); 
	rScannedFidRow[i] = crd.x;
	rScannedFidCol[i] = crd.y;
  }  
  Compute(); 
}

CoordSystemOrthoPhoto::CoordSystemOrthoPhoto(const FileName& fn, const Map& mp, const FileName& fnRefMap)
: CoordSystemCTP(fn, mp->cs() , fnRefMap, true), mapDTM(mp)
{
  fUseCornerFiducials = true;
	if (!mapDTM.fValid())
		throw ErrorObject(TR("DTM is not valid"));
	mapDTM->KeepOpen(true);
  GeoRef grDTM = mapDTM->gr();
  if (grDTM->fGeoRefNone())
    GeoRefNoneError(mapDTM->fnObj, errGeoRef3D);
  if (!grDTM->fNorthOriented())    // if the DTM map has not a GeoRefCorners
    GeoRefCornersError(mapDTM->fnObj, errGeoRef3D + 1);
  init();
  if (csOther != grDTM->cs())
    SetCoordSystemOther(grDTM->cs());
}

void CoordSystemOrthoPhoto::Store()
{
  SetAdditionalInfoFlag(fValid()); // set flag - Wim 18/5/99
  if (fValid()) {// write only AdditInfo for valid GeoRef JanH 14/5/99
    SetAdditionalInfo(sFormula());
  }
  CoordSystemCTP::Store();
  WriteElement("CoordSystem", "Type", "OrthoPhoto");
  WriteElement("CoordSystemOrthoPhoto", "UseCornerFiducials", fUseCornerFiducials);
  WriteElement("CoordSystemOrthoPhoto", "DTM", mapDTM);
  WriteElement("CoordSystemOrthoPhoto", "Principal Distance (mm)", rPrincDistance*1000);
  WriteElement("CoordSystemOrthoPhoto", "Height Accuracy", rHeightAccuracy);
  WriteElement("CoordSystemOrthoPhoto", "Max Orient Iterations", iMaxOrientCount);
  WriteElement("CoordSystemOrthoPhoto", "Max Height Iterations", iMaxHeightCount);
  WriteElement("CoordSystemOrthoPhoto", "Fiducial Marks", iNrFiduc);
  int i;
  for (i = 0; i < iNrFiduc; ++i)
  {
    String sEntry("Photo Fiduc. Mark %i (mm)", i);
    Coord crd = cPhotoFid[i];
    crd.x *= 1000;
    crd.y *= 1000;
    WriteElement("CoordSystemOrthoPhoto", sEntry.c_str(), crd);
  }  
  for (i = 0; i < iNrFiduc; ++i)
  {
    String sEntry("Scanned Fiduc. Mark %i", i);
    RowCol rcScannedFid = RowCol(rScannedFidRow[i], rScannedFidCol[i]);
    WriteElement("CoordSystemOrthoPhoto", sEntry.c_str(), rcScannedFid);
  }
  ///WriteElement("CoordSystemOrthoPhoto", "Principal Point in Scan X-Coord", cScanPrincPoint.x);
  ///WriteElement("CoordSystemOrthoPhoto", "Principal Point in Scan Y-Coord", cScanPrincPoint.y);
  ///WriteElement("CoordSystemOrthoPhoto", "Principal Point in Scan Col", rScanPrincPointCol);
  ///WriteElement("CoordSystemOrthoPhoto", "Principal Point in Scan Row", rScanPrincPointRow);  
  ///WriteElement("CoordSystemOrthoPhoto", "Camera Proj Center E", vecProjCenter(0));
  ///WriteElement("CoordSystemOrthoPhoto", "Camera Proj Center N", vecProjCenter(1));
  ///WriteElement("CoordSystemOrthoPhoto", "Camera Proj Center H", vecProjCenter(2));
  ///WriteElement("CoordSystemOrthoPhoto", "Camera Swing Angle (Kappa)", rKappaFinal);
  ///WriteElement("CoordSystemOrthoPhoto", "Camera Tilt Angle (Phi)", rPhiFinal);
  ///WriteElement("CoordSystemOrthoPhoto", "Camera Tilt Angle (Omega)", rOmegaFinal);
  ///WriteElement("CoordSystemOrthoPhoto", "Iterations for Outer Or.", iIterations);
  ///WriteElement("CoordSystemOrthoPhoto", "Photo2ScanScale (RowCols/Photo-meter)", rPhoto2ScanScale);
  ///WriteElement("CoordSystemOrthoPhoto", "Principal Distance in ScanRowCols", rScanPrincDistance);
  ///WriteElement("CoordSystemOrthoPhoto", "PixelSize (RowCols/Terrain-meter)", rAvgScale);

  ObjectDependency objdep;
  objdep.Add(mapDTM);
  objdep.Store(this);
}

CoordSystemOrthoPhoto::~CoordSystemOrthoPhoto()
{
}

void CoordSystemOrthoPhoto::init()
{
  vecProjCenter = CVector(3); 
  PhotoCrd      = CVector(3); 
  TerrainCrd    = CVector(3);
  vec1stRow  = RVector(3); 
  vec2ndRow  = RVector(3);  
  vec3rdRow  = RVector(3);

  rPrincDistance = 0.150;        // default principal distance of 150 mm
  iNrFiduc = 0;

  rHeightAccuracy = 7.5;     // default terrain Height Accuracy in meters
  iMaxOrientCount = 10;               // default Maximum nr of iteration Counts 
                           // before stopping terrain height search
  iMaxHeightCount = 15;                         
}

String CoordSystemOrthoPhoto::sType() const
{
  return "CoordSystem Orthophoto";
}

String CoordSystemOrthoPhoto::sFormula() const
{
  String s = "INNER ORIENTATION Data: ";
  s &= String("\r\nNr of Fiducial Marks: %d", iNrFiduc);
	int i;
  for (i = 0; i < iNrFiduc; ++i)
  {
    s &= String("\r\nPhoto Fiduc. Mark %i (mm) ", i);
    Coord crd = cPhotoFid[i];
    crd.x *= 1000;
    crd.y *= 1000;
    s &= String(" %.2f, %.2f,", crd.x, crd.y);
  }  
  for (i = 0; i < iNrFiduc; ++i)
  {
    s &= String("\r\nDigitized Fiduc. Mark %i ", i);
	  s &= String(" %.0f, %.0f,",rScannedFidRow[i], rScannedFidCol[i]);
  }
	s &= "\r\n\r\nINNER ORIENTATION Results: ";
	//s &= String("\r\nPrincipal Point in Scan XYs: %.1f , %.1f", cScanPrincPoint.x, cScanPrincPoint.y);
  s &= String("\r\nPrinc Point in Digitized XYs: %.1f , %.1f", rScanPrincPointRow,rScanPrincPointCol);
  for (i = 0; i < iNrFiduc; ++i)
  {  
    s &= String("\r\nColResidu Fid Mrk %i :  %.1f", i, rResidCol[i]);
    s &= String("\r\nRowResidu Fid Mrk %i :  %.1f", i, rResidRow[i]);
  }   
	s &= String("\r\nPhoto2DigitScale (DigXY/Photo-mm) : %.2f ", rPhoto2ScanScale/1000);
  s &= String("\r\nPrincipal Distance in DigXYs: %.0f", rScanPrincDistance);
  s &= String("\r\n\r\nOUTER ORIENTATION Data: ");
 	s &= String("\r\nNr of Ground Control Points: %i", tblCTP->iRecs());
  s &= String("\r\nNr of Active Ground Control Points: %i",iActive); 
	s &= String("\r\nDTM map: %S ",mapDTM->sName());
	s &= String("\r\n\r\nOUTER ORIENTATION Results: ");
	s &= String("\r\nIterations for Outer Orientation.: %d", iIterations);
  s &= String("\r\nCamera Proj Center (E,N,H): %.2f, %.2f, %.2f ",
                vecProjCenter(0),vecProjCenter(1),vecProjCenter(2));
  s &= String("\r\nCamera Rotation (Kap,Phi,Ome): %.2f, %.2f, %.2f ",
                rKappaFinal,rPhiFinal, rOmegaFinal);
	Coord cNadir = Coord(vecProjCenter(0),vecProjCenter(1));
	double rNadirHeight = mapDTM->rValue(cNadir);
  double rFlyingHeight = rPCw - rNadirHeight ;
	s &= String("\r\nEstimated Flying Height (distance to Nadir):  %.2f m",rFlyingHeight);
  s &= String("\r\nPixelSize (Terrain-meters/DigXYs): %.2f ", rAvgScale);
  return s;
}

double CoordSystemOrthoPhoto::rPixSize() const
{
  if (!fValid())
    return rUNDEF;
  return rAvgScale;
}


Coord CoordSystemOrthoPhoto::cConvFromOther(const Coord& c) const
{
  // use a non constant member function to be able to change members TerrainCrd,vec1stRow( etc.
  // which are auxil vectorial member variables
  double rRow, rCol;
  ///Crd2RC(c, rRow, rCol);
  ///Coord crdOut(rRow,rCol);
  ///return crdOut;

  if (!fValid() || c.fUndef()) {
    ///rRow = rUNDEF;
    ///rCol = rUNDEF;
    return crdUNDEF;
  }  
  Coord crd = c;
  if (!cbDTM.fContains(crd))  {
    ///rRow = rUNDEF;
    ///rCol = rUNDEF;               // crd outside DTM
    return crdUNDEF;
  }
  TerrainCrd(0) = crd.x;                
  TerrainCrd(1) = crd.y;  
  TerrainCrd(2) = mapDTM->rValue(crd);
  TerrainCrd -= vecProjCenter;
  //SetTerrainVec(crd.x, crd.y, mapDTM->rValue(crd), vecProjCenter);  
  for (int i = 0; i < 3; i++) {
    vec1stRow(i) = rmRotationU2X(0,i);
    vec2ndRow(i) = rmRotationU2X(1,i);
    vec3rdRow(i) = rmRotationU2X(2,i);
  }  
  /* vec1stRow = rmRotationU2X.GetRowVec(0);    // row a1  a2  a3
  vec2ndRow = rmRotationU2X.GetRowVec(1);       // row a4  a5  a6
  vec3rdRow = rmRotationU2X.GetRowVec(2);       // row a7  a8  a9  in Grabm text
  */    /// Use of functions GetRowVec and GetColVec is too expensive if done for every coord in map
  rRow =  - rScanPrincDistance * (vec1stRow * TerrainCrd) / (vec3rdRow * TerrainCrd);  // can the denominator be 0 ?
  rCol =  - rScanPrincDistance * (vec2ndRow * TerrainCrd) / (vec3rdRow * TerrainCrd);  // only if points are very far outside photo
  rRow += rScanPrincPointRow;       // shift of photo system from Principal Point to real Photo rowcol system
        // where the Princ Point is obtained from inner orientation "iFindPrincPoint"
  rCol += rScanPrincPointCol;
  // RowCol syst is also right-handed (like terrain syst)
  Coord crdOut(rRow,rCol);
  return crdOut;         
}

CoordBounds CoordSystemOrthoPhoto::CalcCoordBounds(MinMax& mm)
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
  
Coord CoordSystemOrthoPhoto::cConvToOther(const Coord& crdIn) const
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
    return crd;
  }    
  double rF1, rF2, rF3, rKu, rKv, rDu, rDv, rPCu, rPCv, rPCw;
  PhotoCrd(0) = rRow - rScanPrincPointRow;    // Put photosyst origin in Princ point
  PhotoCrd(1) = rCol - rScanPrincPointCol;
  // mirror reflection of Y coords to make scan (photo) coord syst right handed (like terrain syst)
      
  PhotoCrd(2) = - rScanPrincDistance; 
  int i = 0;
  for (; i < 3; i++) {
    vec1stRow(i) = rmRotationX2U(0,i);
    vec2ndRow(i) = rmRotationX2U(1,i);
    vec3rdRow(i) = rmRotationX2U(2,i);
  }  
  rF1 =  vec1stRow * PhotoCrd; 
  rF2 =  vec2ndRow * PhotoCrd;
  rF3 =  vec3rdRow * PhotoCrd;
  rKu = rF1 / rF3;     // direction cotangents of lightray from current Photocrd
  rKv = rF2 / rF3;     // in terrainsystem w.r.t. U and V axis resp.
  rPCu = vecProjCenter(0);
  rPCv = vecProjCenter(1);
  rPCw = vecProjCenter(2);
  rDu = rPCu - rKu * rPCw ;  // displacement U component 
  rDv = rPCv - rKv * rPCw ;  // displacement V component 
                                     // along cuurent lightray toward estimated terrain height
  Coord cGround;  // approx ground position in UV coord (planimetric)
  
  if (abs(rKu) < EPS15 && abs(rKv) < EPS15) {    // lightray is vertical, no iteration needed
    cGround.x = rPCu;            // ground coordinate in Nadir point
    cGround.y = rPCv;            // perpenpicularly below Projection Center PC
    if (cbDTM.fContains(cGround)) {
      return cGround;
    }
    else                         // camer not above DTM area
      return crdUNDEF;
  }
  double rKvOverKu, rKuOverKv;
  if (abs(rKu) >= EPS15)
     rKvOverKu = rKv / rKu;   // direction coeff of lightray projected on UV plane
  if (abs(rKv) >= EPS15)
     rKuOverKv = rKu / rKv;   // inverse dir coeff of lightray
                                       
  double rW[2];  // 2 successive estimations of terrain height    
  double rH[2], rD[2];   // 2 succ computations of DTM heigth and vertical move resp   
  rW[0] =  rEstimTerrainHeight; 
  // rW[0] = rPrevTerrainHeight; (in case of segment digitizing in monoplot
  
  int iCount = 0; 
  double rN; // difference of 2 succesive vertical moves
  bool fFirst = true; // true if iteration just started
  i = 0;
  rD[0] = 1000;  // initialize vertical shifts
  rD[1] = 1000;  
  if (abs(rKu) < EPS15 ) {    // lightray is parallel to VW plane
    while ((abs(rD[i]) > rHeightAccuracy) && ( iCount < iMaxHeightCount))  { 
      cGround.x = rPCu;  //  first terrainpoint U  guess (doesn't change while iterating)
      cGround.y = rKv * rW[i] + rDv;  //  where the light ray pierces the W = W[0] level plane
      if (cGround.x < cbDTM.cMin.x) cGround.x = cbDTM.cMin.x;  // if light ray pierces outside DTM
      else if (cGround.x > cbDTM.cMax.x) cGround.x = cbDTM.cMax.x;  // then shift intersection to DTM edge
      if (!cbDTM.fContains(cGround))  {        
        return crdUNDEF;// lightray completely outside "DTM cylinder"
      }
      rH[i] = mapDTM->rValue(cGround);
      if (rH[i] == rUNDEF) {
        return crdUNDEF;
      }
      rD[i] = rW[i] - rH[i];
      if (abs(rD[i]) < rHeightAccuracy) {
        return cGround;                // Ground coord found with sufficient accuracy
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
    } // end while iterating towards accurate ground coordinates      
  }
  else if (abs(rKv) < EPS15 ) {    // lightray is parallel to UW plane
    while ((abs(rD[i]) > rHeightAccuracy) && ( iCount < iMaxHeightCount))  { 
      cGround.y = rPCv;  //  first terrainpoint V  guess (doesn't change while iterating)
      cGround.x = rKu * rW[i] + rDu;  //  where the light ray pierces the W = W[0] level plane
      if (cGround.y < cbDTM.cMin.y) cGround.x = cbDTM.cMin.y;  // if light ray outside DTM
      else if (cGround.y > cbDTM.cMax.y) cGround.y = cbDTM.cMax.y;  // then shift intersection to DTM edge
      if (!cbDTM.fContains(cGround))  {
        return crdUNDEF;          // lightray completely outside "DTM cylinder" 
      }
      rH[i] = mapDTM->rValue(cGround);
      if (rH[i] == rUNDEF) {
        return crdUNDEF;
      }
      rD[i] = rW[i] - rH[i];
      if (abs(rD[i]) < rHeightAccuracy) {
        return cGround;                // Ground coord found with sufficient accuracy
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
    } // end while iterating towards accurate ground coordinates      
  }
  else  {
    while ((abs(rD[i]) > rHeightAccuracy) && ( iCount < iMaxHeightCount))  { 
      cGround.x = rKu * rW[i] + rDu;  //  first terrainpoint U, V guess
      cGround.y = rKv * rW[i] + rDv;  //  where the light ray pierces the W = W[0] level plane
      if (cGround.x < cbDTM.cMin.x) {
        cGround.x = cbDTM.cMin.x;  
        cGround.y = rKvOverKu * (cbDTM.cMin.x - rDu)  + rDv; // oblique shift to DTM MinX edge
      }  
      else if (cGround.x > cbDTM.cMax.x) {
        cGround.x = cbDTM.cMax.x; 
        cGround.y = rKvOverKu * (cbDTM.cMax.x - rDu)  + rDv; // oblique shift to DTM MaxX edge
      }  
      if (cGround.y < cbDTM.cMin.y) {
        cGround.y = cbDTM.cMin.y;  
        cGround.x = rKuOverKv * (cbDTM.cMin.y - rDv)  + rDu; // oblique shift to DTM MinY edge 
      }
      else if (cGround.y > cbDTM.cMax.y) {
        cGround.y = cbDTM.cMax.y; 
        cGround.x = rKuOverKv * (cbDTM.cMax.y - rDv)  + rDu; // oblique shift to DTM MaxY edge 
      }
      if (!cbDTM.fContains(cGround))  { // after 2 shifts lightray piercing point
        return crdUNDEF;             // not yet on DTM bounding rectangle (even not in corner)
      }  
      rH[i] = mapDTM->rValue(cGround);
      if (rH[i] == rUNDEF) {
        return crdUNDEF;
      }  
      rD[i] = rW[i] - rH[i];
      if (abs(rD[i]) < rHeightAccuracy) {
        return cGround;                // Ground coord found with sufficient accuracy
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
    } // end while iterating towards accurate ground coordinates   
  } 
  return crd;
}


int CoordSystemOrthoPhoto::InnerOrientation()
{
  Coord* cResiduals = new Coord[iNrFiduc];  // errors in fid marks inner rient transform
  int iRes = iFindInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, rScannedFidCol, rPhoto2ScanScale,
                                rScanPrincPointRow, rScanPrincPointCol, cScanPrincPoint, cResiduals);
                        // rcScanPrincPoint is used in RowCol2Coord  and Coord2RowCol
                        // transformations  (see here before)
                        // cScanPrincPoint is used by iFindOuterOrientation 
                        // (see hereafter and \tls\solveort.c)
  // princ distance scaled to rowcol units in scanned image:
  rScanPrincDistance = rPrincDistance * rPhoto2ScanScale;
  for (int i=0; i< iNrFiduc; i++) {
    rResidRow[i] = cResiduals[i].x;   
    rResidCol[i] = cResiduals[i].y;
  }
                           
  delete [] cResiduals;
  return iRes;
}

int CoordSystemOrthoPhoto::OuterOrientation()
{
  if (!mapDTM.fValid())
    return -5;
  int iRes = 0;

  RangeReal rrMM = mapDTM->rrMinMax(BaseMapPtr::mmmCALCULATE);
  rEstimTerrainHeight = (4 * rrMM.rHi() + rrMM.rLo()) / 5;  // First terrainHeightGuess
  rPrevTerrainHeight = 0;  // to be used for segment (stream) digitizing in monoplot
  int iNr;
  MinMax mm(RowCol(0.0,0.0),mapDTM->rcSize());
  mm.MaxRow() -= 1;
  mm.MaxCol() -= 1;
  cbDTM = CalcCoordBounds(mm);  // find edges of DTM map
  int iRecs = tblCTP->iRecs();
  if (iRecs == 0) return -1;
  Coord* crdXYRef = new Coord[iRecs];
  Coord* crdXY = new Coord[iRecs];
  double* rZRef = new double[iRecs];
  iNr = 0;
  for (int i = 1; i <= iRecs; ++i) {
    if (fActive(i)) {
      bool fAct = true;
      double r, rY;
      Coord cXYRef;
      double rX = colRefX->rValue(i);
      if ((rX == rUNDEF || abs(rX) > 1e20)||(rX < cbDTM.cMin.x || rX > cbDTM.cMax.x))
        fAct = false;
      if (fAct) {
        rY = colRefY->rValue(i);
        if ((rY == rUNDEF || abs(rY) > 1e20)||(rY < cbDTM.cMin.y || rY > cbDTM.cMax.y))
          fAct = false;
      }
      if (fAct) {
        cXYRef = Coord(rX, rY);
        //r = mapDTM->rValue(cXYRef);
        //r = colZ->rValue(i);
        //if (r == rUNDEF || abs(r) > 1e6 || r > rrMM.rHi() || r < rrMM.rLo())
          //fAct = false;
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
        crdXYRef[iNr].x = colRefX->rValue(i);
        crdXYRef[iNr].y = colRefY->rValue(i);
        r = colRefZ->rValue(i);
        if ( r == rUNDEF )
          rZRef[iNr] = mapDTM->rValue(cXYRef);
        else
          rZRef[iNr] = r;
        crdXY[iNr].x = colX->rValue(i);
        crdXY[iNr].y = colY->rValue(i);
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
     iRes = iFindOuterOrientation(iNr, rScanPrincDistance, mapDTM,
                       crdXY, crdXYRef, rZRef,
                       cScanPrincPoint,
                       rmRotationX2U, rmRotationU2X, vecProjCenter, rAvgScale,
                       rKappaFinal, rPhiFinal, rOmegaFinal, iMaxOrientCount, iIterations);
  delete [] crdXYRef;      // delete all objects of array crdXYRef calling the destructor of each array element
  delete [] rZRef;       // idem rZRef
  delete [] crdXY;  // idem array crdXY,  all created with 'new' for input from table
  _fValid = iRes == 0;
  return iRes;
}

int CoordSystemOrthoPhoto::Compute()
{
  _fValid = false;

  int iRes = 0;  // error code from Inner- and Outer Orientation returned to & by Compute()
  //    -11    no valid fiducial marks found
  //    -12    less than 2 fiducial marks (not enough)
  //    -13    incorrect positions of Fid marks  (e.g. some almost coincide)
  //    -14    incorrect ordering of fiducial marks
  //    -15    wrong nbr of fid marks(not 2,3 or 4) 
  //    -1    no valid control points found
  //    -2    less than iMinNr = 3 control points (not enough)
  //    -3    incorrect positions of control points  (e.g.some almost coincide)
  //    -4    incorrect DTM heights compared to camera proj center
  //    -5    no valid DTM map found 
  
  iRes = InnerOrientation();
  if (iRes == 0)
    iRes = OuterOrientation();
  return iRes;
}

int CoordSystemOrthoPhoto::iMinNr() const
{
  return 3;  
}

Coord CoordSystemOrthoPhoto::crdGetPrincipalPoint()
{
	return cScanPrincPoint;
}


void CoordSystemOrthoPhoto::GetObjectStructure(ObjectStructure& os)
{
	CoordSystemCTP::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "CoordSystemOrthoPhoto", "DTM");
	}		
}	
