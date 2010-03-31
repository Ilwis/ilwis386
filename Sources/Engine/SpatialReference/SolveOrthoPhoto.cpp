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
/* solveort.c
 iFindOrtho (c) Ilwis System Development, ITC
 january 1997,  by Jan Hendrikse
*/
#include "Engine\SpatialReference\SolveOrthoPhoto.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#include "Engine\Base\Algorithm\Fpolynom.h"

#define EPS8 1.e-8
#define EPS4 1.e-4
#define EPS14 1.e-14

template class _export Array<CVector>;
template class _export Array<Coord>;

SolveOrthoPhoto::SolveOrthoPhoto()
{}

// the following are functions from 2D vector geometry applied to Coords:

double SolveOrthoPhoto::rDotProd(Coord cA, Coord cB)
{
  return cA.x * cB.x + cA.y * cB.y;
}

double SolveOrthoPhoto::rNormSquared(Coord cA)
{
  return cA.x * cA.x  + cA.y * cA.y;
}

//double SolveOrthoPhoto::rCrossProd(RowCol rcA, RowCol rcB, RowCol rcC, RowCol rcD); // vector AB X vector DC
double SolveOrthoPhoto::rCrossProd(Coord cA, Coord cB, Coord cC, Coord cD) // (vector AB) X (vector DC)
{
  return ((cB.x -cA.x) * (cC.y - cD.y) -   
           (cB.y - cA.y) * (cC.x -cD.x));
}           

bool SolveOrthoPhoto::fPerpProjection(Coord cSide1, Coord cSide2, Coord &cFoot) // project end point of side2 
{                                                  //at right angles onto line of side1 (2-dimensional)
    double rSide1Sq = rNormSquared(cSide1);
    
    if (rSide1Sq < EPS8) return false;
    cFoot.x = rDotProd(cSide1, cSide2) * cSide1.x / rNormSquared(cSide1);
    cFoot.y = rDotProd(cSide1, cSide2) * cSide1.y / rNormSquared(cSide1);
    return true;
}

int SolveOrthoPhoto::iFindInnerResiduals(int iPoints, Coord cF[], Coord cS[], 
                         Coord cPhotoPrincPoint, Coord cScanPrincPoint,
                         Coord cD[])                  
{ 
  int i;
  if (iPoints == 2) {
    for (i = 0; i < iPoints; ++i) { 
      cD[i].x = 0;          // exact fit, residals == 0;
      cD[i].y = 0;
    }
    return 0;  
  } 

  for (i = 0; i < iPoints; ++i) {            // both systems shifted to their resp origins
    cF[i].x -=  cPhotoPrincPoint.x;
    cF[i].y -=  cPhotoPrincPoint.y;
    cS[i].x -=  cScanPrincPoint.x;
    cS[i].y -=  cScanPrincPoint.y;
  }
  // computation of linear conformal transf: x,y Photo --> s,t Scanned Photo
  //    S.x =  a * F.x  - b * F.y 
  //    S.y =  b * F.x  + a * F.y 
    
  double rXX, rYY, rXS, rXT, rYS, rYT;
  rXX = rYY = rXS = rXT = rYS = rYT = 0;
  for (i = 0; i < iPoints; ++i) {
    rXX += cF[i].x * cF[i].x;
    rYY += cF[i].y * cF[i].y;
    rXS += cF[i].x * cS[i].x;
    rYS += cF[i].y * cS[i].x;
    rXT += cF[i].x * cS[i].y;
    rYT += cF[i].y * cS[i].y;   
  }
  double denom = rXX + rYY;
  if (denom < EPS8) return -13; // error in finding transf parameters
  double rA = (rXS + rYT) / denom; 
  double rB = (rXT - rYS) / denom;                 
  // double rScale = sqrt(rA*rA + rB*rB); // approx scale in rowcols in scanned image per meter
  for (i = 0; i < iPoints; ++i) { 
     cD[i].x = rA * cF[i].x - rB * cF[i].y  - cS[i].x;   // transformed minus original position
     cD[i].y = rB * cF[i].x + rA * cF[i].y  - cS[i].y;   // in Row (X) and Col (Y) units
  }   
 
  return 0;           
}     
      
