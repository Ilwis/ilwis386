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
/* Datum
Copyright Ilwis System Development ITC
march 1996, by Wim Koolhoven
Last change:  WK   22 Mar 96   10:07 am
*/

#ifndef ILWDATUM_H
#define ILWDATUM_H
#include "Engine\SpatialReference\Ellips.h"

class _export Datum
{
public:
	Datum(const String& sName, const String& sArea);
	Datum(Ellipsoid, double dx, double dy, double dz);

	static Ellipsoid ellWGS84();

	LatLon llFromWGS84(const LatLon& ll) { return llhFromWGS84(ll); }
	LatLon llToWGS84(const LatLon& ll) { return llhToWGS84(ll); }
	virtual LatLonHeight llhFromWGS84(const LatLonHeight&) = 0;
	virtual LatLonHeight llhToWGS84(const LatLonHeight&) = 0;
	virtual bool fEqual(const Datum*) const;

	virtual void Store(const FileName& fnCsy);
	String sName();
	String sType();
	virtual String getIdentification(bool wkt=false);

	Ellipsoid ell;
	String sDescription;
	String sArea;
	double dx, dy, dz;

	static String WKTToILWISName(const String& wkt);

protected:
	String m_sName;
	String m_sType;
	static map<String, String> wktToIlwis;
	String identification;
};

class _export MolodenskyDatum: public Datum
{
	friend class CoordSysViaLatLonView;
public:
	MolodenskyDatum(const String& sName, const String& sArea);
	MolodenskyDatum(Ellipsoid, double dx, double dy, double dz);

	LatLonHeight llhFromWGS84(const LatLonHeight&);
	LatLonHeight llhToWGS84(const LatLonHeight&);

	virtual bool fEqual(const Datum*) const;

private:
	static LatLonHeight llhMolodensky(const LatLonHeight& llh,
		const Ellipsoid&,
		double da, double df, 
		double dx, double dy, double dz);
};

class _export BursaWolfDatum : public Datum
{
	friend class CoordSysViaLatLonView;
public:
	BursaWolfDatum(Ellipsoid e, double dX, double dY, double dZ, 
	double rotX, double rotY, double rotZ, double dS);
	virtual void Store(const FileName& fnCsy);
	virtual bool fEqual(const Datum*) const;
	LatLonHeight llhFromWGS84(const LatLonHeight&);
	LatLonHeight llhToWGS84(const LatLonHeight&);

protected:
	double m_rotX;
	double m_rotY;
	double m_rotZ;
	double m_dS;
};

class _export BadekasDatum : public BursaWolfDatum
{
	friend class CoordSysViaLatLonView;
public:
	BadekasDatum(Ellipsoid e, double dX, double dY, double dZ, 
	double rotX, double rotY, double rotZ, double dS,
	double X0, double Y0, double Z0);
	virtual void Store(const FileName& fnCsy);
	virtual bool fEqual(const Datum*) const;
	LatLonHeight llhFromWGS84(const LatLonHeight&);
	LatLonHeight llhToWGS84(const LatLonHeight&);

protected:
	double m_X0;
	double m_Y0;
	double m_Z0;
};

#endif // ILWDATUM_H




