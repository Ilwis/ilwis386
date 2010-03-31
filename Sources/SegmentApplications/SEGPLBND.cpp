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
/* $Log: /ILWIS 3.0/SegmentMap/SEGPLBND.cpp $
 * 
 * 12    19/02/01 10:50 Willem
 * Added error message: attribute column in input map is not possible
 * 
 * 11    29-11-00 12:09 Koolhoven
 * option in commandline is single|unique instead of single|composite
 * 
 * 10    29-11-00 12:04 Koolhoven
 * the fFreezing() made in case of single name erroneously an attribute
 * table with columns, now prevented
 * 
 * 9     24-11-00 19:08 Koolhoven
 * In case of not single name operation now uses a Domain Unique ID and an
 * attribute table with 3 columns: pol1, pol2 and Length
 * 
 * 8     1-03-00 17:21 Wind
 * support of long domain names
 * 
 * 7     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 5     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 4     15-11-99 12:47 Wind
 * solved bug 1280 (no internal domain option)
 * 
 * 3     9/08/99 10:26a Wind
 * adpated to use of quoted file names in sExpression()
 * 
 * 2     3/11/99 12:17p Martin
 * Added support for Case insesitive 
// Revision 1.9  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.8  1997/09/01 17:48:25  Wim
// Make internal domain Class if input map has domain Class,
// otherwise make an ID domain.
//
// Revision 1.7  1997-09-01 10:55:23+02  martin
// Domain of mask set correctly.
//
// Revision 1.6  1997/08/04 15:36:21  Wim
// Improved use of mask. Use now Segment::fInMask(mask)
//
// Revision 1.5  1997-07-30 10:26:10+02  Wim
// Init domain only in fFreezing() function otherwise no proper storing.
//
// Revision 1.4  1997-07-30 10:09:10+02  Wim
// Add "Segments" to domain in case of single name
//
// Revision 1.3  1997-07-29 18:22:31+02  Wim
// fSingleName to distinguish between "single" or "composite" naming
// of the segments.
//
// Revision 1.2  1997-07-29 16:49:11+02  Wim
// Internal domain now has only one item: "Segments"
//
/* SegmentMapPolBoundaries
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    1 Sep 97    7:47 pm
*/                                                                      

#include "SegmentApplications\SEGPLBND.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapPolBoundaries(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapPolBoundaries::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapPolBoundaries(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapPolBoundaries::sSyntax() {
  return "SegmentMapPolBoundaries(polmap,\"mask\",single|unique)\n";
}

SegmentMapPolBoundaries* SegmentMapPolBoundaries::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if ((iParms < 3) || (iParms > 4))
		ExpressionError(sExpr, sSyntax());
	
	// Disable use of attribute maps. A PolygonMapAttribute has no method to access the topologies
	// via the MapVirtual (yet)
	String sInputMapName = as[0];
	char *pCh = sInputMapName.strrchrQuoted('.');
	if ((pCh != 0) && (0 != _strcmpi(pCh, ".mpa")))  // attrib map
		throw ErrorObject(WhatError(String(SSEGErrNoAttColumnAllowed_S.scVal(), as[0]), errSegmentMapPolBoundaries), fn);
	
	String sNaming = as[2];
	bool fSingle;
	if (fCIStrEqual("single" , sNaming))
		fSingle = true;
	else if (fCIStrEqual("unique" , sNaming) || fCIStrEqual("composite" , sNaming))
		fSingle = false;
	else
		ExpressionError(sExpr, sSyntax());
	
	PolygonMap pmp(as[0], fn.sPath());
	return new SegmentMapPolBoundaries(fn, p, pmp, as[1], fSingle);
}

SegmentMapPolBoundaries::SegmentMapPolBoundaries(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("SegmentMapPolBoundaries", "PolygonMap", pmp);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  String sMask;
  ReadElement("SegmentMapPolBoundaries", "Mask", sMask);
  mask.SetMask(sMask);
  String sNaming;
  ReadElement("SegmentMapPolBoundaries", "Naming", sNaming);
  fSingleName = (fCIStrEqual("single" , sNaming));
  Init();
  objdep.Add(pmp.ptr());
}

SegmentMapPolBoundaries::SegmentMapPolBoundaries(const FileName& fn, SegmentMapPtr& p, const PolygonMap& pm, 
                               const String& sMask, bool fSingle)
: SegmentMapVirtual(fn, p, pm->cs(),pm->cb(),pm->dvrs()),
  pmp(pm), fSingleName(fSingle)
{
  DomainType dmt = dmtUNIQUEID;
	Domain dom(fnObj, 0, dmt, "Boundary");
  SetDomainValueRangeStruct(dom);
  mask.SetMask(sMask);
  fNeedFreeze = true;
  Init();
  objdep.Add(pmp.ptr());
    
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
}

void SegmentMapPolBoundaries::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapPolBoundaries");
  WriteElement("SegmentMapPolBoundaries", "PolygonMap", pmp);
  WriteElement("SegmentMapPolBoundaries", "Mask", mask.sMask());
  WriteElement("SegmentMapPolBoundaries", "Naming", "unique");
}