int SolveOrthoPhoto::iFindInnerOrientation(int iPoints, const Coord* cPhotoFid, 
		                   const double* rScannedFidRow, const double* rScannedFidCol,
                           double& rPhoto2ScanScale, double& rScanPrincPointRow, double& rScanPrincPointCol,
						   Coord& cScanPrincPoint, Coord cResiduals[])
{
  Coord cF[4];      // array of Fiducial marks in photo
  Coord cS[4];    // array  of Fiducial marks in scan made from photo (digital image)
  for (int i = 0; i < iPoints; ++i) { 
    cF[i].x = cPhotoFid[i].x;    // collecting fiducial mark Photo positions, in meters
    cF[i].y = cPhotoFid[i].y;    
    cS[i].x = rScannedFidRow[i] - 0.5;     // collecting fiducial mark positions in scan converting to coords
    cS[i].y = rScannedFidCol[i] - 0.5;     // to compute the position of PrincPoint in double precision
  }                                          // x = Row, Y = Col keeping the system right-handed
  if (iPoints < 2) return -12;             // at least 2 fiducial marks needed
  int iRet;                                // return value after residuals calculated (0 if succes)
	
	///***/// in all cases iFindInnerResiduals will be replaced by iFindAffineInnerOrientation
  if (iPoints == 2) {
    cScanPrincPoint.x = (cS[0].x + cS[1].x)/2;
    cScanPrincPoint.y = (cS[0].y + cS[1].y)/2; 
    rScanPrincPointRow = cScanPrincPoint.x + 0.5;
    rScanPrincPointCol = cScanPrincPoint.y + 0.5; 
    Coord cPhotoPrincPoint;
    cPhotoPrincPoint.x = (cF[0].x + cF[1].x)/2;
    cPhotoPrincPoint.y = (cF[0].y + cF[1].y)/2; 
    if ( (iRet = iFindInnerResiduals(iPoints, cF, cS, cPhotoPrincPoint, cScanPrincPoint, cResiduals)) < 0)
	    return iRet;
    double numerator =  rDist2(cS[0], cS[1]) ;
    double denomi = rDist2(cF[0], cF[1]);
    if (denomi < EPS4) return -13;  // fiducials coincide in photo
    rPhoto2ScanScale =  sqrt(numerator / denomi);  // distance on scan / distance on photo  (rowcols/m)
    return 0;
  }
   ///***///case iPoints == 3 must be simplified; the perp projection is wrong for affine transformation
	/// just the midpoint of longest les will be taken as princ point
	/// cPhotoPrincPoint.x = (cF[iOpp2].x - cF[iOpp1].x) / 2;
  /// cPhotoPrincPoint.y = (cF[iOpp2].x - cF[iOpp1].y)/ 2;
	/// cScanPrincPoint.x = (cS[iOpp2].x + cS[iOpp1].x) / 2;
	/// cScanPrincPoint.y = (cS[iOpp2].y + cS[iOpp1].y) / 2;
  if (iPoints == 3) {              // Use Triangle of Fiducial Marks
    double rD01 = rDist2(cS[0],cS[1]);
    double rD02 = rDist2(cS[0],cS[2]);    
    double rD12 = rDist2(cS[1],cS[2]);
    double rDmax = rD12;
    int iAlone = 0;               // initial index of FidMark without opposite FidMark
    int iOpp1 = 1;                // 2 oppsite Fid Marks will be connected (line Opp1 Opp2)
    int iOpp2 = 2;                // their midpoint cMid will be connected to cF[iAlone]
                                  // this median will be projected onto the line Opp1 Opp2
    if ((rD01 < rD02) && (rD12 < rD02)) {
      iAlone = 1;  iOpp1 = 0; iOpp2 = 2; rDmax = rD02;
    }
    if ((rD02 < rD01) && (rD12 < rD01)) {
      iAlone = 2; iOpp1 = 0; iOpp2 = 1;  rDmax = rD01;
    }
    if (rDmax < 2) return -13;   // position of Fid Marks closer than 2 RowCols causes singular case
    Coord cSide1, cSide2, cFoot; // Coords of triangle treated as 2D vectors
    cSide1.x = cS[iOpp2].x - cS[iOpp1].x;  // vector connecting oppos Fid Marks
    cSide1.y = cS[iOpp2].y - cS[iOpp1].y;
    cSide2.x = cS[iAlone].x - cS[iOpp1].x;  // vector connecting 1st and 3rd Fid Marks
    cSide2.y = cS[iAlone].y - cS[iOpp1].y;
    if (!fPerpProjection(cSide1, cSide2, cFoot)) return -13;
    cScanPrincPoint.x = cS[iOpp1].x + cFoot.x;
    cScanPrincPoint.y = cS[iOpp1].y + cFoot.y;
    rScanPrincPointRow = cScanPrincPoint.x + 0.5;
    rScanPrincPointCol = cScanPrincPoint.y + 0.5; 
    Coord cPhotoPrincPoint;
    cSide1.x = cF[iOpp2].x - cF[iOpp1].x;  // vector connecting oppos Fid Marks in photo system
    cSide1.y = cF[iOpp2].y - cF[iOpp1].y;
    cSide2.x = cF[iAlone].x - cF[iOpp1].x;  // vector connecting 1st and 3rd Fid Marks
    cSide2.y = cF[iAlone].y - cF[iOpp1].y;
    if (!fPerpProjection(cSide1, cSide2, cFoot)) return -13;
    cPhotoPrincPoint.x = cF[iOpp1].x + cFoot.x;
    cPhotoPrincPoint.y = cF[iOpp1].y + cFoot.y;
          
    if ( (iRet = iFindInnerResiduals(iPoints, cF, cS, cPhotoPrincPoint, cScanPrincPoint, cResiduals)) < 0)
	    return iRet;
    
    double numerator =  rDmax;
    double denomi = rDist2(cF[iOpp1], cF[iOpp2]);
    if (denomi < EPS4) return -13;  // fiducials in photo cause singularity
    rPhoto2ScanScale =  sqrt(numerator / denomi);
    return 0;
  }  
  
  if (iPoints == 4) {
    double rD01 = rDist2(cS[0],cS[1]);
    double rD02 = rDist2(cS[0],cS[2]);    
    double rD03 = rDist2(cS[0],cS[3]);    
    // find the opposite of cS[0]
    int iOpp = 1;               // initial index of opposite Fid Mark  S[1]
    int iAdj1 = 2;              // initial index of one adjacent Fid Mark
    int iAdj2 = 3;              // initial index of other adj Fid Mark
    if (rD01 < rD02 && rD03 < rD02) {      // S[2] oppos to S[0]
      iOpp = 2; iAdj1 = 1 ; iAdj2 = 3;
    }  
    else if (rD01 < rD03 && rD02 < rD03) { // S[3] oppos to S[0]
      iOpp = 3; iAdj1 = 1 ; iAdj2 = 2;
    }  
    // COMPUTING INTERSECTION OF FID MARK CONNECTING LINES IN SCAN COORD:
    double rDET = rCrossProd(cS[0], cS[iOpp], cS[iAdj1], cS[iAdj2]);    
    if (abs(rDET) < EPS8) return -13;  // fiducials incorrect position
    double rDETlamda = rCrossProd(cS[0], cS[iAdj1], cS[iAdj1], cS[iAdj2]);    
    double rDETmu = rCrossProd(cS[0], cS[iOpp], cS[iAdj1], cS[0]);
    double rLam = rDETlamda / rDET;      
    double rMu = rDETmu / rDET;    
    if ((rLam <= 0) || (rLam >= 1) || (rMu <= 0) || (rMu >= 1))  return -14;
    // computation done twice for testing purposes:
    cScanPrincPoint.x = cS[iAdj1].x + rMu * (cS[iAdj2].x - cS[iAdj1].x);  
    cScanPrincPoint.y = cS[iAdj1].y + rMu * (cS[iAdj2].y - cS[iAdj1].y);
    cScanPrincPoint.x = cS[0].x + rLam * (cS[iOpp].x - cS[0].x);  
    cScanPrincPoint.y = cS[0].y + rLam * (cS[iOpp].y - cS[0].y);
    rScanPrincPointRow = cScanPrincPoint.x + 0.5;
    rScanPrincPointCol = cScanPrincPoint.y + 0.5; 
    // COMPUTING INTERSECTION OF FID MARK CONNECTING LINES IN PHOTOCOORD:
    Coord cPhotoPrincPoint;
    rDET = rCrossProd(cF[0], cF[iOpp], cF[iAdj1], cF[iAdj2]);    
    if (abs(rDET) < EPS8) return -13;  // fiducials incorrect position
    rDETlamda = rCrossProd(cF[0], cF[iAdj1], cF[iAdj1], cF[iAdj2]);    
    rDETmu = rCrossProd(cF[0], cF[iOpp], cF[iAdj1], cF[0]);
    rLam = rDETlamda / rDET;      
    rMu = rDETmu / rDET;    
    if ((rLam <= 0) || (rLam >= 1) || (rMu <= 0) || (rMu >= 1))  return -14;
    // computation done twice for testing purposes:
    cPhotoPrincPoint.x = cF[iAdj1].x + rMu * (cF[iAdj2].x - cF[iAdj1].x);  
    cPhotoPrincPoint.y = cF[iAdj1].y + rMu * (cF[iAdj2].y - cF[iAdj1].y);
    cPhotoPrincPoint.x = cF[0].x + rLam * (cF[iOpp].x - cF[0].x);  
    cPhotoPrincPoint.y = cF[0].y + rLam * (cF[iOpp].y - cF[0].y);
      
    if ( (iRet = iFindInnerResiduals(iPoints, cF, cS, cPhotoPrincPoint, cScanPrincPoint, cResiduals)) < 0)
	    return iRet;
    
    double numerator =  rDist2(cS[iAdj2], cS[iAdj1]);
    double denomi = rDist2(cF[iAdj2], cF[iAdj1]);
    if (denomi < EPS4) return -13;  // some fiducials less than .1 mm apart
    rPhoto2ScanScale =  sqrt(numerator / denomi);   // RowCols per meter
    return 0;
  }  
  else return -15;   // error code for wrong number of fiducial marks
}

