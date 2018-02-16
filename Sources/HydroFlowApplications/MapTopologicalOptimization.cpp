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

#include "HydroFlowApplications\MapTopologicalOptimization.h"
#include "Engine\Table\tbl.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapTopologicalOptimization(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapTopologicalOptimization::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapTopologicalOptimization(fn, (MapPtr &)ptr);
}

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapTopologicalOptimization::MapTopologicalOptimization(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	ReadElement("MapTopologicalOptimization", "FlowDirection", m_mpFlow);
	ReadElement("MapTopologicalOptimization", "DrainageMap", m_mpDrainage);
  ReadElement("MapTopologicalOptimization", "StreamSegmentMap", m_smStream);
  ReadElement("MapTopologicalOptimization", "ModifiedFlowDirectionMap", m_sModifiedflowMap);
  
	fNeedFreeze = true;
  sFreezeTitle = "MapTopologicalOptimization";
	htpFreeze = "ilwisapp\\topological_optimization_algorithm.htm";
}

MapTopologicalOptimization::MapTopologicalOptimization(const FileName& fn, 
																											 MapPtr& p,
																											 const Map& mpDrainage,
																											 const Map& mpFlowDir,
																											 const SegmentMap& segmpStream,
                                                       String sModifiedFlowMap)
: MapFromMap(fn, p, mpDrainage),
  m_mpDrainage(mpDrainage),
  m_mpFlow(mpFlowDir),
  m_smStream(segmpStream),
  m_sModifiedflowMap(sModifiedFlowMap)
{
	fNeedFreeze = true;
	sFreezeTitle = "MapTopologicalOptimization";
	htpFreeze = "ilwisapp\\topological_optimization_algorithm.htm";

	objdep.Add(m_mpFlow);
	objdep.Add(m_smStream);

  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.Store(); 
}

MapTopologicalOptimization::~MapTopologicalOptimization()
{
}

const char* MapTopologicalOptimization::sSyntax()
{ 
  return "MapTopologicalOptimization(DrainageNetworkMap, FlowDirectionMap, StreamSegmentMap, ModifiedFlowDirectionMap)";
}

MapTopologicalOptimization* MapTopologicalOptimization::create(const FileName& fn, 
																															 MapPtr& p, 
																															 const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms != 4 )
      ExpressionError(sExpr, sSyntax());

  Map mpDrainage(as[0], fn.sPath());
	Map mpflowdir(as[1], fn.sPath());
	SegmentMap smStream(as[2], fn.sPath());
	
	
	Domain dm = mpDrainage->dm();
	if (!(dm.fValid() && (fCIStrEqual(dm->fnObj.sFileExt(), "bool.dom"))))
			throw ErrorObject(WhatError(TR("Map should have domain bool"), errMapDrainageNetworkOrdering), mpDrainage->fnObj);

	dm = mpflowdir->dm();
	if (!(dm.fValid() && (fCIStrEqual(dm->fnObj.sFileExt(), "FlowDirection.dom"))))
			throw ErrorObject(WhatError(TR("Use an input map with domain FlowDirection   "), errMapDrainageNetworkOrdering), mpflowdir->fnObj);

	bool fIncompGeoRef = false;
	if (mpDrainage->gr()->fGeoRefNone() && mpflowdir->gr()->fGeoRefNone())
		  fIncompGeoRef = mpDrainage->rcSize() != mpflowdir->rcSize();
	else
			fIncompGeoRef = mpDrainage->gr() != mpflowdir->gr();
	if (fIncompGeoRef)
			throw ErrorIncompatibleGeorefs(mpDrainage->gr()->sName(true, fn.sPath()),
            mpflowdir->gr()->sName(true, fn.sPath()), fn, errMapDrainageNetworkOrdering);
  
  String sModifiedFlowMap = as[3].sVal();

	return new MapTopologicalOptimization(fn, p, mpDrainage, mpflowdir, smStream, sModifiedFlowMap);
}

void MapTopologicalOptimization::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapTopologicalOptimization");
  WriteElement("MapTopologicalOptimization", "DrainageMap", m_mpDrainage);
	WriteElement("MapTopologicalOptimization", "FlowDirection", m_mpFlow);
  WriteElement("MapTopologicalOptimization", "StreamSegmentMap", m_smStream);
  WriteElement("MapTopologicalOptimization", "ModifiedFlowDirectionMap", m_sModifiedflowMap);
}

