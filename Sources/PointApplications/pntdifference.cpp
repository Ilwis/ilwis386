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
#include "PointApplications\pntDifference.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\Point.hs"

IlwisObjectPtr * createPointMapDifference(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapDifference::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapDifference(fn, (PointMapPtr &)ptr);
}

const char* PointMapDifference::sSyntax() {
  return "PointMapDifference(inputmap1, inputmap2)";
}

PointMapDifference* PointMapDifference::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName1 = as[0];
	String sInputMapName2 = as[1];
	FileName fnInput1(sInputMapName1);
	FileName fnInput2(sInputMapName2);
	BaseMap pm2(fnInput2);
	BaseMap pm1(fnInput1);
	CoordBounds cb = pm1->cb();
	cb+= pm2->cb();
	Domain dm;
	dm = pm1->dm();

  //BaseMap basemp(FileName(ssInputMapName));
  return new PointMapDifference(fn, p, cb, dm, pm1, pm2);
}

PointMapDifference::PointMapDifference(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1, fn2;
    ReadElement("PointMapDifference", "InputMap1", fn1);
    ReadElement("PointMapDifference", "InputMap2", fn2);
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

PointMapDifference::PointMapDifference(const FileName& fn, PointMapPtr& p, const CoordBounds& cb, const Domain& dm, const BaseMap& pm1, const BaseMap& pm2)
: PointMapVirtual(fn, p, pm1->cs(),cb,dm), 
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


void PointMapDifference::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapDifference");
  WriteElement("PointMapDifference", "InputMap1", bmap1);
  WriteElement("PointMapDifference", "InputMap2", bmap2);
}

PointMapDifference::~PointMapDifference()
{
}

String PointMapDifference::sExpression() const
{
	return String("PointMapDifference(%S,%S)", bmap1->fnObj.sRelativeQuoted(false), bmap2->fnObj.sRelativeQuoted(false));
}

bool PointMapDifference::fDomainChangeable() const
{
  return false;
}

void PointMapDifference::Init()
{
  //htpFreeze = htpPointMapDifferenceT;
  sFreezeTitle = "PointMapDifference";
}

bool PointMapDifference::fFreezing()						 
{
	trq.SetText("Difference Points" );
	bool sameCsy = bmap1->cs().fEqual(bmap2->cs());

	vector<Geometry *> *points1 = new vector<Geometry *>();
	vector<Geometry *> *points2 = new vector<Geometry *>();
	for(long i=0; i < bmap1->iFeatures(); ++i) {
		Geometry *g = bmap1->getFeature(i);
		if (g)
			points1->push_back(g->clone());
	}

	for(long i=0; i < bmap2->iFeatures(); ++i) {
		Geometry *g = bmap2->getFeature(i);
		if (!g)
			continue;
		Geometry *g2 = sameCsy ? g->clone() : bmap2->getTransformedFeature(i, bmap1->cs());
		points2->push_back(g2);
	}
	GeometryFactory *fact = new GeometryFactory();
	MultiPoint *mp1 =  fact->createMultiPoint(points1);//new MultiPoint(points1, new GeometryFactory());
	MultiPoint *mp2 = fact->createMultiPoint(points2);
	//delete fact;

	Geometry *resultGeom = mp1->difference(mp2);
	if ( dynamic_cast<geos::geom::Point *>(resultGeom) ) {
		AddFeature(resultGeom);
	} if ( dynamic_cast<geos::geom::GeometryCollection *>(resultGeom)) {
		geos::geom::GeometryCollection *multip = dynamic_cast<geos::geom::GeometryCollection *>(resultGeom);
		for(int k=0; k < multip->getNumGeometries(); ++k) {
			if ( dynamic_cast<const geos::geom::Point *>(multip->getGeometryN(k))){
				const Geometry *geom = multip->getGeometryN(k);
				AddFeature(geom);
			}
		}
	}

	return true;
}

void  PointMapDifference::AddFeature(const geos::geom::Geometry *g) {
	ILWIS::Point *s = CPOINT(ptr.newFeature((const_cast<Geometry *>(g))));
	if (s) {
		vector<Geometry *> v = bmap1->getFeatures(Coord(*(s->getCoordinate())));
		if ( v.size() > 0){
			ILWIS::Point *p = CPOINT(v.at(0));
			s->PutVal(p->rValue());
		}
	}
}