int SolveOrthoPhoto::iFindAffineInnerOrientation(int iPoints, const Coord* cFot,
									const double* rScannedFidRow, const double* rScannedFidCol,
									double& rPhoto2ScanScale, double& rScanPrincPointRow, double& rScanPrincPointCol,
									Coord& cScanPrincPoint, Coord cResiduals[])
{
	int iRet = 0; // error code
	if (iPoints < 3) return -12;             // at least 2 fiducial marks needed
                            // return value after residuals calculated (0 if succes)
	int N = 2 * iPoints; // nr of equations (overdetermined if iPoints == 3 or 4 or more
	RealMatrix M(N, 6);	// 6 unknowns (a00,a01,a10,a11, b1, b2) of the affine trnsf matrix)
	CVector b(N);				// system of N equations to solve: 
	CVector v(6);				//  Mv = b
	Coord cPhotoPrincPoint;
	Coord cS[8], cF[8];
	
	// DETERMINE CENTROIDS TO MAKE REDUCED COORDINATE SYSTEMS
	Coord cScanCentroid = Coord(0,0);
	bool fInvalidFiducial;
	for (int i = 0; i < iPoints; ++i) 
	{ // find centroid of fiducials
		cS[i].x = rScannedFidRow[i];
		cS[i].y = rScannedFidCol[i];
		fInvalidFiducial = (abs(cS[i].x)>100000||abs(cS[i].y)>100000); 
		cF[i].x = cFot[i].x;
		cF[i].y = cFot[i].y;
		if( fInvalidFiducial || (abs(cF[i].x)>1000||abs(cF[i].y)>1000))
			return -11;
		cScanCentroid.x += cS[i].x;
		cScanCentroid.y += cS[i].y;
	}
	cScanCentroid.x /= iPoints;
	cScanCentroid.y /= iPoints;
	
	// FIND AFFINE Transf and USE POSITION OF PRINC POINT  FOR 3 OR MORE FIDUCIALS 
	
	for (int i = 0; i < iPoints; ++i) {     // both systems shifted to their resp centroids as origin
		cF[i].x -=  cCameraPPoint.x;			// thus cF origin is replaced by input PrincPnt coordinates 
		cF[i].y -=  cCameraPPoint.y;							
		cS[i].x -=  cScanCentroid.x;							// cS origin is now replaced by cS[] centroid 
		cS[i].y -=  cScanCentroid.y;
	}
		// reduced affine transformation model:	**************************************
			//    S.x =  a00 * F.x  + a01 * F.y + Sx0
			//    S.y =  a10 * F.x  + a11 * F.y + Sy0																				//	**************************************
		// FIND MATRIX OF AFFINE PHOTO TO SCAN(ROWCOL) TRANSFORMATION
	
		/// Sx[i] = Fx[i]*a00 + Fy[i]*a01 + Sx0  for i = 0..n
		/// Sy[i] = Fx[i]*a10 + Fy[i]*a11 + Sy0  for i = 0..n
  iRes = iFindPolynom(3, iPoints, cF, cS, rCoeffRow, rCoeffCol);
	iRes = iFindPolynom(3, iPoints, cS, cF, rCoeffX, rCoeffY);
	for  (int i = 0; i < iPoints; ++i) 
	{	
		M(i, 0)				= cF[i].x;
		M(i, 1)				= cF[i].y;
		M(i + iPoints, 0)	= 0;
		M(i + iPoints, 1)	= 0;
		M(i, 2)				= 0;
		M(i, 3)				= 0;
		M(i + iPoints, 2)	= cF[i].x;
		M(i + iPoints, 3)	= cF[i].y;
		M(i, 4)				= 1;
		M(i, 5)				= 0;
		M(i + iPoints, 4)	= 0;
		M(i + iPoints, 5)	= 1;
		b(i)							= cS[i].x;
		b(i	+ iPoints)		= cS[i].y;	
	}
	v = LeastSquares(M, b);
	
	double rDet = v(0)*v(3) - v(2)*v(1);
	rDet = rCoeffRow[1]*rCoeffCol[2] - rCoeffRow[2]*rCoeffCol[1];
	if (abs(rDet) < EPS14)
		return -14;
	else
		rPhoto2ScanScale =  sqrt(rDet);
	
	rScanPrincPointRow = 
		rCoeffRow[1]*cCameraPPoint.x + rCoeffRow[2]*cCameraPPoint.y + rCoeffRow[0];// use the solved affine matrix 
	rScanPrincPointCol = 
		rCoeffCol[1]*cCameraPPoint.x + rCoeffCol[2]*cCameraPPoint.y + rCoeffCol[0];// to transform camera pp into scanpp
	rScanPrincPointRow += cScanCentroid.x; // and shift back to real rowcol positions
	rScanPrincPointCol +=	cScanCentroid.y;
	cScanPrincPoint = Coord(rScanPrincPointRow,rScanPrincPointCol);
	/*
	rmAffineCamera2Scan(0,0) = rCoeffRow[1];
	rmAffineCamera2Scan(0,1) = rCoeffRow[2];
	rmAffineCamera2Scan(1,0) = rCoeffCol[1];
	rmAffineCamera2Scan(1,1) = rCoeffCol[2];
	rmAffineScan2Camera(0,0) =  v(3)/rDet;//== rCoeffX1
	rmAffineScan2Camera(0,1) = -v(1)/rDet;//== rCoeffX2
	rmAffineScan2Camera(1,0) = -v(2)/rDet;//== rCoeffY1
	rmAffineScan2Camera(1,1) =  v(0)/rDet;//== rCoeffY2 */
	if (iPoints < 4) {
		for (int i = 0; i < 3; ++i) { 
			cResiduals[i].x = 0;          // exact fit, residals == 0;
			cResiduals[i].y = 0;
		}
		return 0;
	}
	else if (iPoints > 3 && iPoints < 9)
	{
		Coord cTrnsf[8];
		for ( int i = 0; i < iPoints; ++i)
		{
			cTrnsf[i].x = rCoeffRow[1]*cF[i].x + rCoeffRow[2]*cF[i].y + rCoeffRow[0];// use the solved affine matrix and shift
			cTrnsf[i].y = rCoeffCol[1]*cF[i].x + rCoeffCol[2]*cF[i].y + rCoeffCol[0];// to transform camera fiducs into scanRowColds
			cResiduals[i].x = cTrnsf[i].x - cS[i].x;  // and find mismatches
			cResiduals[i].y = cTrnsf[i].y - cS[i].y; 
		}
		return 0;
	}
	else return -15;   // error code for wrong number of fiducial marks
}

