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
/* ProjectionEckert1
  Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
  Last change:  JHE  24 Jun 97    9:03 am
*/

#include "Engine\SpatialReference\ECKERT1.H"
#include "Engine\Base\DataObjects\ERR.H"
ProjectionEckert1::ProjectionEckert1()
{}

#define FC	.92131773192356127802
#define RP	.31830988618379067154

XY ProjectionEckert1::xyConv(const PhiLam& pl) const
{
  XY xy;
	xy.x = FC * pl.Lam * (1. - RP * fabs(pl.Phi));
	xy.y = FC * pl.Phi;
	return xy;
}
PhiLam ProjectionEckert1::plConv(const XY& xy) const
{
  PhiLam pl;
	pl.Phi = xy.y / FC;
	pl.Lam = xy.x / (FC * (1. - RP * fabs(pl.Phi)));
	return pl;
}





