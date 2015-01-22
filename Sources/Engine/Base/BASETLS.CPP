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
// basetls.c
// some functions that used to be in base.h
#include "Headers\toolspch.h"
#include "Engine\Base\strtod.h"

double doubleConv(const char* s)
{
	if (s == 0) return rUNDEF;
	char *endptr;
	char *begin = const_cast<char*>(s);

	// skip leading spaces; strtol will return 0 on a string with only spaces
	// which is not what we want
	while (isspace((unsigned char)*begin)) ++begin;

	if (strlen(begin) == 0) return rUNDEF;
	errno = 0;
	double r = strtodILW(begin, &endptr);
	if ((0 == *endptr) && (errno==0))
		return r;
	while (*endptr != 0) { // check trailing spaces
		if (*endptr != ' ')
			return rUNDEF;
		endptr++;
	}
	return r;
}

long longConv(const char* s)
{
	if (s == 0) return iUNDEF;

	char *endptr;
	char *begin = const_cast<char*>(s);

	// skip leading spaces; strtol will return 0 on a string with only spaces
	// which is not what we want
	while (isspace((unsigned char)*begin)) ++begin;

	if (strlen(begin) == 0) return iUNDEF;
	errno = 0;
	long l = strtol(begin, &endptr, 10);
	if ((0 == *endptr) && (errno==0))
		return l;

	return longConv(doubleConv(begin));
}

bool _export fCheckPoint(Coord A, Coord B, Coord crd, int &iLeft, double& dy1);

bool _export fAreaContains(const CoordBuf& buf, Coord crd)
{
  int iLeft = 0;
	CoordBounds cb;
	for (int j = 0; j < buf.iSize(); ++j)
		cb += buf[j];

	if (cb.fContains(crd) ) return true;

  double dy1 = buf[0].y - crd.y;
  for (int i = 1; i < buf.iSize(); ++i)
	   if (fCheckPoint(buf[i-1],buf[i],crd, iLeft, dy1)) 
        return true;
  return (iLeft % 2)!=0;
}

bool _export fCheckPoint(Coord A, Coord B, Coord crd, int &iLeft, double& dy1)
{
  double dy2, x, dxAB, dyAB;
  dy2 = (double) B.y - crd.y;
  if ( abs(dy2) < 0.001 )
    if ( B.x == crd.x ) return true;
  if ( abs(dy1) < 0.001 ) {
    if ( abs(dy2) < 0.001 )
      if ( (A.x < crd.x) ^ (B.x < crd.x) ) return true;
  }
  if ( (dy1 > 0) ^ (dy2 > 0) ) {
    dxAB = B.x - A.x;
    dyAB = B.y - A.y;
    x = A.x - dxAB * dy1 / dyAB;
    if ( x < crd.x - 0.001 )
      iLeft += 1;
    else if ( x < crd.x + 0.001 ) return true;
  }
  dy1 = dy2;
  return false;
}






