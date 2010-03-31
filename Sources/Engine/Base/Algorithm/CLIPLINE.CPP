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
/* clipline.c
   Copyright Ilwis System Development ITC
   aug 1996, Jan Hendrikse
    Last change:  WK   28 Aug 98    4:32 pm
*/
#define CLIPLINE_C
#include "Engine\Base\Algorithm\Clipline.h"

void Locate(const CoordBounds& cb, double rx, double ry,
                                              int& px, int& py )
{
  if (rx < cb.MinX())
    px = -1;
  else if (rx > cb.MaxX())
    px = 1;
  else
    px = 0;
  if (ry < cb.MinY())
    py = -1;
  else if (ry > cb.MaxY())
    py = 1;
  else
    py = 0;
}

void Relocate(const CoordBounds& cb, double& rx, double& ry,
                                int& px, int& py, const Coord& cPnt0, const Coord& cPnt1)
{
  double x0 = cPnt0.x;double y0 = cPnt0.y;
  double x1 = cPnt1.x;double y1 = cPnt1.y;

  if (px != 0)
  {
    if (px == -1)
      rx = cb.MinX();
    else
      rx = cb.MaxX();
    if (x1 == x0)
      ry = rUNDEF;
    else
      ry = y0 + (rx - x0) * (y1 - y0) / (x1 - x0);
    Locate(cb, rx, ry, px, py);
  } // else fOK
  if (py != 0 )
  {
    if (py == -1)
      ry = cb.MinY();
    else
      ry = cb.MaxY();
    if (y1 == y0)
      rx = rUNDEF;
    else
      rx = x0 + (ry - y0) * (x1 - x0) / (y1 - y0);
    Locate(cb, rx, ry, px, py);
  }  // else fOK
}

bool fClipLine(const CoordBounds& cb, Coord& cPnt0, Coord& cPnt1)
{
  double   rx0, rx1, ry0, ry1 ;
  int      px0, px1, py0, py1 ;
  bool     fOK ;

  double x0 = cPnt0.x;double y0 = cPnt0.y;
  double x1 = cPnt1.x;double y1 = cPnt1.y;
  Locate(cb, x0, y0, px0, py0); // locate position of Pnt0
  Locate(cb, x1, y1, px1, py1); // locate position of Pnt1
  if ((px0 * px1 == 1) || (py0 * py1 == 1))
    fOK = false ;
  else {
    fOK = 1;
    rx0 = x0; rx1 = x1; ry0 = y0; ry1 = y1;
    Relocate(cb, rx0, ry0, px0, py0, cPnt0, cPnt1);  // rx0,ry0 replaces Pnt0 on border of cb
    if (px0 != 0)
      fOK = false;
    else
      Relocate(cb, rx1, ry1, px1, py1, cPnt0, cPnt1);// rx1,ry1 replaces Pnt1 by border of cb
  };
  if (fOK)
  {
    cPnt0.x = rx0; cPnt0.y = ry0;  // new first point after clipping
    cPnt1.x = rx1; cPnt1.y = ry1;  // new second point after clipping
  };

  return fOK;
}