int SolveOrthoPhoto::iFindConformInnerOrientation(int iPoints, const Coord* cFot,
									const double* rScannedFidRow, const double* rScannedFidCol,
									double& rPhoto2ScanScale, double& rScanPrincPointRow, double& rScanPrincPointCol,
									Coord& cScanPrincPoint, Coord cResiduals[])
{
	int iRet = 0; // error code
	if (iPoints < 2) return -12;             // at least 2 fiducial marks needed
                            // return value after residuals calculated (0 if succes)
	int N = 2 * iPoints; // nr of equations (overdetermined if iPoints == 3 or more
	RealMatrix M(N, 4);	// 4 unknowns (a, b, c1, c2) of the affine trnsf matrix)
	CVector b(N);				// system of N equations to solve: 
	CVector v(4);				//  Mv = b
	Coord cPhotoPrincPoint;
	Coord cS[8], cF[8];
	
	// DETERMINE CENTROIDS TO MAKE REDUCED COORDINATE SYSTEMS
	Coord cScanCentroid = Coord(0,0);
	bool fInvalidFiducial;
	for (int i = 0; i < iPoints; ++i) 
	{ // find centroid of fiducials
		cS[i].x = rScannedFidRow[i];
		cS[i].y = rScannedFidCol[i];
		fInvalidFiducial = (abs(cS[i].x)>100000||abs(cS[i].y)>100000); 
		cF[i].x = cFot[i].x;
		cF[i].y = cFot[i].y;
		if( fInvalidFiducial || (abs(cF[i].x)>1000||abs(cF[i].y)>1000))
			return -11;
		cScanCentroid.x += cS[i].x;
		cScanCentroid.y += cS[i].y;
	}
	cScanCentroid.x /= iPoints;
	cScanCentroid.y /= iPoints;
	
	// FIND CONFORM Transf and USE POSITION OF PRINC POINT  FOR 2 OR MORE FIDUCIALS 
	
	for (int i = 0; i < iPoints; ++i) {     // both systems shifted to their resp centroids as origin
		cF[i].x -=  cCameraPPoint.x;			// thus cF origin is replaced by input PrincPnt coordinates 
		cF[i].y -=  cCameraPPoint.y;							
		cS[i].x -=  cScanCentroid.x;							// cS origin is now replaced by cS[] centroid 
		cS[i].y -=  cScanCentroid.y;
	}
		// reduced conformal transformation model:	**************************************
			//    S.x =  a * F.x  + b * F.y + Sx0
			//    S.y =  -b * F.x  + a * F.y + Sy0																				//	**************************************
		// FIND MATRIX OF AFFINE PHOTO TO SCAN(ROWCOL) TRANSFORMATION
	
		/// Sx[i] = Fx[i]*a + Fy[i]*b + Sx0  for i = 0..n
		/// Sy[i] = -Fx[i]*b + Fy[i]*a + Sy0  for i = 0..n
	
	for  (int i = 0; i < iPoints; ++i) 
	{	
		M(i, 0)				= cF[i].x;
		M(i, 1)				= cF[i].y;
		M(i + iPoints, 0)	= cF[i].y;
		M(i + iPoints, 1)	= -cF[i].x;
		M(i, 2)				= 1;
		M(i, 3)				= 0;
		M(i + iPoints, 2)	= 0;
		M(i + iPoints, 3)	= 1;
		b(i)							= cS[i].x;
		b(i	+ iPoints)		= cS[i].y;	
	}
	v = LeastSquares(M, b);
	
	double rDet = v(0)*v(0) + v(1)*v(1);
	if (abs(rDet) < EPS14)
		return -14;
	else
		rPhoto2ScanScale =  sqrt(rDet);
	
	rScanPrincPointRow = 
		v(0)*cCameraPPoint.x + v(1)*cCameraPPoint.y + v(2);// use the solved conformal matrix 
	rScanPrincPointCol = 
		-v(1)*cCameraPPoint.x + v(0)*cCameraPPoint.y + v(3);// to transform camera pp into scanpp
	rScanPrincPointRow += cScanCentroid.x; // and shift back to real rowcol positions
	rScanPrincPointCol +=	cScanCentroid.y;
	cScanPrincPoint = Coord(rScanPrincPointRow,rScanPrincPointCol);
	
	rCoeffRow[1] = v(0);
	rCoeffRow[2] = v(1);
	rCoeffCol[1] = -v(1);
	rCoeffCol[2] = v(0);
	rCoeffX[1] = v(0)/rDet;
	rCoeffX[2] = -v(1)/rDet;
	rCoeffY[1] = v(1)/rDet;
	rCoeffY[2] = v(0)/rDet;
	/*
	rmAffineCamera2Scan(0,0) = v(0);
	rmAffineCamera2Scan(0,1) = v(1);
	rmAffineCamera2Scan(1,0) = -v(1);
	rmAffineCamera2Scan(1,1) = v(0);
	rmAffineScan2Camera(0,0) =  v(0)/rDet;
	rmAffineScan2Camera(0,1) = -v(1)/rDet;
	rmAffineScan2Camera(1,0) = v(1)/rDet;
	rmAffineScan2Camera(1,1) =  v(0)/rDet; */
	if (iPoints < 3) {
		for (int i = 0; i < 3; ++i) { 
			cResiduals[i].x = 0;          // exact fit, residals == 0;
			cResiduals[i].y = 0;
		}
		return 0;
	}
	else if (iPoints > 2 && iPoints < 9)
	{
		Coord cTrnsf[8];
		for ( int i = 0; i < iPoints; ++i)
		{
			cTrnsf[i].x = v(0)*cF[i].x + v(1)*cF[i].y + v(2);// use the solved conformal matrix and shift
			cTrnsf[i].y = -v(1)*cF[i].x + v(0)*cF[i].y + v(3);// to transform camera fiducs into scanRowColds
			cResiduals[i].x = cTrnsf[i].x - cS[i].x;  // and find mismatches
			cResiduals[i].y = cTrnsf[i].y - cS[i].y; 
		}
		return 0;
	}
	else return -15;   // error code for wrong number of fiducial marks
}

