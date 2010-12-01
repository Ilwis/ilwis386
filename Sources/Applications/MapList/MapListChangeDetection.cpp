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
// MapListChangeDetection.cpp: implementation of the MapListChangeDetection class.
//
//////////////////////////////////////////////////////////////////////

#include "headers\toolspch.h"
#include "Engine\Applications\MapListVirtual.h"
#include "Applications\MapList\MapListChangeDetection.h"
#include "Headers\Hs\maplist.hs"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace {
	const char* sSyntax() 
	{
		return "MapListChangeDetection(inpmaplist, baselinemap,threshold,undefhandling)\n \
			   MapListChangeDetection(inpmaplist1, inpmaplist2,threshold, undefhandling)\n \
			   MapListChangeDetection(inpmaplist, threshold, undefhandling)";
	}
}

IlwisObjectPtr * createMapListChangeDetection(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *) MapListChangeDetection::create(fn, (MapListPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapListChangeDetection(fn, (MapListPtr &)ptr);
}

MapListChangeDetection::MapListChangeDetection(const FileName& fn, MapListPtr& ptr)
: MapListVirtual(fn, ptr, false)
{
	ReadElement("MapListChangeDetection", "MapList1", mpl1);
	ReadElement("MapListChangeDetection", "MapList2", mpl2);
	ReadElement("MapListChangeDetection", "BaseLine", mapBaseLine);
	ReadElement("MapListChangeDetection", "Threshold", threshold);
	ReadElement("MapListChangeDetection", "Undefhandling", undefHandling);
	Init();
}

MapListChangeDetection::MapListChangeDetection(const FileName& fn, MapListPtr& _ptr,const MapList& mplIn, const Map& mpBase, double threshld, bool undef) :
MapListVirtual(fn, _ptr, mpBase->gr(),mpBase->rcSize(), mplIn->iSize(),""), 
mpl1(mplIn),
mapBaseLine(mpBase),
threshold(threshld),
undefHandling(undef)
{
}

MapListChangeDetection::MapListChangeDetection(const FileName& fn, MapListPtr& _ptr,const MapList& mplIn1, const MapList& mplIn2, double threshld, bool undef) :
MapListVirtual(fn, _ptr, mplIn1[0]->gr(), mplIn1[0]->rcSize(), mplIn1->iSize(),""), 
mpl1(mplIn1),
mpl2(mplIn2),
threshold(threshld),
undefHandling(undef)
{
}


MapListVirtual *MapListChangeDetection::create(const FileName& fn, MapListPtr& ptr, const String& sExpr)
{
	Array<String> as;
	short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms < 2)
		ExpressionError(sExpr, sSyntax());
	MapList mplIn = MapList(FileName(as[0],".mpl"));
	if ( iParms ==3 ) {
		double threshld = as[1].rVal();
		bool undefs = as[2].fVal();
		return new MapListChangeDetection(fn ,ptr, mplIn, MapList(),threshld,undefs);
	}
	FileName fnInput2(as[1]);
	bool undefs = as[3].fVal();
	double threshld = as[2].rVal();
	bool twoList = false;

	if ( fnInput2.sExt == ".mpl")
		twoList = true;
	else {
		if ( fnInput2.sExt == "") {
			if ( FileName(fnInput2,".mpl").fExist()) {
				fnInput2.sExt = ".mpl";
				
			}
			else
				fnInput2.sExt = ".mpr";
		} else {
			fnInput2.sExt = ".mpr";
		}
	}
	if ( twoList) {
		MapList mpL2(fnInput2);
		if ( mpL2->iSize() != mplIn->iSize())
			throw ErrorObject(TR("Map lists must have the same number of maps"));
		if ( mpL2[mpL2->iLower()]->gr() != mplIn[mplIn->iLower()]->gr())
			throw ErrorObject(TR("Map lists must use the same georef"));
		return new MapListChangeDetection(fn ,ptr, mplIn, mpL2, threshld, undefs);
	}
	else {
		Map mapBase = Map(fnInput2);
		if ( mapBase->gr() != mplIn[mplIn->iLower()]->gr())
			throw ErrorObject(TR("Map lists must use the same georef as baseline map"));
		return new MapListChangeDetection(fn ,ptr, mplIn, mapBase, threshld, undefs);
	}
}

MapListChangeDetection::~MapListChangeDetection()
{
}

