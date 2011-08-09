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
/*
// $Log: /ILWIS 3.0/PolygonMap/SEGMASK.cpp $
 * 
 * 6     8-02-01 17:13 Hendrikse
 * implem errormessage TR("Use of attribute maps is not possible: '%S'")
 * 
 * 5     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 4     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 3     9/08/99 12:02p Wind
 * comments
 * 
 * 2     9/08/99 10:26a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.3  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.2  1997/08/04 15:36:21  Wim
// Improved use of mask. Use now Segment::fInMask(mask)
//
/* PolygonMapSymetricDifference
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    5:19 pm
*/                                                                      

//#include <geos/algorithm/SymetricDifference.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Domain\DomainUniqueID.h"
#include "PolygonApplications\SymDifference.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\System\Engine.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createPolygonMapSymetricDifference(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapSymetricDifference::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapSymetricDifference(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapSymetricDifference::sSyntax() {
  return "PolygonMapSymetricDifference(inputmap, combined)";
}

PolygonMapSymetricDifference* PolygonMapSymetricDifference::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName1 = as[0];
	String sInputMapName2 = as[1];
	FileName fnInput1(sInputMapName1);
	FileName fnInput2(sInputMapName2);
	PolygonMap pm2(fnInput2);
	PolygonMap pm1(fnInput1);
	CoordBounds cb = pm1->cb();
	cb+= pm2->cb();
	Domain dm;
	dm = Domain(fn, 0, dmtUNIQUEID, "Pol");

  //BaseMap basemp(FileName(ssInputMapName));
  return new PolygonMapSymetricDifference(fn, p, cb, dm, pm1, pm2);
}

PolygonMapSymetricDifference::PolygonMapSymetricDifference(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1, fn2;
    ReadElement("PolygonMapSymetricDifference", "InputMap1", fn1);
    ReadElement("PolygonMapSymetricDifference", "InputMap2", fn2);
	ReadElement("PolygonMapSymetricDifference", "StartAttTable2", startSecondBatch);
	polmap1 = PolygonMap(fn1);
	polmap2 = PolygonMap(fn2);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(polmap1.ptr());
   objdep.Add(polmap2.ptr());
}

PolygonMapSymetricDifference::PolygonMapSymetricDifference(const FileName& fn, PolygonMapPtr& p, const CoordBounds& cb, const Domain& dm, const PolygonMap& pm1, const PolygonMap& pm2)
: PolygonMapVirtual(fn, p, pm1->cs(),cb,dm), 
	polmap1(pm1), 
	polmap2(pm2)
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


int PolygonMapSymetricDifference::AddAttColumns(Table& tblAtt, PolygonMap polmap,set<String>& names) {
  Table tbl = polmap->tblAtt();
  for(int i=0; i<tbl->iCols(); ++i) {
	  Column col= tbl->col(i);
	  String name;
	  if ( names.find(col->sNam) != names.end()) {
		  name = col->sNam + "_" + polmap->fnObj.sFile;
	  } else
		  name = col->sNam;
	  names.insert(name);
	  Column colNew = tblAtt->colNew(name, col->dvrs());
  }
  return tblAtt->iCols();
}

void PolygonMapSymetricDifference::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapSymetricDifference");
  WriteElement("PolygonMapSymetricDifference", "InputMap1", polmap1);
  WriteElement("PolygonMapSymetricDifference", "InputMap2", polmap2);
  WriteElement("PolygonMapSymetricDifference", "StartAttTable2", startSecondBatch);
}

PolygonMapSymetricDifference::~PolygonMapSymetricDifference()
{
}

String PolygonMapSymetricDifference::sExpression() const
{
	return String("PolygonMapSymetricDifference(%S,%S)", polmap1->fnObj.sRelativeQuoted(false), polmap2->fnObj.sRelativeQuoted(false));
}

bool PolygonMapSymetricDifference::fDomainChangeable() const
{
  return false;
}

void PolygonMapSymetricDifference::Init()
{
  //htpFreeze = htpPolygonMapSymetricDifferenceT;
  sFreezeTitle = "PolygonMapSymetricDifference";
}

