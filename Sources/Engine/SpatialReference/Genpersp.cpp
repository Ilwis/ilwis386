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
/* Projection General Near-sided perspective
   Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
 Last change:  WK   17 Apr 98    4:22 pm
*/
#include "Engine\SpatialReference\GENPERSP.H"
#include "Engine\Base\DataObjects\ERR.H"

//members of class ProjectionGeneralPerspective
/* double p; 
double rp;
double pn1; 
double pfact; 
double h; 
double cg; 
double sg; 
double sw; 
double cw; */

# define EPS10 1.e-10
# define N_POLE 0
# define S_POLE 1
# define EQUIT 2
# define OBLIQ 3

ProjectionGeneralPerspective::ProjectionGeneralPerspective(const Ellipsoid& ell)
:ProjectionAzimuthal(ell)
{ 
	fEll = false;
  fUseParam[pvK0] = false;
  fUseParam[pvHEIGHT] = true;
  //fUseParam[pvTILTED] = true;
  fUseParam[pvTILT] = true;
  fUseParam[pvAZIMYAXIS] = true;
}

void ProjectionGeneralPerspective::Prepare()
{
  if (fabs(fabs( phi0) - M_PI_2) < EPS10)
    mode = phi0 < 0. ? S_POLE : N_POLE;
  else if (fabs(phi0) < EPS10)
    mode = EQUIT;
  else {
    mode = OBLIQ;
    rSinPhi0 = sin(phi0);
    rCosPhi0 = cos(phi0);
  }
  pn1 = rHeight / 6371007.1809185; /* normalize by radius */
  p = 1. + pn1;
  rp = 1. / p;
  h = 1. / pn1;
  pfact = (p + 1.) * h;
//  if (rTilt != 0)
//  {
    cg = cos(rAzimYaxis); sg = sin(rAzimYaxis);
    cw = cos(rTilt); sw = sin(rTilt);
//  }
}

//FORWARD(s_forward); /* spheroid */
XY ProjectionGeneralPerspective::xyConv(const PhiLam& _pl) const
{
  PhiLam pl = _pl;   // local copy of input _pl
  XY xy;
  double  coslam, cosphi, sinphi;
  sinphi = sin(pl.Phi);
  cosphi = cos(pl.Phi);
  coslam = cos(pl.Lam);
  switch ( mode) {
    case OBLIQ:
      xy.y =  rSinPhi0 * sinphi +  rCosPhi0 * cosphi * coslam;
      break;
    case EQUIT:
      xy.y = cosphi * coslam;
    break;
    case S_POLE:
      xy.y = - sinphi;
    break;
    case N_POLE:
      xy.y = sinphi;
    break;
  }
  if (xy.y <  rp)  return xy;
  xy.y =  pn1 / ( p - xy.y);
  xy.x = xy.y * cosphi * sin(pl.Lam);
  switch ( mode) {
    case OBLIQ:
      xy.y *= ( rCosPhi0 * sinphi -
       rSinPhi0 * cosphi * coslam);
    break;
    case EQUIT:
      xy.y *= sinphi;
    break;
    case N_POLE:
      coslam = - coslam;
    case S_POLE:
      xy.y *= cosphi * coslam;
    break;
  }
//  if (rTilt != 0) {
    double yt, ba;

    yt = xy.y *  cg + xy.x *  sg;
    ba = 1. / (yt *  sw *  h +  cw);
    xy.x = (xy.x *  cg - xy.y *  sg) *  cw * ba;
    xy.y = yt * ba;
//  }
  return (xy);
}
//INVERSE(s_inverse); /* spheroid */

PhiLam ProjectionGeneralPerspective::plConv(const XY& _xy) const
{
  XY xy = _xy;
  PhiLam pl;
  double  rh, cosz, sinz;
//  if (rTilt != 0)
//  {
    double bm, bq, yt;
    yt = 1./( pn1 - xy.y *  sw);
    bm = pn1 * xy.x * yt;
    bq = pn1 * xy.y *  cw * yt;
    xy.x = bm *  cg + bq *  sg;
    xy.y = bq *  cg - bm *  sg;
//  }
  rh = _hypot(xy.x, xy.y);
  if ((sinz = 1. - rh * rh *  pfact) < 0.) return pl;
  sinz = ( p - sqrt(sinz)) / ( pn1 / rh + rh /  pn1);
  cosz = sqrt(1. - sinz * sinz);
  if (fabs(rh) <= EPS10) {
    pl.Lam = 0.;
    pl.Phi =  phi0;
  }
  else {
    switch ( mode) {
      case OBLIQ:
        pl.Phi = asin(cosz *  rSinPhi0 + xy.y * sinz *  rCosPhi0 / rh);
        xy.y = (cosz -  rSinPhi0 * sin(pl.Phi)) * rh;
        xy.x *= sinz *  rCosPhi0;
      break;
      case EQUIT:
        pl.Phi = asin(xy.y * sinz / rh);
        xy.y = cosz * rh;
        xy.x *= sinz;
      break;
      case N_POLE:
        pl.Phi = asin(cosz);
        xy.y = -xy.y;
      break;
      case S_POLE:
        pl.Phi = - asin(cosz);
      break;
    }
    pl.Lam = atan2(xy.x, xy.y);
  }
  return (pl);
}




