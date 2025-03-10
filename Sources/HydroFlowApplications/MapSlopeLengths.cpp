/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
// MapSlopeLengths.cpp: implementation of the MapSlopeLengths class.
//
//////////////////////////////////////////////////////////////////////

#include "HydroFlowApplications\MapSlopeLengths.h"
#include "Engine\Table\tbl.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

const double rDefaultEarthRadius = 6371007.0;

using namespace std;

IlwisObjectPtr * createMapOverlandFlowLength(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapOverlandFlowLength::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapOverlandFlowLength(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapOverlandFlowLength::MapOverlandFlowLength(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{

	ReadElement("OverlandFlowLength", "FlowDirection", m_mpFlow);
	CompitableGeorefs(fn, mp, m_mpFlow);
	
	init();
	fNeedFreeze = true;
  sFreezeTitle = "MapOverlandFlowLength";
	//htpFreeze = "ilwisapp\\overland_flow_length_algorithm.htm";
}

MapOverlandFlowLength::MapOverlandFlowLength(const FileName& fn, 
																		 MapPtr& p,
																		 const Map& mpDrainageNetwork,
																		 const Map& mpFlow)
: MapFromMap(fn, p, mpDrainageNetwork),
	m_mpFlow(mpFlow) 
{
	DomainValueRangeStruct dv(0,1e10,0.001);
	SetDomainValueRangeStruct(dv);
	//Verify compatible geo-reference
	CompitableGeorefs(fn, mp, m_mpFlow);
	init();
	
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.Store(); 
}

MapOverlandFlowLength::~MapOverlandFlowLength()
{

}

void MapOverlandFlowLength::init()
{
	
	//Verify domain and attribute table
	Domain dm = mp->dm();
	if (!(dm.fValid())) 
			throw ErrorObject(WhatError(TR("Map should have identifier domain"), errMapSlopeLengths), mp->fnObj);

	Table tbl = mp->tblAtt();
	if (!tbl.fValid())
				throw ErrorNoAttTable(mp->fnObj);
	
	if (!tbl[sDownstreamCoord].fValid())
					ColumnNotFoundError(tbl->fnObj, sDownstreamCoord);

	if (!tbl[sUpstreamCoord].fValid())
					ColumnNotFoundError(tbl->fnObj, sUpstreamCoord);

	if (!tbl[sUpstreamID].fValid())
					ColumnNotFoundError(tbl->fnObj, sUpstreamID);

	Domain flowdm = m_mpFlow->dm();
	if (!(flowdm.fValid() && (fCIStrEqual(flowdm->fnObj.sFileExt(), "FlowDirection.dom"))))
			throw ErrorObject(WhatError(TR("Use an input map with domain FlowDirection   "), errMapSlopeLengths), m_mpFlow->fnObj);

	fNeedFreeze = true;
	sFreezeTitle = "MapOverlandFlowLength";
	//htpFreeze = "ilwisapp\\overland_flow_length_algorithm.htm";

	objdep.Add(m_mpFlow);
}

const char* MapOverlandFlowLength::sSyntax()
{ 
		return "MapOverlandFlowLength(DrainageNetworkOrderMap, FlowDirectionMap)\n";  
}

MapOverlandFlowLength* MapOverlandFlowLength::create(const FileName& fn, 
																							 MapPtr& p, 
																				 const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms < 1)
      ExpressionError(sExpr, sSyntax());

  Map mpstream(as[0], fn.sPath());
	Map mpflowdir(as[1], fn.sPath());
	return new MapOverlandFlowLength(fn, p, mpstream, mpflowdir);
}

void MapOverlandFlowLength::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapOverlandFlowLength");
	WriteElement("OverlandFlowLength", "DrainageNetworkMap", mp);
	WriteElement("OverlandFlowLength", "FlowDirection", m_mpFlow);
}

String MapOverlandFlowLength::sExpression() const
{
  String sExp;
	sExp = String("MapOverlandFlowLength(%S,%S)", 
							 mp->sNameQuoted(true, fnObj.sPath()),
							 m_mpFlow->sNameQuoted(true));
	return sExp;
}

bool MapOverlandFlowLength::fDomainChangeable() const
{
  return false;
}

bool MapOverlandFlowLength::fGeoRefChangeable() const
{
  return false;
}

void InitFlowNums(vector<byte>& vReceiveNum)
{
		//	Flow number				Receive number			Loop ordering
		//	-------								-------					-------	  	
	  //	|6|7|8|								|2|3|4|					|0|1|2|
		//	-------								-------					-------
		//	|5| |1|								|1| |5|					|3|4|5|
		//	-------								-------					-------
		//	|4|3|2|								|8|7|6|					|6|7|8|
		//	-------								-------					-------
		//
		vReceiveNum.resize(9);
		
		vReceiveNum[0] = 2;
		vReceiveNum[1] = 3;
		vReceiveNum[2] = 4;
		vReceiveNum[3] = 1;
		vReceiveNum[4] = 0;
		vReceiveNum[5] = 5;
		vReceiveNum[6] = 8;
		vReceiveNum[7] = 7;
		vReceiveNum[8] = 6;
}

static void SplitString(String s, vector<long> &results)
{
	Array<String> as;
	Split(s, as, ",{}");
	results.clear();
	for (int i=0; i < as.size(); i++)
	{
		long res = as[i].iVal();
		if (res != iUNDEF && res > 0)
			results.push_back(res);
	}
}

bool MapOverlandFlowLength::fFreezing()
{
	trq.SetTitle(sFreezeTitle);
	trq.SetText(TR("Initialize map"));
	trq.Start();

	m_vDrainageMap.Open(iLines(), iCols());
	m_vFlowDir.Open(iLines(), iCols());
	m_vOutput_s.Open(iLines(), iCols());
	m_vOutput_d.Open(iLines(), iCols());

	//Reading input maps
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_vDrainageMap[iRow].Size(iCols()); 
		mp->GetLineRaw(iRow, m_vDrainageMap[iRow]);

		m_vFlowDir[iRow].Size(iCols());
		m_vOutput_s[iRow].Size(iCols());
		m_vOutput_d[iRow].Size(iCols());
		m_mpFlow->GetLineRaw(iRow, m_vFlowDir[iRow]);
		for (long iCol = 0; iCol < iCols(); ++iCol)
		{
			m_vOutput_s[iRow][iCol] = rUNDEF;
			m_vOutput_d[iRow][iCol] = rUNDEF;
		}
		if (trq.fUpdate(iRow, iLines())) return false;	
	}
	trq.fUpdate(iLines(), iLines());
	
	trq.SetText(TR("Calculating overland flow length"));
  Table tbl = mp->tblAtt();
	Column colDownstreamCoord = tbl->col(sDownstreamCoord);
	Column colUpstreamCoord = tbl->col(sUpstreamCoord);
	Column colUpstreamID = tbl->col(sUpstreamID);
	DomainSort* pdsrt = colDownstreamCoord->dmKey()->pdsrt();
	if ( !pdsrt )
		throw ErrorObject(TR("Source map must have domain class or id"));
  long iSize = pdsrt->iSize();

	InitFlowNums(m_vReceiveNum);

	for (long i=1; i<= iSize ; i++ ) {
		long iDrainageID = pdsrt->iOrd(i); 
		if (iDrainageID == iUNDEF)
			continue;

		RowCol rc = mp->gr()->rcConv(colDownstreamCoord->cValue(i));
		m_rcUpstream = mp->gr()->rcConv(colUpstreamCoord->cValue(i));
		//---here also upstream links needed to be able to process drainage links seperately
		vector<long> vUpstreamLinks;
		SplitString(colUpstreamID->sValue(i), vUpstreamLinks);
		bool fUpstreamlins;
		if (vUpstreamLinks.size() > 0)
			fUpstreamlins = true;
		else
			fUpstreamlins = false;
		Lengths2Stream(iDrainageID, rc, fUpstreamlins);
		RowCol rcDownstreamCoord = mp->gr()->rcConv(colDownstreamCoord->cValue(i));
		Lengths2Divide(iDrainageID, rcDownstreamCoord);
		if (trq.fUpdate(i, iSize)) return false;
	}
	trq.fUpdate(iSize, iSize);

	//Write outputs, also create lengths to divide map here
	//hereby commended creating lengths2divide map out,
	/*DomainValueRangeStruct dv(0,1e10,0.001);
	Map mapDivide(FileName(m_sDivide, ".mpr"), mp->gr(), rcSize(), dv);
	mapDivide->CreateMapStore();
	mapDivide->KeepOpen(true);*/
	trq.SetText(TR("Write output map"));
	for (long iRow = 0; iRow < iLines(); iRow++ )
	{
		RealBuf& bDist_s = m_vOutput_s[iRow];
    ptr.PutLineVal(iRow, bDist_s);
		//here also write data to lengths to divide map
		/*RealBuf& bDist_d = m_vOutput_d[iRow];
		mapDivide.ptr()->PutLineVal(iRow, bDist_d);*/
		if (trq.fUpdate(iRow, iLines())) return false;
	}
	trq.fUpdate(iLines(), iLines());

	//Clean up
	m_vDrainageMap.Close();
	m_vFlowDir.Close();
	m_vOutput_s.Close();
	m_vOutput_d.Close();
	m_vReceiveNum.resize(0);
	m_vCellsOnDivide.resize(0);

	return true;
}

