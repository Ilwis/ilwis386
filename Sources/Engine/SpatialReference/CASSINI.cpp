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
/* ProjectionCassini
	 Copyright Ilwis System Development ITC
	 october 1997, by Jan Hendrikse
	Last change:  JHE  10 Oct 97    4:46 pm
*/
#include "Engine\SpatialReference\CASSINI.H"

#define EPS10	1.e-10
// members of ProjectionCassini defined in cassini.h
/*	double m0;
	double n; \
	double t; \
	double a1; \
	double c; \
	double r; \
	double dd; \
	double d2; \
	double a2; \
	double tn; \
	double *en;   // en[5] ?
*/

ProjectionCassini::ProjectionCassini(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
	fEll = true;
	fUseParam[pvLATTS] = true;
	fUseParam[pvK0] = true;
	//Param(pvLAT0, 0);     // needed?
}

void ProjectionCassini::Prepare()
{
	if (ell.e2 > 0) {
		enfn(en);
		m0 = mlfn(phits, sin(phits), cos(phits), en);
	}
}

# define C1	.16666666666666666666
# define C2	.00833333333333333333
# define C3	.04166666666666666666
# define C4	.33333333333333333333
# define C5	.06666666666666666666

XY ProjectionCassini::xyConv(const PhiLam& _pl) const
{
	XY xy;
	PhiLam pl = _pl;
	double n,	t, tn, a1, a2, c;
	if (!ell.fSpherical())
	{ /* ellipsoid */
		xy.y = mlfn(pl.Phi, n = sin(pl.Phi), c = cos(pl.Phi), en);
		n = 1./sqrt(1. - ell.e2 * n * n);
		tn = tan(pl.Phi); t = tn * tn;
		a1 = pl.Lam * c;
		c *= ell.e2 * c / (1 - ell.e2);
		a2 = a1 * a1;
		xy.x = n * a1 * (1. - a2 * t *
			(C1 - (8. - t + 8. * c) * a2 * C2));
		xy.y -= m0 - n * tn * a2 *
			(.5 + (5. - t + 6. * c) * a2 * C3);
		xy.x *= k0;
		xy.y *= k0;
		return xy;
	}
	else { /* spheroid */
		xy.x = asin(cos(pl.Phi) * sin(pl.Lam));
		xy.y = atan2(tan(pl.Phi) , cos(pl.Lam)) - phits;
		xy.x *= k0;
		xy.y *= k0;
		return xy;
	}
}

PhiLam ProjectionCassini::plConv(const XY& _xy) const
{
	PhiLam pl;
	XY xy = _xy;
	xy.x /= k0;
	xy.y /= k0;
	double n, t, r, dd, d2, tn;
	if (!ell.fSpherical())
	{/* ellipsoid */
		double ph1 = invmlfn(m0 + xy.y, ell.e2, en);
		tn = tan(ph1); t = tn * tn;
		n = sin(ph1);
		r = 1. / (1. - ell.e2 * n * n);
		n = sqrt(r);
		r *= (1. - ell.e2) * n;
		dd = xy.x / n;
		d2 = dd * dd;
		pl.Phi = ph1 - (n * tn / r) * d2 *
			(.5 - (1. + 3. * t) * d2 * C3);
		pl.Lam = dd * (1. + t * d2 *
			(-C4 + (1. + 3. * t) * d2 * C5)) / cos(ph1);
		return pl;
	}
	else
	{ /* spheroid */
		pl.Phi = asin(sin(dd = xy.y + phits) * cos(xy.x));
		pl.Lam = atan2(tan(xy.x), cos(dd));
		return pl;
	}
}




