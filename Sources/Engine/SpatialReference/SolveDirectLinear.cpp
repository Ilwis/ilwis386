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
#include "Engine\SpatialReference\SolveDirectLinear.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#define EPS10 1.e-10
#define EPS15 1.e-15
#define SIGN(A) (A>0?1:-1)

SolveDirectLinear::SolveDirectLinear()
{}

int SolveDirectLinear::iFindCoeffsOfDirLin(int iPoints,
                        const Coord* Indep, const Coord* Depen, const double* rZ,
			                  Coefs &Coef)
{
  if (iPoints < 6) return -2;
  int N = 2 * iPoints;
  int M = 11;

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
    A(2*i, 8) = - Depen[i].x * Indep[i].x;
    A(2*i, 9) = - Depen[i].y * Indep[i].x;
    A(2*i,10) = - rZ[i] * Indep[i].x;
    A(2*i+1  , 4) = Depen[i].x;
    A(2*i+1  , 5) = Depen[i].y;
    A(2*i+1  , 6) = rZ[i];
    A(2*i+1  , 7) = 1;
    A(2*i+1  , 8) = - Depen[i].x * Indep[i].y;
    A(2*i+1  , 9) = - Depen[i].y * Indep[i].y;
    A(2*i+1 , 10) = - rZ[i] * Indep[i].y;
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
  }                    /*
  // any plane parall to image fulfills: c8*U + c9*V + c10*W + 1 = denom
  // denom can be found for the plane passing thru the contr points center of grav:
  double denom = rCoeff[8] * rAvgX + rCoeff[9] * rAvgY + rCoeff[10] * rAvgZ + 1;
  // thus the equation of this plane yields W as function of U and V :
  // W = (denom - c8*U - c9*V -1)/c10 provided c10 <>0, (no horizontal camera axis)
  // Substiting W in the DirLinear Transform equations we get
  // Roww and Coll as linear functions of U and V
  // this yields an affine trans from U,V coordinates (lying in one paralleltoimage plane)
  // to ColRows image coordates , from which an average scale can be found:  
  // Roww = [c0*U + c1*V + c2*{(denom - c8*U - c9*V -1)/c10} + c3]/denom
  // Coll = [c4*U + c5*V + c6*{(denom - c8*U - c9*V -1)/c10} + c7]/denom  
  // Roww = [(c0-c2*c8/c10)U + (c1-c2*c9/c10)V + c2*{(denom -1)/c10} + c3]/denom
  // Coll = [(c4-c6*c8/c10)U + (c5-c6*c9/c10)V + c6*{(denom -1)/c10} + c7]/denom
  // Roww = (c0-c2*c8/c10)/denom]*U +(c1-c2*c9/c10)/denom)*V + konst1
  // Coll = (c4-c6*c8/c10)/denom]*U +(c5-c6*c9/c10)/denom)*V + konst2
  // The determinant of this affine transf defines an average RowCol per meter scale:
  if (abs(rCoeff[10]) < EPS10)
    return -1;  // image plane perpendicular to horizontal(UV) plane
  if (abs(denom) < EPS10)
    return -3;  // control points center of gravity too close to image plane 
  double rDet = ( (rCoeff[0]-rCoeff[6]*rCoeff[8]/rCoeff[10]) * (rCoeff[5]-rCoeff[6]*rCoeff[9]/rCoeff[10]) 
                 -(rCoeff[4]-rCoeff[6]*rCoeff[8]/rCoeff[10]) * (rCoeff[1]-rCoeff[2]*rCoeff[9]/rCoeff[10]))/denom*denom;
  if (abs(rDet) < EPS10)
    return -3;  // positions of control points don't allow coeff computation
  else
    rAvgScale = sqrt(rDet);                 */
 
  double rNN = sqrt(Coef[8]*Coef[8]+Coef[9]*Coef[9]+Coef[10]*Coef[10]);
  if ( rNN < EPS10 )
    return -3;
  else  {
    double cosC = Coef[10]/rNN;
    double cosB = Coef[9]/rNN;
    double cosA = Coef[8]/rNN;

    if ( abs(abs(cosC)-1) > .1 )   // camera tilt too much, more then 25 degr
     return -3;
    rAlpha = acos(cosA);  // camera axis angle with U-axis (X terrain)
    rBeta = acos(cosB);   // camera axis angle with V-axis (Y terrain)
    rGamma = acos(cosC);  // camera axis angle with W-axis (Z terrain) vertical           */
    return 0;
  }
}

