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
/* ProjectionSchreiber
   Copyright Ilwis System Development ITC
			october 1996, by Jan Hendrikse
	Last change:  WK   28 Nov 96    4:18 pm
/*
purpose        : double projection of Schreiber (stereographic projection)
*                  for computation of planar RD - coordinates from ellipsoidal
*                  coordinates on Bessel-ellipsoid via series expansion
*
*                  use rd_inv for the inverse projection
*
* specifications : formulas obtained from T.G. Schut - Transformatie van
*                  rechthoekige RD - coordinaten naar geografische coordinaten
*                  op de ellipsoide van Bessel. In : NGT - Geodesia, juni 1992.
* other sources:   G.L. Strang van Hees "Globale en Lokale Geodetische systemen"
*                  Nederl. Cie v. Geodesie  Delft 1994, 2e herziene druk
									 A.J.M. Kosters "Coordinaattransformaties en kaartprojecties"
									 Meetkunige Dienst - Rijkswaterstaat Delft 1993
*/

#include "Engine\SpatialReference\SCHREIB.H"
#include "Engine\Base\DataObjects\ERR.H"

ProjectionSchreiber::ProjectionSchreiber()
{                           /// sinusint heeft het ook niet!
  //fEll = true;
  fUseParam[pvX0] = false;
  fUseParam[pvY0] = false;
  fUseParam[pvLON0] = false;
  fUseParam[pvLAT0] = false;
  fUseParam[pvK0] = false;
  k0 = 1.0;  ///  scale at Amersfoort is already built in polynom transf 0.9999079;
  lam0 = 19395.500 * M_PI / 180.0 / 3600.0;
  phi0 = 187762.178 * M_PI / 180.0 / 3600.0;
  x0 = 155000.0;    // in meters
  y0 = 463000.0;    // in meters
}


#define EPS     1e-15
#define EPS10   1e-10
///#define  NUMSEC      1296000.0    /* aantal boogseconden in een hele omwenteling */


/* initialization of some coefficients zet in Param function in schreib.h*/

///	 double phi0 = 187762.178 ; 					/* seconds of arc */  omzetten
///   double lam0 =  19395.500 ;					 /* seconds of arc */ naar graden
                                           // dan kan rad_per_sec vervallen
///   double x0 = 155000.00 ;                      /* meters */
///   double y0 = 463000.00 ;                      /* meters */

	 ///double rad_per_sec = 2*PI / NUMSEC ;
XY ProjectionSchreiber::xyConv(const PhiLam& _pl) const
{
   const double c01 = 190066.98903 ;
   const double c11 = -11830.85831 ;
   const double c21 =   -114.19754 ;
   const double c03 =    -32.38360 ;
   const double c31 =     -2.34078 ;
   const double c13 =      -.60639 ;
   const double c23 =       .15774 ;
   const double c41 =      -.04158 ;
   const double c05 =      -.00661 ;

   const double d10 = 309020.31810 ;
   const double d02 =   3638.36193 ;
   const double d12 =   -157.95222 ;
   const double d20 =     72.97141 ;
   const double d30 =     59.79734 ;
   const double d22 =     -6.43481 ;
   const double d04 =       .09351 ;
   const double d32 =      -.07379 ;
   const double d14 =      -.05419 ;
   const double d40 =      -.03444 ;

   XY xy;
   PhiLam pl = _pl;
   double dphi, dphi2, dphi3, dphi4 ;
   double dlam, dlam2, dlam3, dlam4, dlam5 ;
   double   dx, dy ;
   if ( (pl.Phi<M_PI/4.0) || (pl.Phi>M_PI/3.0 )) return xy; //lat in [45N,60N]
//   if ( (pl.Lam< 0.0) || (pl.Lam > M_PI/12.0 )) return xy; 
   if (abs(pl.Lam) > M_PI/20.0) return xy;    //long in [0E,9E]
   
   pl.Phi *= (180.0 * 3600.0 / M_PI); // convert angles to arc_seconds
   pl.Lam *= (180.0 * 3600.0 / M_PI);
		///     phi    = phi / rad_per_sec ; 
   double rPhi0 = 187762.178 ;  // local latitude of origin in arcseconds for polynomial computations
   dphi  = ( pl.Phi - rPhi0 ) * 1.0e-4 ;
   dphi2 = dphi *dphi  ;
   dphi3 = dphi2*dphi ;
   dphi4 = dphi3*dphi ;

	 ///     lam   = lam / rad_per_sec ;
   dlam  = ( pl.Lam ) * 1.0e-4 ;
   dlam2 = dlam *dlam ;
   dlam3 = dlam2*dlam ;
   dlam4 = dlam3*dlam ;
   dlam5 = dlam4*dlam ;

   dx = c01*      dlam  + c11*dphi *dlam  + c21*dphi2*dlam  +
	c03*      dlam3 + c31*dphi3*dlam  + c13*dphi *dlam3 +
	c23*dphi2*dlam3 + c41*dphi4*dlam  + c05*      dlam5 ;

   dy = d10*dphi        + d20*dphi2       + d02*dlam2       +
	d12*dphi *dlam2 + d30*dphi3       + d22*dphi2*dlam2 +
	d40*dphi4       + d04*dlam4       + d32*dlam3*dlam2 +
	d14*dphi *dlam4                                     ;

        /// dx and dy are now in meters terrain scale
 ///	 xrd = x0 + dx ;
 ///	 yrd = y0 + dy ;

	 xy.x = ( dx) * k0 / 6371007.1809185;  // scaled and shifted for Amersfoort ///xrd;
	 xy.y = ( dy) * k0 / 6371007.1809185;  // and resized to unit sphere///yrd;
	 return xy;
}

 ///  double rad_per_sec = 2.0*PI / NUMSEC ;
 /// intialisaties voor in de h file   eerst omz in radialen of graden
