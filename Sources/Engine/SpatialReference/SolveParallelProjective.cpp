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
/*
	Last change:  JH   24 Sep 99    5:01 pm
*/

#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\SolveParallelProjective.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#define EPS10 1.e-10
#define EPS15 1.e-15
#define SIGN(A) (A>0?1:-1)

SolveParallelProjective::SolveParallelProjective()
{}

int SolveParallelProjective::iFindCoeffsOfParProj(int iPoints,
                        const Coord* Indep, const Coord* Depen, const double* rZ,
			                  Coefs &Coef)
{
  if (iPoints < 4) return -2;
  int N = 2 * iPoints;
  int M = 8;

  RealMatrix A(N, M);
  CVector b(N), vec(M);

  for (int i = 0; i < iPoints; ++i) {
    for (int j = 0; j < M; ++j) {
      A(2*i,j) = 0;
      A(2*i+1,j) = 0;
    }
    A(2*i, 0) = Depen[i].x;
    A(2*i, 1) = Depen[i].y;
    A(2*i, 2) = rZ[i];
    A(2*i, 3) = 1;
   
    A(2*i+1  , 4) = Depen[i].x;
    A(2*i+1  , 5) = Depen[i].y;
    A(2*i+1  , 6) = rZ[i];
    A(2*i+1  , 7) = 1;
   
	b(2*i) = Indep[i].x;
    b(2*i+1  ) = Indep[i].y;
  }

  try {
    vec = LeastSquares(A, b);
    for (int i = 0; i < M; ++i)
      Coef[i] = vec(i);
  }
  catch (ErrorObject&) {
    return -3;
  }                   
  double det = Coef[0]*Coef[5] - Coef[1]*Coef[4];
	if ( abs(det) < EPS15)   // 
	 return -3;
	// Row =  aU + bV + cW + d  = 0  (I)
  // Col =  hU + iV + jW + k  = 0 (II)
	// (I) and (II) define 2 planes whose intersection line runs from
	// the origin of the sensor to a piercing point at Z = 0 ground null-level)
	// this piercing point PP can be found
	double det1 = Coef[3]*Coef[5] - Coef[1]*Coef[7];
	double det2 = Coef[0]*Coef[7] - Coef[3]*Coef[4];
	double rPPx = det1/det;
	double rPPy = det2/det;
	// the line is defined by the outer product of the normal vectors onto (I) and (II)
	// call this 'support' vector (vx, vy, vz)
	double rVx = Coef[1]*Coef[6] - Coef[2]*Coef[5];
	double rVy = - Coef[0]*Coef[6] + Coef[2]*Coef[4];
	double rVz = Coef[0]*Coef[5] - Coef[1]*Coef[4];
	rKu = rVx / rVz; //   
	rKv = rVy / rVz; //
	return 0;
  
}

int SolveParallelProjective::iEstimateAverageScale(int iPoints,
                           const Coord* Indep, const Coord* Depen, double &rScale)
{
  // The Coord2RowCol equations (I) and (II) below,  are simplified (linear conformal)
  // approxim of the direct linear transformation described below
  // They mp U,V terrain coord to Roww,Coll photo coordinates  (2D -> 2D)
  // assuming coordinates of N controlpoints reduced to centers of gravity
  // and assuming photo plane and terrain plane quasi parallel

  // Row =  aU + bV   (I)
  // Col =  hU + iV   (II)
  if (iPoints < 4) return -2;
  int N = 2 * iPoints;
  int M = 2;

  RealMatrix A(N, M);
  CVector b(N);

  for (int i = 0; i < iPoints; ++i) {
    for (int j = 0; j < 2; ++j) {
      A(2*i,j) = 0;
      A(2*i+1,j) = 0;
    }
    A(2*i, 0) = Depen[i].x;
    A(2*i, 1) = Depen[i].y;
    A(2*i+1  , 0) =  Depen[i].y;
    A(2*i+1  , 1) = -Depen[i].x;
    b(2*i) = Indep[i].x;
    b(2*i+1) = Indep[i].y;
  }
  try {
    CVector vec = LeastSquares(A, b);
    double det =  vec(0)*vec(0) + vec(1)*vec(1);
    if (abs(det) < EPS10)
        return -3;
    rScale = 1/(SIGN(det) * sqrt(abs(det))); //meters per RowCol, <0 if mirror-reversed
  }
  catch (ErrorObject& ) {
    return -3;
  }
  return 0;
}

//Coord SolveParallelProjective::crdNullLevelPiercePoint(const double &rRoww, const double &rColl)
//{
  // The Coord2RowCol equations (I) and (II) below are the parallel projection linear transformation
  // from U,V,W terrain coord to Roww,Coll sensor coordinates  (3D -> 2D)

  // Row =  aU + bV + cW + d    (I)
  // Col =  hU + iV + jW + k   (II)
  // Inverting this transformation is possible for W = 0 
	// yields the piercing point on null-level

  // a U + b V = Row - d   (III)
  // h U + i V = Col - k    (IV)
  // compressed into  aU + bV = r                              (V)
  //              and hU + iV = u                              (VI)
  // Using Cramers determinants rule:
  // det = ai - bh ;  detU = ri - bu  and detV = au - rh
  // yields U = detU/det and V = detV / det  provided  det <> 0

