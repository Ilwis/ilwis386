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
                                                  

//#include <geos/algorithm/SymetricDifference.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Domain\DomainUniqueID.h"
#include "SegmentApplications\segSymDifference.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapSymetricDifference(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapSymetricDifference::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapSymetricDifference(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapSymetricDifference::sSyntax() {
  return "SegmentMapSymetricDifference(inputmap, combined)";
}

SegmentMapSymetricDifference* SegmentMapSymetricDifference::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
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
	dm = Domain(fn, 0, dmtUNIQUEID, "Pol");

  //BaseMap basemp(FileName(ssInputMapName));
  return new SegmentMapSymetricDifference(fn, p, cb, dm, pm1, pm2);
}

SegmentMapSymetricDifference::SegmentMapSymetricDifference(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1, fn2;
    ReadElement("SegmentMapSymetricDifference", "InputMap1", fn1);
    ReadElement("SegmentMapSymetricDifference", "InputMap2", fn2);
    ReadElement("SegmentMapSymetricDifference", "StartAttTable2", startSecondBatch);
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

SegmentMapSymetricDifference::SegmentMapSymetricDifference(const FileName& fn, SegmentMapPtr& p, const CoordBounds& cb, const Domain& dm, const BaseMap& pm1, const BaseMap& pm2)
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
  Table tbl;
  tbl = Table(FileName(ptr.fnObj,".tbt"), ptr.dm());
  String colName2 = pm1->fnObj.sFile == pm2->fnObj.sFile ? pm1->fnObj.sFile + "_1" : pm2->fnObj.sFile;
  Column col1(tbl,pm1->fnObj.sFile,pm1->dm());
  Column col2(tbl,colName2, pm2->dm());
  tbl->AddCol(col1);
  tbl->AddCol(col2);
  set<String> names;
  if ( pm1->fTblAtt()) {
	  startSecondBatch = AddAttColumns(tbl, pm1,names);
  }
  if ( pm2->fTblAtt()) {
	  AddAttColumns(tbl, pm2, names);
  }
  SetAttributeTable(tbl);
 
}

int SegmentMapSymetricDifference::AddAttColumns(Table& tblAtt, BaseMap bmap,set<String>& names) {
  Table tbl = bmap->tblAtt();
  for(int i=0; i<tbl->iCols(); ++i) {
	  Column col= tbl->col(i);
	  String name;
	  if ( names.find(col->sNam) != names.end()) {
		  name = col->sNam + "_" + bmap->fnObj.sFile;
	  } else
		  name = col->sNam;
	  names.insert(name);
	  Column colNew = tblAtt->colNew(name, col->dvrs());
  }
  return tblAtt->iCols();
}

void SegmentMapSymetricDifference::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapSymetricDifference");
  WriteElement("SegmentMapSymetricDifference", "InputMap1", bmap1);
  WriteElement("SegmentMapSymetricDifference", "InputMap2", bmap2);
    WriteElement("SegmentMapSymetricDifference", "StartAttTable2", startSecondBatch);
}

SegmentMapSymetricDifference::~SegmentMapSymetricDifference()
{
}

String SegmentMapSymetricDifference::sExpression() const
{
	return String("SegmentMapSymetricDifference(%S,%S)", bmap1->fnObj.sRelativeQuoted(false), bmap2->fnObj.sRelativeQuoted(false));
}

bool SegmentMapSymetricDifference::fDomainChangeable() const
{
  return false;
}

void SegmentMapSymetricDifference::Init()
{
  //htpFreeze = htpSegmentMapSymetricDifferenceT;
  sFreezeTitle = "SegmentMapSymetricDifference";
}

