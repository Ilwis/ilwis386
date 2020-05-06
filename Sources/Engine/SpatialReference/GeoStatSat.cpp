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
#include "Engine\SpatialReference\GeoStatSat.h"
#include "Engine\SpatialReference\Ellips.h"

#define EPS10	1.e-10

ProjectionGeoStationarySatellite::ProjectionGeoStationarySatellite(const Ellipsoid& ell)
:ProjectionAzimuthal(ell)
{
	fUseParam[pvHEIGHT] = true;
	fUseParam[pvLAT0] = false;
	rHeight = 35785831;
	//ell = new Ellipsoid("CGMS (MeteoSat)");
	phi0 = 0;
}
	
void ProjectionGeoStationarySatellite::Prepare()
{
	a = ell.a; e2 = ell.e2; b = ell.b;
	rDisttoCenter = a + rHeight; //rDista_toCenter
	//double factor = rHeight/a;
	rLatitudeCorrFactor = ell.b * ell.b / a / a;
	rHorizonRadius = rHeight*asin(a / rDisttoCenter)/a; 
	rVisibleHalfArc = acos(a / rDisttoCenter); // arc from Nadir to horizon
}

bool ProjectionGeoStationarySatellite::fLatLonInVisible(const PhiLam pl) const
{
	return acos(cos(pl.Phi)*cos(pl.Lam)) > rVisibleHalfArc;
}

XY ProjectionGeoStationarySatellite::xyBringOutsideXYtoBorder(const XY& _xy) const
{
	XY xyVisiblePoint = _xy;
	if (_xy.x < -rHorizonRadius) xyVisiblePoint.x = -rHorizonRadius;
	if (_xy.x  > rHorizonRadius) xyVisiblePoint.x = rHorizonRadius;
	if (_xy.y < -rHorizonRadius) xyVisiblePoint.y = -rHorizonRadius;
	if (_xy.y  > rHorizonRadius) xyVisiblePoint.y = rHorizonRadius;
	return xyVisiblePoint;
}
XY ProjectionGeoStationarySatellite::xyConv(const PhiLam& _pl) const
{
		XY xy;
		PhiLam pl = _pl;
	//FORWARD(Meteo Satell); //* ellipsoid 
		double tangentLength = sqrt(sqr(rDisttoCenter) - sqr(a));
		double rArcfromNadir = acos(rSinPhi0*sin(pl.Phi) + rCosPhi0*cos(pl.Phi)*cos(pl.Lam));
		if (rArcfromNadir > acos(a / rDisttoCenter)) //visiblehalfarc
			return xy;  // point is not visible from satellite
		
		//double rLatitudeCorrFactor = 0.993243;// =(b*b/a*a; a/b = 1.003395723344 (for ellipsoid CGMS (MeteoSat)) 
		double rGeoCentricLat = atan(rLatitudeCorrFactor * tan (pl.Phi)); //
		double rCosCLat = cos(rGeoCentricLat);
		double rSinCLat = sin(rGeoCentricLat);
		double RL = ell.b / sqrt ( 1 - e2 * rCosCLat * rCosCLat);
		double r1 = rDisttoCenter - RL * rCosCLat * cos(pl.Lam);
		double r2 = RL * rCosCLat * sin(pl.Lam);
		double r3 = RL * rSinCLat;
		double  rn = sqrt( r1*r1 + r2*r2 + r3*r3);
		//view angles of pl-(input)point seen from satellite
		double lamSat =  atan(r2 / r1); //view angle with 0-meridian
		double phiSat =  asin(r3 / rn); //view angle with equator
		xy.x =  atan(r2 / r1) * k0 * rHeight;// pl-input projected metrically
		xy.y =  asin(r3 / rn) * k0 * rHeight;// on plane tangent at nadir
		xy.x /= a; // reduced to unit sphere scale 
		xy.y /= a;
		return xy;
		
		// TESTCALLS TO CHECK	
		// XY Rectangle that bounds latlonQuadrangle of 10 degrees
		double latMi = pl.Phi;
		double latMa = pl.Phi + M_PI / 18;
		double lonMi = pl.Lam;
		double lonMa = pl.Lam + M_PI / 18;
		double xMi, xMa, yMi, yMa;
		bool fCheck = QuadrangleToXYbox(latMi, latMa, lonMi, lonMa, xMi, xMa, yMi, yMa);
		latMa = pl.Phi;
		latMi = pl.Phi - M_PI / 18;
		lonMa = pl.Lam;
		lonMi = pl.Lam - M_PI / 18;
		fCheck = QuadrangleToXYbox(latMi, latMa, lonMi, lonMa, xMi, xMa, yMi, yMa);
		// END TESTCALLS
		
		return xy;
}


