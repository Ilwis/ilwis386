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
/* MapRasterizeSegment
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    5 Mar 98    1:57 pm
*/
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\Raster\SEGRAS.H"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\segment.hs"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapRasterizeSegment(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapRasterizeSegment::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapRasterizeSegment(fn, (MapPtr &)ptr);
}

String wpsmetadataMapRasterizeSegment() {
	WPSMetaData metadata("MapRasterizeSegment");
	return metadata.toString();
}

ApplicationMetadata metadataMapRasterizeSegment(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapRasterizeSegment();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapRasterizeSegment::sSyntax();

	return md;
}
const char* MapRasterizeSegment::sSyntax() {
  return "MapRasterizeSegment(segmap,georef)";
}

MapRasterizeSegment* MapRasterizeSegment::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
//  if ((iParms < 1) || (iParms > 3))  // 8 connected parm is not used:
  if ((iParms < 1) || (iParms > 2))
    ExpressionError(sExpr, sSyntax());
  SegmentMap sm(as[0], fn.sPath());
  GeoRef gr(as[1], fn.sPath());
/*  8 connected parm is not used
  short iCon = 8;
  if (iParms == 3) {
    iCon = (short)as[2];
    if ((iCon != 4) && (iCon != 8))
      throw ErrorExpression(0, sExpr);
  }
  return new MapRasterizeSegment(fn, sm, gr, iCon==8);*/
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, errMapRasterizeSegment);
  return new MapRasterizeSegment(fn, p, sm, gr, true);
}


MapRasterizeSegment::MapRasterizeSegment(const FileName& fn, MapPtr& p)
: MapFromSegmentMap(fn, p)
{
  fNeedFreeze = true;
  sFreezeTitle = "MapRasterizeSegment";
  htpFreeze = "ilwisapp\\segments_to_raster_algorithm.htm";
  objdep.Add(gr().ptr());
}

MapRasterizeSegment::MapRasterizeSegment(const FileName& fn, MapPtr& p,
           const SegmentMap& sm, const GeoRef& gr, const bool& f8C)
: MapFromSegmentMap(fn, p, sm, gr, mfLine)
{
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, errMapRasterizeSegment);
  if (!cs()->fConvertFrom(sm->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), sm->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapRasterizeSegment+2);
  fNeedFreeze = true;
  sFreezeTitle = "MapRasterizeSegment";
  htpFreeze = "ilwisapp\\segments_to_raster_algorithm.htm";
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapRasterizeSegment::Store()
{
  MapFromSegmentMap::Store();
  WriteElement("MapFromSegmentMap", "Type", "MapRasterizeSegment");
}

MapRasterizeSegment::~MapRasterizeSegment()
{
}

String MapRasterizeSegment::sExpression() const
{
  return String("MapRasterizeSegment(%S,%S)", sm->sNameQuoted(true, fnObj.sPath()), gr()->sNameQuoted(true, fnObj.sPath()));
}

long MapRasterizeSegment::iComputePixelRaw(RowCol) const
{
  return iUNDEF;
}

double MapRasterizeSegment::rComputePixelVal(RowCol) const
{
  return rUNDEF;
}

bool MapRasterizeSegment::inBounds(RowCol cur, RowCol rcSize) {
  return !( cur.Col < 0 || cur.Row < 0 || cur.Col >= rcSize.Col || cur.Row >= rcSize.Row);
}

void MapRasterizeSegment::Rasterize(RowCol begin, RowCol end, vector<RowCol>& result, RowCol rcSize) {
	if ( begin.Col < 0 && end.Col < 0)
		return;
	if ( begin.Col > rcSize.Col && end.Col > rcSize.Col)
		return;
	if ( begin.Row < 0 && end.Row < 0)
		return;
	if ( begin.Row > rcSize.Row && end.Row > rcSize.Row)
		return;
	if ( (begin.Col - end.Col) == 0) {   // angle = 90
		if ( end.Row < begin.Row) {
			RowCol temp = end;
		 	end = begin;
			begin = temp;
		}
		for(double y=0; y < end.Row - begin.Row; ++y) {
			RowCol cur = RowCol( (double)begin.Row + y, (double)begin.Col);
			
			if ( inBounds(cur, rcSize))
				result.push_back(cur);
		}
	}
	else {
		double tan = (double)(begin.Row - end.Row) / ( begin.Col - end.Col);
		bool dir = end.Row < begin.Row;
		if  (dir){
			RowCol temp = end;
			end = begin;
			begin = temp;
		}

		RowCol prev;
		RowCol cur;
		if ( abs(tan) <= 1) {
			double xdir = end.Col - begin.Col > 0 ? 1 : -1;
			for(double x=0; x <= abs(end.Col - begin.Col); ++x) {
				cur = RowCol( (double)begin.Row + tan * x  * xdir, (double)begin.Col + xdir * x );\
				if ( cur == prev)
					continue;
				if ( inBounds(cur, rcSize))
					result.push_back(cur);
				prev= cur;
			}
		
		} else {
			double ydir = end.Row - begin.Row > 0 ? 1 : -1;
			for(double y=0; y <= abs(end.Row - begin.Row); ++y) {
				cur = RowCol( (double)begin.Row + y, (double)begin.Col + 1.0/tan * ydir * y);
				if ( cur == prev)
					continue;
				if ( inBounds(cur, rcSize))
					result.push_back(cur);
				prev = cur;
			}
		}
	}
}

