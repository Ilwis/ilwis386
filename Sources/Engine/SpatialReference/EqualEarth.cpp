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
/* ProjectionEqualEarth
  Copyright Ilwis System Development ITC
   august 2025, by Bas Retsios
	Last change:  BR  31 Aug 25    9:56 am
*/

#include "Engine\SpatialReference\EqualEarth.h"
#include "Engine\Base\DataObjects\ERR.H"

#define A1 1.340264
#define A2 -0.081106
#define A3 0.000893
#define A4 0.003796
#define iterations 12
#define EPS	1e-11

ProjectionEqualEarth::ProjectionEqualEarth(const Ellipsoid& ell)
: ProjectionPtr(ell)
, M(sqrt(3.0) / 2.0)
, qp(qsfn(M_PI_2))
, rqda(sqrt(0.5 * qp))
{
	fEll = true;
	if (!ell.fSpherical())
		authset(apa);
}

XY ProjectionEqualEarth::xyConv(const PhiLam& pl) const
{
	XY xy;

	double sinPhi;

	if (ell.fSpherical())
		sinPhi = sin(pl.Phi);
	else
		sinPhi = qsfn(pl.Phi) / qp;

	double l = asin(M * sinPhi);  // theta
	double l2 = l * l;
	double l6 = l2 * l2 * l2;

	xy.x = pl.Lam * cos(l) / (M * (A1 + 3.0 * A2 * l2 + l6 * (7.0 * A3 + 9.0 * A4 * l2)));
	xy.y = l * (A1 + A2 * l2 + l6 * (A3 + A4 * l2));

	if (!ell.fSpherical()) {
		/* Adjust x and y for authalic radius */
		xy.x *= rqda;
		xy.y *= rqda;
	}

	return xy;
}

PhiLam ProjectionEqualEarth::plConv(const XY& _xy) const
{
	PhiLam pl;

	XY xy = _xy;

	// Is this the same as xy.x /= rqda?
	
	//xy.x = xy.x / k0;
	//xy.y = xy.y / k0;


	if (!ell.fSpherical()) {
		/* Adjust x and y for authalic radius */
		xy.x /= rqda;
		xy.y /= rqda;
	}

	double l = xy.y;
	double l2 = l * l;
	double l6 = l2 * l2 * l2;
	for (int i = 0; i < iterations; ++i) {
		double fy = l * (A1 + A2 * l2 + l6 * (A3 + A4 * l2)) - xy.y;
		double fpy = A1 + 3.0 * A2 * l2 + l6 * (7.0 * A3 + 9.0 * A4 * l2);
		double delta = fy / fpy;
		l -= delta;
		l2 = l * l;
		l6 = l2 * l2 * l2;
		if (abs(delta) < EPS)
			break;
	}

	pl.Lam = M * xy.x * (A1 + 3.0 * A2 * l2 + l6 * (7.0 * A3 + 9.0 * A4 * l2)) / cos(l);
	pl.Phi = asin(sin(l) / M);

	/* Ellipsoidal case, convert authalic latitude */
    //if (!ell.fSpherical())
	//	pl.Phi = pj_authalic_lat_inverse(pl.Phi, Q->apa, P, Q->qp);

    //if (abs(ab) > 1 - EPS10) return pl;
	if (!ell.fSpherical())
		pl.Phi = authlat(pl.Phi, apa);

	return pl;
}





