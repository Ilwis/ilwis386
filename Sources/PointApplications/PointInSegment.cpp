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
#include "geos\algorithm\InteriorPointline.h"
#include "geos\algorithm\CentroidLine.h"
#include "PointApplications\PointInSegment.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createPointMapPointInSegment(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapPointInSegment::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapPointInSegment(fn, (PointMapPtr &)ptr);
}

const char* PointMapPointInSegment::sSyntax() {
  return "PointMapPointInSegment(inputmap, option)";
}

PointMapPointInSegment* PointMapPointInSegment::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName1 = as[0];
	FileName fnInput1(sInputMapName1);
	SegmentMap sm1(fnInput1);
	String opt = as[1].toLower();
	if ( opt != "centroid" && opt != "vertex" && opt != "middlepoint") {
		throw ErrorExpression(sExpr, sSyntax());
	}

  //BaseMap basemp(FileName(ssInputMapName));
	return new PointMapPointInSegment(fn, p, sm1, opt);
}

PointMapPointInSegment::PointMapPointInSegment(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	 FileName fn1;
    ReadElement("PointMapPointInSegment", "InputMap1", fn1);
	lines = SegmentMap(fn1);
    ReadElement("PointMapPointInSegment", "Option", option);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(lines.ptr());
}

PointMapPointInSegment::PointMapPointInSegment(const FileName& fn, PointMapPtr& p, const SegmentMap& pm1, const String& _option)
: PointMapVirtual(fn, p, pm1->cs(), pm1->cb(),pm1->dm()), 
	lines(pm1),
	option(_option)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(lines.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
}


void PointMapPointInSegment::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapPointInSegment");
  WriteElement("PointMapPointInSegment", "InputMap1", lines);
  WriteElement("PointMapPointInSegment", "Option", option);
}

PointMapPointInSegment::~PointMapPointInSegment()
{
}
																   
String PointMapPointInSegment::sExpression() const
{
	return String("PointMapPointInSegment(%S,%S)", lines->fnObj.sRelativeQuoted(false), option);
}

bool PointMapPointInSegment::fDomainChangeable() const
{
  return false;
}

void PointMapPointInSegment::Init()
{
  //htpFreeze = htpPointMapPointInSegmentT;
  sFreezeTitle = "PointMapPointInSegment";
}

bool PointMapPointInSegment::fFreezing()
{
	trq.SetText("PointInSegment of features" );
	for(int i=0; i < lines->iFeatures(); ++i) {
		ILWIS::Segment *line = CSEGMENT(lines->getFeature(i));
		if ( line && line->fValid()) {
			Coord cInternal, c;
			geos::algorithm::CentroidLine cl;
			cl.add(line);
			const Coordinate *centroid = cl.getCentroid();
			if ( option == "centroid") {
				c = *centroid;
			} else { 
				geos::algorithm::InteriorPointLine ipl(line);
				ipl.getInteriorPoint(cInternal);
				if ( option == "vertex") {
					c = cInternal;
				} else if ( option == "middlepoint") {
					const CoordinateSequence *seq = line->getCoordinates();
					for(int j = 0; j < seq->size(); ++j) {
						Coord c1 = seq->getAt(j);
						Coord c0,c2;
						if ( c1 != cInternal)
							continue;
						if ( j == 0) {
							c = calcIntersection(c1, seq->getAt(1), centroid);
							break;
						} else if ( j == seq->size() - 1) {
							c = calcIntersection(c1, seq->getAt(j-1), centroid);
							break;
						} else {
							c0 = calcIntersection(c1, seq->getAt(j-1), centroid);
							c2 = calcIntersection(c1, seq->getAt(j+1), centroid);
							double rd1 = (c0.x - centroid->x)*(c0.x - centroid->x) + (c0.y - centroid->y)*(c0.y - centroid->y);
							double rd2 = (c2.x - centroid->x)*(c2.x - centroid->x) + (c2.y - centroid->y)*(c2.y - centroid->y);
							c =  rd1 > rd2 ? c2 : c1;
							break;

						}

					}

				}
			}
			ILWIS::Point *pnt = CPOINT(ptr.newFeature());
			pnt->setCoord(c);
			pnt->PutVal(line->rValue());
		}
	}

	return true;
}

Coord PointMapPointInSegment::calcIntersection(const Coord& c1, const Coord& c2, const Coordinate* centroid) {
	double dx = c1.x - c2.x;
	double dy = c1.y - c2.y;
	double a1 = dx != 0 ? dy/dx : rUNDEF;
	if ( a1 == rUNDEF)
		return Coord(c1.x, centroid->y);
	double b1 = c1.y - a1 * c1.x;
	double a2 = -1/a1;
	double b2 = centroid->y - a2 * centroid->x;
	double xs = (b2 - b1) / (a1 - a2);
	double ys = a1 * xs + b1;
	return Coord(xs,ys);
}









