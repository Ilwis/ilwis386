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
/* DutchRD
   Copyright Ilwis System Development ITC
	 june 1997, by  Jan Hendrikse
	Last change:  JH    19 Apr 04    2:04 pm
*/

#include "Engine\SpatialReference\Dutchrd.h"
#define  EPS10 1.e-10
double  B0 = 52.121097249 * M_PI/180;
double  L0 = 5.387638889  * M_PI/180;
double  rNgauss = 1.00047585668;
double  rMgauss = 0.003773953832;
double  rBOLSTRAAL = 6382644.571;
double	rM0; //kromtestraal nrd-zd in Amersfoort (phi0,lam0)
double	rN0; //kromtestraal west-oost in Amersfoort 

// Ref G. Strang van Hees, "Globale en lokale geodetische systemen"
// Nederl. Cie voor geodesie. Publicatie 30 2e Druk, Delft 1994
// pp. 18 - 21, pp. 26 - 28

ProjectionDutchRD::ProjectionDutchRD(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
	fEll = true;
	fUseParam[pvX0] = false;
	fUseParam[pvY0] = false;
	fUseParam[pvLON0] = false;
	x0 = 155000;
	y0 = 463000;
	k0 = 0.9999079;
	double phi0_old = 52.156160556 * M_PI / 180; // = 52gr 09' 22.178"
	double lam0_old = 5.387638889 * M_PI / 180;  // = 5 gr 23' 15.500"
	double phi0_old2 = 52.156160555555556 * M_PI / 180; // = 52gr 09' 22.178"
	double lam0_old2 = 5.387638888888889 * M_PI / 180;  // = 5 gr 23' 15.500"
	phi0 = (52 + 9.0/60.0 + 22.178/3600.0) * M_PI / 180; //central (geodetic) latitude on ellipsoid 
  lam0 = (5 + 23.0/60.0 + 15.5/3600.0) * M_PI / 180;

	// from here on, the parameter values are not 'Dutch RD-specific' anymore
	// if phi0, lam0, x0, y0 and ell are chosen differently.
	
	///////////explicit computation of Sphere-radius rBOLSTRAAL:
	double rWortel = sqrt(1 - ell.e2 * sin(phi0) * sin(phi0));
	rN0  = ell.a / rWortel; 
	double rW3 = rWortel * rWortel * rWortel;
	rM0  = ell.a * (1 - ell.e2) / rW3;
	rBOLSTRAAL = sqrt(rM0 * rN0);
	///////////explicit computation of Gauss coefficients:
	double e2prime = ell.e2 / (1 - ell.e2);
	double cos4Phi0 = cos(phi0)*cos(phi0)*cos(phi0)*cos(phi0);
	rNgauss = sqrt(1 + e2prime * cos4Phi0);
	double eccSinPhi0 = ell.e * sin(phi0);
	// central isometric latitude on ellipsoid:
	double rQ0 = log(tan(M_PI_4 + phi0/2)) - 0.5 * ell.e * log((1 + eccSinPhi0)/(1 - eccSinPhi0));
	// central latitude on sphere: 
	B0 = asin(sin(phi0)/rNgauss);
	// central isometric latitude on sphere: 
	double rW0 = log(tan(M_PI_4 + B0/2)); 
	rMgauss = rW0 - rNgauss * rQ0;
// Ref G. Strang van Hees, "Globale en lokale geodetische systemen"
// Nederl. Cie voor geodesie. Publicatie 30, 3e Herziene Druk, Delft 1997
// pag 20
}


