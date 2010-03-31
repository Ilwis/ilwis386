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
/* ProjectionStereoGraphic also Non-Polar
   Copyright Ilwis System Development ITC
  may 1996, by Jan Hendrikse
	Last change:  JHE   5 Jul 97    1:37 pm
*/
#include "Engine\SpatialReference\STEREO.H"
#include "Engine\Base\DataObjects\ERR.H"

#define EPS10	1.e-10
#define TOL     1.e-8
#define NITER   8
#define CONV    1.e-10
#define N_POLE  0
#define S_POLE  1
#define EQUIT   2
#define OBLIQ   3

/* protected members declared in stereo.h:
  double sinX1;
  double cosX1;
  double akm1;   */
static double
  ssfn_(double phit, double sinphi, double eccen) {
	sinphi *= eccen;
	return (tan (.5 * (M_PI_2 + phit)) *
	   pow((1. - sinphi) / (1. + sinphi), .5 * eccen));
  }


ProjectionStereoGraphic::ProjectionStereoGraphic(const Ellipsoid& ell)
:ProjectionAzimuthal(ell)
{
}

void ProjectionStereoGraphic::Prepare()
{
 //  first computing the constants related to the 
 // geometry of the given azimuth plane and ellipsoid
  double t;
  double e = ell.e;
  //double phits = phi0;  //voorlopig geen userParam lat of true scale (latts)
  if ( abs(phi0)>M_PI_2 )
    InvalidLat();
  if (fabs((t = fabs(phi0)) - M_PI_2) < EPS10)
    mode = phi0 < 0. ? S_POLE : N_POLE;
  else
    mode = t > EPS10 ? OBLIQ : EQUIT;
  //phits = fabs(phits);
  if (!ell.fSpherical()) {  // ellipsoid case
    double X;
    switch (mode) {
      case N_POLE:
      case S_POLE:
      /*  if (fabs(phits - M_PI_2) < EPS10)   */
          akm1 = 2. * k0 /
                   sqrt(pow(1+e,1+e)*pow(1-e,1-e));
      /*  else {                        // use phi of true scale
          akm1 = cos(phits) /           // in polar case
          tsfn(phits);                  // if not too close to Pole
          t *= e;
          akm1 /= sqrt(1. - t * t);
        }         */
        break;
      case EQUIT:
        akm1 = 2. * k0;
        sinX1 = 0.;
        cosX1 = 1.;
        break;
      case OBLIQ:
        t = sin(phi0);
        X = 2. * atan(ssfn_(phi0, t, e)) - M_PI_2;
        t *= e;
        akm1 = 2. * k0 * cos(phi0) / sqrt(1. - t * t);
        sinX1 = sin(X);
        cosX1 = cos(X);
        break;
    }
  }
  else {
    switch (mode) {   // spherical case
      case OBLIQ:
        rSinPhi0 = sin(phi0);
        rCosPhi0 = cos(phi0);
      case EQUIT:
      case S_POLE:
      case N_POLE:
        akm1 = 2. * k0;
        break; 
      /*  
      case S_POLE:   // use phits if it's not too close to Pole
      case N_POLE:
        akm1 = fabs(phits - M_PI_2) >= EPS10 ?
        cos(phits) / tan(M_PI_4 - .5 * phits) :
        2. * k0 ;   
      break;  */
    }
  }
}


XY ProjectionStereoGraphic::xyConv(const PhiLam& _pl) const
{
  PhiLam pl = _pl;   // local copy of input _pl
  XY xy;
  double e = ell.e;
  if(!ell.fSpherical()) {  // ellipsoid
    double coslam, sinlam, sinX, cosX, X, A, sinphi;

    coslam = cos(pl.Lam);
    sinlam = sin(pl.Lam);
    sinphi = sin(pl.Phi);
    if (mode == OBLIQ || mode == EQUIT) {
      sinX = sin(X = 2. * atan(ssfn_(pl.Phi, sinphi, e)) - M_PI_2);
      cosX = cos(X);
    }
    switch (mode) {
      case OBLIQ:
        A = akm1 / (cosX1 * (1. + sinX1 * sinX +
            cosX1 * cosX * coslam));
        xy.y = A * (cosX1 * sinX - sinX1 * cosX * coslam);
        goto xmul;
      case EQUIT:
        A = akm1 / (1. + cosX * coslam);
        xy.y = A * sinX;
xmul:
        xy.x = A * cosX;
        break;
      case S_POLE:
        pl.Phi = -pl.Phi;
        coslam = - coslam;
        sinphi = -sinphi;
      case N_POLE:
        xy.x = akm1 * tsfn(pl.Phi);
        xy.y = - xy.x * coslam;
        break;
    }
    xy.x = xy.x * sinlam;
    return xy;
  }
  else /* sphere */
  {
    double  sinphi, cosphi, coslam, sinlam;

    sinphi = sin(pl.Phi);
    cosphi = cos(pl.Phi);
    coslam = cos(pl.Lam);
    sinlam = sin(pl.Lam);
    switch (mode) {
      case EQUIT:
        xy.y = 1. + cosphi * coslam;
        goto oblcon;
      case OBLIQ:
        xy.y = 1. + rSinPhi0 * sinphi + rCosPhi0 * cosphi * coslam;
oblcon:
        if (xy.y <= EPS10) return xy;
        xy.x = (xy.y = akm1 / xy.y) * cosphi * sinlam;
        xy.y *= (mode == EQUIT) ? sinphi :
            rCosPhi0 * sinphi - rSinPhi0 * cosphi * coslam;
        break;
      case N_POLE:
        coslam = - coslam;
        pl.Phi = - pl.Phi;
      case S_POLE:
        if (fabs(pl.Phi - M_PI_2) < TOL) return xy;
        xy.x = sinlam * ( xy.y = akm1 * tan(M_PI_4 + .5 * pl.Phi) );
        xy.y *= coslam;
        break;
    }
    return xy;
  }
}

