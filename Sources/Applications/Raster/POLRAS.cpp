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
/* $Log: /ILWIS 3.0/RasterApplication/POLRAS.cpp $
 * 
 * 12    20-03-01 18:49 Koolhoven
 * in fFreezing() use a correction of a half in both (horizontal and
 * vertical) directions to correct for that a segment is the boundary
 * between two polygons
 * 
 * 11    19/02/01 14:38 Willem
 * Added errormessage when input map is attribute map (map.col). This is
 * not possible anymore.
 * 
 * 10    9-11-00 19:38 Hendrikse
 * removed redundant smpPatchTmp->Store() at the begin of fFreezing  and
 * constructor for mpPatchTmp at the end
 * 
 * 9     17-01-00 15:00 Wind
 * adapted to removal of internal coordinates
 * 
 * 8     10-01-00 4:08p Martin
 * disabled the freezing functions. Rowcols need to be changed to true
 * coords. Must be done by someone who understands the algorithm
 * 
 * 7     18-11-99 11:07a Martin
 * Missed an error messages when porting from 2.23
 * 
 * 6     11-11-99 13:05 Wind
 * solved bug 1132 (8 byte real as input)
 * 
 * 5     9/08/99 12:58p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 4     9/08/99 8:58a Wind
 * changed sName() to sNameQuoted() in sExpression() to support long file
 * names
 * 
 * 3     9-06-99 4:12p Martin
 * // -> /*
 * 
 * 2     6/09/99 11:34a Martin
 * Changed Array<int> to ArrayInt to solve compile problems (short, int)
// Revision 1.8  1998/09/16 17:24:41  Wim
// 22beta2
//
// Revision 1.7  1998/03/05 13:05:05  Wim
// Removed supefluous half pixel correction if fFreezing
//
// Revision 1.6  1998-02-18 17:27:29+01  Willem
// *** empty log message ***
//
// Revision 1.5  1997/09/11 09:11:44  Wim
// mpPatchTmp is no longer a member of MapRasterizePolygon,
// that caused the temporary map to linger around much too long
//
// Revision 1.4  1997-08-21 18:42:28+02  Wim
// SetDomainValueRangeStruct in fFreezing() to dvrs() of polygon map
//
// Revision 1.3  1997-08-14 10:22:56+02  Wim
// fInPatch() and fInAllPatches() calculate now with long integers and not with doubles,
// this is a small optimization.
// PrepareLine() for the horizontal line parts rewritten.
//
/* MapRasterizePolygon
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    5 Mar 98    1:56 pm
*/
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\Raster\POLRAS.H"
#include "Engine\Base\System\Engine.h"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\polygon.hs"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapRasterizePolygon(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapRasterizePolygon::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapRasterizePolygon(fn, (MapPtr &)ptr);
}

String wpsmetadataMapRasterizePolygon() {
	WPSMetaData metadata("MapRasterizePolygon");
	return metadata.toString();
}

ApplicationMetadata metadataMapRasterizePolygon(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapRasterizePolygon();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapRasterizePolygon::sSyntax();

	return md;
}

const char* MapRasterizePolygon::sSyntax() {
  return "MapRasterizePolygon(polmap,georef)";
}

MapRasterizePolygon* MapRasterizePolygon::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());

  	// Disable use of attribute maps. A PolygonMapAttribute has no method to access the topologies
	// via the MapVirtual (yet)
	String sInputMapName = as[0];
	char *pCh = sInputMapName.strrchrQuoted('.');
	if ((pCh != 0) && (0 != _strcmpi(pCh, ".mpa")))  // attrib map
		throw ErrorObject(WhatError(String(SMAPErrNoAttColumnAllowed_S.scVal(), as[0]), errMapRasterizePolygon), fn);
	
	PolygonMap pm(as[0], fn.sPath());
	GeoRef gr(as[1], fn.sPath());
	if (gr->fGeoRefNone())
		throw ErrorGeoRefNone(gr->fnObj, errMapRasterizePolygon);
	return new MapRasterizePolygon(fn, p, pm, gr);
}


