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
// MapAggregateMapList.cpp: implementation of the MapAggregateMapList class.
//
//////////////////////////////////////////////////////////////////////

#include "headers\toolspch.h"
#include "Engine\Base\System\SysInfo.h"
#include "Applications\Raster\MapAggregateMapList.h"
#include "Headers\Hs\maplist.hs"

namespace {
  const char* sSyntax() 
  {
    return "MapAggregateMapList(maplist,method)";
  }
}

MapAggregateMapList::MapAggregateMapList(const FileName& fn, MapPtr& ptr)
: MapVirtual(fn, ptr)
{
  ReadElement("MapAggregateMapList", "MapList", ml);
  ReadElement("MapAggregateMapList", "Method", method);
  Init();
}

IlwisObjectPtr * createMapAggregateMapList(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *) MapAggregateMapList::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapAggregateMapList(fn, (MapPtr &)ptr);
}

MapAggregateMapList* MapAggregateMapList::create(const FileName& fn, MapPtr& p, const String& sExpr) {
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 2)
    ExpressionError(sExpr, sSyntax());  
  MapList ml = MapList(FileName(as[0]));
  if (!ml.fValid())
    throw ErrorObject(WhatError("MapList expected",0), WhereError(as[0]));
  String method = as[1].toLower();
  Map mp = ml[ ml->iLower()];
  if ( method == "sum"){
	  RangeReal rr = ml->getRange();
	  ValueRangeReal range(rr.rLo() * ml->iSize(), rr.rHi() * ml->iSize(),mp->dvrs().rStep());
	  return new MapAggregateMapList(fn,p,ml,method,mp->gr(), mp->rcSize(),DomainValueRangeStruct(Domain("value"), range));
  } else if ( method == "count"){
	  ValueRangeInt range(0, ml->iSize());
	  return new MapAggregateMapList(fn,p,ml,method,mp->gr(), mp->rcSize(),DomainValueRangeStruct(Domain("value"), range));
  }
  return new MapAggregateMapList(fn,p,ml,method,mp->gr(), mp->rcSize(),mp->dvrs());
}

MapAggregateMapList::MapAggregateMapList(const FileName& fn, MapPtr& ptr,  const MapList& _ml, const String& meth, 
										 const GeoRef& grf, const RowCol& sz, const DomainValueRangeStruct& dvs)
: MapVirtual(fn, ptr, grf, sz, dvs), method(meth), ml(_ml)
{
	sFreezeTitle = "MapAggregateMapList";
}

MapAggregateMapList::~MapAggregateMapList()
{
}

void MapAggregateMapList::Store()
{
  MapVirtual::Store();
  WriteElement("MapVirtual", "Type", "MapAggregateMapList");
  WriteElement("MapAggregateMapList", "MapList", ml);
  WriteElement("MapAggregateMapList", "Method", method);
}

void MapAggregateMapList::Init()
{
  
}

String MapAggregateMapList::sExpression() const
{
  return String("MapAggregateMapList(%S,%S)", ml->sName(), method);
}

bool MapAggregateMapList::fFreezing()
{ 
  unsigned char *mapRow = 0;
  try {
    trq.SetText(TR("Constructing"));
	Map mp = ml[ ml->iLower()];
	RowCol size = mp->rcSize();
	storeType = mp->st();
	switch(storeType) {
		case stBYTE:
			byteSize = 1; break;
		case stINT:
			byteSize = 2; break;
		case stLONG:
			byteSize = 4; break;
		case stREAL:
			byteSize = 8; break;
		default:
			throw ErrorObject(TR("Store type not handled by this application"));
	}
	__int64 rowSize = size.Col * byteSize ;
	__int64 szAllRows = rowSize * ml->iSize();
	// input
	mapRow = new unsigned char[szAllRows];
	// intermediate
	vector<double> col(ml->iSize());	
	// outputs
	ByteBuf bufb(size.Col);
	IntBuf bufi(size.Col);
	LongBuf bufl(size.Col);
	RealBuf bufr(size.Col);

	for(int r = 0 ; r < size.Row;++r) {
		trq.fUpdate(r, size.Row);
		__int64 offset = 0;
		for(int i = ml->iLower(); i < ml->iSize(); ++i) {
			Map mp = ml[i];
			FileName fnData(mp->fnObj,".mp#");
			FILE *fp = fopen(fnData.sFullPath().c_str(),"r");
			_fseeki64(fp, (__int64) (r * rowSize), SEEK_SET);
			fread((void *)(mapRow + offset),1,rowSize, fp);
			offset += rowSize;
			fclose(fp);
		}
		if ( method == "sum"){
			calcSum(size, rowSize, mapRow, bufr, col);
			pms->PutLineVal(r,bufr);
		} else if ( method == "count"){
			calcLong(size, rowSize, mapRow, bufl, col);
			pms->PutLineVal(r,bufl);
		} else if ( storeType == stBYTE) {
			calcByte(size, rowSize, mapRow, bufb, col);
			pms->PutLineRaw(r,bufb);
		} else if ( storeType == stINT) {
			calcInt(size, rowSize, mapRow, bufi, col);
			pms->PutLineRaw(r,bufi);
		} else if ( storeType == stLONG) {
			calcLong(size, rowSize, mapRow, bufl, col);
			pms->PutLineRaw(r,bufl);
		} else if ( storeType == stREAL) {
			calcReal(size, rowSize, mapRow, bufr, col);
			pms->PutLineVal(r,bufr);
		}
	}

	delete [] mapRow;
  }
  catch (ErrorObject& err)
  {
	delete [] mapRow;
    err.Show();
    return false;
  }
  return true;
}

