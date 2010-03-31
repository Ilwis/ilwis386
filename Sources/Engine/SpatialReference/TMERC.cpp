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
/*$Log: /ILWIS 3.0/CoordinateSystem/TMERC.cpp $
 * 
 * 3     19-12-00 19:50 Hendrikse
 * correction in header comment
 * 
 * 2     19-12-00 9:02 Hendrikse
 * Solved restriction on 1-h*h (for tm spherical at equator)  using
 * sqrt(abs(1-h*h)) in all cases  (in plConv( XY& xy) )
//Revision 1.3  1998/09/16 17:26:42  Wim
//22beta2
//
//Revision 1.2  1997/09/12 16:21:25  janh
//Restrict Forward projection to fronthemisphere only
//Lam -Lam0 cannot exceed +-90 to avoid wrong gratic lines
//
/* ProjectionTransverseMercator
   Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE  12 Sep 97    5:21 pm
*/

#include "Engine\SpatialReference\TMERC.H"
#include "Engine\Base\DataObjects\ERR.H"

ProjectionTransverseMercator::ProjectionTransverseMercator(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
  fEll = true;
  fUseParam[pvLAT0] = true;
  fUseParam[pvK0] = true;
  Param(pvLAT0, 0);
}

#define EPS10	1.e-10
#define EPS14	1.e-14
#define FC1 1.
#define FC2 .5
#define FC3 .16666666666666666666
#define FC4 .08333333333333333333
#define FC5 .05
#define FC6 .03333333333333333333
#define FC7 .02380952380952380952
#define FC8 .01785714285714285714

XY ProjectionTransverseMercator::xyConv(const PhiLam& pl) const
{
  XY xy;
  if (ell.fSpherical()) {
    double cosPhi = cos(pl.Phi);
    double cosLam = cos(pl.Lam);
    if ( abs(pl.Lam) > M_PI_2 - EPS10 ) return xy; // restrict forwardproj to fronthemisphere
    ///if ( (abs(pl.Lam) > M_PI_2 - EPS10) && (abs(pl.Phi) < EPS10) ) return xy;
    double b = cosPhi * sin(pl.Lam);
    if (abs(abs(b) - 1) <= EPS10) return xy;
    xy.x = 0.5 * k0 * log((1 + b) / (1 - b));
    xy.y = cosPhi * cosLam / sqrt(1 - b * b);
    b = abs(xy.y);
    if (b >= 1) {
      if (b - 1 > EPS10)
        return xy;
      else 
        xy.y = 0.;
    } 
    else
      xy.y = acos(xy.y);
    if (pl.Phi < 0.) xy.y = -xy.y;
    xy.y = k0 * (xy.y - phi0);
  }
  else {
    double al, als, n;
    double sinPhi = sin(pl.Phi); 
    double cosPhi = cos(pl.Phi);
    if ( abs(pl.Lam) > M_PI_2 - EPS10 ) return xy;
    ///if ( (abs(pl.Lam) > M_PI_2 - EPS10) && (abs(pl.Phi) < EPS10) ) return xy;
    double b = cosPhi * sin(pl.Lam);
    if (abs(abs(b) - 1) <= EPS10) return xy;
    if ( abs(cosPhi) < EPS10 ) return xy;
    double t = sinPhi/cosPhi ;
    t *= t;
    al = cosPhi * pl.Lam;
    als = al * al;
    al /= sqrt(1. - ell.e2 * sinPhi * sinPhi);
    n = esp * cosPhi * cosPhi;
    xy.x = k0 * al * (FC1 +
    	FC3 * als * (1. - t + n +
    	FC5 * als * (5. + t * (t - 18.) + n * (14. - 58. * t)
    	+ FC7 * als * (61. + t * ( t * (179. - t) - 479. ) )
    	)));
    xy.y = k0 * (mlfn(pl.Phi, sinPhi, cosPhi, en) - ml0 +
    	sinPhi * al * pl.Lam * FC2 * ( 1. +
    	FC4 * als * (5. - t + n * (9. + 4. * n) +
    	FC6 * als * (61. + t * (t - 58.) + n * (270. - 330 * t)
    	+ FC8 * als * (1385. + t * ( t * (543. - t) - 3111.) )
    	))));
  }  
  return xy;
}

PhiLam ProjectionTransverseMercator::plConv(const XY& xy) const
{
  PhiLam pl;
  if (ell.fSpherical()) {
    double h = exp(xy.x / k0);
    double g = .5 * (h - 1. / h);  // = sinh(x) , sinus hyperbol
    if ( abs( phi0 + xy.y) > M_PI ) return pl;
    h = cos(phi0 + xy.y / k0);
    //if ( 1-h*h < EPS14 ) return pl;// replaced by abs security in next line:
    pl.Phi = asin(sqrt(abs(1. - h * h) / (1. + g * g)));
    if (xy.y < 0.) pl.Phi = -pl.Phi;
    pl.Lam = (g || h) ? atan2(g, h) : 0.;
  }  
  else {
    double n, con, cosPhi, d, ds, sinPhi, t;

    pl.Phi = invmlfn(ml0 + xy.y / k0, ell.e2, en);
    if (abs(pl.Phi) >= M_PI_2) {   // outside map
      return pl;
      //pl.Phi = xy.y < 0. ? -M_PI_2 : M_PI_2;
      //pl.Lam = 0.;
    } else {
      sinPhi = sin(pl.Phi);
      cosPhi = cos(pl.Phi);
      t = abs(cosPhi) > 1e-10 ? sinPhi/cosPhi : 0.;
      n = esp * cosPhi * cosPhi;
      con = 1. - ell.e2 * sinPhi * sinPhi;
      d = xy.x * sqrt(con) / k0;
      con *= t;
      t *= t;
      ds = d * d;
      pl.Phi -= (con * ds / (1. - ell.e2)) * FC2 * (1. -
        	 ds * FC4 * (5. + t * (3. - 9. *  n) + n * (1. - 4 * n) -
      	         ds * FC6 * (61. + t * (90. - 252. * n +
      		 45. * t) + 46. * n
                 - ds * FC8 * (1385. + t * (3633. + t * (4095. + 1574. * t)) )
      	        )));
      pl.Lam = d*(FC1 -
               ds*FC3*( 1. + 2.*t + n -
               ds*FC5*(5. + t*(28. + 24.*t + 8.*n) + 6.*n
               - ds * FC7 * (61. + t * (662. + t * (1320. + 720. * t)) )
               ))) / cosPhi;
    }
  }
  return pl;
}

void ProjectionTransverseMercator::Param(ProjectionParamValue pv, double rValue)
{
  ProjectionPtr::Param(pv, rValue);
  if (pvLAT0 == pv) {
    if ( abs(phi0)>M_PI_2 )
      InvalidLat();
    if (!ell.fSpherical()) {
      enfn(en);
      ml0 = mlfn(phi0, sin(phi0), cos(phi0), en);
      esp = ell.e2 / (1. - ell.e2);
    }  
  }    
}            





