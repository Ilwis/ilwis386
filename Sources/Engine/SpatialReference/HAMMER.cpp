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
/* ProjectionHammer-Aitoff  for the Sphere and Equatorial Aspect (JH 1999)
   Copyright Ilwis System Development ITC
   june 1996, by Jan Hendriks && Rein Laan for plConv
	Last change:  JHE  24 Jun 96    9:48 pm
*/
#include "Engine\SpatialReference\HAMMER.H"

ProjectionHammerCylindrical::ProjectionHammerCylindrical()
{ 
  fUseParam[pvLAT0] = true;
  fUseParam[pvK0] = true;
	Param(pvK0, 1.0);
  //Param(pvLON0, 17.0); //suggested for Africa
  //Param(pvLAT0, 1.0);  //    ,,			,,
}

#define EPS10  1.e-10
const double Root2 = sqrt(2.0);

XY ProjectionHammerCylindrical::xyConv(const PhiLam& pl) const
{
  XY xy;
    double sinPhi = sin(pl.Phi - phi0);
    double cosPhi = cos(pl.Phi - phi0);
    double sinLam = sin(pl.Lam/2.0);
    double cosLam = cos(pl.Lam/2.0);
  if (abs(pl.Phi - phi0) > M_PI_2 ) return xy; //beyond 'shifted' poles
  if (( abs(pl.Phi - phi0) < EPS10)       // too close to singular 'shifted' equator endpoints
      && (abs(pl.Lam) > M_PI - EPS10)) return xy;
  double denom = sqrt(1.0 + cosPhi * cosLam);
  xy.x = 2.0 * k0 * Root2 * cosPhi * sinLam /denom;
  xy.y = Root2 * k0 * sinPhi /denom; //true scale at pseudo equator
  return xy;
}
PhiLam ProjectionHammerCylindrical::plConv(const XY& _xy) const
{
  XY xy = _xy;
  xy.x = _xy.x / (2.0 * Root2) / k0;
  xy.y = _xy.y / Root2 / k0;
  PhiLam pl;
  double x2 = xy.x * xy.x;
  double y2 = xy.y * xy.y;
  if (x2 + y2 > 2) return pl;
  double y4 = y2 * y2;
  double numer1 = xy.x * sqrt(2.0 - x2 - y2);
  double numer2 = xy.y * sqrt(2.0 - x2 - y2);
  double denom = sqrt(y4 +(x2 - 2.0) * y2 + 1.0);
  if (abs(denom) < EPS10)  return pl;
  if ( abs(numer1/denom) > M_PI_2 - EPS10 )  return pl;
  pl.Phi = atan(numer2/denom) + phi0;
  pl.Lam = asin(numer1/denom) * 2.0;
  return pl;
}





