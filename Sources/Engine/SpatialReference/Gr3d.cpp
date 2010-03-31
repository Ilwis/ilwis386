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
/* $Log: /ILWIS 3.0/GeoReference/Gr3d.cpp $
 * 
 * 16    11/15/01 17:46 Hendrikse
 * default Observerpnt (cLoc) (viewpnt in the interface) is now soutward
 * of the dtm area
 * implemented use of Up going view line 
 * 
 * 15    8/10/01 20:40 Hendrikse
 * implemented 3 new Set functions for MapVisibility
 * 
 * 14    12-03-01 11:49a Martin
 * typo in extending the minimappane on all sides. use rDX instead of rDY
 * 
 * 13    28-02-01 2:32p Martin
 * default scale is 2.0
 * 
 * 12    2/22/01 19:45 Retsios
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
 * 11    8-02-01 8:28a Martin
 * changed defaults for viewangel and viewheigt
 * 
 * 10    19-12-00 8:19a Martin
 * added getobjectstructure(..)
 * 
 * 9     19-12-00 8:16a Martin
 * added dtm to getobjectstructure
 * 
 * 8     14-12-00 11:10a Martin
 * new default cLoc for gr3d (outside the map looking accross the
 * diagonal)
 * 
 * 7     11-12-00 8:57a Martin
 * added an error message for a missing DTM
 * 
 * 6     8-12-00 4:32p Martin
 * added acces functions for gr3d
 * 
 * 5     6-12-00 2:49p Martin
 * added acces function for members
 * 
 * 4     24-11-00 11:55 Koolhoven
 * header comment
 * 
 * 2     23-11-00 2:46p Martin
 * constructor protected against invalid dtm's
// Revision 1.15  1998/09/16 17:24:43  Wim
// 22beta2
//
// Revision 1.14  1997/11/28 14:55:50  janh.ilwis.itc
// In GeoRef3D constructor set coordsys of grDTM in staead of mapDTM->cs()
//
// Revision 1.13  1997/09/30 07:56:16  Wim
// In constructor if cs differs from cs of dtm replace it.
//
// Revision 1.12  1997-09-16 14:07:04+02  janh
// Static void GeoRefCornersError added and called in 2nd constructor
//
// Revision 1.11  1997/09/08 15:54:45  janh
// *** empty log message ***
//
// Revision 1.10  1997/09/05 14:38:42  janh
// renamed  Coord3RowCol into Crd2RC and made private (see gr3d.h revision)
//
// Revision 1.9  1997/09/04 09:54:48  janh
// try and catch removed from 2nd constructor and from init()
// check on mapDTM.fValid placed before call to geoRefNoneError in init()
//
// Revision 1.8  1997/09/03 17:28:49  janh
// I added static void GeoRefNoneError(fn,err)
// it is called at 2 places: in the 2nd constructor and in init()
//
// Revision 1.7  1997/09/02 14:44:35  janh
// Min and Maxterrainheight are now scaled outside RowCol2Coord function
//
// Revision 1.6  1997/09/01 16:21:25  janh
// In Bresenham algorithm TerrainScaling replaced by rW "descaling"
//
// Revision 1.5  1997/08/25 08:41:02  Wim
// Added ObjectDependency in Store() to be sure that Copy() works
// properly
//
// Revision 1.4  1997-08-15 11:11:50+02  Wim
// Protected other functions against invalid mapDTM
//
// Revision 1.3  1997-08-15 10:58:53+02  Wim
// Protect destructor against invalid mapDTM
//
// Revision 1.2  1997-08-15 10:53:44+02  Wim
// If mapDTM is missing do not call Init()
//
/* GeoRef3D
   Copyright Ilwis System Development ITC
   may 1996, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:12 pm
*/

#include "Engine\SpatialReference\GR3D.H"
#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Base\Algorithm\Realmat.h"
#include "Engine\Base\Algorithm\Clipline.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Headers\Hs\GEOREF3D.hs"
#define EPS10 1.e-10
#define EPS16 1.e-16

