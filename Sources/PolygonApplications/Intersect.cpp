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
 * implem errormessage SSEGErrNoAttColumnAllowed_S
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
/* PolygonMapIntersect
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    5:19 pm
*/                                                                      

//#include <geos/algorithm/Intersect.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "PolygonApplications\Intersect.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\System\Engine.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createPolygonMapIntersect(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapIntersect::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapIntersect(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapIntersect::sSyntax() {
  return "PolygonMapIntersect(inputmap, combined)";
}

PolygonMapIntersect* PolygonMapIntersect::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
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
  return new PolygonMapIntersect(fn, p, cb, dm, pm1, pm2);
}

PolygonMapIntersect::PolygonMapIntersect(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1, fn2;
    ReadElement("PolygonMapIntersect", "InputMap1", fn1);
    ReadElement("PolygonMapIntersect", "InputMap2", fn2);
    ReadElement("PolygonMapIntersect", "StartAttTable2", startSecondBatch);
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

PolygonMapIntersect::PolygonMapIntersect(const FileName& fn, PolygonMapPtr& p, const CoordBounds& cb, const Domain& dm, const PolygonMap& pm1, const PolygonMap& pm2)
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
  Column col2(tbl,colName2, Domain("String"));
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


int PolygonMapIntersect::AddAttColumns(Table& tblAtt, PolygonMap polmap,set<String>& names) {
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


void PolygonMapIntersect::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapIntersect");
  WriteElement("PolygonMapIntersect", "InputMap1", polmap1);
  WriteElement("PolygonMapIntersect", "InputMap2", polmap2);
  WriteElement("PolygonMapIntersect", "StartAttTable2", startSecondBatch);
}

PolygonMapIntersect::~PolygonMapIntersect()
{
}

String PolygonMapIntersect::sExpression() const
{
	return String("PolygonMapIntersect(%S,%S)", polmap1->fnObj.sRelativeQuoted(false), polmap2->fnObj.sRelativeQuoted(false));
}

bool PolygonMapIntersect::fDomainChangeable() const
{
  return false;
}

void PolygonMapIntersect::Init()
{
  //htpFreeze = htpPolygonMapIntersectT;
  sFreezeTitle = "PolygonMapIntersect";
}

bool PolygonMapIntersect::fFreezing()
{
	trq.SetText("Intersecting polygons" );
	Table tblAtt = ptr.tblAtt();
	bool sameCsy = polmap1->cs().fEqual(polmap2->cs());
	for(long i=0; i < polmap1->iFeatures(); ++i) {
		Geometry *resultGeom = NULL;
		Geometry *g1 = polmap1->getFeature(i);
		ILWIS::Polygon *pol1 = CPOLYGON(g1);
		if ( pol1 == NULL)
			continue;
		if ( trq.fUpdate(i,polmap1->iFeatures()))
			return false;
		for(long j = 0; j < polmap2->iFeatures(); ++j) {
			Geometry *g2 = sameCsy ? polmap2->getFeature(j) : polmap2->getTransformedFeature(j, polmap1->cs());
			if ( g2 == NULL)
				continue;
			if(!pol1->EnvelopeIntersectsWith(g2))
				continue;
			ILWIS::Polygon *pol2 = CPOLYGON(g2);
			if ( !pol2->fValid() || pol2->rValue() == rUNDEF )
				continue;
			try{
				Geometry *gTemp = pol1->intersection(pol2);
				if ( gTemp == NULL || gTemp->isEmpty())
					continue;
				bool isStartPolygon = resultGeom == 0;
				Geometry *gTemp2 = resultGeom;
				resultGeom = isStartPolygon ? gTemp : resultGeom->Union(gTemp);
				if ( !isStartPolygon) // do not delete the start polygon; others will not be ILWIS polygons but geos Polygons
					delete gTemp2;
			} catch (geos::util::GEOSException ex ) {
				getEngine()->getLogger()->LogLine(ex.what(),Logger::lmERROR);
			}
			if (!sameCsy)
				delete g2;
		}
		if ( dynamic_cast<geos::geom::Polygon *>(resultGeom) ) {
			AddPolygon(resultGeom, tblAtt, pol1);	
		} if ( dynamic_cast<geos::geom::MultiPolygon *>(resultGeom)) {
			geos::geom::MultiPolygon *multip = (geos::geom::MultiPolygon *)(resultGeom);
			for(int k=0; k < multip->getNumGeometries(); ++k) {
				const Geometry *geom = multip->getGeometryN(k);
				AddPolygon(geom,tblAtt,pol1);
			}
		}
	}
	trq.fUpdate(polmap1->iFeatures(),polmap1->iFeatures());
	return true;
}

void  PolygonMapIntersect::AddPolygon(const geos::geom::Geometry *g, Table& tbl, ILWIS::Polygon * pol) {
	long record=ptr.dm()->pdUniqueID()->iAdd();
	ILWIS::Polygon *p = CPOLYGON(ptr.newFeature(const_cast<Geometry *>(g)));
	p->PutVal( record );
	tbl->col(0)->PutVal(record, pol->sValue(polmap1->dvrs()));
	if ( polmap1->fTblAtt()) {
		AddAttValues(polmap1,tbl,pol,2, record);	
	}
	geos::geom::Point *point = g->getInteriorPoint();
	Coord c(*(point->getCoordinate()));
	vector<Geometry *> pols = polmap2->getFeatures(c);
	String sVal;
	for(int i = 0; i < pols.size(); ++i) {
		ILWIS::Polygon *polygon = (ILWIS::Polygon *)pols.at(i);
		if ( polmap2->fTblAtt()) {
				AddAttValues(polmap2,tbl,polygon,startSecondBatch, record);	
		}
	    if ( sVal != "")
			sVal += ",";
		sVal += polygon->sValue(polmap2->dvrs());
		tbl->col(1)->PutVal(record, sVal);
	}
}

void PolygonMapIntersect::AddAttValues(PolygonMap polmap, Table& tbl,ILWIS::Polygon *pol, int start, long record) {
	Table tblAtt = polmap->tblAtt();
	for(int i = 0; i < tblAtt->iCols(); ++i) {
		Column col = tblAtt->col(i);
		Column colAtt = tbl->col( i + start);
		long iRaw = col->iRaw(pol->iValue());
		String s = colAtt->dm()->sValueByRaw(iRaw);
		colAtt->PutRaw(record,iRaw);
	}
}







