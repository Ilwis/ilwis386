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
// MapDrainageNetworkOrdering.h: interface for the MapDrainageNetworkOrdering class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ILWMAPDRAINAGENETWORKORDERING_H
#define ILWMAPDRAINAGENETWORKORDERING_H

#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\csviall.h"
#include "LargeVector.h"

IlwisObjectPtr * createMapDrainageNetworkOrdering(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

struct RecordObj
{
		RecordObj() {}
		long   id;
		String UpstreamLink;
		RowCol  UpstreamCoord;
		long	 DownstreamLink;
		RowCol  DownstreamCoord;
		long   Strahler;
		long   Streve;
    double  Length;
		double  TotalUpstreamLength;
    RowCol  TostreamCoord;
};

#define iDrainage -1
#define iOffDrainage -2 
#define iJunction 1
#define iInValidFlow 0

class MapDrainageNetworkOrdering : public MapFromMap  
{
	friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapDrainageNetworkOrdering* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapDrainageNetworkOrdering(const FileName& fn, MapPtr& p);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
protected:
	virtual void Store();
    MapDrainageNetworkOrdering(const FileName& fn, MapPtr& p, const Map& map,
														 const Map& mpflowdir,
														 const Map& mpDrainage,
														 double rMinlen,
														 const Domain& dm);
  ~MapDrainageNetworkOrdering();
	
private:
	LargeVector<RealBuf>   m_vDem;
	LargeVector<LongBuf>   m_vDrainageMap;  //vector for the input drainage map 
	LargeVector<ByteBuf>   m_vFlowDir;  //vector for input flow direction 
	vector<RowCol>    m_vStreamLink;	//store the location of cells in a link  	
	vector<RowCol>    m_vJunction;	//store the location of junctions in the network   	
	RecordObj         m_rec;
	vector<RecordObj> m_vRecords;
	double						m_rMinLen;	//the minimum drainage length
	long						  m_iLinkNumber; //link identification number
	double m_rLength;
	long   m_iStrahler;
	long   m_iStreve;
	double m_rTotalUpstreamLength;
	String m_sUpstreamLink;
	

	Map m_mpFlow;
	Map m_mpDrainage;
	Domain m_dm;
	void IniParms();
	bool IsSourceCell(long iRow, long iCol);
	int  IdentifyLink (RowCol& rc);
	void FlagDrainageMap(long iFlag, int iLinkType);
	void UpdateRecord();
	bool IsJunctionExists(RowCol);
	bool IsEdgeCell(long iRow, long iCol);
	bool IsJunction(RowCol rc);
	void StoreJunction(RowCol);
	void EraseJunction(RowCol rc);
	bool IsAllUpstreamIdentified(RowCol node, vector<long>& vUpstreamPos);  
	int  GetDownStreamCell(RowCol& rc);
	long GetUpstream(RowCol rc);
	void DownstreamLink(vector<long>);
	long FindDrainageIndex(long iDrainageID);
	bool isEven(int elem);
	void AddDomainItem(long iItem );
	void CreateTable();
	void StoreSegment(SegmentMap sm, long iSegID, bool fConvert );
	void PatchSegment(SegmentMap sm, long iSegID);
	void CleanSegment(SegmentMap smpTo, SegmentMap smpFrom);

	long ComputeStrahlerOrder(vector<long> vUpstreamLink);
	long ComputeStreveOrder(vector<long> vUpstreamLink);
	double rComputeSlope(double rDrop, double rLength, bool fDegree);
	double rDistance(Coord cd1, Coord cd2);
	double rComputeSinuosity(double, double);
	bool fLatLonCoords();
	bool fEllipsoidalCoords();
	
};

#endif // ILWMapDrainageNetworkOrdering_H
