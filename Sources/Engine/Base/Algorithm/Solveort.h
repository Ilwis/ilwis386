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
/* solveort.h
 iFindOrtho (c) Ilwis System Development, ITC
 january 1997,  by Jan Hendrikse
     Last change:  WK   28 Aug 98    4:37 pm
*/
#include "Engine\Base\DataObjects\Dat2.h"
#include "Engine\Map\Raster\Map.h"

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

double IMPEXP rDotProd(Coord cA, Coord cB);

double IMPEXP rNormSquared(Coord cA);

double IMPEXP rCrossProd(RowCol rcA, RowCol rcB, RowCol rcC, RowCol rcD); // vector AB X vector DC

bool IMPEXP fPerpProjection(RowCol rcSide1, RowCol rcSide2, RowCol &rcFoot);

int IMPEXP iFindInnerRediduals( int iPoints, Coord cF[], Coord cS[], 
                         Coord cPhotoPrincPoint, Coord cScanPrincPoint,
                         Coord cD[]); 
                        

int IMPEXP iFindInnerOrientation(int iPoints, const Coord* cPhotoFid, const RowCol* rcScannedFid,
                           double& rPhoto2ScanScale,
                           RowCol &rcScanPrincPoint, Coord& cScanPrincPoint,
                           Coord cResiduals[]);

int IMPEXP iFindOuterOrientation(int iGroundCtrlPnts, double rScanPrincDistance, const Map& mDTM,
               const Coord* Depen, const Coord* Indep, const double* crdZ,
               Coord cScanPrincPoint,          // pos of PP in scan using double precision
               RealMatrix& rmRotx2u, RealMatrix& rmRotu2x,
               CVector& vecProjC, double& rAvgScale, double& rKa, double& rPh, double& rOm,
               int iMaxOrientCount, int& iIterations) ;
               
RealMatrix IMPEXP rmMakeKappaPhiOmegaMatrix(double k, double f, double w, const RealMatrix& rmUnit);








