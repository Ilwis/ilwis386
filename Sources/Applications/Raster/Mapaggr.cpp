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

#include "Applications\Raster\MAPAGGR.H"
#include "Applications\Table\AGGFUNCS.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\SpatialReference\Grfactor.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapAggregate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapAggregate::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapAggregate(fn, (MapPtr &)ptr);
}

String wpsmetadataMapAggregate() {
	WPSMetaData metadata("MapAggregate");


	return metadata.toString();
}

ApplicationMetadata metadataMapAggregate(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapAggregate();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  String("%s\n%s",MapAggregate::sSyntax(),MapAggregate::sSyntax2());

	return md;
}

#define rHalf 0.4999999999

const char* MapAggregate::sSyntax() {
	return "MapAggregateFunc(map,groupfactor,group|nogroup)\nMapAggregate(map,Func,groupfactor,group|nogroup[,rowoffset,coloffset])\n";
}

const char* MapAggregate::sSyntax2() {
	return "MapAggregateFunc(map,groupfactor,group|nogroup[,grf])\nMapAggregate(map,Func,groupfactor,group|nogroup,grf)\n";
}

static void ThrowTooLargeError(String& sMessage, const FileName& fn)
{
	throw ErrorObject(WhatError(sMessage, errMapAggregate+3), fn);
}

class DATEXPORT ErrorGroupFactor: public ErrorObject
{
public:
	ErrorGroupFactor(const String& sErr,const WhereError& where)
		: ErrorObject(WhatError(sErr, errMapAggregate), where) {}
};

void GroupFactorError(bool fTooSmall, const FileName& fn)
{
	String sErr;
	if (fTooSmall)
		sErr = String(TR("Group factor should be >= 2"));
	else
		sErr = String(TR("Group factor should not exceed row or column size of input map"));
	throw ErrorGroupFactor(sErr, fn);
}

class DATEXPORT ErrorOffset: public ErrorObject
{
public:
	ErrorOffset(const String& sErr, const WhereError& where)
		: ErrorObject(WhatError(sErr, errMapAggregate+2), where) {}
};

void OffsetError(const String& sOff, bool fRow, const FileName& fn)
{
	String sErr;
	if (fRow)
		sErr = String(TR("Invalid row offset: %S").c_str(), sOff);
	else
		sErr = String(TR("Invalid column offset: %S").c_str(), sOff);
	throw ErrorOffset(sErr, fn);
}

MapAggregate* MapAggregate::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	String sAggFunc = IlwisObjectPtr::sParseFunc(sExpr);
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	AggregateFunction* agf = 0;
	int parmOffset = 0; // new style syntax has one parm more
	if ((iParms < 3) || (iParms > 7))
		ExpressionError(sExpr, sSyntax());
	if ( sAggFunc.size() == String("MapAggregate").size()) { // new style
		agf = AggregateFunction::create(String("agg%S", as[1]));
		parmOffset = 1;
	} else
		agf = AggregateFunction::create(String("agg%S", sAggFunc.sRight(3))); // old style
	if ( agf == 0)
		ExpressionError(sExpr, sSyntax());

	Map mp(as[0], fn.sPath());
	long h = mp->iLines();
	long w = mp->iCols();
	long m = min(h,w);
	long iGrpFac = as[1 + parmOffset].iVal();
	if (iGrpFac <= 1)
		GroupFactorError(true, fn);
	if (iGrpFac > m)
		GroupFactorError(false, fn);
	bool fGrpPix = true;
	if (fCIStrEqual(as[2 + parmOffset] , "NoGroup"))
		fGrpPix = false;
	else if (!fCIStrEqual(as[2 + parmOffset] , "Group"))
		ExpressionError(sExpr, sSyntax());
	long iRowOffset = 0, iColOffset = 0;
	if (iParms >= 5) {
		iRowOffset = as[3 + parmOffset].iVal();
		iColOffset = as[4 + parmOffset].iVal();
		if (iRowOffset < 0)
			OffsetError(as[3 + parmOffset], true, fn);
		if (iColOffset < 0)
			OffsetError(as[4 + parmOffset], false, fn);
	}
	String sGrf = fn.sFile;
	if ((iParms == 5) || (iParms == 7))
		sGrf = as[iParms-1 + parmOffset];
	if (fGrpPix && (sGrf.length() == 0))
		ExpressionError(sExpr, sSyntax2());
	return new MapAggregate(fn, p, mp, iGrpFac, fGrpPix, iRowOffset, iColOffset, agf, sGrf);
}

