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
/* ProjectionGoodeHomolosine
   Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE   23 Jun 2005    1:10 pm
*/
#include "Engine\SpatialReference\Goode.h"

//#define Y_COR		0.05280353  // Y-correction for mollweide if sinusoid is spherical
// this is (4866096.4-4529344.9)/6371007.1809185 ~ 0.0528035
// the mollw_y - sinus_y at join-parallel, reduced to unit-sphere

ProjectionGoode::ProjectionGoode(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
	fEll = true;
}

void ProjectionGoode::Prepare()
{
	prjSinusoidal = new ProjectionCylindSinusoidal(ell); 
	prjMollweide = new ProjectionMollweide(ell);
	prjMollweide->Prepare();
	//rY_COR_ELL = 0.054268077;// default Y-correction for mollweide if sinusoid is (WGS) ellipsoidal
	//0.054266 is mollw_y - sinus_y at join-parallel 40:44':11.8" (about 336.4 km southward shift).
	// for projections sinus and moll based on "unit' wgs ellipsoid 
	///rPhiJoin; //= 0.7109879899933945;// (previously assumed equal for sphere and ellipsoid
	///double rPhiJoinSph = 0.7109888816;// improved for sphere 40:44':11.9839"
	///double rPhiJoinWGS = 0.71172405545586;// improved for WGS84ell 40:46':43.6244"
	///// new join-corrections based on really authalic mollweide
	///// 40.9771019361 dwz  40:58':37.56697 = 0.71518534671028807292781262871392 rad
	///// 0.7649156468 - 0.7114977896 = 0.0534178572 (about 340.3 km)
	///// this is mollwWGS_y - sinusWGS_y at join-parallel 40:58':37.56697" on WGS84
	///double rPhiJoinWGSAuthalic = 0.70950973145175;
	///rPhiJoin = rLatitudeAndYcoordAtJoin();
	rPhiJoin = rLatitudeAtJoin();
	PhiLam plJoin;
	plJoin.Phi = rPhiJoin;
	plJoin.Lam = 0.0;
	XY xyJoin = prjMollweide->xyConv(plJoin);
	Ymollweide_at_Join = xyJoin.y;
	double rSinusYcoordAtJoin = prjSinusoidal->xyConv(plJoin).y;
	 Ycorr = Ymollweide_at_Join - rSinusYcoordAtJoin;//was 0.05280353 for sphere
		// auth sphere: Ycorr = 0.052803527368541 on unit-sph ~ 336 km on auth sph
	 //  wgs ellipsoid Ycorr = 0.053414706142711 on 'unit'-wgs  ~ 341 km 
	 //  diff in correction shift ('gap-diff') is ~ 4 km
}

XY ProjectionGoode::xyConv(const PhiLam& pl) const
{
  XY xy;
	PhiLam _pl = pl;
	if (abs(pl.Phi)> M_PI_2) return xy;
	if (abs(pl.Lam)> M_PI) return xy;
	//double xyY; //uncorrected Y for mollw
	//Ycorr = ell.fSpherical() ? Y_COR : rY_COR_ELL;
	bool fNorth = (pl.Phi > 0); 
	// PhiJoin = 40:44':11.8"  = 40.7366111111 = 0.71098798999339450624749364956177
		// according to D.R.Steinwand, "Mapping Raster Imagery to the Interrup Goode
		// Int Journ of Remote Sensing , 1994 Vol. 15, No. 17, 3463 -3471
  
	if (abs(pl.Phi) < rPhiJoin) { // apply SINUSoidal projection within +-40gr 44min 11.8sec
	  return prjSinusoidal->xyConv(_pl);
	}
	else {  // apply MOLLWEIDE beyond 40 44' 11.8" latitude
		xy = prjMollweide->xyConv(_pl);
		if (fNorth)
			xy.y -= Ycorr; 
		else	
			xy.y += Ycorr;
	}
  return xy;
}

PhiLam ProjectionGoode::plConv(const XY& xy) const
{
  PhiLam pl;
	XY xyShifted = xy; //shifted Y for mollw
	double xyY = xy.y; //modified Y for mollw
	if (xy.y >= 0.0) // hemisphere north
		xyShifted.y = xy.y  + Ycorr;
	else
		xyShifted.y = xy.y  - Ycorr;
	//double trylam;
	// PhiJoin = 40:44':11.8"  = 40.7366111111 = 0.71098798999339450624749364956177
		// according to D.R.Steinwand, "Mapping Raster Imagery to the Interrup Goode
		// Int Journ of Remote Sensing , 1994 Vol. 15, No. 17, 3463 -3471
	
	if (abs(xyShifted.y) < rPhiJoin) { // apply SINUSoidal within -40:44:11.8,40:44:11.8
	
		return prjSinusoidal->plConv(xy);
	}
	else { // apply Inverse MOLLW beyond 40 44' 11.8" latitude of tru scale 
		pl = prjMollweide->plConv(xyShifted);
  }
  return pl;
}

Datum* ProjectionGoode::datumDefault() const
{
  return new MolodenskyDatum("WGS 1984", "");
}

double ProjectionGoode::rLatitudeAtJoin()
{ 
	return prjMollweide->rLatitudeWithTrueScale();
}
