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
/* MapFromPointMap
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  JEL   7 May 97    9:39 am
*/
#include "Engine\Applications\MAPFPNT.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\SpatialReference\Distance.h"



MapFromPointMap* MapFromPointMap::create(const FileName& fn, MapPtr& p)
{
  String sType;     
  ObjectInfo::ReadElement("MapFromPointMap", "Type", fn, sType);

  	ApplicationInfo * info = Engine::modules.getAppInfo(sType);
	if ( info != NULL ) {
		vector<void *> extraParms = vector<void *>();
		return (MapFromPointMap *)(info->createFunction)(fn, p, "", extraParms);
	}
 
  InvalidTypeError(fn, "MapFromPointMap", sType);
  return 0;
}

MapFromPointMap::MapFromPointMap(const FileName& fn, MapPtr& p)
: MapVirtual(fn, p)
{
  ReadElement("MapFromPointMap", "PointMap", pmp);
	if (!pmp.fValid()) 
		throw ErrorDummy(); // error already reported in ReadElement
	///ReadElement("MapFromPointMap", "SphericalDistance", fSphericalDistance);
	m_distMeth = Distance::distPLANE;	//default
  String sDistanceMethod;
  ReadElement("MapFromPointMap", "DistanceMethod",sDistanceMethod);  
  if (fCIStrEqual("sphere", sDistanceMethod)) 
    m_distMeth = Distance::distSPHERE;
	else
		m_distMeth = Distance::distPLANE;
	CoordSystemLatLon* csllIn = pmp->cs()->pcsLatLon();
	fCsyLatLonIn  = (0 != csllIn) ? true : false;
	CoordSystemLatLon* csllOut = cs()->pcsLatLon();
	fCsyLatLonOut  = (0 != csllOut) ? true : false;
	objdep.Add(pmp.ptr());
}

MapFromPointMap::MapFromPointMap(const FileName& fn, MapPtr& p,
           const PointMap& pmap, const GeoRef& gr)
: MapVirtual(fn, p, gr, gr->rcSize(), pmap->dvrs()), pmp(pmap)
{
  objdep.Add(pmp.ptr());
  if (pmp->fTblAtt())
    SetAttributeTable(pmp->tblAtt());
	CoordSystemLatLon* csllIn = pmp->cs()->pcsLatLon();
	fCsyLatLonIn  = (0 != csllIn) ? true : false;
	CoordSystemLatLon* csllOut = cs()->pcsLatLon();
	fCsyLatLonOut  = (0 != csllOut) ? true : false;
}

void MapFromPointMap::Store()
{
  MapVirtual::Store();
  WriteElement("MapVirtual", "Type", "MapFromPointMap");
  WriteElement("MapFromPointMap", "PointMap", pmp);
	String sDistanceMethod;
	switch (m_distMeth) {
    case Distance::distPLANE: sDistanceMethod = "plane"; break; 
    case Distance::distSPHERE: sDistanceMethod = "sphere"; break; 
  } 
	WriteElement("MapFromPointMap", "DistanceMethod", sDistanceMethod);
	///WriteElement("MapFromPointMap", "sphereDistance", fsphereDistance);
}

MapFromPointMap::~MapFromPointMap()
{
}

