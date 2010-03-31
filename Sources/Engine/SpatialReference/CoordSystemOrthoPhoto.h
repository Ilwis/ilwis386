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
/* CoordSystemOrthoPhoto.h
   Copyright Ilwis System Development ITC
   september 1999, by Jan Hendrikse
*/

#ifndef ILWCSORTHOPHOTO_H
#define ILWCSORTHOPHOTO_H

#include "Engine\SpatialReference\csctp.h"
#include "Engine\SpatialReference\SolveOrthoPhoto.h"

class _export CoordSystemOrthoPhoto: public CoordSystemCTP,private SolveOrthoPhoto
{
  friend class CoordSystemPtr;
  friend class CoordSystemEditor;
  friend class EditFiducialMarksForm;
  CoordSystemOrthoPhoto(const FileName&);
public:  
  CoordSystemOrthoPhoto(const FileName&, const Map& mapDTM, const FileName& fnRefMap);
  ~CoordSystemOrthoPhoto();
  virtual String sType() const;
  virtual double rPixSize() const;
  virtual Coord cConvFromOther(const Coord&) const;
  virtual Coord cConvToOther(const Coord&) const;
  virtual void Store();
  virtual int Compute();
  virtual int iMinNr() const; 
	Coord crdGetPrincipalPoint();
	void GetObjectStructure(ObjectStructure& os);
  double rHeightAccuracy;
  int iMaxOrientCount;
  int iMaxHeightCount; 
  Map mapDTM;
protected:
  int InnerOrientation();
  int OuterOrientation();
  bool fUseCornerFiducials; // == true if fiducials in corners; false if fiduc at edges
  CoordBounds CalcCoordBounds(MinMax& mm);
  CoordBounds cbDTM;
  int iNrFiduc; 
  double rScannedFidRow[4];
  double rScannedFidCol[4];
  double rResidRow[4];
  double rResidCol[4];
  Coord cPhotoFid[4];
private:
  String sFormula() const;
  double rAvgScale, rPhoto2ScanScale;
  RealMatrix rmRotationU2X, rmRotationX2U ;
  mutable CVector vecProjCenter, PhotoCrd, TerrainCrd;
  double rKappaFinal;                          // camera kappa swing in degrees
  double rPhiFinal;                          // camera phi tilt in degrees
  double rOmegaFinal;                       // camera omega tilt in degrees
  int iIterations;        // nr of iterations in Outer Camera orientation compute()
  mutable RVector vec1stRow, vec2ndRow, vec3rdRow; 
  void init();
  void RC2Crd(double rR, double rC, Coord& c);
  void Crd2RC(const Coord& c, double& rR, double& rC);
  /// Coord cPrincPoint;    // x,y position of pp in camera coord system (usually 0,0)
  Coord cScanPrincPoint;  // position of pp in coord (doubles) of DIGITIZER image
  double rScanPrincPointRow;  // position of pp in rowcols of DIGITIZER TABLET 
  double rScanPrincPointCol;
  double rPrincDistance;     // dist from camera center to photoplane in mm
  double rScanPrincDistance; // princ distance scaled to rowcol units in DIGITIZER TABLET
  double rEstimTerrainHeight;  // Inilial terrain height guess
  double rPrevTerrainHeight;   // needed when segment digitizing in monoplot
};

#endif // ILWCSORTHOPHOTO_H
