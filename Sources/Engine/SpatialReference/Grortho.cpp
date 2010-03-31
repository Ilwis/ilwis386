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
/* GeoRefOrthoPhoto
   Copyright Ilwis System Development ITC
   january 1997, by Jan Hendrikse
	Last change:  JH   11 dec 2004    2:13 pm
*/

#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grortho.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\Georef.hs"
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

GeoRefOrthoPhoto::GeoRefOrthoPhoto(const FileName& fn)
: GeoRefCTP(fn, true)
{
  long iTemp1Max, iTemp2Max, iTempNrFiduc;
  double rTempPrincD;
  init();
  bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	bool fPrev = *fDoNotShowError;
	*fDoNotShowError = true;
	ReadElement("GeoRefOrthoPhoto", "DTM", mapDTM);
	*fDoNotShowError = fPrev;
	if (!mapDTM.fValid())
		throw ErrorObject(SGRErrDTMNotValid);
  mapDTM->KeepOpen(true);
	ReadElement("GeoRefOrthoPhoto", "Principal Distance (mm)", rTempPrincD);// in mm on photograph
  ReadElement("GeoRefOrthoPhoto", "Height Accuracy", rHeightAccuracy);
  ReadElement("GeoRefOrthoPhoto", "Max Orient Iterations", iTemp1Max);
  ReadElement("GeoRefOrthoPhoto", "Max Height Iterations", iTemp2Max);
  ReadElement("GeoRefOrthoPhoto", "Fiducial Marks", iTempNrFiduc);
  iMaxOrientCount = iTemp1Max;
  iMaxHeightCount = iTemp2Max;
  iNrFiduc = iTempNrFiduc;
  rPrincDistance = rTempPrincD/1000;  // in meters for computation
  GeoRef grDTM = mapDTM->gr();
  if (cs() != grDTM->cs())
    SetCoordSystem(grDTM->cs());
  if (!grDTM->fNorthOriented())    // if the DTM mp has not a GeoRefCorners
    GeoRefCornersError(mapDTM->fnObj, errGeoRef3D + 1);
  int i = 0;
  for (; i < iNrFiduc; ++i)
  {
		 String sEntry("Photo Fiduc. Mark %i (mm)", i);
    ReadElement("GeoRefOrthoPhoto", sEntry.scVal(), cPhotoFid[i]);
    cPhotoFid[i].x /= 1000;
    cPhotoFid[i].y /= 1000;
  }  
  for (i=0; i < iNrFiduc; ++i)
  {
    String sEntry("Scanned Fiduc. Mark %i", i);
	Coord crd;
    ReadElement("GeoRefOrthoPhoto", sEntry.scVal(), crd); 
	rScannedFidRow[i] = crd.x;
	rScannedFidCol[i] = crd.y;
  }
  ReadElement("GeoRefOrthoPhoto", "Camera Principal Point", cCameraPPoint);
	ReadElement("GeoRefOrthoPhoto", "Camera Principal Point", cCameraPPoint);
	transfFid = SolveOrthoPhoto::AFFINE;
  String sTransformFiducials;
  ReadElement("GeoRefOrthoPhoto", "Transform Fiducials",sTransformFiducials);
  if (fCIStrEqual("CONFORM", sTransformFiducials))
    transfFid = SolveOrthoPhoto::CONFORM;
  else if (fCIStrEqual("AFFINE", sTransformFiducials)) 
    transfFid = SolveOrthoPhoto::AFFINE;
	else if (fCIStrEqual("BILINEAR", sTransformFiducials)) 
    transfFid = SolveOrthoPhoto::BILINEAR;
  else if (fCIStrEqual("PROJECTIVE", sTransformFiducials))
    transfFid = SolveOrthoPhoto::PROJECTIVE;
  Compute(); 
}

