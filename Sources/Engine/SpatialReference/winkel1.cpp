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
/* ProjectionWinkel1
   Copyright Ilwis System Development ITC
   nov 2004, by Jan Hendrikse / Gerald Evenden USGS
	Last change:  JHE  30 nov 2004    3:42 pm
*/

#include "Engine\SpatialReference\Winkel1.H"
#define EPS10  1.e-10

ProjectionWinkel1::ProjectionWinkel1()
{
	fUseParam[pvLATTS] = true;
	cosphi1 = cos(phits);
}

XY ProjectionWinkel1::xyConv(const PhiLam& pl) const
{
	XY xy;
	if ( abs(pl.Phi) >= M_PI_2 ) return xy;
	xy.x = 0.5 * pl.Lam * (cosphi1 + cos(pl.Phi));
	xy.y = pl.Phi;
  return xy;
}
PhiLam ProjectionWinkel1::plConv(const XY& xy) const
{
  PhiLam pl;
	pl.Phi = xy.y;
	pl.Lam = 2. * xy.x / (cosphi1 + cos(pl.Phi));	
  return pl;
}

void ProjectionWinkel1::Param(ProjectionParamValue pv, double rValue)
{
  ProjectionPtr::Param(pv, rValue);
  if (pvLATTS == pv) {
    k0 = cos(phits);
  }    
}            