static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

static void GeoRefCornersError(const FileName& fn, IlwisError err)
{
  throw ErrorMapWithoutGeoRefCorners(fn, err);
}


GeoRef3D::GeoRef3D(const FileName& fn)
: GeoRefPtr(fn)
{
  ReadElement("GeoRef3D", "DTM", mapDTM);
	if ( mapDTM.fValid() )
	{
	  GeoRef grDTM = mapDTM->gr();
	  if (cs() != grDTM->cs())
	    SetCoordSystem(grDTM->cs());
	  ReadElement("GeoRef3D", "Scale Height", rScaleH);
	  init();
	  ReadElement("GeoRef3D", "Angle", rAngle);
	  ReadElement("GeoRef3D", "View", cView);
	  ReadElement("GeoRef3D", "Location", cLoc);
	  ReadElement("GeoRef3D", "Location Height", rLocH);
	  ///ComputeRot();
	  ReadElement("GeoRef3D", "Distance", rDistance);
	  ReadElement("GeoRef3D", "Horizontal Rotation", rPhi);
	  ReadElement("GeoRef3D", "Vertical Rotation", rTheta);
	  ComputeLoc();
	}
	else
		throw ErrorObject(S3ErrDTMNotValid);
}	

GeoRef3D::GeoRef3D(const FileName& fn, RowCol rc, const Map& mp)
: GeoRefPtr(fn, mp->cs(), rc),
  mapDTM(mp)
{
  GeoRef grDTM = mapDTM->gr();
  if (grDTM->fGeoRefNone())
    GeoRefNoneError(mapDTM->fnObj, errGeoRef3D);
  if (!grDTM->fNorthOriented())    // if the DTM mp has not a GeoRefCorners
    GeoRefCornersError(mapDTM->fnObj, errGeoRef3D + 1);
  CoordBounds cb = mapDTM->cb();
  rScaleH = 2;
  cView.x = (cb.MinX() + cb.MaxX()) / 2; 
  cView.y = (cb.MinY() + cb.MaxY()) / 2; 
  rAngle = 110;
  if (mapDTM.fValid())
 //   rDistance = mapDTM->gr()->rPixSize() * max(mapDTM->rcSize().Row, mapDTM->rcSize().Col) / 2;
	{
		Coord cMax = cb.cMax;
		Coord cMin = cb.cMin;
		double rDis = rDist(cMax, cMin);
		cLoc.x = cView.x;///+ ( cMax.x - cMin.x ) * 0.7;
		cLoc.y = cView.y - ( cMax.y - cMin.y ) * 0.7;
		rDis = rDist(cLoc, cView);
		rLocH = rDis; // * tan( 35.0 * M_PI/180.0);
	}   
  init();
  ComputeRot();
}

GeoRef3D::~GeoRef3D()
{
  if (mapDTM.fValid())
    mapDTM->KeepOpen(false);
}

void GeoRef3D::init()
{
  if (!mapDTM.fValid())
    return;

  if (mapDTM->gr()->fGeoRefNone())
    GeoRefNoneError(mapDTM->fnObj, errGeoRef3D);

  rmRotationX2U = RealMatrix(3);
  rmRotationU2X = RealMatrix(3);
///  iNrSuccesRowCol2Coord = 1;
#ifdef JANH
  iNrCallsRowCol2Coord = 0;
#endif
  mapDTM->KeepOpen(true);
  ///RangeReal rrMM = mapDTM->rrMinMax(true);
  rDTMPixSize = mapDTM->gr()->rPixSize();
  rPrincDistance = max(rcSize().Row, rcSize().Col);
  ///rMinimumTerrainHeight = rrMM.rLo();   set and scaled when needed
  ///rMaximumTerrainHeight = rrMM.rHi();    "        "           "
  MinMax mm(RowCol(0.0,0.0), mapDTM->rcSize());
  //mm.MaxRow() -= 1;
  //mm.MaxCol() -= 1;
  cbDTM = CalcCoordBounds(mm); // find the edges of DTM raster mp in Coords
}