bool PolygonMapSymetricDifference::fFreezing()
{
	trq.SetText("Calculating symetric differences" );
	bool sameCsy = polmap1->cs().fEqual(polmap2->cs());

	vector<Geometry *> v;
	for(long i=0; i < polmap1->iFeatures(); ++i) {
		ILWIS::Polygon *pol = CPOLYGON(polmap1->getFeature(i));
		if ( pol == NULL || pol->fValid() == false || pol->iValue() == iUNDEF)
			continue;
		v.push_back(pol);
	}

	vector<Geometry *> shouldBeDeleted;
	for(long i=0; i < polmap2->iFeatures(); ++i) {
		ILWIS::Polygon *pol = CPOLYGON(polmap2->getFeature(i));
		if ( pol == NULL || pol->fValid() == false || pol->iValue() == iUNDEF)
			continue;
		Geometry *g = sameCsy ? pol : polmap2->getTransformedFeature(i, polmap1->cs());
		if (!sameCsy)
			shouldBeDeleted.push_back(g);
		v.push_back(g);
	}
	Geometry *resultGeom = v.at(0);
	Geometry *prevGeom = NULL;
	for(int i = 1; i < v.size(); ++i) {
		try{
		if ( trq.fUpdate(i,v.size()))
			return false;
		prevGeom = resultGeom;
		resultGeom   = resultGeom->symDifference(v.at(i));
		if ( i > 2)
			delete prevGeom;
		} catch(geos::util::GEOSException ex) {
			getEngine()->getLogger()->LogLine(ex.what(),Logger::lmERROR);
		}

	}


	if ( dynamic_cast<geos::geom::Polygon *>(resultGeom) ) {
		geos::geom::Point *pnt = resultGeom->getInteriorPoint();
		vector<Geometry *> pols = polmap1->getFeatures(Coord(*(pnt->getCoordinate())));
		if ( pols.size() > 0)
			AddPolygon(resultGeom);	
	} else if ( dynamic_cast<geos::geom::MultiPolygon *>(resultGeom)) {
		geos::geom::MultiPolygon *multip =(geos::geom::MultiPolygon *)(resultGeom);
		for(int k=0; k < multip->getNumGeometries(); ++k) {
			const Geometry *geom = multip->getGeometryN(k);
			AddPolygon(geom);
		}
	}
	for(int i=0; i < shouldBeDeleted.size(); ++i)
		delete shouldBeDeleted.at(i);

	return true;
}

void  PolygonMapSymetricDifference::AddPolygon(const geos::geom::Geometry *g) {
	Table tbl = ptr.tblAtt();
	geos::geom::Point *pnt = g->getInteriorPoint();
	vector<Geometry *> pols1 = polmap1->getFeatures(Coord(*(pnt->getCoordinate())));
	vector<Geometry *> pols2 = polmap2->getFeatures(Coord(*(pnt->getCoordinate())));
	long record = ptr.dm()->pdUniqueID()->iAdd();
	ILWIS::Polygon *p = CPOLYGON(ptr.newFeature(const_cast<Geometry *>(g)));
	p->PutVal( record );
	if(pols1.size() > 0) {
		tbl->col(0)->PutRaw(record,CPOLYGON(pols1.at(0))->iValue());
		AddAttValues(polmap1,tbl,CPOLYGON(pols1.at(0)),2,record);
	}
	if (pols2.size() > 0) {
		tbl->col(1)->PutRaw(record,CPOLYGON(pols2.at(0))->iValue());
		AddAttValues(polmap1,tbl, CPOLYGON(pols2.at(0)), 2 + (polmap1->fTblAtt() ? polmap1->tblAtt()->iCols() : 0),record);
	}

	
}

void PolygonMapSymetricDifference::AddAttValues(PolygonMap polmap, Table& tbl,ILWIS::Polygon *pol, int start, long record) {
	Table tblAtt = polmap->tblAtt();
	if ( !tblAtt.fValid())
		return;
	for(int i = 0; i < tblAtt->iCols(); ++i) {
		Column col = tblAtt->col(i);
		Column colAtt = tbl->col( i + start);
		long iRaw = col->iRaw(pol->iValue());
		if ( colAtt.fValid()) {
			String s = colAtt->dm()->sValueByRaw(iRaw);
			colAtt->PutRaw(record,iRaw);
		}
	}
}







