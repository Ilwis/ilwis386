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
/* ProjectionAzimEquidistant
   Copyright Ilwis System Development ITC
    june 1996, by Jan Hendrikse
	Last change:  JHE  20 Jun 96    3:47 pm
*/
#include "Engine\SpatialReference\AEQD.H"

#define EPS10 1.e-10
#define TOL 1.e-14
#define ONE_TOL	 1.00000000000001
//#define TOL9	0.000000001
	
double aasin(double v) 
{
	double av;

	if ((av = abs(v)) >= 1.) {
		if (av > ONE_TOL)
			return rUNDEF;
		return (v < 0. ? - M_PI_2 : M_PI_2);
	}
	return asin(v);
}
	
double aacos(double v) 
{
	double av;

	if ((av = abs(v)) >= 1.) {
		if (av > ONE_TOL)
			return rUNDEF;
		return (v < 0. ? M_PI : 0.);
	}
	return acos(v);
}

double asqrt(double v) { return ((v <= 0) ? 0. : sqrt(v)); }

/* members of ProjectionAzimEquidistant
	double	rSinPhi0; \
	double	rCosPhi0; \
	double	*en; \
	double	M1; \
	double	N1; \
	double	Mp; \
	double	He; \
	double	G; \
	int	mode; */


#define N_POLE	0
#define S_POLE 1
#define EQUIT	2
#define OBLIQ	3

ProjectionAzimEquidistant::ProjectionAzimEquidistant(const Ellipsoid& ell)
:ProjectionAzimuthal(ell)
{
  fUseParam[pvK0] = true;
}

void ProjectionAzimEquidistant::Prepare()
{
  if (fabs(fabs( phi0) - M_PI_2) < EPS10) {
  	 mode =  phi0 < 0. ? S_POLE : N_POLE;
  	 rSinPhi0 =  phi0 < 0. ? -1. : 1.;
  	 rCosPhi0 = 0.;
  } else if (fabs( phi0) < EPS10) {
  	 mode = EQUIT;
  	 rSinPhi0 = 0.;
  	 rCosPhi0 = 1.;
  } else {
  	 mode = OBLIQ;
  	 rSinPhi0 = sin( phi0);
  	 rCosPhi0 = cos( phi0);
  }
  if(!ell.fSpherical())
	{
		enfn(en);
		double e = ell.e;
		switch (mode) {
			case N_POLE:
			case S_POLE:
				Mp = mlfn(M_PI_2, 0., 1., en);
				break;
			case EQUIT:
			case OBLIQ:
				N1 = 1. / sqrt(1. - e * e * rSinPhi0 * rSinPhi0);
				G = rSinPhi0 * (He = e / sqrt(1 - e * e));
				He *= rCosPhi0;
				break;
		}
	}
}

XY ProjectionAzimEquidistant::xyConv(const PhiLam& _pl) const
{
  XY xy;
  PhiLam pl = _pl;
  double  rCosLam, rCosPhi, rSinPhi, rSinLam;
  rSinPhi = sin(pl.Phi);
  rCosPhi = cos(pl.Phi);
	rSinLam = sin(pl.Lam);
  rCosLam = cos(pl.Lam);
	if (!ell.fSpherical()) { /* ellipsoid */
		double e = ell.e;
		double rho, s, H, H2, c, Az, t;
		switch (mode) {
			case N_POLE:
				rCosLam = - rCosLam;
				pl.Phi = -pl.Phi;
			case S_POLE:
				xy.x = (rho = Mp + mlfn(pl.Phi, rSinPhi, rCosPhi, en)) *
					rSinLam;
				xy.y = rho * rCosLam;
				break;
			case EQUIT:
			case OBLIQ:
				if (abs((t = abs(pl.Phi)) - M_PI_2) < TOL) {
					s = abs(aasin(rCosPhi0));
					if (pl.Phi < 0.)
						Az = M_PI;
					else {
						Az = 0;
						s = -s;
					}
				} 
				else if ( t < TOL && abs(pl.Lam) < EPS10) {
					xy.x = xy.y = 0.;
					break;
				} 
				else {
					t = (1 - e * e) * tan(pl.Phi) + e * e * N1 * rSinPhi0 *
						sqrt(1. - e * e * rSinPhi * rSinPhi) / rCosPhi;
					Az = atan2(rSinLam, rCosPhi0 * t - rSinPhi0 * rCosLam);
					t = atan(t);
					s = sin(Az);
					if (abs(s) < TOL)
						s = aasin(rCosPhi0 * sin(t) - rSinPhi0 * cos(t));
					else {
						s = abs(aasin(rSinLam * cos(t) / s));
						if (Az < 0.)
							s = -s;
					}
				}
				H = He * cos(Az);
				H2 = H * H;
				c = N1 * s * (1. + s * s * (- H2 * (1. - H2)/6. +
					s * ( G * H * (1. - H2 - H2) / 8. +
					s * ((H2 * (4. - 7. * H2) - 3. * G * G * (1. - 7. * H2)) /
					120. - s * G * H / 48.))));
				c = abs(c);
				xy.x = c * sin(Az);
				xy.y = c * cos(Az);
				break;
		}
	}
	else { /* sphere */
		switch ( mode) {
			case EQUIT:
				xy.y = rCosPhi * rCosLam;
				goto oblcon;
			case OBLIQ:
				xy.y =  rSinPhi0 * rSinPhi +  rCosPhi0 * rCosPhi * rCosLam;
	oblcon:
				if (fabs(fabs(xy.y) - 1.) < TOL)
      		if (xy.y < 0.)
      			return xy;
      		else
      			xy.x = xy.y = 0.;
				else {
      		xy.y = acos(xy.y);
      		xy.y /= sin(xy.y);
      		xy.x = xy.y * rCosPhi * rSinLam;
      		xy.y *= ( mode == EQUIT) ? rSinPhi :
    			 rCosPhi0 * rSinPhi -  rSinPhi0 * rCosPhi * rCosLam;
				}
				break;
			case N_POLE:
				pl.Phi = -pl.Phi;
				rCosLam = -rCosLam;
			case S_POLE:
				if (fabs(pl.Phi - M_PI_2) < EPS10) return xy;
				xy.x = (xy.y = (M_PI_2 + pl.Phi)) * rSinLam;
				xy.y *= rCosLam;
				break;
		}
	}
	xy.x *= k0;
	xy.y *= k0;
  return (xy);
}