///   double phi0 = 187762.178 ;           /* seconds of arc */
///   double lam0 =  19395.500 ;           /* seconds of arc */

///   double x0 = 155000.00 ;                      /* meters */
///   double y0 = 463000.00 ;                      /* meters */

PhiLam ProjectionSchreiber::plConv(const XY& _xy) const
{
   const double a01 = 3236.0331637 ;
   const double a20 =  -32.5915821 ;
   const double a02 =    -.2472814 ;
   const double a21 =    -.8501341 ;
   const double a03 =    -.0655238 ;
   const double a22 =    -.0171137 ;
   const double a40 =     .0052771 ;
   const double a23 =    -.0003859 ;
   const double a41 =     .0003314 ;
   const double a04 =     .0000371 ;
   const double a42 =     .0000143 ;
   const double a24 =    -.0000090 ;

   const double b10 = 5261.3028966 ;
   const double b11 =  105.9780241 ;
   const double b12 =    2.4576469 ;
   const double b30 =    -.8192156 ;
   const double b31 =    -.0560092 ;
   const double b13 =     .0560089 ;
   const double b32 =    -.0025614 ;
   const double b14 =     .0012770 ;
   const double b50 =     .0002574 ;
   const double b33 =    -.0000973 ;
   const double b51 =     .0000293 ;
   const double b15 =     .0000291 ;

   double dx, dx2, dx3, dx4, dx5 ;
   double dy, dy2, dy3, dy4, dy5 ;
   double dphi, dlam ;

  PhiLam pl;
  XY xy = _xy;
  xy.x *=  6371007.1809185;   // convert from unit sphere to terrain size (meters)
  xy.y *=  6371007.1809185;

  ///xy.x -= x0;       //   convert to meters from origin at Amerfoort (false origin)
  ///xy.y -= y0;

  if ( (xy.x < -300000.0) || (xy.x > 300000.0)) return pl; //Easting beyond limits [-300km,300km]
  if ( (xy.y < -500000.0) || (xy.y > 500000.0 )) return pl; //Northing beyond [-500km,500km]

  dx  = xy.x * 1.0e-5 / k0;/// ( xy.x - x0 ) * 1.0e-5 ;
  dx2 = dx *dx ;                              /// linear units resized to global size
  dx3 = dx2*dx ;                              /// and rescaled for Amersfoort
  dx4 = dx3*dx ;
  dx5 = dx4*dx ;

  dy  = xy.y * 1.0e-5 / k0;///( xy.y - y0 ) * 1.0e-5 ;
  dy2 = dy *dy ;
  dy3 = dy2*dy ;
  dy4 = dy3*dy ;
  dy5 = dy4*dy ;

  dphi = a01*    dy  + a20*dx2     + a02*dy2     + a21*dx2*dy  +
	  a03*    dy3 + a40*dx4     + a22*dx2*dy2 + a04*    dy4 +
	  a41*dx4*dy  + a23*dx2*dy3 + a42*dx4*dy2 + a24*dx2*dy4 ;

  dlam = b10*dx      + b11*dx *dy  + b30*dx3     + b12*dx *dy2 +
	  b31*dx3*dy  + b13*dx *dy3 + b50*dx5     + b32*dx3*dy2 +
	  b14*dx *dy4 + b51*dx5*dy  + b33*dx3*dy3 + b15*dx* dy5 ;

 double rPhi0 = 187762.178 * M_PI / 180.0 / 3600.0 ; 

	 pl.Phi = rPhi0 + ( dphi) * M_PI / (3600.0 * 180.0);  /// * rad_per_sec ;
	 pl.Lam =         ( dlam) * M_PI / (3600.0 * 180.0);  /// * rad_per_sec ;

 return pl;
}






