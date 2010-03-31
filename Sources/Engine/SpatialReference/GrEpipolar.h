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

#ifndef GREPIPOL_H
#define GREPIPOL_H
#include "Engine\SpatialReference\GrScaleRotate.h"

class DATEXPORT GeoRefEpipolar: public GeoRefScaleRotate
{
  friend class GeoRef;
  friend class GeoRefPtr;
	// friend class _export MakeEpipolarFormView;
	friend class MakeEpipolarDocument;
	friend class StereoPairEpiPolar;
  static GeoRefEpipolar* create(const FileName& fn, const String& sExpression);
public:
	_export GeoRefEpipolar(const bool fLeft);
  _export GeoRefEpipolar(const FileName& fn, 
												const double rPivotRow, const double rPivotCol, 
												const double rAngle, const double rScale,
												const bool fLeft);

  virtual String sName(bool fExt = false, const String& sDirRelative = "") const;
  virtual String sType() const;
  virtual String sTypeName() const;
	int _export GeoRefEpipolar::Compute();
	Array<Coord> crdGetFiducials();
	_export RowCol rGetPivotRowCol();
	_export double rGetRotAngle();
	_export double rGetScaleFactor();
	_export void SetPivotRowCol(const RowCol rcPiv);
	_export void SetRotAngle(const double rAng);
	_export void SetScaleFactor(const double rScale);
	_export void SetRowCol(RowCol rcSize);
	int _export iLastComputeError(); // to prevent doing unnecessary computes
protected:
	double  rDotProd(Coord cA, Coord cB);

  double rNormSquared(Coord cA);

  //double rCrossProd(RowCol rcA, RowCol rcB, RowCol rcC, RowCol rcD); // vector AB X vector DC
  double rCrossProd(Coord cA, Coord cB, Coord cC, Coord cD); // (vector AB) X (vector DC)

  //bool fPerpProjection(RowCol rcSide1, RowCol rcSide2, RowCol &rcFoot);
  bool fPerpProjection(Coord cSide1, Coord cSide2, Coord &cFoot); // project end point of side2 
	int  _export iFindPrincPointLocation(); 
	int _export iLocateTransferredPrincPoint();
	int _export iFindRotationAngle();
	int _export iFindScalingFactor();
	void _export SetMatrixGrSmplCoefficients(
										const double rPivotRow_in, const double rPivotCol_in,
										const double rAngle_in, const double rScale_in, 
										const bool fLeft_in);
  int iNrFiduc;
	int iNrOffFlight;
  double rScannedFidRow[4];
  double rScannedFidCol[4];
	bool fSelectedFiducial[4];
	double rScanPrincPointRow;  // position of pp in rowcols of scanned image
  double rScanPrincPointCol;
	bool fLeftPhoto;
	double rScannedScalingPntRow[2];
	double rScannedScalingPntCol[2];
	bool fSelectedScalingPoint[2];
	
private:  
  Coord cScanPrincPoint;  // position of pp in coord (doubles) of scanned image
	Coord cScanTransfPrincPoint;
//	double rPivotRow, rPivotCol;
//	double rAngle, rScale;
	void InitMembers(); // call in constructor to get a consistent state
	int iLstCmptErr;
};

#endif // GREPIPOL_H
