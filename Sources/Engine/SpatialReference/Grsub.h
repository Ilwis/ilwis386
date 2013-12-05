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
/* GeoRefSubMap
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  WK   11 Jun 98    2:03 pm
*/


#ifndef ILWGRSUBMAP_H
#define ILWGRSUBMAP_H
#include "Engine\SpatialReference\Gr.h"


class GeoRefSubMap: public GeoRefPtr
{
	friend class GeoRef;
	friend class GeoRefPtr;
protected:
	GeoRefSubMap(const FileName& fn);
public:
	_export GeoRefSubMap(const FileName& fn, const GeoRef& grf, RowCol rcStart, RowCol rcSize);
	static GeoRefPtr* create(const FileName& fn);

	RowCol _export rcTopLeft() const;
	void _export SetTopLeft(RowCol rcTopLeft);
	void _export SetRowCol(RowCol rcSize);

	virtual String sType() const;
	virtual void Store();
	virtual void Coord2RowCol(const Coord& c, double& rRow, double& rCol) const;
	virtual void RowCol2Coord(double rRow, double rCol, Coord& c) const;
	virtual double rPixSize() const;
	virtual bool fEqual(const IlwisObjectPtr& ptr) const;
	virtual bool fLinear() const;
	virtual bool fNorthOriented() const;
	virtual bool fDependent() const; 
	void GetObjectStructure(ObjectStructure& os);
	virtual bool fGeoRefNone() const;
	GeoRef grfParentGeoRef() { return gr;}
	void BreakDependency();
protected:
	GeoRef gr;
	RowCol rcStart;
};

class GeoRefSubMapCorners: public GeoRefSubMap
{
  friend class GeoRefSubMap;
protected:
	GeoRefSubMapCorners(const FileName& fn);
public:
	_export GeoRefSubMapCorners(const FileName& fn, const GeoRef& grf, RowCol rcStart, RowCol rcBotRight);
	virtual String sType() const;
	virtual void Store();
  void SetBotRight(const RowCol& rc) { _rcBotRight = rc; }
  RowCol rcBotRight() const { return _rcBotRight; }
	virtual bool fEqual(const IlwisObjectPtr& ptr) const;
private:
  RowCol _rcBotRight;
};

class GeoRefSubMapCoords: public GeoRefSubMap
{
  friend class GeoRefSubMap;
protected:
	GeoRefSubMapCoords(const FileName& fn);
public:
	_export GeoRefSubMapCoords(const FileName& fn, const GeoRef& grf, const Coord& crd1, const Coord& crd2);
	virtual String sType() const;
	virtual void Store();
  _export void SetCrd1(const Coord& crd);
  _export void SetCrd2(const Coord& crd);
  Coord crd1() const { return _crd1; }
  Coord crd2() const { return _crd2; }
	virtual bool fEqual(const IlwisObjectPtr& ptr) const;
private:
  void CalcSize(const Coord& crd1, const Coord& crd2);
  Coord _crd1, _crd2;
};

#endif




