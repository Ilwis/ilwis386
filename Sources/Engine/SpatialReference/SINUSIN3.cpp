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
/* ProjectionCylindSinusoidalInterrupted3
   Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE  13 Jun 96   11:12 am
*/
#include "Engine\SpatialReference\SINUSIN3.H"
#define EPS10 1.e-10
const double PI_60 = (M_PI/60);

ProjectionCylindSinusInterrupt3::ProjectionCylindSinusInterrupt3()
{
}

XY ProjectionCylindSinusInterrupt3::xyConv(const PhiLam& pl) const
{
  XY xy;
  if (abs(pl.Phi)> M_PI_2) return xy;
  if (abs(pl.Lam)> M_PI) return xy;
  if ( pl.Phi < 0 ) { // Southern Hemisphere splitted in 3 parts
    if (pl.Lam < - 6 * PI_60)
      xy.x = (pl.Lam + 33 * PI_60) * cos(pl.Phi) - 33 * PI_60; // west of 10W
    else if (pl.Lam < 20 * PI_60)
      xy.x = (pl.Lam - 7 * PI_60) * cos(pl.Phi) + 7 * PI_60;   // between 10W and 60E
    else
      xy.x = (pl.Lam - 40 * PI_60) * cos(pl.Phi) + 40 * PI_60; // east of 60E
  }
  else {     // Northern Hemisphere splitted in 2 parts
    if (pl.Lam < - 6 * PI_60)                                  // west of 10W
      xy.x = (pl.Lam + 33 * PI_60) * cos(pl.Phi) - 33 * PI_60;
    else                                                       // east of 10W
      xy.x = (pl.Lam - 27 * PI_60) * cos(pl.Phi) + 27 * PI_60;
  }
  xy.y = pl.Phi;
  return xy;
}
PhiLam ProjectionCylindSinusInterrupt3::plConv(const XY& xy) const
{
  PhiLam pl;
  double cosfi = cos(xy.y);
  if (cosfi < EPS10) return pl;  // latitude beyond poles
  if ( xy.y < 0 )  { // Southern Hemisphere
    if (abs(xy.x + 33 * PI_60)/cosfi <= 27 * PI_60)   // Sector from -180 to -10
      pl.Lam = (xy.x + 33 * PI_60) / cosfi - 33 * PI_60;
    else if (abs(xy.x - 7 * PI_60)/cosfi <= 13 * PI_60)  // Sector from -10 to 60
      pl.Lam = (xy.x - 7 * PI_60) / cosfi + 7 * PI_60;
    else if (abs(xy.x - 40 * PI_60)/cosfi <= 20 * PI_60)  // Sector from 60 to 180
      pl.Lam = (xy.x - 40 * PI_60) / cosfi + 40 * PI_60;
    else return pl;
  }
  else  {    // Northern Hemisphere
    if (abs(xy.x + 33 * PI_60)/cosfi <= 27 * PI_60)   // Sector from -180 to -10
      pl.Lam = (xy.x + 33 * PI_60) / cosfi - 33 * PI_60;
    else if (abs(xy.x - 27 * PI_60)/cosfi <= 33 * PI_60)  // Sector from -10 to 180
      pl.Lam = (xy.x - 27 * PI_60) / cosfi + 27 * PI_60;
    else return pl;
  }
  pl.Phi = xy.y;
  return pl;
}








