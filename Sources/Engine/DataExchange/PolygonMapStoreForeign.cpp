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
#include "Engine\Table\tbl.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\Col.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\PolygonMapStoreForeign.h"
#include "Engine\Table\Colbinar.h"

PolygonMapStoreForeign::PolygonMapStoreForeign(const FileName& fn, PolygonMapPtr& p,LayerInfo inf) :
	PolygonMapStore(p, fn)
{
 /* if ( li.tbl.fValid())		
	{
		sm = SegmentMap(fn, li); 
		tblPolygon= li.tbl;
		tblTopology = li.tblTop;
		long l;
		if (0 == ptr.ReadElement("PolygonMapStore", "StoreTime", l))
			timStore = tblPolygon->objtime;
		else
			timStore = l;
	}	*/	

		if ( inf.sExpr == "import")
			p.setVersionBinary(ILWIS::Version::bvFORMAT30);
		else
			p.setVersionBinary(ILWIS::Version::bvFORMATFOREIGN);
		
		long l;
		if (0 == ptr.ReadElement("PolygonMapStore", "StoreTime", l))
			timStore = l;
		else
			timStore = l;

		Table tbl = inf.tbl;
		long iNr = tbl->iRecs();;
		Column colCrd = tbl->col("Coords");
		Column colValue = tbl->col("PolygonValue");
		
		Tranquilizer trq("Loading data");
		bool fValues = colValue->dvrs().fValues();
		bool fUseReals = fValues && colValue->dvrs().fUseReals(); // ptr.dvrs().fRealValues();
		for(long i = 1; i <= iNr; ++i) {
			ILWIS::Polygon *pol;
			if (fUseReals){
				pol = new ILWIS::RPolygon(spatialIndex);
			} else {
				pol = new ILWIS::LPolygon(spatialIndex);
			}
			double value;
			if (fValues) {
				if (fUseReals)
					value = colValue->rValue(i);
				else
					value = colValue->iValue(i);
			} else
				value = colValue->iRaw(i);
			BinMemBlock bmb;
			colCrd->GetVal(i, bmb);
			char * b = (char*)bmb.ptr();
			if (b != 0) {
				long iNrPolygons;
				FFBlobUtils::ReadBuf(&b, iNrPolygons);
				for (long j = 0; j < iNrPolygons; ++j) {
					if (j >= 1) {
						TRACE("Warning: multipolygon encountered consisting of %d polygons; not yet supported\n", iNrPolygons);
						break;
					}
					vector<LinearRing*> rings;
					FFBlobUtils::ReadBuf(&b, rings);
					for (long k = 0; k < rings.size(); ++k) {
						if (k == 0)
							pol->addBoundary(rings[0]);
						else
							pol->addHole(rings[k]);
					}
				}
			}
			pol->PutVal(value);
			geometries->push_back(pol);
			if ( i % 100 == 0) {
				trq.fUpdate(i, iNr); 
			}
		}
}

PolygonMapStoreForeign::~PolygonMapStoreForeign()
{
}

void PolygonMapStoreForeign::Store()
{
	if (ptr.fUseAs()) {
		ptr.WriteElement("PolygonMapStore","Format",ptr.getVersionBinary());
		ptr.WriteElement("PolygonMap", "Type", "PolygonMapStore");

		ptr.WriteElement("PolygonMapStore", "Polygons", iPol());

	} else
		PolygonMapStore::Store();
	ptr.WriteElement("PolygonMap", "Topology", "false");
	if ( ptr.fUseAs() )
	{
		ptr.WriteElement("TableStore", "Type", "TableForeign");
		ptr.WriteElement("top:TableStore", "Type", "TableForeign");
	}
}