String MapTopologicalOptimization::sExpression() const
{
  
	String sExp("MapTopologicalOptimization(%S,%S, %S, %S)", mp->sNameQuoted(true, fnObj.sPath()),
               m_mpFlow->sNameQuoted(true), m_smStream->sNameQuoted(true), m_sModifiedflowMap);
  return sExp;
}

bool MapTopologicalOptimization::fDomainChangeable() const
{
  return false;
}

bool MapTopologicalOptimization::fGeoRefChangeable() const
{
  return false;
}

bool MapTopologicalOptimization::fFreezing()
{
	trq.SetTitle(sFreezeTitle);
	trq.SetText(TR("Initialize map"));
	trq.Start();

	//Rasterize the stream segments
	Map tmpRasMap = RasterizeStream();
	tmpRasMap->fErase = true;
	ReadInputMaps(tmpRasMap);

	trq.SetText(TR("Topological Optimization"));
	bool fTransformCoords = cs() != m_smStream->cs();
	for (int j=0; j <m_smStream->iFeatures(); ++j){
		ILWIS::Segment *seg = (ILWIS::Segment *)m_smStream->getFeature(j);
		if (!seg || !seg->fValid())
			continue;

		long iPoints;
		CoordinateSequence *crdbufFrom = seg->getCoordinates();
		iPoints = crdbufFrom->size();
		Coord crdIn = crdbufFrom->getAt(0);
		Coord crdOut = crdbufFrom->getAt(iPoints-1);
		if (fTransformCoords)
		{
			crdIn = cs()->cConv(m_smStream->cs(), crdIn);
			crdOut = cs()->cConv(m_smStream->cs(), crdOut);
		}
		RowCol rcIn = gr()->rcConv(crdIn);
		RowCol rcOut = gr()->rcConv(crdOut);
		AddStreams(rcIn,rcOut,seg->iValue());
		delete crdbufFrom;
		if (trq.fUpdate(j, m_smStream->iFeatures())) return false;
	}

	FileName fnModifiedflowMap(m_sModifiedflowMap, fnObj);
	fnModifiedflowMap.sExt = ".mpr";
	Map mpModFlow(FileName::fnUnique(fnModifiedflowMap), gr(), rcSize(), Domain("FlowDirection"));
	trq.SetText(TR("Write output map"));
	for (long iRow = 0; iRow < iLines(); iRow++ )
	{
		ByteBuf& bFlow = m_vFlowDir[iRow];
		mpModFlow->PutLineRaw(iRow, bFlow);

		ByteBuf& bDraingae = m_vDrainageMap[iRow];
		ptr.PutLineRaw(iRow, bDraingae);
		if (trq.fUpdate(iRow, iLines())) return false;
	}
	trq.fUpdate(iLines(), iLines());
	m_vDrainageMap.Close();
	m_vFlowDir.Close();
	m_vStream.Close();
	return true;
}

bool MapTopologicalOptimization::IsEdgeCell(long iRow, long iCol)
{
		if (iRow <= 0 || iRow >= iLines() - 1 ||
				iCol <= 0 || iCol >= iCols() - 1)
				return true;
		else
				return false;
}

Map MapTopologicalOptimization::RasterizeStream()
{
  if (0 == m_smStream->sms()) {
    throw ErrorObject(TR("Error of stream segment map "));
  }
  FileName fnStream = m_smStream->fnObj;
  fnStream.sExt = ".mpr"; 
  fnStream = FileName::fnUnique(fnStream);
  String sExprRas("MapRasterizeSegment(%S, %S)", m_smStream->fnObj.sFullPathQuoted(), mp->gr()->fnObj.sFullNameQuoted()); 
	 
	Map tmpRasMap = Map(fnStream, sExprRas);
	tmpRasMap->Calc();
  return tmpRasMap;
}

void MapTopologicalOptimization::ReadInputMaps(Map mpTmp)
{
	m_vDrainageMap.Open(iLines(), iCols());
	m_vFlowDir.Open(iLines(), iCols());
	m_vStream.Open(iLines(), iCols());
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		//Reading input drainage map
		m_vDrainageMap[iRow].Size(iCols()); 
		m_mpDrainage->GetLineRaw(iRow, m_vDrainageMap[iRow]);

		//Reading input flow direction map
		m_vFlowDir[iRow].Size(iCols()); 
		m_mpFlow->GetLineRaw(iRow, m_vFlowDir[iRow]);
		
    //Reading input stream raster map
		m_vStream[iRow].Size(iCols()); 
		mpTmp->GetLineRaw(iRow, m_vStream[iRow]);

		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
}

