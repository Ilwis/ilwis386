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
                                                           

//#include <geos/algorithm/Intersect.h>
#include "Engine\Map\Segment\seg.H"
#include "SegmentApplications\SegIntersect.H"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapIntersect(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapIntersect::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapIntersect(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapIntersect::sSyntax() {
  return "SegmentMapIntersect(inputmap1, inputmap2)";
}

SegmentMapIntersect* SegmentMapIntersect::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
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
	if ( pm1->dm() == pm2->dm())
		dm = pm1->dm();
	else {
		dm = Domain(fn, 0, dmtUNIQUEID, "Seg");
	}

  //BaseMap basemp(FileName(ssInputMapName));
  return new SegmentMapIntersect(fn, p, cb, dm, pm1, pm2);
}

SegmentMapIntersect::SegmentMapIntersect(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1, fn2;
    ReadElement("SegmentMapIntersect", "InputMap1", fn1);
    ReadElement("SegmentMapIntersect", "InputMap2", fn2);
	ReadElement("SegmentMapIntersect", "StartAttTable2", startSecondBatch);
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
 Table tbl;
  tbl = Table(FileName(ptr.fnObj,".tbt"), ptr.dm());
  String colName2 = bmap1->fnObj.sFile == bmap2->fnObj.sFile ? bmap1->fnObj.sFile + "_1" : bmap2->fnObj.sFile;
  Column col1(tbl,bmap1->fnObj.sFile,bmap1->dm());
  Column col2(tbl,colName2, Domain("String"));
  tbl->AddCol(col1);
  tbl->AddCol(col2);
  set<String> names;
  if ( bmap1->fTblAtt()) {
	  startSecondBatch = AddAttColumns(tbl, bmap1,names);
  }
  if ( bmap2->fTblAtt()) {
	  AddAttColumns(tbl, bmap2, names);
  }
  SetAttributeTable(tbl);
}


int SegmentMapIntersect::AddAttColumns(Table& tblAtt, BaseMap bmap,set<String>& names) {
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


SegmentMapIntersect::SegmentMapIntersect(const FileName& fn, SegmentMapPtr& p, const CoordBounds& cb, const Domain& dm, const BaseMap& pm1, const BaseMap& pm2)
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
}


void SegmentMapIntersect::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapIntersect");
  WriteElement("SegmentMapIntersect", "InputMap1", bmap1);
  WriteElement("SegmentMapIntersect", "InputMap2", bmap2);
  WriteElement("SegmentMapIntersect", "StartAttTable2", startSecondBatch);
}

SegmentMapIntersect::~SegmentMapIntersect()
{
}

String SegmentMapIntersect::sExpression() const
{
	return String("SegmentMapIntersect(%S,%S)", bmap1->fnObj.sRelativeQuoted(false), bmap2->fnObj.sRelativeQuoted(false));
}

bool SegmentMapIntersect::fDomainChangeable() const
{
  return false;
}

void SegmentMapIntersect::Init()
{
  //htpFreeze = htpSegmentMapIntersectT;
  sFreezeTitle = "SegmentMapIntersect";
}