MapAggregate::MapAggregate(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	String s;
	ReadElement("MapAggregate", "AggregateFunction", s);
	agf = AggregateFunction::create(s);
	ReadElement("MapAggregate", "GroupFactor", iGrpFac);
	if (iGrpFac <= 1)
		GroupFactorError(true, fnObj);
	long h = mp->iLines();
	long w = mp->iCols();
	long m = min(h,w);
	if (iGrpFac > m)
		GroupFactorError(false, fnObj);
	fGrpPixels = true;
	ReadElement("MapAggregate", "GroupPixels", fGrpPixels);
	if (fGrpPixels)
		ReadGeoRefFromFileAndSet();
	ReadElement("MapAggregate", "RowOffset", iRowOffset);
	ReadElement("MapAggregate", "ColOffset", iColOffset);
	if (iRowOffset == iUNDEF)
		iRowOffset = 0;
	if (iColOffset == iUNDEF)
		iColOffset = 0;
	Init();
}

MapAggregate::MapAggregate(const FileName& fn, MapPtr& p,
						   const Map& mp, long iGrpFactor, bool fGrpPix, long iRowOff, long iColOff,
						   AggregateFunction* agfnc, const String& sGrf)
						   : MapFromMap(fn, p, mp), iGrpFac(iGrpFactor), agf(agfnc), 
						   iRowOffset(iRowOff), iColOffset(iColOff), fGrpPixels(fGrpPix)
{
	String sAggName;
	if (0 != agf) {
		sAggName = agf->sName();
		if (!mp->fValues() && agf->fValuesNeeded())
			ValueDomainError(mp->dm()->sName(true, fnObj.sPath()), mp->sTypeName(), errMapAggregate+1);
	}
	if (fGrpPixels)
		if (mp->gr()->fGeoRefNone())
			ptr.SetSize(RowCol((mp->iLines()-iRowOffset)/iGrpFac, (mp->iCols()-iColOffset)/iGrpFac));
		else {
			GeoRef grNew;
			grNew.SetPointer(new GeoRefFactor(FileName(sGrf, ".grf", true), mp->gr(), 1.0/iGrpFac, iRowOffset, iColOffset));
			ptr.SetGeoRef(grNew);
			ptr.SetSize(grNew->rcSize());
		}
		Init();
		objdep.Add(ptr.gr());

		if (mp->fTblAtt()) 
			SetAttributeTable(mp->tblAtt());
		if (!fnObj.fValid())
			objtime = objdep.tmNewest();
		if (fCIStrEqual(sAggName , "aggsum")) {
			RangeReal rr = mp->rrMinMax(BaseMapPtr::mmmNOCALCULATE);
			if (!rr.fValid())
				rr = mp->dvrs().rrMinMax();
			double rStep = mp->dvrs().rStep();
			SetDomainValueRangeStruct(DomainValueRangeStruct(Domain("value"), 
				ValueRange(rr.rLo()*iGrpFac*iGrpFac, rr.rHi()*iGrpFac*iGrpFac, rStep)));
		}
		else if (fCIStrEqual(sAggName , "aggavg")) {
			if (0 == mp->dm()->pdi()) {
				RangeReal rr = mp->rrMinMax(BaseMapPtr::mmmNOCALCULATE);
				if (!rr.fValid())
					rr = mp->dvrs().rrMinMax();
				double rStep = mp->dvrs().rStep()/100.0;
				SetValueRange(ValueRange(rr.rLo(), rr.rHi(), rStep));
			}
		}
		else if (fCIStrEqual(sAggName , "aggstd")) {
			double rStep = mp->dvrs().rStep()/100.0;
			SetDomainValueRangeStruct(DomainValueRangeStruct(Domain("value"), ValueRange(0,1000,rStep)));
		}
		else if (fCIStrEqual(sAggName , "aggcnt"))
		{
			SetDomainValueRangeStruct(DomainValueRangeStruct(Domain("count"), ValueRange(0,iGrpFac*iGrpFac,1)));
		}      
}

