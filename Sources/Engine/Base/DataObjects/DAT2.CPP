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
/* $Log: /ILWIS 3.0/Tools/DAT2.CPP $
 * 
 * 17    7-10-02 11:35 Hendrikse
 * moved rSphericalDistance as static function to the distance.cpp file in
 * Coordsuystems project
 * 
 * 16    3-09-02 14:32 Hendrikse
 * implemented rSphericalDistance for spheric interpolation functions etc
 * 
 * 15    5-07-02 18:59 Koolhoven
 * prevent warings with Visual Studio .Net
 * 
 * 14    20-02-02 14:18 Retsios
 * Spaces before LatLon coordinates caused crash - general robuustness
 * improvement for LatLon::rDegree
 * 
 * 13    12/04/01 15:51 Willem
 * Take modulo to quicken calculation in case of large input Longitude
 * 
 * 12    11/02/01 12:19p Martin
 * forgot to add the 'W' sign modifier when tranlating latlon strings
 * 
 * 11    11/02/01 9:37a Martin
 * changes the rDgree function of LatLon. the scans missed some cases
 * 
 * 10    8/30/01 16:20 Willem
 * Removed the LatLon display limitation outside
 * 
 * 9     11/28/00 1:04p Martin
 * in rDegree sscanf found that -0 == 0 so -0 01 0 == 0 01 0. Check if the
 * first char is a minus instead trusting sscanf
 * 
 * 8     11/28/00 12:31p Martin
 * rDegree and sDegree return UNDEF if an undef is passed
 * 
 * 7     20-06-00 8:25a Martin
 * += operator of Coordbounds now checks if it was undefined
 * 
 * 6     16-06-00 9:20a Martin
 * added constructor for coordbounds (with coordbuf)
 * 
 * 5     8-05-00 18:30 Hendrikse
 * Have set iWidth = 14 at several places to make lon and lat precision
 * equal.
 * Now fShowMin parameter is used in sDegree in case of neg angles for the
 * dms function
 * rDegree is debugged for neg angles from sDegree
 * 
 * 4     4/04/00 1:58p Wind
 * added static functions to latLon for conversions between degrees and
 * strings
 * 
 * 3     21-12-99 4:10p Martin
 * //-> /*
 * 
 * 2     21-12-99 1:02p Martin
 * added _Export and a operator '-' for two coords
// Revision 1.2  1997/07/28 13:21:29  Wim
// 180 W till 180 E, nwo both possible.
//
/* dat2.c
// General Interface for ILWIS 2.0
// april 1993, by Wim Koolhoven
// (c) Computer Department ITC
	Last change:  WK   28 Aug 98    4:53 pm
*/
#define DAT2_C
#include "Headers\toolspch.h"

RowCol _export operator-(const RowCol& rcA, const RowCol& rcB)
{ 
  return RowCol(rcA.Row - rcB.Row, rcA.Col - rcB.Col); 
}

Coord _export operator-(const Coord& crdA, const Coord& crdB)
{ 
  return Coord(crdA.x - crdB.x, crdA.y - crdB.y); 
}

void MinMax::Check()  // makesure that rcMin < rcMax bith with Row and Col
{
  long iTmp;
  if (MinRow() > MaxRow()) {
    iTmp = MinRow();
    MinRow() = MaxRow();
    MaxRow() = iTmp;
  }
  if (MinCol() > MaxCol()) {
    iTmp = MinCol();
    MinCol() = MaxCol();
    MaxCol() = iTmp;
  }
}

bool MinMax::fContains(RowCol rc) const
{
  if (rc.fUndef()) return false;
  if (rc.Row < MinRow()) return 0;
  if (rc.Row > MaxRow()) return 0;
  if (rc.Col < MinCol()) return 0;
  if (rc.Col > MaxCol()) return 0;
  return 1;
}

void MinMax::SetSize(const RowCol& rc)
{
    if (rc.fUndef()) return;
    if (rcMin.fUndef())
      rcMin = RowCol(0L,0L);
    rcMax = RowCol(rcMin.Row + rc.Row, rcMin.Col + rc.Col);
}

