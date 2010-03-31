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
#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\Col.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Table\tbl.h"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\DataExchange\PointMapStoreForeign.h"

PointMapStoreForeign::PointMapStoreForeign(const FileName& fn, PointMapPtr& p, LayerInfo inf) :
	PointMapStore(fn, p, true)
{
	//if ( inf.tbl.fValid() ) // during a delete this may happen.does not matter there as the object is deleted anyway
	//{
	//	tbl = inf.tbl;
	//	colCoord = tbl->col("Coordinate");
	//	colVal = tbl->col("Name");

	//	long l;
	//	if (0 == ptr.ReadElement("PointMapStore", "StoreTime", l))
	//		timStore = tbl->objtime;
	//	else
	//	 timStore = l;
	//}
	Table tbl = inf.tbl;
	long iNr = tbl->iRecs();;
	Column colCrd = tbl->col("Coordinate");
	Column colValue = tbl->col("Name");
	
	for (long i = 0; i < iNr; ++i) 
	{
		SetPoint(colCrd->cValue(i), i, colValue);
	}
}

PointMapStoreForeign::~PointMapStoreForeign()
{
}

void PointMapStoreForeign::Store()
{
	if ( ptr.fUseAs())
		ptr.WriteElement("TableStore", "Type", "TableForeign");
}

void PointMapStoreForeign::DoNotUpdate()
{
	PointMapStore::DoNotUpdate();
}