void MapListChangeDetection::Store()
{
	MapListVirtual::Store();
	WriteElement("MapListVirtual", "Type", "MapListChangeDetection");
	WriteElement("MapListChangeDetection", "MapList1", mpl1);
	WriteElement("MapListChangeDetection", "MapList2", mpl2);
	WriteElement("MapListChangeDetection", "BaseLine", mapBaseLine);
	WriteElement("MapListChangeDetection", "Threshold", threshold);
	WriteElement("MapListChangeDetection", "Undefhandling", undefHandling);
}

void MapListChangeDetection::Init()
{
	sFreezeTitle = "MapListChangeDetection";
}

String MapListChangeDetection::sExpression() const
{
	if ( mapBaseLine.fValid()) {
		return String("MapListChangeDetection(%S, %S,%f,%s)", mpl1->sName(), mapBaseLine->sName(),  threshold, undefHandling ? "true": "false");
	} else if ( mpl2.fValid()) {
		return String("MapListChangeDetection(%S, %S,%f,%s)", mpl1->sName(), mpl2->sName(), threshold, undefHandling ? "true": "false");
	} else
		return String("MapListChangeDetection(%S,%f,%s)", mpl1->sName(), threshold, undefHandling ? "true": "false");
}


bool MapListChangeDetection::fFreezing()
{ 
	trq.SetText(TR("Calculating difference maps"));
	if ( mapBaseLine.fValid())
		withBaseMap();
	else  if ( mpl2.fValid()) {
		twoMapLists();
	} else {
		oneMapList();
	}

	return true;
}
bool MapListChangeDetection::oneMapList() {

	RangeReal rrList;
	for(int i=0; i< mpl1->iSize(); ++i) {
		RangeReal rrSampled = mpl1[0]->rrMinMaxSampled();
		rrList += rrSampled.rHi();
		rrList += rrSampled.rLo();
	}
	double rStep = mpl1[0]->dvrs().rStep();
	ValueRangeReal range(rrList.rLo() - rrList.rHi(), rrList.rHi() + rrList.rHi(),rStep);
	DomainValueRangeStruct dvrs(Domain("value"), range);
	
	for(int i=0; i< mpl1->iSize() - 1; ++i) {
		FileName fn(String("delta_%S_%d.mpr",ptr.fnObj.sFile, i));

		Map mp(fn, mpl1->gr(), mpl1->rcSize(),dvrs);
		ptr.AddMap(mp);
	}
	for(int i = mpl1->iLower() + 1; i< mpl1->iSize(); ++i) {
		Map mpIn1 = mpl1[i];
		Map mpIn2 = mpl1[i-1];
		Map mpOut = map(i-1);
		trq.SetText(String("Calculating change map %S",mpOut->fnObj.sFile)); 
		RealBuf buf1(mpIn1->rcSize().Col);
		RealBuf buf2(mpIn2->rcSize().Col);
		RealBuf bufOut(mpIn1->rcSize().Col);
		for(int line = 0; line < mpIn1->rcSize().Row; ++line) {
			if( trq.fUpdate(line,mpIn1->rcSize().Row))
				return false;
			mpIn1->GetLineVal(line, buf1);
			mpIn2->GetLineVal(line, buf2);
			for(int col=0; col < mpIn1->rcSize().Col; ++col) {
				double delta;
				double v1 = buf1[col];
				double v2 = buf2[col];
				if ( v1 == rUNDEF || v1 == iUNDEF || v1 == shUNDEF || v2 == rUNDEF || v2 == iUNDEF || v2 == shUNDEF) {
					if ( undefHandling)
						delta = 0;
					else
						delta = rUNDEF;
				}
				else {
					delta = v1 - v2;
					if ( abs(delta) < threshold) {
						delta = 0;
					}
				}
				bufOut[col] = delta;
			}
			mpOut->PutLineVal(line, bufOut);
		}
	}
	return true;
}

