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
// MapCatchmentExtraction.h: interface for the MapCatchmentExtraction class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ILWMAPCATCHMENTEXTRACTION_H
#define ILWMAPCATCHMENTEXTRACTION_H

#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Table\Col.h"
#include "Engine\Map\Point\PNT.H"

#define sUpstreamLink  "UpstreamLinkID"
#define	sDownstreamLink  "DownstreamLinkID"
#define	sDownstreamCoord  "DownstreamCoord"

struct AttCols
{
		AttCols() {}
		long         DrainageID; 
		vector<long> UpstreamLink;
		long	 DownstreamLink;
		RowCol DownstreamCoord;
};

struct AttUpstreamLink
{
		AttUpstreamLink() {}
		long         DrainageID; 
		vector<long> UpstreamLink;
};

IlwisObjectPtr * createMapCatchmentExtraction(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class MapCatchmentExtraction : public MapFromMap  
{
	friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapCatchmentExtraction* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapCatchmentExtraction(const FileName& fn, MapPtr& p);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
protected:
	virtual void Store();
	MapCatchmentExtraction(const FileName& fn, MapPtr& p, 
												 const Map& mpDrainage,
												 const Map& mpflowdir);
  ~MapCatchmentExtraction();
	
private:
	vector<LongBuf>   m_vDrainageMap;  //vector for the input drainage map 
	vector<ByteBuf>   m_vFlowDir;      //vector for input flow direction 
	vector<LongBuf>		m_vFlag;         //Flag the identified catchment 
	vector<AttCols> m_vRecords;
	vector<AttUpstreamLink> m_vvUpstreamLinks;	
	Column m_cArea;
	Column m_cPerimeter;
	Table m_tbl;  //attribute table associated with the output map

	Map m_mpFlow;
	bool IsEdgeCell(long iRow, long iCol);
	bool IsNoUpstreams();
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
	void GetAttributes();
	long DelineateCatchment(long iRow, long iCol, long iFlag);
	long FindDownstreamIndex(long DownstreamID);
	void UpdateUpstreamLinkID(long DrainageID, long UpstreamID);
	void EraseDrainage(long DrainageID);
	void ComputeCatchmentAttributes();
	bool fLatLonCoords();
	void ComputeTotalUpstreamArea(DomainSort* pdsrt, Column cArea, Column cTotalArea);
	void ComputerCenterPolygon(FileName fn);
	void ComputeCenterDrainage();
	double GetDistance(RowCol& rc);
	double rDistance(Coord cd1, Coord cd2);
	bool fEllipsoidalCoords();
};

#endif // ILWMapCatchmentExtraction_H
