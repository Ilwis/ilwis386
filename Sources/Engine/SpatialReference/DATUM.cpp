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
/* $Log: /ILWIS 3.0/CoordinateSystem/DATUM.cpp $
 * 
 * 12    3-10-03 22:01 Hendrikse
 * gave corretc signs to BursaWolf and Badekas params in llhFromWGS84 and
 * llhToWGS84 conversion functions
 * 
 * 11    30-09-03 10:14 Hendrikse
 * Debug: corrected 2X a typing mistake: m_RotZ replaced by m_RotY 
 * 
 * 10    26-09-03 17:27 Hendrikse
 * implemented the use of  method  String sType() and its protected
 * associated member 	String m_sType in class Datum  'write to' and 'read
 * from' the ODF (the *.csy file.) the types Bursa-Wolf and
 * Molodensky-Badkas in the [Datum] section. (see store functions)
 * Implemented llhFromWGS84() and llhToWGS84 for both new Datum types.
 * 
 * 9     15-09-03 11:31 Hendrikse
 * added llhFromWGS84(const LatLonHeight& llh) and llhToWGS84(const
 * LatLonHeight& llh) for derived Datums
 * Implemented the constructors and other functions of the newly derived
 * Datum classes
 * 
 * 8     25-08-03 18:52 Hendrikse
 * MolodenskyDatum::Datum is now used in st of Datum
 * 
 * 7     28-03-01 18:25 Koolhoven
 * protected constructor against empty string for ellipsoid
 * instead of strange macro
 * 
 * 6     15/06/00 11:40 Willem
 * Added check for length of sArea in Datum constructor, before calling
 * GetPrivetProfileString
 * 
 * 5     3-06-00 16:16 Hendrikse
 * Implemented datum shifts with  doubles dx, dy, dz;
 * to cope with high precision shift info
 * Made a String sShifts; to display the shifts in the Change Datum form
 * 
 * 4     23-06-99 12:22p Martin
 * changed assert
 * 
 * 3     24-03-99 9:49 Koolhoven
 * Header comment
 * 
 * 2     15-03-99 16:03 Willem
 * Updated for redefinition of Assertion function
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/13 11:09:47  Wim
// Do not allow invalid areas anymore
//
/* Datum
   Copyright Ilwis System Development ITC
   march 1996, by Wim Koolhoven
	Last change:  WK   13 Aug 97    1:07 pm
*/

#include "Engine\SpatialReference\DATUM.H"
#include "Engine\Base\AssertD.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

map<String,String> Datum::wktToIlwis;

void Datum::Store(const FileName& fnCsy)
{
	// Clear the following sections
	ObjectInfo::WriteElement("CoordSystem", "Datum", fnCsy, (char*)NULL);
  ObjectInfo::WriteElement("CoordSystem", "Datum Area", fnCsy, (char*)NULL);
  ObjectInfo::WriteElement("CoordSystem", "Ellipsoid", fnCsy, (char*)NULL);
  ObjectInfo::WriteElement("CoordSystem", "Sphere Radius", fnCsy, (char*)NULL);

	// (Re)write datum info
	ObjectInfo::WriteElement("CoordSystem", "Datum", fnCsy,  sName());
	if (fCIStrEqual("User Defined" , m_sName.sSub(0,12) )) //.substr(12)))
	{
		ObjectInfo::WriteElement("Datum", "dx", fnCsy, dx);
		ObjectInfo::WriteElement("Datum", "dy", fnCsy, dy);
		ObjectInfo::WriteElement("Datum", "dz", fnCsy, dz);      

    ObjectInfo::WriteElement("CoordSystem", "Ellipsoid", fnCsy, ell.sName);
    if (fCIStrEqual("User Defined" , ell.sName.sSub(0,12) ))
    {
      ObjectInfo::WriteElement("Ellipsoid", "a", fnCsy, ell.a);
      ObjectInfo::WriteElement("Ellipsoid", "1/f", fnCsy, 1/ell.f);
    }
    else if (ell.fSpherical())
      ObjectInfo::WriteElement("CoordSystem", "Sphere Radius", fnCsy, ell.a);
	}
	else
		ObjectInfo::WriteElement("CoordSystem", "Datum Area", fnCsy, sArea);
}