GeoRefOrthoPhoto::GeoRefOrthoPhoto(const FileName& fn, const Map& mp, RowCol rc, bool fSubPixelPrecise)
: GeoRefCTP(fn,mp->cs(),rc, true, fSubPixelPrecise), mapDTM(mp)
{
	if (!mapDTM.fValid())
		throw ErrorObject(SGRErrDTMNotValid);
	mapDTM->KeepOpen(true);
  GeoRef grDTM = mapDTM->gr();
  if (grDTM->fGeoRefNone())
    GeoRefNoneError(mapDTM->fnObj, errGeoRef3D);
  if (!grDTM->fNorthOriented())    // if the DTM mp has not a GeoRefCorners
    GeoRefCornersError(mapDTM->fnObj, errGeoRef3D + 1);
  init();
  if (cs() != grDTM->cs())
    SetCoordSystem(grDTM->cs());
}

void GeoRefOrthoPhoto::Store()
{
  SetAdditionalInfoFlag(fValid()); // set flag - Wim 18/5/99
  if (fValid()) {// write only AdditInfo for valid GeoRef JanH 14/5/99
    SetAdditionalInfo(sFormula());
  }
  GeoRefCTP::Store();
  WriteElement("GeoRef", "Type", "GeoRefOrthoPhoto");
  WriteElement("GeoRefOrthoPhoto", "DTM", mapDTM);
  WriteElement("GeoRefOrthoPhoto", "Principal Distance (mm)", rPrincDistance*1000);
  WriteElement("GeoRefOrthoPhoto", "Height Accuracy", rHeightAccuracy);
  WriteElement("GeoRefOrthoPhoto", "Max Orient Iterations", iMaxOrientCount);
  WriteElement("GeoRefOrthoPhoto", "Max Height Iterations", iMaxHeightCount);
  WriteElement("GeoRefOrthoPhoto", "Fiducial Marks", iNrFiduc);
  int i;
  for (i = 0; i < iNrFiduc; ++i)
  {
    String sEntry("Photo Fiduc. Mark %i (mm)", i);

    Coord crd = cPhotoFid[i];
    crd.x *= 1000;
    crd.y *= 1000;
    WriteElement("GeoRefOrthoPhoto", sEntry.scVal(), crd);
  }  
  for (i = 0; i < iNrFiduc; ++i)
  {
    String sEntry("Scanned Fiduc. Mark %i", i);
		if(!fSubPixelPrecision) {
			RowCol rcScannedFid = RowCol(rScannedFidRow[i], rScannedFidCol[i]);
			WriteElement("GeoRefOrthoPhoto", sEntry.scVal(), rcScannedFid);
		}
		else {
			Coord crdRC = Coord(rScannedFidRow[i], rScannedFidCol[i]);
			WriteElement("GeoRefOrthoPhoto", sEntry.scVal(), crdRC);
		}
  }
  WriteElement("GeoRefOrthoPhoto", "Principal Point in Scan X-Coord", m_cScanPrincPoint.x);
  WriteElement("GeoRefOrthoPhoto", "Principal Point in Scan Y-Coord", m_cScanPrincPoint.y);
  WriteElement("GeoRefOrthoPhoto", "Principal Point in Scan Col", rScanPrincPointCol);
  WriteElement("GeoRefOrthoPhoto", "Principal Point in Scan Row", rScanPrincPointRow); 
	WriteElement("GeoRefOrthoPhoto", "Camera Principal Point", cCameraPPoint); 
	String sTransformFiducials;
  switch (transfFid) {
    case SolveOrthoPhoto::CONFORM: sTransformFiducials = "CONFORM"; break; 
    case SolveOrthoPhoto::AFFINE: sTransformFiducials = "AFFINE"; break; 
		case SolveOrthoPhoto::BILINEAR: sTransformFiducials = "BILINEAR"; break;
		case SolveOrthoPhoto::PROJECTIVE: sTransformFiducials = "PROJECTIVE"; break; 
  } 
  WriteElement("GeoRefOrthoPhoto", "Transform Fiducials", sTransformFiducials);
  ObjectDependency objdep;
  objdep.Add(mapDTM);
  objdep.Store(this);
}

GeoRefOrthoPhoto::~GeoRefOrthoPhoto()
{
}

