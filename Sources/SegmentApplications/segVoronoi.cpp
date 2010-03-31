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
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "SegmentApplications\segVoronoi.H"
#include "SegmentApplications\VoronoiDiagramGenerator.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapVoronoi(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapVoronoi::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapVoronoi(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapVoronoi::sSyntax() {
  return "SegmentMapVoronoi(inputmap, mindistance)";
}

SegmentMapVoronoi* SegmentMapVoronoi::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName1 = as[0];
	double minDistance = as[1].rVal();
	FileName fnInput1(sInputMapName1);
	PointMap pm1(fnInput1);
	Domain dm = Domain(fn, pm1->iFeatures(), dmtUNIQUEID, "Seg");

  //BaseMap basemp(FileName(ssInputMapName));
	return new SegmentMapVoronoi(fn, p, dm, pm1, minDistance);
}

SegmentMapVoronoi::SegmentMapVoronoi(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1;
    ReadElement("SegmentMapVoronoi", "InputMap1", fn1);
	points = PointMap(fn1);
    ReadElement("SegmentMapVoronoi", "MinimumDistance", minDistance);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(points.ptr());
}

SegmentMapVoronoi::SegmentMapVoronoi(const FileName& fn, SegmentMapPtr& p, const Domain& dm, const PointMap& pm1, double dist)
: SegmentMapVirtual(fn, p, pm1->cs(), pm1->cb(),dm), 
	points(pm1),
	minDistance(dist)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(points.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
 /* Table tbl;
  tbl = Table(FileName(ptr.fnObj,".tbt"), ptr.dm());
  String colName2 = pm1->fnObj.sFile == pm2->fnObj.sFile ? pm1->fnObj.sFile + "_1" : pm2->fnObj.sFile;
  Column col1(tbl,pm1->fnObj.sFile,pm1->dm());
  Column col2(tbl,colName2, pm2->dm());
  tbl->AddCol(col1);
  tbl->AddCol(col2);
  SetAttributeTable(tbl);*/
 
}


void SegmentMapVoronoi::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapVoronoi");
  WriteElement("SegmentMapVoronoi", "InputMap1", points);
  WriteElement("SegmentMapVoronoi", "MinimumDistance", minDistance);
}

SegmentMapVoronoi::~SegmentMapVoronoi()
{
}

String SegmentMapVoronoi::sExpression() const
{
	return String("SegmentMapVoronoi(%S,%d)", points->fnObj.sRelativeQuoted(false), minDistance);
}

bool SegmentMapVoronoi::fDomainChangeable() const
{
  return false;
}

void SegmentMapVoronoi::Init()
{
  //htpFreeze = htpSegmentMapVoronoiT;
  sFreezeTitle = "SegmentMapVoronoi";
}

bool SegmentMapVoronoi::fFreezing()
{
	trq.SetText("Voronoi of features" );
	long total = points->iFeatures();
	vector<Coord> coords;
	for(int i=0; i < points->iFeatures(); ++i) {
		ILWIS::Point *pnt = CPOINT(points->getFeature(i));
		if ( !pnt || pnt->fValid() == false)
			continue;
		if(trq.fUpdate(i, points->iFeatures()))
			return false;
		const Coordinate *crd = pnt->getCoordinate();
		coords.push_back(*crd);
	}
	VoronoiDiagramGenerator generator;
	generator.generateVoronoi(coords,points->cb(),minDistance);
	Coord c1,c2;
	long index = 0;
	generator.resetIterator();
	trq.fUpdate(0,0);
	trq.SetText("Calculating lines");
	CoordBounds cb = ptr.cb();
	vector<vector<Coord>> boundaryLines(4);
	while( generator.getNext(c1,c2)) {
		ILWIS::Segment *seg = CSEGMENT(ptr.newFeature());
		if(trq.fUpdate(index))
			return false;
		CoordBuf buf(2);
		buf[0] = c1;
		buf[1] = c2;
		onBoundary(c1,boundaryLines);
		onBoundary(c2,boundaryLines);
		seg->PutCoords(2,buf);
		seg->PutVal(++index);
	}
	return true;
}

void SegmentMapVoronoi::onBoundary(Coord c,vector<vector<Coord>> boundaryLines) {
	CoordBounds cb = ptr.cb();
	double r1 = c.x - cb.cMax.x;
	double r2 = c.y - cb.cMax.y;
	double r3 = c.x - cb.cMin.x;
	double r4 = c.y - cb.cMin.y;
	double deltax = cb.width() * 0.001;
	double deltay = cb.height() * 0.001;
	if ( abs(r1) < deltax){
		boundaryLines[0].push_back(c); //top
	}
	if ( abs(r3) < deltax){
		boundaryLines[1].push_back(c); //bottom
	}
	if ( abs(r2) < deltay)  {
		boundaryLines[2].push_back(c); //right
	}
	if ( abs(r4) < deltay){
		boundaryLines[3].push_back(c); //left
	}
}









