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
/* GeoRefEpipolar
   made for Stereoscopy
   Copyright Ilwis System Development ITC
   june 2001, by Jan Hendrikse, Wim Koolhoven, Bas Retsios
*/
#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\GrEpipolar.h"
#define EPS8 1.e-8
#define EPS4 1.e-4

// the following are auxilary functions from 2D vector geometry applied to Coords:

double GeoRefEpipolar::rDotProd(Coord cA, Coord cB)
{
  return cA.x * cB.x + cA.y * cB.y;
}

double GeoRefEpipolar::rNormSquared(Coord cA)
{
  return cA.x * cA.x  + cA.y * cA.y;
}

//double GeoRefEpipolar::rCrossProd(RowCol rcA, RowCol rcB, RowCol rcC, RowCol rcD); // vector AB X vector DC
double GeoRefEpipolar::rCrossProd(Coord cA, Coord cB, Coord cC, Coord cD) // (vector AB) X (vector DC)
{
  return ((cB.x -cA.x) * (cC.y - cD.y) -   
           (cB.y - cA.y) * (cC.x -cD.x));
}           

bool GeoRefEpipolar::fPerpProjection(Coord cSide1, Coord cSide2, Coord &cFoot) // project end point of side2 
{                                                  //at right angles onto line of side1 (2-dimensional)
    double rSide1Sq = rNormSquared(cSide1);
    
    if (rSide1Sq < EPS8) return false;
    cFoot.x = rDotProd(cSide1, cSide2) * cSide1.x / rNormSquared(cSide1);
    cFoot.y = rDotProd(cSide1, cSide2) * cSide1.y / rNormSquared(cSide1);
    return true;
}

GeoRefEpipolar* GeoRefEpipolar::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(7);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  double rPivoRow = as[0].rVal();
	double rPivoCol = as[1].rVal(); 
  double rAngl = as[2].rVal();
	double rScal= as[3].rVal();
	bool fLeft = as[6].fVal();
	return new GeoRefEpipolar(fn, rPivoRow, rPivoCol, rAngl, rScal, fLeft);
}

GeoRefEpipolar::GeoRefEpipolar(const bool fLeft_in)
: GeoRefScaleRotate(FileName())
, iLstCmptErr(iUNDEF)
{
	fLeftPhoto = fLeft_in;
	InitMembers();
}

GeoRefEpipolar::GeoRefEpipolar(const FileName& fn, 
										const double rPivotRow_in, const double rPivotCol_in,
										const double rAngle_in, const double rScale_in, 
										const bool fLeft_in)
: GeoRefScaleRotate(fn, CoordSystem(), RowCol())
, iLstCmptErr(iUNDEF)
{
		InitMembers(); // Just in case those members are used
//	rCol = a11 * c.x + a12 * c.y + b1;
//  rRow = a21 * c.x + a22 * c.y + b2;
		// Copy the input params to the members: needed for sName !!!
		rcPivot.Row = rPivotRow_in;
		rcPivot.Col = rPivotCol_in;
		rAngle = rAngle_in;
		rScale = rScale_in;
		fLeftPhoto = fLeft_in;
		/*
		a11 = cos(rAngle) * rScale_in;
		a22 = -a11;
		a21 = a12 = -sin(rAngle) * rScale_in;
		b1 = rPivotCol;// - a11 * x0 - a12 * y0;  
		b2 = rPivotRow;// - a21 * x0 - a22 * y0; */
		Updated();
}

void GeoRefEpipolar::SetMatrixGrSmplCoefficients(
										const double rPivotRow_in, const double rPivotCol_in,
										const double rAngle_in, const double rScale_in, 
										const bool fLeft_in)
{
//	rCol = a11 * c.x + a12 * c.y + b1;
//  rRow = a21 * c.x + a22 * c.y + b2;
		rcPivot.Row = rPivotRow_in;
		rcPivot.Col = rPivotCol_in;
		rAngle = rAngle_in;
		rScale = rScale_in;
		fLeftPhoto = fLeft_in;
		/*
		a11 = cos(rAngle) / rScale_in;
		a22 = -a11;
		a21 = a12 = -sin(rAngle) / rScale_in;
		b1 = rPivotCol;// - a11 * x0 - a12 * y0;  
		b2 = rPivotRow;// - a21 * x0 - a22 * y0; */
		Updated();
}

void GeoRefEpipolar::InitMembers()
{
	// Get to a consistent state (otherwise iNrFiduc = a random number
	// and the arrays are expected to contain data
	iNrFiduc = 0;
	iNrOffFlight = 0;
	int i;
	for (i=0; i<4; ++i)
	{
		fSelectedFiducial[i] = false;
		rScannedFidRow[i] = rUNDEF;
		rScannedFidCol[i] = rUNDEF;
	}
	for (i=0; i<2; ++i)
	{
		fSelectedScalingPoint[i] = false;
		rScannedScalingPntRow[i] = rUNDEF;
		rScannedScalingPntCol[i] = rUNDEF;
	}
	rScanPrincPointRow = rUNDEF;
	rScanPrincPointCol = rUNDEF;
	cScanPrincPoint = crdUNDEF;
	cScanTransfPrincPoint = crdUNDEF;
}

