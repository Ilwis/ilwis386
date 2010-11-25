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
// MapAggregateMapList.h: interface for the MapAggregateMapList class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Engine\Applications\MAPvirt.H"
#include "Engine\Map\Raster\MapList\maplist.h"

IlwisObjectPtr * createMapAggregateMapList(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapAggregateMapList : public MapVirtual 
{
public:
	static MapAggregateMapList* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapAggregateMapList(const FileName& fn, MapPtr& ptr);
	MapAggregateMapList(const FileName& fn, MapPtr& ptr,  const MapList& _ml, const String& meth, 
										 const GeoRef& grf, const RowCol& sz, const DomainValueRangeStruct& dvs);
	virtual ~MapAggregateMapList();
  virtual void Store();
  virtual bool fFreezing();
  virtual String sExpression() const;
private:
  void Init();
  //void getColumn(unsigned char *mapBlock, long blockSize, long row, long column, double *col) const;
  void getColumn(unsigned char *mapBlock, long blockSize, long rowSize, long row, long column, vector<double>& col) const;
 // double calcValue(double *column, int sz);
  double calcValue(vector<double>& column, int sz);
  void calcInt(const RowCol& size, long blockSize,unsigned char *mapBlock);
  void calcByte(const RowCol& size, long blockSize,unsigned char *mapBlock);
  void calcReal(const RowCol& size, long blockSize,unsigned char *mapBlock);
  void calcLong(const RowCol& size, long blockSize,unsigned char *mapBlock);
  MapList ml;
  String method;
  int byteSize;
};