PhiLam ProjectionGeoStationarySatellite::plConv(const XY& _xy) const
{
		PhiLam pl;
		XY xy = _xy;

	//INVERSE(Meteo Satell);
		// ellipsoid 
		double a2overb2 = 1.0068029776257386147594828786943; // a*a/b*b for CGMS (MeteoSat) ellipsoid
		a2overb2 = 1 / (1 - e2);
		double sinx = sin(xy.x / k0 / rHeight * a); 
		double cosx = cos(xy.x / k0 / rHeight * a);
		double siny = sin(xy.y / k0 / rHeight * a);
		double cosy = cos(xy.y / k0 / rHeight * a);
		double sin2y = siny*siny;
		double cos2y = cosy*cosy;
		//double rDisttoCenterINKM = rDisttoCenter/1000;// in km
		double t1 = sqr(rDisttoCenter*cosx*cosy);
		double tangentLength2 = sqr(rDisttoCenter) - sqr(a);
			// length of tangent-line from satellite to earth
		double t2 = (cos2y + a2overb2 * sin2y) * tangentLength2;//1737121856;//
		if (t1 < t2)
			return PhiLam();
		double SD = sqrt(t1 - t2);
		double SN = (rDisttoCenter*cosx*cosy - SD)/(cos2y + a2overb2 * sin2y);
		double s1 =  rDisttoCenter - SN * cosx*cosy;
		double s2 = SN * sinx * cosy;
		double s3 = SN * siny;
		double sxy = sqrt(s1*s1 + s2*s2);
		pl.Lam = atan(s2/s1);
		//double rGeoCentricLat = atan(a2overb2 * s3 / sxy);
		//pl.Phi = atan(tan (rGeoCentricLat) / rLatitudeCorrFactor); //
		pl.Phi = atan(a2overb2 * s3 / sxy);
		return pl;

		// TESTCALLS TO CHECK 	
		// Latlon Quadrangle inside xy rectangle of 1000km by 1000km
		double xMi = xy.x;
		double xMa = xy.x + 1000000/a;
		double yMi = xy.y;
		double yMa = xy.y + 1000000/a;
		double laMi, laMa, loMi, loMa;
		bool fCheck = XYboxToQuadrangle(xMi, xMa, yMi, yMa, laMi, laMa, loMi, loMa);
		// Latlon Quadrangle inside xy rectangle of 100km by 100km
		xMa = xy.x;
		xMi = xy.x - 100000/a;
		yMa = xy.y;
		yMi = xy.y - 100000/a;
		fCheck = XYboxToQuadrangle(xMi, xMa, yMi, yMa, laMi, laMa, loMi, loMa);
		// END TESTCALLS

		return pl;
}