bool SegmentMapSymetricDifference::fFreezing()
{
	trq.SetText("Calculating symetric differences" );
	bool sameCsy = bmap1->cs().fEqual(bmap2->cs());

	vector<Geometry *> v;
	for(long i=0; i < bmap1->iFeatures(); ++i) {
		Geometry * g = bmap1->getFeature(i);
		Feature *f = CFEATURE(g);
		ILWIS::Polygon *pol = CPOLYGON(g);
		if ( pol)
			g = pol->getBoundary();
		if ( f == NULL || f->fValid() == false || f->iValue() == iUNDEF)
			continue;
		v.push_back(g);
	}
	vector<Geometry *> shouldBeDeleted;
	for(long i=0; i < bmap2->iFeatures(); ++i) {
		Geometry * g = bmap2->getFeature(i);
		Feature *f = CFEATURE(g);
		ILWIS::Polygon *pol = CPOLYGON(g);
		if ( pol)
			g = pol->getBoundary();
		if ( f == NULL || f->fValid() == false || f->iValue() == iUNDEF)
			continue;
		Geometry *g1 = sameCsy ? g : bmap2->getTransformedFeature(i, bmap1->cs());
		if (!sameCsy)
			shouldBeDeleted.push_back(g1);
		v.push_back(g1);
	}
	Geometry *resultGeom = v.at(0);
	Geometry *prevGeom = NULL;
	for(int i = 1; i < v.size(); ++i) {
		if ( trq.fUpdate(i,v.size()))
			return false;
		prevGeom = resultGeom;
		resultGeom   = resultGeom->symDifference(v.at(i));
		if ( i > 2)
			delete prevGeom;

	}

	if ( dynamic_cast<geos::geom::LineString *>(resultGeom) ) {
		geos::geom::Point *pnt = resultGeom->getInteriorPoint();
		vector<Geometry *> lines = bmap1->getFeatures(Coord(*(pnt->getCoordinate())));
		if ( lines.size() > 0)
			AddLine(resultGeom);	
	} else if ( dynamic_cast<geos::geom::MultiLineString *>(resultGeom)) {
		geos::geom::MultiLineString *multil =(geos::geom::MultiLineString *)(resultGeom);
		for(int k=0; k < multil->getNumGeometries(); ++k) {
			const Geometry *geom = multil->getGeometryN(k);
			AddLine(geom);
		}
	}

	for(int i=0; i < shouldBeDeleted.size(); ++i)
		delete shouldBeDeleted.at(i);

	return true;
}

void  SegmentMapSymetricDifference::AddLine(const geos::geom::Geometry *g) {
	Table tbl = ptr.tblAtt();
	geos::geom::Point *pnt = g->getInteriorPoint();
	vector<Geometry *> features1 = bmap1->getFeatures(Coord(*(pnt->getCoordinate())));
	vector<Geometry *> features2 = bmap2->getFeatures(Coord(*(pnt->getCoordinate())));
	long record = ptr.dm()->pdUniqueID()->iAdd();
	ILWIS::Segment *p = CSEGMENT(ptr.newFeature(const_cast<Geometry *>(g)));
	p->PutVal( record );
	if(features1.size() > 0) {
		tbl->col(0)->PutRaw(record,CFEATURE(features1.at(0))->iValue());
		AddAttValues(bmap1,tbl,CFEATURE(features1.at(0)),2,record);
	}
	if (features2.size() > 0) {
		tbl->col(1)->PutRaw(record,CFEATURE(features2.at(0))->iValue());
		AddAttValues(bmap1,tbl, CFEATURE(features2.at(0)), 2 + (bmap1->fTblAtt() ? bmap1->tblAtt()->iCols() : 0),record);
	}

	
}

void SegmentMapSymetricDifference::AddAttValues(BaseMap bmap, Table& tbl,Feature *feature, int start, long record) {
	Table tblAtt = bmap->tblAtt();
	if ( !tblAtt.fValid())
		return;
	for(int i = 0; i < tblAtt->iCols(); ++i) {
		Column col = tblAtt->col(i);
		Column colAtt = tbl->col( i + start);
		long iRaw = col->iRaw(feature->iValue());
		if ( colAtt.fValid()) {
			String s = colAtt->dm()->sValueByRaw(iRaw);
			colAtt->PutRaw(record,iRaw);
		}
	}
}