bool MinMax::fNear(RowCol rc,long iTolerance) const
{
  if (rc.fUndef()) return false;
  if (rc.Row < MinRow() - iTolerance) return 0;
  if (rc.Row > MaxRow() + iTolerance) return 0;
  if (rc.Col < MinCol() - iTolerance) return 0;
  if (rc.Col > MaxCol() + iTolerance) return 0;
  return 1;
}

bool MinMax::fContains(MinMax mm) const // true if partly contains
{
  if (mm.fUndef()) return false;
  if (mm.MinRow() > MaxRow()) return 0;
  if (mm.MaxRow() < MinRow()) return 0;
  if (mm.MinCol() > MaxCol()) return 0;
  if (mm.MaxCol() < MinCol()) return 0;
  return 1;
}

MinMax& MinMax::operator += (RowCol rc)        // change this so that point is inside
{
  if (rc.fUndef()) return *this;
  if (rc.Row < MinRow()) MinRow() = rc.Row;
  if (rc.Row > MaxRow()) MaxRow() = rc.Row;
  if (rc.Col < MinCol()) MinCol() = rc.Col;
  if (rc.Col > MaxCol()) MaxCol() = rc.Col;
  return *this;
}

MinMax& MinMax::operator += (const MinMax& mm) // change this so that mm is inside
{
  if (mm.fUndef()) return *this;
  if (mm.MinRow() < MinRow()) MinRow() = mm.MinRow();
  if (mm.MaxRow() > MaxRow()) MaxRow() = mm.MaxRow();
  if (mm.MinCol() < MinCol()) MinCol() = mm.MinCol();
  if (mm.MaxCol() > MaxCol()) MaxCol() = mm.MaxCol();
  return *this;
}

bool Coord::operator == (const Coord& crd) const
{ return fabs(x - crd.x) < 1e-10 && fabs(y - crd.y) < 1e-10 && fabs(z - crd.z) < 1e-10; }

CoordBounds::CoordBounds(const CoordBuf& cBuf)
{
	for(int i=0; i<cBuf.iSize(); ++i)
		operator+=(cBuf[0]);
}

Coord CoordBounds::middle() const {
	return Coord((cMax.x + cMin.x) / 2.0, (cMax.y + cMin.y) / 2.0,(cMax.z + cMin.z) / 2.0);
}

CoordBounds& CoordBounds::operator *=(double fact) {
	double dw = width() * fact / 2.0;
	double dh = height() * fact / 2.0;
	Coord cm = middle();
	double maxx = cm.x + dw;
	double maxy = cm.y + dh;
	double minx = cm.x - dw;
	double miny = cm.y - dh;
	cMax = Coord(maxx,maxy);
	cMin = Coord(minx,miny);

	return *this;
}
void CoordBounds::Check()  // makesure that rcMin < rcMax bith with Row and Col
{
  double rTmp;
  if (MinX() > MaxX()) {
    rTmp = MinX();
    MinX() = MaxX();
    MaxX() = rTmp;
  }
  if (MinY() > MaxY()) {
    rTmp = MinY();
    MinY() = MaxY();
    MaxY() = rTmp;
  }
  /*if (MinZ() > MaxZ()) {
    rTmp = MinY();
    MinZ() = MaxZ();
    MaxZ() = rTmp;
  }*/
}

bool CoordBounds::fContains(Coord c) const
{
  if (c.fUndef()) return false;
  if (c.x < MinX()) return 0;
  if (c.x > MaxX()) return 0;
  if (c.y < MinY()) return 0;
  if (c.y > MaxY()) return 0;
  /*if ( c.z == rUNDEF || c.z == rUNDEF)
	  return 1;
  if ( abs(c.z) > 1e20)
	  return 1;
  if (c.z < MinZ()) return 0;
  if (c.z > MaxZ()) return 0;*/
  return 1;
}

bool CoordBounds::fNear(Coord c, double rTolerance) const
{
  if (c.fUndef()) return false;
  if (c.x < MinX() - rTolerance) return 0;
  if (c.x > MaxX() + rTolerance) return 0;
  if (c.y < MinY() - rTolerance) return 0;
  if (c.y > MaxY() + rTolerance) return 0;
  //if ( abs(c.z) > 1e20)
	 // return 1;
  //if (c.z < MinZ() - rTolerance) return 0;
  //if (c.z > MaxZ() + rTolerance) return 0;
  return 1;
}