bool ProjectionGeoStationarySatellite::XYboxToQuadrangle
										(double xMin, double xMax,
										double yMin, double yMax,
										double& latMin, double& latMax,
										double& lonMin, double& lonMax) const
{
	XY xyMin;
	xyMin.x = xMin, 
	xyMin.y = yMin;
	XY xyReducedLL = xyBringOutsideXYtoBorder(xyMin);
	XY xyMax;
	xyMax.x = xMax, 
	xyMax.y = yMax;
	XY xyReducedUR = xyBringOutsideXYtoBorder(xyMax);
	if ((xyReducedLL.x == xyReducedUR.x) || (xyReducedLL.y == xyReducedUR.y))
		return false; //selection has zero area
	else
	{
		xyMin = xyReducedLL;
		xyMax = xyReducedUR;
	}
	double rMinimPhi = -rVisibleHalfArc;
	double rMinimLam = -rVisibleHalfArc;
	double rMaximPhi =  rVisibleHalfArc;
	double rMaximLam =  rVisibleHalfArc;
	PhiLam plSouth, plNorth; // 'moving latlons'
	PhiLam plWest, plEast;
	XY xy_Xedge, xy_Yedge;
	plSouth.Phi = rMinimPhi; // initialize increment of south parallel
	plSouth.Lam = 0;
	plNorth.Phi = rMaximPhi; // initialize decrement of north parallel
	plNorth.Lam = 0;
	plWest.Phi = 0;
	plWest.Lam = rMinimLam; // initialize increment of west meridian
	plEast.Phi = 0;
	plEast.Lam = rMaximLam; // initialize decrement of east meridian
	double rXstep = (xyMax.x - xyMin.x)/100;
	double rYstep = (xyMax.y - xyMin.y)/100;
	for (double x = xyMin.x; x < xyMax.x; x += rXstep)
	{
		// run along south-edge
		xy_Xedge.x = x;
		xy_Xedge.y = xyMin.y;  
		if (!plConvAux(xy_Xedge).fUndef())
		{
			plSouth.Phi = max(plConvAux(xy_Xedge).Phi, plSouth.Phi);
			//plWest.Lam = max(plConvAux(xy_Xedge).Lam, plWest.Lam);
			//plEast.Lam = min(plConvAux(xy_Xedge).Lam, plEast.Lam);
		}
		// run along north-edge
		xy_Xedge.y = xyMax.y;
		if (!plConvAux(xy_Xedge).fUndef())
		{
			plNorth.Phi = min(plConvAux(xy_Xedge).Phi, plNorth.Phi);
			//plWest.Lam = max(plConvAux(xy_Xedge).Lam, plWest.Lam);
			//plEast.Lam = min(plConvAux(xy_Xedge).Lam, plEast.Lam);
		}
	}/*
	// south parallel completely south of horizon, but northparall not
	if (plSouth.Phi < rMinimPhi && plNorth.Phi > rMinimPhi)
		plSouth.Phi = rMinimPhi;
	// north parallel completely north of horizon but southparall not
	if (plNorth.Phi > rMaximPhi && plSouth.Phi < rMaximPhi)
		plNorth.Phi = rMaximPhi;
		*/
	for (double y = xyMin.y; y < xyMax.y; y += rYstep)
	{
		xy_Yedge.y = y;
		// run along west-edge
		xy_Yedge.x = xyMin.x;
		if (!plConvAux(xy_Yedge).fUndef())
			plWest.Lam = max(plConvAux(xy_Yedge).Lam, plWest.Lam);
		// run along east-edge
		xy_Yedge.x = xyMax.x;
		if (!plConvAux(xy_Yedge).fUndef())
			plEast.Lam = min(plConvAux(xy_Yedge).Lam, plEast.Lam);
	}/*
	// west meridian completely west of horizon, but eastmeridian not
	if (plWest.Lam < rMinimLam && plEast.Lam > rMinimLam)
		plWest.Lam = rMinimLam;
	// east meridian completely east of horizon, but westmeridian not
	if (plEast.Lam > rMaximLam && plWest.Lam < rMaximLam)
		plEast.Lam = rMaximLam;
	*/
	// inscribed latlon_quadrangle (output):
	latMin = plSouth.Phi; // in radians !!
	lonMin = plWest.Lam;
	latMax = plNorth.Phi;
	lonMax = plEast.Lam;
	double latMinDegr = latMin * 180 / M_PI; // in degrees
	double lonMinDegr = lonMin * 180 / M_PI;
	double latMaxDegr = latMax * 180 / M_PI;
	double lonMaxDegr = lonMax * 180 / M_PI;
	double width = lonMaxDegr - lonMinDegr;
	double height = latMaxDegr - latMinDegr;
	bool fValidQuadrangle =   // at least one shift from initial limits
		!(latMin == rMinimPhi && latMax == rMaximPhi 
		&& lonMin == rMinimLam && lonMax == rMaximLam);
  return fValidQuadrangle;
}

