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
// MapDrainageNetworkOrdering.cpp: implementation of the MapDrainageNetworkOrdering class.
//
//////////////////////////////////////////////////////////////////////

#include "HydroFlowApplications\MapDrainageNetworkOrdering.h"
#include "Engine\Table\tbl.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapDrainageNetworkOrdering(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapDrainageNetworkOrdering::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapDrainageNetworkOrdering(fn, (MapPtr &)ptr);
}

const double rDefaultEarthRadius = 6371007.0;

using namespace std;

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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapDrainageNetworkOrdering::MapDrainageNetworkOrdering(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	ReadElement("DrainageNetworkOrdering", "FlowDirection", m_mpFlow);
	ReadElement("DrainageNetworkOrdering", "DrainageMap", m_mpDrainage);
	m_dm = p.dm();
	ReadElement("DrainageNetworkOrdering", "MinimumDrainageLength", m_rMinLen);
	fNeedFreeze = true;
  sFreezeTitle = "MapDrainageNetworkOrdering";
	//htpFreeze = htpDrainageNetworkOrderingT;
	//FileName fnMap = FileName(m_mpFlow);
	//m_mpFlow = Map(fnMap);
}

MapDrainageNetworkOrdering::MapDrainageNetworkOrdering(const FileName& fn, 
																											 MapPtr& p,
																											 const Map& mp,
																											 const Map& mpflowdir,
																											 const Map& mpDrainage,
																											 double rMinLen,
																											 const Domain& dm) 
: MapFromMap(fn, p, mp),
  m_mpFlow(mpflowdir),
	m_mpDrainage(mpDrainage),
	m_rMinLen(rMinLen),
	m_dm(dm)
{
	//DomainValueRangeStruct dv(1,300);
	SetDomainValueRangeStruct(m_dm);  
  fNeedFreeze = true;
	sFreezeTitle = "MapDrainageNetworkOrdering";
	//htpFreeze = htpDrainageNetworkOrderingT;

	objdep.Add(m_mpFlow);
	objdep.Add(m_mpDrainage);

  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.Store(); 
}

MapDrainageNetworkOrdering::~MapDrainageNetworkOrdering()
{
}

const char* MapDrainageNetworkOrdering::sSyntax()
{ 
  return "MapDrainageNetworkOrdering(DEM, FlowDirectionMap, DrainageNetworkMap, MinimumDrainageLength)";
}

MapDrainageNetworkOrdering* MapDrainageNetworkOrdering::create(const FileName& fn, 
																															 MapPtr& p, 
																															 const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms != 4 )
      ExpressionError(sExpr, sSyntax());

  Map mpDEM(as[0], fn.sPath());
	Map mpflowdir(as[1], fn.sPath());
	Map mpDrainage(as[2], fn.sPath());
	
	
	Domain dm = mpDrainage->dm();
	if (!(dm.fValid() && (fCIStrEqual(dm->fnObj.sFileExt(), "bool.dom"))))
			throw ErrorObject(WhatError(TR("Map should have domain bool"), errMapDrainageNetworkOrdering), mpDrainage->fnObj);

	dm = mpflowdir->dm();
	if (!(dm.fValid() && (fCIStrEqual(dm->fnObj.sFileExt(), "FlowDirection.dom"))))
			throw ErrorObject(WhatError(TR("Use an input map with domain FlowDirection   "), errMapDrainageNetworkOrdering), mpflowdir->fnObj);

	bool fIncompGeoRef = false;
	if (mpDEM->gr()->fGeoRefNone() && mpflowdir->gr()->fGeoRefNone())
		  fIncompGeoRef = mpDEM->rcSize() != mpflowdir->rcSize();
	else
			fIncompGeoRef = mpDEM->gr() != mpflowdir->gr();
	if (fIncompGeoRef)
			throw ErrorIncompatibleGeorefs(mpDEM->gr()->sName(true, fn.sPath()),
            mpflowdir->gr()->sName(true, fn.sPath()), fn, errMapDrainageNetworkOrdering);
  
	if (mpDEM->gr()->fGeoRefNone() && mpDrainage->gr()->fGeoRefNone())
		  fIncompGeoRef = mpDEM->rcSize() != mpDrainage->rcSize();
	else
			fIncompGeoRef = mpDEM->gr() != mpDrainage->gr();
	if (fIncompGeoRef)
			throw ErrorIncompatibleGeorefs(mpDEM->gr()->sName(true, fn.sPath()),
            mpDrainage->gr()->sName(true, fn.sPath()), fn, errMapDrainageNetworkOrdering);

	double rMinLen = as[3].rVal();
  if (rMinLen<0)
    rMinLen = 0;

	FileName fnDom(fn, ".dom");
	fnDom = FileName::fnUnique(fnDom);
	dm = Domain(fnDom, 65536, dmtID);  //set maximum items to 65536, resize it in Freezing function 

	DomainSort *pds = dm->pdsrt();
	pds->dsType = DomainSort::dsMANUAL;
	
  return new MapDrainageNetworkOrdering(fn, p, mpDEM, mpflowdir, mpDrainage, rMinLen, dm);
}