void MapAggregateMapList::calcSum(const RowCol& size, long rowSize,unsigned char *mapRow, RealBuf & bufr, vector<double> & col) {
	for(int c = 0; c < size.Col; ++c) {
		getColumn(mapRow,rowSize,c,col);
		double v = calcValue(col, ml->iSize());
		if ( storeType != stREAL){
			v = dvrs().rValue(v);
		}
		bufr[c] = v;
	}
}

void MapAggregateMapList::calcInt(const RowCol& size, long rowSize,unsigned char *mapRow, IntBuf & bufi, vector<double> & col) {
	for(int c = 0; c < size.Col; ++c) {
		getColumn(mapRow,rowSize,c,col);
		double v = calcValue(col, ml->iSize());
		bufi[c] = v;
	}
}

void MapAggregateMapList::calcByte(const RowCol& size, long rowSize,unsigned char *mapRow, ByteBuf & bufb, vector<double> & col) {
	for(int c = 0; c < size.Col; ++c) {
		getColumn(mapRow,rowSize,c,col);
		double v = calcValue(col, ml->iSize());
		if ( v > 255)
			v = 255;
		bufb[c] = v;
	}
}

void MapAggregateMapList::calcReal(const RowCol& size, long rowSize,unsigned char *mapRow, RealBuf & bufr, vector<double> & col) {
	for(int c = 0; c < size.Col; ++c) {
		getColumn(mapRow,rowSize,c,col);
		double v = calcValue(col, ml->iSize());
		bufr[c] = v;
	}
}

void MapAggregateMapList::calcLong(const RowCol& size, long rowSize,unsigned char *mapRow, LongBuf & bufl, vector<double> & col) {
	for(int c = 0; c < size.Col; ++c) {
		getColumn(mapRow,rowSize,c,col);
		double v = calcValue(col, ml->iSize());
		if ( v >= 2147483648)
			v = iUNDEF;
		bufl[c] = v;
	}
}

double MapAggregateMapList::calcValue(vector<double>& column, int sz) {
//double MapAggregateMapList::calcValue(double *column, int sz) {
	int count = 0;
	if ( method == "average") {
		double sum =0;
		for(int i=0; i < sz; ++i) {
			double v = column[i];
			if ( v == iUNDEF || v == rUNDEF || v == shUNDEF)
				continue;
			sum += column[i];
			++count;
		}
		if ( count > 0)
			return sum / count;
	} else 	if ( method == "minimum") {
		double min = 3e100;
		for(int i=0; i < sz; ++i) {
			double v = column[i];
			if ( v == iUNDEF || v == rUNDEF || v == shUNDEF)
				continue;
			if ( v < min) {
				min = v;
			}
		}
		return min;
	} else 	if ( method == "maximum") {
		double max = -3e100;
		for(int i=0; i < sz; ++i) {
			double v = column[i];
			if ( v == iUNDEF || v == rUNDEF || v == shUNDEF)
				continue;
			if ( v > max) {
				max = v;
			}
		}
		return max;
	} else 	if ( method == "median") {
		set<double> numbers;
		for(int i=0; i < sz; ++i) {
			double v = column[i];
			if ( v == iUNDEF || v == rUNDEF || v == shUNDEF)
				continue;
				
			numbers.insert(v);
		}
		if ( numbers.size() > 0) {
			int count=0;
			for(set<double>::iterator cur=numbers.begin(); cur != numbers.end(); ++cur){
				if ( count >= numbers.size() / 2)
					return ( *cur);
				++count;
			}
		}

	}else  if ( method == "sum") {
		double sum = 0;
		for(int i=0; i < sz; ++i) {
			double v = column[i];
			if ( v == iUNDEF || v == rUNDEF || v == shUNDEF)
				continue;
				
			sum += v;
		}
		return sum;
		
	}else  if ( method == "count") {
		for(int i=0; i < sz; ++i) {
			double v = column[i];
			if ( v == iUNDEF || v == rUNDEF || v == shUNDEF)
				continue;
			++count;
		}
		return count;
	}
			
	return rUNDEF;
}

void MapAggregateMapList::getColumn(unsigned char *mapRow, long rowSize, long col, vector<double>& column) const{
//void MapAggregateMapList::getColumn(unsigned char *mapBlock, long blockSize, long row, long col, double *column) const{
	for(int i =ml->iLower(); i < ml->iSize(); ++i) {
		unsigned char *p = (mapRow + i * rowSize + col * byteSize);

		switch (byteSize) {
			case 1:
				column[i] = *((unsigned char *)p); break;
			case 2:
				column[i] = *((short *)p); break;
			case 4:
				column[i] = *((long *)p);break;
			case 8:
				column[i] = *((double *)p);break;
		}
	}
}