void MapTopologicalOptimization::AddStreams(RowCol rcIn, RowCol rcOut, long id)
{
  if (ReLocateInOutCell(rcOut))
    m_vStream[rcOut.Row][rcOut.Col] = id;
  if (ReLocateInOutCell(rcIn))
  {
    m_vStream[rcIn.Row][rcIn.Col] = id;
    vector<int> vFlowDir;
    vector<int> vFlow2Self;
    InitPars(vFlowDir, vFlow2Self);
    bool fAdd = true;
    RowCol curCell = rcIn;
    while(fAdd)
    {
      int iLocation = 0;  
      for (int i=-1; i<2; ++i)
	    {
        bool fBreak = false;
		    for (int j=-1; j<2; ++j)
		    {
          RowCol rc;
          rc.Row = curCell.Row+i;
          rc.Col = curCell.Col+j;
          bool fStreamCell = m_vStream[rc.Row][rc.Col] == id;
          if (IsEdgeCell(rc.Row, rc.Col) != true && fStreamCell && iLocation != 4)
          {
            m_vDrainageMap[curCell.Row][curCell.Col] = 2;
            m_vFlowDir[curCell.Row][curCell.Col] = vFlowDir[iLocation];
            m_vStream[curCell.Row][curCell.Col] = iUNDEF;
            if (rc == rcOut)
            {
              m_vDrainageMap[rc.Row][rc.Col] = 2;
              if(m_vFlowDir[rc.Row][rc.Col] == vFlow2Self[iLocation])
                m_vFlowDir[rc.Row][rc.Col]  = 0;
              fAdd = false;
            }
            curCell = rc;
            fBreak = true;
            break;
          }
          else if(iLocation == 8)
          {
            fAdd = false; //force to stop avoiding from endless loop
          }
          iLocation++;
        }
        if (fBreak)
            break;
	    }
    } //while()
  }  
}

bool MapTopologicalOptimization::ReLocateInOutCell(RowCol& curCell)
{
  //If the specified in-out cell not located on drainage, 3 by 3 window is used to
  //find the nearest drainage pixel as In or Out pixels
  int ival = m_vDrainageMap[curCell.Row][curCell.Col];
  bool fDrainageCell = m_vDrainageMap[curCell.Row][curCell.Col] == 2;
  if ( fDrainageCell )
     return true;
  for (int i=-1; i<2; ++i)
	{
		for (int j=-1; j<2; ++j)
		{
      RowCol rc;
      rc.Row = curCell.Row+i;
      rc.Col = curCell.Col+j;
      fDrainageCell = m_vDrainageMap[rc.Row][rc.Col] == 2;
      if (IsEdgeCell(rc.Row, rc.Col) != true && fDrainageCell)
      {
        curCell=rc; 
        return true;	 
      }
    }
	}
  return false;
}

void MapTopologicalOptimization::InitPars(vector<int>& vDir,vector<int>& vFlow2Self )
{
		//	Location number				Order in m_vDirection
		//	-------								-------	 loop order of the neighbors 	
	  //	|6|7|8|								|0|1|2|
		//	-------								-------
		//	|5| |1|								|3|4|5|
		//	-------								-------
		//	|4|3|2|								|6|7|8|
		//	-------								-------
		//
		vDir.resize(9);
		vDir[0] = 6;
		vDir[1] = 7;
		vDir[2] = 8;
		vDir[3] = 5;
		vDir[4] = 0;
		vDir[5] = 1;
		vDir[6] = 4;
		vDir[7] = 3;
		vDir[8] = 2;

    vFlow2Self.resize(9);
    vFlow2Self[0] = 2;
		vFlow2Self[1] = 3;
		vFlow2Self[2] = 4;
		vFlow2Self[3] = 1;
		vFlow2Self[4] = 0;
		vFlow2Self[5] = 5;
		vFlow2Self[6] = 8;
		vFlow2Self[7] = 7;
		vFlow2Self[8] = 6;
}