MapRasterizePolygon::MapRasterizePolygon(const FileName& fn, MapPtr& p)
: MapFromPolygonMap(fn, p)
{
  fNeedFreeze = true;
  sFreezeTitle = "MapRasterizePolygon";
  htpFreeze = "ilwisapp\\polygons_to_raster_algorithm.htm";
  objdep.Add(gr().ptr());
}

MapRasterizePolygon::MapRasterizePolygon(const FileName& fn, MapPtr& p,
           const PolygonMap& pm, const GeoRef& gr)
: MapFromPolygonMap(fn, p, pm, gr)
{
//  if (st() == stREAL)
//    throw ErrorObject(SPOLErrRealMapNotSupported, errMapRasterizePolygon+1);
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, errMapRasterizePolygon);
  if (!cs()->fConvertFrom(pm->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pm->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapRasterizePolygon+2);
  fNeedFreeze = true;
  sFreezeTitle = "MapRasterizePolygon";
  htpFreeze = "ilwisapp\\polygons_to_raster_algorithm.htm";
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapRasterizePolygon::Store()
{
  MapFromPolygonMap::Store();
  WriteElement("MapFromPolygonMap", "Type", "MapRasterizePolygon");
}

MapRasterizePolygon::~MapRasterizePolygon()
{
}

String MapRasterizePolygon::sExpression() const
{
  return String("MapRasterizePolygon(%S,%S)", 
                pm->sNameQuoted(true, fnObj.sPath()), 
                gr()->sNameQuoted(true, fnObj.sPath()));
}

//long MapRasterizePolygon::iComputePixelRaw(RowCol) const
//{
//  return iUNDEF;
//}
//  
//double MapRasterizePolygon::rComputePixelVal(RowCol) const
//{
//  return rUNDEF;
//}

bool MapRasterizePolygon::fFreezing() {
	FileName fn(FileName::fnUnique(pm->fnObj),".mps");
	String expr(String("SegmentMapPolBoundaries(%S,*,single)", pm->fnObj.sRelative()));
	SegmentMap segmap(fn,expr);
	segmap->Calc();
	segmap->fErase = true;
	FileName fnRas(FileName::fnUnique(pm->fnObj),".mpr");
	expr = String("MapRasterizeSegment(%S,%S)", fn.sRelative(), ptr.gr()->fnObj.sRelative());
	//getEngine()->Execute(expr);
	Map rasmap(fnRas,expr);
	rasmap->fErase = true;
	rasmap->Calc();
	RowCol rcSize = ptr.gr()->rcSize();
	LongBuf line(rcSize.Col);
	LongBuf newline(rcSize.Col);
	for(long y = 0; y < rcSize.Row; ++y){
		   if (trq.fUpdate(y, rcSize.Row))
			  return false;
			vector<long> border;
			border.push_back(0);
			rasmap->GetLineRaw(y,line);
			for(int x=0;  x < rcSize.Col; ++x) {
				newline[x] = iUNDEF;
				if ( line[x] != iUNDEF) {
					border.push_back(x);
				}
			}
			border.push_back(rcSize.Col);
			for(int i=1; i<border.size(); ++i) {
				long middle = (border[i-1] + border[i] )/ 2;
				long value = FindValue(y,middle);
				for(int j=border[i-1]; j < border[i]; ++j) {
					newline[j] = value;
				}
			}
			ptr.PutLineRaw(y,newline);
	}
	return true;

}

long MapRasterizePolygon::FindValue(long row, long col) const {

	Coord crdRas = ptr.gr()->cConv(RowCol(row, col));
	Coord crdPol = pm->cs()->cConv(ptr.gr()->cs(), crdRas);
	const vector<Geometry *> pols = pm->getFeatures(crdPol);
	if (pols.size() > 0)
		return ((ILWIS::Polygon *)pols[0])->iValue();
	return iUNDEF;
}






