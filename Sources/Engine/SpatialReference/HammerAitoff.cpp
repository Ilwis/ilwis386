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
/* ProjectionHammer-Aitoff (modified ProjectionLambAzimEqualArea)
   Copyright Ilwis System Development ITC
    june 1999, by Jan Hendrikse
	Last change:  JHE   7 Jul 99    3:23 pm
*/
#include "Engine\SpatialReference\HammerAitoff.H"
/* members:	double	sinb1; \
	double	cosb1; \
	double	xmf; \
	double	ymf; \
	double	mmf; \
	double	qp; \
	double	dd; \
	double	rq; \
	double	*apa; \
	int	mode;
    */

#define EPS10	1.e-10
#define NITER	20
#define CONV	1.e-10
#define N_POLE	0
#define S_POLE	1
#define EQUIT	2
#define OBLIQ	3

ProjectionHammerAitoff::ProjectionHammerAitoff(const Ellipsoid& ell)
:ProjectionAzimuthal(ell)
{
  //fUseParam[pvK0] = false;
}

void ProjectionHammerAitoff::Prepare()
{
  double t;

 ///  rSinPhi0 = sinb1;
 ///  rCosPhi0  = cosb1;
  if (fabs((t = fabs( phi0)) - M_PI_2) < EPS10)
  	 mode =  phi0 < 0. ? S_POLE : N_POLE;
  else if (fabs(t) < EPS10)
  	 mode = EQUIT;
  else
  	 mode = OBLIQ;
  if ( ell.e2) { // ellipsoid
	    	double sinphi;
                double es = ell.e2;
	        qp = qsfn(M_PI_2);
	        mmf = .5 / (1. -  es);
	        authset(apa);
		switch ( mode) {
		case N_POLE:
		case S_POLE:
			 dd = 1.;
			break;
		case EQUIT:
			 dd = 1. / ( rq = sqrt(abs(.5 *  qp)));
			 xmf = 1.;
			 ymf = .5 *  qp;
			break;
		case OBLIQ:
			 rq = sqrt(abs(.5 *  qp));
			sinphi = sin( phi0);
			 sinb1 = qsfn(phi0) /  qp;
			 cosb1 = sqrt(abs(1. -  sinb1 *  sinb1));
			 dd = cos( phi0) / (sqrt(abs(1. -  es * sinphi * sinphi)) *
			    rq *  cosb1);
			 ymf = ( xmf =  rq) /  dd;
			 xmf *=  dd;
			break;
		}
  } 
  else { // sphere
    if ( mode == OBLIQ) {
  		rSinPhi0 = sin( phi0);
  		rCosPhi0 = cos( phi0);
    }
  }
}

XY ProjectionHammerAitoff::xyConv(const PhiLam& _pl) const
{
  XY xy;
  PhiLam pl = _pl;
//FORWARD(e_forward); /* ellipsoid */
  if ( !ell.fSpherical()) {
    double coslam2, sinlam2, q, sinb, cosb, b;
    coslam2 = cos(pl.Lam/2.0); // modifying LambEqaulArea into Hammer-Aitoff
    sinlam2 = sin(pl.Lam/2.0); // modifying LambEqaulArea into Hammer-Aitoff
    q = qsfn(pl.Phi);
    if ( mode == OBLIQ ||  mode == EQUIT) {
		sinb = q /  qp;
		cosb = sqrt(abs(1. - sinb * sinb));
    }
    switch ( mode) {
	case OBLIQ:
		b = 1. +  sinb1 * sinb +  cosb1 * cosb * coslam2;
		break;
	case EQUIT:
		b = 1. + cosb * coslam2;
		break;
	case N_POLE:
		b = M_PI_2 + pl.Phi;
		q =  qp - q;
		break;
	case S_POLE:
		b = pl.Phi - M_PI_2;
		q =  qp + q;
		break;
    }
    if (fabs(b) < EPS10) return xy;
    switch ( mode) {
	case OBLIQ:
		xy.y =  ymf * ( b = sqrt(abs(2. / b)) )
		   * ( cosb1 * sinb -  sinb1 * cosb * coslam2);
		goto eqcon;
		///break;
	case EQUIT:
		xy.y = (b = sqrt(abs(2. / (1. + cosb * coslam2)))) * sinb *  ymf;
eqcon:
		xy.x =  xmf * b * cosb * sinlam2;
		break;
	case N_POLE:
	case S_POLE:
		if (q >= 0.) {
			xy.x = (b = sqrt(abs(q))) * sinlam2;
			xy.y = coslam2 * ( mode == S_POLE ? b : -b);
		} else
			xy.x = xy.y = 0.;
    	break;
    }
		xy.x *= 2.0; // modifying LambEqaulArea into Hammer-Aitoff
		xy.x *= k0;	// multiply by scale-factor at CM
		xy.y *= k0;
    return xy;
  }
//FORWARD(s_forward); /* sphere */
  else {
    double  coslam2, sinlam2, cosphi, sinphi;

    sinphi = sin(pl.Phi);
    cosphi = cos(pl.Phi);
    coslam2 = cos(pl.Lam/2.0);// modifying LambEqaulArea into Hammer-Aitoff
		sinlam2 = sin(pl.Lam/2.0);// modifying LambEqaulArea into Hammer-Aitoff
    switch ( mode) {
      case EQUIT:
    	xy.y = 1. + cosphi * coslam2;
    	goto oblcon;
      case OBLIQ:
    	xy.y = 1. + rSinPhi0 * sinphi + rCosPhi0 * cosphi * coslam2;
oblcon:
    	if (xy.y <= EPS10) return xy;
    	xy.x = (xy.y = sqrt(abs(2. / xy.y))) * cosphi * sinlam2;
    	xy.y *=  mode == EQUIT ? sinphi :
    	   rCosPhi0 * sinphi - rSinPhi0 * cosphi * coslam2;
    	break;
      case N_POLE:
     	coslam2 = -coslam2;
      case S_POLE:
    	if (fabs(pl.Phi +  phi0) < EPS10) return xy;
    	xy.y = M_PI_4 - pl.Phi * .5;
    	xy.y = 2. * ( mode == S_POLE ? cos(xy.y) : sin(xy.y));
    	xy.x = xy.y * sinlam2;
    	xy.y *= coslam2;
    	break;
    }
		xy.x *= 2.0; // modifying LambEqaulArea into Hammer-Aitoff
		xy.x *= k0;  // multiply by scale-factor at CM
		xy.y *= k0;
    return xy;
  }
}