String Datum::WKTToILWISName(const String& wkt) {
	if (wktToIlwis.size() == 0 ) {
		char line[1024];
		String sPath = getEngine()->getContext()->sIlwDir();
		sPath &= "\\Resources\\Def\\datum.def";
		FILE* fp = fopen(sPath.scVal(),"r");
		while (fgets(line,1024,fp)){
			if (*line == 0) break;
			String sLine(line);
			String sWKTName = "";
			String ilwisName = sLine.sHead("=");
			Array<String> parts;
			SplitOn(sLine.sTail("="), parts, ",");
			if ( parts.iSize() == 1 ) {
				sWKTName = parts[0];
			}
			else if (parts.iSize() == 3) {
				sWKTName = parts[2];
			} else if ( parts.iSize() == 6) {
				sWKTName = parts[5];
			}
			if ( sWKTName != "") {
				sWKTName = sWKTName.sLeft(sWKTName.length() - 1);
				wktToIlwis[sWKTName] = ilwisName;
			}

			if ( ilwisName == "Zanderij" )
				break;
		}
		fclose(fp);
	}


	String s = wktToIlwis[wkt];
	if ( wktToIlwis.find(wkt) != wktToIlwis.end())
		return wktToIlwis[wkt];



	return "";
}

bool Datum::fEqual(const Datum* datum) const
{
  return 
		dx == datum->dx && 
		dy == datum->dy && 
    dz == datum->dz;
}

Ellipsoid Datum::ellWGS84()
{
	 return Ellipsoid(6378137.0, 298.257223563);
}

Datum::Datum(const String& sNam, const String& sAr)
: m_sName(sNam), sArea(sAr)
{
}

Datum::Datum(Ellipsoid e, double deltaX, double deltaY, double deltaZ)
: m_sName("User Defined"), ell(e), dx(deltaX), dy(deltaY), dz(deltaZ)
{
}

String Datum::sName()
{
	return m_sName;
}

String Datum::sType()
{
	return m_sType;
}

//-------------- Molodensksy
MolodenskyDatum::MolodenskyDatum(const String& sN, const String& sA)
	: Datum(sN, sA)
{
  String sPath = getEngine()->getContext()->sIlwDir();
  sPath &= "\\Resources\\Def\\datum.def";
  char sBuf1[1024];
  char sBuf2[1024];
  char sDesc[1024];
  sDesc[0] = 0;
  String sEll;
  char* s;
  dx = dy = dz = 0;
  Array<String> parts;

	String sShifts = "";
	if (GetPrivateProfileString("Datums", sName().scVal(), "", sBuf1, 1024, sPath.scVal())) {
		String line(sBuf1);
		SplitOn(line, parts, ",");
		ell = Ellipsoid(parts[0]);
		if ( parts.iSize() == 2) {
			sDescription = parts[1];
		} else if (parts.iSize() == 3) {
			sDescription = parts[1];
		} else if (parts.iSize() > 3 ) {
			dx = parts[1].rVal();
			dy = parts[2].rVal();
			dz = parts[3].rVal();
			sDescription = parts[4];
		}
	}    
	if (sArea.length() > 0 && GetPrivateProfileString(sName().scVal(), sArea.scVal(), "", sBuf2, 1024, sPath.scVal())) {
		for (s = sBuf2; *s; ++s)
			if (isspace(*s)) *s = '_';
		sscanf(sBuf2, "%lf,%lf,%lf,%s", &dx, &dy, &dz, sDesc);
		if (0 != dx || 0 != dy || 0 != dz) 
			sShifts &= String(" SHIFTS:%.2f,%.2f,%.2f",dx,dy,dz);
  }
  else
    sArea = String("");
  for (s = sDesc; *s; ++s)
    if (*s == '_') *s = ' ';
  sDescription = sDesc; 
	sDescription &= sShifts;
	if ("" != sEll)
		ell = Ellipsoid(sEll);
}

