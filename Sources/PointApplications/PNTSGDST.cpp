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
/* PointMapSegDist
   Copyright Ilwis System Development ITC
	jan 1996, by Jan Hendrikse
	Last change:  JEL   6 May 97    5:54 pm
*/


#include "PointApplications\PNTSGDST.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\Dmvalue.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\point.hs"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createPointMapSeg(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapSegDist::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapSegDist(fn, (PointMapPtr &)ptr);
}

const char* PointMapSegDist::sSyntaxSegDist() 
  { return "PointMapSegDist(segmap,distance)"; }

const char* PointMapSegDist::sSyntaxSegCoords() 
  { return "PointMapSegCoords(segmap)"; }

const char* PointMapSegDist::sSyntaxSegNodes() 
  { return "PointMapSegNodes(segmap)"; }

const char* PointMapSegDist::sSyntaxSegCoordsOrNodes() 
  { return "PointMapSegCoords(segmap)|PointMapSegNodes(segmap)"; }

PointMapSegDist* PointMapSegDist::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  double rInterval;
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
  if (fCIStrEqual(sFunc, "PointMapSegDist")) {
    Array<String> as(2);
    if (!IlwisObjectPtr::fParseParm(sExpr, as))
      throw ErrorExpression(sExpr, sSyntaxSegDist());
		String sInputSegMapName = as[0];
		char *pCh = sInputSegMapName.strrchrQuoted('.');
		if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
			throw ErrorObject(WhatError(String(SSEGErrNoAttColumnAllowed_S.scVal(), as[0]),
																 errPointMapSegCoords), fn);
    rInterval = as[1].rVal();
    if (rInterval <= 0)
      throw ErrorObject(WhatError(SPNTErrDistanceMustBePositive, errPointMapSegDist), fn);
    SegmentMap smp(as[0], fn.sPath());
    return new PointMapSegDist(fn, p, smp, rInterval);
  }
  else {
		Array<String> as(1);
		if (!IlwisObjectPtr::fParseParm(sExpr, as)) 
			ExpressionError(sExpr, sSyntaxSegCoordsOrNodes());
		String sInputSegMapName = as[0];
		char *pCh = sInputSegMapName.strrchrQuoted('.');
		if (fCIStrEqual(sFunc, "PointMapSegCoords")) {
			if (!IlwisObjectPtr::fParseParm(sExpr, as))
				throw ErrorExpression(sExpr, sSyntaxSegCoords());
			if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
				throw ErrorObject(WhatError(String(SSEGErrNoAttColumnAllowed_S.scVal(), as[0]),
																	 errPointMapSegCoords), fn);
			SegmentMap smp(as[0], fn.sPath());
			return new PointMapSegDist(fn, p, smp, 0);
		}
		else if (fCIStrEqual(sFunc, "PointMapSegNodes")) {
			if (!IlwisObjectPtr::fParseParm(sExpr, as))
				throw ErrorExpression(sExpr, sSyntaxSegNodes());
			if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
				throw ErrorObject(WhatError(String(SSEGErrNoAttColumnAllowed_S.scVal(), as[0]),
																	 errPointMapSegNodes), fn);
			SegmentMap smp(as[0], fn.sPath());
			return new PointMapSegDist(fn, p, smp, rUNDEF);
		}
		else
			throw ErrorAppName(sExpr, fn.sFullName());
	}
  return NULL;
}