void MapDrainageNetworkOrdering::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapDrainageNetworkOrdering");
	WriteElement("DrainageNetworkOrdering", "FlowDirection", m_mpFlow);
	WriteElement("DrainageNetworkOrdering", "DrainageMap", m_mpDrainage);
	WriteElement("DrainageNetworkOrdering", "MinimumDrainageLength", m_rMinLen);
}

String MapDrainageNetworkOrdering::sExpression() const
{
  
	String sExp("MapDrainageNetworkOrdering(%S,%S, %S, %g)", mp->sNameQuoted(true, fnObj.sPath()),
               m_mpFlow->sNameQuoted(true), m_mpDrainage->sNameQuoted(true), m_rMinLen);
  return sExp;
}

bool MapDrainageNetworkOrdering::fDomainChangeable() const
{
  return false;
}

bool MapDrainageNetworkOrdering::fGeoRefChangeable() const
{
  return false;
}

bool MapDrainageNetworkOrdering::fFreezing()
{
	IniParms();
	
	trq.SetTitle(sFreezeTitle);
	trq.SetText(TR("Initialize map"));
	trq.Start();

	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		//Reading input drainage map
		m_vDrainageMap[iRow].Size(iCols()); 
		m_mpDrainage->GetLineVal(iRow, m_vDrainageMap[iRow]);

		//Reading input flow direction map
		m_vFlowDir[iRow].Size(iCols()); 
		m_mpFlow->GetLineRaw(iRow, m_vFlowDir[iRow]);
		for (long iCol = 0; iCol < iCols(); ++iCol)
		{
			if (m_vDrainageMap[iRow][iCol] != iUNDEF)	
				m_vDrainageMap[iRow][iCol] -= 2;  //Initialize input drainage map
			if (IsEdgeCell(iRow, iCol))
				m_vFlowDir[iRow][iCol] = 0;
		}
		if (trq.fUpdate(iRow, iLines())) return false;	
	}
	trq.fUpdate(iLines(), iLines());

	//Create segment map
	FileName fnSeg = fnObj;
	fnSeg.sExt = ".mps";
	SegmentMap segmap = SegmentMap(fnSeg, cs(), cb(), m_dm);
	
	//***a temporary segment map needed to be able to remove/patch a channel   
	FileName fnSegTmp = FileName("tmp");
	fnSegTmp.sExt = ".mps";
	SegmentMap smpTmp(FileName::fnUnique(fnSegTmp), CoordSystem(), 
                    CoordBounds(Coord(0,0), Coord(rcSize().Col, rcSize().Row)), 
                    dvrs());
	smpTmp->fErase = true;

	//*Source link traces starts from a sorcecell along 
	//*the downstream channel till a junction is reached
	trq.SetText(TR("Exterior Drainage Channel Ordering"));
  double rPixDist = mp->gr()->rPixSize() * 1.41; //max. a pixel distance 
	for (long iRow = 1; iRow< iLines() - 1; iRow++ )
	{
		for (long iCol = 1; iCol < iCols() - 1; iCol++)
		{
			if (IsSourceCell(iRow, iCol))
			{
				  RowCol rc;
				  rc.Row = iRow;
				  rc.Col = iCol;
				  m_rLength = 0;
					m_vStreamLink.resize(0);

					int iLink = IdentifyLink(rc);
					if ((m_rLength - rPixDist) <= m_rMinLen ) //remove drainage shorter than threshold,also remove by default a drainage of only one pixel  
					{

						FlagDrainageMap(iOffDrainage, iLink);
						if ((iLink == iJunction) && IsJunctionExists(rc))
						{
							if (IsJunction(rc) != true)
							{
								long iIndex = GetUpstream(rc);
								EraseJunction(rc);
								m_rLength = m_vRecords[iIndex].Length;
								m_vStreamLink.resize(0);
								iLink = IdentifyLink(rc); 
								PatchSegment(smpTmp, m_vRecords[iIndex].id);
								m_vRecords[iIndex].DownstreamCoord = m_vStreamLink[m_vStreamLink.size()-1];
                m_vRecords[iIndex].TostreamCoord = m_vStreamLink[m_vStreamLink.size()-1];
                if (iLink == iJunction)
								{
									StoreJunction(rc);
                  if (m_vStreamLink.size() > 1)
										m_vRecords[iIndex].TostreamCoord = m_vStreamLink[m_vStreamLink.size()-2];
                }
                FlagDrainageMap(m_vRecords[iIndex].id, iLink);
								m_vRecords[iIndex].Length = m_rLength;
							}
						} 
					}	
					else 
					{
						m_iLinkNumber++;
						AddDomainItem(m_iLinkNumber);
						StoreSegment(smpTmp, m_iLinkNumber, false);
						UpdateRecord();
						if (iLink == iJunction)
						{
							StoreJunction(rc);
              if (m_vStreamLink.size() > 1)
										m_rec.TostreamCoord = m_vStreamLink[m_vStreamLink.size()-2];
            }
						FlagDrainageMap(m_iLinkNumber, iLink);
						m_vRecords.push_back(m_rec);
					}
			} //IsSourceCell
		}
		if (trq.fUpdate(iRow, iLines())) return false;
	}
	CleanSegment(segmap, smpTmp);
	trq.fUpdate(iLines(), iLines());
	//*Interior link traces from junction to junction in m_vJunction
	//*every time a junction is erased from the vector, while a drainage
	//*starting at the junction has been identified 
	//*The traces stop, when every junction in m_vJunction has been evaluated 
	trq.SetText(TR("Interior Drainage Channel Ordering"));
	size_t size = m_vJunction.size();
	while (m_vJunction.size() != 0 )
	{
		for (long index=0; index < (long)m_vJunction.size(); ++index)
		{		
			RowCol node (m_vJunction[index]);
			m_vStreamLink.resize(0);
			vector<long> vUpstreamLink;
		  if (IsAllUpstreamIdentified(node, vUpstreamLink))
			{
				m_vStreamLink.push_back(node);
				RowCol rcDownCell = node;
				int iFlow = GetDownStreamCell(rcDownCell);
				if (iFlow != iInValidFlow)  
				{
					m_iLinkNumber++;
					Coord c1 = mp->gr()->cConv(node);
					Coord c2 = mp->gr()->cConv(rcDownCell);
					m_rLength = rDistance(c1 ,c2);
					int iLink = IdentifyLink(rcDownCell);
					AddDomainItem(m_iLinkNumber);
					StoreSegment(segmap, m_iLinkNumber, true);
					m_iStrahler = ComputeStrahlerOrder(vUpstreamLink);
					m_iStreve = ComputeStreveOrder(vUpstreamLink);
					UpdateRecord();
          if (iLink == iJunction){
          	StoreJunction(rcDownCell);
            if (m_vStreamLink.size() > 1)
										m_rec.TostreamCoord = m_vStreamLink[m_vStreamLink.size()-2];
          }
          FlagDrainageMap(m_iLinkNumber, iLink);
					DownstreamLink(vUpstreamLink);
					m_vRecords.push_back(m_rec);
				}
				EraseJunction(node);
				if (trq.fUpdate(index, (long)m_vJunction.size())) return false;
			}
		} //for
	}

	//Cleanup
	m_vJunction.resize(0);
	m_vStreamLink.resize(0);
	m_vDem.Close();
	m_vFlowDir.Close();
	
	//Write drainage map
	m_dm->pdsrt()->Resize(m_iLinkNumber);
	trq.SetText(TR("Write Drainage Network Raster Map"));
  for (long iRow = 0; iRow < iLines(); iRow++ )
	{
		LongBuf& bDrainage = m_vDrainageMap[iRow];

		ptr.PutLineRaw(iRow, bDrainage);
		if (trq.fUpdate(iRow, iLines())) return false;
	}
	trq.fUpdate(iLines(), iLines());
	m_vDrainageMap.Close();

	
	//Read DEM map
	trq.SetText(TR("Reading DEM"));
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_vDem[iRow].Size(iCols()); 
		mp->GetLineVal(iRow, m_vDem[iRow]);
		if (trq.fUpdate(iRow, iLines())) return false;	
	}
	trq.fUpdate(iLines(), iLines());

	//Create table
	CreateTable();
	
	if (fTblAtt())
		segmap->SetAttributeTable(tblAtt());
	
	return true;
}

