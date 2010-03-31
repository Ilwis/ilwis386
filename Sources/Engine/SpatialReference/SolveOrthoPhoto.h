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
/* SolveOrthoPhoto.h
 iFindOrtho (c) Ilwis System Development, ITC
 january 1997,  by Jan Hendrikse
     Last change:  WK   28 Aug 98    4:37 pm
*/

#ifndef ILWSOLVEORT_H
#define ILWSOLVEORT_H

#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\SpatialReference\SolveCameraPosition.h"
#include "Engine\SpatialReference\SolveOblique.h"


#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

class IMPEXP SolveOrthoPhoto: private SolveCameraPosition, private SolveOblique

{
	friend class EditFiducialMarksForm;
protected:
  SolveOrthoPhoto();
  double  rDotProd(Coord cA, Coord cB);

  double rNormSquared(Coord cA);

  //double rCrossProd(RowCol rcA, RowCol rcB, RowCol rcC, RowCol rcD); // vector AB X vector DC
  double rCrossProd(Coord cA, Coord cB, Coord cC, Coord cD); // (vector AB) X (vector DC)

  //bool fPerpProjection(RowCol rcSide1, RowCol rcSide2, RowCol &rcFoot);
  bool fPerpProjection(Coord cSide1, Coord cSide2, Coord &cFoot); // project end point of side2 

  int iFindInnerResiduals( int iPoints, Coord cF[], Coord cS[], 
                         Coord cPhotoPrincPoint, Coord cScanPrincPoint,
                         Coord cD[]); 
                        

  int  iFindInnerOrientation(int iPoints, const Coord* cPhotoFid, const double* rScannedFidRow, const double* rScannedFidCol,
                           double& rPhoto2ScanScale, double& rScanPrincPointRow, double& rScanPrincPointCol,
						   Coord& cScanPrincPoint, Coord cResiduals[]);
	
	int  iFindAffineInnerOrientation(int iPoints, const Coord* cFot, const double* rScannedFidRow, const double* rScannedFidCol,
																	double& rPhoto2ScanScale, double& rScanPrincPointRow, double& rScanPrincPointCol,
																	Coord& cScanPrincPoint, Coord cResiduals[]);

	int iFindConformInnerOrientation(int iPoints, const Coord* cFot, const double* rScannedFidRow, const double* rScannedFidCol,
																	double& rPhoto2ScanScale, double& rScanPrincPointRow, double& rScanPrincPointCol,
																	Coord& cScanPrincPoint, Coord cResiduals[]);
	
	int iComputeInnerOrientation(int iPoints, const Coord* cFot, const double* rScannedFidRow, const double* rScannedFidCol,
																	double& rPhoto2ScanScale, double& rScanPrincPointRow, double& rScanPrincPointCol,
																	Coord& cScanPrincPoint, Coord cResiduals[]);
  
	int iFindOuterOrientation(int iGroundCtrlPnts, double rScanPrincDistance, const Map& mDTM,
               const Coord* Depen, const Coord* Indep, const double* crdZ,
               Coord cScanPrincPoint,          // pos of PP in scan using double precision
               RealMatrix& rmRotx2u, RealMatrix& rmRotu2x,
               CVector& vecProjC, double& rAvgScale, double& rKa, double& rPh, double& rOm,
               int iMaxOrientCount, int& iIterations) ;
               
  RealMatrix rmMakeKappaPhiOmegaMatrix(double k, double f, double w, const RealMatrix& rmUnit);

	double rPrincDistance;     // dist from camera center to cam-photo-plane in mm
	Coord cCameraPPoint;
	 //2d  transformations from camera-photo-plane to scan cq digtablet:
	enum TransfFiducials {CONFORM, AFFINE, BILINEAR, PROJECTIVE} transfFid;
	double rCoeffX[10];
  double rCoeffY[10];
  double rCoeffRow[10];
  double rCoeffCol[10];
	double rCoeffRefY[10];
	int iRes;
	//CVector v(8);				//  Vector with coefficients for inner orientation BILINEAR, PROJECTIVE
											// computed by inner orientation and used in outer orient and Coord2RowCol, RowCol2Coord transf
	CVector vecProjCenter;
	double rPCu, rPCv, rPCw;      // Proj center coords in terrain system
	double rKappaFinal;                          // camera kappa swing in degrees
  double rPhiFinal;                          // camera phi tilt in degrees
  double rOmegaFinal;                       // camera omega tilt in degrees
	
	virtual CVector cvGetProjectionCenter();
	virtual CVector cvGetCameraAngles();
};

#endif
