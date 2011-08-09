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
/* SegmentMapAttribute
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  13 May 97   11:53 am
*/
#include "SegmentApplications\SEGATTRB.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapAttribute(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapAttribute::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else{
		if ( parms.size() > 0 ) {
			SegmentMap smp = *((SegmentMap *)parms[0]);
			String col = *((String *)parms[1]);
			return (IlwisObjectPtr *)new SegmentMapAttribute(fn, (SegmentMapPtr &)ptr, smp, col);
		}
		return (IlwisObjectPtr *)new SegmentMapAttribute(fn, (SegmentMapPtr &)ptr);
	}
}

class DATEXPORT ErrorInvalidAttDomain: public ErrorObject
{
public:
  ErrorInvalidAttDomain(const String& sDomain, const WhereError& where)
  : ErrorObject(WhatError(String(TR("Invalid domain type for attribute column: '%S'").c_str(), sDomain), 
                errSegmentMapAttribute+1), where) {}
};

const char* SegmentMapAttribute::sSyntax() {
  return "SegmentMapAttribute(segmap,attribcol)\nsegmap.attribcol";
}

SegmentMapAttribute* SegmentMapAttribute::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  SegmentMap smp(as[0], fn.sPath());
  return new SegmentMapAttribute(fn, p, smp, as[1]);
}

SegmentMapAttribute::SegmentMapAttribute(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("SegmentMapAttribute", "SegmentMap", smp);
    ReadElement("SegmentMapAttribute", "AttribTable", tblAttrb);
    if (!tblAttrb.fValid())
      tblAttrb = tblAtt();
    if (!tblAttrb.fValid()) {
      ErrorNoAttTable(fn  , errSegmentMapAttribute).Show(); // no throw to prevent invalid object
      return;
    }
    ReadElement("SegmentMapAttribute", "AttribColumn", sColName);
    colAtt = tblAttrb[sColName];
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  if (!colAtt.fValid()) {
    ErrorNotFound(WhereErrorColumn(tblAttrb->fnObj, sColName)).Show(); // no throw to prevent invalid object
    return;
  }
  Init();
  objdep.Add(smp.ptr());
  objdep.Add(colAtt.ptr());
}

SegmentMapAttribute::SegmentMapAttribute(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm, 
                                         const String& sColName)
: SegmentMapVirtual(fn, p, sm->cs(),sm->cb(),sm->dvrs()), smp(sm)
{
  fNeedFreeze = true;
  colAtt = Column(tblAttrb, sColName);
  if (colAtt.fValid())
    tblAttrb = Table(colAtt->fnTbl);
  else {
    tblAttrb = smp->tblAtt();
    if (!tblAttrb.fValid())
      throw ErrorNoAttTable(smp->sTypeName(), errSegmentMapAttribute);
    colAtt = tblAttrb[sColName];
    if (!colAtt.fValid())
      throw ErrorNotFound(WhereErrorColumn(tblAttrb->fnObj, sColName));
  }  
  SetDomainValueRangeStruct(colAtt->dvrs());
  if (!dvrs().fValues() && (0 == dm()->pdsrt()))
    throw ErrorInvalidAttDomain(dm()->sName(true, fnObj.sPath()), sTypeName());
  Init();
  objdep.Add(smp.ptr());
  objdep.Add(colAtt.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
}

void SegmentMapAttribute::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapAttribute");
  WriteElement("SegmentMapAttribute", "SegmentMap", smp);
  WriteElement("SegmentMapAttribute", "AttribTable", tblAttrb);
  WriteElement("SegmentMapAttribute", "AttribColumn", colAtt);
}

SegmentMapAttribute::~SegmentMapAttribute()
{
}

String SegmentMapAttribute::sExpression() const
{
  return String("%S.%S", smp->sNameQuoted(false, fnObj.sPath()), colAtt->sNameQuoted());
}

bool SegmentMapAttribute::fDomainChangeable() const
{
  return false;
}
/*
long SegmentMapAttribute::iFeatures() const
{
  return smp->iFeatures();
}
*/
void SegmentMapAttribute::Init()
{
  htpFreeze = "ilwisapp\attribute_map_of_segment_map_algorithm.htm";
  sFreezeTitle = "SegmentMapAttribute";
  DomainSort* pdsrt = colAtt->dmKey()->pdsrt();
  assert(pdsrt);
  long iSize = pdsrt->iSize();
  fFastAccess = iSize <= 1000;
  if (!fFastAccess)
    return;
/*  aiAttRaw.Resize(iSize, 1);
  aiAttVal.Resize(iSize, 1);
  arAttVal.Resize(iSize, 1);*/
  aiAttRaw.Resize(iSize); // now zero based
  aiAttVal.Resize(iSize); // now zero based
  arAttVal.Resize(iSize); // now zero based
  for (short i=1; i<= iSize ; i++ ) {
    aiAttRaw[i-1] = colAtt->iRaw(i);
    aiAttVal[i-1] = colAtt->iValue(i);
    arAttVal[i-1] = colAtt->rValue(i);
  }
}

bool SegmentMapAttribute::fFreezing()
{
  trq.SetText(String(TR("Calculating '%S'").c_str(), sName(true, fnObj.sPath())));
  bool fUseReals = dvrs().fUseReals();
  long iSeg = smp->iFeatures();
  for(int i = 0; i < iSeg; ++i) {
	ILWIS::Segment *seg= CSEGMENT(smp->getFeature(i)); 
	if ( !seg || seg->fValid() == false)
		continue;
    if (trq.fUpdate(i, iSeg))
      return false;
    ILWIS::Segment *segNew = CSEGMENT(pms->newFeature(seg));
	unsigned long iRaw = seg->iValue();
    if (fUseReals) {
      if (fFastAccess) {
        if ((iRaw < 1) || (iRaw > arAttVal.iSize()))
          segNew->PutVal(rUNDEF);
        else
          segNew->PutVal(arAttVal[iRaw-1]);
      }  
      else
        segNew->PutVal(colAtt->rValue(iRaw));
    }
    else {
      if (fFastAccess) {
        if ((iRaw < 1) || (iRaw > aiAttRaw.iSize()))
			segNew->PutVal(iUNDEF);
        else
          segNew->PutVal(aiAttRaw[iRaw-1]);
      }  
      else
        segNew->PutVal(colAtt->iRaw(iRaw));
    }
  }
  trq.fUpdate(iSeg, iSeg);
  return true;
}