int SolveOrthoPhoto::iComputeInnerOrientation(int iPoints, const Coord* cFot, const double* rScannedFidRow, const double* rScannedFidCol,
																				double& rPhoto2ScanScale, double& rScanPrincPointRow, double& rScanPrincPointCol,
																			 Coord& cScanPrincPoint, Coord cResiduals[])
{// FOR BILINEAR AND PROJECTIVE transformation
	int iRet = 0; // error code
	if (iPoints < 4) return -12;             // at least 2 fiducial marks needed
                            // return value after residuals calculated (0 if succes)
	int N = 2 * iPoints; // nr of equations (overdetermined if iPoints == 3 or 4 or more
	Coord cPhotoPrincPoint;
	Coord cS[8], cF[8];
	
	// DETERMINE CENTROIDS TO MAKE REDUCED COORDINATE SYSTEMS
	Coord cScanCentroid = Coord(0,0);
	bool fInvalidFiducial;
	for (int i = 0; i < iPoints; ++i) 
	{ // find centroid of fiducials
		cS[i].x = rScannedFidRow[i];
		cS[i].y = rScannedFidCol[i];
		fInvalidFiducial = (abs(cS[i].x)>100000||abs(cS[i].y)>100000); 
		cF[i].x = cFot[i].x;
		cF[i].y = cFot[i].y;
		if( fInvalidFiducial || (abs(cF[i].x)>1000||abs(cF[i].y)>1000))
			return -11;
		cScanCentroid.x += cS[i].x;
		cScanCentroid.y += cS[i].y;
	}
	cScanCentroid.x /= iPoints;
	cScanCentroid.y /= iPoints;
	
	// FIND BILINEAR Transf and USE POSITION OF PRINC POINT  FOR 4 OR MORE FIDUCIALS 
	
	for (int i = 0; i < iPoints; ++i) {     // both systems shifted to their resp centroids as origin
		cF[i].x -=  cCameraPPoint.x;			// thus cF origin is replaced by input PrincPnt coordinates 
		cF[i].y -=  cCameraPPoint.y;							
		cS[i].x -=  cScanCentroid.x;							// cS origin is now replaced by cS[] centroid 
		cS[i].y -=  cScanCentroid.y;
	}
	
	if (transfFid == BILINEAR) {
		// reduced bilineartransformation model: from camera to scan **********************
			//    S.x =  rCoeffRow0 + rCoeffRow1 * F.x  + rCoeffRow2 * F.y + rCoeffRow3 * F.x * F.y 
			//    S.y =  rCoeffCol0 + rCoeffCol1 * F.x  + rCoeffCol2 * F.y + rCoeffCol3 * F.x * F.y																				
	//	**************************************
		iRes = iFindPolynom(4, iPoints, cF, cS, rCoeffRow, rCoeffCol);
		// reduced inverse bilineartransformation model: from scan to camera **************
			//    F.x =  rCoeffX0 + rCoeffX1 * S.x  + rCoeffX2 * S.y + rCoeffX3 * S.x * S.y 
			//    F.y =  rCoeffY0 + rCoeffY1 * S.x  + rCoeffY2 * S.y + rCoeffY3 * S.x * S.y																				
	//	**************************************
		iRes = iFindPolynom(4, iPoints, cS, cF, rCoeffX, rCoeffY);

		double rDet = rCoeffRow[1]*rCoeffCol[2] - rCoeffRow[2]*rCoeffCol[1];
		if (abs(rDet) < EPS14)
			return -14;
		else
			rPhoto2ScanScale =  sqrt(rDet);
		// use the coeffs to transform camera pp into scanpp:
		rScanPrincPointRow = 
			rCoeffRow[0] + rCoeffRow[1]*cCameraPPoint.x + rCoeffRow[2]*cCameraPPoint.y 
			+ rCoeffRow[3]*cCameraPPoint.x*cCameraPPoint.y;
		rScanPrincPointCol = 
			rCoeffCol[0] + rCoeffCol[1]*cCameraPPoint.x + rCoeffCol[2]*cCameraPPoint.y 
			+ rCoeffCol[3]*cCameraPPoint.x*cCameraPPoint.y;// 
		rScanPrincPointRow += cScanCentroid.y; // and shift back to real rowcol positions
		rScanPrincPointCol +=	cScanCentroid.x;
		cScanPrincPoint = Coord(rScanPrincPointRow,rScanPrincPointCol);

		if (iPoints < 5) {
			for (int i = 0; i < 4; ++i) { 
				cResiduals[i].x = 0;          // exact fit, residals == 0;
				cResiduals[i].y = 0;
			}
			return 0;
		}
		else if (iPoints > 4 && iPoints < 9)
		{
			double rRowTrnsf[8], rColTrnsf[8];//use the coeffs to transform camera fiducs into scanRowColds:
			for ( int i = 0; i < iPoints; ++i)
			{
				rRowTrnsf[i] = rCoeffRow[0] + rCoeffRow[1]*cF[i].x + rCoeffRow[2]*cF[i].y + rCoeffRow[3]*cF[i].x*cF[i].y;
				rColTrnsf[i] = rCoeffCol[0] + rCoeffCol[1]*cF[i].x + rCoeffCol[2]*cF[i].y + rCoeffCol[3]*cF[i].x*cF[i].y;
				cResiduals[i].x = rRowTrnsf[i] - cS[i].x;  // and find mismatches
				cResiduals[i].y = rColTrnsf[i] - cS[i].y; 
			}
			return 0;
		}
	}
	else if (transfFid == PROJECTIVE) {
		// reduced projective transformation model:	**************************************
		// S.y = (rCoeffY0 * F.x + rCoeffY1 * F.y + rCoeffY2 ) / (rCoeffY6 * F.x + rCoeffY7 * F.y + 1)
		// S.x = (rCoeffY3 * F.x + rCoeffY4 * F.y + rCoeffY5) / (rCoeffY6 * F.x + rCoeffY7 * F.y + 1)																		
	//	**************************************
		iRes = iFindOblique(iPoints, cF, cS, rCoeffY);
    if (iRes == 0)
		 iRes = iFindOblique(iPoints, cS, cF, rCoeffRefY);
		double rDet = rCoeffY[0]*rCoeffY[4] - rCoeffY[3]*rCoeffY[1];
		if (abs(rDet) < EPS14)
			return -14;
		else
			rPhoto2ScanScale =  sqrt(rDet);
		// use the coeffs to transform camera pp into scanpp:
		rScanPrincPointRow = 
			(rCoeffY[0]*cCameraPPoint.x + rCoeffY[1]*cCameraPPoint.y + rCoeffY[2]) /
			(rCoeffY[6]*cCameraPPoint.x + rCoeffY[7]*cCameraPPoint.y + 1);
		rScanPrincPointCol = 
			(rCoeffY[3]*cCameraPPoint.x + rCoeffY[4]*cCameraPPoint.y + rCoeffY[5]) /
			(rCoeffY[6]*cCameraPPoint.x + rCoeffY[7]*cCameraPPoint.y + 1); 
		rScanPrincPointRow += cScanCentroid.y; // and shift back to real rowcol positions
		rScanPrincPointCol +=	cScanCentroid.x;
		cScanPrincPoint = Coord(rScanPrincPointRow,rScanPrincPointCol);

		if (iPoints < 5) {
			for (int i = 0; i < 4; ++i) { 
				cResiduals[i].x = 0;          // exact fit, residals == 0;
				cResiduals[i].y = 0;
			}
			return 0;
		}
		else if (iPoints > 4 && iPoints < 9)
		{
			double rRowTrnsf[8], rColTrnsf[8];//use the coeffs to transform camera fiducs into scanRowColds:
			for ( int i = 0; i < iPoints; ++i)
			{
				rRowTrnsf[i] = (rCoeffY[0]*cF[i].x + rCoeffY[1]*cF[i].y + rCoeffY[2]) /
												(rCoeffY[6]*cF[i].x + rCoeffY[7]*cF[i].y + 1);
				rColTrnsf[i] = (rCoeffY[3]*cF[i].x + rCoeffY[4]*cF[i].y + rCoeffY[5]) /
												(rCoeffY[6]*cF[i].x + rCoeffY[7]*cF[i].y + 1); 
				cResiduals[i].x = rRowTrnsf[i] - cS[i].x;  // and find mismatches
				cResiduals[i].y = rColTrnsf[i] - cS[i].y; 
			}
			return 0;
		}
	}
	
	else return -15;   // error code for wrong number of fiducial marks
	return 0;
}

