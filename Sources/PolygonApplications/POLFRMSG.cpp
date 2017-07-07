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
// $Log: /ILWIS 3.0/PolygonMap/POLFRMSG.cpp $
 * 
 * 14    18-05-05 15:07 Retsios
 * [bug=6448,6546] When supplying a point map for labels, the point map's
 * Domain is insufficient to determine the domain of the output - both the
 * domain and the value range are needed, otherwise in fFreezing the raw
 * values from the point map do not match the ones of the output polygon
 * map.
 * 
 * 13    21-02-01 18:45 Koolhoven
 * in create() check on "unique" with correct param
 * 
 * 12    19/02/01 10:49 Willem
 * Added error message: attribute column in input map is not possible
 * 
 * 11    5-02-01 19:39 Koolhoven
 * added option "unique" to syntax. (same as leaving away)
 * 
 * 10    5-02-01 19:11 Koolhoven
 * added MB_TOPMOST flag to messagebox which is shown when polygonization
 * fails
 * 
 * 9     12-12-00 10:13 Hendrikse
 * placed Init()  in 'read-" constructor to make that sFreezeTitle is
 * shown during freezing (calculation) of the map
 * 
 * 8     16-06-00 18:55 Koolhoven
 * Auto identifiers now uses DomainUniqueID
 * 
 * 7     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 6     10-12-99 3:14p Martin
 * removed internal coordinates and used true coords
 * 
 * 5     9/29/99 10:29a Wind
 * added case insensitive string comparison
 * 
 * 4     9/08/99 12:59p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 3     9/08/99 11:57a Wind
 * comments
 * 
 * 2     9/08/99 10:22a Wind
 * adpated to use of quoted file names
*/
// Revision 1.9  1998/09/16 17:25:54  Wim
// 22beta2
//
// Revision 1.8  1997/08/14 11:59:50  Wim
// Allow syntax PolygonMapFromSegment(segmap), without specifying a mask
//
// Revision 1.7  1997-08-07 14:39:05+02  Wim
// Intern domain now counts starting with 1, instead of using internal polygonnumbers
// which was confusing to users.
// Using domain initializes all polygons on iUNDEF.
//
// Revision 1.6  1997-08-04 17:41:42+02  Wim
// Improved use of masks
//
// Revision 1.5  1997-08-01 17:22:22+02  Wim
// Use proper counting when creating an internal domain
//
// Revision 1.4  1997-07-30 17:57:15+02  Wim
// When polygonizing fails it now gives an error message
//
// Revision 1.3  1997-07-30 17:32:08+02  Wim
// Split sSyntax() over two lines
//
// Revision 1.2  1997-07-29 16:59:31+02  Wim
// Use "Pol" as prefix for the internal domain
//
/* PolygonMapFromSegment
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  WK   14 Aug 97    1:59 pm
*/

#include "geos/operation/polygonize/Polygonizer.h"
#include "PolygonApplications\POLFRMSG.H"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Engine\Base\mask.h"
#include "Headers\Hs\polygon.hs"

IlwisObjectPtr * createPolygonMapFromSegment(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapFromSegment::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapFromSegment(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapFromSegment::sSyntax() {
  return "PolygonMapFromSegment(segmap,\"mask\"[,auto])\n"
"PolygonMapFromSegment(segmap,\"mask\",unique|domain|labelpntmap[,auto])";
}

PolygonMapFromSegment* PolygonMapFromSegment::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms < 1)
		throw ErrorExpression(sExpr, sSyntax());
	
	// Disable use of attribute maps. A SegmentMapAttribute has no method to access the segments
	// via the MapVirtual (yet)
	String sInputMapName = as[0];
	char *pCh = sInputMapName.strrchrQuoted('.');
	if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
		throw ErrorObject(WhatError(String(TR("Use of attribute maps is not possible: '%S'").c_str(), as[0]), errPolygonMapFromSegment), fn);
	
	SegmentMap smp(as[0], fn.sPath());
	String sMask;
	if (iParms > 1)
		sMask = as[1];
	bool fAuto = false;
	if (iParms == 3)
		if (fCIStrEqual(as[2], "auto"))
		{
			iParms--;
			fAuto = true;
		}
		if (iParms == 4)
			if (fCIStrEqual(as[3], "auto"))
			{
				iParms--;
				fAuto = true;
			}
			if (iParms > 3)
				throw ErrorExpression(sExpr, sSyntax());

			if (3 == iParms) 
				if (fCIStrEqual(as[2], "unique"))
				{
					iParms--;
				}
				if (3 == iParms)
				{
					FileName fn2(as[2]);
					if (".mpp" == fn2.sExt)
					{
						PointMap pm(as[2], fn.sPath());
						return new PolygonMapFromSegment(fn, p, smp, sMask, pm, fAuto);
					}
					else if (".dom" == fn2.sExt)
					{
						Domain dm(as[2], fn.sPath());
						return new PolygonMapFromSegment(fn, p, smp, sMask, dm, fAuto);
					}
					else
						throw ErrorExpression(sExpr, sSyntax());
				}
				return new PolygonMapFromSegment(fn, p, smp, sMask, fAuto);
}

