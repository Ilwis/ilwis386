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
/* ProjectionCylindSinusoidal
   Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE   3 Jun 2005    1:10 pm
*/
// Source John P. Snyder
// "Map Projections, A Working Manual"
// USGS Prof Paper 1395, Washington 1987
#include "Engine\SpatialReference\SINUS.H"

#define EPS     1e-15
#define EPS10   1e-10
#define NITER   10

ProjectionCylindSinusoidal::ProjectionCylindSinusoidal(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
	fEll = true;
	fTrulyEllipsoidal = !ell.fSpherical(); // eccentricity not 0;
}

XY ProjectionCylindSinusoidal::xyConv(const PhiLam& pl) const
{
  XY xy;
	PhiLam _pl = pl;
  if (abs(pl.Phi)> M_PI_2) return xy;
  if (abs(pl.Lam)> M_PI) return xy;
  double rDenom, rM; // see JPSnyder Chapter 30
  double e2 = ell.e2;
  double e4 = e2*e2;
  double e6 = e4*e2;
  double f =pl.Phi;
  double s2f = sin(2*f);
  double s4f = sin(4*f);
  double s6f = sin(6*f);
  if (fTrulyEllipsoidal) {			/* ellipsoid alorithm */
		rDenom = sqrt(1-ell.e2*sin(pl.Phi)*sin(pl.Phi));
		rM = (1-e2/4-3*e4/64-5*e6/256)*f - (3*e2/8+3*e4/32+45*e6/1024)*s2f
			+ (15*e4/256+45*e6/1024)*s4f - (35*e6/3072)*s6f; //eq (3-21)
		xy.y = rM;
  }
  else {										/* sphere */
		rDenom = 1.0;
		xy.y = pl.Phi;
  }
  xy.x = pl.Lam * cos(_pl.Phi) / rDenom;
	//// Snyder scalefactor formulas: h and k
//	double rScaleAlongMerid = sqrt(1 + pl.Lam*pl.Lam * sin(pl.Phi)*sin(pl.Phi));//h
  
	// verify x-scale and find mollweide circle intersection
  double y90 = ell.b* acos(1/ell.a);
  double a2 = ell.a*ell.a;
  double e = ell.e;
  double b =ell.b;
  double pi2 = M_PI*M_PI;
  double p2; 
  if (!ell.fSpherical()) 
	  p2 = a2 + a2*(1-e2)/2/e*log((1+e)/(1-e));
  else
	  p2 = a2*2;
  double yn = b*M_PI/4; // first guess in Newton approx
  double dy;
  double Ny = yn*yn + a2*pi2/4*cos(yn/b)*cos(yn/b) - p2;
  double dNdy = 2*yn - a2*pi2/4/b*sin(yn*2/b);
  for (int i = NITER; i ; --i) {
	Ny = yn*yn + a2*pi2/4*cos(yn/b)*cos(yn/b) - p2;
	dNdy = 2*yn - a2*pi2/4/b*sin(yn*2/b);
	dy = - Ny/dNdy;
	yn += dy;
 	if (abs(dy) < EPS)
 			break;
  }
  double ynDegrees = yn * 180 / M_PI/b;
  double xnSinus = ell.a*M_PI/2*cos(yn/b);
  double xnMollw = sqrt(p2 - yn*yn);
  return xy;
}

PhiLam ProjectionCylindSinusoidal::plConv(const XY& xy) const
{
  PhiLam pl;
  double cosfi, trylam;
	double e2 = ell.e2;
	double e4 = e2*e2;
	double e6 = e2*e4;
	double rMu, fi;
	double one_e2 = 1 - ell.e2;
	double d = (1 - sqrt(one_e2))/(1 + sqrt(one_e2));
	double d2 = d*d;
	double d3 = d2*d;
	double d4 = d2*d2;
	double s2m, s4m, s6m, s8m; 
	if (fTrulyEllipsoidal) {		/* ellipsoid algorithm */
		rMu = xy.y/(1-e2/4-3*e4/64-5*e6/256);
		s2m = sin(2*rMu); s4m = sin(4*rMu);
		s6m = sin(6*rMu); s8m = sin(8*rMu);
		fi = rMu + (3*d/2-27*d3/32)*s2m+(21*d2/16-55*d4/32)*s4m
				+(151*d3/96)*s6m + (1097*d4/512)*s8m;
		cosfi = cos(fi);
		if (abs(cosfi) < EPS10) return pl;
		trylam =  xy.x /cosfi;
		if (abs(trylam)> M_PI) return pl;
		pl.Phi = fi;
		pl.Lam = xy.x * sqrt(1 - ell.e2 * sin(fi)*sin(fi)) / cos(fi);
	}
	else {								 /* sphere */
		pl.Phi = xy.y;
		cosfi = cos(xy.y);
		if (abs(cosfi) < EPS10) return pl;
		trylam =  xy.x /cosfi;
		if (abs(trylam)> M_PI) return pl;
		pl.Lam = xy.x / cosfi;
	}
	
  return pl;
}

Datum* ProjectionCylindSinusoidal::datumDefault() const
{
  return new MolodenskyDatum("WGS 1984", "");
}






