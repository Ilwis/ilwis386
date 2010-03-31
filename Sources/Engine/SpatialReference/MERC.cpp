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
/* ProjectionMercator, ProjectionParamsMercator
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   24 Oct 96    7:34 pm
*/

#include "Engine\SpatialReference\MERC.H"
#define EPS10  1.e-10

ProjectionMercator::ProjectionMercator(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
  fEll = true;
  fUseParam[pvLATTS] = true;  
  fUseParam[pvK0] = false; // k0 is computed from phits in Param(pv, rValue)
}

XY ProjectionMercator::xyConv(const PhiLam& pl) const
{
  XY xy;
  if ( abs(pl.Phi) > M_PI_2 - EPS10 ) return xy;
  if (ell.fSpherical()) {
    xy.x = k0 * pl.Lam;
    xy.y = k0 * log(tan(M_PI_4 + .5 * pl.Phi));
  }
  else {
    xy.x = k0 * pl.Lam;
    xy.y = -k0 * log(tsfn(pl.Phi));
  }  
  return xy;
}

PhiLam ProjectionMercator::plConv(const XY& xy) const
{
  PhiLam pl;
/*  
  Waarom zouden deze niet mogen ?? 
  Op 88 N is y groter dan pi !
  if ( abs(xy.x) > M_PI) return pl;
  if ( abs(xy.y) > M_PI) return pl;
*/  
  if (ell.fSpherical()) {
    pl.Phi = M_PI_2 - 2. * atan(exp(-xy.y / k0));
    pl.Lam = xy.x / k0;
  }  
  else {
    pl.Phi = phi2(exp(-xy.y / k0));
    pl.Lam = xy.x / k0;
  }
  return pl;
}

void ProjectionMercator::Param(ProjectionParamValue pv, double rValue)
{
  ProjectionPtr::Param(pv, rValue);
  if (pvLATTS == pv) {
    if (ell.fSpherical()) 
      k0 = cos(phits);
    else
      k0 = msfn(phits);
  }    
}            






