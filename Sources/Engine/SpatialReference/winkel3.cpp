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
/* ProjectionWinkel3
   Copyright Ilwis System Development ITC
   nov 2004, by Jan Hendrikse
	Last change:  JHE  30 nov 2004    3:42 pm
*/

#include "Engine\SpatialReference\Winkel3.H"


#define EPS10  1.e-10

ProjectionWinkel3::ProjectionWinkel3()
{
  fUseParam[pvLATTS] = true; 
}

XY ProjectionWinkel3::xyConv(const PhiLam& pl) const
{
  XY xy;
  if ( abs(pl.Phi) >= M_PI_2 ) return xy;
	/*
 	double c, d;

	if (d = acos(cos(pl.Phi) * cos(0.5 * pl.Lam))) { // basic Aitoff 
		c = sin(pl.Phi) / sin(d);
		if ((xy.x = 1. - c * c) < EPS10)
			if (xy.x < -EPS10) return xy;
			else xy.x = 0.;
		else
			xy.x = 2. * d * sqrt(xy.x);
		if (pl.Lam < 0.) xy.x = - xy.x;
		xy.y = d * c;
	} 
	else
		xy.x = xy.y = 0.; */
	ProjectionPtr* ptr = new ProjectionAitoff();
	const ProjectionAitoff* prjAit = dynamic_cast<const ProjectionAitoff*>(ptr);
	xy = prjAit->xyConv(pl);
	xy.x = (xy.x + pl.Lam * n) * 0.5;
	xy.y = (xy.y + pl.Phi) * 0.5;
	return xy;
}

XY ProjectionWinkel3::xyConv1(const PhiLam& pl) const
{
  XY xy;
  if ( abs(pl.Phi) >= M_PI_2 ) return xy;
  double rDelta = acos( cos(pl.Lam/2) * cos(pl.Phi));
	double rAlpha = acos(sin(pl.Phi)/sin(rDelta));
	if (pl.Lam < 0)
		xy.x = (n * pl.Lam - 2 * rDelta * sin(rAlpha))/2;
	else
		xy.x = (n * pl.Lam + 2 * rDelta * sin(rAlpha))/2;
	xy.y = (pl.Phi + rDelta * cos(rAlpha))/2;
  return xy;
}
XY ProjectionWinkel3::xyConv0(const PhiLam& _pl) const //cf Ipbueker et al. Istanbul
{
  XY xy;
	PhiLam pl = _pl;
	double Lam = pl.Lam; double Phi = pl.Phi;
  if ( abs(pl.Phi) >= M_PI_2 ) return xy;
	double coscos = cos(Lam/2)*cos(Phi);
  double D = acos(coscos);
	double C = 1 - coscos*coscos;
	double DC2 = D/(sqrt(C));
	//if (pl.Lam < 0)
	//	xy.x = (n * pl.Lam - 2 * DC2*cos(Phi)*sin(Lam/2))/2;
	//else
	xy.x = (n * pl.Lam + 2 * DC2*cos(Phi)*sin(Lam/2))/2;
	xy.y = (pl.Phi + DC2 * sin(Phi))/2;
  return xy;
}

PhiLam ProjectionWinkel3::plConv0(const XY& xy) const
{
  PhiLam pl, plNext;
  XY xytemp;
  double deltaX, deltaY;
  double deltaLam = 0.1, deltaPhi = 0.1;
  int count = 0;
  double dXdLam, dYdLam;
  double dXdPhi, dYdPhi;
  
  plNext.Phi = xy.y;      
  plNext.Lam = xy.x / n;
  if (abs(pl.Phi) < EPS10 || abs(pl.Lam) < EPS10) return plNext;
  
  while (((abs(deltaLam) + abs(deltaPhi)) > 0.0001) && count < 200)
  {
	xytemp = xyConv(plNext);
	count++;
	// find from the xyConv formulas respectively:
	// dX/dLam,   dX/dPhi, Jacobian of xyConv 
	// dY/dLam,   dY/dPhi, for approxim pl (see (*))
		FindJacobian( plNext.Lam,  plNext.Phi,
				 dXdLam,  dXdPhi,
				 dYdLam,  dYdPhi);

	double det = dXdLam*dYdPhi - dYdLam*dXdPhi;
	deltaX = xytemp.x - xy.x;
	deltaY = xytemp.y - xy.y;
	deltaLam = (deltaX*dYdPhi - deltaY*dXdPhi)/det;// Jacobian inverted
	deltaPhi = (-deltaX*dYdLam + deltaY*dXdLam)/det;
	plNext.Lam = plNext.Lam + deltaLam;
	plNext.Phi = plNext.Phi + deltaPhi;
  }
  if (count < 200)
  {
	pl.Lam = plNext.Lam;
	pl.Phi = plNext.Phi;
  }
  return pl;
}