bool MapListChangeDetection::withBaseMap() {

	RangeReal rrMap = mapBaseLine->rrMinMaxSampled();
	RangeReal rrList;
	for(int i=0; i< mpl1->iSize(); ++i) {
		RangeReal rrSampled = mpl1[0]->rrMinMaxSampled();
		rrList += rrSampled.rHi();
		rrList += rrSampled.rLo();
	}
	double rStep = min(mapBaseLine->dvrs().rStep(), mpl1[0]->dvrs().rStep());
	ValueRangeReal range(rrMap.rLo() - rrList.rHi(), rrMap.rHi() + rrList.rHi(),rStep);
	DomainValueRangeStruct dvrs(Domain("value"), range);
	
	for(int i=0; i< mpl1->iSize(); ++i) {
		FileName fn(String("delta_%S_%d.mpr",ptr.fnObj.sFile, i));

		Map mp(fn, mpl1->gr(), mpl1->rcSize(),dvrs);
		ptr.AddMap(mp);
	}
	for(int i = mpl1->iLower(); i< mpl1->iSize(); ++i) {
		Map mpIn = mpl1[i];
		Map mpOut = map(i);
		trq.SetText(String("Calculating change map %S",mpOut->fnObj.sFile)); 
		RealBuf buf(mpIn->rcSize().Col);
		RealBuf bufBase(mpIn->rcSize().Col);
		RealBuf bufOut(mpIn->rcSize().Col);
		for(int line = 0; line < mpIn->rcSize().Row; ++line) {
			if( trq.fUpdate(line,mpIn->rcSize().Row))
				return false;
			mpIn->GetLineVal(line, buf);
			mapBaseLine->GetLineVal(line,bufBase);
			for(int col=0; col < mpIn->rcSize().Col; ++col) {
				double delta;
				double v1 = buf[col];
				double v2 = bufBase[col];
				if ( v1 == rUNDEF || v1 == iUNDEF || v1 == shUNDEF || v2 == rUNDEF || v2 == iUNDEF || v2 == shUNDEF) {
					if ( undefHandling)
						delta = 0;
					else
						delta = rUNDEF;
				}
				else {
					delta = v1 - v2;
					if ( abs(delta) < threshold) {
						delta = 0;
					}
				}
				bufOut[col] = delta;
			}
			mpOut->PutLineVal(line, bufOut);
		}
	}
	return true;
}

bool MapListChangeDetection::twoMapLists() {
	RangeReal rrList1;
	for(int i=0; i< mpl1->iSize(); ++i) {
		RangeReal rrSampled = mpl1[0]->rrMinMaxSampled();
		rrList1 += rrSampled.rHi();
		rrList1 += rrSampled.rLo();
	}
	RangeReal rrList2;
	for(int i=0; i< mpl2->iSize(); ++i) {
		RangeReal rrSampled = mpl2[0]->rrMinMaxSampled();
		rrList2 += rrSampled.rHi();
		rrList2 += rrSampled.rLo();
	}
	double rStep = min(mpl2[0]->dvrs().rStep(), mpl1[0]->dvrs().rStep());
	ValueRangeReal range(rrList1.rLo() - rrList2.rHi(), rrList1.rHi() + rrList2.rHi(),rStep);
	DomainValueRangeStruct dvrs(Domain("value"), range);
	for(int i=0; i< mpl1->iSize(); ++i) {
		FileName fn(String("delta_%S_%d.mpr",ptr.fnObj.sFile, i));

		Map mp(fn, mpl1->gr(), mpl1->rcSize(),dvrs);
		ptr.AddMap(mp);
	}
	for(int i = mpl1->iLower(); i< mpl1->iSize(); ++i) {
		Map mpIn1 = mpl1[i];
		Map mpIn2 = mpl2[i];
		Map mpOut = map(i);
		trq.SetText(String("Calculating change map %S",mpOut->fnObj.sFile)); 
		RealBuf buf1(mpIn1->rcSize().Col);
		RealBuf buf2(mpIn2->rcSize().Col);
		RealBuf bufOut(mpOut->rcSize().Col);
		for(int line = 0; line < mpIn1->rcSize().Row; ++line) {
			if( trq.fUpdate(line,mpIn1->rcSize().Row)) 
				return false;
			mpIn1->GetLineVal(line, buf1);
			mpIn2->GetLineVal(line,buf2);
			for(int col=0; col < mpIn1->rcSize().Col; ++col) {
				double delta;
				double v1 = buf1[col];
				double v2 = buf2[col];
				if ( v1 == rUNDEF || v1 == iUNDEF || v1 == shUNDEF || v2 == rUNDEF || v2 == iUNDEF || v2 == shUNDEF) {
					if ( undefHandling)
						delta = 0;
					else
						delta = rUNDEF;
				}
				else {
					delta = v1 - v2;
					if ( abs(delta) < threshold) {
						delta = 0;
					}
				}
				bufOut[col] = delta;
			}
			mpOut->PutLineVal(line, bufOut);
		}
	}
	return true;
}
