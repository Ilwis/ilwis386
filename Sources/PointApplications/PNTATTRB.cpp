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
/* PointMapAttribute
   Copyright Ilwis System Development ITC
   nov 1995, by Jelle Wind
	Last change:  JEL   6 May 97    5:54 pm
*/

#include "PointApplications\PNTATTRB.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\point.hs"


IlwisObjectPtr * createPointMapAttribute(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapAttribute::create(fn, (PointMapPtr &)ptr, sExpr);
	else {
		if ( parms.size() > 0 ) {
			PointMap pmp = *((PointMap *)parms[0]);
			String col = *((String *)parms[1]);
			return (IlwisObjectPtr *)new PointMapAttribute(fn, (PointMapPtr &)ptr, pmp, col);
		}
		return (IlwisObjectPtr *)new PointMapAttribute(fn, (PointMapPtr &)ptr);
	}
}

class DATEXPORT ErrorInvalidAttDomain: public ErrorObject
{
public:
  ErrorInvalidAttDomain(const String& sDomain, const WhereError& where)
  : ErrorObject(WhatError(String(SPNTErrWrongAttrDomain.scVal(), sDomain), 
                errPolygonMapAttribute+1), where) {}
};

const char* PointMapAttribute::sSyntax() {
  return "PointMapAttribute(pntmap,attribcol)\npntmap.attribcol";
}

PointMapAttribute* PointMapAttribute::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  PointMap pmp(as[0], fn.sPath());
  return new PointMapAttribute(fn, p, pmp, as[1]);
}

PointMapAttribute::PointMapAttribute(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  String sColName;
  try {
//    ObjectInfo::ReadElement("PointMapAttribute", "PointMap", fnObj, pmp);
//    ObjectInfo::ReadElement("PointMapAttribute", "AttribTable", fnObj, tblAttrb);
    ReadElement("PointMapAttribute", "PointMap", pmp);
    ReadElement("PointMapAttribute", "AttribTable", tblAttrb);
    if (!tblAttrb.fValid())
      tblAttrb = tblAtt();
    if (!tblAttrb.fValid()) {
      ErrorNoAttTable(fn, errPointMapAttribute).Show(); // no throw to prevent invalid object
      return;
    }
//    jectInfo::ReadElement("PointMapAttribute", "AttribColumn", fnObj, sColName);
    ReadElement("PointMapAttribute", "AttribColumn", sColName);
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
}

PointMapAttribute::~PointMapAttribute()
{
}

PointMapAttribute::PointMapAttribute(const FileName& fn, PointMapPtr& p,
           const PointMap& pmap, const String& sColName)
: PointMapVirtual(fn, p, pmap->cs(), pmap->cb(), pmap->dvrs()), pmp(pmap)
{
  if (fnObj.sPath() == "")
	  const_cast<FileName&>(fnObj).Dir(pmp->fnObj.sPath());
  _iPoints = pmp->iFeatures();
  colAtt = Column(tblAttrb, sColName);
  if (colAtt.fValid())
    tblAttrb = Table(colAtt->fnTbl);
  else {
    tblAttrb = pmp->tblAtt();
    if (!tblAttrb.fValid())
      throw ErrorNoAttTable(pmp->sTypeName(), errPointMapAttribute);
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
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void PointMapAttribute::InitFreeze()
{
  _iPoints = pmp->iFeatures();
}

void PointMapAttribute::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapAttribute");
  WriteElement("PointMapAttribute", "PointMap", pmp);
  WriteElement("PointMapAttribute", "AttribTable", tblAttrb);
  WriteElement("PointMapAttribute", "AttribColumn", colAtt);
}

String PointMapAttribute::sExpression() const
{
  String s1 = pmp->sNameQuoted(false, fnObj.sPath());
  String s2 = colAtt->sNameQuoted();
  String str("%S.%S", s1, s2);
  return str;
}

bool PointMapAttribute::fDomainChangeable() const
{
  return false;
}

void PointMapAttribute::Init()
{
  htpFreeze = "ilwisapp\\attribute_map_of_point_map_algorithm.htm";
  sFreezeTitle = "PointMapAttribute";
  DomainSort* pdsrt = colAtt->dmKey()->pdsrt();
  assert(pdsrt);
  long iSize = pdsrt->iSize();
  fFastAccess = iSize <= 1000;
  if (!fFastAccess)
    return;

  aiAttRaw.Resize(iSize); // now zero based
  aiAttVal.Resize(iSize);
  arAttVal.Resize(iSize);
  for (short i=1; i<= iSize ; i++ ) {
    aiAttRaw[i-1] = colAtt->iRaw(i);
    aiAttVal[i-1] = colAtt->iValue(i);
    arAttVal[i-1] = colAtt->rValue(i);
  }
	ptr.SetMinMax(colAtt->rrMinMax());
	ptr.SetMinMax(colAtt->riMinMax());
}

long PointMapAttribute::iRaw(long iRec) const
{
  long iRaw = pmp->iRaw(iRec);
  if (fFastAccess) {
    if ((iRaw < 1) || (iRaw > (int)aiAttRaw.iSize()))
      return iUNDEF;
    return aiAttRaw[iRaw-1];
  }
  else
    return colAtt->iRaw(iRaw);
}

long PointMapAttribute::iValue(long iRec) const
{
  long iRaw = pmp->iRaw(iRec);
  if (fFastAccess) {
    if ((iRaw < 1) || (iRaw > (int)aiAttVal.iSize()))
      return iUNDEF;
    return aiAttVal[iRaw-1];
  }  
  else
    return colAtt->iValue(iRaw);
}

double PointMapAttribute::rValue(long iRec) const
{
  long iRaw = pmp->iRaw(iRec);
  if (fFastAccess) {
    if ((iRaw < 1) || (iRaw > (int)arAttVal.iSize()))
      return rUNDEF;
    return arAttVal[iRaw-1];
  }
  else
    return colAtt->rValue(iRaw);
}

String PointMapAttribute::sValue(long iRec, short iWidth, short iDec) const
{
  long iRaw = pmp->iRaw(iRec);
  return colAtt->sValue(iRaw, iWidth, iDec);
}

Coord PointMapAttribute::cValue(long iRec) const
{
  return pmp->cValue(iRec);
}

long PointMapAttribute::iPnt() const
{
	return pmp->iFeatures();
}