void MapOverlandFlowLength::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
{
	bool fIncompGeoRef = false;
	if (mp1->gr()->fGeoRefNone() && mp2->gr()->fGeoRefNone())
		  fIncompGeoRef = mp1->rcSize() != mp2->rcSize();
	else
			fIncompGeoRef = mp1->gr() != mp2->gr();
	if (fIncompGeoRef)
			throw ErrorIncompatibleGeorefs(mp1->gr()->sName(true, fn.sPath()),
            mp2->gr()->sName(true, fn.sPath()), fn, errMapSlopeLengths);

}

bool MapOverlandFlowLength::IsEdgeCell(long iRow, long iCol)
{
		if (iRow <= 0 || iRow >= iLines() - 1 ||
				iCol <= 0 || iCol >= iCols() - 1)
				return true;
		else
				return false;
}

void MapOverlandFlowLength::Lengths2Stream(long iStreamID, RowCol rc, bool fUpstreamlins)
{
		//To match the index from 0 in the input matrix
	    rc.Row = rc.Row;
		rc.Col = rc.Col;
		m_rcUpstream.Row = m_rcUpstream.Row;  
		m_rcUpstream.Col = m_rcUpstream.Col;

		m_vOutput_s[rc.Row][rc.Col] = 0; //It should be 0 to outlet cell of the segment
		vector<RowCol> vStartCells;
		vStartCells.push_back(rc);
		
		vector<RowCol>::iterator pos;
		vector<RowCol> vFlow2Cells; //*stores cells that drain to start cells 
		m_vCellsOnDivide.resize(0);
		do
		{
			vFlow2Cells.resize(0); 
			for (pos = vStartCells.begin(); pos < vStartCells.end(); ++pos)
			{
					RowCol rcStart (*pos);
					int index = 0;
					bool isDivide = true;
					for (int i=-1; i<2; ++i)
					{
						for (int j=-1; j<2; ++j)
						{
							RowCol rcCur; 
							rcCur.Row = rcStart.Row + i;
							rcCur.Col = rcStart.Col + j;
							if (IsEdgeCell(rcCur.Row, rcCur.Col) != true)
							{
								bool isFlowNum = (m_vFlowDir[rcCur.Row][rcCur.Col] == m_vReceiveNum[index]);
								//bool isUpStream = find(vUpstream.begin(), vUpstream.end(), rcCur) != vUpstream.end();
								bool isUpStream = ((rcCur == m_rcUpstream) && (fUpstreamlins==true));
								bool isUndef = (m_vOutput_s[rcCur.Row][rcCur.Col] == rUNDEF);
								bool isOverlandPixel = ((m_vDrainageMap[rcCur.Row][rcCur.Col] < 1) || (m_vDrainageMap[rcCur.Row][rcCur.Col] == iStreamID)); 
								bool isFlow = ((isFlowNum) && (isUpStream != true) && (isUndef));
								if (isFlow && isOverlandPixel)
								{
									if (m_vDrainageMap[rcCur.Row][rcCur.Col] == iStreamID)
										m_vOutput_s[rcCur.Row][rcCur.Col] = 0; //set dist value to 0, it should be
									else
									{
										Coord c1 = mp->gr()->cConv(rcStart);
										Coord c2 = mp->gr()->cConv(rcCur);
										double  rDist = rDistance(c1, c2);
										m_vOutput_s[rcCur.Row][rcCur.Col] = m_vOutput_s[rcStart.Row][rcStart.Col] + rDist;
									}
									vFlow2Cells.push_back(rcCur);
									isDivide = false;
								}
							}
							index++;
						}
					}
					if (isDivide)
						m_vCellsOnDivide.push_back(rcStart);
			}
			vStartCells.swap(vFlow2Cells);

		}while(vFlow2Cells.size() != 0); //no more element can be located 
}

