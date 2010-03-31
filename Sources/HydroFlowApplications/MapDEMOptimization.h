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
// MapDEMOptimization.h: interface for the MapDEMOptimization class.
//
//////////////////////////////////////////////////////////////////////

/* DEM Optimization
   Copyright Ilwis System Development ITC
   May 2003,by Lichun Wang
*/
#ifndef ILWMAPDEMOPTIMIZATION_H
#define ILWMAPDEMOPTIMIZATION_H

#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Table\Col.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"

IlwisObjectPtr * createMapDEMOptimization(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class MapDEMOptimization : public MapFromMap  
{
friend MapFromMap;
enum DrainageMapType{dmtSegment, dmtPolygon};
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapDEMOptimization* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapDEMOptimization(const FileName& fn, MapPtr& p);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	virtual bool fValueRangeChangeable() const;
	
protected:
	virtual void Store();
	MapDEMOptimization(const FileName& fn, MapPtr& p, const Map& map, const SegmentMap& sm,
										 String sTable,	
										 String sBufdist,
										 String sSmooth, 
										 String Sharp);
										 
	MapDEMOptimization(const FileName& fn, MapPtr& p, const Map& map, const SegmentMap& sm,
										 double rBufdist,
										 double rSmooth, 
										 double rharp);
										 
	MapDEMOptimization(const FileName& fn, MapPtr& p, const Map& map, const PolygonMap& pm,double rSharp);
										 
	
  ~MapDEMOptimization();
	Map	m_mp;
	SegmentMap m_sm;
	PolygonMap m_pm; 
private:
	bool WriteData(vector<RealBuf>&);
	void Init();
	DrainageMapType   m_DrainageMapType;
	String m_sbm;
	bool	 m_IsUseTable;	
	String m_sTbl;
	String m_sBufdist;
	String m_sSmooth;
	String m_sSharp;
	double m_rBufdist;
	double m_rSmooth;
	double m_rSharp;
};

#endif // ILWMAPDEMOPTIMIZATION_H
