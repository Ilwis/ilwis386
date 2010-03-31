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
/*$Log: /ILWIS 3.0/CoordinateSystem/Oblimerc.cpp $
 * 
 * 3     13-09-99 12:38p Martin
 * //->/*
 * 
 * 2     9-09-99 10:23a Martin
 * added 2.22 stuf
//Revision 1.1  1998/09/16 17:26:42  Wim
//22beta2
//
/* ProjectionObliqueMercator */
#include "Engine\SpatialReference\Oblimerc.h"
#include "Headers\Hs\DAT.hs"

// members in oblimerc.h
/*	double	alpha, lamc, lam1, phi1, lam2, phi2, Gamma, al, bl, el,
		singam, cosgam, sinrot, cosrot, u_0;
	int		fEllips, fRot;   */  // rot similar to azim in genpersp ?
#define TOL	1.e-7
#define EPS10	1.e-10
#define TSFN0(x)	tan(.5 * (M_PI_2 - (x)))

ProjectionObliqueMercator::ProjectionObliqueMercator(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
  fEll = true;
  fUseParam[pvLAT0] = true;
  fUseParam[pvLON0] = true;
  fUseParam[pvK0] = true;
  fUseParam[pvAZIMCLINE] = true;  // user can define azim of central line
  fUseParam[pvNORIENTED] = true; // user can decide to 'north-oriented' csy
  Param(pvLAT0, 1.);      // set a non-zero default central latitude
}

void ProjectionObliqueMercator::Prepare()
{
  double con, com, rCosPhi0, d, f, rSinPhi0; 
  //int azi;

  fEllips = !ell.fSpherical();
  fRot	= fNoriented; //pj_param( params, "bno_rot").i == 0;
  //if (azi	= pj_param( params, "talpha").i)
  {        // if azim is given, from which tiepoints are found
    lamc = lam0; //pj_param( params, "rlonc").f;
    alpha = rAzimCLine; //pj_param( params, "ralpha").f;
    if (abs( phi0) <= TOL)
      throw  ErrorObject(SDATErrCentralLatEqualZero, 2211);
    if (abs(abs( phi0) - M_PI_2) <= TOL)
      throw  ErrorObject(SDATErrCentralLatEqual90, 2211);
    if (abs(abs( alpha) - M_PI_2) <= TOL)
      throw  ErrorObject(SDATErrAzimuthEqual90, 2211);
	///E_ERROR(-32);
  }     /*
  else
  { // if tiepoints phi1,lam1,phi2,lam2 given, and alpha computed
  	lam1	= pj_param( params, "rlon_1").f;
  	phi1	= pj_param( params, "rlat_1").f;
  	lam2	= pj_param( params, "rlon_2").f;
  	phi2	= pj_param( params, "rlat_2").f;
  	if (abs(phi1 - phi2) <= TOL ||
  		(con = abs(phi1)) <= TOL ||
  		abs(con - M_PI_2) <= TOL ||
  		abs(abs(phi0) - M_PI_2) <= TOL ||
  		abs(abs(phi2) - M_PI_2) <= TOL) E_ERROR(-33);
  }   */
  com = ( fEllips) ? sqrt(1 - ell.e2) : 1.;
  if (abs( phi0) > EPS10)
  {
  	rSinPhi0 = sin( phi0);
  	rCosPhi0 = cos( phi0);
  	if (fEllips)
  	{
  		con = 1. -  ell.e2 * rSinPhi0 * rSinPhi0;
  		bl = rCosPhi0 * rCosPhi0;
  		bl = sqrt(1. +  ell.e2 *  bl *  bl / (1 - ell.e2));
  		al =  bl *  k0 * com / con;
  		d =  bl * com / (rCosPhi0 * sqrt(con));
  	}
  	else
  	{
  		bl = 1.;
  		al =  k0;
  		d = 1. / rCosPhi0;
  	}
  	if ((f = d * d - 1.) <= 0.)
  		f = 0.;
  	else {
  		f = sqrt(f);
  		if ( phi0 < 0.)
  			f = -f;
  	}
  	 el = f += d;
  	if ( fEllips)
  		el *= pow(tsfn( phi0),  bl);
  	else
  		el *= TSFN0( phi0);
  }
  else
  {
  	bl = 1. / com;
  	 al =  k0;
  	 el = d = f = 1.;
  }
//if (azi)           // voorlopig enige keus
  {
  	Gamma = asin(sin( alpha) / d);
     // 21/4/98 Wim: changing lam0 is not allowed!
     //	lam0 =  lamc - asin((.5 * (f - 1. / f)) * tan( Gamma)) /  bl;
  }
/*else
  {
  	if ( fEllips)
  	{
  		h = pow(tsfn( phi1),  bl);
  		l = pow(tsfn( phi2),  bl);
  	}
  	else {
  		h = TSFN0( phi1);
  		l = TSFN0( phi2);
  	}
  	f =  el / h;
  	p = (l - h) / (l + h);
  	j =  el *  el;
  	j = (j - l * h) / (j + l * h);
  	if ((con =  lam1 -  lam2) < - M_PI)
  		 lam2 -= M_PI * 2;
  	else if (con > M_PI)
  		 lam2 += M_PI * 2;
  	 lam0 = adjlon(.5 * ( lam1 +  lam2) - atan(
  		 j * tan(.5 *  bl * ( lam1 -  lam2)) / p) /  bl);
  	 Gamma = atan(2. * sin( bl * adjlon( lam1 -  lam0)) /
  		 (f - 1. / f));
  	 alpha = asin(d * sin( Gamma));
  }  */
   singam = sin( Gamma);
   cosgam = cos( Gamma);
   //f = pj_param( params, "brot_conv").i ?  Gamma :  alpha; ///// unclear flag param b rot_conv
   f = alpha;
   sinrot = sin(f);
   cosrot = cos(f);
   //u_0 = pj_param( params, "bno_uoff").i ? 0. : /////  uncl flag nno_uoff  ( offset?
   //	abs( al * atan(sqrt(d * d - 1.) /  cosrot) /  bl);
    u_0 = 0.;
  if ( phi0 < 0.)      // center at south hemisphere
  	 u_0 = -  u_0;
} // end Prepare()

