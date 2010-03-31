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
/* Hungarian EOV
   Copyright Ilwis System Development ITC
	 march 2006, by  Jan Hendrikse
	Last change:  JH    19 March 06    2:04 pm
*/

#include "Engine\SpatialReference\EOV_Hungary.h"
#define  EPS10 1.e-10
const double  rBOLSTRA = 6379743.001;
const double  rN = 1.000719704936; 
const double  rK = 1.003110007693;

ProjectionEOVHungary::ProjectionEOVHungary(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
	fEll = true;
	fUseParam[pvX0] = false;
	fUseParam[pvY0] = false;
	fUseParam[pvLON0] = false;
	x0 = 650000;//EOV uses an opposite XY convention
	y0 = 200000;// easting = Y; northing = X
	k0 = 0.99993;
	phi0 = (47.1) * M_PI / 180; //central (geodetic) latitude on ellipsoid 
  lam0 = (19 + 2.0/60.0 + 54.8584/3600.0) * M_PI / 180;
}


XY ProjectionEOVHungary::xyConv(const PhiLam& pl) const
{
	XY xy;
	double rPhi, rLam; // 'conformal' latlon
	double ecc = ell.e;
  double eccsinPhi = ecc * sin(pl.Phi); //(geodetic) latitude on ellipsoid
	double rLog1 = log(rK)+rN*log(tan(M_PI_4 + pl.Phi/2));
	double rLog2 = 0.5 * rN * ecc * log(( 1 - eccsinPhi)/(1 + eccsinPhi));
	double rLogBracket = rLog1 + rLog2;
	double rBracket = exp(rLogBracket);
	rPhi = 2*atan(rBracket) - M_PI_2;
	rLam = rN * pl.Lam;

	double rPhiS, rLamS; //latlon "on rotated sphere"
	double sinPhiS = sin(rPhi) * cos(phi0) - cos(rPhi) * sin(phi0) * cos(rLam);
	rPhiS = asin(sinPhiS);
	double sinLamS = cos(rPhi) * sin(rLam) / cos(rPhiS);
	rLamS = asin(sinLamS);
	
	xy.y = (rBOLSTRA /ell.a) * k0 * log(tan(M_PI_4+rPhiS/2));
	xy.x = (rBOLSTRA /ell.a) * k0 * rLamS;// adapted unit sphere
	return xy;
}

PhiLam ProjectionEOVHungary::plConv(const XY& xy) const
{
	PhiLam pl;
	double rPhiS, rLamS; //latlon "on rotated sphere"
	double ecc = ell.e;
	double R = rBOLSTRA/ell.a; // adapted unit sphere
	rPhiS = 2*atan(exp(xy.y/R/k0)) - M_PI_2;
	rLamS = xy.x / R / k0;
	double rPhi;
	double sinPhi = sin(rPhiS) * cos(phi0) + cos(rPhiS) * sin(phi0) * cos(rLamS);
	rPhi = asin(sinPhi);
	double sinLam = cos(rPhiS) * sin(rLamS) / cos(rPhi);
	double rLam = asin(sinLam);
	//iterative computation of ellipsoidal phi:
	double eccsinPhi = ecc * sin(phi0);
	double logdenom = rN*ecc/2*(log(rK)+log((1 + eccsinPhi)/(1 - eccsinPhi)));
	double logRoot = log(tan(M_PI_4 + phi0/2))/rN;
	double rPhiNext = 2*atan(exp(logRoot)) - M_PI_2;
	double rPhiStart = rPhiNext - 1; // just to get started
	int iCount = 0;
	while (abs(rPhiNext - rPhiStart) > EPS10) {
	  rPhiStart = rPhiNext;
	  eccsinPhi = ecc * sin(rPhiNext);
	  logdenom = log(rK)+rN*ecc/2*log((1 - eccsinPhi)/(1 + eccsinPhi));
	  logRoot = (log(tan(M_PI_4 + rPhi/2))-logdenom)/rN;
	  rPhiNext = 2*atan(exp(logRoot)) - M_PI_2;
	  iCount++;
	  if (iCount > 100) return pl;
	}
	pl.Phi = rPhiNext;							// (geodetic) latitude on ellipsoid (approximated)
	pl.Lam = rLam / rN; /// + lam0;
	return pl;
}

Datum* ProjectionEOVHungary::datumDefault() const
{
  return new MolodenskyDatum("Hungarian Datum 1972 (HD72)", "");
}