bool MapFromPointMap::fGeoRefChangeable() const
{
  return true;
}
/*
double MapFromPointMap::rDistance(const Coord crd1, const Coord crd2)
{
	double rDistan;
	switch(distMeth) {
		case  distplane:
			rDistan = rDist(crd1, crd2); break;
		case   distsphere: 
		{
			double rEarthRadius = 6371007;
			LatLon llP1, llP2;
			if (!fCsyLatLonOut) {
				llP1 = cs()->llConv(crd1);
				llP2 = cs()->llConv(crd2);
			}
			else {
				llP1 = LatLon(crd1.y, crd1.x);
				llP2 = LatLon(crd2.y, crd2.x);
			}
			rDistan = rsphereDistance(rEarthRadius, llP1, llP2);
		}
	}
	return rDistan;
}

double MapFromPointMap::rDistance2(const Coord crd1, const Coord crd2)
{
	double rDistan2;
	switch(distMeth) {
		case  distplane:
			rDistan2 = rDist2(crd1, crd2); break;
		case   distsphere:
		{
			double rEarthRadius = 6371007;
			LatLon llP1, llP2;
			if (!fCsyLatLonOut) {
				llP1 = cs()->llConv(crd1);
				llP2 = cs()->llConv(crd2);
			}
			else {
			  llP1 = LatLon(crd1.y, crd1.x);
			  llP2 = LatLon(crd2.y, crd2.x);
			}
			rDistan2 = rSphericalDistance(rEarthRadius, llP1, llP2);
			rDistan2 *= rDistan2;
		}
	}
	return rDistan2;
}
*/
double MapFromPointMap::rLimDistDefault(const PointMap& pmap, bool fSphericalDistance)
{
  CoordBounds cb = pmap->cb();
  double rPointDensity = pmap->iFeatures() / (cb.MaxX() - cb.MinX()) / (cb.MaxY() - cb.MinY());
	CoordSystemLatLon* csllIn = pmap->cs()->pcsLatLon();
	CoordSystemProjection* csprojIn = pmap->cs()->pcsProjection();
	CoordSystem csy; 
	csy.SetPointer(csllIn);;
	LatLon llLowLeft, llUpLeft;
	LatLon llUpRight, llLowRight;
	Distance::DistanceMethod disMeth = 
		fSphericalDistance ? Distance::distSPHERE : Distance::distPLANE;
	Distance dis(pmap->cs(),  disMeth);
	
	/*
	if (fSphericalDistance   // if user wants spherical distance
			|| (0 != csllIn) )   // latlon systems must use it 
	{
		double rEarthRadius = 6371007;
		if (0 != csprojIn) {  
		  llLowLeft		= csprojIn->llConv(cb.cMin); 
		  llUpRight		= csprojIn->llConv(cb.cMax); 
			llLowRight	= csprojIn->llConv(Coord(cb.MaxX(),cb.MinY())); 
		  llUpLeft		= csprojIn->llConv(Coord(cb.MinX(),cb.MaxY())); 
		}
		else if (0 != csllIn) {
			llLowLeft		= LatLon(cb.MinY(), cb.MinX()); 
		  llUpRight		= LatLon(cb.MaxY(), cb.MaxX()); 
			llLowRight	= LatLon(cb.MinY(), cb.MaxX()); 
		  llUpLeft		= LatLon(cb.MaxY(), cb.MinX()); 
		} */
		//double rDiagonalArcLength = rSphericalDistance(csllIn->ell.a, llLowLeft, llUpRight);
		//double rDiagonalArcLengthSimple = rDistance2(cb.cMin, cb.cMax); //testing
		//rPointDensity  = pmap->iFeatures() / rDiagonalArcLengthSimple / rDiagonalArcLengthSimple;
		///double rMeridiansArcLength = rSphericalDistance(rEarthRadius, llLowLeft, llUpLeft);
		///double rLowerParallelLength = rSphericalDistance(rEarthRadius, llLowLeft, llLowRight);
		///double rUpperParallelLength = rSphericalDistance(rEarthRadius, llUpLeft, llUpRight);
	double rMeridiansArcLength = dis.rDistance(cb.cMin, Coord(cb.MinX(),cb.MaxY()));
	double rLowerParallelLength = dis.rDistance(cb.cMin, Coord(cb.MaxX(),cb.MinY()));
	double rUpperParallelLength = dis.rDistance(Coord(cb.MinX(),cb.MaxY()), cb.cMax);
	double rGraticulQuadrangleArea = (rLowerParallelLength + rUpperParallelLength) * 
																			rMeridiansArcLength / 2;
	rPointDensity  = pmap->iFeatures() / rGraticulQuadrangleArea;
	
	return sqrt(5 / rPointDensity);
}