void MapAggregate::Store()
{
	MapFromMap::Store();
	WriteElement("MapFromMap", "Type", "MapAggregate");
	if (0 != agf)
		WriteElement("MapAggregate", "AggregateFunction", agf->sName());
	WriteElement("MapAggregate", "GroupFactor", iGrpFac);
	WriteElement("MapAggregate", "GroupPixels", fGrpPixels);
	WriteElement("MapAggregate", "RowOffset", iRowOffset);
	WriteElement("MapAggregate", "ColOffset", iColOffset);
}

MapAggregate::~MapAggregate()
{
	if (0 != agf) 
		delete agf;
	delete [] bufListValReal;
	delete [] bufListValRaw;
}

void MapAggregate::Init()
{
	sFreezeTitle = "MapAggregate";
	htpFreeze = "ilwisapp\\aggregate_algorithm.htm";
	//  long iInpMapLines = mp->iLines();
	long iInpMapCols = mp->iCols();
	bufListValReal = new RealBuf[iGrpFac];
	for (int i=0; i < iGrpFac; i++)
		bufListValReal[i].Size(iInpMapCols);
	bufListValRaw = new LongBuf[iGrpFac];
	for (int i=0; i < iGrpFac; i++)
		bufListValRaw[i].Size(iInpMapCols);
}

bool MapAggregate::fFreezing()
{
	if (fGrpPixels) // follow 'normal' procedure
		return MapVirtual::fFreezing();
	// else special case
	trq.SetText(String(TR("Calculating '%S'").c_str(), sName(true, fnObj.sPath())));
	RealBuf bufOutReal(iCols());
	LongBuf bufOutRaw(iCols());
	long i, j;
	for (j=0; j < iCols(); ++j) {
		bufOutReal[j] = rUNDEF;
		bufOutRaw[j] = iUNDEF;
	}
	for (i=0; i < iRowOffset ; ++i) 
		pms->PutLineRaw(i, bufOutRaw);
	long iLastLine = ((iLines()-iRowOffset) / iGrpFac) * iGrpFac;
	long iLastCol = ((iCols()-iColOffset) / iGrpFac) * iGrpFac;
	if (dvrs().fValues()) {
		for (i=iRowOffset; i < iLastLine; i += iGrpFac) {
			ComputeLineVal((i-iRowOffset)/iGrpFac, bufOutReal, 0, (iCols()-iColOffset)/iGrpFac);
			// expand buffer
			for (j=iCols()-1; j >= iLastCol; --j) 
				bufOutReal[j] = rUNDEF;
			for (; j >= iColOffset; --j)
				bufOutReal[j] = bufOutReal[(j-iColOffset)/iGrpFac];
			for (j=0; j < iColOffset; ++j)
				bufOutReal[j] = rUNDEF;
			for (j=0; j < iGrpFac; ++j) {
				if (trq.fUpdate(i+j, iLines()))
					return false;
				pms->PutLineVal(i+j, bufOutReal);
			}
		}
		for (j=0; j < iCols(); ++j)
			bufOutReal[j] = rUNDEF;
		for (i=iLastLine; i < iLines(); ++i)
			pms->PutLineVal(i, bufOutReal);
		trq.fUpdate(iLines(), iLines());
	}
	else {
		for (i=iRowOffset; i < iLastLine; i += iGrpFac) {
			ComputeLineRaw((i-iRowOffset)/iGrpFac, bufOutRaw, 0, (iCols()-iColOffset)/iGrpFac);
			// expand buffer
			for (j=iCols()-1; j >= iLastCol; --j) 
				bufOutRaw[j] = iUNDEF;
			for (; j >= iColOffset; --j)
				bufOutRaw[j] = bufOutRaw[(j-iColOffset)/iGrpFac];
			for (j=0; j < iColOffset; ++j)
				bufOutRaw[j] = iUNDEF;
			for (j=0; j < iGrpFac; ++j) {
				if (trq.fUpdate(i+j, iLines()))
					return false;
				pms->PutLineRaw(i+j, bufOutRaw);
			}
		}
		for (j=0; j < iCols(); ++j)
			bufOutRaw[j] = iUNDEF;
		for (i=iLastLine; i < iLines(); ++i)
			pms->PutLineRaw(i, bufOutRaw);
		trq.fUpdate(iLines(), iLines());
	}
	return true;
}