PointMapSegDist::PointMapSegDist(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  String sType;
  ReadElement("PointMapVirtual", "Type", sType);
  rInterval = 0;
  if (fCIStrEqual(sType, "PointMapSegDist"))
    ReadElement("PointMapSegDist", "Interval", rInterval);
  else if (fCIStrEqual(sType,"PointMapSegCoords"))
    rInterval = 0;
  else if (fCIStrEqual(sType, "PointMapSegNodes"))
    rInterval = rUNDEF;
  else
    throw ErrorInvalidType(fn, "PointMapVirtual", sType);
  ReadElement("PointMapSegDist", "SegmentMap", smp);
  fNeedFreeze = true;
  Init();
  objdep.Add(smp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

PointMapSegDist::~PointMapSegDist()
{
}

PointMapSegDist::PointMapSegDist(const FileName& fn, PointMapPtr& p,
                                 const SegmentMap& smap, double rInt)
: PointMapVirtual(fn, p, smap->cs(), smap->cb(), smap->dvrs()), smp(smap), rInterval(rInt)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(smp.ptr());
  if (smp->fTblAttSelf())
    SetAttributeTable(smp->tblAtt());
}

void PointMapSegDist::Store()
{
  PointMapVirtual::Store();
  if (rInterval > 0) {
    WriteElement("PointMapVirtual", "Type", "PointMapSegDist");
    WriteElement("PointMapSegDist", "Interval", rInterval);
  }
  else if (rInterval == 0)
    WriteElement("PointMapVirtual", "Type", "PointMapSegCoords");
  else
    WriteElement("PointMapVirtual", "Type", "PointMapSegNodes");
  WriteElement("PointMapSegDist", "SegmentMap", smp);
}

String PointMapSegDist::sExpression() const
{
  if (rInterval > 0)
    return String("PointMapSegDist(%S,%g)", smp->sNameQuoted(true, fnObj.sPath()), rInterval);
  else if (rInterval == 0)
    return String("PointMapSegCoords(%S)", smp->sNameQuoted(true, fnObj.sPath()));
  else  /// rInterval < 0
    return String("PointMapSegNodes(%S)", smp->sNameQuoted(true, fnObj.sPath()));
}

void PointMapSegDist::Init()
{
  if (rInterval > 0){
     htpFreeze = "ilwisapp\\segment_density.htm";
    sFreezeTitle = "PointMapSegDist";
  }
  else if (rInterval == 0) {
    htpFreeze = "ilwisapp\\segments_to_points.htm";
    sFreezeTitle = "PointMapSegCoords";
  }
  else {
    htpFreeze ="";
    sFreezeTitle = "PointMapSegNodes";
  }
}


bool PointMapSegDist::fFreezing()
{
  Init();
  trq.SetText(SPNTTextCalculating);
  long iCode,iNumbrLegs;
  CoordinateSequence *crdBuf;
  if (rInterval == 0 ) {   //  take all coordinates in segment map
    Coord crd;
	for (int i = 0; i < smp->iFeatures(); ++i){
		ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
		if(seg == NULL || !seg->fValid())
			continue;
      if (trq.fUpdate(i, smp->iFeatures()))
        return false;
      if (fUseReals()) {
        double rValue = seg->rValue();
        crdBuf = seg->getCoordinates();
		iNumbrLegs = crdBuf->size();
        for (long iLeg=0; iLeg < iNumbrLegs; iLeg++) 
				{
          crd = crdBuf->getAt(iLeg); 
          if ((iLeg == 0) || (iLeg == iNumbrLegs-1)) // begin or end point
            if (iUNDEF != pms->iRec(crd)) // check on exist already
              continue;
          pms->iAddVal(crd, rValue);
        }
		delete crdBuf;
      }
      else {
        iCode = seg->iValue();
         crdBuf = seg->getCoordinates();
		iNumbrLegs = crdBuf->size();
        for (long iLeg=0; iLeg < iNumbrLegs; iLeg++) {
			crd = crdBuf->getAt(iLeg); 
          if ((iLeg == 0) || (iLeg == iNumbrLegs-1)) // begin or end point
            if (iUNDEF != pms->iRec(crd)) // check on exist already
              continue;
          pms->iAddRaw(crd, iCode);
        }
		delete crdBuf;
      }
    }// end for seg->fValid
  }// end if (rInterval == 0 )

  else if (rInterval < 0 )  { // take only all nodes in segment map
    Coord crd;
    for (int i = 0; i < smp->iFeatures(); ++i){
		ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
		if(seg == NULL || !seg->fValid())
			continue;
      if (trq.fUpdate(i, smp->iFeatures()))
        return false;
      if (fUseReals()) {
        double rValue = seg->rValue();
				crd = seg->crdBegin();
        if (iUNDEF == pms->iRec(crd))
          pms->iAddVal(crd, rValue);
        crd = seg->crdEnd();
        if (iUNDEF == pms->iRec(crd))
          pms->iAddVal(crd, rValue);
      }
      else {
        iCode = seg->iValue();
        crd = seg->crdBegin();
        if (iUNDEF == pms->iRec(crd))
          pms->iAddRaw(crd, iCode);
        crd = seg->crdEnd();
        if (iUNDEF == pms->iRec(crd))
          pms->iAddRaw(crd, iCode);
      }
    }// end for seg->fValid
  }// end if (rInterval < 0 )

  else {         // points on segments at given interval spacing
    double dx,dy,d2,rCos, rSin, rDist;
    Coord crd;  CoordBuf crdBuf1(1000);
	CoordinateSequence *crdBuf2;
    for (int i = 0; i < smp->iFeatures(); ++i){
		ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
		if(seg == NULL || !seg->fValid())
			continue;
      if (trq.fUpdate(i,smp->iFeatures()))
        return false;
      if (fUseReals()) {
        double rValue = seg->rValue();
         crdBuf = seg->getCoordinates();
		iNumbrLegs = crdBuf->size();
        for (long iLeg=0; iLeg < iNumbrLegs; iLeg++)
          crdBuf1[iLeg]= crdBuf->getAt(iLeg);
        // store begin point ???????
        rDist = 0;
        for (long iLeg=1; iLeg < iNumbrLegs; iLeg++) {
          dx = crdBuf1[iLeg].x - crdBuf1[iLeg-1].x;
          dy = crdBuf1[iLeg].y - crdBuf1[iLeg-1].y;
          d2 = sqrt(dx*dx + dy*dy);
          if (d2 == 0)
            continue;
          rCos = dx/d2; rSin = dy/d2;
          rDist += d2;
          while (rDist > 0) {
            crd.x = crdBuf1[iLeg].x - rDist*rCos;
            crd.y = crdBuf1[iLeg].y - rDist*rSin;
            pms->iAddVal(crd, rValue);
            rDist -= rInterval;
          }// end while rDist > 0
        }// end for iLeg < iNumbrLegs
        // add last point
        crd = crdBuf1[iNumbrLegs-1];
        if (iUNDEF == pms->iRec(crd)) // check on exist of end point
          pms->iAddVal(crd, rValue);
      }// end if fUseReals
      else {
        iCode = seg->iValue();
        crdBuf2 = seg->getCoordinates();
		iNumbrLegs = crdBuf2->size();
        for (long iLeg=0; iLeg < iNumbrLegs; iLeg++)
          crdBuf1[iLeg]= crdBuf2->getAt(iLeg);
        // store begin point ???????
        rDist = 0;
        for (long iLeg=1; iLeg < iNumbrLegs; iLeg++) {
          dx = crdBuf1[iLeg].x - crdBuf1[iLeg-1].x;
          dy = crdBuf1[iLeg].y - crdBuf1[iLeg-1].y;
          d2 = sqrt(dx*dx + dy*dy) ; 
          if (d2 == 0)
            continue;
          rCos = dx/d2; rSin = dy/d2;          
          rDist += d2;
          while (rDist > 0) {
            crd.x = crdBuf1[iLeg].x - rDist*rCos;
            crd.y = crdBuf1[iLeg].y - rDist*rSin;
            pms->iAddRaw(crd, iCode);
            rDist -= rInterval;
          }// end while rDist > 0
        }// end for iLeg < iNumbrLegs
        // add last point
        crd = crdBuf1[iNumbrLegs-1];
        if (iUNDEF == pms->iRec(crd)) // check on exist of end point
          pms->iAddRaw(crd, iCode);
      }// end else (if not fUseReals)
    }//  end for seg->fValid
  }// end if (rInterval > 0)
  _iPoints = pms->iPnt();
  return true;
}




