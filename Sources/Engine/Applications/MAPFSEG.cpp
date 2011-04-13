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
/* MapFromSegmentMap
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  JEL   7 May 97    9:39 am
*/
#include "Engine\Applications\MAPFSEG.H"
//#include "Applications\Raster\MAPITPCN.H"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\valrange.h"

MapFromSegmentMap* MapFromSegmentMap::create(const FileName& fn, MapPtr& p)
{
	String sType;
	if (0 == ObjectInfo::ReadElement("MapFromSegmentMap", "Type", fn, sType))
		return 0;

	vector<CommandInfo *> infos;
    Engine::modules.getCommandInfo(sType, infos);
	if ( infos.size() > 0 ) {
		vector<void *> extraParms = vector<void *>();
		return (MapFromSegmentMap *)(infos[0]->createFunction)(fn, p, "", extraParms);
	}
	InvalidTypeError(fn, "MapFromSegmentMap", sType);
	return 0;
}


MapFromSegmentMap::MapFromSegmentMap(const FileName& fn, MapPtr& p)
: MapVirtual(fn, p)
{
  FileName fnSeg;
  ReadElement("MapFromSegmentMap", "SegmentMap", sm);
	if (!sm.fValid()) 
		throw ErrorDummy(); // error already reported in ReadElement
  objdep.Add(sm.ptr());
}

MapFromSegmentMap::MapFromSegmentMap(const FileName& fn, MapPtr& p,
           const SegmentMap& segm, const GeoRef& gr, const MapFormat mf)
: MapVirtual(fn, p, gr, gr->rcSize(), segm->dvrs(), mf), sm(segm)
{
  objdep.Add(sm.ptr());
  if (sm->fTblAtt())
    SetAttributeTable(sm->tblAtt());
}

void MapFromSegmentMap::Store()
{
  MapVirtual::Store();
  WriteElement("MapVirtual", "Type", "MapFromSegmentMap");
  WriteElement("MapFromSegmentMap", "SegmentMap", sm);
}

MapFromSegmentMap::~MapFromSegmentMap()
{
}

bool MapFromSegmentMap::fGeoRefChangeable() const
{
  return true;
}

