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
#include "Engine\SpatialReference\Distance.h"
#include "Engine\SpatialReference\csviall.h"

static double rSphericalDistance(double rRadius, const LatLon& ll_1, const LatLon& ll_2)
{
		if (ll_1.fUndef() || ll_2.fUndef())
		return rUNDEF;
// ll_1 and ll_2 are supposed LatLons in degrees
	double phi1 = ll_1.Lat * M_PI/180.0; //conversion to radians
  double lam1 = ll_1.Lon * M_PI/180.0; 
  double phi2 = ll_2.Lat * M_PI/180.0; ;
  double lam2 = ll_2.Lon * M_PI/180.0; ;
  double sinhalfc = fabs(sin((phi2- phi1)/2)* sin((phi2- phi1)/2) +
		 cos(phi1)*cos(phi2)* sin((lam2 - lam1)/2)* sin((lam2 - lam1)/2));
	sinhalfc = sqrt(sinhalfc);
  double c; // the shortest spherical arc
  if (sinhalfc < sqrt(2.0)/2)
		c = 2.0 * asin(sinhalfc);
	else 
	{
		phi2 = -phi2;
		lam2 = M_PI + lam2;
		sinhalfc = fabs(sin((phi2- phi1)/2)* sin((phi2- phi1)/2)+
			cos(phi1)*cos(phi2)*sin((lam2 - lam1)/2)*sin((lam2 - lam1)/2));
		sinhalfc = sqrt(sinhalfc);
		c = M_PI - 2.0 * asin(sinhalfc);
	}
	return c * rRadius;
}

Distance::Distance()
	: cs(CoordSystem()),
	  distMeth(distPLANE)
{
	m_fCsyViaLatLon = false;
	m_fCsyProjected = false;
	m_rEarthRadius = 6371007.1809185;
}

Distance::Distance(const CoordSystem& csy, DistanceMethod distM)
	: cs(csy), distMeth(distM)
{
	m_fCsyViaLatLon = cs.fValid() && (cs->pcsViaLatLon() != 0);
	m_fCsyProjected = cs.fValid() && (cs->pcsProjection() != 0);
	if (m_fCsyViaLatLon == true || m_fCsyProjected == true)
		m_rEarthRadius = cs->pcsViaLatLon()->ell.a;
	else
		m_rEarthRadius = 6371007.1809185;//needed if no ellipsoid available
}

Distance::Distance(const CoordSystem& csy, DistanceMethod distM, double rRadius)
	: cs(csy), distMeth(distM)
{
	m_fCsyViaLatLon = cs.fValid() && (cs->pcsViaLatLon() != 0);
	m_fCsyProjected = cs.fValid() && (cs->pcsProjection() != 0);
	m_rEarthRadius = rRadius;
}

double Distance::rDistance(const Coord& crd1, const Coord& crd2)
{
	double rDistan;
	if ((distMeth == distSPHERE) && m_fCsyViaLatLon)
	{
		LatLon llP1, llP2;
		if (m_fCsyProjected)
		{
			llP1 = cs->llConv(crd1);
			llP2 = cs->llConv(crd2);
		}
		else 
		{
			llP1 = LatLon(crd1.y, crd1.x);
			llP2 = LatLon(crd2.y, crd2.x);
		}
		rDistan = rSphericalDistance(m_rEarthRadius, llP1, llP2);
	}
	else	
		rDistan = sqrt(rDist2(crd1, crd2));

	return rDistan;
}

double Distance::rDistance2(const Coord& crd1, const Coord& crd2)
{
	double rDistan;
	if ((distMeth == distSPHERE) && m_fCsyViaLatLon)
	{
		LatLon llP1, llP2;
		if (m_fCsyProjected)
		{
			llP1 = cs->llConv(crd1);
			llP2 = cs->llConv(crd2);
		}
		else 
		{
			llP1 = LatLon(crd1.y, crd1.x);
			llP2 = LatLon(crd2.y, crd2.x);
		}
		rDistan = rSphericalDistance(m_rEarthRadius, llP1, llP2);
		rDistan *= rDistan;
	}
	else	
		rDistan = rDist2(crd1, crd2);

	return rDistan;
}
