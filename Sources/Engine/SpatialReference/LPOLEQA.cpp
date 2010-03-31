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
/* ProjectionLPolarEqualAreaphic
   Copyright Ilwis System Development ITC
  may 1996, by Jan Hendrikse
	Last change:  JHE  24 May 96    3:42 pm
*/
#include "Engine\SpatialReference\LPOLEQA.H"

ProjectionLPolarEqualArea::ProjectionLPolarEqualArea(const Ellipsoid& ell)
:ProjectionAzimuthal(ell)
{
}

#define EPS10 1.E-10

XY ProjectionLPolarEqualArea::xyConv(const PhiLam& pl) const
{
  XY xy;
  double rCosc = rSinPhi0 * sin(pl.Phi) + rCosPhi0 * cos(pl.Phi) * cos(pl.Lam);
  if (rCosc < EPS10 ) return xy; // pl om opposite halfsphere with resp to Phi0,Lam0
  double rK = sqrt(2 /(1 + rCosc));
  xy.x = rK * cos(pl.Phi) * sin(pl.Lam);
  xy.y = rK * (rCosPhi0 * sin(pl.Lam) - rSinPhi0 * cos(pl.Phi) * cos(pl.Lam));
  return xy;
}
PhiLam ProjectionLPolarEqualArea::plConv(const XY& xy) const
{
  PhiLam pl;
  double rRho = sqrt(xy.x*xy.x + xy.y*xy.y);
  if ( rRho >= 2 - EPS10) return pl;   // point beyond edge of projected sphere
  double rC = 2 * asin (rRho/2);
  pl.Phi =  asin(cos(rC) * rSinPhi0 + (xy.y * sin(rC) * rCosPhi0)/ rRho );
  pl.Lam = atan2(xy.x * sin(rC) , rRho * rCosPhi0 * cos(rC) - xy.y * rSinPhi0 * sin(rC));
  return pl;
}