int SolveOrthoPhoto::iFindOuterOrientation(int iGroundCtrlPnts, double rScanPrincDistance, const Map& mDTM,
               const Coord* Depen,                      // Depen = photo coord in scan ,
               const Coord* Indep,  const double* crdZ, // Indep = terrainXY, crdZ = terrainZ
               Coord cScanPrincPoint,                 // pos of PP in scan using double precision
               RealMatrix& rmRotx2u, RealMatrix& rmRotu2x,
               CVector& vecProjC, double& rAvgScale, double& rKa, double& rPh, double& rOm,
               int iMaxOrientCount, int& iIterations)
{
  double rMaxTerrainHeight = mDTM->rrMinMax(true).rHi();
  double rKappa, rPhi, rOmega;        // approximate rotation angles, camera tilt
  RealMatrix rmRotation(3);
  double rU0, rV0, rW0;               // approx.coord of proj center in terrainsystem (meters)
  double rDU0, rDV0, rDW0;            // corrections to position of proj center in terrainsystem (meters)
  double rFFi, rGGi;
  double rfi, rgi; 
  double rDKap, rDPhi, rDOme;         // corrections to rotation angles of camera   
  // ****** make rmUnit, a unit  3 X 3 matrix to construct rot matrices
  int i,j;
  RealMatrix rmUnit(3);
  for (i=0; i<3; i++) {   //
    for (j=0; j<3; j++) {
       rmUnit(i,j) = 0;
    if (i==j) rmUnit(i,j) = 1; 
    }
  }  
  /// end rmUnit
  //  ******
  if (iGroundCtrlPnts < 3) return -2;      // at least 3 points in both crd systems needed
  // Start of iterative search for rmRot and vecProjC
  
  Array<Coord> cScannedPhoto, cUV ;  // camera-photoplane and 2d terrain coordinates resp
  cScannedPhoto.Resize(iGroundCtrlPnts); cUV.Resize(iGroundCtrlPnts); 
  double rSumCamX = 0;
  double rSumCamY = 0;
  double rSumU = 0;
  double rSumV = 0;
  double rSumW = 0;
	for (i = 0; i < iGroundCtrlPnts; ++i) { 
    cScannedPhoto[i].x = Depen[i].x - cScanPrincPoint.x;  // setting coordinates from scan
    cScannedPhoto[i].y = Depen[i].y - cScanPrincPoint.y ;  // relative to princ point in scan as origin
	}
	//$$$$$$$$$$$$$$$$$$$$$   Here the rmAffineScan2Camera matrix and cCameraPPoint must be 
	//$$$$$$$$$$$$$$$$$$$$$   applied in order to give Depen[i] the real camera coordinates:
	Array<Coord> cCam;
	cCam.Resize(iGroundCtrlPnts);
	for (i = 0; i < iGroundCtrlPnts; ++i) { 
		if (transfFid == CONFORM || transfFid == AFFINE) {
			cCam[i].x = cScannedPhoto[i].x * rCoeffX[1] 
									+ cScannedPhoto[i].y * rCoeffX[2] + cCameraPPoint.x;
			cCam[i].y = cScannedPhoto[i].x * rCoeffY[1] 
									+ cScannedPhoto[i].y * rCoeffY[2] + cCameraPPoint.y;
		}
		else if (transfFid == BILINEAR) {
			cCam[i].x = rCoeffX[0] + rCoeffX[1]*cScannedPhoto[i].x +  rCoeffX[2]*cScannedPhoto[i].y 
											+ rCoeffX[3]*cScannedPhoto[i].x*cScannedPhoto[i].y + cCameraPPoint.x; 
			cCam[i].y = rCoeffY[0] + rCoeffY[1]*cScannedPhoto[i].x +  rCoeffY[2]*cScannedPhoto[i].y 
											+ rCoeffY[3]*cScannedPhoto[i].x*cScannedPhoto[i].y + cCameraPPoint.y;
		}
		else if (transfFid == PROJECTIVE) {
			cCam[i].x = rCoeffRefY[0]*cScannedPhoto[i].x + rCoeffRefY[1]*cScannedPhoto[i].y +  rCoeffRefY[2] /
									(rCoeffRefY[6]*cScannedPhoto[i].x+ rCoeffRefY[6]*cScannedPhoto[i].y + 1); 
			cCam[i].y = rCoeffRefY[3]*cScannedPhoto[i].x  + rCoeffRefY[4]*cScannedPhoto[i].y +  rCoeffRefY[5] /
									(rCoeffRefY[6]*cScannedPhoto[i].x+ rCoeffRefY[6]*cScannedPhoto[i].y + 1); 
		}
     // computing sum of coord values (for the "centers of gravity") :
    rSumCamX += cCam[i].x ;          // in photo plane
    rSumCamX += cCam[i].y ;
    
    cUV[i] = Indep[i];                 // collecting terrain coord   
    rSumU += Indep[i].x;               // computing sum of terrain coord
    rSumV += Indep[i].y;
    rSumW += crdZ[i];      //            
  }
  double rAvgCamX = rSumCamX / iGroundCtrlPnts;
  double rAvgCamY = rSumCamY / iGroundCtrlPnts;
  double rAvgU = rSumU / iGroundCtrlPnts;
  double rAvgV = rSumV / iGroundCtrlPnts;
  double rAvgW = rSumW / iGroundCtrlPnts;
	Array<Coord> cCam0;
	cCam0.Resize(iGroundCtrlPnts);
  for (i = 0; i < iGroundCtrlPnts; ++i) { // both systems shifted to their resp center of gravity
    cCam0[i] = cCam[i];
		cCam0[i].x -= rAvgCamX;
    cCam0[i].y -= rAvgCamY;
    cUV[i].x -= rAvgU;
    cUV[i].y -= rAvgV;
  }
  // ******
  // computation of estimated linear conformal transf: xy ScannedPhoto --> UVterrain
  //    U =  a * cCam.x  - b * cCam.y + U0
  //    V =  b * cCam.x  + a * cCam.y + V0
    
  double rXX, rYY, rXU, rXV, rYU, rYV;
  rXX = rYY = rXU = rXV = rYU = rYV = 0;
  for (i = 0; i < iGroundCtrlPnts; ++i) {
    rXX += cCam0[i].x * cCam0[i].x;
    rYY += cCam0[i].y * cCam0[i].y;
    rXU += cCam0[i].x * cUV[i].x;
    rYU += cCam0[i].y * cUV[i].x;
    rXV += cCam0[i].x * cUV[i].y;
    rYV += cCam0[i].y * cUV[i].y;   
  }
  double denom = rXX + rYY;
  if (abs(denom) < EPS8) return -3; // error code for singular case  used in message (in src\dsp\service\grfedit.c
  double rA = (rXU + rYV) / denom; 
  double rB = (rXV - rYU) / denom;                 
  rAvgScale = sqrt(rA*rA + rB*rB);    // approx scale in meters per rowcol unit in scanned image
  
  // ******
  //  initialize iteration with start guesses
  rU0 = rAvgU - rA * rAvgCamX + rB * rAvgCamY;         // first guess of 
  rV0 = rAvgV - rB * rAvgCamX - rA * rAvgCamY;         // projection center
  //rW0 = rAvgW + rScanPrincDistance * rAvgScale;          // coordinates in terrain system
	rW0 = rAvgW + rPrincDistance * rAvgScale; // based on camera-crd system
  if (rW0 - rMaxTerrainHeight < 10) return -4;     // computed DTM values are too high w.r.t. ProjCenter 
  rKappa = atan2( rB , rA);                              // first guess of kappa rotation angle
  rPhi = 0.0;                       // start value zero
  rOmega = 0.0;                     // start value zero
  rDU0 = 1000.0;
  rDV0 = 1000.0;               // initialize proj center shifts
  rDW0 = 1000.0;               // at large values (1 km) to enter while loop      
  rDKap = 1.0;
  rDPhi = 1.0;                 // initialize rot angle corrections
  rDOme = 1.0;                 // at large values (1 radian) to enter while loop
        
  // iterative loop to find the six orientation parameters U0, V0, W0, (projection center)
  //  and Kappa, Phi, Omega (rotation of the camera with respect to terrain system
  //  Sources:  K.A. Grabmaier : Lecture Notes "Production of Ortho-images and Image maps", 1995
  //                & Lecture Notes on Orientation Theory, "Single Bundle Resection" , ITC Enschede NL
  
  Array<CVector> vecPhoto(iGroundCtrlPnts);                // array of 3D points in photo plane, iGroundCtrlPnts many 
  Array<CVector> vecPhotoRotated(iGroundCtrlPnts);         // idem rotated by KappaPhiOmegaMatrix
  Array<double> rU, rV, rW;                 //terrain coordinates as arrays with iGroundCtrlPnts places
  for  (i = 0; i < iGroundCtrlPnts; ++i) {
    vecPhoto[i] =  CVector(3);           //  give the camera vectors dimension 3
    vecPhotoRotated[i] = CVector(3);     //  also those rotated toward UVW system
  }
  //
  rU.Resize(iGroundCtrlPnts);
  rV.Resize(iGroundCtrlPnts);
  rW.Resize(iGroundCtrlPnts);

  for  (i = 0; i < iGroundCtrlPnts; ++i) {
      vecPhoto[i](0) = cCam[i].x - cCameraPPoint.x;
      vecPhoto[i](1) = cCam[i].y - cCameraPPoint.y ;
      // mirror reflection of Y coords to make scan (photo) coord syst right handed (like terrain syst)
      vecPhoto[i](2) = - rPrincDistance;   
    
      rU[i] = Indep[i].x;
      rV[i] = Indep[i].y;   
      rW[i] = crdZ[i];
  }    
  
  int N = 2 * iGroundCtrlPnts;
  RealMatrix A(N, 6);
  CVector b(N);
  
  iIterations = 0;
  double rWiMinW0_inv;
  double rWAvgMinW0 = abs(rAvgW - rW0);     // scale factor to equilibrate Epsilons and 
  double EPSshift = EPS8 * rWAvgMinW0;      // shift columns (0,1,2) of condition matrix A
  while (((abs(rDU0)  > EPSshift) || (abs(rDV0) > EPSshift) || (abs(rDW0) > EPSshift) ||
         (abs(rDKap)  > EPS8) || (abs(rDPhi) > EPS8) || (abs(rDOme) > EPS8))
         && (iIterations < iMaxOrientCount) ) {   
    rmRotation = rmMakeKappaPhiOmegaMatrix(rKappa, rPhi, rOmega, rmUnit);
    iIterations++;  
    for  (i = 0; i < iGroundCtrlPnts; ++i) {
      rWiMinW0_inv = 1.0 / (rW[i] - rW0);
      rFFi = (rU[i] - rU0) * rWiMinW0_inv;
      rGGi = (rV[i] - rV0) * rWiMinW0_inv;
      vecPhotoRotated[i] = (rmRotation * vecPhoto[i]).GetColVec(0);
      rfi = vecPhotoRotated[i](0) / vecPhotoRotated[i](2);
      rgi = vecPhotoRotated[i](1) / vecPhotoRotated[i](2);
      ///vecPhoto[i] = (rmRotation * vecPhoto[i]).GetColVec(0); 
      ///rfi = vecPhoto[i](0) / vecPhoto[i](2);
      ///rgi = vecPhoto[i](1) / vecPhoto[i](2);
      
      A(2*i, 1) = A(2*i+1, 0) = 0.0;
      A(2*i, 0) = A(2*i+1, 1) = rWiMinW0_inv * rWAvgMinW0;
       
      A(2*i, 2) =   -rFFi * rWiMinW0_inv * rWAvgMinW0;    
      A(2*i, 3)   = -rfi * rgi; 
      A(2*i, 4)   =   1.0 + rfi*rfi ; 
      A(2*i, 5)   = -rgi; 
  
      A(2*i+1, 2) = -rGGi * rWiMinW0_inv * rWAvgMinW0;
      A(2*i+1, 3) = -(1.0 + rgi*rgi) ;
      A(2*i+1, 4) = rfi * rgi;
      A(2*i+1, 5) = rfi;
      
      b(2*i)   = rFFi - rfi;
      b(2*i+1) = rGGi - rgi;
    }     // System of correction equations is now ready
          // (overdetermined in case of iGroundCtrlPnts > 3 )      
    CVector v(6); //, v3Points(6);
    RealMatrix Acopy(N,6); 
    Acopy = A; 
    Acopy.Transpose();
    RealMatrix SquareM = Acopy * A;
    //double rDt = SquareM.rDet();
    if ( SquareM.fSingular()) return -3; 
                       // singul matrix in normal eq for orientation parameters
                                         // no reliable solution for b possible
    if (iGroundCtrlPnts == 3) 
      A.Solve(b, v);
    else  
      v = LeastSquares(A, b);
    rDU0 = v(0) * rWAvgMinW0 ;
    rDV0 = v(1) * rWAvgMinW0 ; 
    rDW0 = v(2) * rWAvgMinW0 ;
    rDOme = v(3);
    rDPhi = v(4); 
    rDKap = v(5);
    rU0 += rDU0;         // slow down the corrections by factor 2
    rV0 += rDV0; 
    rW0 += rDW0;
    rKappa += rDKap;
    rPhi   += rDPhi; 
    rOmega += rDOme;
  } // end while iterations not sufficiently converging or less than 10  
  
	rPCu = rU0; 
	rPCv = rV0; 
	rPCw = rW0;
  vecProjC(0) = rU0;
  vecProjC(1) = rV0;
  vecProjC(2) = rW0;   // final position of Projection Center
	RealMatrix rmRot =  rmMakeKappaPhiOmegaMatrix( rKappa, rPhi, rOmega, rmUnit); //final rotation matrix from xyc to uvw
  rmRotx2u = rmRot;
  rmRot.Transpose();
  rmRotu2x = rmRot;
  rKa = rKappa * 180 / M_PI; // angle of image coord system rotated about z axis w.r.t. ground system
  rPh = rPhi * 180 / M_PI;  // phi tilt of aerial photograph w.r.t. horizontal plane about y axis
  rOm = rOmega * 180 / M_PI;  // omegatilt of aerial photograph w.r.t. horizontal plane about x axis
  return 0;
}