// Getter/Setter functions
void GeoRefEpipolar::SetRowCol(RowCol rcSize)
{
	GeoRefScaleRotate::SetRowCol(rcSize);
}

String GeoRefEpipolar::sType() const
{
  return "GeoReference Epipolar";
}

String GeoRefEpipolar::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("GeoRefEpipolar(%li,%li,%g,%g,%i)", rcPivot.Row, rcPivot.Col, 
																rAngle, rScale, (int)fLeftPhoto);
}

String GeoRefEpipolar::sTypeName() const
{
  return sType();
}

int GeoRefEpipolar::iFindPrincPointLocation() /*int iPoints,
															const double* rScannedFidRow, const double* rScannedFidCol,
															double& rScanPrincPointRow, double& rScanPrincPointCol,
															Coord& cScanPrincPoint) */
{
	Coord cS[4];    // array  of Fiducial marks in scan made from photo (digital image)
	iNrFiduc = 0;
  for (int i = 0; i < 4; ++i) 
	{
		if (fSelectedFiducial[i] && rScannedFidRow[i] != rUNDEF && rScannedFidCol[i] != rUNDEF)
		{
			cS[iNrFiduc].x = rScannedFidRow[i];     // collecting fiducial mark positions in scan converting to coords
			cS[iNrFiduc].y = rScannedFidCol[i];     // to compute the position of PrincPoint in double precision
			++iNrFiduc;
		}
  }                                          // x = Row, Y = Col keeping the system right-handed
  if (iNrFiduc < 2)												// at least 2 fiducial marks needed to calc from fiducials
	{
		if (cScanPrincPoint.fUndef())
			return -12;
		else
			return 0;
	}

  //int iRet;                                // return value after residuals calculated (0 if succes)
  if (iNrFiduc == 2) {
    cScanPrincPoint.x = (cS[0].x + cS[1].x)/2;
    cScanPrincPoint.y = (cS[0].y + cS[1].y)/2; 
    rScanPrincPointRow = cScanPrincPoint.x;
    rScanPrincPointCol = cScanPrincPoint.y; 
		
    double d =  rDist2(cS[0], cS[1]) ;
    if (d < EPS4) return -13;  // fiducials coincide in photo
    return 0;
  }    
  if (iNrFiduc == 3) {              // Use Triangle of Fiducial Marks
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
    rScanPrincPointRow = cScanPrincPoint.x;
    rScanPrincPointCol = cScanPrincPoint.y; 
    return 0;
  }  
	if (iNrFiduc == 4) {
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
    rScanPrincPointRow = cScanPrincPoint.x;
    rScanPrincPointCol = cScanPrincPoint.y; 
		double d =  rDist2(cS[iAdj2], cS[iAdj1]);
    if (d < EPS4) return -13;  // some fiducials less than .1 mm apart
    return 0;
  }  
  else return -15;   // error code for wrong number of fiducial marks
}

int GeoRefEpipolar::iLocateTransferredPrincPoint()//Coord& cScanTransferredPrincPoint)
{
	if (cScanTransfPrincPoint.fUndef()) // check input map bounds must be added
		return -17;
	return 0; // should contain check of valid placement of transferred PP
}

int GeoRefEpipolar::iFindRotationAngle() /*const Coord cPP, const Coord cTransfPP, double& rAngl) */
{
	Coord cPP = cScanPrincPoint;
	Coord cTransfPP = cScanTransfPrincPoint;
	double oppos, adjac;

	if (fLeftPhoto)
	{
		oppos = cPP.x - cTransfPP.x;
		adjac = cTransfPP.y - cPP.y;
	}
	else
	{
		oppos = cTransfPP.x - cPP.x;
		adjac = cPP.y - cTransfPP.y;
	}
		//rScanTransfPrincPointCol - rScanPrincPointCol;// is < 0 in righthand photo
	// atan2 is used to produce always an angle between -90 and 90:
	// int iLeft = fLeftPhoto ? 1 : -1;
//	int iLeft = (int)(fLeftPhoto)*2 - 1; // = 1 for left and -1 for right photo
	// if (iLeft * adjac < 0) return -17; // transferred PrincP at wrong side of own PrincP
	///if (abs(adjac) < 10) return -16; // photobase too short
	// if (adjac < 0) return -17; // transferred PrincP at wrong side of own PrincP
	rAngle = atan2(oppos, adjac); // > 0 if photobase (flightline) goes up from Left to Right
	return 0;
}

