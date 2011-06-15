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
/* SegmentMapDensifyCoords
   Copyright Ilwis System Development ITC
   august 1996, Jan Hendrikse
    Last change:  JEL   6 May 97    6:02 pm
*/

#include "SegmentApplications\SEGDENSF.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapDensifyCoords(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapDensifyCoords::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapDensifyCoords(fn, (SegmentMapPtr &)ptr);
}

#define EPS10 1.e-10

const char* SegmentMapDensifyCoords::sSyntax() {
  return "SegmentMapDensifyCoords(segmap,distance)";
}

SegmentMapDensifyCoords* SegmentMapDensifyCoords::create(const FileName& fn, SegmentMapPtr& p,
                                                 const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
	String sInputSegMapName = as[0];
	char *pCh = sInputSegMapName.strrchrQuoted('.');
  if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
		throw ErrorObject(WhatError(String(SSEGErrNoAttColumnAllowed_S.scVal(), as[0]),
																 errSegmentMapDensifyCoords), fn);
  SegmentMap smp(as[0], fn.sPath());
  double rDistance = as[1].rVal();
  return new SegmentMapDensifyCoords(fn, p, smp, rDistance);
}

SegmentMapDensifyCoords::SegmentMapDensifyCoords(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = false;
  String sColName;
  ReadElement("SegmentMapDensifyCoords", "SegmentMap", smp);
  ReadElement("SegmentMapDensifyCoords", "Distance", rDistance);
  Init();
  objdep.Add(smp.ptr());
}

SegmentMapDensifyCoords::SegmentMapDensifyCoords(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm,
                                     double rDist)
: SegmentMapVirtual(fn, p, sm->cs(),sm->cb(),sm->dvrs()), smp(sm),
  rDistance(rDist)
{
  if (rDistance < EPS10)
    throw ErrorObject(WhatError(SSEGErrCoordDistNotPositive, errSegmentMapDensifyCoords+1), sTypeName());
  fNeedFreeze = true;
  Init();
  objdep.Add(smp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  if (smp->fTblAttSelf())
    SetAttributeTable(smp->tblAtt());
}

void SegmentMapDensifyCoords::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapDensifyCoords");
  WriteElement("SegmentMapDensifyCoords", "SegmentMap", smp);
  WriteElement("SegmentMapDensifyCoords", "Distance", rDistance);
}

SegmentMapDensifyCoords::~SegmentMapDensifyCoords()
{
}

String SegmentMapDensifyCoords::sExpression() const
{
  return String("SegmentMapDensifyCoords(%S,%g)", smp->sNameQuoted(true, fnObj.sPath()),
                rDistance);
}

bool SegmentMapDensifyCoords::fDomainChangeable() const
{
  return false;
}

bool SegmentMapDensifyCoords::fValueRangeChangeable() const
{
  return false;
}

void SegmentMapDensifyCoords::Init()
{
  htpFreeze = "ilwisapp\\densify_segment_coordinates_functionality_algorithm.htm";
  sFreezeTitle = "SegmentMapDensifyCoords";
}

bool SegmentMapDensifyCoords::fFreezing()
{
  Init();
  bool fUseRaw = !dvrs().fUseReals();                         
  ILWIS::Segment *segOut;
  long iCrdOut;
  trq.SetText(SSEGTextDensifyCoordinates);
 for (int i=0; i < smp->iFeatures(); ++i )  {
	  ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
	  if ( seg == NULL || !seg->fValid())
		  continue;
    if ( trq.fUpdate((i), smp->iFeatures()) )
      return false;
    CoordBuf crdBuf;
    seg->Densify(rDistance,crdBuf);                                      
    iCrdOut = crdBuf.iSize();
    segOut = CSEGMENT(pms->newFeature());          
	segOut->PutCoords(crdBuf.clone());
    segOut->PutVal(seg->rValue());   // take over the (numeric) val of orig segment
  }// end for seg.fValid
  return true;            
}