long MapAggregate::iComputePixelRaw(RowCol rc) const
{
	LongBuf buf(1);
	ComputeLineRaw(rc.Row, buf, rc.Col, 1);
	return buf[0];
}

double MapAggregate::rComputePixelVal(RowCol rc) const
{
	RealBuf buf(1);
	ComputeLineVal(rc.Row, buf, rc.Col, 1);
	return buf[0];
}

void MapAggregate::ComputeLineRaw(long Line, ByteBuf& buf, long iFrom, long iNum) const
{
	if (dvrs().fValues()) {
		RealBuf rb(iNum);
		ComputeLineVal(Line, rb, iFrom, iNum);
		for (int i=0; i < iNum; i++)
			buf[i] = byteConv(dvrs().iRaw(rb[i]));
	}
	else {
		LongBuf lb(iNum);
		ComputeLineRaw(Line, lb, iFrom, iNum);
		for (int i=0; i < iNum; i++)
			buf[i] = byteConv(lb[i]);
	}
}

void MapAggregate::ComputeLineRaw(long Line, IntBuf& buf, long iFrom, long iNum) const
{
	if (dvrs().fValues()) {
		RealBuf rb(iNum);
		ComputeLineVal(Line, rb, iFrom, iNum);
		for (int i=0; i < iNum; i++)
			buf[i] = shortConv(dvrs().iRaw(rb[i]));
	}
	else {
		LongBuf lb(iNum);
		ComputeLineRaw(Line, lb, iFrom, iNum);
		for (int i=0; i < iNum; i++)
			buf[i] = shortConv(lb[i]);
	}
}

void MapAggregate::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
	if (dvrs().fValues()) {
		RealBuf rb(iNum);
		ComputeLineVal(Line, rb, iFrom, iNum);
		for (int i=0; i < iNum; i++)
			buf[i] = dvrs().iRaw(rb[i]);
	}
	else {
		//const_cast<MapAggregate *>(this)->
		ReadBufLinesRaw(Line, iFrom, iNum);
		for (long i=iFrom; i < iFrom + iNum; i++) {
			if (0 != agf) {
				agf->reset();
				long iRaw;
				for (long j=0; j < iGrpFac; ++j)
					for (long k=i*iGrpFac; k < (i+1)*iGrpFac; ++k) {
						iRaw = bufListValRaw[j][k];
						agf->AddRaw(0L, iRaw);
					}
					buf[i] = agf->iRawResult(0L);
			}
			else
				buf[i] = bufListValRaw[0][i*iGrpFac];
		}
	}
}

void MapAggregate::ComputeLineVal(long Line, LongBuf& buf, long iFrom, long iNum) const
{
	RealBuf rb(iNum);
	ComputeLineVal(Line, rb, iFrom, iNum);
	for (int i=0; i < iNum; i++)
		buf[i] = longConv(rb[i]);
}

void MapAggregate::ComputeLineVal(long Line, RealBuf& buf, long iFrom, long iNum) const
{
	//const_cast<MapAggregate *>(this)->
	bool fVals = mp->dvrs().fValues(); // input domain value
	if (fVals) {
		ReadBufLinesReal(Line, iFrom, iNum);
		for (long i=iFrom; i < iFrom + iNum; i++) {
			if (0 != agf) {
				agf->reset();
				double rVal;
				for (long j=0; j < iGrpFac; ++j)
					for (long k=i*iGrpFac; k < (i+1)*iGrpFac; ++k) {
						rVal = bufListValReal[j][k];
						agf->AddRaw(0L, rVal);  ///???
					}
					buf[i] = agf->rValResult(0L);
			}
			else
				buf[i] = bufListValReal[0][i*iGrpFac];
		}
	}
	else { // if output is values but input is not (e.g. class or id domain)
		ReadBufLinesRaw(Line, iFrom, iNum);
		for (long i=iFrom; i < iFrom + iNum; i++) {
			if (0 != agf) {
				agf->reset();
				long iRaw;
				for (long j=0; j < iGrpFac; ++j)
					for (long k=i*iGrpFac; k < (i+1)*iGrpFac; ++k) {
						iRaw = bufListValRaw[j][k];
						agf->AddRaw(0L, iRaw);
					}
					buf[i] = agf->rValResult(0L);
			}
			else
				buf[i] = bufListValRaw[0][i*iGrpFac];
		}
	}
}

