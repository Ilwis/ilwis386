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
// StereoPairEpiPolar.cpp: implementation of the StereoPairEpiPolar class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine\Stereoscopy\StereoPairEpiPolar.h"
#include "Engine\SpatialReference\GrEpipolar.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Headers\Hs\map.hs"
#include "Headers\Hs\stereoscopy.hs"
#include "Headers\Hs\Georef.hs"
#include "Headers\Hs\Digitizr.hs"
#define EPS4 1.e-4

const char* StereoPairEpiPolar::sSyntax() {
  return  "StereoPairEpiPolar(mapL,pivotLeftRow,pivotLeftCol,RotAngleLeft,ScaleFacLeft,\n"
														"mapR,pivotRightRow,pivotRightCol,RotAngleRight,ScaleFacRight)";
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
StereoPairEpiPolar::StereoPairEpiPolar()
{
} */

StereoPairEpiPolar::StereoPairEpiPolar(const FileName& fn, StereoPairPtr& p)
: StereoPairVirtual(fn, p)
{
	//String sGeoRefEpiLeft;
	//ReadElement("StereoPairEpiPolar", "GeoRefEpiPolarLeft", sGeoRefEpiLeft);
 	GeoRefEpipolar *grEpiLeft = new GeoRefEpipolar(true); // delete is delegated to GeoRef
	GeoRefEpipolar *grEpiRight = new GeoRefEpipolar(false); // by SetPointer
  grNewLeft = GeoRef(RowCol(1, 1));  // make GeoRefNone
  grNewRight = GeoRef(RowCol(1, 1)); // make GeoRefNone
	grNewLeft->DoNotStore(true);
	grNewRight->DoNotStore(true);
  grNewLeft.SetPointer(grEpiLeft);
  grNewRight.SetPointer(grEpiRight);

	ReadElement("StereoPairEpiPolar", "LeftPhotoIn", sLeftInputPhoto);
	ReadElement("StereoPairEpiPolar", "RightPhotoIn", sRightInputPhoto);
	bool fActive;
	ReadElement("GeoRefEpiPolarLeft", "Nr of Active Fiducial Marks", grEpiLeft->iNrFiduc);
	for (int i = 0; i < 4; ++i)
  {
    String sEntry("Active Fiducial Mark %i ", i);
		ReadElement("GeoRefEpiPolarLeft", sEntry.scVal(), fActive);
		grEpiLeft->fSelectedFiducial[i] = fActive;
	}
	Coord crd;
	for (int i = 0; i < 4; ++i)
  {
    String sEntry("Scanned Fiducial Mark %i", i);
    ReadElement("GeoRefEpiPolarLeft", sEntry.scVal(), crd); 
		grEpiLeft->rScannedFidRow[i] = crd.x;
		grEpiLeft->rScannedFidCol[i] = crd.y;
	}

	ReadElement("GeoRefEpiPolarLeft", "Principal Point in Scan X-Coord", grEpiLeft->cScanPrincPoint.x);
  ReadElement("GeoRefEpiPolarLeft", "Principal Point in Scan Y-Coord", grEpiLeft->cScanPrincPoint.y);
  ReadElement("GeoRefEpiPolarLeft", "Principal Point in Scan Col", grEpiLeft->rScanPrincPointCol);
  ReadElement("GeoRefEpiPolarLeft", "Principal Point in Scan Row", grEpiLeft->rScanPrincPointRow);

	ReadElement("GeoRefEpiPolarLeft", "Transferred PrincPoint", grEpiLeft->cScanTransfPrincPoint);
	ReadElement("GeoRefEpiPolarLeft", "Nr of Active Scaling Points", grEpiLeft->iNrOffFlight);
	for (int i = 0; i < 2; ++i)
  {
		String sEntry("Active Scaling Point %i ", i);
		ReadElement("GeoRefEpiPolarLeft", sEntry.scVal(), fActive);
		grEpiLeft->fSelectedScalingPoint[i] = fActive;
	}
	for (int i = 0; i < 2; ++i)
  {
    String sEntry("Scanned Scaling Point %i", i);
    ReadElement("GeoRefEpiPolarLeft", sEntry.scVal(), crd); 
		grEpiLeft->rScannedScalingPntRow[i] = crd.x;
		grEpiLeft->rScannedScalingPntCol[i] = crd.y;
	}

	ReadElement("GeoRefEpiPolarRight", "Nr of Active Fiducial Marks", grEpiRight->iNrFiduc);
	for (int i = 0; i < 4; ++i)
  {
    String sEntry("Active Fiducial Mark %i ", i);
		ReadElement("GeoRefEpiPolarRight", sEntry.scVal(), fActive);
		grEpiRight->fSelectedFiducial[i] = fActive;
	}
	for ( int i = 0; i < 4; ++i)
  {
    String sEntry("Scanned Fiducial Mark %i", i);
    ReadElement("GeoRefEpiPolarRight", sEntry.scVal(), crd); 
		grEpiRight->rScannedFidRow[i] = crd.x;
		grEpiRight->rScannedFidCol[i] = crd.y;
	}

	ReadElement("GeoRefEpiPolarRight", "Principal Point in Scan X-Coord", grEpiRight->cScanPrincPoint.x);
  ReadElement("GeoRefEpiPolarRight", "Principal Point in Scan Y-Coord", grEpiRight->cScanPrincPoint.y);
  ReadElement("GeoRefEpiPolarRight", "Principal Point in Scan Col", grEpiRight->rScanPrincPointCol);
  ReadElement("GeoRefEpiPolarRight", "Principal Point in Scan Row", grEpiRight->rScanPrincPointRow);

	ReadElement("GeoRefEpiPolarRight", "Transferred PrincPoint", grEpiRight->cScanTransfPrincPoint);
	ReadElement("GeoRefEpiPolarRight", "Nr of Active Scaling Points", grEpiRight->iNrOffFlight);
	for (int i=0; i < 2; ++i)
  {
		String sEntry("Active Scaling Point %i ", i);
		ReadElement("GeoRefEpiPolarRight", sEntry.scVal(), fActive);
		grEpiRight->fSelectedScalingPoint[i] = fActive;
	}
	for (int i=0; i < 2; ++i)
  {
    String sEntry("Scanned Scaling Point %i", i);
    ReadElement("GeoRefEpiPolarRight", sEntry.scVal(), crd); 
		grEpiRight->rScannedScalingPntRow[i] = crd.x;
		grEpiRight->rScannedScalingPntCol[i] = crd.y;
	}

	FileName fnLeftInputPhoto (sLeftInputPhoto, fn);
	FileName fnRightInputPhoto (sRightInputPhoto, fn);
	Map mpL (fnLeftInputPhoto);
	Map mpR (fnRightInputPhoto);
	objdep.Add(mpR.ptr());
  objdep.Add(mpL.ptr());
}

StereoPairEpiPolar::StereoPairEpiPolar(const FileName& fn, StereoPairPtr& p, 
																	     const Map& mpL, //const GeoRefEpipolar& grL,
																			 const Map& mpR	 //const GeoRefEpipolar& grR
																			 )
: StereoPairVirtual(fn, p, mpL, mpR)
{
	sLeftInputPhoto = mpL->sName(true); // true means include extension
	sRightInputPhoto = mpR->sName(true);
	GeoRefEpipolar *grEpiLeft = new GeoRefEpipolar(true); // delete delegated to GeoRef
	GeoRefEpipolar *grEpiRight = new GeoRefEpipolar(false); // by SetPointer

  grNewLeft = GeoRef(RowCol(1, 1));  // make GeoRefNone
  grNewRight = GeoRef(RowCol(1, 1)); // make GeoRefNone
	grNewLeft->DoNotStore(true);
	grNewRight->DoNotStore(true);
  grNewLeft.SetPointer(grEpiLeft);
  grNewRight.SetPointer(grEpiRight);
	
	//ptr.fnObj = FileName(fn);
	FileName fnLeftOut = FileName(fn);
	fnLeftOut.sFile &= "_Left";
	fnLeftOut.sExt = ".mpr";
	if (fnLeftOut.fExist())
        FileAlreadyExistError(fnLeftOut);
	FileName fnRightOut = FileName(fn);
	fnRightOut.sFile &= "_Right";
	fnRightOut.sExt = ".mpr";
	if (fnRightOut.fExist())
        FileAlreadyExistError(fnRightOut);
	objdep.Add(mpR.ptr());
  objdep.Add(mpL.ptr());

} 

	
StereoPairEpiPolar::~StereoPairEpiPolar()
{

//	if (grEpiLeft)
//		delete grEpiLeft;
//	if (grEpiRight)
//		delete grEpiRight;

}

void StereoPairEpiPolar::Store()
{
	GeoRefEpipolar * grEpiLeft = grNewLeft->pgEpipolar();
	GeoRefEpipolar * grEpiRight = grNewRight->pgEpipolar();

  StereoPairVirtual::Store();
  WriteElement("StereoPairVirtual", "Type", "StereoPairEpiPolar");
	WriteElement("StereoPairEpiPolar", "LeftPhotoIn", sLeftInputPhoto);
	WriteElement("StereoPairEpiPolar", "RightPhotoIn", sRightInputPhoto);
	//String sGeoRefEpiLeft;
  //WriteElement("StereoPairEpiPolar", "GeoRefEpiPolarLeft", sGeoRefEpiLeft);
	WriteElement("GeoRefEpiPolarLeft", "Nr of Active Fiducial Marks", grEpiLeft->iNrFiduc);
	WriteElement("GeoRefEpiPolarLeft", "Nr of Active Scaling Points", grEpiLeft->iNrOffFlight);
	int i;
	bool fActive;
  for (i = 0; i < 4; ++i)
  {
    String sEntry("Active Fiducial Mark %i ", i);
    if (grEpiLeft->fSelectedFiducial[i]) 
			fActive = true;
		else
			fActive = false;
		WriteElement("GeoRefEpiPolarLeft", sEntry.scVal(), fActive);
  }  
  for (i = 0; i < 4; ++i)
  {
    String sEntry("Scanned Fiducial Mark %i", i);
		Coord crdScannedFid (grEpiLeft->rScannedFidRow[i], grEpiLeft->rScannedFidCol[i]);
    WriteElement("GeoRefEpiPolarLeft", sEntry.scVal(), crdScannedFid);
  }
	WriteElement("GeoRefEpiPolarLeft", "Principal Point in Scan X-Coord", grEpiLeft->cScanPrincPoint.x);
  WriteElement("GeoRefEpiPolarLeft", "Principal Point in Scan Y-Coord", grEpiLeft->cScanPrincPoint.y);
  WriteElement("GeoRefEpiPolarLeft", "Principal Point in Scan Col", grEpiLeft->rScanPrincPointCol);
  WriteElement("GeoRefEpiPolarLeft", "Principal Point in Scan Row", grEpiLeft->rScanPrincPointRow); 
	WriteElement("GeoRefEpiPolarLeft", "Transferred PrincPoint", grEpiLeft->cScanTransfPrincPoint);
	for (i = 0; i < 2; ++i)
  {
    String sEntry("Active Scaling Point %i ", i);
    if (grEpiLeft->fSelectedScalingPoint[i]) 
			fActive = true;
		else
			fActive = false;
		WriteElement("GeoRefEpiPolarLeft", sEntry.scVal(), fActive);
  }  
	for (i = 0; i < 2; ++i)
  {
    String sEntry("Scanned Scaling Point %i", i);
		Coord crdScannedScaleingPnt (grEpiLeft->rScannedScalingPntRow[i], grEpiLeft->rScannedScalingPntCol[i]);
    WriteElement("GeoRefEpiPolarLeft", sEntry.scVal(), crdScannedScaleingPnt);
  }
	
	//String sGeoRefEpiRight;
  //WriteElement("StereoPairEpiPolar", "GeoRefEpiPolarRight", sGeoRefEpiLeft);
	WriteElement("GeoRefEpiPolarRight", "Nr of Active Fiducial Marks", grEpiRight->iNrFiduc);
	WriteElement("GeoRefEpiPolarRight", "Nr of Active Scaling Points", grEpiRight->iNrOffFlight);
	fActive;
  for (i = 0; i < 4; ++i)
  {
    String sEntry("Active Fiducial Mark %i ", i);
    if (grEpiRight->fSelectedFiducial[i]) 
			fActive = true;
		else
			fActive = false;
		WriteElement("GeoRefEpiPolarRight", sEntry.scVal(), fActive);
  }  
  for (i = 0; i < 4; ++i)
  {
    String sEntry("Scanned Fiducial Mark %i", i);
		Coord crdScannedFid (grEpiRight->rScannedFidRow[i], grEpiRight->rScannedFidCol[i]);
    WriteElement("GeoRefEpiPolarRight", sEntry.scVal(), crdScannedFid);
  }
	WriteElement("GeoRefEpiPolarRight", "Principal Point in Scan X-Coord", grEpiRight->cScanPrincPoint.x);
  WriteElement("GeoRefEpiPolarRight", "Principal Point in Scan Y-Coord", grEpiRight->cScanPrincPoint.y);
  WriteElement("GeoRefEpiPolarRight", "Principal Point in Scan Col", grEpiRight->rScanPrincPointCol);
  WriteElement("GeoRefEpiPolarRight", "Principal Point in Scan Row", grEpiRight->rScanPrincPointRow); 
	WriteElement("GeoRefEpiPolarRight", "Transferred PrincPoint", grEpiRight->cScanTransfPrincPoint);
	for (i = 0; i < 2; ++i)
  {
    String sEntry("Active Scaling Point %i ", i);
    if (grEpiRight->fSelectedScalingPoint[i]) 
			fActive = true;
		else
			fActive = false;
		WriteElement("GeoRefEpiPolarRight", sEntry.scVal(), fActive);
  }  
	for (i = 0; i < 2; ++i)
  {
    String sEntry("Scanned Scaling Point %i", i);
		Coord crdScannedScaleingPnt (grEpiRight->rScannedScalingPntRow[i], grEpiRight->rScannedScalingPntCol[i]);
    WriteElement("GeoRefEpiPolarRight", sEntry.scVal(), crdScannedScaleingPnt);
  }
	/*RowCol rcLeftPiv = grEpiLeft->rGetPivotRowCol();
	Coord rcPivotLeft = Coord(rcLeftPiv.Row, rcLeftPiv.Col);
	double rRotAngleLeft = grEpiLeft->rGetRotAngle(); 
	double rScaleFactorLeft = grEpiLeft->rGetScaleFactor();
  WriteElement("GeoRefEpiPolarLeft", "Pivot", rcPivotLeft);
	WriteElement("GeoRefEpiPolarLeft", "Rotation Angle", rRotAngleLeft);
	WriteElement("GeoRefEpiPolarLeft", "ScaleFactorLeft", rScaleFactorLeft);
	String sGeoRefEpiRight;
  WriteElement("StereoPairEpiPolar", "GeoRefEpiPolarRight", sGeoRefEpiRight);
	RowCol rcRightPiv = grEpiRight->rGetPivotRowCol();
	Coord crdPivotRight = Coord(rcRightPiv.Row, rcRightPiv.Col);
	double rRotAngleRight = grEpiRight->rGetRotAngle(); 
	double rScaleFactorRight = grEpiRight->rGetScaleFactor();
	WriteElement("GeoRefEpiPolarRight", "Pivot", rcRightPiv);
	WriteElement("GeoRefEpiPolarRight", "Rotation Angle", rRotAngleRight);
	WriteElement("GeoRefEpiPolarRight", "ScaleFactor", rScaleFactorRight);*/
}

bool StereoPairEpiPolar::fFreezing()
{
	// GET THE GEOREFEPIPOL PARAMS:
	Coord crdPivotLeft = Coord(0,0);
	Coord crdPivotRight = Coord(0,0);
	double rRotAngleLeft = 0;
	double rRotAngleRight = 0;
	double rScaleFactorLeft = 1;
	double rScaleFactorRight = 1;

	GeoRefEpipolar * grEpiLeft = grNewLeft->pgEpipolar();
	GeoRefEpipolar * grEpiRight = grNewRight->pgEpipolar();

	grEpiLeft->Compute();
	grEpiRight->Compute();
	// First check if we may continue calculating and resampling
	String sErr = sErrGeorefs();
	if (sErr.length() > 0)
	{
		MessageBox(0,sErr.scVal(),0,MB_OK|MB_ICONSTOP);
		return false;
	}
	// if we arrived here, it means it is okay

	Map mpLeftInputPhoto (sLeftInputPhoto);
	Map mpRightInputPhoto (sRightInputPhoto);

	RowCol rcLeftPhotoSize = mpLeftInputPhoto->rcSize();
	RowCol rcRightPhotoSize = mpRightInputPhoto->rcSize();
	rScaleFactorLeft = max(1,grEpiRight->rScale/grEpiLeft->rScale);
	rScaleFactorRight = max(1,grEpiLeft->rScale/grEpiRight->rScale);
	// Here 2 grScaleRotate's must be constructed: 1 for Left een for Right
	// follwong statements will be replaced by 

		// for the time being I use assigments to local variables:
	////grEpiLeft:
	long iLeftPivotRow	= (long)grEpiLeft->rScanPrincPointRow;
	long iLeftPivotCol	= (long)grEpiLeft->rScanPrincPointCol;
	double rLeftAngle	=  (long)grEpiLeft->rGetRotAngle(); 
	double rLeftScale =	rScaleFactorLeft; 
	bool fResamplingLeftNeeded = (abs(rLeftAngle) > EPS4) && (rLeftScale > EPS4);
						
	////grEpiRight:
	long iRightPivotRow	= (long)grEpiRight->cScanTransfPrincPoint.x;
	long iRightPivotCol	= (long)grEpiRight->cScanTransfPrincPoint.y;
	double rRightAngle	=	grEpiRight->rGetRotAngle();
	double rRightScale =	rScaleFactorRight; 
	bool fResamplingRightNeeded = (abs(rRightAngle) > EPS4) && (rRightScale > EPS4);	

	grEpiLeft->SetMatrixGrSmplCoefficients(
										grEpiLeft->rScanPrincPointRow, grEpiLeft->rScanPrincPointCol,
										grEpiLeft->rGetRotAngle(), rScaleFactorLeft, 
										true);
	grEpiRight->SetMatrixGrSmplCoefficients(
										grEpiRight->cScanTransfPrincPoint.x, grEpiRight->cScanTransfPrincPoint.y,
										grEpiRight->rGetRotAngle(), rScaleFactorRight, 
										false);
// // Epipolar photo name is made using the StereoPair name:
	FileName fnEpiPhotoLeft (ptr.sName());
  fnEpiPhotoLeft.sFile &= "_Left";
  fnEpiPhotoLeft.sExt = ".mpr";

	FileName fnEpiPhotoRight (ptr.sName());
  fnEpiPhotoRight.sFile &= "_Right";
  fnEpiPhotoRight.sExt = ".mpr";
	
// New target grf names are made using the StereoPair name:
	FileName fnGrfEpiLeft(ptr.sName());
	fnGrfEpiLeft.sFile &= "_Left";
	fnGrfEpiLeft.sExt = ".grf";
	FileName fnGrfEpiRight(ptr.sName());
	fnGrfEpiRight.sFile &= "_Right";
	fnGrfEpiRight.sExt = ".grf";

	GeoRef grLeftIn, grRightIn;
	grLeftIn = GeoRef(mpLeftInputPhoto->gr());
	grRightIn = GeoRef(mpRightInputPhoto->gr());

	double rBaseCols, rBaseRows, rBaselength;
	if (fResamplingLeftNeeded) {
		rBaseCols = grEpiLeft->cScanTransfPrincPoint.y - grEpiLeft->rScanPrincPointCol;
		rBaseRows = grEpiLeft->cScanTransfPrincPoint.x - grEpiLeft->rScanPrincPointRow;
		rBaselength = sqrt(rBaseCols*rBaseCols + rBaseRows*rBaseRows) * rScaleFactorLeft;
	}
	else // either (fResamplingRightNeeded) or no rescaling at all
	{
		rBaseCols = grEpiLeft->rScanPrincPointCol - grEpiRight->cScanTransfPrincPoint.y;
		rBaseRows = grEpiLeft->rScanPrincPointRow - grEpiRight->cScanTransfPrincPoint.x;
		rBaselength = sqrt(rBaseCols*rBaseCols + rBaseRows*rBaseRows) * rScaleFactorRight;
	}
	double rNewPivLeftCol, rNewPivLeftRow, rNewPivRightCol, rNewPivRightRow;
	double rNewSizeLeftRow, rNewSizeLeftCol, rNewSizeRightRow, rNewSizeRightCol;
	double rNewBeyLeftRow, rNewBeyLeftCol, rNewBeyRightRow, rNewBeyRightCol;//row and cols lying beyond pivot
	// find now bounding rectangle of the rotated left-hand image:
	if (rLeftAngle > 0) {
		rNewPivLeftRow = (iLeftPivotCol * sin(rLeftAngle) + 
														iLeftPivotRow * cos(rLeftAngle)) * rScaleFactorLeft;
		rNewPivLeftCol = (iLeftPivotCol * cos(rLeftAngle) + 
											(rcLeftPhotoSize.Row - iLeftPivotRow) * sin(rLeftAngle)) * rScaleFactorLeft;
		rNewBeyLeftRow = ((rcLeftPhotoSize.Row - iLeftPivotRow) * cos(rLeftAngle) 
												+ (rcLeftPhotoSize.Col - iLeftPivotCol) * sin(rLeftAngle)) * rScaleFactorLeft;
		rNewSizeLeftRow = rNewPivLeftRow + rNewBeyLeftRow;
		rNewBeyLeftCol = (iLeftPivotRow * sin(rLeftAngle) + 
												+ (rcLeftPhotoSize.Col - iLeftPivotCol) * cos(rLeftAngle)) * rScaleFactorLeft;
		rNewSizeLeftCol = rNewPivLeftCol + rNewBeyLeftCol;
	}
	else {
		rNewPivLeftRow = ((rcLeftPhotoSize.Col - iLeftPivotCol) * abs(sin(rLeftAngle)) + 
														iLeftPivotRow * cos(rLeftAngle)) * rScaleFactorLeft;
		rNewPivLeftCol = (iLeftPivotRow * abs(sin(rLeftAngle))  
												+ iLeftPivotCol * cos(rLeftAngle)) * rScaleFactorLeft;
		rNewBeyLeftRow = ((rcLeftPhotoSize.Row - iLeftPivotRow) * cos(rLeftAngle) 
												+ iLeftPivotCol * abs(sin(rLeftAngle))) * rScaleFactorLeft;
		rNewSizeLeftRow = rNewPivLeftRow + rNewBeyLeftRow;
		rNewBeyLeftCol = ((rcLeftPhotoSize.Row - iLeftPivotRow) * abs(sin(rLeftAngle)) + 
												+ (rcLeftPhotoSize.Col - iLeftPivotCol) * cos(rLeftAngle)) * rScaleFactorLeft;
		rNewSizeLeftCol = rNewPivLeftCol + rNewBeyLeftCol;
	}
	/// bounding rectangle of right-hand image:
	if (rRightAngle > 0) {;
		rNewPivRightRow = (iRightPivotCol * sin(rRightAngle) + 
														iRightPivotRow * cos(rRightAngle)) * rScaleFactorRight;
		rNewPivRightCol = (iRightPivotCol * cos(rRightAngle) + 
											(rcRightPhotoSize.Row  - iRightPivotRow) * sin(rRightAngle)) * rScaleFactorRight;
		rNewBeyRightRow = ((rcRightPhotoSize.Row - iRightPivotRow) * cos(rRightAngle) 
												+ (rcRightPhotoSize.Col - iRightPivotCol) * sin(rRightAngle)) * rScaleFactorRight;
		rNewSizeRightRow = rNewPivRightRow + rNewBeyRightRow;
		rNewBeyRightCol = (iRightPivotRow * sin(rRightAngle) + 
												+ (rcRightPhotoSize.Col - iRightPivotCol) * cos(rRightAngle)) * rScaleFactorRight;
		rNewSizeRightCol = rNewPivRightCol + rNewBeyRightCol;
	}
	else {
		rNewPivRightRow = ((rcRightPhotoSize.Col - iRightPivotCol) * abs(sin(rRightAngle)) + 
													iRightPivotRow * cos(rRightAngle)) * rScaleFactorRight;
		rNewPivRightCol = (iRightPivotCol * cos(rRightAngle) + 
													iRightPivotRow * abs(sin(rRightAngle))) * rScaleFactorRight;
		rNewBeyRightRow =  ((rcRightPhotoSize.Row - iRightPivotRow) * cos(rRightAngle) 
												+ iRightPivotCol * abs(sin(rRightAngle))) * rScaleFactorRight;
		rNewSizeRightRow = rNewPivRightRow + rNewBeyRightRow;
		rNewBeyRightCol =  ((rcLeftPhotoSize.Row - iRightPivotRow) * abs(sin(rRightAngle)) + 
												+ (rcRightPhotoSize.Col - iRightPivotCol) * cos(rRightAngle)) * rScaleFactorRight;
		rNewSizeRightCol = rNewPivRightCol + rNewBeyRightCol;
	}
	double rNewPivRowCommon = max(rNewPivLeftRow, rNewPivRightRow);
	double rNewPivColCommon = max(rNewPivLeftCol, rNewPivRightCol);
	double rBottomMargin = max(rNewBeyLeftRow, rNewBeyRightRow);
	double rRightHandMargin = max(rNewBeyLeftCol, rNewBeyRightCol);
	double rNewCommonHeight = rNewPivRowCommon + rBottomMargin;
	double rNewCommonWidth = rNewPivColCommon + rRightHandMargin;

	//ALTERNATIVE WAY TO FIND BOUNDING RECT AND NEW PIVOT POSITIONS (KARL G)
	// Rotation/Scaling of input photo corners: (from upperleft clockwise)
		// left-photo, rotating around its PP, corners 1,2,3,4):
	RowCol rcBoundLeft, rcBoundRight, rcBoundBoth, rc;
	double row1 = ((1 - iLeftPivotRow) * cos(rLeftAngle)) +
															((1 - iLeftPivotCol) * sin(rLeftAngle));
	double col1 = ((1 - iLeftPivotCol) * cos(rLeftAngle)) -
															((1 - iLeftPivotRow) * sin(rLeftAngle));
	double row2 = ((1 - iLeftPivotRow) * cos(rLeftAngle)) +
															((rcLeftPhotoSize.Col - iLeftPivotCol) * sin(rLeftAngle));
	double col2 = ((rcLeftPhotoSize.Col - iLeftPivotCol) * cos(rLeftAngle)) -
															((1 - iLeftPivotRow) * sin(rLeftAngle));
	double row3 = ((rcLeftPhotoSize.Row - iLeftPivotRow) * cos(rLeftAngle)) +
															((rcLeftPhotoSize.Col - iLeftPivotCol) * sin(rLeftAngle));
	double col3 = ((rcLeftPhotoSize.Col - iLeftPivotCol) * cos(rLeftAngle)) -
															((rcLeftPhotoSize.Row - iLeftPivotRow) * sin(rLeftAngle));
	double row4 = ((rcLeftPhotoSize.Row - iLeftPivotRow) * cos(rLeftAngle)) +
															((1 - iLeftPivotCol) * sin(rLeftAngle));
	double col4 = ((1 - iLeftPivotCol) * cos(rLeftAngle)) -
															((rcLeftPhotoSize.Row - iLeftPivotRow) * sin(rLeftAngle));
	double rMinimRowLeft = min(min(row1,row2),min(row3,row4)) * rLeftScale;
	double rMinimColLeft = min(min(col1,col2),min(col3,col4)) * rLeftScale;
	double rMaximRowLeft = max(max(row1,row2),max(row3,row4)) * rLeftScale;
	double rMaximColLeft = max(max(col1,col2),max(col3,col4)) * rLeftScale;
		// right=hand-photo, rotating around its PP, corners 5.6.7.8): 
	double row5 = ((1 - iRightPivotRow) * cos(rRightAngle)) +
															((1 - iRightPivotCol) * sin(rRightAngle));
	double col5 = ((1 - iRightPivotCol) * cos(rRightAngle)) -
															((1 - iRightPivotRow) * sin(rRightAngle));
	double row6 = ((1 - iRightPivotRow) * cos(rRightAngle)) +
															((rcRightPhotoSize.Col - iRightPivotCol) * sin(rRightAngle));
	double col6 = ((rcRightPhotoSize.Col - iRightPivotCol) * cos(rRightAngle)) -
															((1 - iRightPivotRow) * sin(rRightAngle));
	double row7 = ((rcRightPhotoSize.Row - iRightPivotRow) * cos(rRightAngle)) +
															((rcRightPhotoSize.Col - iRightPivotCol) * sin(rRightAngle));
	double col7 = ((rcRightPhotoSize.Col - iRightPivotCol) * cos(rRightAngle)) -
															((rcRightPhotoSize.Row - iRightPivotRow) * sin(rRightAngle));
	double row8 = ((rcRightPhotoSize.Row - iRightPivotRow) * cos(rRightAngle)) +
															((1 - iRightPivotCol) * sin(rRightAngle));
	double col8 = ((1 - iRightPivotCol) * cos(rRightAngle)) -
															((rcRightPhotoSize.Row - iRightPivotRow) * sin(rRightAngle));
	double rMinimRowRight = min(min(row5,row6),min(row7,row8)) * rRightScale;
	double rMinimColRight = min(min(col5,col6),min(col7,col8)) * rRightScale;
	double rMaximRowRight = max(max(row5,row6),max(row7,row8)) * rRightScale;
	double rMaximColRight = max(max(col5,col6),max(col7,col8)) * rRightScale;
	double rMinimAllRow = min(rMinimRowLeft,rMinimRowRight);
	double rMinimAllCol = min(rMinimColLeft,rMinimColRight);
	double rMaximAllRow = max(rMaximRowLeft,rMaximRowRight);
	double rMaximAllCol = max(rMaximColLeft,rMaximColRight);
	double rRectangleHegiht = rMaximAllRow - rMinimAllRow + 1;
	double rRectangleWidth = rMaximAllCol - rMinimAllCol + 1;
	// rNewPivRowCommon = - rMinimAllRow + 1;
	// rNewPivColCommon = - rMinimAllCol + 1;//deze 4 statements
	// rNewCommonHeight = rRectangleHegiht;	// als de alternatieve
	// rNewCommonWidth = rRectangleHegiht; //boundrectan wordt gebruikt
	GeoRefScaleRotate grScaRotLeft(fnGrfEpiLeft,mpLeftInputPhoto->gr(),
																iLeftPivotRow, iLeftPivotCol,
																rLeftAngle, rLeftScale,
																rNewPivRowCommon, rNewPivColCommon);
	GeoRefScaleRotate grScaRotRight(fnGrfEpiRight,mpRightInputPhoto->gr(),
																iRightPivotRow, iRightPivotCol,
																rRightAngle, rRightScale,
																rNewPivRowCommon, rNewPivColCommon);
	
	FileName fnLeftIn(mpLeftInputPhoto->fnObj);
	FileName fnRightIn(mpRightInputPhoto->fnObj);
	// The left & right georefs declared below must be pointers in order to
	// correctly be passed with SetPointer.
	// hopefully they will be auto-deleted from memory
	
	// At this point we know the "Coord" size that the new georef should have
	// (= exactly rOverlapW x rOverlapH)
	// The puzzle is to make sure all 4 NEW corners reside in the rectangle
	// This is done for the upper-left corner with original RC = (0,0)
	
	
	
	RowCol rcCommonSize = //RowCol(mmLimits.rcSize());
		RowCol(rNewCommonHeight, rNewCommonWidth);
	grScaRotLeft.SetRowCol(rcCommonSize);
	grScaRotRight.SetRowCol(rcCommonSize);

	grScaRotLeft.fErase = true;
	grScaRotRight.fErase = true;
	grScaRotLeft.Store();
	grScaRotRight.Store();

	String sResampleMethod;
	if(	 (mpLeftInputPhoto->dm()->pdi() || 
				mpLeftInputPhoto->dm()->pdv() || 
				mpLeftInputPhoto->dm()->pdcol())
				&& 
				(mpRightInputPhoto->dm()->pdi() || 
				mpRightInputPhoto->dm()->pdv() || 
				mpRightInputPhoto->dm()->pdcol()))
			sResampleMethod = String("BiLinear");
	else
			sResampleMethod = String("NearestNeighbour");

	// If file already exists, delete it (overwrite)
	if (ptr.mapLeft.fValid())
	{
		ptr.mapLeft->fErase = true;
		ptr.mapLeft.SetPointer(0);
	}
	
	ptr.mapLeft = Map(fnEpiPhotoLeft,
										String("MapResample(%S,%S,%S,Patch,GrNoneAllowed)", 
										fnLeftIn.sFullNameQuoted(), grScaRotLeft.fnObj.sFullNameQuoted(),sResampleMethod));
  // ptr.mapLeft->fErase = false;
  // mpLeftOutputPhoto->Store();

	// If file already exists, delete it (overwrite)
	if (ptr.mapRight.fValid())
	{
		ptr.mapRight->fErase = true;
		ptr.mapRight.SetPointer(0);
	}

	ptr.mapRight = Map(fnEpiPhotoRight,
										String("MapResample(%S,%S,%S,Patch,GrNoneAllowed)", 
										fnRightIn.sFullNameQuoted(), grScaRotRight.fnObj.sFullNameQuoted(),sResampleMethod));
  // ptr.mapRight->fErase = false;
	String sDescrp(SMAPTextStereoMateOf_S.scVal(), ptr.sName());
	String sDescrLeft("Left-hand %S", sDescrp);
	String sDescrRight("Right-hand %S", sDescrp);
	ptr.mapLeft->sDescription = sDescrLeft;
	ptr.mapRight->sDescription = sDescrRight;
	// mpLeftOutputPhoto->Store();
  // mpRightOutputPhoto->Store();

	// Now perform the resampling
	ptr.mapLeft->Calc();
	if (!ptr.mapLeft->fCalculated()) 
		return false;

	// Now prepare the left-image resample result for deletion
	// This is needed to cover the situation in which the right-image
	// resampling action is stopped by the user
	ptr.mapLeft->fErase = true;

	ptr.mapRight->Calc();
	if (!ptr.mapRight->fCalculated()) 
		return false;

	// Now the right result and left result are both accepted
	ptr.mapLeft->fErase = false;
	
	ptr.mapLeft->BreakDependency();
	ptr.mapRight->BreakDependency();
//	ptr.mapLeft = mpLeftOutputPhoto;
	grScaRotLeft.fErase = false;
	grScaRotRight.fErase = false;
  return true;
}

GeoRefEpipolar *StereoPairEpiPolar::pGetEpiLeft()
{
	return grNewLeft->pgEpipolar();
}

GeoRefEpipolar *StereoPairEpiPolar::pGetEpiRight()
{
	return grNewRight->pgEpipolar();
}

String StereoPairEpiPolar::sLeftInputMapName() const
{
	return sLeftInputPhoto;
}

String StereoPairEpiPolar::sRightInputMapName() const
{
	return sRightInputPhoto;
}

String StereoPairEpiPolar::sErrGeorefs()
{
	String sErr = ""; // default: no error to report
	GeoRefEpipolar * grEpiLeft = grNewLeft->pgEpipolar();
	GeoRefEpipolar * grEpiRight = grNewRight->pgEpipolar();
	int iErrLeft = grEpiLeft->iLastComputeError();
	int iErrRight = grEpiRight->iLastComputeError();
	if (iErrLeft != iUNDEF && iErrRight != iUNDEF)
	{
		int iErr = 0; // The dominant error message: first left, then right, and if no error check combo
		if (iErrLeft)
		{
			iErr = iErrLeft;
			sErr = SStcUILeftPhoto;
		}
		else if (iErrRight)
		{
			iErr = iErrRight;
			sErr = SStcUIRightPhoto;
		}
		switch (iErr)
		{
			case 0: // no error in left or right; check if combination is ok
				if (grEpiLeft->iNrOffFlight != grEpiRight->iNrOffFlight)
					sErr = SStcErrWrongNrScPts;
				break;
			case -12:
				sErr += SDGErrTooFewFiducials;
				break;
			case -13:
				sErr += SDGErrIncorrectFiducialPos;
				break;
			case -14:
				sErr += SDGErrIncorrectFiducialOrder;
				break;
			case -15:
				sErr += SDGErrWrongNumber;
				break;
			case -16:
				sErr += SGRErrPhotobaseTooShort;
				break;
			case -17:
				sErr += SGRErrTPPAtWrongSide;
				break;
			case -18:
				sErr += SGRErrScalePt1TooClose;
				break;
			case -19:
				sErr += SGRErrScalePt2TooClose;
				break;
		}
	}
	else // should NEVER happen !!!
		sErr = SStcErrGrfNotYetCmp;
	
	return sErr;
}

String StereoPairEpiPolar::sExpression() const
{
	String sL = sLeftInputPhoto;
	String sR = sRightInputPhoto;
	String sPair = ptr.sName();
	return String(SStcTextStereopairFrom_S_S_S.scVal(),sPair,sL,sR);
}