void MapDrainageNetworkOrdering::IniParms()
{
	m_vDrainageMap.Open(iLines(), iCols());  //allocate memory for the input drainage map 
	m_vFlowDir.Open(iLines(), iCols());  //allocate memory for the input flow direction map 
	m_vDem.Open(iLines(), iCols()); 
	m_iLinkNumber = 0; 
	m_iStrahler = 1;
	m_iStreve = 1;
	m_sUpstreamLink = "0";
	m_rTotalUpstreamLength = 0;
}

bool MapDrainageNetworkOrdering::IsSourceCell(long iRow, long iCol)
{
	//Examine the neighbor cells to locate a sourcecell, 
	//return true, if no any cells flow into it.  
	//Figure shows a flow path location number   
	//	-------         
	//	|6|7|8|         
	//	-------
	//	|5| |1|
	//	-------
	//	|4|3|2|
	//	-------
	//
	int in = 0;
	int iUpFlow[9] = { 2, 3, 4, 1, 0, 5, 8, 7, 6 };
	
	if ((m_vDrainageMap[iRow][iCol] == iDrainage) && (m_vFlowDir[iRow][iCol] != 0))
	{
		for (int i=-1; i<2; ++i)
		{
			for (int j=-1; j<2; ++j)
			{
				if ((m_vFlowDir[iRow+i][iCol+j] == iUpFlow[in]) && 
					  (m_vDrainageMap[iRow+i][iCol+j] > iOffDrainage))
					return false;
				in++;
			}
		}
		return true;
	}
	else
		return false;
}

