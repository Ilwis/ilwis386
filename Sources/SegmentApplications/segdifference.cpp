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

//#include <geos/algorithm/Difference.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Segment\seg.H"
#include "SegmentApplications\segDifference.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapDifference(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapDifference::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapDifference(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapDifference::sSyntax() {
  return "SegmentMapDifference(inputmap1, inputmap2)";
}

SegmentMapDifference* SegmentMapDifference::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName1 = as[0];
	String sInputMapName2 = as[1];
	FileName fnInput1(sInputMapName1);
	FileName fnInput2(sInputMapName2);
	SegmentMap pm2(fnInput2);
	SegmentMap pm1(fnInput1);
	CoordBounds cb = pm1->cb();
	cb+= pm2->cb();
	Domain dm;
	dm = pm1->dm();

  //BaseMap basemp(FileName(ssInputMapName));
  return new SegmentMapDifference(fn, p, cb, dm, pm1, pm2);
}

SegmentMapDifference::SegmentMapDifference(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1, fn2;
    ReadElement("SegmentMapDifference", "InputMap1", fn1);
    ReadElement("SegmentMapDifference", "InputMap2", fn2);
	bmap1 = BaseMap(fn1);
	bmap2 = BaseMap(fn2);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(bmap1.ptr());
   objdep.Add(bmap2.ptr());
}

SegmentMapDifference::SegmentMapDifference(const FileName& fn, SegmentMapPtr& p, const CoordBounds& cb, const Domain& dm, const BaseMap& pm1, const BaseMap& pm2)
: SegmentMapVirtual(fn, p, pm1->cs(),cb,dm), 
	bmap1(pm1), 
	bmap2(pm2)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(pm1.ptr());
  objdep.Add(pm2.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if ( pm1->fTblAtt())
	SetAttributeTable(pm1->tblAtt());
 
}


void SegmentMapDifference::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapDifference");
  WriteElement("SegmentMapDifference", "InputMap1", bmap1);
  WriteElement("SegmentMapDifference", "InputMap2", bmap2);
}

SegmentMapDifference::~SegmentMapDifference()
{
}

String SegmentMapDifference::sExpression() const
{
	return String("SegmentMapDifference(%S,%S)", bmap1->fnObj.sRelativeQuoted(false), bmap2->fnObj.sRelativeQuoted(false));
}

bool SegmentMapDifference::fDomainChangeable() const
{
  return false;
}

void SegmentMapDifference::Init()
{
  //htpFreeze = htpSegmentMapDifferenceT;
  sFreezeTitle = "SegmentMapDifference";
}

bool SegmentMapDifference::fFreezing()						 
{
	trq.SetText("Difference segments" );
	bool sameCsy = bmap1->cs().fEqual(bmap2->cs());
	for(long i=0; i < bmap1->iFeatures(); ++i) {
		Geometry *resultGeom = bmap1->getFeature(i);
		Geometry *f1 = resultGeom;
		if ( f1 == NULL)
			continue;
		if ( trq.fUpdate(i,bmap1->iFeatures()))
			return false;
		for(long j = 0; j < bmap2->iFeatures(); ++j) {
			Geometry *f2 = sameCsy ? bmap2->getFeature(j) : bmap2->getTransformedFeature(j, bmap1->cs());
			if ( f2 == NULL)
				continue; 
			Geometry *gTemp = f1->difference(f2);
			if ( !gTemp || gTemp->isEmpty())
				continue;
			if ( dynamic_cast<ILWIS::Segment *>(resultGeom) == 0) // do not delete the start segments; others will not be ILWIS polygons but geos linestrings
				delete resultGeom;
			resultGeom = gTemp;
			if(!sameCsy)
				delete f2;
		}
		if ( dynamic_cast<geos::geom::LineString *>(resultGeom) ) {
				AddFeature(resultGeom, CFEATURE(f1));
			} if ( dynamic_cast<geos::geom::GeometryCollection *>(resultGeom)) {
				geos::geom::GeometryCollection *multip = (geos::geom::GeometryCollection *)(resultGeom);
				for(int k=0; k < multip->getNumGeometries(); ++k) {
					if ( dynamic_cast<const geos::geom::LineString *>(multip->getGeometryN(k))){
						const Geometry *geom = multip->getGeometryN(k);
						AddFeature(geom,CFEATURE(f1));
					}
				}
			}

	}
	return true;
}

void  SegmentMapDifference::AddFeature(const geos::geom::Geometry *g, Feature *f1) {
	ILWIS::Segment *s = CSEGMENT(ptr.newFeature(const_cast<Geometry *>(g)));
	s->PutVal( f1->rValue() );
}







