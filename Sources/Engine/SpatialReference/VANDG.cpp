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
/* van der Grinten projection
   Copyright Ilwis System Development ITC
   june 1996, by Jan Hendrikse
	Last change:  JHE  13 Jun 96    9:47 pm
*/
#include "Engine\SpatialReference\VANDG.H"

ProjectionVanderGrinten::ProjectionVanderGrinten()
{}
# define TOL		1.e-10
# define THIRD		.33333333333333333333
# define TWO_THRD	.66666666666666666666
# define C2_27		.07407407407407407407
# define PI4_3		4.18879020478639098458
# define PISQ		9.86960440108935861869
# define TPISQ		19.73920880217871723738
# define HPISQ		4.93480220054467930934

XY ProjectionVanderGrinten::xyConv(const PhiLam& _pl) const
{
  XY xy;
  PhiLam pl = _pl;
  double  al, al2, g, g2, p2;

  p2 = abs(pl.Phi / M_PI_2);
  if ((p2 - TOL) > 1.) return xy;
	if (p2 > 1.)
		p2 = 1.;
	if (abs(pl.Phi) <= TOL) {
		xy.x = pl.Lam;
		xy.y = 0.;
	} else if (abs(pl.Lam) <= TOL || abs(p2 - 1.) < TOL) {
		xy.x = 0.;
		xy.y = M_PI * tan(.5 * asin(p2));
		if (pl.Phi < 0.) xy.y = -xy.y;
	} else {
		al = .5 * abs(M_PI / pl.Lam - pl.Lam / M_PI);
		al2 = al * al;
		g = sqrt(1. - p2 * p2);
		g = g / (p2 + g - 1.);
		g2 = g * g;
		p2 = g * (2. / p2 - 1.);
		p2 = p2 * p2;
		xy.x = g - p2; g = p2 + al2;
		xy.x = M_PI * (al * xy.x + sqrt(al2 * xy.x * xy.x - g * (g2 - p2))) / g;
		if (pl.Lam < 0.) xy.x = -xy.x;
		xy.y = abs(xy.x / M_PI);
		xy.y = 1. - xy.y * (xy.y + 2. * al);
		if (xy.y < -TOL) return xy;
		if (xy.y < 0.)	xy.y = 0.;
		else		xy.y = sqrt(xy.y) * (pl.Phi < 0. ? -M_PI : M_PI);
	}
	return xy;
}

PhiLam ProjectionVanderGrinten::plConv(const XY& xy) const
{
  PhiLam pl;
	double t, c0, c1, c2, c3, al, r2, r, m, d, ay, x2, y2;

	x2 = xy.x * xy.x;
	if ((ay = abs(xy.y)) < TOL) {
		pl.Phi = 0.;
		t = x2 * x2 + TPISQ * (x2 + HPISQ);
		pl.Lam = abs(xy.x) <= TOL ? 0. :
		   .5 * (x2 - PISQ + sqrt(t)) / xy.x;
		return pl;
	}
	y2 = xy.y * xy.y;
	r = x2 + y2;	r2 = r * r;
	c1 = - M_PI * ay * (r + PISQ);
	c3 = r2 + 2 * M_PI * (ay * r + M_PI * (y2 + M_PI * (ay + M_PI_2)));
	c2 = c1 + PISQ * (r - 3. *  y2);
	c0 = M_PI * ay;
	c2 /= c3;
	al = c1 / c3 - THIRD * c2 * c2;
	m = 2. * sqrt(-THIRD * al);
	d = C2_27 * c2 * c2 * c2 + (c0 * c0 - THIRD * c2 * c1) / c3;
	if (((t = abs(d = 3. * d / (al * m))) - TOL) <= 1.) {
		d = t > 1. ? (d > 0. ? 0. : M_PI) : acos(d);
		pl.Phi = M_PI * (m * cos(d * THIRD + PI4_3) - THIRD * c2);
		if (xy.y < 0.) pl.Phi = -pl.Phi;
		t = r2 + TPISQ * (x2 - y2 + HPISQ);
		pl.Lam = abs(xy.x) <= TOL ? 0. :
		   .5 * (r - PISQ + (t <= 0. ? 0. : sqrt(t))) / xy.x;
	} else
		return pl;
	return pl;
}




