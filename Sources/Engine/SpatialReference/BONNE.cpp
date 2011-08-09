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
/* $Log: /ILWIS 3.0/CoordinateSystem/BONNE.cpp $
 * 
 * 5     3-01-05 18:17 Hendrikse
 * put pvLATTS in st of pvLAT0 (Esri uses Stand_Parallel_1 for it)
 * 
 * 4     6/07/01 18:41 Hendrikse
 * implemented correct use of pvLATTS and phits 
 * 
 * 3     9-12-99 9:37 Koolhoven
 * Header comment
 * 
 * 2     2-12-99 14:20 Hendrikse
 * made better variable names cotPhi0, M1, made other vars explicit like
 * rSinphi, rCosPhi, rh,E; Added the fNorth check to correct atan2 calls
 * for the southern hemisphere (phits<0) in plConv for pl.Lam computation.
//Revision 1.2  1998/10/05 13:03:59  janh
//userParam pvk0 disabled
//
//Revision 1.1  1998/09/16 17:26:42  Wim
//22beta2
//
/* ProjectionBonne
	 Copyright Ilwis System Development ITC
	 october 1997, by Jan Hendrikse
	Last change:  JHE  10 Oct 97    5:15 pm
*/

#include "Engine\SpatialReference\BONNE.H"
#include "Headers\Hs\proj.hs"

#define EPS10	1.e-10
/* members defined in prj.h
	double phits; \ // latit of true scale
/* members defined in bonne.h
	double cotPhi0; \
	double am1; \
	double M1; \
	double *en;  // of misschien en[5] ? zie eckert of robinson
*/

ProjectionBonne::ProjectionBonne(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
	fEll = true;
	fUseParam[pvLATTS] = true;
	//fUseParam[pvK0] = true;
	Param(pvLATTS, 1.0);     // default latit of true scale diff from 0
}

void ProjectionBonne::Prepare()
{
	double c;
	 //P->phi1 = pj_param(P->params, "rlat_1").f;
	if (abs(phits) < EPS10) //E_ERROR(-23);
		throw ErrorObject(TR("Latitude of True Scale should not be 0"), 2211);
	fNorth = (phits > 0);
	if (ell.e2 > 0) {
		enfn(en);
		am1 = sin(phits);
		c = cos(phits);
		M1 = mlfn(phits, am1, c, en);
		am1 = c / (sqrt(1. - ell.e2 * am1 * am1) * am1);
	}
	else
	{
		if (abs(phits) + EPS10 >= M_PI_2)
			cotPhi0 = 0.;
		else
			cotPhi0 = 1. / tan(phits);
	}
}

XY ProjectionBonne::xyConv(const PhiLam& pl) const
{
	XY xy;
	if (!ell.fSpherical()) { /* ellipsoid */

		double  rSinPhi = sin(pl.Phi);
		double  rCosPhi = cos(pl.Phi);
		double rh = am1 + M1 - mlfn(pl.Phi, rSinPhi , rCosPhi , en);
		double E = rCosPhi * pl.Lam / (rh * sqrt(1. - ell.e2 * rSinPhi * rSinPhi));
		xy.x = rh * sin(E);
		xy.y = am1 - rh * cos(E);
		return xy;
	}
	else { /* sphere */
		double rh = cotPhi0 + phits - pl.Phi;
		if (abs(rh) > EPS10) {
			double E = pl.Lam * cos(pl.Phi) / rh;
			xy.x = rh * sin(E);
			xy.y = cotPhi0 - rh * cos(E);
		}
		else
			xy.x = xy.y = 0.;
		return xy;
	}
}

PhiLam ProjectionBonne::plConv(const XY& _xy) const
{
	PhiLam pl;
	XY xy = _xy;
	double newY;
	double rSignPhi0 = (fNorth) ? 1.0 : -1.0;
	if (ell.fSpherical()) { /* sphere */
		newY = cotPhi0 - xy.y;
		double rh = sqrt(xy.x*xy.x + newY*newY) * rSignPhi0;
		// rh = hypot(xy.x, xy.y = cotPhi0 - xy.y);
		pl.Phi = cotPhi0 + phits - rh;
		if (abs(pl.Phi) > M_PI_2) return pl;
		if (abs(abs(pl.Phi) - M_PI_2) <= EPS10)
			pl.Lam = 0.;
		else
			if (fNorth)
			  pl.Lam = rh * atan2(xy.x, newY) / cos(pl.Phi);
			else
				pl.Lam = rh * atan2(-xy.x, -newY) / cos(pl.Phi);
		return pl;
	}
	else /* ellipsoid */
	{
		double s, rh;
		//rh = hypot(xy.x, xy.y = am1 - xy.y);
		newY = am1 - xy.y;
		rh = sqrt(xy.x*xy.x + newY*newY) * rSignPhi0;
		pl.Phi = invmlfn(am1 + M1 - rh, ell.e2, en);
		if ((s = abs(pl.Phi)) < M_PI_2)
		{
			s = sin(pl.Phi);
			if (fNorth)
			  pl.Lam = rh * atan2(xy.x, newY) *
					sqrt(1. - ell.e2 * s * s) / cos(pl.Phi);
			else
				pl.Lam = rh * atan2(-xy.x, -newY) *
					sqrt(1. - ell.e2 * s * s) / cos(pl.Phi);
		}
		else if (abs(s - M_PI_2) <= EPS10)
			pl.Lam = 0.;
		//else I_ERROR;
		return pl;
	}
}