void MapOverlandFlowLength::Lengths2Divide(long iStreamID, RowCol rcDownstreamCoord)
{
	vector<RowCol>::iterator pos;
	for (pos = m_vCellsOnDivide.begin(); pos < m_vCellsOnDivide.end(); ++pos)
	{
		RowCol rcStart = (*pos);
		m_vOutput_d[rcStart.Row][rcStart.Col] = 0;
		bool fCheck = true;
		do
		{
				int iFlowNum = m_vFlowDir[rcStart.Row][rcStart.Col];
				RowCol rcEnd;
				switch (iFlowNum)
				{
					case 1: {	//location 1/E
						rcEnd.Row = rcStart.Row;
						rcEnd.Col = rcStart.Col+1;
					}
					break;
					case 2: { //location 2/SE 
						rcEnd.Row = rcStart.Row+1;
						rcEnd.Col = rcStart.Col+1;
					}
					break;
					case 3: {	//location 3/S
						rcEnd.Row = rcStart.Row+1;
						rcEnd.Col = rcStart.Col;
					}
					break;
					case 4:{ //Location 4/SW
						rcEnd.Row = rcStart.Row+1;
						rcEnd.Col = rcStart.Col-1;
					}
					break;
					case 5:{	//location 5/W
						rcEnd.Row = rcStart.Row;
						rcEnd.Col = rcStart.Col-1;
					}
					break;
					case 6:{	//location 6/NW 
						rcEnd.Row = rcStart.Row-1;
						rcEnd.Col = rcStart.Col-1;
					}
					break;
					case 7:{	//location 7/N
						rcEnd.Row = rcStart.Row-1;
						rcEnd.Col = rcStart.Col;
					}
					break;
					case 8:{	//location 8/NE
						rcEnd.Row = rcStart.Row-1;
						rcEnd.Col = rcStart.Col+1;
					}
					break;
					default:{
						fCheck = false;	
					}
					break;	
				}
				Coord c1 = mp->gr()->cConv(rcStart);
				Coord c2 = mp->gr()->cConv(rcEnd);
				double  rDist = rDistance(c1, c2) + m_vOutput_d[rcStart.Row][rcStart.Col];
				 
				if ((m_vOutput_d[rcEnd.Row][rcEnd.Col] == rUNDEF) || (m_vOutput_d[rcEnd.Row][rcEnd.Col] > rDist))
				{
						m_vOutput_d[rcEnd.Row][rcEnd.Col] = rDist;
				}
				if (IsEdgeCell(rcEnd.Row, rcEnd.Col)) 
					fCheck = false;
				else if ((rcDownstreamCoord.Row-1 == rcEnd.Row) && (rcDownstreamCoord.Col-1 == rcEnd.Col))
					fCheck = false;
				
				rcStart = rcEnd;
		}while(fCheck);  
	}
}

