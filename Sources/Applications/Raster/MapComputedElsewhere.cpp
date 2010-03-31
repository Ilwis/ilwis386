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
/* MapComputedElsewhere
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  JH   18 Nov 99   12:20 pm
*/
#include "Engine\Applications\MAPFMAP.H"
#include "Applications\Raster\MapComputedElsewhere.h"

#include "Engine\Base\DataObjects\valrange.h"

IlwisObjectPtr * createMapComputedElsewhere(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapComputedElsewhere::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapComputedElsewhere(fn, (MapPtr &)ptr);
}

MapComputedElsewhere* MapComputedElsewhere::create(const FileName& fn, MapPtr& p)
{ 
    return new MapComputedElsewhere(fn, p);  
}

MapComputedElsewhere* MapComputedElsewhere::create(const FileName& fn, MapPtr& p, const String& sExpr )
{ 
	Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms !=1)
    ExpressionError(sExpr, "MapComputedElsewhere(mpComputor)");
  Map mpComputor(as[0], fn.sPath());
  return new MapComputedElsewhere(fn, p, mpComputor);  
}

MapComputedElsewhere::MapComputedElsewhere(const FileName& fn, MapPtr& p)
: MapVirtual(fn, p)
{
  ReadElement("MapComputedElsewhere", "Map", mpComputor);
  objdep.Add(mpComputor.ptr());
}

MapComputedElsewhere::MapComputedElsewhere(const FileName& fn, MapPtr& p,
           const Map& mpComp)
: MapVirtual(fn, p, mpComp->gr(), mpComp->rcSize(), mpComp->dvrs()), mpComputor(mpComp)
{
  ptr.SetSize(mpComp->rcSize());
  SetCoordSystem(mpComp->cs());
  SetCoordBounds(mpComp->cb());
  objdep.Add(mpComp.ptr());
}

void MapComputedElsewhere::Store()
{
  MapVirtual::Store();
  WriteElement("MapVirtual", "Type", "MapComputedElsewhere");
  WriteElement("MapComputedElsewhere", "Map", mpComputor);
}

MapComputedElsewhere::~MapComputedElsewhere()
{
}

String MapComputedElsewhere::sExpression() const
{
	return String("-");
}

bool MapComputedElsewhere::fFreezing()
{
	if (ptr.fUpToDate())
		return true;
	if (!mpComputor->fCalculated())
    mpComputor->Calc(); 
	return mpComputor->fCalculated();
}