int MapDrainageNetworkOrdering::IdentifyLink(RowCol& rc)
{
	//trace along a channel link from the given cell rc
	//the function will stop when it a junction (returns 1) or
	//an edge cell or a dead-end point is reached 

  int iLink = -2;
	Coord c1, c2;
	c1 = mp->gr()->cConv(rc);
	while(iLink < 0)
	{
		m_vStreamLink.push_back(rc);
		if (IsEdgeCell(rc.Row, rc.Col)) 
		{ 
			c2 = mp->gr()->cConv(rc);
			m_rLength = m_rLength + rDistance(c1, c2);
			iLink = 0;
		}
		else if (IsJunction(rc))
		{
			c2 = mp->gr()->cConv(rc);
			m_rLength = m_rLength + rDistance(c1, c2);
			iLink = iJunction;
		}
    else
		{
			int iFlow = GetDownStreamCell(rc);
		  if (iFlow == iInValidFlow)
      {
				iLink = 0; //dead-end cell, no flow can advance at this position
			}
			else 
			{
				c2 = mp->gr()->cConv(rc);
				m_rLength = m_rLength + rDistance(c1, c2);
				c1 = c2;
			}
    }
  }
	return iLink;
}

void MapDrainageNetworkOrdering::UpdateRecord()
{
		m_rec.id = m_iLinkNumber;
		m_rec.UpstreamLink = m_sUpstreamLink;
		m_rec.UpstreamCoord = m_vStreamLink[0];
		m_rec.DownstreamLink = 0;
		m_rec.DownstreamCoord = m_vStreamLink[m_vStreamLink.size()-1];
		m_rec.Strahler = m_iStrahler;
		m_rec.Streve = m_iStreve;
		m_rec.Length = m_rLength;
		m_rec.TotalUpstreamLength = m_rTotalUpstreamLength;
    m_rec.TostreamCoord = m_vStreamLink[m_vStreamLink.size()-1];
}

void MapDrainageNetworkOrdering::AddDomainItem(long iItem )
{
	String sUniqueID = String("%li", iItem);
	m_dm->pdsrt()->iAdd(sUniqueID,true);
}

void MapDrainageNetworkOrdering::FlagDrainageMap(long iFlag, int iLink)
{
	//Flag the drainage with the unique drainage ID
	if (iLink == iJunction)
			m_vStreamLink.resize(m_vStreamLink.size()-1);
	for (vector<RowCol>::iterator pos = m_vStreamLink.begin(); 
								 pos < m_vStreamLink.end(); ++pos)
	{		
				RowCol rc (*pos);
				m_vDrainageMap[rc.Row][rc.Col] = iFlag;	
	}
}

bool MapDrainageNetworkOrdering::IsJunctionExists(RowCol node)
{
	//Find junction node in m_vJunction that matches value
	//If no such junction exists, the function returns the last 
	bool IsExists = find(m_vJunction.begin(), m_vJunction.end(), node) != m_vJunction.end();
	if 	(IsExists)
  		return true;
	else
			return false;
}