void GeoRefOrthoPhoto::init()
{
  vecProjCenter = CVector(3); 
  
  rPrincDistance = 0.150;        // default principal distance of 150 mm
  iNrFiduc = 0;
  rHeightAccuracy = 7.5;     // default terrain Height Accuracy in meters
  iMaxOrientCount = 10;               // default Maximum nr of iteration Counts 
                           // before stopping terrain height search
  iMaxHeightCount = 15;
	transfFid = SolveOrthoPhoto::CONFORM;
	cCameraPPoint = Coord(0,0);
}

String GeoRefOrthoPhoto::sType() const
{
  return "GeoReference Orthophoto";
}

String GeoRefOrthoPhoto::sFormula() const
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
    s &= String("\r\nScanned Fiduc. Mark %i ", i);
	  s &= String(" %.2f, %.2f,",rScannedFidRow[i], rScannedFidCol[i]);
  }
	s &= "\r\n\r\nINNER ORIENTATION Results: ";
  s &= String("\r\nPrinc Point in Scanned RowCols: %.1f , %.1f", rScanPrincPointRow,rScanPrincPointCol);
  for (i = 0; i < iNrFiduc; ++i)
  {  
    s &= String("\r\nColResidu Fid Mrk %i :  %.1f", i, rResidCol[i]);
    s &= String("\r\nRowResidu Fid Mrk %i :  %.1f", i, rResidRow[i]);
  } 
	s &= String("\r\nPhoto2ScanScale (RowCols/Photo-mm) : %.2f ", rPhoto2ScanScale/1000);
  //s &= String("\r\nPrincipal Distance in ScanRowCol Units: %.0f", rScanPrincDistance);
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
  s &= String("\r\nPixelSize (Terrain-meters/RowCol): %.2f ", rAvgScale);
	if(fSubPixelPrecision)
		s &= String("\r\nFiducials and Tiepoints have RowCol positions with sub-pixel precision");
  return s;
}

double GeoRefOrthoPhoto::rPixSize() const
{
  if (!fValid())
    return rUNDEF;
  return rAvgScale;
}


void GeoRefOrthoPhoto::Coord2RowCol(const Coord& c, double& rRow, double& rCol) const
{
  // use a non constant member function to be able to change members TerrainCrd,vec1stRow( etc.
  // which are auxil vectorial member variables
  const_cast<GeoRefOrthoPhoto *>(this)->Crd2RC(c, rRow, rCol);
}

void GeoRefOrthoPhoto::Crd2RC(const Coord& c, double& rRow, double& rCol)
{
  if (!fValid() || c.fUndef()) {
    rRow = rUNDEF;
    rCol = rUNDEF;
    return;
  }  
  Coord crd = c;
  if (!cbDTM.fContains(crd))  {
    rRow = rUNDEF;
    rCol = rUNDEF;               // crd outside DTM
    return;
  }
	CVector TerrainCrd (3);
  TerrainCrd(0) = crd.x;                
  TerrainCrd(1) = crd.y;  
	double rHeight = mapDTM->rValue(crd);
	if (rUNDEF == rHeight) {
    rRow = rUNDEF;
    rCol = rUNDEF;
    return;
	}
  TerrainCrd(2) = rHeight;
  TerrainCrd -= vecProjCenter;
  //SetTerrainVec(crd.x, crd.y, mapDTM->rValue(crd), vecProjCenter);  
	RVector vec1stRow(3), vec2ndRow(3), vec3rdRow(3); 
  for (int i = 0; i < 3; i++) {
    vec1stRow(i) = rmRotationU2X(0,i);
    vec2ndRow(i) = rmRotationU2X(1,i);
    vec3rdRow(i) = rmRotationU2X(2,i);
  }  
  // transf from terrein 3D to camera XY (2D) coordinates:
  double CameraCrdX =  - rPrincDistance * (vec1stRow * TerrainCrd) / (vec3rdRow * TerrainCrd);  // can the denominator be 0 ?
  double CameraCrdY =  - rPrincDistance * (vec2ndRow * TerrainCrd) / (vec3rdRow * TerrainCrd);  // only if points are very far outside photo
  //CameraCrdX += cCameraPPoint.x; ?????????????????
	//CameraCrdY += cCameraPPoint.y; ?????????????????
	// transf from  camrera XY (2D) coordinates to scan RowCols:
	if (transfFid == SolveOrthoPhoto::CONFORM || transfFid == SolveOrthoPhoto::AFFINE) {
	rRow = CameraCrdX * rCoeffRow[1] 
								+ CameraCrdY * rCoeffRow[2]; 
	rCol = CameraCrdX * rCoeffCol[1] 
								+ CameraCrdY * rCoeffCol[2]; 
	}
  else if (transfFid == SolveOrthoPhoto::BILINEAR) {
		rRow =  rCoeffRow[0] + rCoeffRow[1] * CameraCrdX  
			+ rCoeffRow[2] * CameraCrdY + rCoeffRow[3] * CameraCrdX * CameraCrdY; 
    rCol =  rCoeffCol[0] + rCoeffCol[1] * CameraCrdX  
			+ rCoeffCol[2] * CameraCrdY + rCoeffCol[3] * CameraCrdX * CameraCrdY;	
	}
	else if (transfFid == SolveOrthoPhoto::PROJECTIVE) {
		rRow = 
			(rCoeffY[0]*CameraCrdX + rCoeffY[1]*CameraCrdY + rCoeffY[2]) /
			(rCoeffY[6]*CameraCrdX + rCoeffY[7]*CameraCrdY + 1);
		rCol = 
			(rCoeffY[3]*CameraCrdX + rCoeffY[4]*CameraCrdY + rCoeffY[5]) /
			(rCoeffY[6]*CameraCrdX + rCoeffY[7]*CameraCrdY + 1); 
	}
	rRow += rScanPrincPointRow;       // shift of photo system from Principal Point to real Photo rowcol system
        // where the Princ Point is obtained from inner orientation "iFindPrincPoint"
  rCol += rScanPrincPointCol;
  // RowCol syst is also right-handed (like terrain syst)
}