bool CoordBounds::fContains(const CoordBounds& cb) const // true if partly contains
{
  if (cb.fUndef()) return false;
  if (cb.MinX() > MaxX()) return 0;
  if (cb.MaxX() < MinX()) return 0;
  if (cb.MinY() > MaxY()) return 0;
  if (cb.MaxY() < MinY()) return 0;
 /* if ( cb.MinZ() == rUNDEF || cb.MaxZ() == rUNDEF)
	  return 1;
  if ( abs(cb.MinZ())  > 1e20)
	  return 1;
  if (cb.MinZ() > MaxZ()) return 0;
  if (cb.MaxZ() < MinZ()) return 0;*/
  return 1;
}

CoordBounds& CoordBounds::operator += (Coord c)        // change this so that point is inside
{
	if (!c.fUndef()) 
	{
		if ( MinX() == rUNDEF ) MinX() = c.x;
		if ( MinY() == rUNDEF ) MinY() = c.y;
		if ( MaxX() == rUNDEF ) MaxX() = c.x;
		if ( MaxY() == rUNDEF ) MaxY() = c.y;
		if (c.x < MinX()) MinX() = c.x;
		if (c.x > MaxX()) MaxX() = c.x;
		if (c.y < MinY()) MinY() = c.y;
		if (c.y > MaxY()) MaxY() = c.y;
	/*	if ( c.z != rUNDEF && abs(c.z) < 1e20) {
			if ( MaxZ() == rUNDEF || abs(MaxZ()) > 1e20) MaxZ() = c.z;
			if ( MinZ() == rUNDEF || abs(MaxZ()) > 1e20) MinZ() = c.z;
			if (c.z < MinZ()) MinZ() = c.z;
			if (c.z > MaxZ()) MaxZ() = c.z;
		}*/
	}    
	return *this;
}

CoordBounds& CoordBounds::operator += (const CoordBounds& cb) // change this so that cb is inside
{
  if (!cb.fUndef()) 
	{
		if ( MinX() == rUNDEF ) MinX() = cb.MinX();
		if ( MinY() == rUNDEF ) MinY() = cb.MinY();
		if ( MaxX() == rUNDEF ) MaxX() = cb.MaxX();
		if ( MaxY() == rUNDEF ) MaxY() = cb.MaxY();
		if (cb.MinX() < MinX()) MinX() = cb.MinX();
		if (cb.MaxX() > MaxX()) MaxX() = cb.MaxX();
		if (cb.MinY() < MinY()) MinY() = cb.MinY();
		if (cb.MaxY() > MaxY()) MaxY() = cb.MaxY();
		/*if ( abs(cb.MinZ()) < 1e20 && cb.MaxZ() < 1e20) {
			if ( MinZ() == rUNDEF ) MinZ() = cb.MinZ();
			if ( MaxZ() == rUNDEF ) MaxZ() = cb.MaxZ();
			if (cb.MinZ() < MinZ()) MinZ() = cb.MinZ();
			if (cb.MaxZ() > MaxZ()) MaxZ() = cb.MaxZ();
		}*/


  }  
  return *this;
}

bool Coord::fNear(const Coord& c,double rTolerance) const
{ double r = rDist2(*this,c);
  if (r == rUNDEF) return false;
  return r < rTolerance * rTolerance;
}

bool RowCol::fNear(RowCol rc,long iTolerance) const
{
  if (rc.fUndef()) return false;
  if (abs(rc.Row - Row) > iTolerance) return 0;
  if (abs(rc.Col - Col) > iTolerance) return 0;
  return 1;
}

String LatLon::sLat(int iWidth) const
{
	return sLat(Lat, iWidth);
}

String LatLon::sLat(double Lat, int iWidth)
{
	if (iWidth <= 0)
		iWidth = 14;

	if (iWidth < 3)
		return String('*', iWidth);

	if (Lat < iUNDEF)
		return String("%*s", -iWidth, "? N/S");
	
	bool fNorth = Lat >= 0;
	String sValue = sDegree(Lat, iWidth);
	int iLen = sValue.length();
	if (iLen < iWidth - 1)
		sValue &= " ";
	sValue &= fNorth ? "N" : "S";  
	return sValue; 
}

String LatLon::sLon(int iWidth) const
{
	return sLon(Lon, iWidth);
}