bool MapDrainageNetworkOrdering::IsJunction(RowCol rc)
{
	//For the specified cell at given loaction rc, 
	//checks whether it is a junction,
	//Flow path number
	//	-------
	//	|6|7|8|
	//	-------
	//	|5| |1|
	//	-------
	//	|4|3|2|
	//	-------
	// 
	int pos = 0;
	int iUpFlows = 0;
	int iUpFlow[9] = { 2, 3, 4, 1, 0, 5, 8, 7, 6 } ;

	for (int i=-1; i<2; ++i)
	{
		for (int j=-1; j<2; ++j)
		{
				int iFlow = m_vFlowDir[rc.Row+i][rc.Col+j];
				int iDm = m_vDrainageMap[rc.Row+i][rc.Col+j];

				if ((m_vFlowDir[rc.Row+i][rc.Col+j] == iUpFlow[pos]) && 
					  (m_vDrainageMap[rc.Row+i][rc.Col+j] > iOffDrainage))
        {   
					iUpFlows++;
					if (iUpFlows > 1)
						return true;
        }
				pos++;
		}
	}
	return false;
}

void MapDrainageNetworkOrdering::EraseJunction(RowCol rc)
{
	vector<RowCol>::iterator pos = find(m_vJunction.begin(), m_vJunction.end(), rc);
	m_vJunction.erase(pos);
}

void MapDrainageNetworkOrdering::StoreJunction(RowCol rc)
{
	if (!IsJunctionExists(rc))
        m_vJunction.push_back(rc);
}

int MapDrainageNetworkOrdering::GetDownStreamCell(RowCol& rc)
{
	//Return a cell that the current given cell rc flows to,
	//otherwise, return the same cell as the given cell, This means
	//that the given cell doesn't flow to any other cell.   

	int iPos = m_vFlowDir[rc.Row][rc.Col];
	switch (iPos)
	{
				case 1: 	//East
						rc.Row = rc.Row;
						rc.Col = rc.Col+1;
						break;
				case 2:  //South East 
						rc.Row = rc.Row+1;
						rc.Col = rc.Col+1;
						break;
				case 3: 	//South
						rc.Row = rc.Row+1;
						rc.Col = rc.Col;
						break;
				case 4: //South West
						rc.Row = rc.Row+1;
						rc.Col = rc.Col-1;
						break;
				case 5:	//West
						rc.Row = rc.Row;
						rc.Col = rc.Col-1;
						break;
				case 6:	//North West 
						rc.Row = rc.Row-1;
						rc.Col = rc.Col-1;
						break;
				case 7:	//North
						rc.Row = rc.Row-1;
						rc.Col = rc.Col;
						break;
				case 8:	//North East
						rc.Row = rc.Row-1;
						rc.Col = rc.Col+1;
						break;
	}
	
	if ((m_vDrainageMap[rc.Row][rc.Col] == iDrainage) &&
			(m_vFlowDir[rc.Row][rc.Col] != 0))
			return iPos;
	else
	    return 0; //invalid a flow path
}

//Find drainage index in m_vRecords
long MapDrainageNetworkOrdering::FindDrainageIndex(long iDrainageID)
{
		for (vector<RecordObj>::iterator pos = m_vRecords.begin(); 
								 pos < m_vRecords.end(); ++pos)
		{		
				RecordObj ro (*pos);
				if (ro.id == iDrainageID)
			 		return pos - m_vRecords.begin();
		}
		return -1;
}

//Get upstream index in m_vRecords 
long MapDrainageNetworkOrdering::GetUpstream(RowCol rc)
{
	int pos = 0;
	int iUpFlow[9] = { 2, 3, 4, 1, 0, 5, 8, 7, 6 } ;
	RecordObj ro ;
	for (int i=-1; i<2; ++i)
	{
		for (int j=-1; j<2; ++j)
		{
			  pos++;
				long iDrainageID = m_vDrainageMap[rc.Row+i][rc.Col+j];
				if (m_vFlowDir[rc.Row+i][rc.Col+j] == iUpFlow[pos-1]) 
				{
					if (iDrainageID > iDrainage ) //has been identified
					{   
							return FindDrainageIndex(iDrainageID);												
					}
						
				}
		}
	}
	return 0;
}

void MapDrainageNetworkOrdering::DownstreamLink(vector<long> vPos)
{

		for (vector<long>::iterator pos = vPos.begin(); 
								 pos < vPos.end(); ++pos)
		{		
				long iPos (*pos);
				m_vRecords[iPos].DownstreamLink = m_iLinkNumber;
		}
}

bool MapDrainageNetworkOrdering::IsAllUpstreamIdentified(RowCol node, vector<long>& vUpstreamPos) 
																												 
