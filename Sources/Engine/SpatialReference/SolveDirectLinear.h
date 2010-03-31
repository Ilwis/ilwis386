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
/* SolveDirectLinear.h
	Last change:  JH   24 Sep 99    4:49 pm
*/

#ifndef ILWSOLVEDIRLIN_H
#define ILWSOLVEDIRLIN_H

#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\SpatialReference\SolveCameraPosition.h"

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

class IMPEXP SolveDirectLinear: private SolveCameraPosition
{
protected:
  SolveDirectLinear();
  double rHeightAccuracy;
  int iMaxOrientCount;
  int iMaxHeightCount; 
  Map mapDTM;
	typedef double Coefs[11];
  RangeReal rrMMDTM;
  CoordBounds cbDTM;
  double rAvgScale;
  double rAvgX, rAvgY, rAvgZ, rAvgRow, rAvgCol;
  double rCoeff[11];

  int iFindCoeffsOfDirLin(int iPoints,
		 const Coord* Indep, const Coord* Depen, const double* rZ, Coefs &Coef);
  int iEstimateAverageScale(int iPoints,
                 const Coord* Indep, const Coord* Depen, double &rScale);
  Coord crdInverseOfDirectLinear(const double &rCol, const double &rRow, const double &rW);
  int iFindProjectionCenter();
  int iFindLightRayParameters(const double &rCol, const double &rRow);

  double rAlpha, rBeta, rGamma; // Euler angles of optical axis of camera
  double rPCu, rPCv, rPCw;      // Proj center coords in terrain system
  double rKu, rKv; // slopes of lightray du/dw and dv/dw respectively
  double rDu, rDv; // XY coords of piercingpoint lightray with groundplane W = 0
  Array<Coord>  cActiveCtrl; // array of active ctrlpnt coords
  Array<double> rVerticDist;// array with their distances from trend plane
  int iFindNonCoplanarity(int iPoints, const Coord* crdXY, const double* crdZ);  
  double rTrendPlaneCoeff[3];
 //double rZdiff[];	

  double rNonCoplanarity;// RMS, root of mean of squared distances 
                       // of ctrlpoints from their 'trend' surface
  double rZConfidRange; // estimated Z confidence of points outside trend surface    
  int iOutlier1, iOutlier2;	//  relative rec_nr of 2 outliers 
  double rOut1,rOut2; // Z-diff (vertical dist) of outliers from trend plane

	virtual CVector cvGetProjectionCenter();
	virtual CVector cvGetCameraAngles();
  double rGetNonCoplanarity();
	double rGetZConfidRange(); 
};

#endif // ILWSOLVEDIRLIN_H