bool ProjectionGeoStationarySatellite::QuadrangleToXYbox
							(double latMin, double latMax,
							double lonMin, double lonMax,
							double& xMin, double& xMax,
							double& yMin, double& yMax) const
{
	// SPHERICAL ASSUMPTION WHEN RETRIEVING BOUNDING RECTANGLE
	//if ((latMin == latMax) || (lonMin == lonMax))
	//	return false; // quadrangle of zero area
	PhiLam plMinMin; plMinMin.Phi = latMin; plMinMin.Lam = lonMin;
	PhiLam plMaxMax; plMaxMax.Phi = latMax; plMaxMax.Lam = lonMax;
	PhiLam plMinMax; plMinMax.Phi = latMin; plMinMax.Lam = lonMax;
	PhiLam plMaxMin; plMaxMin.Phi = latMax; plMaxMin.Lam = lonMin;
	// assume nadir at (lat=0,lon=0) and ellipsoid == sphere
	if (fLatLonInVisible(plMinMin) && fLatLonInVisible(plMaxMax)
			&& fLatLonInVisible(plMinMin) && fLatLonInVisible(plMaxMin))
		return false; // quadrangle completely invisible

	XY xyLL;// = xyConvAux(plMinMin); // lowerleft corner of quadrangle
	XY xyUR;// = xyConvAux(plMaxMax); // upprright corner  
	XY xyLR;// = xyConvAux(plMinMax); // lowrright corner  
	XY xyUL;// = xyConvAux(plMaxMin); // upperleft corner

	XY xySouthEdge, xyNorthEdge, xyWestEdge, xyEastEdge;
	xySouthEdge.x = rHorizonRadius;
	xySouthEdge.y = rHorizonRadius;
	xyNorthEdge.x = -rHorizonRadius;
	xyNorthEdge.y = -rHorizonRadius;
	xyWestEdge.x = rHorizonRadius;
	xyWestEdge.y = rHorizonRadius;
	xyEastEdge.x = -rHorizonRadius;
	xyEastEdge.y = -rHorizonRadius;
	PhiLam plParallel, plMeridian;
	
	double rLonStep = (lonMax - lonMin)/100;
	for (double lo = lonMin; lo < lonMax; lo += rLonStep)
	{
		plParallel.Phi = latMin;
		// run along south-edge
		plParallel.Lam = lo;  
		if (!xyConvAux(plParallel).fUndef())
			xySouthEdge.y = min(xyConvAux(plParallel).y, xySouthEdge.y);
		// run along north-edge
		plParallel.Phi = latMax;
		if (!xyConvAux(plParallel).fUndef())
			xyNorthEdge.y = max(xyConvAux(plParallel).y, xyNorthEdge.y);
	}
	xyLL.y = xySouthEdge.y;
	xyLR.y = xySouthEdge.y;
	xyUL.y = xyNorthEdge.y;
	xyUR.y = xyNorthEdge.y	;

	double rLatStep = (latMax - latMin)/100;
	for (double la = latMin; la < latMax; la += rLatStep)
	{
		plMeridian.Lam = lonMin;
		// run along west-edge
		plMeridian.Phi = la;  
		if (!xyConvAux(plMeridian).fUndef())
			xyWestEdge.x = min(xyConvAux(plMeridian).x, xyWestEdge.x);
		// run along east-edge
		plMeridian.Lam = lonMax;
		if (!xyConvAux(plParallel).fUndef())
			xyEastEdge.x = max(xyConvAux(plMeridian).x, xyEastEdge.x);
	}
	xyLL.x = xyWestEdge.x;
	xyLR.x = xyWestEdge.x;
	xyUL.x = xyEastEdge.x;
	xyUR.x = xyEastEdge.x;

	// bounding box (output):
	xMin = min(xyLL.x, xyUL.x) * a;// bounding box
	xMax = max(xyLR.x, xyUR.x) * a;// at real world scale 
	yMin = min(xyLL.y, xyLR.y) * a;// (meters)
	yMax = max(xyUL.y, xyUR.y) * a;
	double width = xMax - xMin;
	double height = yMax - yMin;
	return true; //Quadrangle is valid i.e. has a computable XYbox
}
/********************** comment ******************/
// XYboxToQuadrangle is not needed if we always deliver the selected rectangle
// including sometimes the completely undefined rows and columns 
// (area outside the horizon)
// But it could be used to return the quadrangle corners as feedback 
// and also in case we want just to deliver 
// the image inside the minimal rectangle AND inside the horizon.

