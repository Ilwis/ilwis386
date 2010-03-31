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
/* PolygonArea
   Copyright Ilwis System Development ITC
   april 1995, by Wim Koolhoven
   february 1997, split by Willem Nieuwenhuis
   Last change:  WK   26 Feb 97    1:08 pm
*/

#include "Engine\Map\Polygon\POLAREA.H"
#include "Headers\Hs\polygon.hs"

void PolArea::Buf(CoordBuf& buf, long iSize)
{
    for(long i=1; i<iSize; ++i)
        Line(buf[i-1], buf[i]);
}

void PolArea::Line(Coord p, Coord q)
{
  rBkw += q.x * p.y;
  rFrw += p.x * q.y;
  rLen += rDist(p,q);
}

void PolArea::Top(Topology top)
{
  int i;
  long iNr;
  CoordBuf buf(1000);
  top.GetCoords(iNr, buf);
  for (i = 1; i < iNr; ++i)
    Line(buf[i-1], buf[i]);
}

static void TooMuchTopsError()
{
	// The arbitrary inserted limit of topologies was reached (suggests corrupt data)
  ErrorObject err(WhatError(SPOLErrTooManyBoundaries, 9999));
  err.SetTitle(SPOLTitleTopologyError);
  throw err;
//  err.Show();
}

void PolArea::FollowTop(Topology ts)
{
	Topology t = ts;
  long iTops = 0;
	/* 32000 was arbitrarily chosen to be the upper limit in iTops in order to
	   prevent the hanging of ILWIS with a corrupt dataset. An upper limit must be
		 set with the current implementation; we just don't know how big iTops can
		 get with a complicated (and existing) data set.
	*/
  do {
    if (++iTops > 32000)
		{
      TooMuchTopsError();
      return;
    }  
    Top(t);
    t = t.topNext();
  } while (t != ts);
}

void PolArea::Pol(ilwPolygon pol)
{
  Topology ts = pol.topStart();
  FollowTop(ts);
}