PolygonMapFromSegment::PolygonMapFromSegment(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  Init();
  String sColName;
  try {
    ReadElement("PolygonMapFromSegment", "SegmentMap", smap);
    sMask = "*";
    ReadElement("PolygonMapFromSegment", "Mask", sMask);
    ReadElement("PolygonMapFromSegment", "AutoCorrect", fAutoCorrect);
    try {
      ReadElement("PolygonMapFromSegment", "Labels", pmapLbl);
    }
    catch (const ErrorObject& ) {
    }
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  objdep.Add(smap.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

PolygonMapFromSegment::PolygonMapFromSegment(
     const FileName& fn, PolygonMapPtr& p,
     const SegmentMap& sm, const String& sMsk, bool fAuto)
: PolygonMapVirtual(fn, p, sm->cs(),sm->cb(),
                    Domain(fn, sm->iFeatures(), dmtUNIQUEID, "Pol")),
  smap(sm), sMask(sMsk), fAutoCorrect(fAuto)
{
  Init();
  objdep.Add(smap.ptr());
  Store();
}

PolygonMapFromSegment::PolygonMapFromSegment(
     const FileName& fn, PolygonMapPtr& p,
     const SegmentMap& sm, const String& sMsk, const Domain& dm, bool fAuto)
: PolygonMapVirtual(fn, p, sm->cs(),sm->cb(), dm),
  smap(sm), sMask(sMsk), fAutoCorrect(fAuto)
{
  Init();
  objdep.Add(smap.ptr());
  Store();
}

PolygonMapFromSegment::PolygonMapFromSegment(
     const FileName& fn, PolygonMapPtr& p,
     const SegmentMap& sm, const String& sMsk, const PointMap& pmLbl, bool fAuto)
: PolygonMapVirtual(fn, p, sm->cs(),sm->cb(), pmLbl->dvrs()),
  smap(sm), sMask(sMsk), pmapLbl(pmLbl), fAutoCorrect(fAuto)
{
  Init();
  objdep.Add(smap.ptr());
  objdep.Add(pmapLbl.ptr());
  Store();
}

void PolygonMapFromSegment::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapFromSegment");
  WriteElement("PolygonMapFromSegment", "SegmentMap", smap);
  WriteElement("PolygonMapFromSegment", "Mask", sMask);
  WriteElement("PolygonMapFromSegment", "AutoCorrect", fAutoCorrect);
  if (pmapLbl.fValid())
    WriteElement("PolygonMapFromSegment", "Labels", pmapLbl);
  else
    WriteElement("PolygonMapFromSegment", "Labels", (char*)0);
}

PolygonMapFromSegment::~PolygonMapFromSegment()
{
}

String PolygonMapFromSegment::sExpression() const
{
  String s1, s2, s3;
  s1 = String("PolygonMapFromSegment(%S,\"%S\"",
              smap->sNameQuoted(false, fnObj.sPath()), sMask);
  if (pmapLbl.fValid())
    s2 = String(",%S",
                pmapLbl->sNameQuoted(true, fnObj.sPath()));
  else if (dm()->fnObj == fnObj)
    s2 = "";
  else
    s2 = String(",%S",
                dm()->sNameQuoted(true, fnObj.sPath()));
  s3 = fAutoCorrect ? ",auto)" : ")";
  return String("%S%S%S", s1, s2, s3);
}

bool PolygonMapFromSegment::fDomainChangeable() const
{
  if (pmapLbl.fValid())
    return false;
  else
    return true;
}

void PolygonMapFromSegment::Init()
{
  fNeedFreeze = true;
  fSegmentsChecked = false;
  htpFreeze = "ilwisapp\\segments_to_polygons_algorithm.htm";
  sFreezeTitle = "PolygonMapFromSegment";
}

bool PolygonMapFromSegment::fFreezing()
{
	Mask mask(smap->dm(), sMask);
	bool fMask = sMask.length() > 0;

	CoordBuf crdBuf;
	long iSeg = smap->iFeatures();
	trq.SetTitle(sFreezeTitle);
	trq.SetText(TR("Copy Segments"));
	geos::operation::polygonize::Polygonizer polygonizer;
	for (long i=0; i < smap->iFeatures(); ++i) {
		const ILWIS::Segment *seg = (ILWIS::Segment *)smap->getFeature(i);
		if ( !seg || !seg->fValid())
			continue;
		if (trq.fUpdate(i, iSeg))
			return false;
		if (fMask && !seg->fInMask(dvrs(),mask))
			continue;
		//seg->GetCoords(iNr, crdBuf,true);
		//LinearRing *ring = new LinearRing(seg->getCoordinates()->clone(), new GeometryFactory());
		polygonizer.add((Geometry *)seg);
	}
	vector<geos::geom::Polygon *> *polygons = polygonizer.getPolygons();
	// fouten afhandleing, maar eerst testen;
	for(long i = 0; i < polygons->size(); ++i) {
		geos::geom::Polygon *gpol = polygons->at(i);
		ILWIS::Polygon *pol = CPOLYGON(pms->newFeature(gpol));
		if (pmapLbl.fValid()) {
			vector<Feature *> points = pmapLbl->getFeatures(pol->cbBounds());
			for (vector<Feature *>::iterator pntit = points.begin(); pntit != points.end(); ++pntit) {
				ILWIS::Point * pnt = CPOINT(*pntit);
				if (pol->contains(pnt)) {
					if (dvrs().fRealValues()) {
						double rV = pnt->rValue();
						pol->PutVal(rV);
					} else{
						long iRaw = pnt->iValue();
						pol->PutVal(iRaw);
					}
					break;
				}
			}
		} else
			pol->PutVal(dm()->pdsrt()->iKey(i+1));
	}

	return true;
}