//Forward Proj Equations
XY  ProjectionObliqueMercator::xyConv(const PhiLam& pl) const
{
	XY xy;

	double  con, q, s, ul, us, vl, vs;
	vl = sin( bl * pl.Lam);
	if (abs(abs(pl.Phi) - M_PI_2) <= EPS10) {
		ul = pl.Phi < 0. ? - singam :  singam;
		us =  al * pl.Phi /  bl;
	} else {
		q =  el / ( fEllips ? pow(tsfn(pl.Phi),  bl)
			: TSFN0(pl.Phi));
		s = .5 * (q - 1. / q);
		ul = 2. * (s *  singam - vl *  cosgam) / (q + 1. / q);
		con = cos( bl * pl.Lam);
		if (abs(con) >= TOL) {
			us =  al * atan((s *  cosgam + vl *  singam) / con) /  bl;
			if (con < 0.)
				us += M_PI *  al /  bl;
		} else
			us =  al *  bl * pl.Lam;
	}
	if (abs(abs(ul) - 1.) <= EPS10) return xy; //F_ERROR;
	vs = .5 *  al * log((1. - ul) / (1. + ul)) /  bl;
	us -=  u_0;
	if (! fRot) { // if map csy parallel to central line
    xy.x = vs;  /// here USGS source GIE gives us
		xy.y = us;  ///   ,,   ,,    ,,  GIE  ,,   vs
	} else {      // if map csy 'north oriented'
		xy.x = vs *  cosrot + us *  sinrot;
		xy.y = us *  cosrot - vs *  sinrot;
	}
	return xy;
}

// INVERSE Proj Equations /* ellipsoid & spheroid */
PhiLam ProjectionObliqueMercator::plConv(const XY& xy) const
{
	PhiLam pl;
	double  q, s, ul, us, vl, vs;
	if (! fRot) 
	{
		vs = xy.x;    /// here USGS source GIE  gives us
		us = xy.y;    ///   ,,   ,,    ,,  GIE  ,,    vs
	} 
	else
	{
		vs = xy.x *  cosrot - xy.y *  sinrot;
		us = xy.y *  cosrot + xy.x *  sinrot;
	}
	us +=  u_0;
	q = exp(-  bl * vs /  al);
	s = .5 * (q - 1. / q);
	vl = sin( bl * us /  al);
	ul = 2. * (vl *  cosgam + s *  singam) / (q + 1. / q);
	if (abs(abs(ul) - 1.) < EPS10) 
	{
		pl.Lam = 0.;
		pl.Phi = ul < 0. ? -M_PI_2 : M_PI_2;
	} 
	else 
	{
		pl.Phi =  el / sqrt((1. + ul) / (1. - ul));
		if ( fEllips) 
		{
		  pl.Phi = phi2(pow(pl.Phi, 1. /  bl));
			if ((pl.Phi ) == HUGE_VAL)
				return pl; //I_ERROR;
		} 
		else
			pl.Phi = M_PI_2 - 2. * atan(pl.Phi);
		pl.Lam = - atan2((s *  cosgam -
			vl *  singam), cos( bl * us /  al)) /  bl;
	}
	return pl;
}




