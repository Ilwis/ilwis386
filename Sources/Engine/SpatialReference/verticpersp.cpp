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
/* ProjectionGeoStationarySatellite
   Copyright Ilwis System Development ITC
    may 2004, by Jan Hendrikse
*/
#include "Engine\SpatialReference\verticpersp.h"

#define EPS10	1.e-10


ProjectionVerticalPerspective::ProjectionVerticalPerspective(const Ellipsoid& ell)
:ProjectionAzimuthal(ell)
{
  //fUseParam[pvK0] = false;
	fUseParam[pvHEIGHT] = true;
	fUseParam[pvLAT0] = true;
	//"Map Projections - A Working Manual"  John P.Snyder USGS Professional Paper 1395
	//US Govt Printing Office, Washington 1987  page 176
}
	
void ProjectionVerticalPerspective::Prepare()
{
	rSinPhi0 = sin(phi0);
	rCosPhi0 = cos(phi0);
	rSin2Phi0= sqr(rSinPhi0);
	a = ell.a; e2 = ell.e2;
	rDista = a + rHeight; //rDista_toCenter
	N1 = a / sqrt(1 - e2 * rSin2Phi0);
	rGeoCentLat0 = phi0 - asin(N1 * e2 * rSinPhi0 * rCosPhi0/rDista);
	double P = rDista/a;//for debugging
	double Lat0Degrees = phi0 * 180 / M_PI;
	double GCLat0Degrees = rGeoCentLat0 * 180 / M_PI;
}


XY ProjectionVerticalPerspective::xyConv(const PhiLam& _pl) const
{
		XY xy;
		PhiLam pl = _pl;
	//FORWARD(John Snyder); /* ellipsoid */ // equatorial aspect only if phi0 = 0
		double tangentLength = sqrt(sqr(rDista) - sqr(a));
		double visiblehalfarc = acos(rSinPhi0*sin(pl.Phi) + rCosPhi0*cos(pl.Phi)*cos(pl.Lam));
		if (visiblehalfarc > acos(a / rDista))
			return xy;  // point is not visible from satellite

		double rSinPhi = sin(pl.Phi);
		double rSin2Phi = rSinPhi*rSinPhi;
		double rCosPhi = cos(pl.Phi);
		double rSinLam = sin(pl.Lam);
		double rCosLam = cos(pl.Lam);
		double N = a / sqrt(1 - e2 * rSin2Phi);
		double C = N * rCosPhi/a; // eq 23-15
		double S = N * (1 - e2) * rSinPhi/a; // eq 23-16
		double rLatitudeCorrFactor = ell.b * ell.b / a /a;
		//double rGeoCentLat0 = atan(rLatitudeCorrFactor * tan (phi0));
		//rGeoCentLat0 = phi0 - asin(N1*ell.e2*rSinPhi0*rCosPhi0/rDista);
		double H = rDista*cos(rGeoCentLat0)/rCosPhi0 - N1;
		double K = H/(rDista*cos(phi0-rGeoCentLat0)/a - S*rSinPhi0 - C*rCosPhi0*rCosLam);
 		//double K = rHeight / (rDista / ell.a - C * rCosLam);// eq 23-19
		xy.x =  K * C * rSinLam / a * k0;
		xy.y =  K * (rDista/a * sin(phi0-rGeoCentLat0) + S*rCosPhi0 - C*rSinPhi0*rCosLam) /a * k0;
		return (xy);
		// in case of sphere we have\;
		// N = 1; C = rCosPhi;
		// S = rSinPhi
		// K = rHeight / (rDista / ell.a - rCosPhi * rCosPhi
		// xy.x =  K * rCosPhi * rSinLam/ ell.a;
		// xy.y =  K * rSinPhi / ell.a; or = K * rSinLam / ell.a
}


PhiLam ProjectionVerticalPerspective::plConv(const XY& _xy) const
{
		PhiLam pl;
		XY xy = _xy;
	//INVERSE(John Snyder); // equatorial aspect only // equatorial aspect only
		/* ellipsoid */
		
		double H, P, B, D, L, G, J, u, v, t, K, X, Y, S;
		double N1 = a / sqrt(1 - e2 * rSin2Phi0);
		H = rDista*cos(rGeoCentLat0)/rCosPhi0 - N1;
		P = rDista / a;
		B = P * cos(phi0 - rGeoCentLat0);
		D = P * sin(phi0 - rGeoCentLat0);
		L = 1 - e2 * sqr(rCosPhi0);
		G = 1 - e2 * sqr(rSinPhi0);
		J = 2 * e2 * rSinPhi0 * rCosPhi0;
		xy.x *= a / k0; // bring xy from unit sphere to real world scale
		xy.y *= a / k0;
		u = -2* B * L * H - 2 * D * G * xy.y + B * J * xy.y + D * H * J;// eq 23-27
		v = L * H * H  + G * sqr(xy.y) - H * J * xy.y + (1 - e2) * sqr(xy.x);//eq 23-28
		t = P * P * (1 - e2 * sqr(cos(rGeoCentLat0))) - (1 - e2);
		K = ( -u + sqrt( u*u - 4*t*v)) / 2 / t;// eq 23-30
		X = a * ((B - H / K) * rCosPhi0 - (xy.y/K - D) * rSinPhi0);// eq 23-31
		Y = a * xy.x / K;// eq 23-32
		S = (xy.y / K - D) * rCosPhi0 + (B - H / K) * rSinPhi0;
		pl.Lam = atan(Y / X);
		pl.Phi = atan(S / sqrt((1 - e2) * (1 - e2 - S * S)));
		return pl;
}