MolodenskyDatum::MolodenskyDatum(Ellipsoid e, double dX, double dY, double dZ)
: Datum(e, dX, dY, dZ)
{
}

bool MolodenskyDatum::fEqual(const Datum* datum) const
{
	return Datum::fEqual(datum);
}

LatLonHeight MolodenskyDatum::llhFromWGS84(const LatLonHeight& llh)
{
	Ellipsoid ellWGS84 = Datum::ellWGS84();
  return llhMolodensky(llh, ellWGS84,
                    ell.a - ellWGS84.a, ell.f - ellWGS84.f,
                    -dx, -dy, -dz);
}

LatLonHeight MolodenskyDatum::llhToWGS84(const LatLonHeight& llh)
{
	Ellipsoid ellWGS84 = Datum::ellWGS84();
  return llhMolodensky(llh, ell,
                       ellWGS84.a - ell.a, ellWGS84.f - ell.f,
                       dx, dy, dz);
}

// Molodensky datum transformation
// DMA Technical Report
// Department of Defense
// World Geodetic System 1984
// Its definition and relationships with local geodetic systems
// table 7.2, page 7-8/7-9        
LatLonHeight MolodenskyDatum::llhMolodensky(const LatLonHeight& llh, 
                const Ellipsoid& ell,
                double da, double df, 
                double dx, double dy, double dz)
{
  double phi = llh.Phi();
  double lam = llh.Lambda();
  double h = llh.rHeight;
  double sinPhi = sin(phi);
  double cosPhi = cos(phi);
  double sinLam = sin(lam);
  double cosLam = cos(lam);
  double sin2Phi = sinPhi * sinPhi;

  // n = radius of curvature in the prime vertical
  double n = ell.a / sqrt(1 - ell.e2 * sin2Phi);
  // m = radius of curvature in the meridian
  double rTmp = 1 - ell.e2 * sin2Phi;
  double m = ell.a * (1 - ell.e2) / sqrt(rTmp * rTmp * rTmp);
  
  double dPhi, dLam, dh;
  dPhi = - dx * sinPhi * cosLam - dy * sinPhi * sinLam + dz * cosPhi 
         + da * (n * ell.e2 * sinPhi * cosPhi) / ell.a
         + df * (m * ell.a / ell.b + n * ell.b / ell.a) * sinPhi * cosPhi;
  dPhi /= m + h;
  
  dLam = (-dx * sinLam + dy * cosLam) / ((n + h) * cosPhi);
  
  dh =   dx * cosPhi * cosLam + dy * cosPhi * sinLam + dz * sinPhi
       - da * ell.a / n + df * n * sin2Phi * ell.b / ell.a;

  phi += dPhi;
  lam += dLam;
  h += dh;
  LatLonHeight llhRes;
  llhRes.Phi(phi);
  llhRes.Lambda(lam);
  llhRes.rHeight = h;           
  return llhRes;     
}                

BursaWolfDatum::BursaWolfDatum(Ellipsoid e, double dX, double dY, double dZ, 
															              double rotX, double rotY, double rotZ, double dS)
: Datum(e, dX, dY, dZ)
{
	m_sType = "User Defined BursaWolf";
	m_rotX = rotX;
	m_rotY = rotY;
	m_rotZ = rotZ;
	m_dS   = dS;
}

void BursaWolfDatum::Store(const FileName& fnCsy)
{
	Datum::Store(fnCsy);
	String sRotX = String("%.10f",m_rotX);
	String sRotY = String("%.10f",m_rotY);
	String sRotZ = String("%.10f",m_rotZ);
	String sDs = String("%.10f",m_dS);
	ObjectInfo::WriteElement("Datum", "Type", fnCsy, sType());
	ObjectInfo::WriteElement("Datum", "rotX", fnCsy, sRotX);
	ObjectInfo::WriteElement("Datum", "rotY", fnCsy, sRotY);
	ObjectInfo::WriteElement("Datum", "rotZ", fnCsy, sRotZ);
	ObjectInfo::WriteElement("Datum", "dS",   fnCsy, sDs);
}

