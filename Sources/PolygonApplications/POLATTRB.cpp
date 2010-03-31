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
/* PolygonMapAttribute
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  JEL   6 May 97    5:58 pm
*/

#include "PolygonApplications\POLATTRB.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Engine\Table\COLREAL.H"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\polygon.hs"

IlwisObjectPtr * createPolygonMapAttribute(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapAttribute::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else{
		if ( parms.size() > 0 ) {
			PolygonMap smp = *((PolygonMap *)parms[0]);
			String col = *((String *)parms[1]);
			return (IlwisObjectPtr *)new PolygonMapAttribute(fn, (PolygonMapPtr &)ptr, smp, col);
		}
		return (IlwisObjectPtr *)new PolygonMapAttribute(fn, (PolygonMapPtr &)ptr);
	}
}

class DATEXPORT ErrorInvalidAttDomain: public ErrorObject
{
public:
  ErrorInvalidAttDomain(const String& sDomain, const WhereError& where)
  : ErrorObject(WhatError(String(SPOLErrInvalidColumnDomain_S.scVal(), sDomain), 
                errPolygonMapAttribute+1), where) {}
};

const char* PolygonMapAttribute::sSyntax() {
  return "PolygonMapAttribute(polmap,attribcol)\npolmap.attribcol";
}

PolygonMapAttribute* PolygonMapAttribute::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  PolygonMap pmp(as[0], fn.sPath());
  return new PolygonMapAttribute(fn, p, pmp, as[1]);
}

PolygonMapAttribute::PolygonMapAttribute(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("PolygonMapAttribute", "PolygonMap", pmp);
    ReadElement("PolygonMapAttribute", "AttribTable", tblAttrb);
    if (!tblAttrb.fValid())
      tblAttrb = tblAtt();
    if (!tblAttrb.fValid())
      ErrorNoAttTable(fn, errPolygonMapAttribute).Show(); // no throw to prevent invalid object
    ReadElement("PolygonMapAttribute", "AttribColumn", sColName);
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
  objdep.Add(pmp.ptr());
  objdep.Add(colAtt.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

PolygonMapAttribute::PolygonMapAttribute(const FileName& fn, PolygonMapPtr& p, const PolygonMap& pm,
                                         const String& sColName)
: PolygonMapVirtual(fn, p, pm->cs(),pm->cb(),pm->dvrs()), pmp(pm)
{
  fNeedFreeze = true;
  colAtt = Column(tblAttrb, sColName);
  if (colAtt.fValid())
    tblAttrb = Table(colAtt->fnTbl);
  else {
    tblAttrb = pmp->tblAtt();
    if (!tblAttrb.fValid())
      throw ErrorNoAttTable(pmp->sTypeName(), errPolygonMapAttribute);
    colAtt = tblAttrb[sColName];
    if (!colAtt.fValid())
      throw ErrorNotFound(WhereErrorColumn(tblAttrb->fnObj, sColName));
  }  
  SetDomainValueRangeStruct(colAtt->dvrs());
  if (!dvrs().fValues() && (0 == dm()->pdsrt()))
    throw ErrorInvalidAttDomain(dm()->sName(true, fnObj.sPath()), sTypeName());
  Init();
  objdep.Add(pmp.ptr());
  objdep.Add(colAtt.ptr());
}

void PolygonMapAttribute::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapAttribute");
  WriteElement("PolygonMapAttribute", "PolygonMap", pmp);
  WriteElement("PolygonMapAttribute", "AttribTable", tblAttrb);
  WriteElement("PolygonMapAttribute", "AttribColumn", colAtt);
}

PolygonMapAttribute::~PolygonMapAttribute()
{
}

String PolygonMapAttribute::sExpression() const
{
  return String("%S.%S", pmp->sNameQuoted(false, fnObj.sPath()), colAtt->sNameQuoted());
}

bool PolygonMapAttribute::fDomainChangeable() const
{
  return false;
}

void PolygonMapAttribute::Init()
{
  htpFreeze = htpPolygonMapAttributeT;
  sFreezeTitle = "PolygonMapAttribute";
  DomainSort* pdsrt = colAtt->dmKey()->pdsrt();
  assert(pdsrt);
  long iSize = pdsrt->iSize();
  fFastAccess = iSize <= 1000;
  if (!fFastAccess)
    return;
/*  aiAttRaw.Resize(iSize, 1);
  aiAttVal.Resize(iSize, 1);
  arAttVal.Resize(iSize, 1);*/
  aiAttRaw.Resize(iSize);  // now zero based
  aiAttVal.Resize(iSize);  // now zero based
  arAttVal.Resize(iSize);  // now zero based
  for (short i=1; i<= iSize ; i++ ) {
    aiAttRaw[i-1] = colAtt->iRaw(i);
    aiAttVal[i-1] = colAtt->iValue(i);
    arAttVal[i-1] = colAtt->rValue(i);
  }
}

bool PolygonMapAttribute::fFreezing()
{
  StoreType stp = st();
  trq.SetText(SPOLTextCopyPolygons);
  long iPol = pmp->iFeatures();
  for (long i=0; i < iPol; ++i) {
    if (trq.fUpdate(i, iPol))
      return false;
    ILWIS::Polygon *polOld = (ILWIS::Polygon *)pmp->getFeature(i);
	ILWIS::Polygon *polNew = (ILWIS::Polygon *)ptr.newFeature(polOld);
    long iRaw = polOld->iValue();
    if (stp == stREAL) {
      if (fFastAccess) {
        if ((iRaw < 1) || (iRaw > (int)arAttVal.iSize()))
          polNew->PutVal(rUNDEF);
        else
          polNew->PutVal(arAttVal[iRaw-1]);
      }  
      else
        polNew->PutVal(colAtt->rValue(iRaw));
    }
    else {
      if (fFastAccess)
        if ((iRaw < 1) || (iRaw > (int)aiAttRaw.iSize()))
          polNew->PutVal(iUNDEF);
        else
          polNew->PutVal(aiAttRaw[iRaw-1]);
	  else
        polNew->PutVal(colAtt->iRaw(iRaw));
    }
  }
  return true;
} 





