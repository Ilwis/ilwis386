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
/* MapAttribute
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  JEL   8 Jul 97    1:48 pm
*/
#include "Applications\Raster\MAPATTRB.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapAttribute(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapAttribute::create(fn, (MapPtr &)ptr, sExpr);
	else  {
		if ( parms.size() == 0)
			return (IlwisObjectPtr *)new MapAttribute(fn, (MapPtr &)ptr);
		else  {
			Map *mp = (Map *)parms[1];
			String *scol = (String *)parms[0];
			return (IlwisObjectPtr *)new MapAttribute(fn, (MapPtr &)ptr, *mp, *scol);
		}
	}
}

String wpsmetadataMapAttribute() {
	WPSMetaData metadata("MapAttribute");

	return metadata.toString();
}

ApplicationMetadata metadataMapAtrribute(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapAttribute();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapAttribute::sSyntax();

	return md;
}

const char* MapAttribute::sSyntax() {
  return "MapAttribute(map,attribcol)\nmap.attribcol";
}

class DATEXPORT ErrorInvalidAttDomain: public ErrorObject
{
public:
  ErrorInvalidAttDomain(const String& sDomain, const WhereError& where)
  : ErrorObject(WhatError(String(SMAPErrInvalidAttrDomain.scVal(), sDomain.scVal()), 
                errMapAttribute), where) {}
};

MapAttribute* MapAttribute::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  return new MapAttribute(fn, p, mp, as[1]);
}

MapAttribute::MapAttribute(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	String sColName;
	try
	{
		fOtherAttTable = false;
		ReadElement("MapAttribute", "AttribTable", tblAttrb);
		if (!tblAttrb.fValid())
			tblAttrb = tblAtt();
		if (!tblAttrb.fValid())
		{
			ErrorNoAttTable(fn/*, errMapAttribute+1*/).Show(); // no throw to prevent invalid object
			return;
		}
		ReadElement("MapAttribute", "AttribColumn", sColName);
		colAtt = tblAttrb[sColName];
		if (fTblAttSelf())
		{
			Table tbl = tblAtt();
			fOtherAttTable = tbl.ptr() == tblAttrb.ptr();
		}
	}
	catch (const ErrorObject& err)   // catch to prevent invalid object
	{
		err.Show();
		return;
	}
	if (!colAtt.fValid())
	{
		ErrorNotFound(WhereErrorColumn(tblAttrb->fnObj, sColName)).Show(); // no throw to prevent invalid object
		return;
	}
	Init();
	objdep.Add(colAtt.ptr());
}

MapAttribute::MapAttribute(const FileName& fn, MapPtr& p,
           const Map& mp, const String& sColName)
: MapFromMap(fn, p, mp)
{
	String sCol;
	String sTable;
	tblAttrb = mp->tblAtt();
	Column::SplitTableAndColumn(tblAttrb, sColName, sTable, sCol);
	fOtherAttTable = sTable.length() != 0;
	if (fOtherAttTable)
		tblAttrb = Table(sTable);

	if (!tblAttrb.fValid())
		throw ErrorNoAttTable(mp->sTypeName()/*, errMapAttribute+1*/);

	colAtt = tblAttrb[sCol];
	if (!colAtt.fValid())
		throw ErrorNotFound(WhereErrorColumn(tblAttrb->fnObj, sColName));

	SetDomainValueRangeStruct(colAtt->dvrs());
	if (!dvrs().fValues() && (0 == dm()->pdsrt()) && (0 == dm()->pdp()) && (0 == dm()->pdcol()))
		throw ErrorInvalidAttDomain(dm()->sName(true, fnObj.sPath()), sTypeName());

	ptr.SetMinMax(colAtt->rrMinMax());
	Init();
	objdep.Add(colAtt.ptr());
	if (!fnObj.fValid())  // 'inline' object
	{
		objtime = objdep.tmNewest();
		const_cast<FileName&>(fnObj).Dir(mp->fnObj.sPath());
	}
}

void MapAttribute::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapAttribute");
  WriteElement("MapAttribute", "AttribTable", tblAttrb);
  WriteElement("MapAttribute", "AttribColumn", colAtt);
}

MapAttribute::~MapAttribute()
{
}

void MapAttribute::Init()
{
  fNeedFreeze = false;
  htpFreeze = "ilwisapp\\attribute_map_of_raster_map_algorithm.htm";
  sFreezeTitle = "MapAttribute";
  DomainSort* pdsrt = colAtt->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(SMAPErrNoDomainSort);

  long iSize = pdsrt->iSize();
  fRawsDifferent = false;
  if (vr().fValid() && colAtt->vr().fValid())
    fRawsDifferent = vr() != colAtt->vr();
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
    if (!fRawsDifferent)
      aiAttRaw[i-1] = colAtt->iRaw(i);
    else 
      aiAttRaw[i-1] = dvrs().iRaw(colAtt->rValue(i));
    aiAttVal[i-1] = colAtt->iValue(i);
    arAttVal[i-1] = colAtt->rValue(i);
  }
}

bool MapAttribute::fDomainChangeable() const
{
  return false;
}

bool MapAttribute::fValueRangeChangeable() const
{
  return false;
}