PhiLam ProjectionAzimEquidistant::plConv(const XY& _xy) const
{
  PhiLam pl;
  XY xy = _xy;
	xy.x /= k0;
	xy.y /= k0;
	if (!ell.fSpherical()) { /* ellipsoid */
		double e = ell.e;
		double c, Az, cosAz, A, B, D, E, F, psi, t;
		if ((c = rHypot(xy.x, xy.y)) > M_PI) {
			if (c - EPS10 > M_PI) return pl;
			c = M_PI;
		} 
		else if (c < EPS10) {
			pl.Phi = phi0;
			pl.Lam = 0.;
			return (pl);
		}
		if (mode == OBLIQ || mode == EQUIT) {
			cosAz = cos(Az = atan2(xy.x, xy.y));
			t = rCosPhi0 * cosAz;
			B = e * e * t / (1 - e * e);
			A = - B * t;
			B *= 3. * (1. - A) * rSinPhi0;
			D = c / N1;
			E = D * (1. - D * D * (A * (1. + A) / 6. + B * (1. + 3.*A) * D / 24.));
			F = 1. - E * E * (A / 2. + B * E / 6.);
			psi = aasin(rSinPhi0 * cos(E) + t * sin(E));
			pl.Lam = aasin(sin(Az) * sin(E) / cos(psi));
			if ((t = abs(psi)) < EPS10)
				pl.Phi = 0.;
			else if (abs(t - M_PI_2) < 0.)
				pl.Phi = M_PI_2;
			else
				pl.Phi = atan((1. - e * e * F * rSinPhi0 / sin(psi)) * tan(psi) /
					(1 - e * e));
		} 
		else if (mode == N_POLE) {
			pl.Phi = invmlfn(Mp - c, e * e, en);
			pl.Lam = atan2(xy.x, - xy.y);
		} 
		else {
			pl.Phi = invmlfn(c - Mp, e * e, en);
			pl.Lam = atan2(xy.x, xy.y);
		}
		return (pl);
	}

  else { /* spherical */
		double cosc, c_rh, sinc;

		if ((c_rh = sqrt(xy.x*xy.x + xy.y*xy.y)) > M_PI) {
  		if (c_rh - EPS10 > M_PI) return pl;
  		c_rh = M_PI;
		} else if (c_rh < EPS10) {
  		pl.Phi =  phi0;
  		pl.Lam= 0.;
  		return (pl);
		}
		if ( mode == OBLIQ ||  mode == EQUIT) {
  		sinc = sin(c_rh);
  		cosc = cos(c_rh);
  		if ( mode == EQUIT) {
						 if (abs(xy.y * sinc / c_rh)  >= 1 ) return pl;
        		pl.Phi = asin(xy.y * sinc / c_rh);
  			xy.x *= sinc;
  			xy.y = cosc * c_rh;
  		} 
					else {
						if (abs(cosc *  rSinPhi0 + xy.y * sinc *  rCosPhi0 /
  							 c_rh)  >= 1) return pl;
				 pl.Phi = asin(cosc *  rSinPhi0 + xy.y * sinc *  rCosPhi0 /
				c_rh);
				xy.y = (cosc -  rSinPhi0 * sin(pl.Phi)) * c_rh;
				xy.x *= sinc *  rCosPhi0;
			}
			pl.Lam= xy.y == 0. ? 0. : atan2(xy.x, xy.y);
		} else if ( mode == N_POLE) {
		pl.Phi = M_PI_2 - c_rh;
		pl.Lam= atan2(xy.x, -xy.y);
		} else {
		pl.Phi = c_rh - M_PI_2;
		pl.Lam= atan2(xy.x, xy.y);
		}
	}
  return (pl);
}





