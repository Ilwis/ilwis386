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
/* SegmentMapSubMap
   Copyright Ilwis System Development ITC
   aug 1996, Jan Hendrikse
    Last change:  JEL  27 May 97   11:29 am
*/

#include "Engine\Base\Algorithm\Clipline.h"
#include "SegmentApplications\SEGSUBMP.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Engine\Base\DataObjects\ARRAY.H"
#include "Headers\Hs\Segment.hs"

IlwisObjectPtr * createSegmentMapSubMap(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapSubMap::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapSubMap(fn, (SegmentMapPtr &)ptr);

	return NULL;
}

const char* SegmentMapSubMap::sSyntax() {
  return "SegmentMapSubMap(segmap,crdminx,crdminy,crdmaxx,crdmaxy)";
}

SegmentMapSubMap* SegmentMapSubMap::create(const FileName& fn, SegmentMapPtr& p,
                                                 const String& sExpr)
{
  Array<String> as(5);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
	String sInputSegMapName = as[0];
	char *pCh = sInputSegMapName.strrchrQuoted('.');
  if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
		throw ErrorObject(WhatError(String(SSEGErrNoAttColumnAllowed_S.scVal(), as[0]),
																 errSegmentMapSubMap), fn);
  SegmentMap smp(as[0], fn.sPath());
  CoordBounds cb(Coord(as[1].rVal(), as[2].rVal()), Coord(as[3].rVal(), as[4].rVal()));
  return new SegmentMapSubMap(fn, p, smp, cb);
}

SegmentMapSubMap::SegmentMapSubMap(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = false;
  String sColName;
  ReadElement("SegmentMapSubMap", "SegmentMap", smp);
  Init();
  objdep.Add(smp.ptr());
}

SegmentMapSubMap::SegmentMapSubMap(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm,
                                   const CoordBounds& cb)
: SegmentMapVirtual(fn, p, sm->cs(),cb,sm->dvrs()), smp(sm)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(smp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  if (smp->fTblAttSelf())
    SetAttributeTable(smp->tblAtt());
}

void SegmentMapSubMap::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapSubMap");
  WriteElement("SegmentMapSubMap", "SegmentMap", smp);
}

SegmentMapSubMap::~SegmentMapSubMap()
{
}

String SegmentMapSubMap::sExpression() const
{
  if ( 0 != smp->cs()->pcsLatLon())  
	return String("SegmentMapSubMap(%S,%.7f,%.7f,%.7f,%.7f)", smp->sNameQuoted(false, fnObj.sPath()),
                                    cb().MinX(), cb().MinY(), cb().MaxX(), cb().MaxY());
  else
	return String("SegmentMapSubMap(%S,%.3f,%.3f,%.3f,%.3f)", smp->sNameQuoted(false, fnObj.sPath()),
                                    cb().MinX(), cb().MinY(), cb().MaxX(), cb().MaxY());
	return "";
}

bool SegmentMapSubMap::fDomainChangeable() const
{
  return false;
}

bool SegmentMapSubMap::fValueRangeChangeable() const
{
  return false;
}

void SegmentMapSubMap::Init()
{
  htpFreeze = "ilwisapp\\submap_of_segment_map_algorithm.htm";
  sFreezeTitle = "SegmentMapSubMap";
}


bool SegmentMapSubMap::fFreezing()
{
  Init();
  bool fUseRaw = !fUseReals();
  trq.SetText(SSEGTextCalculating);
  ILWIS::Segment *segOut;
  long  iCrdOut;
  CoordBounds cbClip = cb();
  Array<CoordBuf> acrdBuf;
  
  for( int i =0; i< smp->iFeatures(); ++i) {
	  ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
	  if ( !seg || !seg->fValid())
		  continue;
	  if ( (trq.fUpdate(i, smp->iFeatures())) )
		  return false;

	  seg->Clip(cbClip, acrdBuf);
	  for ( unsigned long i = 0; i < acrdBuf.iSize(); i++ ) // treat all buffers in array
	  {                                            // from nr 0 to nr iSize-1
		  iCrdOut = acrdBuf[i].iSize();
		  segOut = CSEGMENT(pms->newFeature());          // make a new segm in new seg map
		  segOut->PutCoords(iCrdOut, acrdBuf[i]);
		  segOut->PutVal(seg->rValue());   // take over the (numeric) val of orig Segment
	  }
  }
  return true;            // freezing returns true if no errors occurred
}