CoordBounds GeoRefOrthoPhoto::CalcCoordBounds(MinMax& mm)
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
  
void GeoRefOrthoPhoto::RowCol2Coord(double rRow, double rCol, Coord& crd) const
{
// use a non constant member function to be able to change members PhotoCrd,vec1stRow( etc.)
// which are auxil vectorial member variables
  const_cast<GeoRefOrthoPhoto *>(this)->RC2Crd(rRow, rCol, crd);
}

void GeoRefOrthoPhoto::RC2Crd(double rRow, double rCol, Coord& crd)
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
  double rF1, rF2, rF3, rKu, rKv, rDu, rDv, rU0, rV0, rW0;
	CVector CameraCrd(3);
  double ScanCrdX = rRow - rScanPrincPointRow;    // Put photosyst origin in Princ point
  double ScanCrdY = rCol - rScanPrincPointCol;
  // mirror reflection of Y coords to make scan (photo) coord syst right handed (like terrain syst)
  if (transfFid == SolveOrthoPhoto::CONFORM || transfFid == SolveOrthoPhoto::AFFINE) {
		CameraCrd(0) = ScanCrdX * rCoeffX[1] 
									+ ScanCrdY * rCoeffX[2] + cCameraPPoint.x; 
		CameraCrd(1) = ScanCrdX * rCoeffY[1] 
									+ ScanCrdY * rCoeffY[2] + cCameraPPoint.y;
		CameraCrd(2) = - rPrincDistance; 
	}
	else if (transfFid == SolveOrthoPhoto::BILINEAR) {
		CameraCrd(0) = rCoeffX[0] + rCoeffX[1]*ScanCrdX +  rCoeffX[2]*ScanCrdY 
										+ rCoeffX[3]*ScanCrdX*ScanCrdY + cCameraPPoint.x; 
		CameraCrd(1) = rCoeffY[0] + rCoeffY[1]*ScanCrdX +  rCoeffY[2]*ScanCrdY 
										+ rCoeffY[3]*ScanCrdX*ScanCrdY + cCameraPPoint.y;
		CameraCrd(2) = - rPrincDistance; 
	}
	else if (transfFid == SolveOrthoPhoto::PROJECTIVE) {
			CameraCrd(0) = rCoeffRefY[0]*ScanCrdX + rCoeffRefY[1]*ScanCrdY +  rCoeffRefY[2] /
									(rCoeffRefY[6]*ScanCrdX+ rCoeffRefY[6]*ScanCrdY + 1); 
			CameraCrd(1) = rCoeffRefY[3]*ScanCrdX  + rCoeffRefY[4]*ScanCrdY +  rCoeffRefY[5] /
									(rCoeffRefY[6]*ScanCrdX+ rCoeffRefY[6]*ScanCrdY + 1); 
			CameraCrd(2) = - rPrincDistance; 
		}
  int i = 0;
	RVector vec1stRow(3), vec2ndRow(3), vec3rdRow(3); 
  for (; i < 3; i++) {
    vec1stRow(i) = rmRotationX2U(0,i);
    vec2ndRow(i) = rmRotationX2U(1,i);
    vec3rdRow(i) = rmRotationX2U(2,i);
  }  
  rF1 =  vec1stRow * CameraCrd; 
  rF2 =  vec2ndRow * CameraCrd;
  rF3 =  vec3rdRow * CameraCrd;
  rKu = rF1 / rF3;     // direction cotangents of lightray from current Photocrd
  rKv = rF2 / rF3;     // in terrainsystem w.r.t. U and V axis resp.
  rU0 = vecProjCenter(0);
  rV0 = vecProjCenter(1);
  rW0 = vecProjCenter(2);
  rDu = rU0 - rKu * rW0 ;  // displacement U component 
  rDv = rV0 - rKv * rW0 ;  // displacement V component 
                                     // along cuurent lightray toward estimated terrain height
  Coord cGround;  // approx ground position in UV coord (planimetric)
  
  if (abs(rKu) < EPS15 && abs(rKv) < EPS15) {    // lightray is vertical, no iteration needed
    cGround.x = rU0;            // ground coordinate in Nadir point
    cGround.y = rV0;            // perpenpicularly below Projection Center PC
    if (cbDTM.fContains(cGround)) {
      crd = cGround;
      return;
    }
    else                         // camer not above DTM area
      crd = crdUNDEF;
    return; 
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
      cGround.x = rU0;  //  first terrainpoint U  guess (doesn't change while iterating)
      cGround.y = rKv * rW[i] + rDv;  //  where the light ray pierces the W = W[0] level plane
      if (cGround.x < cbDTM.cMin.x) cGround.x = cbDTM.cMin.x;  // if light ray pierces outside DTM
      else if (cGround.x > cbDTM.cMax.x) cGround.x = cbDTM.cMax.x;  // then shift intersection to DTM edge
      if (!cbDTM.fContains(cGround))  {    
        crd = crdUNDEF;             // lightray completely outside "DTM cylinder"
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
    } // end while iterating towards accurate ground coordinates      
  }
  else if (abs(rKv) < EPS15 ) {    // lightray is parallel to UW plane
    while ((abs(rD[i]) > rHeightAccuracy) && ( iCount < iMaxHeightCount))  { 
      cGround.y = rV0;  //  first terrainpoint V  guess (doesn't change while iterating)
      cGround.x = rKu * rW[i] + rDu;  //  where the light ray pierces the W = W[0] level plane
      if (cGround.y < cbDTM.cMin.y) cGround.x = cbDTM.cMin.y;  // if light ray outside DTM
      else if (cGround.y > cbDTM.cMax.y) cGround.y = cbDTM.cMax.y;  // then shift intersection to DTM edge
      if (!cbDTM.fContains(cGround))  {
        crd = crdUNDEF;              // lightray completely outside "DTM cylinder"
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
    } // end while iterating towards accurate ground coordinates   
  }  
}


