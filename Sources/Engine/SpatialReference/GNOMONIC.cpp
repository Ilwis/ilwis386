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
/* ProjectionGnomonic
   Copyright Ilwis System Development ITC
  may 1996, by Jan Hendrikse
	Last change:  JHE  25 Jun 96    6:25 pm
*/
#include "Engine\SpatialReference\GNOMONIC.H"

#define EPS10 1.e-10
#define N_POLE	0
#define S_POLE 1
#define EQUIT	2
#define OBLIQ	3

ProjectionGnomonic::ProjectionGnomonic()
:ProjectionAzimuthal()
{
  fUseParam[pvK0] = false;
}

const double TOLPL = cos(M_PI_2 - .1);
const double TOLXY = tan(M_PI_2 - .1);

void ProjectionGnomonic::Prepare()
{
 //  first computing the constants related to the 
 // geometry of the given azimuth plane and ellipsoid

    if (fabs(fabs( phi0) - M_PI_2) < EPS10)
		 mode =  phi0 < 0. ? S_POLE : N_POLE;
	else if (fabs( phi0) < EPS10)
		 mode = EQUIT;
	else {
		 mode = OBLIQ;
		 rSinPhi0 = sin( phi0);
		 rCosPhi0 = cos( phi0);
	}
}


XY ProjectionGnomonic::xyConv(const PhiLam& _pl) const
{
  XY xy;
  PhiLam pl = _pl;
  double  coslam, cosphi, sinphi;

	sinphi = sin(pl.Phi);
	cosphi = cos(pl.Phi);
	coslam = cos(pl.Lam);
	switch ( mode) {
	case EQUIT:
		xy.y = cosphi * coslam;
		break;
	case OBLIQ:
		xy.y =  rSinPhi0 * sinphi +  rCosPhi0 * cosphi * coslam;
		break;
	case S_POLE:
		xy.y = - sinphi;
		break;
	case N_POLE:
		xy.y = sinphi;
		break;
	}
        // xy.y is the cos of the angular dist between Phi0,Lam0 and pl
	if (xy.y < TOLPL) return xy; // pl to far apart from center
	xy.x = (xy.y = 1. / xy.y) * cosphi * sin(pl.Lam);
	switch ( mode) {
	case EQUIT:
		xy.y *= sinphi;
		break;
	case OBLIQ:
		xy.y *=  rCosPhi0 * sinphi -  rSinPhi0 * cosphi * coslam;
		break;
	case N_POLE:
		coslam = - coslam;
	case S_POLE:
		xy.y *= cosphi * coslam;
		break;
	}
	return xy;
}

PhiLam ProjectionGnomonic::plConv(const XY& _xy) const
{
  XY xy;  // local ( non-constant) xy
  xy = _xy;
  PhiLam pl;
  	double  rh, cosz, sinz;

	rh = sqrt(xy.x*xy.x + xy.y*xy.y);
        if ( rh >  TOLXY ) return pl;
	sinz = sin(pl.Phi = atan(rh));
	cosz = sqrt(1. - sinz * sinz);
	if (fabs(rh) <= EPS10) {
		pl.Phi =  phi0;
		pl.Lam= 0.;
	} else {
		switch ( mode) {
		case OBLIQ:
			pl.Phi = cosz *  rSinPhi0 + xy.y * sinz *  rCosPhi0 / rh;
			if (fabs(pl.Phi) >= 1.)
				pl.Phi = pl.Phi > 0. ? M_PI_2 : - M_PI_2;
			else
				pl.Phi = asin(pl.Phi);
			xy.y = (cosz -  rSinPhi0 * sin(pl.Phi)) * rh;
			xy.x *= sinz *  rCosPhi0;
			break;
		case EQUIT:
			pl.Phi = xy.y * sinz / rh;
			if (fabs(pl.Phi) >= 1.)
				pl.Phi = pl.Phi > 0. ? M_PI_2 : - M_PI_2;
			else
				pl.Phi = asin(pl.Phi);
			xy.y = cosz * rh;
			xy.x *= sinz;
			break;
		case S_POLE:
			pl.Phi -= M_PI_2;
			break;
		case N_POLE:
			pl.Phi = M_PI_2 - pl.Phi;
			xy.y = -xy.y;
			break;
		}
		pl.Lam= atan2(xy.x, xy.y);
	}
	return pl;
}





