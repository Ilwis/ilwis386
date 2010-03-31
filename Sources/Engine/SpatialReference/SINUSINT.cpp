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
/* ProjectionCylindSinusoidalInterrupted
   Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE  17 Jun 96   11:02 am
*/
#include "Engine\SpatialReference\SINUSINT.H"
#define EPS10 1.e-10

ProjectionCylindSinusInterrupt::ProjectionCylindSinusInterrupt()
{
}

XY ProjectionCylindSinusInterrupt::xyConv(const PhiLam& pl) const
{
  XY xy;
  if (abs(pl.Phi)> M_PI_2) return xy;
  if (abs(pl.Lam)> M_PI) return xy;
  if (pl.Lam < 0)
   xy.x = (pl.Lam + M_PI_2) * cos(pl.Phi) - M_PI_2;
  else
   xy.x = (pl.Lam - M_PI_2) * cos(pl.Phi) + M_PI_2;
  xy.y = pl.Phi;
  return xy;
}
PhiLam ProjectionCylindSinusInterrupt::plConv(const XY& xy) const
{
  PhiLam pl;
  double cosfi = cos(xy.y);
  if (cosfi < EPS10) return pl;  // latitude beyond poles
  if (abs(xy.x + M_PI_2)/cosfi < M_PI_2)        // Western Hemisphere
      pl.Lam = (xy.x + M_PI_2) / cosfi - M_PI_2;
  else if (abs(xy.x - M_PI_2)/cosfi <= M_PI_2)  // Eastern Hemisphere
      pl.Lam = (xy.x - M_PI_2) / cosfi + M_PI_2;
  else return pl;                           // Not projected

  pl.Phi = xy.y;
  return pl;
}