void GeoRef3D::Store()
{
  if (!mapDTM.fValid())
    return;
  GeoRefPtr::Store();
  WriteElement("GeoRef", "Type", "GeoRef3D");
  WriteElement("GeoRef3D", "DTM", mapDTM);
  WriteElement("GeoRef3D", "Angle", rAngle);
  WriteElement("GeoRef3D", "Scale Height", rScaleH); // vertical scaling ('exaggeration')
  WriteElement("GeoRef3D", "View", cView);          // with cView as fixed point
  WriteElement("GeoRef3D", "ViewHeight", rViewH);
  WriteElement("GeoRef3D", "Location", cLoc);
  WriteElement("GeoRef3D", "Location Height", rLocH);
  WriteElement("GeoRef3D", "Distance", rDistance);
  WriteElement("GeoRef3D", "Horizontal Rotation", rPhi);
  WriteElement("GeoRef3D", "Vertical Rotation", rTheta);
#ifdef JANH
  WriteElement("GeoRef3D", "NrCallsRowCol2Coord", iNrCallsRowCol2Coord);
#endif
  ObjectDependency objdep;
  objdep.Add(mapDTM);
  objdep.Store(this);
}

String GeoRef3D::sType() const
{
  return "GeoReference 3D";
}

void GeoRef3D::ComputeRot()
{
  if (!mapDTM.fValid())
    return;
  double rDX, rDY, rDZ, rD, rD2;
  bool fComputeLoc=false;
  rViewH = mapDTM->rValue(cView);
  if (rScaleH != 1) {
    rScaleH = max(rScaleH, 0.001);  // no vertical scales less then 0.001 allowed
    fComputeLoc = true;
  }
  if (rViewH == rUNDEF || cLoc.fUndef() || cView.fUndef() || rScaleH == rUNDEF) return;
  rDX = cLoc.x - cView.x;    // distance between observer point cLoc (proj center)
  rDY = cLoc.y - cView.y;    // and viewpoint cView is computed
  rDZ = rLocH - rViewH;
  rD = rDX * rDX + rDY * rDY;
  rD2 = sqrt(rD);
  if (rD2 < 1) rD2 = 1;
  rD = rDX * rDX + rDY * rDY + rDZ * rDZ;
  if (rD < 1) rD = 1;
  rDistance = sqrt(rD);
  if (rD2 > 1)
    rPhi = 90.0 + atan2(rDY, rDX) * 180.0 / M_PI;
  else

    rPhi = 0.0;
  if (rPhi > 180.0) rPhi -= 360;

  rTheta = 90.0 - atan2(rDZ, rD2) * 180.0 / M_PI;
  if (rTheta < 0.0) {
    rTheta = 0;
    fComputeLoc=true;
  }  
  ///else if (rTheta > 90.0) {
  ///  rTheta = 90;
  ///  fComputeLoc=true;
  ///}
  if ( fComputeLoc ) ComputeLoc();
  CalcTrig();
}

void GeoRef3D::ComputeLoc()
{
  if (!mapDTM.fValid())
    return;
  if (!cbDTM.fContains(cView))  { // cView is outside DTM bounds (changed wrongly) in grf editor)
    cView.x = (cbDTM.MinX() + cbDTM.MaxX()) / 2;
    cView.y = (cbDTM.MinY() + cbDTM.MaxY()) / 2;            // force it to the center
  }
  rViewH = mapDTM->rValue(cView);
  if (rViewH == rUNDEF || rPhi == rUNDEF || rTheta == rUNDEF) return;

  cLoc.x = cView.x + sin(M_PI/180.0 * rPhi)
                   * sin(M_PI/180.0 * rTheta) * rDistance;
  cLoc.y = cView.y - cos(M_PI/180.0 * rPhi)
                   * sin(M_PI/180.0 * rTheta) * rDistance;
  rLocH =  rViewH + cos(M_PI/180.0 * rTheta) * rDistance ;
  CalcTrig();
}

