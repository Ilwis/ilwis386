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

 Created on: 2020-03-16
 ***************************************************************/
/* ProjectionGeoStationarySatelliteGoes
   Copyright Ilwis System Development ITC
   March 2020, by Ambro Gieske and Jan Hendrikse
*/
#include "Engine\SpatialReference\GeoStatSatGoes.h"
#include "Engine\SpatialReference\Ellips.h"

#define EPS10	1.e-10

ProjectionGeoStationarySatelliteGoes::ProjectionGeoStationarySatelliteGoes(const Ellipsoid& ell)
:ProjectionAzimuthal(ell)
{
	fUseParam[pvHEIGHT] = true;
	fUseParam[pvLAT0] = false;
	rHeight = 35786023;
	//ell = new Ellipsoid("CGMS (MeteoSat)");
	phi0 = 0;
	lam0 = -75.0 * M_PI / 180.0;
}
	
void ProjectionGeoStationarySatelliteGoes::Prepare()
{
	a = ell.a; e2 = ell.e2; b = ell.b;
	rDisttoCenter = a + rHeight; //rDista_toCenter
	//double factor = rHeight/a;
	rLatitudeCorrFactor = ell.b * ell.b / a / a;
	rHorizonRadius = rHeight*asin(a / rDisttoCenter)/a; 
	rVisibleHalfArc = acos(a / rDisttoCenter); // arc from Nadir to horizon
}

bool ProjectionGeoStationarySatelliteGoes::fLatLonInVisible(const PhiLam pl) const
{
	return acos(cos(pl.Phi)*cos(pl.Lam)) > rVisibleHalfArc;
}

XY ProjectionGeoStationarySatelliteGoes::xyConv(const PhiLam& _pl) const
{
	XY xy;
	PhiLam pl = _pl;
	double tangentLength = sqrt(sqr(rDisttoCenter) - sqr(a));
	double rArcfromNadir = acos(rSinPhi0*sin(pl.Phi) + rCosPhi0*cos(pl.Phi)*cos(pl.Lam));
	if (rArcfromNadir > acos(a / rDisttoCenter)) //visiblehalfarc
		return xy;  // point is not visible from satellite
	double rGeoCentricLat = atan(rLatitudeCorrFactor * tan (pl.Phi)); //
	double rCosCLat = cos(rGeoCentricLat);
	double rSinCLat = sin(rGeoCentricLat);
	double RL = ell.b / sqrt ( 1 - e2 * rCosCLat * rCosCLat);
	double r1 = rDisttoCenter - RL * rCosCLat * cos(pl.Lam);
	double r2 = RL * rCosCLat * sin(pl.Lam);
	double r3 = RL * rSinCLat;
	double  rn = sqrt( r1*r1 + r2*r2 + r3*r3);
	//view angles of pl-(input)point seen from satellite
	xy.x =  asin(r2 / rn) * k0 * rHeight;// pl-input projected metrically
	xy.y =  atan(r3 / r1) * k0 * rHeight;// on plane tangent at nadir
	xy.x /= a; // reduced to unit sphere scale 
	xy.y /= a;
	return xy;
}

PhiLam ProjectionGeoStationarySatelliteGoes::plConv(const XY& _xy) const
{
	PhiLam pl;
	XY xy = _xy;
	double a2overb2 = 1 / (1 - e2);
	double sinx = sin(xy.x / k0 / rHeight * a); 
	double cosx = cos(xy.x / k0 / rHeight * a);
	double siny = sin(xy.y / k0 / rHeight * a);
	double cosy = cos(xy.y / k0 / rHeight * a);
	double sin2x = sinx*sinx;
	double cos2x = cosx*cosx;
	double sin2y = siny*siny;
	double cos2y = cosy*cosy;
	double t1 = sqr(rDisttoCenter*cosx*cosy);
	double tangentLength2 = sqr(rDisttoCenter) - sqr(a);
	// length of tangent-line from satellite to earth
	double t2 = (sin2x+cos2x*(cos2y + a2overb2 * sin2y)) * tangentLength2;//1737121856;//
	if (t1 < t2)
		return PhiLam();
	double SD = sqrt(t1 - t2);
	double SN = (rDisttoCenter*cosx*cosy - SD)/(cos2y + a2overb2 * sin2y);
	double s1 =  rDisttoCenter - SN * cosx*cosy;
	double s2 = SN * sinx;
	double s3 = SN * siny * cosx;
	double sxy = sqrt(s1*s1 + s2*s2);
	pl.Lam = atan(s2/s1);
	pl.Phi = atan(a2overb2 * s3 / sxy);
	return pl;
}
