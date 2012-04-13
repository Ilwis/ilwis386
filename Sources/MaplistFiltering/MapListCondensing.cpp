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

Created on: 2011-11-8
***************************************************************/
// MapListCondensing.cpp: implementation of the MapListCondensing class.
//
//////////////////////////////////////////////////////////////////////

#include "headers\toolspch.h"
#include "Engine\Applications\MapListVirtual.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "MapListCondensing.h"
#include "Headers\Hs\maplist.hs"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace {
	const char* sSyntax() 
	{
		return "MapListCondensing(inpmaplist, firstmap, extra, method)\n ";
	}
}

IlwisObjectPtr * createMapListCondensing(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ){
	if ( sExpr != "")
		return (IlwisObjectPtr *) MapListCondensing::create(fn, (MapListPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapListCondensing(fn, (MapListPtr &)ptr);
}

String wpsmetadataMapListCondensing() {
	WPSMetaData metadata("MapListCondensing");
	return metadata.toString();
}

ApplicationMetadata metadataMapListCondensing(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapListCondensing();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotMAPLIST;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  sSyntax();

	return md;
}

MapListCondensing::MapListCondensing(const FileName& fn, MapListPtr& ptr)
: MapListVirtual(fn, ptr, false)
{
	ReadElement("MapListCondensing", "Maplist", mpl1);
	ReadElement("MapListCondensing", "StartIndex", startIndex);
	ReadElement("MapListCondensing", "Step", step);
	long m;
	ReadElement("MapListCondensing", "Method", m);
	method = (MapListCondensing::Method)m;
	Init();
}

MapListCondensing::MapListCondensing(const FileName& fn, MapListPtr& _ptr,const MapList& mplIn, int start, int extr, MapListCondensing::Method& m):
MapListVirtual(fn, _ptr, mplIn->gr(),mplIn->rcSize(), mplIn->iSize(),""), 
mpl1(mplIn),
startIndex(start),
step(extr),
method(m)
{
}

MapListVirtual *MapListCondensing::create(const FileName& fn, MapListPtr& ptr, const String& sExpr)
{
	Array<String> as;
	short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms != 4)
		ExpressionError(sExpr, sSyntax());
	MapList mplIn = MapList(FileName(as[0],".mpl"));
	int start = as[1].iVal();
	if ( start < 0 || start > mplIn->iSize())
		throw ErrorObject(TR("Incorrect start index"));
	int step = as[2].iVal();
	if ( step <= 0)
		ExpressionError(sExpr, sSyntax());
	MapListCondensing::Method m;
	if ( as[3].toLower() == "max" )
		m = MapListCondensing::mMax;
	if ( as[3].toLower() == "min" )
		m = MapListCondensing::mMin;
	if ( as[3].toLower() == "mean" )
		m = MapListCondensing::mMean;
	if ( as[3].toLower() == "pred" )
		m = MapListCondensing::mPred;
	if ( as[3].toLower() == "median" )
		m = MapListCondensing::mMedian;
	return new MapListCondensing(fn ,ptr, mplIn, start,  step, m);
}

MapListCondensing::~MapListCondensing()
{
}

void MapListCondensing::Store()
{
	MapListVirtual::Store();
	WriteElement("MapListVirtual", "Type", "MapListCondensing");
	WriteElement("MapListCondensing", "MapList", mpl1);
	WriteElement("MapListCondensing", "StartIndex", startIndex);
	WriteElement("MapListCondensing", "Step", step);
	WriteElement("MapListCondensing", "Method", method);
}

void MapListCondensing::Init()
{
	sFreezeTitle = "MapListCondensing";
}

String MapListCondensing::sExpression() const
{
	String sM;
	if ( method == MapListCondensing::mMax)
		sM = "max";
	if ( method == MapListCondensing::mMin)
		sM = "min";
	if ( method == MapListCondensing::mMean)
		sM = "mean";
	if ( method == MapListCondensing::mPred)
		sM = "pred";
	if ( method == MapListCondensing::mMedian)
		sM = "median";

	return String("MapListCondensing(%S, %d, %d,%S)", mpl1->sName(),startIndex, step,  sM);
}
String MapListCondensing::sMethodToString(Method method)
{
	switch(method)
	{
	case mMax:
		return "max";
	case mMin:
		return "min";
	case mMean:
		return "mean";
	case mPred:
		return "pred";
	case mMedian:
		return "median";
	default:
		return "unknown";
	}
}

bool MapListCondensing::fFreezing()
{ 
	trq.SetText(TR("Constructing"));
	//Map mp = mpl1[ mpl1->iLower()];
	int a=0;
	int mapInIdx = startIndex;

	RangeReal rrList;
	for (int i=0; i < mpl1->iSize(); ++i) {
		RangeReal rrSampled = mpl1[i]->rrMinMaxSampled();
		rrList += rrSampled.rLo();
		rrList += rrSampled.rHi();
	}
	double rStep = mpl1[0]->dvrs().rStep();
	ValueRangeReal range (rrList.rLo(), rrList.rHi(), rStep);
	DomainValueRangeStruct dvrs (Domain("value"), range);


	RowCol size = mpl1->rcSize();

	vector<RealBuf> buffers(step);
	for(int i = 0; i < buffers.size(); ++i)
		buffers[i].Size(size.Col);
	int maxMaps = ( mpl1->iSize()  - mapInIdx)/ step;
	for(int i = 0; i < maxMaps; ++i) {
		FileName fn(String("aggr_%S_%d_%d.mpr", sMethodToString(method),step,i));
		ptr.AddMap(Map(fn, mpl1->gr(), mpl1->rcSize(), dvrs));
	}
	for( int y =0; y < size.Row; ++y) {
		int j = 0;
		for(int i = mapInIdx; i < mpl1->iSize(); ++i) {
			int index = i - mapInIdx;
			if ( (index  % step) != 0 || index == 0) {
				Map mp = mpl1[i];
				RealBuf& buf = buffers.at(j++);
				mp->GetLineVal(y,buf);
			} else {
				j = 0;
				RealBuf bufOut(size.Col);
				for( int x = 0; x < size.Col; ++x) {
					switch (method) {
						case mMean:
							calcMean(buffers, x, bufOut); break;
						case mMedian:
							calcMedian(buffers, x, bufOut); break;
						case mMax:
							calcMax(buffers, x, bufOut); break;
						case mMin:
							calcMin(buffers, x, bufOut); break;
					}
				} 
				int outIndex = index / step;
				if ( outIndex < maxMaps)
					ptr.map(outIndex)->PutLineVal(y, bufOut);
			}
		}
	}

	return true;
}

void MapListCondensing::calcMean(vector<RealBuf>& buffers, int x, RealBuf& bufOut) {
	bufOut[x] = 0;
	double div = step;
	for(int b = 0; b < step; ++b) {
		double v = buffers[b][x];
		if (  v != rUNDEF) {
			bufOut[x] += v;
		} else {
			div--;
		}
	}
	if ( div > 0)
		bufOut[x] /= div;
	else
		bufOut[x] = rUNDEF;
}

void MapListCondensing::calcMedian(vector<RealBuf>& buffers, int x, RealBuf& bufOut) {
	bufOut[x] = 0;
	vector<double> values;
	for(int b = 0; b < step; ++b) {
		double v = buffers[b][x];
		values.push_back(v);
	}
	std::sort(values.begin(), values.end());
	bufOut[x] = values[ values.size() / 2];
}

void MapListCondensing::calcMax(vector<RealBuf>& buffers, int x, RealBuf& bufOut) {
	bufOut[x] = 0;
	double rMaxV = rUNDEF;
	for(int b = 0; b < step; ++b) {
		double v = buffers[b][x];
		rMaxV = max(rMaxV, v);
	}
	bufOut[x] = rMaxV;
}

void MapListCondensing::calcMin(vector<RealBuf>& buffers, int x, RealBuf& bufOut) {
	bufOut[x] = 0;
	double rMinV = 1e308;
	for(int b = 0; b < step; ++b) {
		double v = buffers[b][x];
		rMinV = min(rMinV, v);
	}
	bufOut[x] = rMinV;
}