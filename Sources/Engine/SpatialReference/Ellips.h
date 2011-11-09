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
/* Ellipsoid
Copyright Ilwis System Development ITC
march 1996, by Wim Koolhoven
Last change:  WK    8 Apr 98   10:40 am
*/

#ifndef ILWELLIPSOID_H
#define ILWELLIPSOID_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include <Geos.h>

class DATEXPORT Ellipsoid;

struct CoordCTS	: public Coordinate
	//
	// CoordCTS is the metric coordinates in the Conventional Terrestrial System
	// of a geographical position. The origin is in the mass centre of the earth
	// with the X and Y axes in the plane of the equator. The X-axis passes through
	// the meridian of Greenwich, and the Z-axis coincides with the earth's axis of
	// rotation. The three axes are mutually othogonal and form a right-handed
	// system
	//
{
	CoordCTS() : Coordinate(rUNDEF,rUNDEF,rUNDEF) {}
	CoordCTS(double x, double y, double z): Coordinate(x,y,z) {};
};

struct LatLonHeight : public LatLon
	//
	// LatLon is the geographic coordinates in Ellipsiodal Geographic Coordinates
	// of a geographic position plus the height measured along the ellipsoid normal
	// from the point to the ellipsoid surface
	//
{
	double rHeight;
	LatLonHeight(): rHeight(0) {}
	LatLonHeight(const LatLon& ll, double height = 0) 
		: LatLon(ll), rHeight(height) {}
	LatLonHeight(double lat, double lon, double height) 
		: LatLon(lat, lon), rHeight(height) {}
};

class DATEXPORT Ellipsoid 
{
public:
	_export Ellipsoid();
	_export Ellipsoid(double a, double f1);
	_export Ellipsoid(const String&);
	static void GetEllipsoids(char* sBuf, int iSize);

	bool fSpherical() const { return 0 == e; }
	bool operator == (const Ellipsoid& ell) const
	{ return fEqual(ell); }
	bool operator != (const Ellipsoid& ell) const
	{ return !fEqual(ell); }
	bool _export fEqual(const Ellipsoid&) const;
	String sName;
	double a, b, e, e2, f;
	virtual String _export getIdentification(bool wkt=false);

	LatLonHeight llhConv(const Ellipsoid& ell, const LatLonHeight& llh) const;

	_export CoordCTS ctsConv(const LatLonHeight&) const;
	_export LatLonHeight llhConv(const CoordCTS&) const;
	void InvalidEllipsoidDef();
	double _export rEllipsoidalDistance(const LatLon& llBegin, const LatLon& llEnd) const;
	double _export rEllipsoidalAzimuth(const LatLon& llBegin, const LatLon& llEnd) const;
	_export CoordCTS ctsConv(const CoordCTS& ctsIn,	const CoordCTS& ctsPivot,
		double tx, double ty, double tz,
		double Rx, double Ry, double Rz,
		double s) const;//3D linearized similarity transf

private:  
	void init(double a, double f1);
	String identification;
};






#endif //ILWELLIPSOID_H