int GeoRefEpipolar::iFindScalingFactor()/*int iPoints, 
																				const double* rOffFlightRow, 
																				const double* rOffFlightCol,
																			 const Coord cPP, const Coord cTransPP, 
																			 double& rScal) */
{
	Coord cPP = cScanPrincPoint;
	Coord cTransPP = cScanTransfPrincPoint;
	Coord cFirst, cSecond;

	iNrOffFlight = 0;
  for (int i = 0; i < 2; ++i) 
	{
		if (fSelectedScalingPoint[i] && rScannedScalingPntRow[i] != rUNDEF &&
			rScannedScalingPntCol[i] != rUNDEF)
		{
			// I have a point .. where to put it? To First or to Second? Solution:
			++iNrOffFlight;
			if (iNrOffFlight == 1)
			{
				cFirst.x = rScannedScalingPntRow[i];
				cFirst.y = rScannedScalingPntCol[i];
			}
			else
			{
				cSecond.x = rScannedScalingPntRow[i];
				cSecond.y = rScannedScalingPntCol[i];
			}
		}
  }

	if (iNrOffFlight > 0)
	{
		Coord cPhotoBaseVector;
		cPhotoBaseVector.x = cTransPP.x - cPP.x; 
		cPhotoBaseVector.y = cTransPP.y - cPP.y;
		double rPhotoBaseLength = sqrt(rNormSquared(cPhotoBaseVector));
		if (rPhotoBaseLength < EPS4) 
			return - 17; 
		double rFirstDist, rSecondDist = 0; 
		if (iNrOffFlight == 1)
		{
			rFirstDist = rCrossProd(cPP, cTransPP, cFirst, cPP) / rPhotoBaseLength;
			// (vector cPP_cTraPP) X (vector cPP_cFirst)
			if (abs(rFirstDist) < rPhotoBaseLength / 2)
			{
				// The point has a problem, so report to the user
				// The following statement retraces which number the point had (1 or 2,
				// in order to return -18 or -19
				if (fSelectedScalingPoint[0] && rScannedScalingPntRow[0] != rUNDEF &&
					rScannedScalingPntCol[0] != rUNDEF)
					return -18; // point was nr. 1
				else
					return -19; // point was nr. 2
			}
		}
		else if (iNrOffFlight == 2)
		{
			rFirstDist = rCrossProd(cPP, cTransPP, cFirst, cPP) / rPhotoBaseLength;
			rSecondDist = rCrossProd(cPP, cTransPP, cSecond, cPP) / rPhotoBaseLength; 
			//  (vec cPP_cTraPP) X (vec cPP_cSecond)
			if (abs(rSecondDist) < rPhotoBaseLength / 2)
				return -19;
			if (abs(rFirstDist) < rPhotoBaseLength / 2)
				return -18;
		}
		rScale = abs(rFirstDist) + abs(rSecondDist);
	}
	else
		rScale = 1; // No active / selected scaling points
	return 0;
}

int GeoRefEpipolar::Compute()
{
  int iRes = 0;  // error code returned to & by Compute()
  //    -11    no valid fiducial marks found
  //    -12    less than 2 fiducial marks (not enough)
  //    -13    incorrect positions of Fid marks  (e.g. some almost coincide)
  //    -14    incorrect ordering of fiducial marks
  //    -15    wrong nbr of fid marks(not 2,3 or 4) 
  //    -16    transferred princ point too close to princ point
	//    -17    transferred princ point at wrong side
	//		-18			upward point not far enough off flight-line 
	//		-19			downward point not far enough off flight-line 
	
	iRes = iFindPrincPointLocation();/*iNrFiduc, rScannedFidRow, rScannedFidCol,
															rScanPrincPointRow, rScanPrincPointCol,
															 cScanPrincPoint); */
	if (iRes == 0)
		iRes = iLocateTransferredPrincPoint();//crdTransfPP);
	if (iRes == 0)
		iRes = iFindRotationAngle();//crdPP, crdTransfPP, rAngle);
	if (iRes == 0)
		iRes = iFindScalingFactor();/*iNrFiduc, rScannedFidRow, 
																	rScannedFidCol, crdPP, crdTransfPP, rScale);*/
  iLstCmptErr = iRes;

	return iRes;
}

RowCol GeoRefEpipolar::rGetPivotRowCol()
{
	return rcPivot;
}

double GeoRefEpipolar::rGetRotAngle()
{
	return rAngle;
}

double GeoRefEpipolar::rGetScaleFactor()
{
	return rScale;
}

void GeoRefEpipolar:: SetPivotRowCol(const RowCol rcPiv)
{
	rcPivot = rcPiv;
}

void GeoRefEpipolar::SetRotAngle(const double rAng)
{
	rAngle = rAng;
}

void GeoRefEpipolar::SetScaleFactor(const double rSc)
{
	rScale = rSc;
}

int GeoRefEpipolar::iLastComputeError()
{
	return iLstCmptErr; // no compute yet => iUNDEF (might be useful to know)
}