RealMatrix SolveOrthoPhoto::rmMakeKappaPhiOmegaMatrix(double k, double f, double w, const RealMatrix& rmUnit)
{
   double rCosKappa, rSinKappa, rCosPhi, rSinPhi, rCosOmega, rSinOmega;
   RealMatrix Kap(rmUnit), Phi(rmUnit), Ome(rmUnit);

   rCosKappa = cos(k); rSinKappa = sin(k); 
   rCosPhi = cos(f);   rSinPhi = sin(f); 
   rCosOmega = cos(w); rSinOmega = sin(w);
   Kap(0,1) = -rSinKappa;
   Kap(1,0) = rSinKappa;  
   Kap(0,0) = Kap(1,1) = rCosKappa;
   Phi(0,2) = rSinPhi;
   Phi(2,0) = -rSinPhi;
   Phi(0,0) = Phi(2,2) = rCosPhi;
   Ome(1,2) = -rSinOmega;
   Ome(2,1) = rSinOmega;
   Ome(1,1) = Ome(2,2) = rCosOmega;
   Ome *= Phi;     // yields Omega * Phi  matrix
   Ome *= Kap;     // yields Omega * Phi * Kappa  matrix
   return Ome;
}   

CVector SolveOrthoPhoto::cvGetProjectionCenter()
{
	//return vecProjCenter;
	CVector v(3);
	v(0) = rPCu;
	v(1) = rPCv;
	v(2) = rPCw;
	return v;
}

CVector SolveOrthoPhoto::cvGetCameraAngles()
{
	CVector cvResult(3);
	cvResult(0) = rKappaFinal;
	cvResult(1) = rPhiFinal;
	cvResult(2) = rOmegaFinal;
	return cvResult;
}
