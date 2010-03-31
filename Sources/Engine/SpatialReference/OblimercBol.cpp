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
/*$Log: /ILWIS 3.0/CoordinateSystem/OblimercBol.cpp $
 * 
 * 1     4-05-06 15:15 Hendrikse
//Revision 1.1  2006/09/16 17:26:42  Jan Hendrikse
/* ProjectionObliqueMercator */

#include "Engine\SpatialReference\Oblimerc.h"
#include "Headers\Hs\DAT.hs"

#define TOL	1.e-7
#define EPS10	1.e-10

ProjectionObliqueMercator::ProjectionObliqueMercator(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
  fEll = true;
  fUseParam[pvLAT0] = true;
  fUseParam[pvLON0] = true;
  fUseParam[pvK0] = true;
  fUseParam[pvAZIMCLINE] = true;  // user can define azim of central line
  fUseParam[pvNORIENTED] = true; // user can decide to 'north-oriented' csy
  Param(pvLAT0, 0.);      // set to zero the default central latitude
}

void ProjectionObliqueMercator::Prepare()
{	 
  alpha = rAzimCLine; //pj_param( params, "ralpha").f;
  if (abs(abs(phi0) - M_PI_2) <= TOL)
    throw  ErrorObject(SDATErrCentralLatEqual90, 2211);
  if (abs(abs(alpha) - M_PI_2) <= TOL)
    throw  ErrorObject(SDATErrAzimuthEqual90, 2211);
  rSinPhi0 = sin( phi0);
  rCosPhi0 = cos( phi0);
  if (fNoriented)
	   f = alpha - M_PI_2;
   else
	   f = 0;
   sinrot = sin(f);
   cosrot = cos(f);
  if (ell.fSpherical()) {
	phiPole = asin(rCosPhi0 * sin(alpha));
	lamPole = atan2(-cos(alpha),(-rSinPhi0*sin(alpha))) + lam0; // lam0 is used later
	double fi = phiPole*180/M_PI; double la = lamPole*180/M_PI;
  }
  else { //Snyder Alternate B
	double e2 = ell.e2;
	double e = ell.e;
	double sqrt1_e2 = sqrt(1 - e2);
	double cos4phi0 = rCosPhi0*rCosPhi0*rCosPhi0*rCosPhi0;//Snyder Ch9
	B = sqrt(1 + e2 * cos4phi0/(1-e2));
	A = B*k0*sqrt1_e2/(1 - e2*rSinPhi0*rSinPhi0);// (9-12) 
	double t0 = tan(M_PI_4 - phi0/2)/pow((1 - e*rSinPhi0)/(1 + e*rSinPhi0), e/2); //(9-13) 
	D=B*sqrt1_e2/rCosPhi0/sqrt(1 - e2*rSinPhi0*rSinPhi0);//(9-14) 
	if (D*D < 1) D = max(1.0, D);
	int iSignPhi0 = (phi0>0) ? 1 : -1;
	double F = D + iSignPhi0*sqrt(D*D - 1)/e; //(9-35) 
	E = F*pow(t0,B); //(9- 36) 
	double G = (F - 1/F)/2; //(9-19) 
	gam0 = asin (sin(alpha)/D); //(9-37) 
	// lam0 = lam - arcsin (G*tan(gam0))/B  //(9-38)
	double uPhi0Lamc = iSignPhi0*A/B*atan2(sqrt(D*D - 1),cos(alpha)); // (9-39) 
	double vPhi0Lamc = 0; 
  }	
} // end Prepare()

XY  ProjectionObliqueMercator::xyConv(const PhiLam& pl) const
{
	XY xy;
	double v, u; // 'oblique' cartes coords (v along central line of true scale
	if ( ell.fSpherical()) {
		double U = sin(phiPole)*sin(pl.Phi-phi0)-cos(phiPole)*cos(pl.Phi-phi0)*sin(pl.Lam);
		v = k0*atan2((tan(pl.Phi-phi0)*cos(phiPole) + sin(phiPole)*sin(pl.Lam)),cos(pl.Lam));
		u = 0.5*k0*log((1+U)/(1-U)); // lam0 is used later
	}
	else {
		double fi = pl.Phi;
		double e = ell.e;
		double t = tan(M_PI_4-fi/2)/pow((1-e*sin(fi))/(1+e*sin(fi)), e/2); //(9-13) 
		double Q = E/pow(t,B); //(9-25) 
		double S = (Q - 1/Q)/2; //(9-26) 
		double T = (Q + 1/Q)/2; //(9-27) 
		double V = sin(B*(pl.Lam)); //(9-28) 
		double U = (-V*cos(gam0) + S*sin(gam0))/T; //(9-29) 
		if(abs(abs(U)-1) < EPS10) return xy;
		if(abs(abs(pl.Phi)-M_PI_2)<EPS10) //close to N/S pole
			v = A/B*log(tan(M_PI_4+gam0/2)); 
		
		u = A*atan((S*cos(gam0)+V*sin(gam0))/cos(B*pl.Lam))/B;// (9-31) 
		v = A*log((1+U)/(1-U))/2/B;   //(9-30)
	}
	xy.X = v *  cosrot + u *  sinrot;
	xy.Y = - v * sinrot + u *  cosrot;
	return xy;
}

PhiLam ProjectionObliqueMercator::plConv(const XY& xy) const
{
	PhiLam pl;
	double vs, us; // 'oblique' cartes coords (v along central line of true scale
	vs = xy.X *  cosrot - xy.Y *  sinrot;
	us = xy.X *  sinrot + xy.Y *  cosrot;
	if ( ell.fSpherical()) {
	  pl.Phi = asin((tanh(us/k0)*sin(phiPole) + cos(phiPole)*sin(vs/k0))/cosh(us/k0)) + phi0;
	  pl.Lam = atan((sin(vs/k0)*sin(phiPole) - cos(phiPole)*sinh(us/k0))/cos(vs/k0));
	  return pl;
	}	
	return pl;
}




