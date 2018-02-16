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
// MapCatchmentMerge.h: interface for the MapCatchmentMerge class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ILWMAPCATCHMENTMERGE_H
#define ILWMAPCATCHMENTMERGE_H

#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Table\Col.h"
#include "Engine\Map\Point\PNT.H"
#include "LargeVector.h"

#define sStrahler  "Strahler"
#define	sStreve  "Streve"

struct OutletLocation
{
		OutletLocation() {}
		long   StreamID; 
		RowCol rc;
		long	 StreveOrder;
		double rLen1;
		double rLen2;
		bool   isOnNode;
		bool   fExtractOverlandFlowPath;  
};

struct DrainageAtt
{
		DrainageAtt() {}
		long ID;
		vector<long> UpstreamID;
		RowCol DownStreamCoord;
		RowCol UpstreamCoord;
    RowCol TostreamCoord;
		long iDownStreamID;
		long iOrder;
		long CatchmentLink;
    double rLenght;
};

struct AttLongestPath
{
  Coord UpstreamCoord;
  Coord DownstreamCoord;
  double rLength;
};

IlwisObjectPtr * createMapCatchmentMerge(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class MapCatchmentMerge : public MapFromMap  
{
	friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapCatchmentMerge* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapCatchmentMerge(const FileName& fn, MapPtr& p);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
protected:
	virtual void Store();
	
  MapCatchmentMerge(const FileName& fn, MapPtr& p,
										const Domain& dm,		 	
										const Map& mpNetworkOrder,
										const Map& mpFlow,
										const Map& mpFAcc,
										const Map& mpDEM,
										const PointMap& pmpOutlets,
										bool bUndefined,
										bool bExtractStreams);
  
  MapCatchmentMerge(const FileName& fn, MapPtr& p,
										const Domain& dm,		 	
										const Map& mpNetworkOrder,
										const Map& mpFlow,
										const Map& mpFAcc,
										const Map& mpDEM,
										const PointMap& pmpOutlets,
										bool bUndefined,
										bool bExtractStreams,
										String LongestFlowPathSegmentMap);
  
  MapCatchmentMerge(const FileName& fn, MapPtr& p,
										const Domain& dm,
										const Map& mpNetworkOrder,
										const Map& mpFlow,
										const Map& mpDEM,
										const Map& mpFAcc,
										String  sOrderSystem,
										long    iStreamOrders,
										bool	bExtractOriginalOrder);
	
	MapCatchmentMerge(const FileName& fn, MapPtr& p,
										const Domain& dm,
										const Map& mpNetworkOrder,
										const Map& mpFlow,
										const Map& mpFAcc,
										const Map& mpDEM,
										String     sOrderSystem,
										long       iStreamOrders,
										bool	bExtractOriginalOrder,
										String LongestFlowPathSegmentMap);
                    
  ~MapCatchmentMerge();
	
private:
	LargeVector<LongBuf>   m_vStreamMap;   
	LargeVector<ByteBuf>   m_vFlowDir;      //vector for input flow direction 
	LargeVector<LongBuf>	  m_vOutput;
	LargeVector<LongBuf>   m_vFAcc;
	vector<OutletLocation> m_vOutlet;
	Table m_tbl;  //attribute table associated with the output map
	Domain m_dm;

	bool m_UseOutlets;
	bool m_fExtractStreams;
	bool m_fUndefined;
	long m_iOutletVal;
	String m_sOrderSystem;
	String m_sMergeOption;
	String m_sLongestFlowPathSegmentMap;
	long m_iMaxOrderNumber;
	bool m_bExtractOriginalOrder;
	double m_rSourceWaterFlowPathLen;
	Map m_mpFlow;
	Map m_mpFacc;
	Map m_mpDEM;
	PointMap m_pmpOutlets;
	SegmentMap m_segLongestSegmap;
	long m_iStreamOrders;
	String m_sUpLinkCatchment;
	vector<long> m_vUpCatchmentIDs;
	vector<long> m_vStreamsInCatchment;
	vector<DrainageAtt> m_vDrainageAtt;
	vector<Coord> m_vStreamCoord;	//store coordinate of cells in a linksplited   	
	vector<Coord> m_vStream;
	vector<byte> m_vFlowSelf;
	void init();
	void InitPars();
	void InitInOutMaps();
	void WriteOutputRaster();
	void InitOutletVector();
	void CreatePolygonMap(FileName fn);
	void GeneratePolygonStatistics(FileName fn, Table&);
	SegmentMap ExtractSegments();
	void ReadFlowAccumulation();
	void SortOutlets();
	RowCol GetRowCol(Column, long);
	bool IsEdgeCell(long iRow, long iCol);
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
	void AddDomainItem(Domain dm,long item);
	long MergeCatchment(long iRow, long iCol, long iFlag, boolean fExtractOriginal, RowCol, vector<long> UpstreamID);
	void AddLink2StreamSegments();
	void UpdateLink2StreamSegments(long iCatchmentID, OutletLocation ol);
	void CreateTable();
	void BuildUpLinkCatchment(long in, long jn, int iFlow, long iFlag);
	void IdentifyStreamsInCatchment(long in, long jn);
	void UpdateUpLinkCatchment(long id);
	void UpdateDownLinkCatchment(long id);
	bool fLatLonCoords();
	void ComputeCatchmentArea(Table tbl);
	void ComputeTotalUpStreamCatchmentArea(Table tbl);
  
	void ComputeOtherAttributes();
	void ComputeCenterPolygon(FileName fn);
	void EvaluateJointOutletsbyOrder();
	long FindDrainageIndex(long iDrainageID); 
	void MergeCatchmentsRemained(long& id);
	void ExtractOriginalOrder(long& id);
	void PutOutlet(long id, RowCol rc, long iFlag, bool);
	bool IsJointOutlet(long downstreamid);
	bool IsUpstreamsMerged(vector<long> vUpStreamLink);
	void Merge(long id, DrainageAtt datt, bool fExtractOriginalOrder);
	String BuildMaskIDs();

	double CalculateLength(RowCol, RowCol, long);
	double GetSplitSegmentLength(long iStreamID);
	double rDistance(Coord cd1, Coord cd2);
	bool fEllipsoidalCoords();
	int  GetDownStreamCell(RowCol& rc);
	bool fRelocatOutlet(RowCol& rc, int);
	void CleanSegment(SegmentMap smpTo, SegmentMap smpFrom);
	void CreateTableSegmentsExtracted(SegmentMap sm);
	vector<long> GetSegmentIDsExtracted();
	void ExtractUpstreamFlowPath(RowCol, long);
	void StoreSourceSegment(long);
	Coord StoreSegment(SegmentMap,long, long);
	Coord SplitSegment(SegmentMap,long,double,long, RowCol);
	Coord ComputeCenterDrainage(long, double, SegmentMap);
	void CreateTableLongestFlowPath(FileName fn, vector<AttLongestPath>);
	double rComputeSinuosity(double, double);
	double rGetElevation(RowCol);
	Column m_cUpstreamCoord;
};

#endif // ILWMapCatchmentMerge_H
