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
/* ProjectionEckert2
  Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
  Last change:  JHE  24 Jun 97    9:03 am
*/

#include "Engine\SpatialReference\ECKERT2.H"
#include "Engine\Base\DataObjects\ERR.H"

#define FXC	0.46065886596178063902
#define FYC	1.44720250911653531871
#define C13	0.33333333333333333333
#define ONEEPS	1.0000001
ProjectionEckert2::ProjectionEckert2()
{}

XY ProjectionEckert2::xyConv(const PhiLam& pl) const
{
  XY xy;
	xy.x = FXC * pl.Lam * (xy.y = sqrt(4. - 3. * sin(fabs(pl.Phi))));
	xy.y = FYC * (2. - xy.y);
	if ( pl.Phi < 0.) xy.y = -xy.y;
	return xy;
}
PhiLam ProjectionEckert2::plConv(const XY& _xy) const
{
  XY xy = _xy;
  PhiLam pl;
	pl.Lam = xy.x / (FXC * ( pl.Phi = 2. - fabs(xy.y) / FYC) );
	pl.Phi = (4. - pl.Phi * pl.Phi) * C13;
	if (fabs(pl.Phi) >= 1.) {
		if (fabs(pl.Phi) > ONEEPS)	return pl;
		else
			pl.Phi = pl.Phi < 0. ? -M_PI_2 : M_PI_2;
	} else
		pl.Phi = asin(pl.Phi);
	if (xy.y < 0)
		pl.Phi = -pl.Phi;
	return pl;
}