void GeoRef3D::CalcTrig()
{
  if (!mapDTM.fValid())
    return;
  rCosPhi = cos(M_PI/180.0 * rPhi);
  rSinPhi = sin(M_PI/180.0 * rPhi);
  rCosTheta = cos(M_PI/180.0 * rTheta);
  rSinTheta = sin(M_PI/180.0 * rTheta);
  ///if (rDistance < 1)
  rAngle = max(0.1, abs(rAngle));       // avoid view angle being negative or nearly zero
  rAngle = min(179.0, rAngle);            // avoid superwide angle, i.e. too small scale
  rTanHalfAngle = tan(M_PI/360.0 * rAngle); // tangent of half view angle

///  rFactorRC2XY = min(rcSize().Row, rcSize().Col)
///               / max(mapDTM->cb().width(), mapDTM->cb().height());
///                   / max(cbDTM.width(), cbDTM.height());
  rScaleFactor = rPrincDistance / rTanHalfAngle;
  rHeightScaleCorrection = rViewH * (1.0 - rScaleH);
  RangeReal rrMM = mapDTM->rrMinMax(true);
  rMinimumTerrainHeight = rrMM.rLo() * rScaleH + rHeightScaleCorrection;//ScaleCorrected
  rMaximumTerrainHeight = rrMM.rHi() * rScaleH + rHeightScaleCorrection;//     "
  rMaxDepth = rLocH - rMinimumTerrainHeight;
	rMaxClimb = rMaximumTerrainHeight - rLocH;
  FindOuterOrientation(rmRotationX2U, rmRotationU2X);
}

void GeoRef3D::Coord2RowCol
  (const Coord& c, double& rRow, double& rCol) const
{
  if (mapDTM.fValid())
    const_cast<GeoRef3D *>(this)->Crd2RC(c, rRow, rCol, mapDTM->rValue(c));
  else
    rRow=rCol=rUNDEF; 
}

void GeoRef3D::Crd2RC
  (const Coord& c, double& rRow, double& rCol, double rHeight)
{
  if (rViewH == rUNDEF || rHeight == rUNDEF || !mapDTM.fValid())
    { rRow=rCol=rUNDEF; return; }
  ///double rXs = c.x - cView.x;
  ///double rYs = c.y - cView.y;
  ///double rZs = rHeight * rScaleH - rViewH; 
	CVector TerrainCrd(3);
	TerrainCrd(0) = c.x - cLoc.x;
  TerrainCrd(1) = c.y - cLoc.y;
  TerrainCrd(2) = rScaleH * rHeight - rLocH + rHeightScaleCorrection;

  RVector vec1stRow(3), vec2ndRow(3), vec3rdRow(3);
	int i = 0;
  for (; i < 3; i++) {
    vec1stRow(i) = rmRotationU2X(0,i);
    vec2ndRow(i) = rmRotationU2X(1,i);
    vec3rdRow(i) = rmRotationU2X(2,i);
  }
  double rZ = vec3rdRow * TerrainCrd;
  if (rZ > -1)  {      // terrain distant less than 1 meter from observer
     rRow=rCol=rUNDEF;   // or behind observer if rZ > 0
     return;
  }

  rCol =  - rScaleFactor * (vec1stRow * TerrainCrd) / rZ;  // can the denominator be 0 ?
  rRow =  rScaleFactor * (vec2ndRow * TerrainCrd) / rZ;  // only if points are very far outside photo
  rCol += rcSize().Col/2.0 ;
  rRow += rcSize().Row/2.0 ;
 
  ///double rX3 =  rXs * rCosPhi + rYs * rSinPhi;
  ///double rY3 = -rXs * rSinPhi + rYs * rCosPhi;
  ///double rZ3 = -rY3 * rSinTheta + rZs * rCosTheta;
  ///rY3 = rY3 * rCosTheta + rZs * rSinTheta;

  ///double rDZ = rDistance - rZ3;
  /*
  if (rDZ > 1) {
    double rFactor =  rFactorRC2XY * rDistance / (rDZ * rTanAngle);
    ///if (rAngle < 0.1)
    ///  rFactor /= rDistance;
    ///else
    //// rFactor /= rDZ * rFieldofViewSize ;

    rCol =  rX3 * rFactor + rcSize().Col / 2;
    rRow = -rY3 * rFactor + rcSize().Row / 2;   /*
  }
  else
    rRow=rCol=rUNDEF;     */
}

