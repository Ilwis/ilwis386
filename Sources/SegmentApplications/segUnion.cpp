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
// $Log: /ILWIS 3.0/SegmentMap/SEGMASK.cpp $
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
/* SegmentMapUnion
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    5:19 pm
*/                                                                      

//#include <geos/algorithm/Union.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "SegmentApplications\segUnion.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapUnion(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapUnion::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapUnion(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapUnion::sSyntax() {
  return "SegmentMapUnion(inputmap, combined)";
}

SegmentMapUnion* SegmentMapUnion::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
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
		dm = Domain(fn,0, dmtUNIQUEID, "Seg");
	}

  //BaseMap basemp(FileName(ssInputMapName));
  return new SegmentMapUnion(fn, p, cb, dm, pm1, pm2);
}

SegmentMapUnion::SegmentMapUnion(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1, fn2;
    ReadElement("SegmentMapUnion", "InputMap1", fn1);
    ReadElement("SegmentMapUnion", "InputMap2", fn2);
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

SegmentMapUnion::SegmentMapUnion(const FileName& fn, SegmentMapPtr& p, const CoordBounds& cb, const Domain& dm, const BaseMap& pm1, const BaseMap& pm2)
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


void SegmentMapUnion::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapUnion");
  WriteElement("SegmentMapUnion", "InputMap1", bmap1);
  WriteElement("SegmentMapUnion", "InputMap2", bmap2);
}

SegmentMapUnion::~SegmentMapUnion()
{
}

String SegmentMapUnion::sExpression() const
{
	return String("SegmentMapUnion(%S,%S)", bmap1->fnObj.sRelativeQuoted(false), bmap2->fnObj.sRelativeQuoted(false));
}

bool SegmentMapUnion::fDomainChangeable() const
{
  return false;
}

void SegmentMapUnion::Init()
{
  //htpFreeze = htpSegmentMapUnionT;
  sFreezeTitle = "SegmentMapUnion";
}

bool SegmentMapUnion::fFreezing()
{
	trq.SetText("Union of Segments" );
	long record = 0;
	bool sameCsy = bmap1->cs().fEqual(bmap2->cs());
	vector<Geometry *> mergedGeometries;
	vector<Geometry *> inputGeometries;
	for(long i=0; i < bmap1->iFeatures(); ++i) {
		Geometry *g1 = bmap1->getFeature(i);
		std::string sss = g1->getGeometryType();
		if (!g1 || g1->isEmpty()) continue;
		inputGeometries.push_back(g1->clone());
	}

	for(long i=0; i < bmap2->iFeatures(); ++i) {
		Geometry *g2 = bmap2->getFeature(i);
		if (!g2 || g2->isEmpty()) continue;
		if ( sameCsy)
			inputGeometries.push_back( g2->clone());
		else
			inputGeometries.push_back( bmap2->getTransformedFeature(i, bmap1->cs()));
	}

	trq.SetText("Merging segments");
	if(!MergeSegments(inputGeometries, mergedGeometries))
		return false;

	for(int i =0; i < inputGeometries.size(); ++i)
		delete inputGeometries[i];


	trq.fUpdate(inputGeometries.size(), inputGeometries.size());
	trq.SetText("Creating new map");
	for(long i=0; i < mergedGeometries.size(); ++i) {
		Geometry *g = mergedGeometries[i];
		if ( g->isEmpty())
			continue;
		if ( trq.fUpdate(i,mergedGeometries.size()))
			return false;

		if ( dynamic_cast<geos::geom::LineString *>(g) ) {
			AddFeature(g, record);	
		} else if ( dynamic_cast<geos::geom::Polygon *>(g) ) {
			AddPolygonBoundaries((geos::geom::Polygon *)g, record);	
		} else if ( dynamic_cast<geos::geom::GeometryCollection *>(g) ) {
			for(int k=0; k < g->getNumGeometries(); ++k) {
				const Geometry *ls = g->getGeometryN(k);
				if ( g->getGeometryType() == "Polygon")
					AddPolygonBoundaries(dynamic_cast<const geos::geom::Polygon *>(g),record);
				else
					AddFeature(ls,record);
			}
		}
	}

	for(int i =0; i < mergedGeometries.size(); ++i)
		delete mergedGeometries[i];
	return true;
}

bool SegmentMapUnion::MergeSegments(vector<Geometry *> inputGeometries, vector<Geometry *>& mergedGeometries) {
	vector<bool> handled(inputGeometries.size(),false);
	for(long i=0; i < inputGeometries.size(); ++i) {
		if(trq.fUpdate(i,inputGeometries.size()))
			return false;
		Geometry *gInput1 = inputGeometries[i];
		if ( gInput1->isEmpty()) continue;
		vector<Geometry *> intersects;
		for(long j=0; j < inputGeometries.size(); ++j) {
			if ( i == j ) continue;
			if ( handled[j]) continue;
			Geometry *gInput2 = inputGeometries[j];
			if ( (dynamic_cast<Feature *>(gInput1))->EnvelopeIntersectsWith(gInput2)) {
				intersects.push_back(gInput2->clone());
				handled[j] = handled[i] = true;
			}
		}
		Geometry *gTotal = NULL;
		for( int k = 0; k < intersects.size(); ++k) {
			if ( gTotal == NULL)
				gTotal = gInput1->Union(intersects[k]);
			else {
				Geometry *gNew = gTotal->Union(intersects[k]);
				delete gTotal;
				gTotal = gNew;
			}
		}
		if ( gTotal && !gTotal->isEmpty())
			mergedGeometries.push_back(gTotal);
		else if ( handled[i] == false)
			mergedGeometries.push_back(gInput1->clone());
		handled[i] = true;
		for(int n=0; n < intersects.size(); ++n)
			delete intersects[n];
	}
	return true;
}

void  SegmentMapUnion::AddFeature(const geos::geom::Geometry *g, long& record) {
	ILWIS::Segment *p = CSEGMENT(ptr.newFeature(const_cast<Geometry *>(g)));
	if (!p) return;
	p->PutVal( record );
	++record;
}
void SegmentMapUnion::AddPolygonBoundaries(const geos::geom::Polygon *pol, long& record) {
	const LineString *s = pol->getExteriorRing();
	AddFeature(s,record);
	for(int n=0; n < pol->getNumInteriorRing(); ++n) {
		s = pol->getInteriorRingN(n);
		AddFeature(s,record);

	}
}