// QuadrangleToXY is needed when the user enters valid minLatLon and maxLatlon,
// (invible latlons are refused in the UInterface)
// so that a containing minimal rectangle can be computed  
// and for its display on screen; the rectangle contents will be delivered.

XY ProjectionGeoStationarySatellite::xyConvAux(const PhiLam& _pl) const
{
		XY xy;
		PhiLam pl = _pl;
	//FORWARD(Meteo Satell); //* ellipsoid 
		double tangentLength = sqrt(sqr(rDisttoCenter) - sqr(a));
		double rArcfromNadir = acos(rSinPhi0*sin(pl.Phi) + rCosPhi0*cos(pl.Phi)*cos(pl.Lam));
		if (rArcfromNadir > acos(a / rDisttoCenter)) //visiblehalfarc
			return xy;  // point is not visible from satellite
		
		//double rLatitudeCorrFactor = 0.993243;// =(b*b/a*a; a/b = 1.003395723344 (for ellipsoid CGMS (MeteoSat)) 
		double rGeoCentricLat = atan(rLatitudeCorrFactor * tan (pl.Phi)); //
		double rCosCLat = cos(rGeoCentricLat);
		double rSinCLat = sin(rGeoCentricLat);
		double RL = ell.b / sqrt ( 1 - e2 * rCosCLat * rCosCLat);
		double r1 = rDisttoCenter - RL * rCosCLat * cos(pl.Lam);
		double r2 = RL * rCosCLat * sin(pl.Lam);
		double r3 = RL * rSinCLat;
		double  rn = sqrt( r1*r1 + r2*r2 + r3*r3);
		//view angles of pl-(input)point seen from satellite
		double lamSat =  atan(r2 / r1); //view angle with 0-meridian
		double phiSat =  asin(r3 / rn); //view angle with equator
		xy.x =  atan(r2 / r1) * k0 * rHeight;// pl-input projected metrically
		xy.y =  asin(r3 / rn) * k0 * rHeight;// on plane tangent at nadir
		xy.x /= a; // reduced to unit sphere scale 
		xy.y /= a;
		return xy;
}

PhiLam ProjectionGeoStationarySatellite::plConvAux(const XY& _xy) const
{
		PhiLam pl;
		XY xy = _xy;

	//INVERSE(Meteo Satell);
		// ellipsoid 
		double a2overb2 = 1.0068029776257386147594828786943; // a*a/b*b for CGMS (MeteoSat) ellipsoid
		a2overb2 = 1 / (1 - e2);
		double sinx = sin(xy.x / k0 / rHeight * a); 
		double cosx = cos(xy.x / k0 / rHeight * a);
		double siny = sin(xy.y / k0 / rHeight * a);
		double cosy = cos(xy.y / k0 / rHeight * a);
		double sin2y = siny*siny;
		double cos2y = cosy*cosy;
		double rDisttoCenterINKM = rDisttoCenter/1000;// in km
		double t1 = sqr(rDisttoCenterINKM*cosx*cosy);
		double tangentLength2 = sqr(rDisttoCenterINKM) - sqr(a/1000);
			// length of tangent-line from satellite to earth
		double t2 = (cos2y + a2overb2 * sin2y) * tangentLength2;//1737121856;//
		double SD = sqrt(t1 - t2);
		double SN = (rDisttoCenterINKM*cosx*cosy - SD)/(cos2y + a2overb2 * sin2y);
		double s1 =  (rDisttoCenterINKM - SN * cosx*cosy) * 1000;
		double s2 = SN * sinx * cosy * 1000;
		double s3 = SN * siny * 1000 ;
		double sxy = sqrt(s1*s1 + s2*s2);
		pl.Lam = atan(s2/s1);
		//double rGeoCentricLat = atan(a2overb2 * s3 / sxy);
		//pl.Phi = atan(tan (rGeoCentricLat) / rLatitudeCorrFactor); //
		pl.Phi = atan(a2overb2 * s3 / sxy);
		return pl;
}
