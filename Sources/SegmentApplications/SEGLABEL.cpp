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
// $Log: /ILWIS 3.0/SegmentMap/SEGLABEL.cpp $
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
// Revision 1.2  1997/09/22 14:26:44  Wim
// Default in create() is now "Yes"
// (why "no" option exists is still unclear to me)
//
/* SegmentMapLabels
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   22 Sep 97    4:26 pm
*/                                                                      

#include "SegmentApplications\SEGLABEL.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapLabels(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapLabels::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapLabels(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapLabels::sSyntax() {
  return "SegmentMapLabels(segmap,pointmap,setdomain)";
}

SegmentMapLabels* SegmentMapLabels::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 2) || (iParms > 3))
    throw ErrorExpression(sExpr, sSyntax());
	String sInputSegMapName = as[0];
	char *pCh = sInputSegMapName.strrchrQuoted('.');
  if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
		throw ErrorObject(WhatError(String(TR("Use of attribute maps is not possible: '%S'").c_str(), as[0]),
																 errSegmentMapLabels), fn);
  SegmentMap smp(as[0], fn.sPath());
  PointMap pmp(as[1], fn.sPath());
  bool fSetDom = true;
  if (iParms == 3)
    fSetDom = as[2].fVal();
  return new SegmentMapLabels(fn, p, smp, pmp, fSetDom);
}

SegmentMapLabels::SegmentMapLabels(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("SegmentMapLabels", "SegmentMap", smp);
    ReadElement("SegmentMapLabels", "PointMap", pmp);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  ReadElement("SegmentMapLabels", "SetDomain", fSetDomain);
  Init();
  objdep.Add(smp.ptr());
  objdep.Add(pmp.ptr());
}

SegmentMapLabels::SegmentMapLabels(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm, 
                                   const PointMap& pm, bool fSetDom)
: SegmentMapVirtual(fn, p, sm->cs(),sm->cb(),sm->dvrs()), smp(sm), pmp(pm), fSetDomain(fSetDom)
{
  fNeedFreeze = true;
  if (!cs()->fConvertFrom(pmp->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), errPolygonMapLabels);
  if (fSetDomain)
    SetDomainValueRangeStruct(pmp->dvrs());
  Init();
  objdep.Add(smp.ptr());
  objdep.Add(pmp.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if (!fSetDomain)
    if (smp->fTblAttSelf())
      SetAttributeTable(smp->tblAtt());
}

void SegmentMapLabels::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapLabels");
  WriteElement("SegmentMapLabels", "SegmentMap", smp);
  WriteElement("SegmentMapLabels", "PointMap", pmp);
  WriteElement("SegmentMapLabels", "SetDomain", fSetDomain);
}

SegmentMapLabels::~SegmentMapLabels()
{
}

String SegmentMapLabels::sExpression() const
{
  return String("SegmentMapLabels(%S,%S,%s)", smp->sNameQuoted(false, fnObj.sPath()), 
                                              pmp->sNameQuoted(false, fnObj.sPath()), pcYesNo(fSetDomain));
}

bool SegmentMapLabels::fDomainChangeable() const
{
  return false;
}

void SegmentMapLabels::Init()
{
  htpFreeze = "ilwisapp\\assign_labels_to_segments_algorithm.htm";
  sFreezeTitle = "SegmentMapLabels";
}

bool SegmentMapLabels::fFreezing()
{
  bool fUseReal = st() == stREAL;
  trq.SetText(String(TR("Initializing '%S'").c_str(), sName(true, fnObj.sPath())));
  long iSeg = smp->iFeatures();
  long iSegNr = 1;
  CoordBuf crdBuf;
 for (int i=0; i < iSeg; ++i )  {
	  ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
	  if ( seg == NULL || !seg->fValid())
		  continue;
    if (trq.fUpdate(i, iSeg))
      return false;
    ILWIS::Segment *segNew = CSEGMENT(pms->newFeature());
    CoordinateSequence *seq = seg->getCoordinates();
	segNew->PutCoords(seq);
    if (fSetDomain)  // initialize with undef
      segNew->PutVal(iUNDEF);
    else
      segNew->PutVal(seg->rValue());
  }
  trq.SetText(String(TR("Labeling '%S'").c_str(), sName(true, fnObj.sPath())));
  long iPnt = pmp->iFeatures();
  CoordSystem csOld = pmp->cs();
  bool fTransformCoords = cs() != csOld;
  for (long i=0; i < iPnt; i++) {
    if (trq.fUpdate(i, iPnt))
      return false;
    Coord crd = pmp->cValue(i);
    if (fTransformCoords)
       crd = cs()->cConv(csOld, crd);
    // find segment with point crd
	long index=iUNDEF;
    ILWIS::Segment *seg = pms->segFirst(index);
    long dummy;
    pms->crdPoint(crd,&seg,dummy,rUNDEF,false);
    if (seg && seg->fValid()) {
      if (fUseReal)
        seg->PutVal(pmp->rValue(i));
      else
		seg->PutVal(pmp->iRaw(i));
    }  
  }
  trq.fUpdate(iPnt, iPnt);
  return true;
}