int SolveDirectLinear::iEstimateAverageScale(int iPoints,
                           const Coord* Indep, const Coord* Depen, double &rScale)
{
  // The Coord2RowCol equations (I) and (II) below,  are simplified (linear conformal)
  // approxim of the direct linear transformation described below
  // They mp U,V terrain coord to Roww,Coll photo coordinates  (2D -> 2D)
  // assuming coordinates of N controlpoints reduced to centers of gravity
  // and assuming photo plane and terrain plane quasi parallel

  // Row =  aU + bV   (I)
  // Col =  hU + iV   (II)
  if (iPoints < 6) return -2;
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

Coord SolveDirectLinear::crdInverseOfDirectLinear(const double &rRoww, const double &rColl,
																									const double &rW)
{
  // The Coord2RowCol equations (I) and (II) below are the so-called direct linear transformation
  // from U,V,W terrain coord to Roww,Coll photo coordinates  (3D -> 2D)

  // Row = ( aU + bV + cW + d ) / ( eU + fV + gW + 1)   (I)
  // Col = ( hU + iV + jW + k ) / ( eU + fV + gW + 1)   (II)
  // Inverting this transformation is possible for an estimated terrain height rW
  // Solving the equations for U and V, if W = rW is given,  yields the following:

  // ( a - eRow)U + (b - fRow)V = Row - d - (c - gRow) * rW    (III)
  // ( h - eCol)U + (i - fCol)V = Col - k - (j - gCol) * rW    (IV)
  // compressed into  pU + qV = r                              (V)
  //              and sU + tV = u                              (VI)
  // Using Cramers determinants rule:
  // det = pt - qs ;  detU = rt - qu  and detV = pu - rs
  // yields U = detU/det and V = detV / det  provided  det <> 0
  double a = rCoeff[0];
  double b = rCoeff[1];
  double c = rCoeff[2];
  double d = rCoeff[3];
  double e = rCoeff[8];
  double f = rCoeff[9];
  double g = rCoeff[10];
  double h = rCoeff[4];
  double i = rCoeff[5];
  double j = rCoeff[6];
  double k = rCoeff[7];
  double p = a - e * rRoww;
  double q = b - f * rRoww;
  double r = rRoww - d - (c - g * rRoww) * rW;
  double s = h - e * rColl;
  double t = i - f * rColl;
  double u = rColl - k - (j - g * rColl) * rW;
  double rDet = p * t - q * s;
  //  U = ( r * t - q * u ) / rDet;
  //  V = ( p * u - r * s ) / rDet;
  // after lightray param calculation we could use:
  //       U = [(k2*t - k4*q)/det] * W + (k1*t - k3*q)/det = rKu * rW + rDu
  //       V = [(k4*p - k2*s)/det] * W + (k3*p - k1*s)/det = rKv * rW + rDv
  Coord crd;
  if (abs(rDet) < EPS10)
    return crdUNDEF;
  else  {

    crd.x = ( r * t - q * u ) / rDet;
    crd.y = ( p * u - r * s ) / rDet;
    return crd;
  }
}

int SolveDirectLinear:: iFindProjectionCenter()
{
  // Roww = ( aU + bV + cW + d ) / ( eU + fV + gW + 1)   (I)
  // Coll = ( hU + iV + jW + k ) / ( eU + fV + gW + 1)   (II)
  //  aU + bV + cW + d = 0  plane of terrain points projected on line Roww = 0
  //  hU + iV + jW + k = 0  plane of terrain points projected on line Coll = 0
  //  eU + fV + gW + 1 = 0  plane of terrain points projected on line RowCol = infinity
  double a = rCoeff[0];
  double b = rCoeff[1];
  double c = rCoeff[2];
  double d = rCoeff[3];
  double e = rCoeff[8];
  double f = rCoeff[9];
  double g = rCoeff[10];
  double h = rCoeff[4];
  double i = rCoeff[5];
  double j = rCoeff[6];
  double k = rCoeff[7];
  double rDet = a*i*g + b*j*e + c*h*f - c*i*e - b*h*g - a*j*f;
  if (abs(rDet) < EPS10) 
    return -1; // impossible to find Proj Center due to position of the mentioned planes
  double rDetU = -d*i*g - b*j - c*k*f + c*i + b*k*g + d*j*f;
  double rDetV = -a*k*g - d*j*e - c*h + c*k*e + d*h*g + a*j;
  double rDetW = -a*i - b*k*e - d*h*f + d*i*e + b*h + a*k*f;
  rPCu = rDetU/rDet;  // Cramer's rule for 3 linear equations
  rPCv = rDetV/rDet;
  rPCw = rDetW/rDet;
  rPCu += rAvgX;  // shift back to center of gravity of ctrl points
  rPCv += rAvgY;
  rPCw += rAvgZ;
  return 0;
}


int SolveDirectLinear::iFindLightRayParameters(const double &rRow, const double &rCol)
{
  // Roww = ( aU + bV + cW + d ) / ( eU + fV + gW + 1)   (I)
  // Coll = ( hU + iV + jW + k ) / ( eU + fV + gW + 1)   (II)
  // Solving the equations for U and V, if Roww and Coll given,  yields the following:

  // ( a - eRow)U + (b - fRow)V = Roww - d - (c - gRow) * W    (III)
  // ( h - eCol)U + (i - fCol)V = Coll - k - (j - gCol) * W    (IV)
  // Compressed and writing right-hand sides as functions of W:
  //   pU + qV = k1 + k2W   = r
  //   sU + tV = k3 + k4W   = u
  // Using Cramers determinants rule:
  // det = pt - qs ;
  // detU = (k1 + k2W)t - q(k3 + k4W)  = k1*t - k3*q + (k2*t - k4*q)W
  // detV = (k3 + k4W)p - s(k1 + k2W)  = k3*p - k1*s + (k4*p - k2*s)W
  // yields U = detU/det and V = detV / det  provided  det <> 0
  // hence U = [(k2*t - k4*q)/det] * W + (k1*t - k3*q)/det
  //       V = [(k4*p - k2*s)/det] * W + (k3*p - k1*s)/det
  double a = rCoeff[0];
  double b = rCoeff[1];
  double c = rCoeff[2];
  double d = rCoeff[3];
  double e = rCoeff[8];
  double f = rCoeff[9];
  double g = rCoeff[10];
  double h = rCoeff[4];
  double i = rCoeff[5];
  double j = rCoeff[6];
  double k = rCoeff[7];
    // Solving members rKu rKv (light ray slopes) and rDu rDv (piercing point
      //  with W = rAvgZ, reduced ground plane) if rRow and rCol are given:
  double rRoww = rRow - rAvgRow;  // convert from true
  double rColl = rCol - rAvgCol;  // to reduced RowCols
  double p = a - e * rRoww;       // as assumed in equat I and II
  double q = b - f * rRoww;
  double k1 = rRoww - d;
  double k2 = g * rRoww - c;
  double s = h - e * rColl;
  double t = i - f * rColl;
  double k3 = rColl - k;
  double k4 = g * rColl - j;
  double rDet = p * t - q * s;
  if (abs(rDet) < EPS10)
    return -3;
  rKu = (k2*t - k4*q)/rDet;
  rKv = (k4*p - k2*s)/rDet;
  rDu = (k1*t - k3*q)/rDet; 
  rDv = (k3*p - k1*s)/rDet;
      
  // alternative way of solving  
  // U = [(rRoww - d - (c - g * rRoww) * rW) * t - q * (rColl - k - (j - g * rColl) * rW)]/rDet
  //   = [g * (t * rRoww - q * rColl) - c * t + q * j]/rDet * rW
  //           + [(rRoww - d) * t - q * (rColl - k)]/rDet
  ///  Brief: U = rKu * rW + rDu
  //  rKu = (g * (t * rRoww - q * rColl) - c * t + q * j)/rDet;
  //  rDu = ((rRoww - d) * t - q * (rColl - k))/rDet;
  // V = [p * (rColl - k - (j - g * rColl) * rW) - s * (rRoww - d - (c - g * rRoww) * rW)]/rDet
  //   = [g * (p * rColl - s * rRoww) - p * j + s * c]/rDet * rW
  //           + [(rColl - k) * p - s * (rRoww - d)]/rDet
  ///  Brief: V = rKv * rW + rDv
  //  rKv = (g * (p * rColl - s * rRoww) - p * j + s * c)/rDet;
  //  rDv = ((rColl - k) * p - s * (rRoww - d))/rDet;
  // test for validity of lightray parameters using piercing groundcoord:
  double rDenomtest = ( e*rDu + f*rDv + 1) ;
  double rRowtest = ( a*rDu + b*rDv + d ) / rDenomtest + rAvgRow;  //(I)
  double rColtest = ( h*rDu + i*rDv + k ) / rDenomtest + rAvgCol;  //(II)
  if (abs(rRowtest - rRoww)> 100000 || abs(rColtest - rColl) > 100000 ){
    return -3;
  }  
  rDu += rAvgX - rKu * rAvgZ;  // change to piercing pnt with W = 0
  rDv += rAvgY - rKv * rAvgZ;  // (from reduced  into true ground coords)
    return 0;
}

int SolveDirectLinear::iFindNonCoplanarity(int iPoints,const Coord* crdXY, const double* crdZ)
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
  /*double rLack = 0.0;
  for (i = 0; i < iPoints; i++) 
    rLack += vecMX_A(i) * vecMX_A(i);
  double rt = sqrt(rLack / iPoints); // RMS root mean square */
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

CVector SolveDirectLinear::cvGetProjectionCenter()
{
	CVector cvResult(3);
	cvResult(0) = rPCu;
	cvResult(1) = rPCv;
	cvResult(2) = rPCw;
	return cvResult;
}

CVector SolveDirectLinear::cvGetCameraAngles()
{
	CVector cvResult(3);
	cvResult(0) = rAlpha;
	cvResult(1) = rBeta;
	cvResult(2) = rGamma;
	return cvResult;
}

double SolveDirectLinear::rGetNonCoplanarity()
{
	return rNonCoplanarity;
}

double SolveDirectLinear::rGetZConfidRange()
{
	return rZConfidRange;
}