{
	//* Evaluate if every upstream has been identified/traced
	//* If true, also generate upstreams IDs,  
	//* calculate the total upstream length
	//* store the index of upstreams to vUpstreamPos, which indicates the location of upstreams in m_vRecords
	
	int pos = 0;
	int iUpFlows = 0;
	int iUpFlow[9] = { 2, 3, 4, 1, 0, 5, 8, 7, 6 } ;
	m_rTotalUpstreamLength = 0;
	vUpstreamPos.resize(0);
	m_sUpstreamLink = "";

	for (int i=-1; i<2; ++i)
	{
		for (int j=-1; j<2; ++j)
		{
			  pos++;
				long iVal = m_vDrainageMap[node.Row+i][node.Col+j];
				if (m_vFlowDir[node.Row+i][node.Col+j] == iUpFlow[pos-1]) 
				{
					if (iVal == iDrainage )
					{   
						return false;
					}
					else if (iVal > 0 )
					{
						long index = FindDrainageIndex(iVal);
						m_rTotalUpstreamLength = m_rTotalUpstreamLength + m_vRecords[index].Length;  
						String sUpstreamLink = m_sUpstreamLink.length() != 0 ? String(",%li",iVal) : String("%li", iVal);
						m_sUpstreamLink &= sUpstreamLink;
						vUpstreamPos.push_back(index);
					}	
				}
		}
	}
	m_sUpstreamLink = String("{%S}", m_sUpstreamLink); 
	return true;
}

bool MapDrainageNetworkOrdering::IsEdgeCell(long iRow, long iCol)
{
		if (iRow == 0 || iRow == iLines() - 1 ||
				iCol == 0 || iCol == iCols() - 1)
				return true;
		else
				return false;
}

bool MapDrainageNetworkOrdering::isEven(int elem)
{
		return elem % 2 == 0;
}

long MapDrainageNetworkOrdering::ComputeStrahlerOrder(vector<long> vUpstreamLink)
{
		vector<long> vStrahler;
		for (vector<long>::iterator pos = vUpstreamLink.begin(); 
								 pos < vUpstreamLink.end(); ++pos)
		{		
				long iPos (*pos);
				vStrahler.push_back(m_vRecords[iPos].Strahler);
		}
		
		//returns the position of the first element with max. in vValue
		vector<long>::iterator pos = max_element(vStrahler.begin(), vStrahler.end());
		long iMax = *pos;
		
		pos = min_element(vStrahler.begin(), vStrahler.end());
		long iMin = *pos;
		
		if (iMax > iMin)
			return iMax;
		else
			return iMax+1;
}

long MapDrainageNetworkOrdering::ComputeStreveOrder(vector<long> vUpstreamLink)
{
		long iStreve = 0;
		for (vector<long>::iterator pos = vUpstreamLink.begin(); 
								 pos < vUpstreamLink.end(); ++pos)
		{		
				long iPos (*pos);
				iStreve = iStreve + m_vRecords[iPos].Streve;
		}
		return iStreve;
}

double MapDrainageNetworkOrdering::rComputeSlope(double rDrop, double rLength, bool fDegree)
{
		if (rLength > 0)
		{
			if (fDegree  ) 
				return (180/M_PI)*atanl(abs(rDrop/rLength)); //degree
			else
				return (rDrop/rLength)*100; //percent
		}
		else
			return 0;
}

double MapDrainageNetworkOrdering::rDistance(Coord cd1, Coord cd2)
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

double MapDrainageNetworkOrdering::rComputeSinuosity(double rLength, double rStraightLenght)
{
	if (rStraightLenght > 0)
	 return rLength/rStraightLenght;
	else
	 return 0;
}

