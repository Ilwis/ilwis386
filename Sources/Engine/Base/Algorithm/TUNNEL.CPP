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
// tunnel.c
// tunneling function
// by Wim Koolhoven
// (c) Computer Department ITC

#define TUNNEL_C
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Tunnel.h"

bool fInTunnel(double d2,
	       const Coord& cBegin,
	       const Coord& cMiddle,
	       const Coord& cEnd)
{
  double dx, dy, dx2, dy2, x0, y0, l2, xd2, yd2;

  if (d2 < 1e-20) d2 = 1e-20;

// JW 17 oct. 1996:
// d2 is quadrat of diameter of tunnel corridor, so divide by 2*2   
  d2 /= 4;
  
  dx = cEnd.x - cBegin.x;
  dy = cEnd.y - cBegin.y;
  dx2 = dx * dx;
  dy2 = dy * dy;
  l2 = dx2 + dy2;
  if (dx2 > 1e-20 && dy2 > 1e-20)
    if ( dy2 < dx2 ) {
      y0 = cBegin.y + dy * (cMiddle.x - cBegin.x) / dx;
      yd2 = d2 * l2 / dx2;
      return (cMiddle.y - y0) * (cMiddle.y - y0) <= yd2;
    }
    else {
      x0 = cBegin.x + dx * (cMiddle.y - cBegin.y) / dy;
      xd2 = d2 * l2 / dy2;
      return (cMiddle.x - x0) * (cMiddle.x - x0) <= xd2;
    }
  else {
    dx = cMiddle.x - cBegin.x;
    dy = cMiddle.y - cBegin.y;
    return dx * dx + dy * dy <= d2;
  }
}

bool fInTunnel(double d2,
	       RowCol rcBegin,
	       RowCol rcMiddle,
	       RowCol rcEnd)
{
  double dx, dy, dx2, dy2, x0, y0, l2, xd2, yd2;

  if (d2 < 1e-20) d2 = 1e-20;

// JW 17 oct. 1996:
// d2 is quadrat of diameter of tunnel corridor, so divide by 2*2   

  d2 /= 4;
  dx = rcEnd.Col - rcBegin.Col;
  dy = rcEnd.Row - rcBegin.Row;
  dx2 = dx * dx;
  dy2 = dy * dy;
  l2 = dx2 + dy2;
  if (dx2 > 1e-20 && dy2 > 1e-20)
    if ( dy2 < dx2 ) {
      y0 = rcBegin.Row + dy * (rcMiddle.Col - rcBegin.Col) / dx;
      yd2 = d2 * l2 / dx2;
      return (rcMiddle.Row - y0) * (rcMiddle.Row - y0) <= yd2;
    }
    else {
      x0 = rcBegin.Col + dx * (rcMiddle.Row - rcBegin.Row) / dy;
      xd2 = d2 * l2 / dy2;
      return (rcMiddle.Col - x0) * (rcMiddle.Col - x0) <= xd2;
    }
  else {
    dx = rcMiddle.Col - rcBegin.Col;
    dy = rcMiddle.Row - rcBegin.Row;
    return dx * dx + dy * dy <= d2;
  }
}








