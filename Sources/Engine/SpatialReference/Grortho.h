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
/* GeoRefOrthoPhoto   grOrtho.h
   Copyright Ilwis System Development ITC
   january 1997, by Jan Hendrikse
	Last change:  WK   12 Feb 98    4:49 pm
*/

#ifndef ILWGRORT_H
#define ILWGRORT_H

#include "Engine\Map\Raster\Map.h"
#include "Engine\SpatialReference\GRCTP.H"
#include "Engine\SpatialReference\SolveOrthoPhoto.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"

class DATEXPORT GeoRefOrthoPhoto: public GeoRefCTP, private SolveOrthoPhoto
{
  friend class GeoRefPtr;
  friend class _export GeoRefEditor;
  friend class _export EditFiducialMarksForm;
  GeoRefOrthoPhoto(const FileName&);
public:  
  _export GeoRefOrthoPhoto(const FileName&, const Map& mapDTM, RowCol, bool fSubPixelPrecise);
  ~GeoRefOrthoPhoto();
  virtual String sType() const;
  virtual double rPixSize() const;
  virtual void Coord2RowCol(const Coord& c, double& rR, double& rC) const;
  virtual void RowCol2Coord(double rR, double rC, Coord& c) const;
  virtual bool fEqual(const IlwisObjectPtr&) const;
  virtual void Store();
  virtual void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  virtual int Compute();
  virtual int iMinNr() const; 		
  virtual void Rotate(bool fSwapRows, bool fSwapCols, bool fRotate);
	RowCol _export rcGetPrincipalPoint();
	Coord _export crdGetPrincipalPoint();
	void GetObjectStructure(ObjectStructure& os);
  double rHeightAccuracy;
  int iMaxOrientCount;
  int iMaxHeightCount; 
  Map mapDTM;
	bool fAffineInnerOrientation;
	
protected:
  int InnerOrientation();
  int OuterOrientation();
  CoordBounds CalcCoordBounds(MinMax& mm);
  CoordBounds cbDTM;
  int iNrFiduc; 
  double rScannedFidRow[8];
  double rScannedFidCol[8];
  double rResidRow[8];
  double rResidCol[8];
  Coord cPhotoFid[8];
	double rScanPrincPointRow;  // position of pp in rowcols of scanned image
  double rScanPrincPointCol;
	
private:
  String sFormula() const;
  double rAvgScale, rPhoto2ScanScale;
  RealMatrix rmRotationU2X, rmRotationX2U; //3d (orthogonal) trnsf from scan to terrain
  int iIterations;        // nr of iterations in Outer Camera orientation compute()
  void init();
  void RC2Crd(double rR, double rC, Coord& c);
  void Crd2RC(const Coord& c, double& rR, double& rC);
  /// Coord cPrincPoint;    // x,y position of pp in camera coord system (usually 0,0)
  Coord m_cScanPrincPoint;  // position of pp in coord (doubles) of scanned image
  
  double rScanPrincDistance; // princ distance scaled to rowcol units in scanned image
  double rEstimTerrainHeight;  // Inilial terrain height guess
  double rPrevTerrainHeight;   // needed when segment digitizing in monoplot
};

#endif // ILWGRORT_H