void MapDrainageNetworkOrdering::CreateTable()
{
	trq.SetText(TR("Create Drainage Network Table"));
	FileName fnTbl(fnObj, ".tbt");
	fnTbl = FileName::fnUnique(fnTbl);
	
	Table	tbl = Table(fnTbl, m_dm);
  SetAttributeTable(tbl);

	Domain dmcrd;
	//Domain dmcrdbuf("CoordBuf");
	dmcrd.SetPointer(new DomainCoord(mp->cs()->fnObj));

	Column cUpstreamLink = tbl->colNew(String("UpstreamLinkID"), Domain("string")); 
  cUpstreamLink->SetOwnedByTable(true);
  cUpstreamLink->SetReadOnly(true);
	Column cUpstreamCoord = tbl->colNew(String("UpstreamCoord"), dmcrd);
	Column cUpstreamHeight = tbl->colNew(String("UpstreamElevation"), Domain("value"));
	Column cDownstreamLink = tbl->colNew(String("DownstreamLinkID"), Domain("value"), ValueRange(1,32767,1));
  cDownstreamLink->SetOwnedByTable(true);
  cDownstreamLink->SetReadOnly(true);
	Column cDownstreamCoord = tbl->colNew(String("DownstreamCoord"), dmcrd);
	Column cDownstreamHeight = tbl->colNew(String("DownstreamElevation"), Domain("value"));
	Column cDrop = tbl->colNew(String("ElevationDifference"), Domain("value"));
	
	Column cStrahler = tbl->colNew(String("Strahler"), Domain("value"), ValueRange(1,32767,1));
  cStrahler->SetOwnedByTable(true);
  cStrahler->SetReadOnly(true);
	
	Column cStreve = tbl->colNew(String("Shreve"), Domain("value"), ValueRange(1,32767,1));
  cStreve->SetOwnedByTable(true);
  cStreve->SetReadOnly(true);

	Column cLength = tbl->colNew(String("Length"), Domain("value"));
	Column cStraightLength = tbl->colNew(String("StraightLength"), Domain("value"));
	Column cSlopAlongDrainage = tbl->colNew(String("SlopeAlongDrainage%"), Domain("value"), ValueRange(0,1e10,0.01));
	Column cSlopAlongDrainageDegree = tbl->colNew(String("SlopeAlongDrainageDegree"), Domain("value"), ValueRange(0,1e10,0.001));
	Column cSlopDrainageStraight = tbl->colNew(String("SlopeDrainageStraight%"), Domain("value"), ValueRange(0,1e10,0.01));
	Column cSlopDrainageStraightDegree = tbl->colNew(String("SlopeDrainageStraightDegree"), Domain("value"), ValueRange(0,1e10,0.001));
	Column cSinuosity = tbl->colNew(String("Sinuosity"), Domain("value"), ValueRange(0,1e10,0.001));
	Column cTotalUpstreamLength = tbl->colNew(String("TotalUpstreamAlongDrainageLength"), Domain("value"));
  Column cTostreamCoord = tbl->colNew(String("TostreamCoord"), dmcrd);

  FileName fnDom(fnObj, ".dom");
	fnDom = FileName::fnUnique(fnDom);
	Domain dm = Domain(fnDom, 50, dmtCLASS);  
	Column cStrahlerClass = tbl->colNew(String("StrahlerClass"), Domain(fnDom));
	Coord c1, c2;
	long index = 0;
	double rUpstreamHeight, rDownstreamHeight;
  long iMaxOrder=0; 
	for (vector<RecordObj>::iterator pos = m_vRecords.begin(); 
								 pos < m_vRecords.end(); ++pos)
	{		
				RecordObj rec (*pos);
				cUpstreamLink->PutVal(rec.id, rec.UpstreamLink);

				RowCol rc = rec.UpstreamCoord;
				//rc.Row = rc.Row +1;
				//rc.Col = rc.Col +1;
				c1 = mp->gr()->cConv(rc);
				cUpstreamCoord->PutVal(rec.id, c1);

				rUpstreamHeight = m_vDem[rec.UpstreamCoord.Row][rec.UpstreamCoord.Col];
				cUpstreamHeight->PutVal(rec.id, rUpstreamHeight);
				
				cDownstreamLink->PutVal(rec.id, rec.DownstreamLink);
				
				rc = rec.DownstreamCoord;
				//rc.Row = rc.Row + 1;
				//rc.Col = rc.Col + 1;
				c2 = mp->gr()->cConv(rc);
				cDownstreamCoord->PutVal(rec.id, c2);

				rDownstreamHeight = m_vDem[rec.DownstreamCoord.Row][rec.DownstreamCoord.Col];
				cDownstreamHeight->PutVal(rec.id, rDownstreamHeight);
				
				double rDrop = rUpstreamHeight - rDownstreamHeight;
				cDrop->PutVal(rec.id, rDrop);
				
        if (rec.Strahler > iMaxOrder)
          iMaxOrder = rec.Strahler;
				cStrahler->PutVal(rec.id, rec.Strahler);
				cStreve->PutVal(rec.id, rec.Streve);
				cStrahlerClass->PutRaw(rec.id, rec.Strahler);
				cLength->PutVal(rec.id, rec.Length);
				
				double rStraightLength = rDistance(c1, c2);
				cStraightLength->PutVal(rec.id, rStraightLength);
				
				double rSlop = rComputeSlope(rDrop, rec.Length, false);
				cSlopAlongDrainage->PutVal(rec.id, rSlop);

				rSlop = rComputeSlope(rDrop, rec.Length, true);
				cSlopAlongDrainageDegree->PutVal(rec.id, rSlop);

				rSlop = rComputeSlope(rDrop, rStraightLength, false);
				cSlopDrainageStraight->PutVal(rec.id, rSlop);

				rSlop = rComputeSlope(rDrop, rStraightLength, true);
				cSlopDrainageStraightDegree->PutVal(rec.id, rSlop);

				double rSinuosity = rComputeSinuosity(rec.Length, rStraightLength);
				cSinuosity->PutVal(rec.id, rSinuosity);

				cTotalUpstreamLength->PutVal(rec.id, rec.TotalUpstreamLength);

        rc = rec.TostreamCoord;
				c2 = mp->gr()->cConv(rc);
				cTostreamCoord->PutVal(rec.id, c2);
				if (trq.fUpdate(index, (long)m_vRecords.size())) return;
				index++;
	}
  dm->pdsrt()->Resize(iMaxOrder);
	trq.fUpdate((long)m_vRecords.size(), (long)m_vRecords.size());
	m_vRecords.resize(0);
}