XY ProjectionDutchRD::xyConv(const PhiLam& pl) const
{
	XY xy;
	double rQ, rQ1, rDQ, rW, rB, rDL;
	double ecc = ell.e;
	double sinPsi, cosPsi, sinhalfPsi, coshalfPsi, tanhalfPsi;
  double eccsinPhi = ecc * sin(pl.Phi); //(geodetic) latitude on ellipsoid
	rQ1 = log(tan(M_PI_4 + pl.Phi/2));
	///rQ1 = atanh(sinPhi);
	rDQ = 0.5 * ecc * log(( 1 + eccsinPhi)/(1 - eccsinPhi));
	///rDQ = ecc * atanh( ecc * sinPhi);
	rQ = rQ1 - rDQ;										// isometric latitude on ellipsoid
	rW = rNgauss * rQ + rMgauss;			// isometric latitude on Schreiber sphere
	rB = 2 * atan(exp(rW)) - M_PI_2;	//  latitude on sphere
	rDL =  rNgauss * pl.Lam; ///rNgauss * (pl.Lam - lam0);
	double sin2halfPsi = sin((rB - B0)/2) * sin((rB - B0)/2) +
										 sin(rDL/2) * sin(rDL/2) * cos(rB) * cos(B0);
	coshalfPsi = sqrt( 1 - sin2halfPsi);
	sinhalfPsi = sqrt(sin2halfPsi);
	tanhalfPsi = sinhalfPsi / coshalfPsi;
	sinPsi = 2 * sinhalfPsi * coshalfPsi; 
	if (abs(sinPsi) < EPS10) {
		xy.x = 0;
		xy.y = 0;
		return xy;
	}
	cosPsi = 1 - 2 * sin2halfPsi;
	double sinAlpha = sin(rDL) * cos(rB) / sinPsi;     // Alpha,Rho are polar coords in projection plane
	double cosAlpha = (sin(rB) - sin(B0) * cosPsi) / (cos(B0) * sinPsi); // origin Amersfoort
	double rRho = 2 * k0 * rBOLSTRAAL * tanhalfPsi / ell.a; //           // at unit sphere scale
	xy.x = rRho * sinAlpha;
	xy.y = rRho * cosAlpha;
	return xy;
}

PhiLam ProjectionDutchRD::plConv(const XY& xy) const
{
	PhiLam pl;
	double rRho2 = xy.x * xy.x  + xy.y * xy.y;
	double rRho = sqrt(xy.x * xy.x  + xy.y * xy.y);// Hypoth function better ?
	if (rRho2 < EPS10) {
	  pl.Phi = phi0;
	  pl.Lam = 0.0;
	  return pl;        // point in projection origin (Amersfoort)
	}
	double sinAlpha = xy.x  / rRho;
	double cosAlpha = xy.y  / rRho;
	double rPsi = 2 * atan(rRho * ell.a /(2 * k0 * rBOLSTRAAL)); // geocentric latitude
	double sinB = cosAlpha * cos(B0) * sin(rPsi) + sin(B0) * cos(rPsi);
	double rB = asin(sinB);								// latitude on Schreiber sphere
	double sinDL = sinAlpha * sin(rPsi) / cos(rB);
	double rW = log(tan(rB/2.0 + M_PI_4));// isometric latitude on sphere
	double rQ = (rW - rMgauss) / rNgauss;	// isometric latitude on ellipsoid
	double rPhiApprox = 2.0 * atan(exp(rQ)) - M_PI_2;
	double ecc = ell.e;
	double rDQ = 0.5 * ecc * log(( 1 + ecc * sin(rPhiApprox))
												 /(1 - ecc * sin(rPhiApprox)));
	double rPhiNext = 2.0 * atan(exp(rQ + rDQ)) - M_PI_2;
	int iCount = 0;
	while (abs(rPhiNext - rPhiApprox) > EPS10) {
	  rPhiApprox = rPhiNext;
	  double rEccSin = ecc * sin(rPhiNext);
	  rDQ = 0.5 * ecc * log(( 1 + rEccSin )/(1 - rEccSin));
	  rPhiNext = 2.0 * atan(exp(rQ + rDQ)) - M_PI_2;
	  iCount++;
	  if (iCount > 10) return pl;
	}
	pl.Phi = rPhiNext;							// (geodetic) latitude on ellipsoid (approximated)
	pl.Lam = asin(sinDL)/rNgauss ; /// + lam0;
	return pl;
}

Datum* ProjectionDutchRD::datumDefault() const
{
  return new MolodenskyDatum("Rijks Driehoeksmeting", "");
}





