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
/* PolygonMapUnion
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    5:19 pm
*/                                                                      

//#include <geos/algorithm/Union.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "PolygonApplications\Union.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createPolygonMapUnion(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapUnion::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapUnion(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapUnion::sSyntax() {
  return "PolygonMapUnion(inputmap, combined)";
}

PolygonMapUnion* PolygonMapUnion::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
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
	if ( pm1->dm() == pm2->dm())
		dm = pm1->dm();
	else {
		dm = Domain(fn, pm1->iFeatures() + pm2->iFeatures(), dmtUNIQUEID, "Pol");
	}

  //BaseMap basemp(FileName(ssInputMapName));
  return new PolygonMapUnion(fn, p, cb, dm, pm1, pm2);
}

PolygonMapUnion::PolygonMapUnion(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1, fn2;
    ReadElement("PolygonMapUnion", "InputMap1", fn1);
    ReadElement("PolygonMapUnion", "InputMap2", fn2);
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

PolygonMapUnion::PolygonMapUnion(const FileName& fn, PolygonMapPtr& p, const CoordBounds& cb, const Domain& dm, const PolygonMap& pm1, const PolygonMap& pm2)
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
 
}

void PolygonMapUnion::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapUnion");
  WriteElement("PolygonMapUnion", "InputMap1", polmap1);
  WriteElement("PolygonMapUnion", "InputMap2", polmap2);
}

PolygonMapUnion::~PolygonMapUnion()
{
}

String PolygonMapUnion::sExpression() const
{
	return String("PolygonMapUnion(%S,%S)", polmap1->fnObj.sRelativeQuoted(false), polmap2->fnObj.sRelativeQuoted(false));
}

bool PolygonMapUnion::fDomainChangeable() const
{
  return false;
}

void PolygonMapUnion::Init()
{
  //htpFreeze = htpPolygonMapUnionT;
  sFreezeTitle = "PolygonMapUnion";
}

bool PolygonMapUnion::fFreezing()
{
	trq.SetText("Union of polygons" );
	long record = 0;
	bool sameCsy = polmap1->cs().fEqual(polmap2->cs());
	vector<Geometry *> mergedPolygons;
	vector<ILWIS::Polygon *> inputPolygons;
	for(long i=0; i < polmap1->iFeatures(); ++i) {
		ILWIS::Polygon *pol1 = CPOLYGON(polmap1->getFeature(i));
		if (!pol1 || !pol1->fValid()) continue;
		inputPolygons.push_back(CPOLYGON(pol1->clone()));
	}

	for(long i=0; i < polmap2->iFeatures(); ++i) {
		ILWIS::Polygon *pol2 = CPOLYGON(polmap2->getFeature(i));
		if (!pol2 || !pol2->fValid()) continue;
		if ( sameCsy)
			inputPolygons.push_back( CPOLYGON(pol2->clone()));
		else
			inputPolygons.push_back( CPOLYGON(polmap2->getTransformedFeature(i, polmap1->cs())));
	}

	trq.SetText("Merging polygons");
	if(!MergePolygons(inputPolygons, mergedPolygons))
		return false;

	for(int i =0; i < inputPolygons.size(); ++i)
		delete inputPolygons[i];


	trq.fUpdate(inputPolygons.size(), inputPolygons.size());
	trq.SetText("Creating new map");
	for(long i=0; i < mergedPolygons.size(); ++i) {
		Geometry *g = mergedPolygons[i];
		if ( g->isEmpty())
			continue;
		if ( trq.fUpdate(i,mergedPolygons.size()))
			return false;
		
		if ( dynamic_cast<geos::geom::Polygon *>(g) ) {
			AddPolygon(g,record);	
		} else if ( dynamic_cast<geos::geom::GeometryCollection *>(g)) {
			geos::geom::GeometryCollection *gc = (geos::geom::GeometryCollection *)g;
			for(int k = 0; k < gc->getNumGeometries(); ++k) {
				if ( dynamic_cast<const geos::geom::Polygon *>(gc->getGeometryN(k)) ) {
					const geos::geom::Polygon *g3 = (geos::geom::Polygon *)gc->getGeometryN(k);
					AddPolygon(g3,record);	
				}
			}
		}
	}

	for(int i =0; i < mergedPolygons.size(); ++i)
		delete mergedPolygons[i];
	return true;
}

bool PolygonMapUnion::MergePolygons(vector<ILWIS::Polygon *> inputPolygons, vector<Geometry *>& mergedPolygons) {
	vector<bool> handled(inputPolygons.size(),false);
	for(long i=0; i < inputPolygons.size(); ++i) {
		if(trq.fUpdate(i,inputPolygons.size()))
			return false;
		ILWIS::Polygon *gInput1 = inputPolygons[i];
		if ( gInput1->isEmpty()) continue;
		vector<Geometry *> intersects;
		for(long j=0; j < inputPolygons.size(); ++j) {
			if ( i == j ) continue;
			if ( handled[j]) continue;
			ILWIS::Polygon *gInput2 = inputPolygons[j];
			if ( gInput1->EnvelopeIntersectsWith(gInput2)) {
				intersects.push_back(gInput2->clone());
				handled[j] = handled[i] = true;
			}
		}
		Geometry *gTotal = NULL;
		for( int k = 0; k < intersects.size(); ++k) {
			if ( gTotal == NULL)
				gTotal = gInput1->Union(intersects[k]);
			else {
				try {
					Geometry *gNew = gTotal->Union(intersects[k]);
					delete gTotal;
					gTotal = gNew;
				} catch (geos::util::GEOSException ) {
				}
			}
		}
		if ( gTotal && !gTotal->isEmpty())
			mergedPolygons.push_back(gTotal);
		else if ( handled[i] == false)
			mergedPolygons.push_back(gInput1->clone());
		handled[i] = true;
		for(int n=0; n < intersects.size(); ++n)
			delete intersects[n];
	}
	return true;
}

void  PolygonMapUnion::AddPolygon(const geos::geom::Geometry *g, long& record) {
	ILWIS::Polygon *p = CPOLYGON(ptr.newFeature(const_cast<Geometry *>(g)));
	p->PutVal( record );
	++record;
}