SegmentMapPolBoundaries::~SegmentMapPolBoundaries()
{
}

String SegmentMapPolBoundaries::sExpression() const
{
  return String("SegmentMapPolBoundaries(%S,\"%S\",%s)",
    pmp->sNameQuoted(false, fnObj.sPath()), mask.sMask(),
    "unique");
}

bool SegmentMapPolBoundaries::fDomainChangeable() const
{
  return false;
}

void SegmentMapPolBoundaries::Init()
{
  htpFreeze = htpSegmentMapPolBoundariesT;
  sFreezeTitle = "SegmentMapPolBoundaries";
}

bool SegmentMapPolBoundaries::fFreezing()
{
	DomainSort* pdsrt = dm()->pdsrt();
	if (fSingleName) {
	  pdsrt->Resize(0); // clean up domain
	  pdsrt->iAdd("Boundary");
	}
	else
		pdsrt->Resize( pmp->iFeatures()); 

	for(int i=0; i < pmp->iFeatures(); ++i) {
		ILWIS::Polygon *pol = (ILWIS::Polygon *)pmp->getFeature(i);
		if (!pol || pol->fValid()==false)
			continue;
		if (trq.fUpdate(i, pmp->iFeatures()))
			return false;
		const Geometry *geom = pol->getBoundary();
		for(int j = 0; j < geom->getNumGeometries(); ++j) {
			const Geometry *g = geom->getGeometryN(j);
			ILWIS::Segment *seg = CSEGMENT(pms->newFeature());
			CoordinateSequence *seq = g->getCoordinates();
			seg->PutCoords(seq);
			seg->PutVal(fSingleName ? 1L : i);
		}
	}
 // trq.SetText(SSEGTextExtractSegments);
 // mask.SetDomain(pmp->dm());
 // SegmentMap smp = pmp->smp();

 // long iSeg = smp->iSeg();
	//bool fRaw = pmp->fRawAvailable();
 // 
 // DomainSort* pdsrt = dm()->pdsrt();
 // if (fSingleName) {
	//  pdsrt->Resize(0); // clean up domain
	//	pdsrt->iAdd("Boundary");
	//}
	//else
	//	pdsrt->Resize(iSeg);  // could only be less

	//Table tbl;
	//Column col1, col2, colLength;
	//if (!fSingleName) {
	//	tbl = tblAtt();
	//	if (!tbl.fValid()) {
	//		FileName fnAtt(fnObj, ".tbt", true);
 // 		tbl = Table(fnAtt, dm());
	//		SetAttributeTable(tbl);
	//	}
	//	else
	//		tbl->CheckNrRecs();
	//	col1 = tbl->col("Pol1");
	//	if (!col1.fValid())
	//		col1 = Column(tbl, "Pol1", pmp->dvrs());
	//	col2 = tbl->col("Pol2");
	//	if (!col2.fValid())
	//		col2 = Column(tbl, "Pol2", pmp->dvrs());
	//	colLength = tbl->col("Length");
	//	if (!colLength.fValid())
	//		colLength = tbl->colNew("Length",DomainValueRangeStruct(0, 1e307, 0.001));	
	//}

 // Segment seg = smp->segFirst();
	//int iNrNew = 0;

 // for(long i=0; i<iSeg; ++i, ++seg)
 // {   
 //   if (!seg.fValid()) 
	//		continue;
 //   if (trq.fUpdate(i, iSeg)) 
	//		return false;
 //   Topology top(pmp, seg.iCurr());
 //   if (!top.fValid()) 
	//		continue;
 //   const Polygon polLeft = top.polLeft();
 //   const Polygon polRight = top.polRight();
 //   if (!fSelectSegment(polLeft, polRight, mask)) 
	//		continue;

 //   long iCrd=0;
 //   CoordBuf crdBuf(1000);

 //   Segment segNew = pms->newFeature();
 //   top.GetCoords(iCrd, crdBuf);
 //   segNew.PutCoords(iCrd, crdBuf);
	//	if (fSingleName)
	//		segNew.PutRaw(1);
	//	else {
	//		segNew.PutRaw(++iNrNew);
	//		if (fRaw) {
	//			col1->PutRaw(iNrNew, polLeft.iRaw());
	//			col2->PutRaw(iNrNew, polRight.iRaw());
	//		}
	//		else {
	//			col1->PutVal(iNrNew, polLeft.rValue());
	//			col2->PutVal(iNrNew, polRight.rValue());
	//		}
	//		colLength->PutVal(iNrNew, segNew.rLength());
	//	}
 // }
 // trq.fUpdate(iSeg, iSeg);

	//if (!fSingleName) {
	//	pdsrt->Resize(iNrNew);
	//	col1->SetOwnedByTable(true);
	//	col1->SetReadOnly(true);
	//	col2->SetOwnedByTable(true);
	//	col2->SetReadOnly(true);
	//	colLength->SetOwnedByTable(true);
	//	colLength->SetReadOnly(true);
	//	tbl->Updated();
	//}
  return true;
}






