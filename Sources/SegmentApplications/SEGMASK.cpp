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
/* SegmentMapMask
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    5:19 pm
*/                                                                      

#include "SegmentApplications\SEGMASK.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapMask(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapMask::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapMask(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapMask::sSyntax() {
  return "SegmentMapMask(segmap,\"mask\")";
}

SegmentMapMask* SegmentMapMask::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
	String sInputSegMapName = as[0];
	char *pCh = sInputSegMapName.strrchrQuoted('.');
  if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
		throw ErrorObject(WhatError(String(TR("Use of attribute maps is not possible: '%S'").c_str(), as[0]),
																 errSegmentMapMask), fn);
  SegmentMap smp(as[0], fn.sPath());
  return new SegmentMapMask(fn, p, smp, as[1]);
}

SegmentMapMask::SegmentMapMask(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("SegmentMapMask", "SegmentMap", smp);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  String sMask;
  ReadElement("SegmentMapMask", "Mask", sMask);
  mask.SetMask(sMask);
  Init();
  objdep.Add(smp.ptr());
}

SegmentMapMask::SegmentMapMask(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm, 
                               const String& sMask)
: SegmentMapVirtual(fn, p, sm->cs(),sm->cb(),sm->dvrs()), smp(sm)//, asMask(asMsk)
{
  mask.SetMask(sMask);
  fNeedFreeze = true;
  Init();
  objdep.Add(smp.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if (smp->fTblAttSelf())
    SetAttributeTable(smp->tblAtt());
}

void SegmentMapMask::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapMask");
  WriteElement("SegmentMapMask", "SegmentMap", smp);
  WriteElement("SegmentMapMask", "Mask", mask.sMask());
}

SegmentMapMask::~SegmentMapMask()
{
}

String SegmentMapMask::sExpression() const
{
  return String("SegmentMapMask(%S,\"%S\")", smp->sNameQuoted(false, fnObj.sPath()), mask.sMask());
}

bool SegmentMapMask::fDomainChangeable() const
{
  return false;
}

void SegmentMapMask::Init()
{
  htpFreeze = "ilwisapp\\mask_segments_algorithm.htm";
  sFreezeTitle = "SegmentMapMask";
}

bool SegmentMapMask::fFreezing()
{
  trq.SetText(String(TR("Copy with mask '%S'").c_str(), sName(true, fnObj.sPath())));
  long iSeg = smp->iFeatures();
  long iSegNr = 1;
  CoordBuf crdBuf;
  mask.SetDomain(dm());
  for (int i=0; i < smp->iFeatures(); ++i )  {
	  ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
	  if ( seg == NULL || !seg->fValid())
		  continue;
    if (trq.fUpdate(i, iSeg))
      return false;
	if (!seg->fInMask(smp->dvrs(),mask))
      continue;
    ILWIS::Segment *segNew = CSEGMENT(pms->newFeature());
    CoordinateSequence *seq = seg->getCoordinates();
    segNew->PutCoords(seq);
    segNew->PutVal(seg->rValue());
  }
  trq.fUpdate(iSeg, iSeg);
  return true;
}




