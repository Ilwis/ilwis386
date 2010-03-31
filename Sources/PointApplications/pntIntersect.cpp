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
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "PointApplications\PntIntersect.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\Point.hs"

IlwisObjectPtr * createPointMapIntersect(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapIntersect::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapIntersect(fn, (PointMapPtr &)ptr);
}

const char* PointMapIntersect::sSyntax() {
  return "PointMapIntersect(inputmap1, inputmap2)";
}

PointMapIntersect* PointMapIntersect::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
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
		dm = Domain(fn, 0, dmtUNIQUEID, "Pnt");
	}

  //BaseMap basemp(FileName(ssInputMapName));
  return new PointMapIntersect(fn, p, cb, dm, pm1, pm2);
}

PointMapIntersect::PointMapIntersect(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1, fn2;
    ReadElement("PointMapIntersect", "InputMap1", fn1);
    ReadElement("PointMapIntersect", "InputMap2", fn2);
	ReadElement("PointMapIntersect", "StartAttTable2", startSecondBatch);
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

PointMapIntersect::PointMapIntersect(const FileName& fn, PointMapPtr& p, const CoordBounds& cb, const Domain& dm, const BaseMap& pm1, const BaseMap& pm2)
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

int PointMapIntersect::AddAttColumns(Table& tblAtt, BaseMap bmap,set<String>& names) {
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


void PointMapIntersect::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapIntersect");
  WriteElement("PointMapIntersect", "InputMap1", bmap1);
  WriteElement("PointMapIntersect", "InputMap2", bmap2);
  WriteElement("PointMapIntersect", "StartAttTable2", startSecondBatch);
}

PointMapIntersect::~PointMapIntersect()
{
}

String PointMapIntersect::sExpression() const
{
	return String("PointMapIntersect(%S,%S)", bmap1->fnObj.sRelativeQuoted(false), bmap2->fnObj.sRelativeQuoted(false));
}

bool PointMapIntersect::fDomainChangeable() const
{
  return false;
}

void PointMapIntersect::Init()
{
  //htpFreeze = htpPointMapIntersectT;
  sFreezeTitle = "PointMapIntersect";
}

bool PointMapIntersect::fFreezing()
{
	trq.SetText("Intersecting points" );
	Table tblAtt = ptr.tblAtt();
	bool sameCsy = bmap1->cs().fEqual(bmap2->cs());
	long total = bmap1->iFeatures() * bmap2->iFeatures(); 
	for(long i=0; i < bmap1->iFeatures(); ++i) {
		Geometry *resultGeom = NULL;
		Geometry *g1 = bmap1->getFeature(i);
		Feature *f = CFEATURE(g1);
		if ( dynamic_cast<geos::geom::Polygon *>(g1))
			g1 = ((geos::geom::Polygon *)g1)->getBoundary();
		if ( g1->isEmpty() || f->fValid() == false)
			continue;
		if ( trq.fUpdate(i,bmap1->iFeatures()))
			return false;
		for(long j = 0; j < bmap2->iFeatures(); ++j) {
			Geometry *g2 = sameCsy ? bmap2->getFeature(j) : bmap2->getTransformedFeature(j, bmap1->cs());
			if ( dynamic_cast<geos::geom::Polygon *>(g2))
				g2 = ((geos::geom::Polygon *)g2)->getBoundary();	 
			if ( g2 == NULL)
				continue;
			Geometry *gTemp = g1->intersection(g2);
			if ( gTemp == NULL || gTemp->isEmpty())
				continue;
			bool isStartPoint = resultGeom == 0;
			Geometry *gTemp2 = resultGeom;
			resultGeom = isStartPoint ? gTemp : resultGeom->Union(gTemp);
			if ( !isStartPoint) // do not delete the start polygon; others will not be ILWIS polygons but geos Polygons
				delete gTemp2;
			if (!sameCsy)
				delete g2;
		}
		if ( dynamic_cast<geos::geom::Point *>(resultGeom) ) {
			AddFeature(resultGeom,tblAtt,f);	
		} if ( dynamic_cast<geos::geom::GeometryCollection *>(resultGeom)) {
			geos::geom::GeometryCollection *multip = (GeometryCollection *)(resultGeom);
			for(int k=0; k < multip->getNumGeometries(); ++k) {
				if ( dynamic_cast<const geos::geom::Point *>( multip->getGeometryN(k))) {
					const Geometry *geom = multip->getGeometryN(k);
					AddFeature(geom,tblAtt,f);
				}
			}
		}
	}
	return true;
}

void  PointMapIntersect::AddFeature(const geos::geom::Geometry *g, Table& tbl, Feature *f) {
	ILWIS::Point *p = CPOINT(ptr.newFeature(const_cast<Geometry *>(g)));
	long record = ptr.dm()->pdUniqueID()->iAdd();
	p->PutVal( record );
	tbl->col(0)->PutVal(record, f->sValue(bmap1->dvrs()));
	if ( bmap1->fTblAtt()) {
		AddAttValues(bmap1,tbl,f,2, record);	
	}
	Coord crd(*(g->getCoordinate()));
	vector<Geometry *> pnts = bmap2->getFeatures(crd);
	String sVal;
	for(int i = 0; i < pnts.size(); ++i) {
		Feature *s1 = CFEATURE(pnts.at(i));
		if ( bmap2->fTblAtt()) {
				AddAttValues(bmap2,tbl,s1,startSecondBatch, record);	
		}
	    if ( sVal != "")
			sVal += ",";
		sVal += s1->sValue(bmap2->dvrs());
		tbl->col(1)->PutVal(record, sVal);
	}
}

void PointMapIntersect::AddAttValues(BaseMap bmap, Table& tbl,Feature *pnt, int start, long record) {
	Table tblAtt = bmap->tblAtt();
	for(int i = 0; i < tblAtt->iCols(); ++i) {
		Column col = tblAtt->col(i);
		Column colAtt = tbl->col( i + start);
		long iRaw = col->iRaw(pnt->iValue());
		String s = colAtt->dm()->sValueByRaw(iRaw);
		colAtt->PutRaw(record,iRaw);
	}
}







