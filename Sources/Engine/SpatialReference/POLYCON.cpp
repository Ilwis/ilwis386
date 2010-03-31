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
//$Log: POLYCON.C $
//Revision 1.6  1998/10/03 13:24:53  janh
//Adde in xyConv(pl) 2 times  the multiplication * x0
//
//Revision 1.5  1998/10/02 14:23:51  janh
//implemented scale factor at central meridian as fUserParam
//
//Revision 1.4  1998/09/16 17:26:42  Wim
//22beta2
//
//Revision 1.3  1997/08/27 10:35:06  janh
//Added derived class statement  :ProjectionPtr(ell) in constructor
//
//Revision 1.2  1997/08/27 10:28:24  janh
//Ade fEll = true in constructor
//Added call enfn(en) before calling mlfn in Prepare()
//
/* ProjectionPolyconic
   copyright Ilwis System Development ITC
   june 1996, by Jan Hendrikse
	Last change:  JHE  27 Aug 97   11:34 am
*/

/* members declared in polycon.h :
        double ml0; \
	double *en;                                                          {

	}
*/
#include "Engine\SpatialReference\POLYCON.H"

#define TOL	1e-10
#define CONV	1e-10
#define N_ITER	10
#define I_ITER 20
#define ITOL 1.e-12

ProjectionPolyConic::ProjectionPolyConic(const Ellipsoid& ell)
:ProjectionPtr(ell)
{
  fEll = true;
  fUseParam[pvLAT0] = true;
  fUseParam[pvK0] = true;
  Param(pvLAT0, 0.0);
}

void ProjectionPolyConic::Prepare()
{
  double es = ell.e2;
  if ( es) {   // ellipsoidal case
    enfn(en);
	  ml0 = mlfn( phi0, sin( phi0), cos( phi0),  en);
  }
  else
  	 ml0 = - phi0;
         // spherical case
}

XY ProjectionPolyConic::xyConv(const PhiLam& _pl) const
 /* ellipsoid & sphere */
{
  XY xy;
  PhiLam pl = _pl;
  if   (!ell.fSpherical()) {

//FORWARD(e_forward); /* ellipsoid */
    double  ms, sp, cp;

    if (fabs(pl.Phi) <= TOL) { xy.x = pl.Lam * k0; xy.y = - ml0 * k0; }
    else {
    	sp = sin(pl.Phi);
    	ms = fabs(cp = cos(pl.Phi)) > TOL ? msfn(pl.Phi) / sp : 0.;
	xy.x = ms * sin(pl.Lam*= sp) * k0;
	xy.y = ((mlfn(pl.Phi, sp, cp,  en) -  ml0) + ms * (1. - cos(pl.Lam))) * k0;
    }
    return (xy);
  }
  else {
// FORWARD(s_forward); /* sphere */
    double  cot, E;
    if (fabs(pl.Phi) <= TOL) { xy.x = pl.Lam; xy.y =  ml0; }
    else {
	cot = 1. / tan(pl.Phi);
	xy.x = k0 * sin(E = pl.Lam* sin(pl.Phi)) * cot;
	xy.y = k0 * (pl.Phi -  phi0 + cot * (1. - cos(E)));
    }
    return (xy);
  }
}

PhiLam ProjectionPolyConic::plConv(const XY& _xy) const
  /* ellipsoid & sphere */
{
  PhiLam pl;
  XY xy = _xy;
  xy.x /= k0;
  xy.y /= k0;
  double es = ell.e2;
  double one_es = 1 - es;
  if   (!ell.fSpherical()) {
// INVERSE(e_inverse); /* ellipsoid */
    xy.y +=  ml0;
    if (fabs(xy.y) <= TOL) { pl.Lam= xy.x; pl.Phi = 0.; }
    else {
      double r, c, sp, cp, s2ph, ml, mlb, mlp, dPhi;
      int i;
      r = xy.y * xy.y + xy.x * xy.x;
      for (pl.Phi = xy.y, i = I_ITER; i ; --i) {
      	sp = sin(pl.Phi);
      	s2ph = sp * ( cp = cos(pl.Phi));
	if (fabs(cp) < ITOL)
          return pl;
        c = sp * (mlp = sqrt(1. -  es * sp * sp)) / cp;
	ml = mlfn(pl.Phi, sp, cp,  en);
	mlb = ml * ml + r;
	mlp =  one_es / (mlp * mlp * mlp);
	pl.Phi += ( dPhi =
		( ml + ml + c * mlb - 2. * xy.y * (c * ml + 1.) ) / (
		 es * s2ph * (mlb - 2. * xy.y * ml) / c +
		2.* (xy.y - ml) * (c * mlp - 1. / s2ph) - mlp - mlp ));
	if (fabs(dPhi) <= ITOL)
	  break;
      }
      if (!i)
        return pl;
      c = sin(pl.Phi);
      pl.Lam= asin(xy.x * tan(pl.Phi) * sqrt(1. -  es * c * c)) / sin(pl.Phi);
    }
    return (pl);
  }
  else {
// INVERSE(s_inverse); /* sphere */
    double B, dphi, tp;
    int i;

    if (fabs(xy.y =  phi0 + xy.y) <= TOL) { pl.Lam= xy.x; pl.Phi = 0.; }
    else {
      pl.Phi = xy.y;
      B = xy.x * xy.x + xy.y * xy.y;
      i = N_ITER;
      do {
     	tp = tan(pl.Phi);
     	pl.Phi -= (dphi = (xy.y * (pl.Phi * tp + 1.) - pl.Phi -
		.5 * ( pl.Phi * pl.Phi + B) * tp) /
		((pl.Phi - xy.y) / tp - 1.));
      } while (fabs(dphi) > CONV && --i);
      if (! i) return pl;
      pl.Lam= asin(xy.x * tan(pl.Phi)) / sin(pl.Phi);
    }
    return (pl);
  }
}