long MapAttribute::iComputePixelRaw(RowCol rc) const
{
  long iRaw = mp->iRaw(rc);
  if (fFastAccess){
    if ((iRaw < 1) || (iRaw > (long)aiAttRaw.iSize()))
      return iUNDEF;
    return aiAttRaw[iRaw-1];
  } 
  else if (!fRawsDifferent)
    return colAtt->iRaw(iRaw);
  else 
    return dvrs().iRaw(colAtt->rValue(iRaw));
}

double MapAttribute::rComputePixelVal(RowCol rc) const
{
  long iRaw = mp->iRaw(rc);
  if (fFastAccess) {
    if ((iRaw < 1) || (iRaw > (long)arAttVal.iSize()))
      return rUNDEF;
    return arAttVal[iRaw-1];
  }  
  else
    return colAtt->rValue(iRaw);
}

void MapAttribute::ComputeLineRaw(long Line, ByteBuf& buf, long iFrom, long iNum) const
{
  LongBuf b(iNum);
  mp->GetLineRaw(Line, b, iFrom, iNum);
  if (fFastAccess)
    for (long i = 0; i < iNum; ++i)
      if ((b[i] < 1) || (b[i] > (long)aiAttRaw.iSize()))
        buf[i] = 0;
      else
        buf[i] = byteConv(aiAttRaw[b[i]-1]);  
  else
    for (long i = 0; i < iNum; ++i)
      if (b[i] == 0)
        buf[i] = 0;
      else if (!fRawsDifferent)
        buf[i] = byteConv(colAtt->iRaw(b[i]));  

      else 
        buf[i] = byteConv(dvrs().iRaw(colAtt->rValue(b[i]))); 
}


void MapAttribute::ComputeLineRaw(long Line, IntBuf& buf, long iFrom, long iNum) const
{
  LongBuf b(iNum);
  mp->GetLineRaw(Line, b, iFrom, iNum);
  if (fFastAccess)
    for (long i = 0; i < iNum; ++i)
      if ((b[i] < 1) || (b[i] > (long)aiAttRaw.iSize()))
        buf[i] = shUNDEF;
      else
        buf[i] = shortConv(aiAttRaw[b[i]-1]);
  else
    for (long i = 0; i < iNum; ++i)
      if (b[i] == iUNDEF)
        buf[i] = shUNDEF;
      else if (!fRawsDifferent)
        buf[i] = shortConv(colAtt->iRaw(b[i]));
      else 
        buf[i] = shortConv(dvrs().iRaw(colAtt->rValue(b[i])));
}

void MapAttribute::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  LongBuf b(iNum);
  mp->GetLineRaw(Line, b, iFrom, iNum);
  if (fFastAccess)
    for (long i = 0; i < iNum; ++i)
      if ((b[i] < 1) || (b[i] > (long)aiAttRaw.iSize()))
        buf[i] = iUNDEF;
      else
        buf[i] = aiAttRaw[b[i]-1];
  else
    for (long i = 0; i < iNum; ++i)
      if (b[i] == iUNDEF)
        buf[i] = iUNDEF;
      else if (!fRawsDifferent)
        buf[i] = colAtt->iRaw(b[i]);
      else 
        buf[i] = dvrs().iRaw(colAtt->rValue(b[i]));
}

void MapAttribute::ComputeLineVal(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  LongBuf b(iNum);
  mp->GetLineRaw(Line, b, iFrom, iNum);
  if (fFastAccess)
    for (long i = 0; i < iNum; ++i)
      if ((b[i] < 1) || (b[i] > (long)arAttVal.iSize()))
        buf[i] = iUNDEF;
      else
        buf[i] = aiAttVal[b[i]-1];
  else
    for (long i = 0; i < iNum; ++i)
      if (b[i] == iUNDEF)
        buf[i] = iUNDEF;
      else
        buf[i] = colAtt->iValue(b[i]);
}

void MapAttribute::ComputeLineVal(long Line, RealBuf& buf, long iFrom, long iNum) const
{
  LongBuf b(iNum);
  mp->GetLineRaw(Line, b, iFrom, iNum);
  if (fFastAccess)
    for (long i = 0; i < iNum; ++i)
      if ((b[i] < 1) || (b[i] > (long)arAttVal.iSize()))
        buf[i] = rUNDEF;
      else
        buf[i] = arAttVal[b[i]-1];
  else
    for (long i = 0; i < iNum; ++i)
      if (b[i] == iUNDEF)
        buf[i] = rUNDEF;
      else
        buf[i] = colAtt->rValue(b[i]);
}

String MapAttribute::sExpression() const
{
  if (fOtherAttTable)
    return String("%S.%S.%S", mp->sNameQuoted(false, fnObj.sPath()), tblAttrb->sNameQuoted(false, fnObj.sPath()), colAtt->sNameQuoted());
  else
    return String("%S.%S", mp->sNameQuoted(false, fnObj.sPath()), colAtt->sNameQuoted());
}

bool MapAttribute::fFreezing()
{
  trq.SetText(String(SMAPTextCalculating_S.scVal(), sName(true, fnObj.sPath())));
  if ( dm()->pdv())
  {
    RealBuf buf(iCols());
    for (long i = 0; i < iLines(); ++i) 
    {
        if (trq.fUpdate(i, iLines()) ) 
            return false; 
        ComputeLineVal(i, buf, 0, buf.iSize());
        pms->PutLineVal(i, buf, 0, buf.iSize());
    } 
  }
  else
    MapVirtual::fFreezing();
    

  return true;
}




