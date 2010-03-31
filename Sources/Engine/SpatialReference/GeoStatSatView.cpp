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
/* ProjectionGeoStationarySatelliteView
   Copyright Ilwis System Development ITC
    jan 2005, by Jan Hendrikse/ Gerald Evenden
*/
#include "Engine\SpatialReference\GeoStatSatView.h"
#include "Engine\SpatialReference\Ellips.h"

#define EPS10	1.e-10

ProjectionGeoStationarySatelliteView::ProjectionGeoStationarySatelliteView(const Ellipsoid& ell)
:ProjectionAzimuthal(ell)
{
	fUseParam[pvHEIGHT] = true;
	fUseParam[pvLAT0] = false;
	fUseParam[pvK0] = false;
	rHeight = 35785831;
	//ell = new Ellipsoid("CGMS (MeteoSat)");
	phi0 = 0;
}
	
void ProjectionGeoStationarySatelliteView::Prepare()
{
	a = ell.a; e2 = ell.e2;
	rDista = a + rHeight; //rDista_toCenter
	//double factor = rHeight/a;
	rLatitudeCorrFactor = ell.b * ell.b / a / a;
	//rDegLengthEW = a * M_PI / 180;
	//rDegLengthNS = a * (1 - e2) * M_PI / 180;
	radius_g_1 = rHeight / a;
	radius_g = 1. + radius_g_1; // (a+h)/a
	C  = radius_g * radius_g - 1.0;
	if (!ell.fSpherical())
	{
		radius_p      = sqrt (1 - e2);
		radius_p2     = 1 - e2;
		radius_p_inv2 = 1/(1 - e2);
	}
	else
	{
		radius_p = 1.0;
		radius_p2 = 1.0;
		radius_p_inv2 = 1.0;
	}
}


XY ProjectionGeoStationarySatelliteView::xyConv(const PhiLam& _pl) const
{
	XY xy;
	PhiLam pl = _pl;
	double r, Vx, Vy, Vz, tmp;
	if (!ell.fSpherical())
	{
		/* Calculation of geocentric latitude. */
		pl.Phi = atan (radius_p2 * tan (pl.Phi));
		/* Calculation of the three components of the vector from satellite to
		** position on earth surface (lon,lat).*/
		r = (radius_p) / _hypot(radius_p * cos (pl.Phi), sin (pl.Phi));
		Vx = r * cos (pl.Lam) * cos (pl.Phi);
		Vy = r * sin (pl.Lam) * cos (pl.Phi);
		Vz = r * sin (pl.Phi);
		/* Check visibility. */
		if (((radius_g - Vx) * Vx - Vy * Vy - Vz * Vz * radius_p_inv2) < 0.)
			return xy;
				
			//view angles of pl-(input)point seen from satellite
			//double lamSat =  atan(r2 / r1); //view angle with 0-meridian
			//double PhiSat =  asin(r3 / rn); //view angle with equator
			
		/* Calculation based on view angles from satellite. */
		tmp = radius_g - Vx;
		xy.x = radius_g_1 * atan (Vy / tmp);
		xy.y = radius_g_1 * atan (Vz / _hypot (Vy, tmp));
	}
	else
	{
				/* Calculation of the three components of the vector from satellite to
		** position on earth surface (lon,lat).*/
		tmp = cos(pl.Phi);
		Vx = cos (pl.Lam) * tmp;
		Vy = sin (pl.Lam) * tmp;
		Vz = sin (pl.Phi);
		/* Check visibility.*/
		if (((radius_g - Vx) * Vx - Vy * Vy - Vz * Vz) < 0.) 
			return xy;
		/* Calculation based on view angles from satellite.*/
		tmp = radius_g - Vx;
		xy.x = radius_g_1 * atan(Vy / tmp);
		xy.y = radius_g_1 * atan(Vz / _hypot(Vy, tmp));
	}
	return xy;
}


PhiLam ProjectionGeoStationarySatelliteView::plConv(const XY& _xy) const
{
	PhiLam pl;
	XY xy = _xy;
	double Vx, Vy, Vz, a, b, det, k;
	if (!ell.fSpherical())
	{		// ellipsoid 
		/* Setting three components of vector from satellite to position.*/
		Vx = -1.0;
		Vy = tan (xy.x / radius_g_1);
		Vz = tan (xy.y / radius_g_1) * _hypot(1.0, Vy);
		/* Calculation of terms in cubic equation and determinant.*/
		a = Vz / radius_p;
		a   = Vy * Vy + a * a + Vx * Vx;
		b   = 2 * radius_g * Vx;
		if ((det = (b * b) - 4 * a * C) < 0.) return pl;
		/* Calculation of three components of vector from satellite to position.*/
		k  = (-b - sqrt(det)) / (2. * a);
		Vx = radius_g + k * Vx;
		Vy *= k;
		Vz *= k;
		/* Calculation of longitude and latitude.*/
		pl.Lam  = atan2 (Vy, Vx);
		pl.Phi = atan (Vz * cos (pl.Lam) / Vx);
		pl.Phi = atan (radius_p_inv2 * tan (pl.Phi));
	}
	else
	{
		/* Setting three components of vector from satellite to position.*/
		Vx = -1.0;
		Vy = tan (xy.x / (radius_g - 1.0));
		Vz = tan (xy.y / (radius_g - 1.0)) * sqrt (1.0 + Vy * Vy);
		/* Calculation of terms in cubic equation and determinant.*/
		a   = Vy * Vy + Vz * Vz + Vx * Vx;
		b   = 2 * radius_g * Vx;
		if ((det = (b * b) - 4 * a * C) < 0.) return pl;
		/* Calculation of three components of vector from satellite to position.*/
		k  = (-b - sqrt(det)) / (2 * a);
		Vx = radius_g + k * Vx;
		Vy *= k;
		Vz *= k;
		/* Calculation of longitude and latitude.*/
		pl.Lam = atan2 (Vy, Vx);
		pl.Phi = atan (Vz * cos (pl.Lam) / Vx);
	}
	return pl;
}

//Datum* ProjectionGeoStationarySatellite::datumDefault() const
//{
//  return new MolodenskyDatum("CGMS Datum (MeteoSat)", "");
//}  Not only the MSG satellite is geostationary, but algorithm is from MSG docs 
