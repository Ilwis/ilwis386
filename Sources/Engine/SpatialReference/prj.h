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
/* Projection, ProjectionPtr
Copyright Ilwis System Development ITC
march 1996, by Wim Koolhoven
Last change:  WK    9 Apr 98   11:09 am
*/

#ifndef ILWPRJ_H
#define ILWPRJ_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\DATUM.H"

class DATEXPORT Projection;
class DATEXPORT ProjectionPtr;

enum ProjectionParamType { ppNONE, ppLAT, ppLON, ppLATLON, ppCOORD, 
ppINT, ppREAL, ppBOOL, ppANGLE, ppSCALE,
ppPOSREAL, ppACUTANGLE, ppZONE };

enum ProjectionParamValue { pvNONE, pvX0, pvY0, pvLON0,
pvLATTS, pvLAT0, pvK0, pvNORTH,
pvZONE, pvLAT1, pvLAT2, pvHEIGHT, 
pvTILTED, pvTILT, pvAZIMYAXIS, pvAZIMCLINE,
pvPOLE, pvNORIENTED, pvLAST };

struct XY      // X and Y in radians
{
	double x, y;  
	XY(): x(rUNDEF), y(rUNDEF) {}
	bool fUndef() { return x == rUNDEF || x == rUNDEF; }
};

struct PhiLam  // Phi and Lam in radians
{
	double Phi, Lam;
	PhiLam(): Phi(rUNDEF), Lam(rUNDEF) {}
	bool fUndef() { return Phi == rUNDEF || Lam == rUNDEF; }
	void AdjustLon();
};      

class DATEXPORT ProjectionPtr: public IlwisObjectPtr
{
	friend class Projection;
	friend class GeoTiffExporter;
	static ProjectionPtr* create(const FileName&, const Ellipsoid&);
	static ProjectionPtr* create(const String& sName, const Ellipsoid&);
public:
	virtual void _export Prepare();
	//virtual bool fPrepare(ErrorObject&);
	virtual void _export Store();
	virtual String _export sName(bool fExt = false, const String& sDirRelative = "") const;
	virtual String _export sType();
	bool fEllipsoid() const { return fEll; }
	bool fLatLon2Coord() const { return f2Coord; }
	bool fCoord2LatLon() const { return f2LatLon; }
	virtual XY xyConv(const PhiLam&) const;
	virtual PhiLam plConv(const XY&) const;
	bool fEqual(const IlwisObjectPtr & ptr) const;
	virtual String _export getIdentification(bool wkt=false);

	String _export sParamName(ProjectionParamValue) const;
	String _export sParamUiName(ProjectionParamValue) const;
	ProjectionParamType _export ppParam(ProjectionParamValue) const;
	ProjectionParamType _export ppAllParam(ProjectionParamValue pv) const;
	long   _export iParam(ProjectionParamValue) const;   // ppProjectionParamValue
	double _export rParam(ProjectionParamValue) const;   // ppLAT, ppLON, ppREAL, ppANGLE ,ppACUTANGLE
	LatLon llParam(ProjectionParamValue) const;  // ppLATLON
	Coord  cParam(ProjectionParamValue) const;   // ppCOORD
	virtual void _export Param(ProjectionParamValue, long);
	virtual void _export Param(ProjectionParamValue, double);
	virtual void _export Param(ProjectionParamValue, const LatLon&);
	virtual void _export Param(ProjectionParamValue, const Coord&);

	virtual Datum _export *datumDefault() const;
	static Projection _export WKTToILWISName(const String& wkt);
	const Ellipsoid& ell;
	double x0, y0, lam0;  // used by CoordSystemPtr
	virtual long _export iMaxZoneNr() const;
	virtual String _export sInvalidZone() const;
	virtual long iGetZoneNr() const;
	virtual bool fGetHemisphereN() const;
	virtual double rGetCentralScaleFactor() const;
	bool fUseParam[pvLAST];
protected:
	_export ProjectionPtr(const FileName&, const Ellipsoid&);
	_export ProjectionPtr(const Ellipsoid&);
	ProjectionPtr();
	bool f2Coord, f2LatLon, fEll;
	double phi0, phits;        // latitude of map origin, latitude of true scale 
	double rPhi1, rPhi2;       // standard parallel latitudes
	double k0;                 // scale at central meridian
	long iNr;                  // zone number for UTM  prj
	bool fNorth;               // Northern hemisphere is selected 
	bool fNoriented;           // Y-axis of CSYSTEM is North-Oriented, Oblique Mercator
	double rAzimCLine;             // azimuth of central line ('great circle' of true scale in Obl Merc
	double rHeight, rTilt, rAzimYaxis;  //  parameters for General Perspective prj
	bool fTilted;              // General Perspective is projected on Tilted plane

	double rHypot(double x, double y) const;
	double _export msfn(double phi) const;
	double _export tsfn(double phi) const;
	double _export qsfn(double phi) const;
	double _export phi2(double ts) const;
	double _export phi1(double qs) const;
	void _export enfn(double* en) const;
	double _export mlfn(double phi, double sphi, double cphi, const double *en) const;
	double _export invmlfn(double arg, double es, const double *en) const;
	void _export authset(double *APA) const;
	double _export authlat(double beta, const double *APA) const;
	void _export InvalidLat();

	void _export InvalidUtmZone();
	void _export InvalidGaussBoagaZone();
	void _export InvalidGKColombiaZone();
	void _export InvalidGKGermanyZone();
	void _export InvalidLaFranceZone();

	void _export InvalidStandardParallels();
private:
	void init();  
	String _sName;
	String identification;
};

class DATEXPORT Projection: public IlwisObject
{
	static IlwisObjectPtrList listPrj;
public:  
	_export Projection();
	_export Projection(const FileName& fn);
	_export Projection(const String& sName);
	_export Projection(const FileName& fn, const Ellipsoid&);
	_export Projection(const String& sName, const Ellipsoid&);
	_export Projection(const Projection& prj);
	void operator = (const Projection& prj) { SetPointer(prj.pointer()); }
	ProjectionPtr* ptr() const { return static_cast<ProjectionPtr*>(pointer()); }
	ProjectionPtr* operator -> () const { return ptr(); }
};

#endif // ILWPRJ_H