void MapAggregate::ReadLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
	if (iLine < 0)
		iLine = 0;
	else if (iLine >= mp->iLines())
		iLine = mp->iLines()-1;
	mp->GetLineRaw(iLine, buf, iFrom, iNum);
}

void MapAggregate::ReadLineVal(long iLine, RealBuf& buf, long iFrom, long iNum) const
{
	if (iLine < 0)
		iLine = 0;
	else if (iLine >= mp->iLines())
		iLine = mp->iLines()-1;
	mp->GetLineVal(iLine, buf, iFrom, iNum);
}

void MapAggregate::ReadBufLinesReal(long iLine, long iFrom, long iNum) const
{
	long iInputLine = iLine*iGrpFac + iRowOffset;
	long iInputFrom = iFrom*iGrpFac + iColOffset;
	long iInputNum = iNum*iGrpFac;
	if ((iInputFrom == iLastInpFrom) && (iInputNum == iLastInpNum)) {
		if (iInputLine == iLastInpLine) // already read in
			return;
		if (iLastInpLine == iInputLine - 1) { // roulate buffers and only read last line
			for (int i=0; i < iGrpFac-1; i++)
				Swap(bufListValReal[i], bufListValReal[i+1]);
			ReadLineVal(iInputLine+iGrpFac-1, bufListValReal[iGrpFac-1], iInputFrom, iInputNum);
			iLastInpLine = iInputLine;
			return;
		}
	}
	// read all lines
	for (int i=0; i < iGrpFac; i++)
		ReadLineVal(iInputLine+i, bufListValReal[i], iInputFrom, iInputNum);
	iLastInpLine = iInputLine;
	iLastInpFrom = iInputFrom;
	iLastInpNum = iInputNum;
}

void MapAggregate::ReadBufLinesRaw(long iLine, long iFrom, long iNum) const
{
	long iInputLine = iLine*iGrpFac + iRowOffset;
	long iInputFrom = iFrom*iGrpFac + iColOffset;
	long iInputNum = iNum*iGrpFac;
	if ((iInputFrom == iLastInpFrom) && (iInputNum == iLastInpNum)) {
		if (iInputLine == iLastInpLine) // already read in
			return;
		if (iLastInpLine == iInputLine - 1) { // roulate buffers and only read last line
			for (int i=0; i < iGrpFac-1; i++)
				Swap(bufListValRaw[i], bufListValRaw[i+1]);
			ReadLineRaw(iInputLine+iGrpFac-1, bufListValRaw[iGrpFac-1], iInputFrom, iInputNum);
			iLastInpLine = iInputLine;
			return;
		}
	}
	// read all lines
	for (int i=0; i < iGrpFac; i++)
		ReadLineRaw(iInputLine+i, bufListValRaw[i], iInputFrom, iInputNum);
	iLastInpLine = iInputLine;
	iLastInpFrom = iInputFrom;
	iLastInpNum = iInputNum;
}

String MapAggregate::sExpression() const
{
	String sAgg;
	if (0 != agf) {
		sAgg = agf->sName();
		sAgg = sAgg.sRight(sAgg.length()-3);
	}
	String sGrp = "Group";
	if (!fGrpPixels)
		sGrp = "NoGroup";
	String sOff;
	if ((iRowOffset != 0) || (iColOffset != 0))
		sOff = String(",%li,%li", iRowOffset, iColOffset);
	return String("MapAggregate%S(%S,%li,%S%S)", sAgg, mp->sNameQuoted(true, fnObj.sPath()), iGrpFac, sGrp, sOff);
}


bool MapAggregate::fDomainChangeable() const
{
	if (0 == agf)
		return false;
	String sAggName = agf->sName();
	return (fCIStrEqual(sAggName,"aggsum") || fCIStrEqual(sAggName, "aggavg") 
		|| fCIStrEqual(sAggName,"aggstd") || fCIStrEqual(sAggName,"aggcnt"));
}

bool MapAggregate::fValueRangeChangeable() const
{
	return fDomainChangeable();
}