void ProjectionWinkel3::FindJacobian0(double Lam, double Phi,
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

PhiLam ProjectionWinkel3::plConv(const XY& _xy) const
{
	PhiLam pl;
	XY xy = _xy;
	PhiLam plNext;
	plNext.Phi = xy.y; //initial guess     
  plNext.Lam = xy.x / n;
  XY xyNext = xyConv(plNext); 
	xyNext.x += 0.0001; xyNext.y += 0.0001; // to allow the while loop  to start
	double deltaX = (xyNext.x - xy.x)/2;
	double deltaY = (xyNext.y - xy.y)/2;
  double deltaLam = 0.1;
	double deltaPhi = 0.1;
	int count = 0;
	double dXdLam, dYdLam;
  double dXdPhi, dYdPhi;
	double dXdLamW, dYdLamW;
  double dXdPhiW, dYdPhiW;
  ProjectionPtr* ptr = new ProjectionAitoff();
	const ProjectionAitoff* prjAit = dynamic_cast<const ProjectionAitoff*>(ptr);
  //Projection prj = cspr->prj;//
	while (((abs(deltaLam) > 0.000001) || (abs(deltaPhi)> 0.000001)) && (count < 300))
  {
		xy = xyNext;
		xyNext = xyConv(plNext);  // eq 3
		if (xyNext.x == rUNDEF || xyNext.y == rUNDEF)
			break;
		prjAit->FindJacobian( plNext.Lam,  plNext.Phi,
				  dXdPhi, dXdLam, 
				  dYdPhi, dYdLam);
		// change from Aitoff Jacobian to Winkel Jacobian:
		if (xyNext.x < 0) {
			dXdLamW = (n - dXdLam)/2; 
			dXdPhiW = - dXdPhi/2;
		}
		else
		{
			dXdLamW = (n + dXdLam)/2; 
			dXdPhiW = dXdPhi/2;
		}
		dYdLamW = dYdLam/2;
		dYdPhiW = (dYdPhi + 1)/2;
		double det = dXdPhiW*dYdLamW - dXdLamW*dYdPhiW;
		deltaX = (xyNext.x - xy.x)/2;
		deltaY = (xyNext.y - xy.y)/2;
		deltaPhi = (deltaX*dYdLamW - deltaY*dXdLamW) /det; // eq 15
		deltaLam = (deltaY*dXdPhiW - deltaX*dYdPhiW) /det; // eq 16
		plNext.Phi -= deltaPhi;
		plNext.Lam -= deltaLam;
		count++;
	}
	//if (count < 200)
		pl = plNext;
	return pl;
}
	
void ProjectionWinkel3::FindJacobian(double Lam, double Phi,
				double& dX_dPhi, double& dX_dLam,
				double& dY_dPhi, double& dY_dLam)  const
{
	double coscos = cos(Lam/2)*cos(Phi);
	double D = acos(coscos);
	double C = 1 - coscos*coscos;
	double DC32 = D/(sqrt(C*C*C));
	double cossin = cos(Phi)/sin(Lam/2);
	//double b = acos(coscos);
	//double sinsin = sin(Phi)/sin(b);
	//double a = acos(sinsin);
	//double dAcos = -1/sqrt(1 - coscos*coscos);
	//double db_dLam = -dAcos * sin(Lam/2)*cos(Phi)/2;
	//double db_dPhi = -dAcos * cos(Lam/2)*sin(Phi);
	//dAcos = -1/sqrt(1 - sinsin*sinsin);
	//double da_dLam = dAcos * (-sin(Phi)*cos(b)*db_dLam)/sin(b)/sin(b);
	//double da_dPhi = dAcos * (sin(b)*cos(Phi)-sin(Phi)*cos(b)*db_dPhi)/sin(b)/sin(b);

	double sinLamsin2Phi = sin(Lam)*sin(2*Phi);
	double sinPhisinLam_2 = sin(Phi)*sin(Lam/2);
	//sinPhisinLam_2 = sin(Phi)*sin(Lam/2);
	dX_dPhi = sinLamsin2Phi/4/C - DC32*sinPhisinLam_2;
	dX_dLam = (cossin*cossin/C + DC32*coscos*sin(Phi)*sin(Phi) + n)/2;
	dY_dPhi = (sin(Phi)*sin(Phi)*cos(Lam/2)/C + DC32*(1-cos(Lam/2)*cos(Lam/2))*cos(Phi)+1)/2;
	dY_dLam = (sin(2*Phi)*sin(Lam/2)/C - DC32*cos(Phi)*cos(Phi)*sin(Phi)*sin(Lam))/8;

/*
	if (Lam < 0)
	{
		df1_dLam = (n - 2*db_dLam*sin(a) - 2*b*cos(a)*da_dLam)/2;
		df1_dPhi = -db_dPhi*sin(a) - b*cos(a)*da_dPhi;
	}
	else
	{
		df1_dLam = (n + 2*db_dLam*sin(a) + 2*b*cos(a)*da_dLam)/2;
		df1_dPhi = db_dPhi*sin(a) + b*cos(a)*da_dPhi;
	}
	df2_dLam = (db_dLam*cos(a) - b*sin(a)*da_dLam)/2;
	df2_dPhi = (1 + db_dPhi*cos(a) - b*sin(a)*da_dPhi)/2;
*/
}

void ProjectionWinkel3::Param(ProjectionParamValue pv, double rValue)
{
  ProjectionPtr::Param(pv, rValue);
  if (pvLATTS == pv) {
    n = cos(phits);
  }    
}      


