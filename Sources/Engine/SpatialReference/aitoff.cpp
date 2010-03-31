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
/* ProjectionAitoff
	Copyright Ilwis System Development ITC
   nov 2004, by Jan Hendrikse
	Last change:  JHE  30 nov 2004    3:42 pm
*/

#include "Engine\SpatialReference\aitoff.h"
#define EPS8	1e-8


ProjectionAitoff::ProjectionAitoff()
{
}

//FORWARD(s_forward); (spherical)
XY ProjectionAitoff::xyConv(const PhiLam& pl) const
{
  XY xy;
	double cosa, b;
	if (abs(pl.Phi)+abs(pl.Lam) < EPS8)
	{
		xy.x =xy.y = 0;
		return xy; // the origin
	}
	else if (abs(M_PI_2 - abs(pl.Phi)) < EPS8 || 
		       abs(M_PI - abs(pl.Lam)) < EPS8) return xy; //the edge of the latlon rectangle
	b = acos(cos(pl.Phi) * cos(0.5 * pl.Lam));  /* basic Aitoff */
	if (abs(b) < EPS8) return xy;
	cosa = sin(pl.Phi) / sin(b);
	xy.x = 1. - cosa * cosa;
	if (abs(xy.x) < EPS8)
	  xy.x = 0.;
	else
			xy.x = 2. * b * sqrt(xy.x);
	if (pl.Lam < 0.) xy.x = - xy.x;
	xy.y = b * cosa;
	return xy;
}

PhiLam ProjectionAitoff::plConv(const XY& xy) const
{
  PhiLam pl;
	double a, b;
	if (abs(xy.y) < EPS8) return pl;
	a = atan(xy.x/2/xy.y);
	if (xy.y > 0)
		b = sqrt(xy.x * xy.x + 4*xy.y * xy.y)/2;
	else
		b = -sqrt(xy.x*xy.x + 4*xy.y*xy.y)/2;
	if (abs(cos(b)) < EPS8) return pl;
	double sinPhi = cos(a) * sin(b);
	if (abs(abs(sinPhi) - 1) < EPS8) return pl;
	pl.Phi = asin(sinPhi);
	double cosPhi = sqrt(1 - sinPhi*sinPhi);
	if (xy.x > 0)
		pl.Lam = 2*acos(cos(b)/cosPhi);
	else
		pl.Lam = -2*acos(cos(b)/cosPhi);
	double dXdPhi; double dXdLam;
	double dYdPhi; double dYdLam;
	if (abs(pl.Phi-M_PI_4) <0.01 && abs(pl.Lam-M_PI_4)<0.01)
		FindJacobian(pl.Lam, pl.Phi,
				 dXdPhi,  dXdLam,
				 dYdPhi,  dYdLam) ;
  return pl;
}

void ProjectionAitoff::FindJacobian(double Lam, double Phi,
				double& dX_dPhi, double& dX_dLam,
				double& dY_dPhi, double& dY_dLam)  const
{
	PhiLam pl1; pl1.Phi = Phi;pl1.Lam = Lam;
	PhiLam pl2; pl2.Phi = Phi + 0.00001;pl2.Lam = Lam + 0.00001;
	XY xy1 = xyConv(pl1);
	XY xy2 = xyConv(pl2);
	dX_dPhi = (xy2.x - xy1.x) / (pl2.Phi - pl1.Phi);
	dX_dLam = (xy2.x - xy1.x) / (pl2.Lam - pl1.Lam);
	dY_dPhi = (xy2.y - xy1.y) / (pl2.Phi - pl1.Phi);
	dY_dLam = (xy2.y - xy1.y) / (pl2.Lam - pl1.Lam);
}
