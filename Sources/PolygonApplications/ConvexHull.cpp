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
// Improved use of mask. Use now TINPoint::fInMask(mask)
//
/* PolygonMapConvexHull
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    5:19 pm
*/                                                                      

#include <geos/algorithm/ConvexHull.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "PolygonApplications\ConvexHull.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\System\Engine.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\Point.hs"

IlwisObjectPtr * createPolygonMapConvexHull(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapConvexHull::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapConvexHull(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapConvexHull::sSyntax() {
  return "PolygonMapConvexHull(inputmap, combined)";
}

PolygonMapConvexHull* PolygonMapConvexHull::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName = as[0];
	bool combined = as[1].toLower() == "true";
	FileName fnInput(sInputMapName);


  //BaseMap basemp(FileName(ssInputMapName));
  return new PolygonMapConvexHull(fn, p, BaseMap(fnInput), combined);
}

PolygonMapConvexHull::PolygonMapConvexHull(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	  FileName fn;
    ReadElement("PolygonMapConvexHull", "InputMap", fn);
	bmp = BaseMap(fn);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
     ReadElement("PolygonMapConvexHull", "Combined", fCombined);
  Init();
  objdep.Add(bmp.ptr());
}

PolygonMapConvexHull::PolygonMapConvexHull(const FileName& fn, PolygonMapPtr& p, const BaseMap& sm, bool combined)
: PolygonMapVirtual(fn, p, sm->cs(),sm->cb(), fCombined ? Domain(fn, 1, dmtUNIQUEID, "Pol") : sm->dm()), bmp(sm), fCombined(combined)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(bmp.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if ( !fCombined && sm->fTblAtt()) {
	  ptr.SetAttributeTable(sm->tblAtt());
  }
 
}


void PolygonMapConvexHull::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapConvexHull");
  WriteElement("PolygonMapConvexHull", "InputMap", bmp);
  WriteElement("PolygonMapConvexHull", "Combined", fCombined);
}

PolygonMapConvexHull::~PolygonMapConvexHull()
{
}

String PolygonMapConvexHull::sExpression() const
{
	String comb = fCombined ? "true" : "false";
	return String("PolygonMapConvexHull(%S,%S)", bmp->sNameQuoted(true, fnObj.sPath()), comb);
}

bool PolygonMapConvexHull::fDomainChangeable() const
{
  return false;
}

void PolygonMapConvexHull::Init()
{
  //htpFreeze = htpPolygonMapConvexHullT;
  sFreezeTitle = "PolygonMapConvexHull";
}

bool PolygonMapConvexHull::fFreezing()
{
	trq.SetText("adding features" );

	long iFeatures = bmp->iFeatures();
	vector<Geometry *> *features = new vector<Geometry *>();
	for (int i=0; i < iFeatures; ++i )  {
		Geometry *g = bmp->getFeature(i);
		Feature *original = dynamic_cast<Feature *>(g);
		if ( !original || !original->fValid())
			continue;
		trq.fUpdate(i, iFeatures);
		if ( fCombined) {
			features->push_back(g);		
		} else {
			try{
				geos::algorithm::ConvexHull hull(g);
				geos::geom::Geometry *geom = hull.getConvexHull();
				if ( dynamic_cast<geos::geom::Polygon *>(geom)) {
					Feature *feature = ptr.newFeature(dynamic_cast<geos::geom::Polygon *>(geom));
					feature->PutVal(original->rValue());
				}
			}catch (geos::util::GEOSException ex ) {
				getEngine()->getLogger()->LogLine(ex.what(),Logger::lmERROR);
			}
		}
	}
	if ( fCombined) {
		try{
			GeometryFactory *fact = new GeometryFactory();
			GeometryCollection *col = fact->createGeometryCollection(features);
			geos::algorithm::ConvexHull hull(col);
			geos::geom::Geometry *geom = hull.getConvexHull();
			if ( dynamic_cast<geos::geom::Polygon *>(geom)) {
				Feature *feature = ptr.newFeature(dynamic_cast<geos::geom::Polygon *>(geom));
				feature->PutVal(1L);
			} 
			delete fact;
		}catch (geos::util::GEOSException ex ) {
			getEngine()->getLogger()->LogLine(ex.what(),Logger::lmERROR);
			throw ErrorObject(ex.what());
		}
		//delete col;

	}
	trq.fUpdate(iFeatures, iFeatures);
	return true;
}







