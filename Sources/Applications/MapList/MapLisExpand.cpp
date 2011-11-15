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
// MapListExpand.cpp: implementation of the MapListExpand class.
//
//////////////////////////////////////////////////////////////////////

#include "headers\toolspch.h"
#include "Engine\Applications\MapListVirtual.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\MapList\MapListExpand.h"
#include "Headers\Hs\maplist.hs"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace {
	const char* sSyntax() 
	{
		return "MapListExpand(inpmaplist, extra, method)\n ";
	}
}

IlwisObjectPtr * createMapListExpand(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *) MapListExpand::create(fn, (MapListPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapListExpand(fn, (MapListPtr &)ptr);
}

String wpsmetadataMapListExpand() {
	WPSMetaData metadata("MapListExpand");
	return metadata.toString();
}

ApplicationMetadata metadataMapListExpand(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapListExpand();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotMAPLIST;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  sSyntax();

	return md;
}

MapListExpand::MapListExpand(const FileName& fn, MapListPtr& ptr)
: MapListVirtual(fn, ptr, false)
{
	ReadElement("MapListExpand", "Maplist", mpl1);
	ReadElement("MapListExpand", "Extra", extra);
	long m;
	ReadElement("MapListExpand", "Method", m);
	method = (MapListExpand::Method)m;
	Init();
}

MapListExpand::MapListExpand(const FileName& fn, MapListPtr& _ptr,const MapList& mplIn, int extr, MapListExpand::Method& m) :
MapListVirtual(fn, _ptr, mplIn->gr(),mplIn->rcSize(), mplIn->iSize(),""), 
mpl1(mplIn),
extra(extr),
method(m)
{
}

MapListVirtual *MapListExpand::create(const FileName& fn, MapListPtr& ptr, const String& sExpr)
{
	Array<String> as;
	short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms != 3)
		ExpressionError(sExpr, sSyntax());
	MapList mplIn = MapList(FileName(as[0],".mpl"));
	int extra = as[1].iVal();
	if ( extra <= 0)
		ExpressionError(sExpr, sSyntax());
	MapListExpand::Method m;
	if ( as[2].toLower() == "max" )
		m = MapListExpand::mMax;
	if ( as[2].toLower() == "min" )
		m = MapListExpand::mMin;
	if ( as[2].toLower() == "half" )
		m = MapListExpand::mHalf;
	if ( as[2].toLower() == "linear" )
		m = MapListExpand::mLinear;
	return new MapListExpand(fn ,ptr, mplIn, extra, m);
}

MapListExpand::~MapListExpand()
{
}

void MapListExpand::Store()
{
	MapListVirtual::Store();
	WriteElement("MapListVirtual", "Type", "MapListExpand");
	WriteElement("MapListExpand", "MapList", mpl1);
	WriteElement("MapListExpand", "Extra", extra);
	WriteElement("MapListExpand", "Method", method);
}

void MapListExpand::Init()
{
	sFreezeTitle = "MapListExpand";
}

String MapListExpand::sExpression() const
{
	String sM;
	if ( method == MapListExpand::mMax)
		sM = "max";
	if ( method == MapListExpand::mMin)
		sM = "min";
	if ( method == MapListExpand::mHalf)
		sM = "half";
	if ( method == MapListExpand::mLinear)
		sM = "linear";

	return String("MapListExpand(%S, %d,%S)", mpl1->sName(), extra,  sM);
}


bool MapListExpand::fFreezing()
{ 
	trq.SetText(TR("Calculating Expanded list"));
	RangeReal rrList = mpl1->getRange();

	double rStep = mpl1[0]->dvrs().rStep();
	ValueRangeReal range(rrList.rLo() - rrList.rHi(), rrList.rHi() + rrList.rHi(),rStep);
	DomainValueRangeStruct dvrs(Domain("value"), range);

	for(int i=0; i< mpl1->iSize() * (extra + 1); ++i) {
		FileName fn(String("%S_%d.mpr",ptr.fnObj.sFile, i));

		Map mp(fn, mpl1->gr(), mpl1->rcSize(),dvrs);
		ptr.AddMap(mp);
	}

	for(int i=0; i< mpl1->iSize(); ++i) {
		RealBuf buf1(mpl1->rcSize().Col);
		Map mpIn1 = mpl1[i];
		vector<Map> extraMaps;
		for(int ex=0; ex < extra; ++ex)
			extraMaps.push_back(this->map(1 + i * (extra + 1) + ex));
		Map mpOut = this->map(i * (extra + 1));
		for(int line = 0; line < mpIn1->rcSize().Row; ++line) {
			if( trq.fUpdate(line,mpIn1->rcSize().Row))
				return false;
			mpIn1->GetLineVal(line, buf1);
			mpOut->PutLineVal(line, buf1);
			for(int k=0; k < extra; ++k) {
				RealBuf buf2(mpIn1->rcSize().Col);
				RealBuf buf3(mpIn1->rcSize().Col);

				if ( line < mpIn1->rcSize().Row)
					mpIn1->GetLineVal(line + 1, buf2);
				else
					mpIn1->GetLineVal(line, buf2);

				for(int p = 0; p < buf2.iSize(); ++p) {
					double v1 = buf1[p];
					double v2 = buf2[p];
					if ( method = MapListExpand::mMax) {
						buf3[p] = rMAX(v1, v2);
					} else if ( method = MapListExpand::mMin) {
						buf3[p] = rMIN(v1, v2);
					} else if( method = MapListExpand::mHalf) {
						if ( k < extra /2)
							buf3[p] = v1;
						else
							buf3[p] = v2;

					} else if( method = MapListExpand::mLinear) {
						double f = (double) k / extra;
						buf3[p] = v1 + f * ( v2 - v1);
					}
				}
				extraMaps[k]->PutLineVal(line, buf3);
			}
		}
	}

return true;
}