PhiLam ProjectionHammerAitoff::plConv(const XY& _xy) const
{
  PhiLam pl;
  XY xy = _xy;
	xy.x /= k0; // divide by scale-factor at CM
	xy.y /= k0;
	xy.x /= 2.0; // modifying LambEqaulArea into Hammer-Aitoff
//INVERSE(e_inverse);
  if ( !ell.fSpherical()) {    /* ellipsoid */
 	double cCe, sCe, q, rho, ab;

	switch ( mode) {
	  case EQUIT:
	  case OBLIQ:
		  if ((rho = sqrt((xy.x /= dd)*xy.x + (xy.y *=  dd)*xy.y)) < EPS10) {
			  pl.Lam = 0.;
			  pl.Phi =  phi0;
			  return (pl);
		  }
      if (abs(.5 * rho / rq) > 1 - EPS10) return pl;
      sCe = 2. * asin(.5 * rho /  rq);
		  cCe = cos(sCe);
		  sCe = sin(sCe);
		  xy.x *= (sCe);
		  if ( mode == OBLIQ) {
		    ab = cCe *  sinb1 + xy.y * sCe *  cosb1 / rho;
			  q =  qp * ab;
			  xy.y = rho *  cosb1 * cCe - xy.y *  sinb1 * sCe;
		  }
		  else {
		    ab = xy.y * sCe / rho;
			  q =  qp * ab;
			  xy.y = rho * cCe;
		  }
		  break;
	  case N_POLE:
		  xy.y = -xy.y;
	  case S_POLE:
		  q = xy.x * xy.x + xy.y * xy.y;
		  if (!q) {
			  pl.Lam= 0.;
			  pl.Phi =  phi0;
			  return (pl);
		  }

	   ///	q =  qp - q;

		  ab = 1. - q /  qp;
		  if ( mode == S_POLE)
			  ab = - ab;
		  break;
	  }
    if (abs(xy.x)+abs(xy.y) < EPS10) return pl;
	  pl.Lam = atan2(xy.x, xy.y) * 2.0; // modifying LambEqaulArea into Hammer-Aitoff
    if (abs(ab) > 1 - EPS10) return pl;
	  pl.Phi = authlat(asin(ab),  apa);
		return pl;
  }
  else {  //INVERSE(s_inverse);  sphere
    double  cosz, rh, sinz;
  
    rh = sqrt(xy.x*xy.x + xy.y*xy.y);
    if ((pl.Phi = rh * .5 ) > 1.) return pl;
    pl.Phi = 2. * asin(pl.Phi);
    if ( mode == OBLIQ ||  mode == EQUIT) {
      sinz = sin(pl.Phi);
    	cosz = cos(pl.Phi);
    }
    switch ( mode) {
      case EQUIT:
    	  pl.Phi = fabs(rh) <= EPS10 ? 0. : asin(xy.y * sinz / rh);
       	xy.x *= sinz;
    	  xy.y = cosz * rh;
      	break;
      case OBLIQ:
    	  pl.Phi = fabs(rh) <= EPS10 ?  phi0 :
    	   asin(cosz * rSinPhi0 + xy.y * sinz * rCosPhi0 / rh);
    	  xy.x *= sinz * rCosPhi0;
    	  xy.y = (cosz - sin(pl.Phi) * rSinPhi0) * rh;
        break;
      case N_POLE:
    	  xy.y = -xy.y;
    	  pl.Phi = M_PI_2 - pl.Phi;
    	  break;
      case S_POLE:
      	pl.Phi -= M_PI_2;
      	break;
    }
    pl.Lam= (xy.y == 0. && ( mode == EQUIT ||  mode == OBLIQ)) ?
         0. : atan2(xy.x, xy.y) * 2.0;// modifying LambEqaulArea into Hammer-Aitoff
    return pl;
  }
}





