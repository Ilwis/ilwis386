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
/* ProjectionLCylindEqualArea
   Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE   3 Jun 96    1:26 pm
*/
#include "Engine\SpatialReference\CYLEQARE.H"

ProjectionLCylindEqualArea::ProjectionLCylindEqualArea(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
  fEll = true;
  fUseParam[pvLATTS] = true;  
  fUseParam[pvK0] = false; // k0 is computed from phits in Param(pv, rValue)
}

XY ProjectionLCylindEqualArea::xyConv(const PhiLam& pl) const
{
  XY xy;
  if (abs(pl.Phi)> M_PI_2) return xy;
  if (abs(pl.Lam)> M_PI) return xy;
//FORWARD; /* ellipsoid */
	if ( !ell.fSpherical()) {
		xy.x = k0 * pl.Lam ;
		xy.y = .5 * qsfn(pl.Phi) / k0;
	}
//FORWARD; /* sphere */
	else {
		xy.x = pl.Lam * k0;
		xy.y = sin(pl.Phi) / k0;
	}
	return xy;
}
PhiLam ProjectionLCylindEqualArea::plConv(const XY& xy) const
{
  PhiLam pl;
//INVERSE;
  if ( !ell.fSpherical())     /* ellipsoid */
	{
		pl.Phi = authlat(asin( 2. * xy.y * k0 / qp), apa);
		pl.Lam = xy.x / k0;
	}
	else /* sphere */
	{
		double sinfi = xy.y * k0;
		double lam =  xy.x / k0;
		if (abs(sinfi)>= 1) return pl;
		if (abs(lam)> M_PI) return pl;
		pl.Phi = asin(sinfi);
		pl.Lam = lam;
	}
	return pl;
}

void ProjectionLCylindEqualArea::Param(ProjectionParamValue pv, double rValue)
{
  ProjectionPtr::Param(pv, rValue);
  if (pvLATTS == pv) {
		if (ell.fSpherical()) 
      k0 = cos(phits);
    else
		{
			double t = sin(phits);
      k0 = cos(phits) / sqrt(1. - ell.e2 * t * t);
			authset(apa);
			qp = qsfn(M_PI_2);
		}
  }    
}            