void GeoRef3D::MakeThetaPhiMatrix(RealMatrix& rmThetaPhi)
{
  if (!mapDTM.fValid())
    return;
   int i,j;
   RealMatrix rmUnit(3);
   for (i=0; i<3; i++) {   //
     for (j=0; j<3; j++) {
        rmUnit(i,j) = 0;
        if (i==j) rmUnit(i,j) = 1;
     }
   }
   RealMatrix rmTheta(rmUnit), rmPhi(rmUnit);
   rmPhi(0,1) = rSinPhi;
   rmPhi(1,0) = -rSinPhi;
   rmPhi(0,0) = rmPhi(1,1) = rCosPhi;
   rmTheta(1,2) = rSinTheta;
   rmTheta(2,1) = -rSinTheta;
   rmTheta(1,1) = rmTheta(2,2) = rCosTheta;
   rmTheta *= rmPhi;
   rmThetaPhi = rmTheta;     // yields Theta * Phi  matrix
}

CoordBounds GeoRef3D::CalcCoordBounds(const MinMax& mm)
{
  CoordBounds cb;
  if (!mapDTM.fValid())
    return cb;
  Coord c;
  GeoRef gr = mapDTM->gr();
  if (!gr.fValid())
    return cb;
  gr->RowCol2Coord(mm.MinRow()+0.5, mm.MinCol()+0.5, c);
  cb += c;
  gr->RowCol2Coord(mm.MinRow()+0.5, mm.MaxCol()-0.5, c);
  cb += c;
  gr->RowCol2Coord(mm.MaxRow()-0.5, mm.MinCol()+0.5, c);
  cb += c;
  gr->RowCol2Coord(mm.MaxRow()-0.5, mm.MaxCol()-0.5, c);
  cb += c;
  // to ensure that they are inside the mp
  ///cb.MinX() += 1;
  ///cb.MaxX() -= 1;
  ///cb.MinY() += 1;
  ///cb.MaxY() -= 1;
  return cb;
}  

void GeoRef3D::FindOuterOrientation(RealMatrix& rmRotx2u, RealMatrix& rmRotu2x)
{
  if (!mapDTM.fValid())
    return;
  RealMatrix rmRot;
  MakeThetaPhiMatrix(rmRot); //final rotation matrix from terrain to picture
  rmRotu2x = rmRot;
  rmRot.Transpose();
  rmRotx2u = rmRot;
}

void GeoRef3D::RowCol2Coord (double rRow, double rCol, Coord& crd) const
{
  if (!mapDTM.fValid()) {
    crd = Coord();
    return;
  }
// use a non constant member function to be able to change members TerrainCrd etc.
// which are help member variables
  const_cast<GeoRef3D *>(this)->RC2Crd(rRow, rCol, crd);
///   crd = Coord();
}

