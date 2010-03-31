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
/* MapFromMap
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  JEL  13 Jul 97   11:01 pm
*/
#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\valrange.h"

MapFromMap* MapFromMap::create(const FileName& fn, MapPtr& p)
{
  String sType;
  ObjectInfo::ReadElement("MapFromMap", "Type", fn, sType);

  	ApplicationInfo * info = Engine::modules.getAppInfo(sType);
	if ( info != NULL ) {
		vector<void *> extraParms = vector<void *>();
		return (MapFromMap *)(info->createFunction)(fn, p, "", extraParms);
	}

 	InvalidTypeError(fn, "MapFromMap", sType);
  return 0;
}

MapFromMap::MapFromMap(const FileName& fn, MapPtr& p)
: MapVirtual(fn, p)
{
  ReadElement("MapFromMap", "Map", mp);
	if (!mp.fValid()) 
		throw ErrorDummy(); // error already reported in ReadElement
  ptr._gr = mp->gr();
  ptr._rcSize = mp->rcSize();
  objdep.Add(mp.ptr());
}

MapFromMap::MapFromMap(const FileName& fn, MapPtr& p,
           const Map& mp, MapFormat mf)
: MapVirtual(fn, p, mp->gr(), mp->rcSize(), mp->dvrs(), mf), mp(mp)
{
  ptr.SetSize(mp->rcSize());
  SetCoordSystem(mp->cs());
  SetCoordBounds(mp->cb());
  objdep.Add(mp.ptr());
  if (mp->fTblAttSelf())
    SetAttributeTable(mp->tblAtt());
}

void MapFromMap::Store()
{
  MapVirtual::Store();
  WriteElement("MapVirtual", "Type", "MapFromMap");
  WriteElement("MapFromMap", "Map", mp);
}

MapFromMap::~MapFromMap()
{
}



