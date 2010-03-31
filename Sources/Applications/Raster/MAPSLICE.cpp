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
/* MapSlicing
   Copyright Ilwis System Development ITC
   jan. 1996, by Jelle Wind
	Last change:  JEL   7 May 97    9:30 am
*/
#include "Applications\Raster\MAPSLICE.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmgroup.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapSlicing(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapSlicing::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapSlicing(fn, (MapPtr &)ptr);
}

String wpsmetadataSlicing() {
	WPSMetaData metadata("MapSlicing");
	metadata.AddTitle("Slicing");
	metadata.AddAbstract("Slice classification on raster map");
	metadata.AddKeyword("spatial");
	metadata.AddKeyword("raster");
	metadata.AddKeyword("Classification");
	WPSParameter parm1("inputmap","string");
	parm1.AddTitle("Filename Inputmap");
	WPSParameter parm2("domain","string");
	parm2.AddTitle("Filename group domain");
	metadata.AddParameter(parm1);
	metadata.AddParameter(parm2);
	WPSParameter parmout("outputmap","string",false);
	parmout.AddTitle("Filename Outputmap");
	metadata.AddParameter(parmout);

	return metadata.toString();
}

ApplicationMetadata metadataSlicing(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA") {
		md.wpsxml = wpsmetadataSlicing();
	}
	if ( query->queryType == "OUTPUTTYPE")
		md.returnType = IlwisObject::iotRASMAP;

	return md;
}


const char * MapSlicing::sSyntax()
 { return "MapSlicing(map,groupdomain)"; }

class DATEXPORT ErrorGroupDomain: public ErrorObject
{
public:
  ErrorGroupDomain(const String& sDomain, const WhereError& where)
  : ErrorObject(WhatError(String(SMAPErrDomainGroupOnly_S.scVal(), sDomain), errMapSlicing+1), where) {}
};

MapSlicing* MapSlicing::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  Domain dm(as[1]);
  return new MapSlicing(fn, p, mp, dm);
}

MapSlicing::MapSlicing(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  Init();
  objdep.Add(dm().ptr());
}

MapSlicing::MapSlicing(const FileName& fn, MapPtr& p,
           const Map& mp, const Domain& dm)
: MapFromMap(fn, p, mp)
{
  if (!dm->pdgrp())
    throw ErrorGroupDomain(dm->sName(), sTypeName());    
  SetDomainValueRangeStruct(dm);
  if (!mp->dvrs().fValues())
    ValueDomainError(mp->dm()->sName(), sTypeName(), errMapSlicing);
  Init();
  objdep.Add(dm.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapSlicing::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapSlicing");
}

MapSlicing::~MapSlicing()
{
}

void MapSlicing::Init()
{
  fNeedFreeze = false;
  htpFreeze = htpMapSlicingT;
  sFreezeTitle = "MapSlicing";
  dgrp = dm()->pdgrp();
}

bool MapSlicing::fDomainChangeable() const
{
  return false;
}

long MapSlicing::iComputePixelRaw(RowCol rc) const
{
  return dgrp->iClassify(mp->rValue(rc));
}

double MapSlicing::rComputePixelVal(RowCol) const
{
  return rUNDEF;
}

void MapSlicing::ComputeLineRaw(long Line, ByteBuf& buf, long iFrom, long iNum) const
{
  RealBuf b(iNum);
  mp->GetLineVal(Line, b, iFrom, iNum);
  for (long i = 0; i < iNum; ++i)
    buf[i] = byteConv(dgrp->iClassify(b[i]));
}

void MapSlicing::ComputeLineRaw(long Line, IntBuf& buf, long iFrom, long iNum) const
{
  RealBuf b(iNum);
  mp->GetLineVal(Line, b, iFrom, iNum);
  for (long i = 0; i < iNum; ++i)
    buf[i] = shortConv(dgrp->iClassify(b[i]));
}

void MapSlicing::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  RealBuf b(iNum);
  mp->GetLineVal(Line, b, iFrom, iNum);
  for (long i = 0; i < iNum; ++i)
    buf[i] = shortConv(dgrp->iClassify(b[i]));
}

void MapSlicing::ComputeLineVal(long , LongBuf& buf, long , long iNum) const
{
  for (long i = 0; i < iNum; ++i)
    buf[i] = iUNDEF;
}

void MapSlicing::ComputeLineVal(long , RealBuf& buf, long , long iNum) const
{
  for (long i = 0; i < iNum; ++i)
    buf[i] = rUNDEF;
}

String MapSlicing::sExpression() const
{
  return String("MapSlicing(%S,%S)", mp->sNameQuoted(false, fnObj.sPath()), dm()->sNameQuoted(false, fnObj.sPath()));
}