bool MapOverlandFlowLength::fLatLonCoords()
{
	CoordSystemLatLon* csll = mp->cs()->pcsLatLon();
	return (0 != csll);
}

static double rSphericalDistance(double rRadius, const LatLon& ll_1, const LatLon& ll_2)
{
		if (ll_1.fUndef() || ll_2.fUndef())
		return rUNDEF;
		double phi1 = ll_1.Lat * M_PI/180.0; //conversion to radians
		double lam1 = ll_1.Lon * M_PI/180.0; 
		double phi2 = ll_2.Lat * M_PI/180.0; ;
		double lam2 = ll_2.Lon * M_PI/180.0; ;
		double sinhalfc = fabs(sin((phi2- phi1)/2)* sin((phi2- phi1)/2) +
		 cos(phi1)*cos(phi2)* sin((lam2 - lam1)/2)* sin((lam2 - lam1)/2));
		sinhalfc = sqrt(sinhalfc);
		double c; // the shortest spherical arc
		if (sinhalfc < sqrt(2.0)/2)
			c = 2.0 * asin(sinhalfc);
		else 
		{
			phi2 = -phi2;
			lam2 = M_PI + lam2;
			sinhalfc = fabs(sin((phi2- phi1)/2)* sin((phi2- phi1)/2)+
				cos(phi1)*cos(phi2)*sin((lam2 - lam1)/2)*sin((lam2 - lam1)/2));
			sinhalfc = sqrt(sinhalfc);
			c = M_PI - 2.0 * asin(sinhalfc);
		}
		return c * rRadius;
}

double MapOverlandFlowLength::rDistance(Coord cd1, Coord cd2)
{
	double rDist;
	if (fLatLonCoords())
	{
			double rRadi = rDefaultEarthRadius;
			if(mp->cs()->pcsViaLatLon())
				rRadi = mp->cs()->pcsViaLatLon()->ell.a;
			
			LatLon llStart = LatLon(cd1.y, cd1.x);
			LatLon llEnd  = LatLon(cd2.y, cd2.x);
			if (fEllipsoidalCoords()) 
			{
				CoordSystemViaLatLon* pcsvll= mp->cs()->pcsViaLatLon();
				if ( (llStart.Lat == llEnd.Lat) && (llStart.Lon == llEnd.Lon))
					rDist = 0; //seems a bug in rEllipsoidalDistance, always get some value, even when llStart and llEnd the same? 
				else
					rDist = pcsvll->ell.rEllipsoidalDistance(llStart, llEnd);
				if (rDist < 8000)
					return rDist;
			}
			rDist = rSphericalDistance(rRadi, llStart, llEnd);
	}
	else
	{	
		double dx = (cd1.x - cd2.x);
		double dy = (cd1.y - cd2.y);
		rDist = sqrt(dx * dx + dy * dy);
	}
	return rDist;
}

bool MapOverlandFlowLength::fEllipsoidalCoords()
{
	CoordSystemViaLatLon* csviall = mp->cs()->pcsViaLatLon();
	bool fSpheric = true;
	if (csviall)
		fSpheric= (csviall->ell.fSpherical());
	return (0 != csviall &&  0 == fSpheric);
}