void *MapRasterizeSegment::CreateMapBuffers() {
	int xsize = ptr.rcSize().Col;
	int ysize = ptr.rcSize().Row;
	void *buffer;
	switch (st()){
		case stBYTE:
		case stLONG:
		case stINT:
			{
				vector<LongBuf> *buf = new vector<LongBuf>();
				buf->resize(ysize);
				for(int i=0; i < ysize;++i){
					(*buf)[i].Size(xsize);
					for(int j=0; j < xsize; ++j)
						(*buf)[i][j] = iUNDEF;

				}
				buffer = buf;
			}
			break;
		case stREAL:
			{
				vector<RealBuf> *buf = new vector<RealBuf>();
				buf->resize(ysize);
				for(int i=0; i < ysize;++i){
					(*buf)[i].Size(xsize);
					for(int j=0; j < xsize; ++j)
						(*buf)[i][j] = rUNDEF;

				}
				buffer = buf;
			}
			break;

	};
	return buffer;
}

void MapRasterizeSegment::PutInMapBuffer(void *buf, const vector<RowCol>& result, const ILWIS::Segment* seg) {
	switch (st()) {
		case stBYTE:
		case stLONG:
		case stINT:
			{
				vector<LongBuf> *buffer = (vector<LongBuf> *)buf;
				for(int i =0; i<result.size(); ++i) {
					RowCol sz = result[i];
					if ( sz.Row >= buffer->size())
						continue;
					LongBuf &b = (*buffer)[sz.Row];
					b[sz.Col] = seg->iValue();
				}
			}
			break;
		case stREAL:
			{
				vector<RealBuf> *buffer = (vector<RealBuf> *)buf;
				for(int i =0; i<result.size(); ++i) {
					RowCol sz = result[i];
					RealBuf &b = (*buffer)[sz.Row];
					b[sz.Col] = seg->rValue();
				}
			}
		
			break;

	};
}

void MapRasterizeSegment::StoreMapBuffer(void *buf) {

	for(int y = 0; y < ptr.rcSize().Row; ++y) {
		switch (st())  {
			case stBYTE:
			case stLONG:
			case stINT: 
				{
				vector<LongBuf> *buffer = (vector<LongBuf> *)buf;
				if ( ptr.dvrs().fRawAvailable())
					ptr.PutLineRaw(y, (*buffer)[y]);
				else
					ptr.PutLineVal(y, (*buffer)[y]);
				}
				break;
			case stREAL:
				{				
				vector<RealBuf> *buffer = (vector<RealBuf> *)buf;
				ptr.PutLineVal(y, (*buffer)[y]);
				}
				break;

		};
	}
}
bool MapRasterizeSegment::fFreezing()
{
	if (0 == sm->sms()) {
		throw ErrorObject(TR("Rasterizing of Attribute Map not possible"));
	}

	bool fTransformCoords = cs() != sm->cs();
	void *buffer = CreateMapBuffers();
	RowCol rcSize = ptr.gr()->rcSize();
	CoordBounds cb1 = ptr.cb();
	CoordBounds cb2 = sm->cb();
	for (int i=0; i<sm->iFeatures(); ++i) {
		  ILWIS::Segment *segCur = CSEGMENT(sm->getFeature(i));
		  if (!segCur || !segCur->fValid())
			  continue;
		  if (trq.fUpdate(i, sm->iFeatures()))
			  return false;
		  CoordinateSequence *seq = segCur->getCoordinates();
		  for(int j=0; j<seq->size() - 1;++j) {
			  Coordinate cBegin = seq->getAt(j);
			  Coordinate cEnd = seq->getAt(j+1);
			  vector<RowCol> line;
			  if ( fTransformCoords) {
					cBegin = cs()->cConv(sm->cs(), cBegin);
					cEnd = cs()->cConv(sm->cs(), cEnd);
			  }
			  RowCol rcBegin = ptr.gr()->rcConv(cBegin);
			  RowCol rcEnd = ptr.gr()->rcConv(cEnd);
			  Rasterize(rcBegin, rcEnd,line,rcSize);
			  if ( line.size() > 0)
				PutInMapBuffer(buffer, line,segCur);
		  }
		  delete seq;

	}
	StoreMapBuffer(buffer);
	delete buffer;
	return true;
}

