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
/* ProjectionOrthoGraphic  // also Non-Polar
   Copyright Ilwis System Development ITC
  may 1996, by Jan Hendrikse
	Last change:  JEL  19 Oct 96   12:54 pm
*/
#include "Engine\SpatialReference\ORTHOGR.H"
#include "Engine\Base\DataObjects\ERR.H"

#define EPS10  1.e-10
#define OBLIQ  3
#define N_POLE 0
#define S_POLE 1
#define EQUIT  2

ProjectionOrthoGraphic::ProjectionOrthoGraphic()
:ProjectionAzimuthal()
{
  fUseParam[pvK0] = false;
}

void ProjectionOrthoGraphic::Prepare()
{
 //  first computing the constants related to the 
 // geometry of the given azimuth plane and ellipsoid
  if ( abs(phi0)>M_PI_2 )
    InvalidLat();
  if (fabs(fabs( phi0) - M_PI_2) <= EPS10)
    mode =  phi0 < 0. ? S_POLE : N_POLE;
  else if (fabs( phi0) > EPS10) {
    mode = OBLIQ;
    rSinPhi0 = sin( phi0);
    rCosPhi0 = cos( phi0);
  }
  else
    mode = EQUIT;
}


XY ProjectionOrthoGraphic::xyConv(const PhiLam& _pl) const
{               // sphere
  XY xy;
  PhiLam pl = _pl;
  double  coslam, cosphi, sinphi;

  cosphi = cos(pl.Phi);
  coslam = cos(pl.Lam);
  switch ( mode) {
    case EQUIT:
      if (cosphi * coslam < - EPS10) return xy;
        xy.y = sin(pl.Phi);
      break;
    case OBLIQ:
      if ( rSinPhi0 * (sinphi = sin(pl.Phi)) +
        rCosPhi0 * cosphi * coslam < - EPS10) return xy;
      xy.y =  rCosPhi0 * sinphi -  rSinPhi0 * cosphi * coslam;
      break;
    case N_POLE:
      coslam = - coslam;
    case S_POLE:
      if (fabs(pl.Phi -  phi0) - EPS10 > M_PI_2) return xy;
        xy.y = cosphi * coslam;
      break;
   }
   xy.x = cosphi * sin(pl.Lam);
   return xy;
}
PhiLam ProjectionOrthoGraphic::plConv(const XY& _xy) const
{
  PhiLam pl;
  XY xy = _xy;
  double  rh, cosc, sinc;

  if ((sinc = (rh = sqrt(xy.x*xy.x + xy.y*xy.y))) > 1.) {
    if ((sinc - 1.) > EPS10) return pl;
    sinc = 1.;
  }
  cosc = sqrt(1. - sinc * sinc); /* in this range OK */
  if (fabs(rh) <= EPS10)
    pl.Phi =  phi0;
    else 
       switch ( mode) {
         case N_POLE:
           xy.y = -xy.y;
           pl.Phi = acos(sinc);
           break;
         case S_POLE:
           pl.Phi = - acos(sinc);
           break;
         case EQUIT:
           pl.Phi = xy.y * sinc / rh;
           xy.x *= sinc;
           xy.y = cosc * rh;
           goto sinchk;
         case OBLIQ:
           pl.Phi = cosc *  rSinPhi0 + xy.y * sinc *  rCosPhi0 / rh;
           xy.y = (cosc -  rSinPhi0 * pl.Phi) * rh;
           xy.x *= sinc *  rCosPhi0;
sinchk:
           if (fabs(pl.Phi) >= 1.)
             pl.Phi = pl.Phi < 0. ? -M_PI_2 : M_PI_2;
           else
             pl.Phi = asin(pl.Phi);
           break;
      }
      pl.Lam= (xy.y == 0. && ( mode == OBLIQ ||  mode == EQUIT)) ?
            (xy.x == 0. ? 0. : xy.x < 0. ? -M_PI_2 : M_PI_2) : atan2(xy.x, xy.y);
return pl;
}





