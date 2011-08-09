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
/* ProjectionAlbersEqualAreaConic
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
        double rC;        C for Albers equalArea conic  
	double rEc;  
	double apa[3] 
// See J.Snyder 'Map Projections, a Working manual' 
// USGS Paper 1395 Washington 1987 Ch 3 auxil latitudes eq (3-16) 
*/
#include "Engine\SpatialReference\ALBERS.H"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\proj.hs"

#define EPS10 1.e-10

ProjectionAlbersEqualAreaConic::ProjectionAlbersEqualAreaConic(const Ellipsoid& ell)
:ProjectionConic(ell)
{
	fUseParam[pvK0] = false;
}

void ProjectionAlbersEqualAreaConic::Prepare()
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
  rN = rSinPhi1;      // Cone constant of a cone which is tangent at rPhi1
  rSinPhi0 = sin(phi0);
  rCosPhi0 = cos(phi0);
	if(!ell.fSpherical())
	{
    enfn(en); // fill array for meridinal distance
		double ml1, m1;
		///if (!(P->en = pj_enfn(P->es))) E_ERROR_0;
		m1 = msfn(rPhi1);
		ml1 = qsfn(rPhi1);
		if (fSecant) { /* secant cone */
			double ml2, m2;
			m2 = msfn(rPhi2);
			ml2 = qsfn(rPhi2);
			rN = (m1 * m1 - m2 * m2) / (ml2 - ml1);
		}
		double e = ell.e;
		rEc = 1. - .5 * (1. - e*e) * log((1. - e) /
			(1. + e)) / e;
		rC = m1 * m1 + rN * ml1;
		rRho0 = sqrt(rC - rN * qsfn(phi0)) / rN;
		authset(apa);
	}
	else
	{
		if (fSecant)
			rN = (rSinPhi1 + rSinPhi2) / 2;       // Cone constant
		rC = rCosPhi1 * rCosPhi1 + 2 * rN * rSinPhi1;
		rRho0 = sqrt(rC - 2 * rN * rSinPhi0) / rN;
	}
}

XY ProjectionAlbersEqualAreaConic::xyConv(const PhiLam& pl) const
{
  XY xy;
	 //ellipsoid & sphere 
	double rRho;
	if(!ell.fSpherical()) 
		rRho = rC -  rN * qsfn(pl.Phi); 
	else 
	  rRho = rC -  2 * rN * sin(pl.Phi);
	if (rRho  < EPS10) 
		return xy;
  rRho = sqrt(rRho) / rN;
	if ( abs(pl.Phi) > M_PI_2 - EPS10 ) return xy;
  if (abs(pl.Lam) > M_PI)  return xy;
  double rTheta = pl.Lam * rN;
  xy.x =  (rRho * sin(rTheta) );
  xy.y =  (rRho0 - rRho * cos(rTheta) );
  return  xy;
}

PhiLam ProjectionAlbersEqualAreaConic::plConv(const XY& xy) const
{
  PhiLam pl;
	/* ellipsoid & spheroid */;
  XY _xy = xy;
  _xy.y = rRho0 - _xy.y;
  double rRho = rHypot(_xy.x, _xy.y);
	if (rRho > 0.0) {
		if (rN < 0.) {
			rRho = -rRho;
			_xy.x = -_xy.x;
			_xy.y = -_xy.y;
		}
		double rRhoN =  rRho * rN;
		double plPhi;
		if (!ell.fSpherical()) {
			double q = (rC - rRhoN * rRhoN) / rN;
			if (abs(q / rEc) >= 1)
				return pl;
			double beta = asin( q / rEc );//for authalic latitude
			pl.Phi = authlat(beta, apa);
			if (abs(pl.Phi) > M_PI_2)
				return pl;
		} 
		else {
			plPhi = (rC - rRhoN * rRhoN) / (rN + rN);
			if (fabs(plPhi) <= 1.)
				pl.Phi = asin(plPhi);
			else
				pl.Phi = plPhi < 0. ? -M_PI_2 : M_PI_2;
		}		
		pl.Lam = atan2(_xy.x, _xy.y) / rN;
	} 
	else 
	{
		pl.Lam = 0.;
		pl.Phi = rN > 0. ? M_PI_2 : - M_PI_2;
	}
	return pl;
}





