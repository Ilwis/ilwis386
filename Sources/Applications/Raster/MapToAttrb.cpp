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
/* MapToAttribute
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  JEL   8 Jul 97    1:48 pm
*/
#include "Applications\Raster\MAPTOATTRB.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapToAttribute(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapToAttribute::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapToAttribute(fn, (MapPtr &)ptr);
}

String wpsmetadataMapToAtrribute() {
	WPSMetaData metadata("MapToAttribute");

	return metadata.toString();
}

ApplicationMetadata metadataMapToAtrribute(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapToAtrribute();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapToAttribute::sSyntax();

	return md;
}

const char* MapToAttribute::sSyntax() {
  return "MapToAttribute(map,table, sourcecolumn, targercolumn, linktable)";
}

String MapToAttribute::sExpression() const {
	return String("MapToAttribute(%S,%S,%S,%S,%s)", mp->sNameQuoted(true, fnObj.sPath()),
		tblAttrb->sName(),
		colAttSource->sName(), 
		colAttTarget->sName(),
		mustLink ? "true" : "false");
}

MapToAttribute* MapToAttribute::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as(5);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  return new MapToAttribute(fn, p, mp, as[1], as[2], as[3], as[4]);
}

MapToAttribute::MapToAttribute(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	String sColName1, sColName2;
	try
	{
		ReadElement("MapToAttribute", "ForeignTable", tblAttrb);
		if (!tblAttrb.fValid())
			tblAttrb = tblAtt();
		if (!tblAttrb.fValid())
		{
			ErrorNoAttTable(fn/*, errMapToAttribute+1*/).Show(); // no throw to prevent invalid object
			return;
		}
		ReadElement("MapToAttribute", "SourceColumn", sColName1);
		colAttSource = tblAttrb[sColName1];

		ReadElement("MapToAttribute", "TargetColumn", sColName2);
		colAttTarget = tblAttrb[sColName2];

		ReadElement("MapToAttribute", "MustLink", mustLink);

	}
	catch (const ErrorObject& err)   // catch to prevent invalid object
	{
		err.Show();
		return;
	}
	if (!colAttSource.fValid())
	{
		ErrorNotFound(WhereErrorColumn(tblAttrb->fnObj, sColName1)).Show(); // no throw to prevent invalid object
		return;
	}
	if (!colAttTarget.fValid())
	{
		ErrorNotFound(WhereErrorColumn(tblAttrb->fnObj, sColName2)).Show(); // no throw to prevent invalid object
		return;
	}
	objdep.Add(colAttSource.ptr());
	objdep.Add(colAttTarget.ptr());
}

MapToAttribute::MapToAttribute(const FileName& fn, MapPtr& p,
           const Map& mp, const String& tablename, const String& sourcename, const String& targetname, const String& shouldlink)
: MapFromMap(fn, p, mp)
{
	String sCol;
	String sTable;
	tblAttrb = Table(FileName(tablename));

	if (!tblAttrb.fValid())
		throw ErrorObject(TR("Not a valid table for this operation"));

	colAttSource = tblAttrb[sourcename];
	if (!colAttSource.fValid())
		throw ErrorNotFound(WhereErrorColumn(tblAttrb->fnObj, sourcename));
	if ( !colAttSource->dm()->pdv())
		throw ErrorObject(TR("Source column must be value column"));

	colAttTarget = tblAttrb[targetname];
	if (!colAttTarget.fValid())
		throw ErrorNotFound(WhereErrorColumn(tblAttrb->fnObj, targetname));
	if ( !colAttTarget->dm()->pdsrt())
		throw ErrorObject(TR("Source column must be ID or class domain"));

	SetDomainValueRangeStruct(colAttTarget->dvrs());
	String link =  shouldlink;
	link.toLower();
	mustLink =  link == "yes" || link == "true" || link == "1";


	ptr.SetMinMax(colAttTarget->rrMinMax());
	objdep.Add(colAttTarget.ptr());
	objdep.Add(colAttSource.ptr());
	if (!fnObj.fValid())  // 'inline' object
	{
		objtime = objdep.tmNewest();
		const_cast<FileName&>(fnObj).Dir(mp->fnObj.sPath());
	}
}

void MapToAttribute::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapToAttribute");
  WriteElement("MapToAttribute", "ForeignTable", tblAttrb);
  WriteElement("MapToAttribute", "SourceColumn", colAttSource);
  WriteElement("MapToAttribute", "TargetColumn", colAttTarget);
  WriteElement("MapToAttribute", "MustLink", mustLink);
}

MapToAttribute::~MapToAttribute()
{
}

bool MapToAttribute::fFreezing() {
	map<long, long> value2id;
	DomainSort *dmsrt = colAttTarget->dm()->pdsrt();
	for(int r=1; r <= colAttSource->iRecs(); ++r) {
		long nr = colAttSource->iValue(r);
		String id = colAttTarget->sValue(r).sTrimSpaces();
		long raw = dmsrt->iRaw(id);
		value2id[nr] = raw;
	}

	long iNrRows = mp->iLines();
	long iNrCols = mp->iCols();
	LongBuf iBufIn(iNrCols);
	LongBuf iBufOut(iNrCols);

	for (long iRow = 0; iRow < iNrRows; ++iRow)  
	{  
		if (trq.fUpdate(iRow, iNrRows))
			return false;
		mp->GetLineVal(iRow, iBufIn);
		for(int i=0; i < iNrCols; ++i){
			iBufOut[i] = value2id[iBufIn[i]];
		}
		pms->PutLineRaw(iRow, iBufOut);
	}
	if ( mustLink) {
		//maptob.tbt = TableChangeDomain(mapto,cls)
		String outname("%S.tbt", sName());
		String expr("%S:=TableChangeDomain(%S, %S);",outname, tblAttrb->sName(), colAttTarget->sName());
		getEngine()->Execute(expr);
		//Table tbl(FileName(outname));
		SetAttributeTable(Table(FileName(outname)));
	}

	return true;
}

bool MapToAttribute::fDomainChangeable() const
{
  return false;
}

bool MapToAttribute::fValueRangeChangeable() const
{
  return false;
}





