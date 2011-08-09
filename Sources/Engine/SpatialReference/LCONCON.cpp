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
/* ProjectionLambConfConic
   copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE  17 Oct 96    5:49 pm
*/
/* members declared in prj.h and conic.h

	double	rPhi1; \      parameters rPhi1,rPh2; members of ProjectionPtr
	double	rPhi2; \
	double	rN; \       cone constant; member of ProjectionConic
	double	rRho0; \  radius from cone apex to phi0; member of ProjectionConic
	double	rF; \    F for Lamb conf conic
        double rC        C for Albers equalArea conic  */

#include "Engine\SpatialReference\LCONCON.H"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\proj.hs"

#define EPS10 1.e-10

ProjectionLambConfConic::ProjectionLambConfConic(const Ellipsoid& ell)
:ProjectionConic(ell)
{
}

void ProjectionLambConfConic::Prepare()
{
 //  first computing the constants related to the 
 // geometry of the given cone and ellipsoid
  if ( abs(rPhi1) > abs(rPhi2) ) {   // rPhi1 should be latitude
    double rTemp = rPhi2;           // closest to equator
    rPhi2 = rPhi1;                 // hence swap if needed
    rPhi1 = rTemp;
  }
  if ( abs(rPhi1 + rPhi2) < EPS10 )
     throw  ErrorObject(TR("Standard Parallels should define a cone, not a cylinder"), 2211);
  fSecant = abs(rPhi1 - rPhi2) >= EPS10;
  rN = rSinPhi1;                   // Cone constant of cone being tangent at rPhi1
  if (!ell.fSpherical()) {         // Ellipsoidal case
    double m1 = msfn(rPhi1);
    double t1 = tsfn(rPhi1);
    if (fSecant)  /* secant cone */
       rN = log(m1 / msfn(rPhi2)) / log(t1 / tsfn(rPhi2));  // cone constant

    rF =  m1 * pow(t1, -rN) / rN ;
    rC = (rRho0 = m1 * pow(t1, -rN) / rN);
    rRho0 = rF * pow(tsfn(phi0), rN);
  } 
  else  {                          // Spherical case
    if (fSecant)
      rN = log(rCosPhi1 / rCosPhi2) /
                         log(tan(M_PI_4 + rPhi2/2) /tan(M_PI_4 + rPhi1/2));
    rF = rCosPhi1 * pow(tan(M_PI_4 + rPhi1/2), rN) / rN;
    rRho0 = rF * pow(tan(M_PI_4 + phi0/2), -rN);
  }
}

XY ProjectionLambConfConic::xyConv(const PhiLam& pl) const
 /* ellipsoid & sphere */
{
  XY xy;
  double rRho;
  if (abs(pl.Phi) >  M_PI_2 - EPS10) return xy;
  if (abs(abs(pl.Phi) - M_PI_2) < EPS10) {
    if ((pl.Phi * rN) <= 0.) return xy;   // point near pole
      rRho = 0.;
    }
  else
    rRho = rF *
     (!ell.fSpherical() ? pow(tsfn(pl.Phi), rN) :
                         pow(tan(M_PI_4 + pl.Phi/2), -rN));
  double rTheta = pl.Lam * rN;
  xy.x =  (rRho * sin(rTheta) ) * k0;
  xy.y =  (rRho0 - rRho * cos(rTheta) ) * k0;
  return  xy;
}

PhiLam ProjectionLambConfConic::plConv(const XY& xy) const
  /* ellipsoid & spheroid */
{
  PhiLam pl;
  double rRho;
  double rY = rRho0 - xy.y / k0;
  double rX = xy.x / k0;
  rRho = sqrt(rX*rX + rY*rY);
  if (rRho >0 ) {
    if (rN < 0.) {         // Southern Hemisphere
      rRho = -rRho;
      rX = -rX;
      rY = -rY;
    }
    if (!ell.fSpherical()) {
      pl.Phi = phi2(pow(rRho / rF, 1./ rN));
      if (abs(pl.Phi) >= M_PI_2) return pl;
    }
    else     // spherical case
      pl.Phi = 2. * atan(pow(rF / rRho, 1./ rN)) - M_PI_2;
    pl.Lam = atan2(rX, rY) / rN;
  }
  else {      //  North or South Pole
      pl.Lam = 0.;
      pl.Phi = rN > 0. ? M_PI_2 : - M_PI_2;
  }
  return pl;
}