bool BursaWolfDatum::fEqual(const Datum* datum) const
{
	const BursaWolfDatum* bursawolfDatum = dynamic_cast<const BursaWolfDatum*>(datum);
  if (0 == bursawolfDatum)
		return false;
  return 
		m_rotX == bursawolfDatum->m_rotX && 
		m_rotY == bursawolfDatum->m_rotY && 
    m_rotZ == bursawolfDatum->m_rotZ &&
		m_dS == bursawolfDatum->m_dS &&
		Datum::fEqual(datum); 
}

LatLonHeight BursaWolfDatum::llhFromWGS84(const LatLonHeight& llh)
{
	Ellipsoid ellWGS84 = Datum::ellWGS84();
	CoordCTS ctsIn, ctsOut;
	CoordCTS ctsPivot(0,0,0);
	ctsIn = ellWGS84.ctsConv(llh);
	ctsOut = ell.ctsConv(ctsIn, ctsPivot, -dx, -dy, -dz,
												-m_rotX, -m_rotY, -m_rotZ, -m_dS);
	return ell.llhConv(ctsOut);
}

LatLonHeight BursaWolfDatum::llhToWGS84(const LatLonHeight& llh)
{
	Ellipsoid ellWGS84 = Datum::ellWGS84();
	CoordCTS ctsIn, ctsOut;
	CoordCTS ctsPivot(0,0,0);
	ctsIn = ell.ctsConv(llh);
	ctsOut = ell.ctsConv(ctsIn, ctsPivot, dx, dy, dz,
												m_rotX, m_rotY, m_rotZ, m_dS);
	return ellWGS84.llhConv(ctsOut);
}

BadekasDatum::BadekasDatum(Ellipsoid e, double dX, double dY, double dZ, 
															          double rotX, double rotY, double rotZ, double dS,
																				double X0, double Y0, double Z0)
: BursaWolfDatum(e, dX, dY, dZ, rotX, rotY, rotZ, dS)
{
	m_sType = "User Defined Badekas";
	m_X0 = X0;
	m_Y0 = Y0;
	m_Z0 = Z0;
}

void BadekasDatum::Store(const FileName& fnCsy)
{
	BursaWolfDatum::Store(fnCsy);
	ObjectInfo::WriteElement("Datum", "Type", fnCsy, sType());
	ObjectInfo::WriteElement("Datum", "X0", fnCsy, m_X0);
	ObjectInfo::WriteElement("Datum", "Y0", fnCsy, m_Y0);
	ObjectInfo::WriteElement("Datum", "Z0", fnCsy, m_Z0);
}

bool BadekasDatum::fEqual(const Datum* datum) const
{
	const BadekasDatum* badekasDatum = dynamic_cast<const BadekasDatum*>(datum);
  if (0 == badekasDatum)
		return false;
  return 
		m_X0 == badekasDatum->m_X0 && 
		m_Y0 == badekasDatum->m_Y0 && 
    m_Z0 == badekasDatum->m_Z0 &&
		BursaWolfDatum::fEqual(datum); 
}

LatLonHeight BadekasDatum::llhFromWGS84(const LatLonHeight& llh)
{
  Ellipsoid ellWGS84 = Datum::ellWGS84();
	CoordCTS ctsIn, ctsOut;
	CoordCTS ctsPivot(m_X0,m_Y0,m_Z0);
	ctsIn = ellWGS84.ctsConv(llh);
	ctsOut = ell.ctsConv(ctsIn, ctsPivot, -dx, -dy, -dz,
												-m_rotX, -m_rotY, -m_rotZ, -m_dS);
	return ell.llhConv(ctsOut);
}

LatLonHeight BadekasDatum::llhToWGS84(const LatLonHeight& llh)
{
  Ellipsoid ellWGS84 = Datum::ellWGS84();
	CoordCTS ctsIn, ctsOut;
	CoordCTS ctsPivot(m_X0,m_Y0,m_Z0);
	ctsIn = ell.ctsConv(llh);
	ctsOut = ell.ctsConv(ctsIn, ctsPivot, dx, dy, dz,
												m_rotX, m_rotY, m_rotZ, m_dS);
	return ellWGS84.llhConv(ctsOut);
}