String LatLon::sLon(double Lon, int iWidth)
{
	if (iWidth <= 0)
		iWidth = 14;

	if (iWidth < 4)
		return String('*', iWidth);

	if (Lon < iUNDEF)
		return String("%*s", -iWidth, "? E/W");
	
    double rLon = fmod(Lon, 360.0);
	
	while (rLon < -180)
		rLon += 360;
	while (rLon > 180) 
		rLon -= 360;
	bool fEast = rLon >= 0;
	
	String sValue = sDegree(rLon, iWidth);
	
	int iLen = sValue.length();
	if (iLen < iWidth - 1)
		sValue &= " ";
	sValue &= fEast ? "E" : "W";  
	return sValue; 
}

String LatLon::sValue(int iWidth) const
{
	if (iWidth <= 0)
		iWidth = 29;

	if (fUndef())
		return String("%*s", -iWidth, "? N/S, ? E/W");

	if (iWidth < 8)
		return String('*', iWidth);

	iWidth -= 1;  
	int iLat = iWidth / 2;
	int iLon = iWidth - iLat;

	String sValue = sLat(iLat);
	sValue &= ",";
	sValue &= sLon(iLon);

	return sValue;  
}

String LatLon::sDegree(double rDegree, int iWidth, bool fShowMin) 
{
	if ( rDegree == rUNDEF)
		return "?";
	
  if (iWidth <= 0)
    iWidth = 14;
	double rDeg = abs(rDegree);
	int iDeg = (int)floor(rDeg);
  rDeg -= iDeg;
  rDeg *= 60;
  int iMin = (int)floor(rDeg);
  rDeg -= iMin;
  rDeg *= 60;

  if (iWidth < 7) {
    if (iMin > 30)
      iDeg += 1;
  }
  else if (iWidth < 10) {
    if (rDeg > 30) {
      iMin += 1;
      if (iMin > 59) {
        iDeg += 1;
        iMin -= 60;
      }
    }
  }
  if (rDeg > 59.99) {
    iMin += 1;
    rDeg = 0;
  }
  if (iMin > 59) {
    iDeg += 1;
    iMin -= 60;
  }
  
	if (fShowMin)
		iDeg = rDegree < 0 ? -iDeg : iDeg;
  String sValue = String("%3i", iDeg);
  if (iWidth > 4) 
    sValue &= "°";
  if (iWidth > 6) {
    String s = String("%2i", iMin);
    if (s[0] == ' ')
      s[0] = '0';
    sValue &= s;  
  }    
  if (iWidth > 7)
    sValue &= "\'";
  if (iWidth > 9) {
    String s;
    int iDec = iWidth - 12;
    if (iDec <= 0) {
      s = String("%2i", shortConv(floor(rDeg)));
      if (iWidth > 10)
        s &= "\"";
    }  
    else
      s = String("%*.*f\"", iWidth-9, iDec, rDeg);  
    if (s[0] == ' ')
      s[0] = '0';
    sValue &= s;  
  }      
	return sValue;
}

double LatLon::rDegree(const String& sDegree)
{	
	if ( sDegree == "?")
		return rUNDEF;
	double r = rUNDEF;
	String arsVals[3];
	int iSign1 = 1;
	int j=0;
	for(unsigned int i=0; i < sDegree.size(); ++i)
	{
		char c = sDegree[i];
		while ((c == ' ') && (i<sDegree.size()))
			c = sDegree[++i]; // trim leading spaces and erroneously doubled spaces
		while ((isdigit(c) || c == '.' || c =='-') && (i<sDegree.size()))
		{
			if (j <= 2) // if j>2 at this point, probably the user is using too difficult syntax
				arsVals[j] += c;
			c = sDegree[++i];
		}	
		if ( j >=2 &&  ( c == 'S' || c == 'W' )) // j>=2: allow a.o. "-10 10 10W"
			iSign1 = -1;
		++j;
	}
	int iSign2 = arsVals[0][0] =='-' ? -1 : 1; // negative sign on southern hemisphere

	r = iSign1 * ( arsVals[0].iVal() + iSign2 * arsVals[1].iVal() / 60.0 + iSign2 * arsVals[2].rVal() / 3600.0 );

	return r;

}


#ifdef __DLL__
int FAR PASCAL LibMain(HANDLE, WORD, WORD, LPSTR)
{ return 1; }
#endif
