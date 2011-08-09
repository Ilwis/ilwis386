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
/* CoordSystemLatLon
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  WK    8 Apr 98   11:14 am
*/

#include "Engine\SpatialReference\Cslatlon.h"

CoordSystemLatLon::CoordSystemLatLon(const FileName& fn)
: CoordSystemViaLatLon(fn)
{
  rUnitSize = 0;
}

CoordSystemLatLon::CoordSystemLatLon(const FileName& fn, int iOpt)
: CoordSystemViaLatLon(fn,iOpt)
{
  rUnitSize = 0;
}

CoordSystemLatLon::~CoordSystemLatLon()
{
}

void CoordSystemLatLon::Store()
{
  CoordSystemViaLatLon::Store();
  WriteElement("CoordSystem", "Type", "LatLon");
}

String CoordSystemLatLon::sType() const
{
  return "Coordinate System LatLon";
}

bool CoordSystemLatLon::fLatLon2Coord() const
{
  return true;
}

bool CoordSystemLatLon::fCoord2LatLon() const
{
  return true;
}

String CoordSystemLatLon::sValue(const Coord& c, short wid, short dec) const
{
  if (wid < 0)
    wid = iWidth();
  LatLon ll = llConv(c);
  return ll.sValue(wid);
}

/*
Coord CoordSystemLatLon::cValue(const String& s) const
{
  Coord crd;
  if (2 != sscanf(s.c_str(), "(%lg,%lg)", &crd.y, &crd.x))
    if (2 != sscanf(s.c_str(), "%lg,%lg", &crd.y, &crd.x))
      sscanf(s.c_str(), "%lg%lg", &crd.y, &crd.x);
  return crd;
}

*/

Coord CoordSystemLatLon::cValue(const String& s) const
{
  Coord crd;
	String sLat, sLon, sCrdPair;
	if ( s[0] == '(' && s[s.length() - 1] == ')')
		sCrdPair = s.sSub(1, s.length() - 2);
	else 
		sCrdPair  = s;
	Array<String> asComma, asSpace, asDegree;
	Split(sCrdPair , asComma , ",");
	if (asComma.iSize() == 1) {  // no comma's found
		Split(sCrdPair , asSpace , " ");
		if (asSpace.iSize() == 2) // metric coords
			sscanf(sCrdPair.c_str(), "%lg%lg", &crd.y, &crd.x);
		else {
			int i = 0;
			int iHalf = asSpace.iSize() / 2;
			while (i < iHalf)
			{
				sLat &= asSpace[0 + i]; sLat &= " ";
				sLon &= asSpace[iHalf + i]; sLon &= " ";
				i++;
			}
			crd = Coord(LatLon::rDegree(sLon),LatLon::rDegree(sLat));
		}
	}
	else // delimited by comma
	{
		Split(asComma[0], asSpace, " ");
		Split(asComma[0], asDegree, "°");
		if (asSpace.iSize() == 1 && asDegree.iSize() == 1) 
													// metric coords without spaces
			sscanf(s.c_str(), "%lg,%lg", &crd.y, &crd.x);
		else
		{ 
			Split(sCrdPair , asComma , ",");
			sLat = asComma[0];
			sLon = asComma[1];
			crd = Coord(LatLon::rDegree(sLon),LatLon::rDegree(sLat));
		}
	}
	return crd;
}


LatLon CoordSystemLatLon::llConv(const Coord& crd) const
{
  LatLon ll;
  ll.Lat = crd.y;
  ll.Lon = crd.x;
  return ll;
}

Coord CoordSystemLatLon::cConv(const LatLon& ll) const
{
  Coord crd;
  crd.x = ll.Lon;
  crd.y = ll.Lat;
  return crd;
}



