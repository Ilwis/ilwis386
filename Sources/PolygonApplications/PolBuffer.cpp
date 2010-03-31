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
/* PolygonMapBuffer
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    5:19 pm
*/                                                                      

#include <geos/opBuffer.h>
#include <geos\operation\buffer\BufferParameters.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "PolygonApplications\PolBuffer.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\System\Engine.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createPolygonMapBuffer(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapBuffer::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapBuffer(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapBuffer::sSyntax() {
  return "PolygonMapBuffer(inputmap, distance, endstyle)";
}

PolygonMapBuffer* PolygonMapBuffer::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
	Array<String> as(3);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName = as[0];

	FileName fnInput(sInputMapName);
	 
  //BaseMap basemp(FileName(ssInputMapName));
  return new PolygonMapBuffer(fn, p, BaseMap(fnInput), as[1].rVal(), as[2]);
}

PolygonMapBuffer::PolygonMapBuffer(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	  FileName fn;
    ReadElement("PolygonMapBuffer", "InputMap", fn);
	bmp = BaseMap(fn);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  ReadElement("PolygonMapBuffer", "Distance", distance);
  ReadElement("PolygonMapBuffer","CapStyle",endStyle);
  Init();
  objdep.Add(bmp.ptr());
}

PolygonMapBuffer::PolygonMapBuffer(const FileName& fn, PolygonMapPtr& p, const BaseMap& sm, 
                               double rDist, const String& style)
: PolygonMapVirtual(fn, p, sm->cs(),sm->cb(), Domain(fn, sm->iFeatures(), dmtUNIQUEID, "Pol")), bmp(sm)//, asMask(asMsk)
{
  distance = rDist;
  fNeedFreeze = true;
  Init();
  objdep.Add(bmp.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  endStyle = geos::operation::buffer::BufferParameters::CAP_ROUND;
  if ( String(style).toLower() == "round")
	  endStyle =geos::operation::buffer::BufferParameters::CAP_ROUND;
  else if (String(style).toLower() == "butt")
	  endStyle =geos::operation::buffer::BufferParameters::CAP_FLAT;
   else if (String(style).toLower() == "square")
	  endStyle =geos::operation::buffer::BufferParameters::CAP_SQUARE;
}


void PolygonMapBuffer::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapBuffer");
  WriteElement("PolygonMapBuffer", "InputMap", bmp);
  WriteElement("PolygonMapBuffer", "Distance", distance);
  WriteElement("PolygonMapBuffer", "CapStyle", endStyle);
}

PolygonMapBuffer::~PolygonMapBuffer()
{
}

String PolygonMapBuffer::sExpression() const
{
	String style;
	if ( endStyle == 1)
		style = "Round";
	else if ( endStyle == 2)
		style = "Butt";
	else if (endStyle == 3)
		style == "Square";
  return String("PolygonMapBuffer(%S,%f,%S)", bmp->sNameQuoted(false, fnObj.sPath()), distance, style);
}

bool PolygonMapBuffer::fDomainChangeable() const
{
  return false;
}

void PolygonMapBuffer::Init()
{
  //htpFreeze = htpPolygonMapBufferT;
  sFreezeTitle = "PolygonMapBuffer";
}

bool PolygonMapBuffer::fFreezing()
{
	trq.SetText("Buffering" );
	long iFeatures = bmp->iFeatures();
	long iPntNr = 1;
	CoordBuf crdBuf;
	vector<Geometry *> *geoms = new vector<Geometry *>();
	for (int i=0; i < iFeatures; ++i )  {
		Geometry *g = bmp->getFeature(i);
		Feature *feature = dynamic_cast<Feature *>(g);
		if ( !feature || !feature->fValid())
			continue;
		trq.fUpdate(i, iFeatures);
		geoms->push_back(g);
	}
	try{
		GeometryFactory fact;
		GeometryCollection *col = fact.createGeometryCollection(geoms);
		geos::operation::buffer::BufferOp buffer(col);
		buffer.setEndCapStyle(endStyle);

		Geometry *g = buffer.getResultGeometry(distance);
		trq.SetText("Creating features");
		trq.fUpdate(0, iFeatures);
		if ( dynamic_cast<MultiPolygon *>(g)) {
			MultiPolygon *mp = (MultiPolygon *)g;
			for(long p=0; p < mp->getNumGeometries(); ++p) {
				geos::geom::Polygon *pol = (geos::geom::Polygon *)mp->getGeometryN(p);
				ILWIS::Polygon *polNew = CPOLYGON(ptr.newFeature(pol));
				polNew->PutVal(p+1);
				if (trq.fUpdate(p, mp->getNumGeometries()))
					return false;

			}
		} else if ( dynamic_cast<geos::geom::Polygon *>(g)) {
			geos::geom::Polygon *pol = (geos::geom::Polygon *)g;
			ILWIS::Polygon *polNew = CPOLYGON(ptr.newFeature(g));
			polNew->PutVal(1L);
		}
	} catch (geos::util::GEOSException ex ) {
		getEngine()->getLogger()->LogLine(ex.what(),Logger::lmERROR);
		throw ErrorObject(ex.what());
	}
	trq.fUpdate(iFeatures, iFeatures);
	return true;
}

LinearRing *PolygonMapBuffer::makeRing(const LineString *line) const {
	CoordinateSequence *seq = line->getCoordinates();
	return new LinearRing(seq, new GeometryFactory());
}