void GeoRef3D::RC2Crd(double rRow, double rCol, Coord& crd)
{
//  rRow = rcSize().Row / 3;
//  rCol = rcSize().Col / 3;
  crd = crdUNDEF;
  if (!mapDTM.fValid())
    return;
//  iNrCallsRowCol2Coord++;
  if (rRow == rUNDEF || rCol == rUNDEF) {
    ///crd.x = 111.0;
    ///crd.y = 111.0;
    return;
  }
  double rF1, rF2, rF3, rKu, rKv; // rPCw;
  //  rPCu, rPCv, replaced by cLoc
  CVector PhotoCrd(3);
  PhotoCrd(0) = (rCol - 0.5 - rcSize().Col/2) / rScaleFactor;    // Put photosyst origin in Princ point
  PhotoCrd(1) = (rcSize().Row/2 - rRow + 0.5) / rScaleFactor;
  // mirror reflection of Y coords to make scan (photo) coord syst right handed (like terrain syst)
  PhotoCrd(2) = - 1; 
                 ///// ***** ORIENTATION OF LIGHTRAY IN TERRAINSYSTEM
                 ///// ***** & CONVERSION OF COORDS TO DTM ROWCOL UNITS
  RVector vec1stRow(3), vec2ndRow(3), vec3rdRow(3);
	int i = 0;
  for (; i < 3; i++) {
    vec1stRow(i) = rmRotationX2U(0,i);
    vec2ndRow(i) = rmRotationX2U(1,i);
    vec3rdRow(i) = rmRotationX2U(2,i);
  }
  rF1 =  vec1stRow * PhotoCrd;
  rF2 =  vec2ndRow * PhotoCrd;
  rF3 =  vec3rdRow * PhotoCrd;
	bool fLookingUpward = (rF3 > EPS10);
  ///if (rF3 > -EPS10) {   // light ray horizontal or downward from terrain point crd to observer
       // no coordinates computed
    ///crd.x = 222.0;
    ///crd.y = 222.0;
	///if (abs(rF3) < EPS10) { 
  ///  return;
  ///}                     
	//**** FOLLOWING coordS are XY positions in TERRAIN COORD system ***//
  Coord  cB,    // lightray piercepnt with w==minimterrainheight  in terrain coordinate system
								// if fLookingUpward, it is the piercepnt with w==maximterrainheight
								// if Looking horizontal, cB is on the DTM edge, lying farrest from cLoc
         cEntry, cExit, // points where lightray pierces DTM cylinder vertical faces
         cStart, cFinish;  // points where Bresenham path starts, ends respectively
	if (abs(rF3) <= EPS10) { // lightray horizontal
		cB.x = (rF1 < 0) ? cbDTM.MinX() : cbDTM.MaxX();
		cB.y = (rF2 < 0) ? cbDTM.MinY() : cbDTM.MaxY();
	}
	else {
		rKu = rF1 / rF3;     // direction cotangents of lightray from current Photocrd
	  rKv = rF2 / rF3;     // in terrainsystem w.r.t. U (==X) and V (==Y) axis resp.

		if (rF3 < - EPS10) {  // looking downward
		  cB.x = -rKu * rMaxDepth + cLoc.x;
		  //  where the light ray pierces the W = Wminimum level plane
		  cB.y = -rKv * rMaxDepth + cLoc.y;
		}
		else if (rMaxClimb > 0)
		{
			cB.x = rKu * rMaxClimb + cLoc.x;
		  //  where the light ray pierces the W = Wmaximum level plane
		  cB.y = rKv * rMaxClimb + cLoc.y;
		}
		else
			return;
	}
  // Conversion of lightray parameters to DTM RowCol system:
  GeoRef gr = mapDTM->gr();
  RowCol  rcLoc, rcB;   // 2 points on lightray  in DTM RowCols
  rcLoc = gr->rcConv(cLoc);
  rcB  = gr->rcConv(cB);

                 ///// ***** FIND SIZE OF MOVES ALONG LIGHTRAY  IN Bresenham ALGORITHM
                 ///// *****
  long iMaxRowMove = rcB.Row - rcLoc.Row;
  long iMaxColMove = rcB.Col - rcLoc.Col;
  bool fRowMoveDominates = abs(iMaxRowMove) > abs(iMaxColMove);
  double rWmove;
	if(fLookingUpward)
		rWmove = abs(rMaxClimb) / max(abs(iMaxRowMove), abs(iMaxColMove));
	else
		rWmove = - abs(rMaxDepth) / max(abs(iMaxRowMove), abs(iMaxColMove));
  double wStart; //initial z-coord of point that will move down along the z-axis
                 // followin the horizontal Bresenham path
  int iRowMove = (iMaxRowMove >= 0) ? 1 : -1;
  int iColMove = (iMaxColMove >= 0) ? 1 : -1;

                 ///// ***** CLIPPING LIGHTRAY PATH IF NEEDED
                 ///// ***** REJECTING LIGHTRAYS OUTSIDE BOX
                 ///// ***** FINDING START AND FINISH FOR SEARCH

  bool fDTMContainsB = cbDTM.fContains(cB);
  if (cbDTM.fContains(cLoc) && fDTMContainsB) {  // no clipping needed, Bresenham path found
      cStart  = cLoc;
      cFinish = cB;
      wStart = rLocH;
  }
  else {
    cEntry = cLoc;                          // fClipLine will shift cLoc to cEntry and
    cExit = cB;                            // and cB to cExit if they are outside DTM cylinder
    double wEntry, wExit;                  // heights of ray at entry & exit cylinder
    if (fClipLine(cbDTM, cEntry, cExit)) { //
      if (abs(rKu) < EPS16 && abs(rKv) < EPS16) { // vertical lightray in terms of slope values
        crd = cLoc;
        return;
      }
      if (abs(rKv) > abs(rKu)) {   // fRowMoveDominates if gr north oriented
        wEntry = (cEntry.y -cLoc.y) / rKv + rLocH;
        wExit = (cExit.y -cLoc.y) / rKv + rLocH;
        cStart  = cEntry;
        cFinish = cExit;
      }
      else {
        wEntry = (cEntry.x -cLoc.x) / rKu + rLocH;
        wExit = (cExit.x -cLoc.x) / rKu + rLocH;
        cStart  = cEntry;
        cFinish = cExit;
      }
      wStart = wEntry;
    }
    else {     // lightray doesn't come inside clipcylinder
      return ;
    }

    if (!fDTMContainsB && wExit > rMaximumTerrainHeight) {
                      // ray shoots over toplevel of DTMcylinder
      return ;
    }
    if (!fDTMContainsB && wEntry < rMinimumTerrainHeight) {
                      // ray shoots under bottom of DTMcylinder
      return ;
    }
    if (mapDTM->rValue(cEntry) * rScaleH + rHeightScaleCorrection > wEntry) {  
                      // ray 'dives' under terrainsurface at DTM border
      ///crd.x = cView.x + 3;
      ///crd.y = cView.y + 3;
      return ;
    }
  }
  RowCol rcStart  = gr->rcConv(cStart);
  RowCol rcFinish = gr->rcConv(cFinish);

          ///// *******
          ///// *******  BRESENHAM ALGORITHM used to find the pixels perp below lightray
          //  connecting rcStart with rcFinish
          // see A.S.Glassner  'Graphic Gems'  vol. 1,  Academic Press Boston USA */

  ///double rW = wStart;   // height of point moving on lightray, initially at DTM box face
  /// Before starting Bresenham, in order to speed up the piercing point search
  /// let's "descale" the light ray once in stead of the terrain points 500 times:
  double rW = (wStart - rHeightScaleCorrection)/rScaleH; // "Descale" the lightray traject
  rWmove /= rScaleH;            // "Descale" also the movesteps along the lightray
  double rH, rDiff;   // height of underlying terrainpoint and height difference

  long dx = rcFinish.Col - rcStart.Col;
  long dy = rcFinish.Row - rcStart.Row;
  long ax = abs(dx)<<1;
  long ay = abs(dy)<<1;
  //iColmove iRowmove are +1 or -1 see previous definition
  long iMoveCount = 0;
  RowCol rcMovingPnt = rcStart; // start search from Proj center (begin of Bresenham path)
  if (!fRowMoveDominates ) {    // dominant move in u-direction ( x or col direction)
    long d = ay - (ax>>1);
    while (true) {
      iMoveCount++;
      if (iMoveCount > 1000) {
         ///crd.x = cView.x - 1;
         ///crd.y = cView.y - 1;
         return;
      }
      rH = mapDTM->rValue(rcMovingPnt);
      if (rH != rUNDEF) {
        ///rH = rH * rScaleH + rHeightScaleCorrection; //terrain scaling replaced by rW scaling
        rDiff = rW - rH;
        if (rDiff <= EPS10)
          break;
      }
      if (rcMovingPnt.Col == rcFinish.Col)
        break;
      rW += rWmove;
      if (d >= 0) {
        rcMovingPnt.Row += iRowMove;
        d -= ax;
      }
      rcMovingPnt.Col += iColMove;
      d += ay;
    } // end while iterating towards accurate ground coordinates
  }   // dominantly in col direction
  else {                    // dominant move in v-direction ( y or row direction)
    long d = ax -(ay>>1);
    while (true)  {
      iMoveCount ++;
      if (iMoveCount > 1000) {
         ///crd.x = cView.x - 2;
         ///crd.y = cView.y - 2;
         return;
      }
      rH = mapDTM->rValue(rcMovingPnt);
      if (rH != rUNDEF) {
        ///rH = rH * rScaleH + rHeightScaleCorrection;  //terrain scaling replaced by rW scaling
        rDiff = rW - rH;
        if (rDiff <= 0)
          break;
      }
      if (rcMovingPnt.Row == rcFinish.Row)
        break;
      rW += rWmove;
      if (d >= 0) {
        rcMovingPnt.Col += iColMove;
        d -= ay;
      }
      rcMovingPnt.Row += iRowMove;
      d += ax;
    } // end while iterating towards accurate ground coordinates
  }   // dominantly in row direction
	rW += rWmove;
	double rHSC = rHeightScaleCorrection;//(fLookingUpward) ? -rHeightScaleCorrection : rHeightScaleCorrection;
  if (rDiff == rUNDEF || (rW * rScaleH + rHSC > rMaximumTerrainHeight - 0.6) ||
					(mapDTM->rValue(rcMovingPnt)==rUNDEF) ) {
    ///crd.x = cView.x - 3;
    ///crd.y = cView.y - 3;
    return;
  }
  else {
    ///if (mapDTM->fInside(rcMovingPnt))
    if ((rcMovingPnt.Row < mapDTM->rcSize().Row - 1) && 
			(rcMovingPnt.Col < mapDTM->rcSize().Col - 1) &&
        rcMovingPnt.Row > 0 && rcMovingPnt.Col > 0)
      crd = gr->cConv(rcMovingPnt);  // terrain point found !!
    ///else {
    ///  crd.x = cView.x - 4;   // point found outside mapDTM
    ///  crd.y = cView.y - 4;
    ///}
  }
}