bool SegmentMapIntersect::fFreezing()
{
	trq.SetText("Intersecting Lines" );
	Table tblAtt = ptr.tblAtt();
	bool sameCsy = bmap1->cs().fEqual(bmap2->cs());
	long total = bmap1->iFeatures() * bmap2->iFeatures(); 
	for(long i=0; i < bmap1->iFeatures(); ++i) {
		Geometry *resultGeom = NULL;
		Geometry *g1 = bmap1->getFeature(i);
		Feature *f = CFEATURE(g1);
		if ( g1 == NULL || g1->isEmpty() || f->fValid() == false)
			continue;
		if ( trq.fUpdate(i,bmap1->iFeatures()))
			return false;
		for(long j = 0; j < bmap2->iFeatures(); ++j) {
			Geometry *g2 = sameCsy ? bmap2->getFeature(j) : bmap2->getTransformedFeature(j, bmap1->cs());
			if ( g2 == NULL)
				continue;
			if(!f->EnvelopeIntersectsWith(g2))
				continue;
			Geometry *gTemp = g1->intersection(g2);
			if ( gTemp == NULL || gTemp->isEmpty())
				continue;
			bool isStartSegment = resultGeom == 0;
			Geometry *gTemp2 = resultGeom;
			resultGeom = isStartSegment ? gTemp : resultGeom->Union(gTemp);
			if ( !isStartSegment) // do not delete the start polygon; others will not be ILWIS polygons but geos Polygons
				delete gTemp2;
			if (!sameCsy)
				delete g2;
		}
		if ( dynamic_cast<geos::geom::LineString *>(resultGeom) ) {
			AddSegment(resultGeom, tblAtt, CFEATURE(g1));	
		} if ( dynamic_cast<geos::geom::MultiLineString *>(resultGeom)) {
			geos::geom::MultiLineString *multip = (MultiLineString *)(resultGeom);
			for(int k=0; k < multip->getNumGeometries(); ++k) {
				const Geometry *geom = multip->getGeometryN(k);
				AddSegment(geom, tblAtt, CFEATURE(g1));
			}
		} if ( dynamic_cast<geos::geom::MultiPolygon *>(resultGeom)) {
			geos::geom::MultiPolygon *mp = (geos::geom::MultiPolygon *)resultGeom;
			for(int n=0; n < mp->getNumGeometries(); ++n) {
				AddPolygonBoundaries( (geos::geom::Polygon *)mp->getGeometryN(n),tblAtt, CFEATURE(g1));
			}
		}

	}
	return true;
}

void SegmentMapIntersect::AddPolygonBoundaries(const geos::geom::Polygon *pol, Table& tbl, Feature * feature) {
	const LineString *s = pol->getExteriorRing();
	AddSegment(s, tbl, feature);
	for(int n=0; n < pol->getNumInteriorRing(); ++n) {
		s = pol->getInteriorRingN(n);
		AddSegment(s, tbl, feature);

	}
}

void  SegmentMapIntersect::AddSegment(const geos::geom::Geometry *g, Table& tbl, Feature * seg) {
	long record=ptr.dm()->pdUniqueID()->iAdd();
	ILWIS::Segment *s = CSEGMENT(ptr.newFeature(const_cast<Geometry *>(g)));
	s->PutVal( record );
	tbl->col(0)->PutVal(record, seg->sValue(bmap1->dvrs()));
	if ( bmap1->fTblAtt()) {
		AddAttValues(bmap1,tbl,seg,2, record);	
	}
	CoordinateSequence *seq = g->getCoordinates();
	Coord c;
	if ( seq->size() == 2) {
		c = Coord((seq->getAt(0).x + seq->getAt(1).x)/2,(seq->getAt(0).y + seq->getAt(1).y)/2 );
	} else if ( seq->size() > 2) {
		c = Coord(seq->getAt(1).x,seq->getAt(1).y);
	}   
	else
		return;
	delete seq;
	vector<Geometry *> segs = bmap2->getFeatures(c);
	String sVal;
	for(int i = 0; i < segs.size(); ++i) {
		Feature *s1 = CFEATURE(segs.at(i));
		if ( bmap2->fTblAtt()) {
				AddAttValues(bmap2,tbl,s1,startSecondBatch, record);	
		}
	    if ( sVal != "")
			sVal += ",";
		sVal += s1->sValue(bmap2->dvrs());
		tbl->col(1)->PutVal(record, sVal);
	}
}

void SegmentMapIntersect::AddAttValues(BaseMap bmap, Table& tbl,Feature *seg, int start, long record) {
	Table tblAtt = bmap->tblAtt();
	for(int i = 0; i < tblAtt->iCols(); ++i) {
		Column col = tblAtt->col(i);
		Column colAtt = tbl->col( i + start);
		long iRaw = col->iRaw(seg->iValue());
		String s = colAtt->dm()->sValueByRaw(iRaw);
		colAtt->PutRaw(record,iRaw);
	}
}







