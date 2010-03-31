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
/* ProjectionEckert3
  Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
  Last change:  JHE  24 Jun 97    9:03 am
*/

#include "Engine\SpatialReference\ECKERT3.H"
#include "Engine\Base\DataObjects\ERR.H"

#define XF	.42223820031577120149
#define RXF	2.36833142821314873781
#define YF	.84447640063154240298
#define RYF	1.18416571410657436890
ProjectionEckert3::ProjectionEckert3()
{}

XY ProjectionEckert3::xyConv(const PhiLam& pl) const
{
  XY xy;
	if (fabs(xy.x = pl.Phi / M_PI_2) >= 1.)
		xy.x = XF * pl.Lam;
	else
		xy.x = XF * (1. + sqrt(1. - xy.x*xy.x)) * pl.Lam;
	xy.y = YF * pl.Phi;
	return (xy);
}
PhiLam ProjectionEckert3::plConv(const XY& _xy) const
{
  XY xy = _xy;
	double t;
  PhiLam pl;
	pl.Lam = xy.x * RXF;
	if (fabs(t = (pl.Phi = RYF * xy.y) / M_PI_2) < 1.)
		pl.Lam /= 1. + sqrt(1. - t*t);
	return (pl);
}