PhiLam ProjectionStereoGraphic::plConv(const XY& _xy) const
{
  XY xy = _xy;
  PhiLam pl;
  double e = ell.e;
  if (!ell.fSpherical())  { // ellipsoid
    double cosphi, sinphi, tp, phi_l, rho, halfe, halfpi;
    int i;

    rho = sqrt(xy.x*xy.x + xy.y*xy.y);
    if (abs(rho) <=EPS10) {
      pl.Phi = phi0;
      pl.Lam = 0;
      return pl;
    }  // this check was not in USGS source
    switch (mode) {
      case OBLIQ:
      case EQUIT:
        cosphi = cos( tp = 2. * atan2(rho * cosX1 , akm1) );
        sinphi = sin(tp);
        phi_l = asin(cosphi * sinX1 + (xy.y * sinphi * cosX1 / rho));
        tp = tan(.5 * (M_PI_2 + phi_l));
        xy.x *= sinphi;
        xy.y = rho * cosX1 * cosphi - xy.y * sinX1* sinphi;
        halfpi = M_PI_2;
        halfe = .5 * e;
        break;
      case N_POLE:
        xy.y = -xy.y;
      case S_POLE:
        phi_l = M_PI_2 - 2. * atan(tp = - rho / akm1);
        halfpi = -M_PI_2;
        halfe = -.5 * e;
        break;
    }
    for (i = NITER; i--; phi_l = pl.Phi) {
         sinphi = e * sin(phi_l);
      pl.Phi = 2. * atan(tp * pow((1.+sinphi)/(1.-sinphi),
               halfe)) - halfpi;
      if (fabs(phi_l - pl.Phi) < CONV) {
        if (mode == S_POLE)
          pl.Phi = -pl.Phi;
        pl.Lam = (xy.x == 0. && xy.y == 0.) ? 0. : atan2(xy.x, xy.y);
        return (pl);
      }
    }
    return pl;
  }

  else {  /* sphere */
    double  c, rh, sinc, cosc;

    sinc = sin(c = 2. * atan((rh = sqrt(xy.x*xy.x + xy.y*xy.y)) / akm1));
    cosc = cos(c);
    pl.Lam = 0.;
    if (abs(rh) <=EPS10) {
      pl.Phi = phi0;
      pl.Lam = 0;
      return pl;
    }  // this check was not in USGS source  
    switch (mode) {
      case EQUIT:
        if (fabs(rh) <= EPS10)
          pl.Phi = 0.;
        else
          pl.Phi = asin(xy.y * sinc / rh);
        if (cosc != 0. || xy.x != 0.)
          pl.Lam = atan2(xy.x * sinc, cosc * rh);
        break;
      case OBLIQ:
        if (fabs(rh) <= EPS10)
          pl.Phi = phi0;
        else
          pl.Phi = asin(cosc * rSinPhi0 + xy.y * sinc * rCosPhi0 / rh);
        if ((c = cosc - rSinPhi0 * sin(pl.Phi)) != 0. || xy.x != 0.)
           pl.Lam = atan2(xy.x * sinc * rCosPhi0, c * rh);
        break;
      case N_POLE:
        xy.y = -xy.y;
      case S_POLE:
        if (fabs(rh) <= EPS10)
          pl.Phi = phi0;
        else
          pl.Phi = asin(mode == S_POLE ? - cosc : cosc);
          pl.Lam = (xy.x == 0. && xy.y == 0.) ? 0. : atan2(xy.x, xy.y);
        break;
    }
    return pl;
  }
}