void GeoRef3D::GetObjectDependencies(Array<FileName>& afnObjDep)
{
  GeoRefPtr::GetObjectDependencies(afnObjDep);
  Map mapDTM;
  ReadElement("GeoRef3D", "DTM", mapDTM);
  if (mapDTM.fValid())
    afnObjDep &= mapDTM->fnObj;
}

void GeoRef3D::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  FileName fnDat(fnObj, ".gr#", true);
  if (!File::fExist(fnDat))
    return;
  ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
  if (asSection != 0) {
    (*asSection) &= "TableStore";
    (*asEntry) &= "Data";
  }
}

double GeoRef3D::rGetViewAngle() const
{
	return rAngle;
}

Coord GeoRef3D::crdFocalPoint() const
{
	return cView;
}

void GeoRef3D::SetFocalPoint(const Coord& crd)
{
	cView = crd;
	ComputeRot();	
}

Coord GeoRef3D::crdViewPoint() const
{
	return cLoc ;
}

void GeoRef3D::SetViewPoint(const Coord& crd)
{
	cLoc = crd;
	ComputeRot();	
}

void GeoRef3D::SetViewHeight(const double& rH)
{
	rLocH = rH;
	ComputeRot();	
}

void GeoRef3D::SetScaleHeight(const double& rS)
{
	rScaleH = rS;
}

void GeoRef3D::SetViewAngle(const double& rA)
{
	rAngle = rA;
}

void GeoRef3D::GetObjectStructure(ObjectStructure& os)
{
	GeoRefPtr::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "GeoRef3D", "DTM");		
	}		
	
}