void MapDrainageNetworkOrdering::StoreSegment(SegmentMap sm, long iSegID, bool fConvert)
{
	ILWIS::Segment * seg;
	CoordBuf crdBuf((int)m_vStreamLink.size());

	for (long index=0; index < m_vStreamLink.size(); ++index)
	{		
		RowCol rc (m_vStreamLink[index]);
		if (fConvert)
			crdBuf[index] = mp->gr()->cConv(rc);
		else
			crdBuf[index] = Coord(rc.Col, rc.Row);
	}
	if (m_vStreamLink.size() > 0)
	{
		seg = CSEGMENT(sm->newFeature());
		seg->PutCoords(crdBuf.iSize(), crdBuf);
		seg->PutVal(iSegID);	
	}
}

void MapDrainageNetworkOrdering::PatchSegment(SegmentMap sm, long iSegID)
{
	for (int j=0; j<sm->iFeatures(); ++j)
	{
		ILWIS::Segment *segCur = (ILWIS::Segment *) sm->getFeature(j);
		if (!segCur || !segCur->fValid())
			continue;
		if (segCur->iValue() == iSegID)
		{
				CoordinateSequence *crdbuf1 = segCur->getCoordinates();
			segCur->Delete();
			long iCount = crdbuf1->size() + m_vStreamLink.size();
			CoordBuf crdbuf2(iCount);				
			for (int i = 0; i < crdbuf1->size(); ++i)
			{
				crdbuf2[i] = crdbuf1->getAt(i);
			}
			for (int i = 0; i < m_vStreamLink.size(); ++i)
			{
				crdbuf2[i+crdbuf1->size()] = Coord(m_vStreamLink[i].Col, m_vStreamLink[i].Row) ;
			}
			ILWIS::Segment * seg = CSEGMENT(sm->newFeature());
			seg->PutCoords(iCount, crdbuf2);
			seg->PutVal(iSegID);
			delete crdbuf1;
			break;
		}
	} // end of for
}

void MapDrainageNetworkOrdering::CleanSegment(SegmentMap smpTo, SegmentMap smpFrom)
{
	for (int j=0; j<smpFrom->iFeatures(); ++j)
	{
		ILWIS::Segment *segCur = (ILWIS::Segment *)smpFrom->getFeature(j);
		if (!segCur || !segCur->fValid())
			continue;
		
		CoordinateSequence *crdbuf = segCur->getCoordinates();
		for (int i = 0; i < crdbuf->size(); ++i) 
		{
			RowCol rc = RowCol(crdbuf->getAt(i).y, crdbuf->getAt(i).x);	
			crdbuf->setAt(mp->gr()->cConv(rc),i);
		}
		ILWIS::Segment * seg = CSEGMENT(smpTo->newFeature());
		seg->PutCoords(crdbuf);
		seg->PutVal(segCur->iValue());
	}

}

bool MapDrainageNetworkOrdering::fLatLonCoords()
{
	CoordSystemLatLon* csll = mp->cs()->pcsLatLon();
	return (0 != csll);
}

bool MapDrainageNetworkOrdering::fEllipsoidalCoords()
{
	CoordSystemViaLatLon* csviall = mp->cs()->pcsViaLatLon();
	bool fSpheric = true;
	if (csviall)
		fSpheric= (csviall->ell.fSpherical());
	return (0 != csviall &&  0 == fSpheric);
}