Coord SolveParallelProjective::crdInverseOfParallelProjective(const double &rRoww, const double &rColl,
																									const double &rW)
{
  // The Coord2RowCol equations (I) and (II) below are the parallel projection linear transformation
  // from U,V,W terrain coord to Roww,Coll sensor coordinates  (3D -> 2D)

  // Row =  aU + bV + cW + d    (I)
  // Col =  hU + iV + jW + k   (II)
  // Inverting this transformation is possible for an estimated terrain height rW
  // Solving the equations for U and V, if W = rW is given,  yields the following:

  // a U + b V = Row - d - c  * rW    (III)
  // h U + i V = Col - k - j  * rW    (IV)
  // compressed into  aU + bV = r                              (V)
  //              and hU + iV = u                              (VI)
  // Using Cramers determinants rule:
  // det = ai - bh ;  detU = ri - bu  and detV = au - rh
  // yields U = detU/det and V = detV / det  provided  det <> 0
  double a = rCoeff[0];
  double b = rCoeff[1];
  double c = rCoeff[2];
  double d = rCoeff[3];
  
  double h = rCoeff[4];
  double i = rCoeff[5];
  double j = rCoeff[6];
  double k = rCoeff[7];
  double p = a;
  double q = b;
  double r = rRoww - d - c * rW;
  double s = h;
  double t = i;
  double u = rColl - k - j  * rW;
  double rDet = a * i - b * h;
  //  U = ( r * i - b * u ) / rDet;
  //  V = ( a * u - r * h ) / rDet;
  // after lightray param calculation we could use:
  //       U = [(k2*t - k4*q)/det] * W + (k1*t - k3*q)/det = rKu * rW + rDu
  //       V = [(k4*p - k2*s)/det] * W + (k3*p - k1*s)/det = rKv * rW + rDv
  Coord crd;
  if (abs(rDet) < EPS10)
    return crdUNDEF;
  else  {

    crd.x = ( r * i - b * u ) / rDet;
    crd.y = ( a * u - r * h ) / rDet;
    return crd;
  }
}



int SolveParallelProjective::iFindNonCoplanarity(int iPoints,const Coord* crdXY, const double* crdZ)
{
  if (iPoints < 3)
    return -3;
  cActiveCtrl.Resize(iPoints); // array of active ctrlpnt coords
  rVerticDist.Resize(iPoints);  // array with their distances from trend plane
  CVector vecX(3);          // vector of coefficients of trend plane
  RealMatrix matM(iPoints,3);
  CVector vecA(iPoints);    // vector of Z-coords of ctrl-points
  CVector vecMX_A(iPoints); // find residuals from ||MX - A||, root of sumofsquares 
  
  ////***** FIND TREND PLANE FOR ACTIVE CTRL POINTS ******////
  for (int i = 0; i < iPoints; i++)  {
    cActiveCtrl[i].x = crdXY[i].x + rAvgX;
    cActiveCtrl[i].y = crdXY[i].y + rAvgY;
    vecA(i) = crdZ[i];
    matM(i,0) = 1.0;                   // constant factor
    matM(i,1) = crdXY[i].x;                 // X
    matM(i,2) = crdXY[i].y;                 // Y
  }    
  vecX = LeastSquares(matM, vecA); // solve overdet system M*X = A
  for (int i = 0; i < 3; i++ )
  {
    rTrendPlaneCoeff[i] = vecX(i);
  }
  ////***** FIND RESIDUALS (VERTICAL DIST) FOR EACH CTRL POINT ******////
  ////***** FIND RMS FROM SUM OF SQUARED DISTANCES ***/////
  double rSumOfSquares = 0.0;
  for (int i = 0; i < iPoints; i++)  {
    vecMX_A(i) = 0.0;
    for (int j = 0; j < 3; j++)  // compute first M*X :
      vecMX_A(i) += matM(i,j) * vecX(j);
    vecMX_A(i) = vecA(i) - vecMX_A(i);// then A - M*X, this is positive above trendpl
    rVerticDist[i] = vecMX_A(i);
    //rZdiff[i] = rVerticDist[i];
    rSumOfSquares += rVerticDist[i] * rVerticDist[i];
  }
   rNonCoplanarity = sqrt(rSumOfSquares/iPoints);
  
  ////***** FIND TWO OUTLIERS WITH MAX DIST TO TREND-PLANE *****////
  double rDelta = 0;
  double rLack = 0;
  int iN1, iN2;
  for (int i = 0; i < iPoints; i++) {
    rDelta = vecMX_A(i); // vertical distance from trend plane
    if (abs(rDelta) > abs(rLack)) {
      rLack = rDelta;         // keep or take the absol.largest
      iN1 = i;
    }  
  }
  rOut1 = rLack;
  iOutlier1 = iN1 + 1;
  rLack = 0;
  for (int i = 0; i < iPoints; i++) {
    rDelta = abs(vecMX_A(i)); // vertical distance from trend plane
    if (abs(rDelta) > abs(rLack) && i != iN1) {
      rLack = rDelta;     // keep or take the absol. second-largest
      iN2 = i;
    }  
  }  
  rOut2 = rLack;
  iOutlier2 = iN2 + 1;
  rZConfidRange = sqrt(abs(rOut1 * rOut2));
  return 0;
}                              
/*
CVector SolveParallelProjective::cvGetProjectionCenter()
{
	CVector cvResult(3);
	cvResult(0) = rPCu;
	cvResult(1) = rPCv;
	cvResult(2) = rPCw;
	return cvResult;
}

CVector SolveParallelProjective::cvGetCameraAngles()
{
	CVector cvResult(3);
	cvResult(0) = rAlpha;
	cvResult(1) = rBeta;
	cvResult(2) = rGamma;
	return cvResult;
}   */

double SolveParallelProjective::rGetNonCoplanarity()
{
	return rNonCoplanarity;
}

double SolveParallelProjective::rGetZConfidRange()
{
	return rZConfidRange;
}
