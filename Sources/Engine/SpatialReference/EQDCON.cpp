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
//$Log: EQDCON.C $
//Revision 1.4  1998/10/05 12:53:59  janh
//userParam pvk0 desabled (user can't change it anymore)
//
//Revision 1.3  1998/09/16 17:26:42  Wim
//22beta2
//
//Revision 1.2  1997/09/25 11:29:46  janh
//In function Prepare() I added enfn(en) which makes an array of coefficients
//neede for the rectifying latitude in this projection
//This was done 3 weeks ago but check in went wrong
//
/* ProjectionEquiDistantconic
   copyright Ilwis System Development ITC
   june 1996, by Jan Hendrikse
	Last change:  JHE  25 Sep 97   12:18 pm
*/
/* members declared in prj.h and conic.h

	double	rPhi1; \      parameters rPhi1,rPh2; members of ProjectionPtr
	double	rPhi2; \
	double	rN; \       cone constant; member of ProjectionConic
	double	rRho0; \  radius from cone apex to phi0; member of ProjectionConic
	double	rF; \    F for Lamb conf conic
        double rC        C for Albers equalArea conic and Equidistant Conic */

#include "Engine\SpatialReference\EQDCON.H"
#include "Engine\Base\DataObjects\ERR.H"
#define EPS10 1.e-10

ProjectionEquiDistConic::ProjectionEquiDistConic(const Ellipsoid& ell)
:ProjectionConic(ell)
{
  fUseParam[pvK0] = false;
}

void ProjectionEquiDistConic::Prepare()
{
 //  first computing the constants related to the 
 // geometry of the given cone and ellipsoid

  if (abs(rPhi1 + rPhi2) < EPS10)
    InvalidStandardParallels();
  fSecant = (abs(rPhi1 - rPhi2) >= EPS10);
  rN = rSinPhi1;                   // Cone constant of cone being tangent at rPhi1

  if (!ell.fSpherical()) {         // Ellipsoidal case
    enfn(en);
    double m1 = msfn(rPhi1);
    double ml1 = mlfn(rPhi1,rSinPhi1, rCosPhi1, en);
    if (fSecant)  /* secant cone */
      rN = (m1 - msfn(rPhi2)) /(mlfn(rPhi2, rSinPhi2, rCosPhi2, en) - ml1);
    rC = ml1 + m1 / rN;
    rRho0 = rC - mlfn(phi0, sin(phi0),cos(phi0), en);
  }

  else  {                          // Spherical case
    if (fSecant)
      rN = (cos(rPhi1) - cos(rPhi2)) / (rPhi2 - rPhi1);
    rC = rPhi1 + rCosPhi1 / rN;
    rRho0 = rC - phi0;
  }
}

XY ProjectionEquiDistConic::xyConv(const PhiLam& _pl) const
 /* ellipsoid & sphere */
{
  PhiLam pl = _pl;
  XY xy;
  double sinPhi = sin(pl.Phi);
  double cosPhi = cos(pl.Phi);
  double rRho  = rC - (!ell.fSpherical()
       ? mlfn(pl.Phi, sinPhi, cosPhi, en) : pl.Phi);

  if (fabs(pl.Phi) > M_PI_2 - EPS10) return xy;
  if ((pl.Phi * rN) <= 0.) return xy;
  else {
    xy.x = rRho * sin( pl.Lam *= rN );
    xy.y = rRho0 - rRho * cos(pl.Lam);
    return xy;
  }
}

PhiLam ProjectionEquiDistConic::plConv(const XY& xy) const
  /* ellipsoid & spheroid */
{
  PhiLam pl;
  double rRho;
  double rY = rRho0 - xy.y;
  double rX = xy.x;
  rRho = sqrt(rX*rX + rY*rY);
  if (rRho >0 ) {
    if (rN < 0.) {         // Southern Hemisphere
      rRho = -rRho;
      rX = -rX;
      rY = -rY;
    }
    pl.Phi = rC - rRho;
    if (!ell.fSpherical())
      pl.Phi = invmlfn(pl.Phi, ell.e2, en);
    if (abs(pl.Phi) > M_PI_2) return pl;

    pl.Lam = atan2(rX, rY) / rN;
  }
  else {      //  North or South Pole
    pl.Lam = 0.;
    pl.Phi = rN > 0. ? M_PI_2 : - M_PI_2;
  }
  return pl;
}





