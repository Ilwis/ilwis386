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
/* ProjectionWagner7
   Copyright Ilwis System Development ITC
   nov 2004, by Jan Hendrikse
	Last change:  JHE  30 nov 2004    3:42 pm
*/

#include "Engine\SpatialReference\wagner7.h"
#include "Engine\SpatialReference\aitoff.h"

#define EPS10  1.e-10
#define THIRD	0.3333333333333333333

ProjectionWagner7::ProjectionWagner7()
{
  fUseParam[pvLATTS] = true; 
}

XY ProjectionWagner7::xyConv(const PhiLam& _pl) const
{
  XY xy;
	PhiLam pl = _pl;
  double s, c0, c1;

	s = 0.90631 * sin(pl.Phi);
	c0 = sqrt(1. - s * s);
	c1 = sqrt(2./(1. + c0 * cos(pl.Lam *= THIRD)));
	xy.x = 2.66723 * c0 * c1 * sin(pl.Lam);
	xy.y = 1.24104 * s * c1;
  return xy;
}

PhiLam ProjectionWagner7::plConv(const XY& xy) const
{
  PhiLam pl, plNext;
  XY xytemp;
  double deltaX, deltaY;
  double deltaLam = 200, deltaPhi = 200;
  int count = 0;
  double dLam_dX, dLam_dY;
  double dPhi_dX, dPhi_dY;
  double dXdLam, dYdLam;
  double dXdPhi, dYdPhi;
  
  plNext.Phi = xy.y;      
  plNext.Lam = xy.x / n/1.061;
  if (abs(pl.Phi) < EPS10 || abs(pl.Lam) < EPS10) return plNext;
  
  while (abs(deltaLam) + abs(deltaPhi) > 100 && count < 20)
  {
	xytemp = xyConv(plNext);
	count++;
	// find from the xyConv formulas respectively:
	// dX/dLam,   dX/dPhi, Jacobian of xyConv 
	// dY/dLam,   dY/dPhi, for approxim pl (see (*))
	FindJacobian( plNext.Lam,  plNext.Phi,
				 dXdLam,  dXdPhi,
				 dYdLam,  dYdPhi);
	if (abs(dXdLam) < EPS10 || abs(dYdLam) < EPS10) return plNext;
	dLam_dX = 1/dXdLam/1.061; dLam_dY = 1/dYdLam;
	if (abs(dXdPhi) < EPS10 || abs(dYdPhi) < EPS10) return plNext;
	dPhi_dX = 1/dXdPhi; dPhi_dY = 1/dYdPhi;
	double det = dXdLam*dYdPhi - dYdLam*dXdPhi;
	deltaX = xy.x - xytemp.x;
	deltaY = xy.y - xytemp.y;
	//deltaLam = deltaX*dLam_dX + deltaY*dLam_dY;// Jacobian of plConv
	//deltaPhi = deltaX*dPhi_dX + deltaY*dPhi_dY;// to beevaluated for plNext
	deltaLam = (deltaX*dYdPhi - deltaY*dXdPhi)/det;// Jacobian inverted
	deltaPhi = (-deltaX*dYdLam + deltaY*dXdLam)/det;
	plNext.Lam = plNext.Lam + deltaLam;
	plNext.Phi = plNext.Phi + deltaPhi;
  }
  if (count < 20)
  {
	pl.Lam = plNext.Lam;
	pl.Phi = plNext.Phi;
  }
  return pl;
}

void ProjectionWagner7::FindJacobian(double Lam, double Phi,
				double& dX_dLam, double& dX_dPhi,
				double& dY_dLam, double& dY_dPhi) const
{
	double coscos = cos(Lam/2)*cos(Phi);
	double b = acos(coscos);
	double sinsin = sin(Phi)/sin(b);
	double a = acos(sinsin);
	double dAcos = -1/sqrt(1 - coscos*coscos);
	double db_dLam = -dAcos * sin(Lam/2)*cos(Phi)/2;
	double db_dPhi = -dAcos * cos(Lam/2)*sin(Phi);
	dAcos = -1/sqrt(1 - sinsin*sinsin);
	double da_dLam = dAcos * (-sin(Phi)*cos(b)*db_dLam)/sin(b)/sin(b);
	double da_dPhi = dAcos * (sin(b)*cos(Phi)-sin(Phi)*cos(b)*db_dPhi)/sin(b)/sin(b);
	if (Lam < 0)
	{
		dX_dLam = (n - 2*db_dLam*sin(a) - 2*b*cos(a)*da_dLam)/2;
		dX_dPhi = -db_dPhi*sin(a) - b*cos(a)*da_dPhi;
	}
	else
	{
		dX_dLam = (n + 2*db_dLam*sin(a) + 2*b*cos(a)*da_dLam)/2;
		dX_dPhi = db_dPhi*sin(a) + b*cos(a)*da_dPhi;
	}
	dY_dLam = (db_dLam*cos(a) - b*sin(a)*da_dLam)/2;
	dY_dPhi = (1 + db_dPhi*cos(a) - b*sin(a)*da_dPhi)/2;
}

void ProjectionWagner7::Param(ProjectionParamValue pv, double rValue)
{
  ProjectionPtr::Param(pv, rValue);
  if (pvLATTS == pv) {
    n = cos(phits);
  }    
}      