bool GeoRefOrthoPhoto::fEqual(const IlwisObjectPtr& obj) const
{
  const GeoRefPtr* grp = dynamic_cast<const GeoRefPtr*>(&obj);
  if (grp == 0)
    return false;
  if (grp == this)
    return true;
  if (rcSize() != grp->rcSize())
    return false;
  const GeoRefOrthoPhoto* grOrthoPhoto = dynamic_cast<const GeoRefOrthoPhoto*>(grp);
  if (grOrthoPhoto == 0)
	  return false;

  //   case Ortho PHOTO GEOREF:
  if (rPrincDistance != grOrthoPhoto->rPrincDistance)
    return false;
  if (mapDTM != grOrthoPhoto->mapDTM)
    return false;
  if (rmRotationX2U != grOrthoPhoto->rmRotationX2U)
    return false;
  if (rmRotationU2X != grOrthoPhoto->rmRotationU2X)
    return false;
  if (vecProjCenter != grOrthoPhoto->vecProjCenter)
    return false;    
  if (tblCTP->sName() != grOrthoPhoto->tblCTP->sName())
    return false;
  return true;
}

int GeoRefOrthoPhoto::InnerOrientation()
{
  ///Coord* cResiduals = new Coord[iNrFiduc];  // errors in fid marks inner rient transform
	Coord cResiduals [8];
	Coord cScanPrincPoint;
	if (cCameraPPoint == crdUNDEF)
		cCameraPPoint = Coord(0,0);
	
	int iRes;	
	switch (transfFid)
	{
		case SolveOrthoPhoto::CONFORM:
			iRes = iFindConformInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, rScannedFidCol, rPhoto2ScanScale, 
																rScanPrincPointRow, rScanPrincPointCol, cScanPrincPoint, cResiduals);
			break;
		case SolveOrthoPhoto::AFFINE:
			iRes = iFindAffineInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, rScannedFidCol, rPhoto2ScanScale,
                                rScanPrincPointRow, rScanPrincPointCol, cScanPrincPoint, cResiduals);
			break;
		case SolveOrthoPhoto::BILINEAR:	
		case SolveOrthoPhoto::PROJECTIVE:
			iRes = iComputeInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, rScannedFidCol, rPhoto2ScanScale,
                                rScanPrincPointRow, rScanPrincPointCol, cScanPrincPoint, cResiduals);
			break;
		default:
			iRes = iFindAffineInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, rScannedFidCol, rPhoto2ScanScale,
                                rScanPrincPointRow, rScanPrincPointCol, cScanPrincPoint, cResiduals);
	}
	//if (transf == GeoRefOrthoPhoto::CONFORM)
	//  iRes = iFindConformInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, rScannedFidCol, rPhoto2ScanScale, 
	//															rScanPrincPointRow, rScanPrincPointCol, cScanPrincPoint, cResiduals);
	//else
	//	iRes = iFindAffineInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, rScannedFidCol, rPhoto2ScanScale,
  //                              rScanPrincPointRow, rScanPrincPointCol, cScanPrincPoint, cResiduals);
  /*double a00 = rmAffineCamera2Scan(0,0);
	double a01 = rmAffineCamera2Scan(0,1);
	double a10 = rmAffineCamera2Scan(1,0);
	double a11 = rmAffineCamera2Scan(1,1);
	double b00 = rmAffineScan2Camera(0,0);
	double b01 = rmAffineScan2Camera(0,1);
	double b10 = rmAffineScan2Camera(1,0);
	double b11 = rmAffineScan2Camera(1,1); 
  rScanPrincDistance = rPrincDistance * rPhoto2ScanScale; */
	m_cScanPrincPoint = cScanPrincPoint;
  for (int i = 0; i < iNrFiduc; i++) {
    rResidRow[i] = cResiduals[i].x;   
    rResidCol[i] = cResiduals[i].y;
  }
                           
  //delete [] cResiduals;
  return iRes;
}

