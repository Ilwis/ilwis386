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
/* $Log: /ILWIS 3.0/GeoReference/GR3D.H $
 * 
 * 8     11/15/01 17:06 Hendrikse
 * added rMaxClimb member for the case of 'climbing' viewline, (observer
 * lower than focus point)
 * 
 * 7     8/10/01 20:39 Hendrikse
 * 3 new 'Set' functions to set parameters needed for MapVisibility
 * 
 * 6     2/22/01 19:45 Retsios
 * Moved some member variables that were used as local variables to
 * locals.
 * Reason: solve bug # 4317: strange lines in 3d grid
 * Errors would occur due to interference of the functions Col2RC and
 * RC2Col with eachother in GeoRef3D (if during calculating the image
 * mouse-movements occurred). These functions also were not re-entrant in
 * both GeoRef3D and GeoRefOrthoPhoto, but it is not certain if this ever
 * occurred.
 * Disadvantage of the change: probably a performance penalty due to the
 * constructor calls for every call to the functions Col2RC and RC2Col.
 * 
 * 5     19-12-00 8:19a Martin
 * added getobjectstructure(..)
 * 
 * 4     6-12-00 2:49p Martin
 * added acces function for members
 * 
 * 3     1-12-00 16:29 Koolhoven
 * header comment
 * 
 * 2     1-12-00 4:23p Martin
 * added export
//Revision 1.5  1998/09/16 17:24:43  Wim
//22beta2
//
//Revision 1.4  1997/09/08 15:51:51  janh
//*** empty log message ***
//
//Revision 1.3  1997/09/08 15:09:51  janh
//*** empty log message ***
//
//Revision 1.2  1997/09/05 14:25:20  janh
//renamed Coord2RowCol(Coord&, double&, double&, double) into
//Crd2RC(Coord&, double&, double&, double) and made it private
//changed the names also in the calls from GeoRef3D::Coord2RowCol in gr3d.c
//from MapApply3D::MakePoints in \mapfmap\map3d.c
//and from *Grid3DDrawer::MakePoints (3 places) in ann\grid3drwr.c
//
/* GeoRef3D
   Copyright Ilwis System Development ITC
   may 1996, by Wim Koolhoven
	Last change:  JHE   8 Sep 97    4:51 pm
*/


#ifndef ILWGR3D_H
#define ILWGR3D_H
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Map\Raster\Map.h"
//#include "Engine\Drawers\Drawer.h"

class DATEXPORT GeoRef3D: public GeoRefPtr
{
  friend class GeoRefPtr;
  friend class _export GeoRef3DEditor;
  friend class _export GeoRef3DForm;
  friend class _export Grid3DDrawer;
  friend class DATEXPORT MapApply3D;
public:
  GeoRef3D(const FileName& fn);
  _export GeoRef3D(const FileName& fn, RowCol rc, const Map& mapDTM);
  _export ~GeoRef3D();
  virtual void _export Store();
  virtual void Coord2RowCol(const Coord& c, double& rRow, double& rCol) const;
  virtual void RowCol2Coord(double rRow, double rCol, Coord& crd) const;
  virtual String sType() const;
  virtual void GetObjectDependencies(Array<FileName>& afnObjDep);
  virtual void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const;
	double _export rGetViewAngle() const;
	Coord _export crdFocalPoint() const;
	void _export SetFocalPoint(const Coord& crd);
	Coord _export crdViewPoint() const;
	void _export SetViewPoint(const Coord& crd);
	void _export SetViewHeight(const double& rH);
	void _export SetScaleHeight(const double& rS);
	void _export SetViewAngle(const double& rA);

	void _export GetObjectStructure(ObjectStructure& os);
//protected:
  void _export CalcTrig();
  Map mapDTM;
  CoordBounds CalcCoordBounds(const MinMax& mm); 
  CoordBounds cbDTM; ///cbFieldofView;
private:
  void init();
  void MakeThetaPhiMatrix(RealMatrix& rmThetaPhi);
  void FindOuterOrientation(RealMatrix& rmRotx2u, RealMatrix& rmRotu2x);
  void _export ComputeRot();
  void _export ComputeLoc();
  double rAngle, rScaleH, rDistance, rPhi, rTheta; ///rFieldofViewSize;
  double rScaleFactor, rDTMPixSize;
  double rPrincDistance, rHeightScaleCorrection;
  double rMaxDepth, rMinimumTerrainHeight, rMaximumTerrainHeight, rMaxClimb;
  Coord cView, cLoc;
  double rViewH, rLocH;
  double rCosPhi, rSinPhi, rSinTheta, rCosTheta, rTanHalfAngle; ///rFactorRC2XY; 
  RealMatrix rmRotationX2U, rmRotationU2X;
  ///int iNrSuccesRowCol2Coord; // how many times RC2Coord function was succesfully used  
#ifdef JANH
  int iNrCallsRowCol2Coord; // how many times the RC2Coord function was called                                                
#endif 
  void _export Crd2RC(const Coord& c, double& rR, double& rC, double rHeight);
  void _export RC2Crd(double rR, double rC, Coord& crd);   
};

#endif