int GeoRefOrthoPhoto::OuterOrientation()
{
  if (!mapDTM.fValid())
    return -5;
  int iRes = 0;

  RangeReal rrMM = mapDTM->rrMinMax(true);
  rEstimTerrainHeight = (4 * rrMM.rHi() + rrMM.rLo()) / 5;  // First terrainHeightGuess
  rPrevTerrainHeight = 0;  // to be used for segment (stream) digitizing in monoplot
  int iNr;
  MinMax mm(RowCol(0.0,0.0),mapDTM->rcSize());
  mm.MaxRow() -= 1;
  mm.MaxCol() -= 1;
  cbDTM = CalcCoordBounds(mm);  // find edges of DTM mp
  int iRecs = tblCTP->iRecs();
  if (iRecs == 0) return -1;
  Coord* crdXY = new Coord[iRecs];
  Coord* crdRowCol = new Coord[iRecs];
  double* crdZ = new double[iRecs];
  iNr = 0;
  for (int i = 1; i <= iRecs; ++i) {
    if (fActive(i)) {
      bool fAct = true;
      double r, rY;
      Coord cXY;
      double rX = colX->rValue(i);
      if ((rX == rUNDEF || abs(rX) > 1e20)||(rX < cbDTM.cMin.x || rX > cbDTM.cMax.x))
        fAct = false;
      if (fAct) {
        rY = colY->rValue(i);
        if ((rY == rUNDEF || abs(rY) > 1e20)||(rY < cbDTM.cMin.y || rY > cbDTM.cMax.y))
          fAct = false;
      }
      if (fAct) {
        cXY = Coord(rX, rY);
        //r = mapDTM->rValue(cXY);
        //r = colZ->rValue(i);
        //if (r == rUNDEF || abs(r) > 1e6 || r > rrMM.rHi() || r < rrMM.rLo())
          //fAct = false;
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
        crdXY[iNr].x = colX->rValue(i);
        crdXY[iNr].y = colY->rValue(i);
        r = colZ->rValue(i);
        if ( r == rUNDEF )
          crdZ[iNr] = mapDTM->rValue(cXY);
        else
          crdZ[iNr] = r;
        crdRowCol[iNr].x = colRow->rValue(i) - 0.5;
        crdRowCol[iNr].y = colCol->rValue(i) - 0.5;
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
                       crdRowCol, crdXY, crdZ,
                       m_cScanPrincPoint,
                       rmRotationX2U, rmRotationU2X, vecProjCenter, rAvgScale,
                       rKappaFinal, rPhiFinal, rOmegaFinal, iMaxOrientCount, iIterations);
  delete [] crdXY;      // delete all objects of array crdXY calling the destructor of each array element
  delete [] crdZ;       // idem crdZ
  delete [] crdRowCol;  // idem array crdRowCol,  all created with 'new' for input from table
  _fValid = iRes == 0;
  return iRes;
}

int GeoRefOrthoPhoto::Compute()
{
  _fValid = false;

  int iRes = 0;  // error code from Inner- and Outer Orientation returned to & by Compute()
  //    -11    invalid fiducial marks found
  //    -12    less than 2 fiducial marks (not enough)
  //    -13    incorrect positions of Fid marks  (e.g. some almost coincide)
  //    -14    incorrect ordering of fiducial marks
  //    -15    wrong nbr of fid marks(not 2,3 or 4) 
  //    -1    no valid control points found
  //    -2    less than iMinNr = 3 control points (not enough)
  //    -3    incorrect positions of control points  (e.g.some almost coincide)
  //    -4    incorrect DTM heights compared to camera proj center
  //    -5    no valid DTM mp found 
  
  iRes = InnerOrientation();
  if (iRes == 0)
    iRes = OuterOrientation();
  return iRes;
}

void GeoRefOrthoPhoto::Rotate(bool fSwapRows, bool fSwapCols, bool fRotate)
{
  GeoRefPtr::Rotate(fSwapRows, fSwapCols, fRotate);
}

int GeoRefOrthoPhoto::iMinNr() const
{
  return 3;  
}

void GeoRefOrthoPhoto::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  FileName fnDat(fnObj, ".gr#", true);
  ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
  if (asSection != 0) {
    (*asSection) &= "TableStore";
    (*asEntry) &= "Data";
  }
}

RowCol GeoRefOrthoPhoto::rcGetPrincipalPoint()
{
	RowCol rcPP = RowCol(rScanPrincPointRow,rScanPrincPointCol);
	return rcPP;
}

Coord GeoRefOrthoPhoto::crdGetPrincipalPoint()
{
	Coord cPP = Coord(rScanPrincPointRow,rScanPrincPointCol);
	return cPP;
}

void GeoRefOrthoPhoto::GetObjectStructure(ObjectStructure& os)
{
	GeoRefCTP::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "GeoRefOrthoPhoto", "DTM");		
	}		
}	
