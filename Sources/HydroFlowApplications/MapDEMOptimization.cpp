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
/* DEM-Optimization
   Copyright Ilwis System Development ITC
   June 2003, by Lichun Wang
*/
#include "HydroFlowApplications\MapDEMOptimization.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"
#include "Applications\Raster\SEGRAS.H"

IlwisObjectPtr * createMapDEMOptimization(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapDEMOptimization::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapDEMOptimization(fn, (MapPtr &)ptr);
}

using namespace std;

MapDEMOptimization* MapDEMOptimization::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
		Array<String> as;
		int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
		if (iParms < 2 || iParms > 6 )
			ExpressionError(sExpr, sSyntax());
		Map mp(as[0], fn.sPath());

		SegmentMap sm;
		PolygonMap pm;
		Table tblAtt;
		Column colBuf, colSmooth,colSharp;

		//pre-defied columns
		String sBufdist = "Buffer_dist";
		String sSmooth = "Smooth_drop";
		String sSharp = "Sharp_drop";

		//Drainage map --- segments 
		//iParms == 2 -- use linked table and predefined columns
		//iParms == 3 && as[2].rVal() == rUNDEF --- use the specified table with pre-defined columns
		//iParms == 6 && as[2].rVal() == rUNDEF --- use table with user specified columns
		FileName fnSeg = FileName(as[1]);

		if (iParms == 2 || (iParms == 3 && as[2].rVal() == rUNDEF) ||
			  (iParms == 6 && as[2].rVal() == rUNDEF)) 
		{
			if (fnSeg.sExt == ".mps" || fnSeg.sExt == "")
				sm = SegmentMap(fnSeg);
			else
				ExpressionError(sExpr, sSyntax());
						
			if (iParms == 2 || (iParms == 6 && fCIStrEqual(as[2].sVal(), "")))	// use linked table
			{
					tblAtt = sm->tblAtt();
					if (!tblAtt.fValid())
						throw ErrorNoAttTable(sm->fnObj);
			}
			else
					tblAtt = Table(as[2]);	//user specified table
			
			if (iParms == 6 && as[2].rVal() == rUNDEF)
			{
				sBufdist = as[3];	//user specified columns
				sSmooth = as[4];
				sSharp = as[5];
			}
			colBuf = tblAtt[sBufdist];
			if (!colBuf.fValid())
					ColumnNotFoundError(tblAtt->fnObj, sBufdist);

			colSmooth = tblAtt[sSmooth];
			if (!colSmooth.fValid())
					ColumnNotFoundError(tblAtt->fnObj, sSmooth);

			colSharp = tblAtt[sSharp];
			if (!colSharp.fValid())
					ColumnNotFoundError(tblAtt->fnObj, sSharp);

			return new MapDEMOptimization(fn, p, mp, sm, tblAtt->fnObj.sFile, sBufdist,sSmooth,sSharp); 
		}

		//check if use the numeric value for each of parameters
		if(iParms >= 4 && as[2].rVal() != rUNDEF && as[3].rVal() != rUNDEF)
 		{
			if (fnSeg.sExt == ".mps" || fnSeg.sExt == "")
				sm = SegmentMap(as[1]);
			else
				ExpressionError(sExpr, sSyntax());

			double rSharp;
			if (iParms == 4)
				rSharp = 0;
			else
			{ 
				if (as[2].rVal() != rUNDEF)
					rSharp = as[4].rVal();
				else
					ExpressionError(sExpr, sSyntax());
			}
			return new MapDEMOptimization(fn, p, mp, sm, as[2].rVal(),as[3].rVal(),rSharp); 
		}

		//drainage map --- polygons
		//the third parameter should be numeric
		if( iParms == 3 && as[2].rVal() != rUNDEF) 
		{
			FileName fnPol = FileName(as[1]);
			if (fnPol.sExt == ".mpa" || fnPol.sExt == "")
			{	
				pm = PolygonMap(fnPol);
				return new MapDEMOptimization(fn, p, mp, pm, as[2].rVal()); 
			}
		}

		ExpressionError(sExpr, sSyntax());

		return 0;
}

MapDEMOptimization::MapDEMOptimization(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
		
		String sdmt;
		ReadElement("MapDEMOptimization", "DEM", m_mp);
		ReadElement("MapDEMOptimization", "DrainageMapType", sdmt);

		ReadElement("MapDEMOptimization", "UseTable", m_IsUseTable);
		if (m_IsUseTable)
		{
				m_DrainageMapType = dmtSegment;
				ReadElement("MapDEMOptimization", "DrainageMap", m_sm);	
				ReadElement("MapDEMOptimization", "Table", m_sTbl);
				ReadElement("MapDEMOptimization", "Buffer", m_sBufdist);
				ReadElement("MapDEMOptimization", "Smooth_drop", m_sSmooth);
				ReadElement("MapDEMOptimization", "Sharp_drop", m_sSharp);
		}
		else if (fCIStrEqual(sdmt, "segment"))
		{
				m_DrainageMapType = dmtSegment;
				ReadElement("MapDEMOptimization", "DrainageMap", m_sm);
				ReadElement("MapDEMOptimization", "Buffer", m_rBufdist);
				ReadElement("MapDEMOptimization", "Smooth_drop", m_rSmooth);
				ReadElement("MapDEMOptimization", "Sharp_drop", m_rSharp);
		}
		else
		{
				m_DrainageMapType = dmtPolygon;
				ReadElement("MapDEMOptimization", "DrainageMap", m_pm);
				ReadElement("MapDEMOptimization", "Sharp_drop", m_rSharp);
		}
		
		Init();
}

MapDEMOptimization::MapDEMOptimization(const FileName& fn, MapPtr& p, 
																			 const Map& mp, const SegmentMap& sm,
																			 String sTbl,
																			 String sBufdist,
																			 String sSmooth,
																			 String sSharp)
	: MapFromMap(fn, p, mp),
		m_sTbl(sTbl),
		m_sBufdist(sBufdist), 
		m_sSmooth(sSmooth),
		m_sSharp(sSharp),
		m_sm(sm),
		m_mp(mp),
		m_IsUseTable(true),
		m_DrainageMapType(dmtSegment)
{
		
		Init();	
}

MapDEMOptimization::MapDEMOptimization(const FileName& fn, MapPtr& p, 
																			 const Map& mp, const SegmentMap& sm,
																			 double rBufdist,
																			 double rSmooth,
																			 double rSharp)
	: MapFromMap(fn, p, mp),
		m_rBufdist(rBufdist), 
		m_rSmooth(rSmooth),
		m_rSharp(rSharp),
		m_sm(sm),
		m_mp(mp),
		m_IsUseTable(false),
		m_DrainageMapType(dmtSegment)
{
		Init();
}

MapDEMOptimization::MapDEMOptimization(const FileName& fn, MapPtr& p, const Map& mp, const PolygonMap& pm,double rSharp)
: MapFromMap(fn, p, mp),
	m_rSharp(rSharp),
	m_pm(pm),
	m_mp(mp),
	m_DrainageMapType(dmtPolygon),
	m_IsUseTable(false)
{
		Init();
}

void MapDEMOptimization::Init()
{
		fNeedFreeze = true;
		sFreezeTitle = "MapDEMOptimization";
		htpFreeze = "ilwisapp\\dem_optimization_algorithm.htm";
		
		switch (m_DrainageMapType)
		{
			case dmtSegment:
				if (m_IsUseTable)
				{
					Table tblAtt = Table(m_sTbl);

					Column cSmooth = tblAtt[m_sSmooth];
					if (!cSmooth.fValid())
						ColumnNotFoundError(tblAtt->fnObj, m_sSmooth);
					
					Column cSharp = tblAtt[m_sSharp];
					if (!cSharp.fValid())
						ColumnNotFoundError(tblAtt->fnObj, m_sSharp);
						
					Column cBuffer = tblAtt[m_sBufdist];
					if (!cBuffer.fValid())
						ColumnNotFoundError(tblAtt->fnObj, m_sBufdist);
					
					objdep.Add(tblAtt);
					objdep.Add(cSmooth);
					objdep.Add(cSharp);
					objdep.Add(cBuffer);
				}
				objdep.Add(m_sm);
				break;
			case dmtPolygon:
				objdep.Add(m_pm);
				break;
		}
		if (!fnObj.fValid())
			objtime = objdep.tmNewest();
		ptr.Store();
}
	
const char* MapDEMOptimization::sSyntax() {
	
	char* sSt = "MapDEMOptimization(DEM,Segment)\n"
							"MapDEMOptimization(DEM,Segment,Table)\n"
							"MapDEMOptimization(DEM,Segment,,ColBuffer,ColSmoothDrop,ColSharpDrop)\n"
							"MapDEMOptimization(DEM,Segment,Table,ColBuffer,ColSmoothDrop,ColSharpDrop)\n"
							"MapDEMOptimization(DEM,Segment,Buffer,SmoothDrop,SharpDrop)\n"
							"MapDEMOptimization(DEM,Polygon,SharpDrop)\n";
														
	return sSt;
}

void MapDEMOptimization::Store()
{
		MapFromMap::Store();
		WriteElement("MapFromMap", "Type", "MapDEMOptimization");
		WriteElement("MapDEMOptimization", "DEM", m_mp);		
		WriteElement("MapDEMOptimization", "UseTable", m_IsUseTable);

		if (m_IsUseTable)
		{ 
			  WriteElement("MapDEMOptimization", "DrainageMapType", "Segment");		
				WriteElement("MapDEMOptimization", "DrainageMap", m_sm->fnObj.sFile);
				WriteElement("MapDEMOptimization", "Table", m_sTbl);
				WriteElement("MapDEMOptimization", "Buffer", m_sBufdist);
				WriteElement("MapDEMOptimization", "Smooth_drop", m_sSmooth);
				WriteElement("MapDEMOptimization", "Sharp_drop", m_sSharp);

		}
		else 
		{
				switch (m_DrainageMapType)
				{
					case dmtSegment:
						WriteElement("MapDEMOptimization", "DrainageMapType", "Segment");		
						WriteElement("MapDEMOptimization", "DrainageMap", m_sm->fnObj.sFile);
						WriteElement("MapDEMOptimization", "Buffer", m_rBufdist);
						WriteElement("MapDEMOptimization", "Smooth_drop", m_rSmooth);
						WriteElement("MapDEMOptimization", "Sharp_drop", m_rSharp);
						break;
					case dmtPolygon:
						WriteElement("MapDEMOptimization", "DrainageMapType", "Polygon");		
						WriteElement("MapDEMOptimization", "DrainageMap", m_pm->fnObj.sFile);
						WriteElement("MapDEMOptimization", "Sharp_drop", m_rSharp);
						break;
				}
		}
		
}

String MapDEMOptimization::sExpression() const
{
		
		switch (m_DrainageMapType)
		{
					case dmtSegment:
						if (m_IsUseTable)
						{

							return String("MapDEMOptimization(%S,%S,%S,%S,%S,%S)", 
									mp->fnObj.sRelativeQuoted(), 
									m_sm->fnObj.sRelativeQuoted(),
									FileName(m_sTbl).sRelativeQuoted(),
									m_sBufdist, 
									m_sSmooth, 
									m_sSharp);
						}
						else
							return String("MapDEMOptimization(%S,%S,%g,%g,%g)", 
									mp->fnObj.sRelativeQuoted(), 
									m_sm->fnObj.sRelativeQuoted(),
									m_rBufdist, 
									m_rSmooth, 
									m_rSharp);
						break;
					case dmtPolygon:
						return String("MapDEMOptimization(%S,%S,%g)", 
									mp->fnObj.sRelativeQuoted(), 
									m_pm->fnObj.sRelativeQuoted(),
									m_rSharp);
						break;
		}

		return String();
}

bool MapDEMOptimization::fGeoRefChangeable() const
{
  return false;
}

bool MapDEMOptimization::fDomainChangeable() const
{
  return false;
}
bool MapDEMOptimization::fValueRangeChangeable() const
{
  return false;
}

bool MapDEMOptimization::fFreezing()
{
		trq.SetTitle(sFreezeTitle);
		trq.SetText(TR("Reading DEM"));
		trq.Start();
		
		Table tbl;
		Column colBuffer;
		Column colSmoothDrop;
		Column colSharpDrop;

		Map mpTmpDist;
		Map mpTmpThies;
		Map mpTmpPol;
		switch (m_DrainageMapType)
		{
			case dmtSegment:
				{
					//Rasterize the segment map 
					FileName fnTmpSeg = FileName::fnUnique(m_mp->fnObj);
					String sExprSeg("MapRasterizeSegment(%S, %S)", m_sm->fnObj.sFullPathQuoted(), 
			                                          m_mp->gr()->fnObj.sFullPathQuoted());
					Map mpTmpSeg = Map(fnTmpSeg, sExprSeg);
					mpTmpSeg->Calc();
					mpTmpSeg->fErase = true;
		
					FileName fnTmpDist = FileName::fnUnique(m_mp->fnObj);
					FileName fnTmpThies = FileName::fnUnique(mpTmpSeg->fnObj);
					if (m_IsUseTable)
					{
						tbl = Table(m_sTbl);
						colBuffer = tbl->col(m_sBufdist);
						colSmoothDrop = tbl->col(m_sSmooth);
						colSharpDrop = tbl->col(m_sSharp);

						//Compute the distance and Thiessen maps
						String sNull = "";
						String sExprThies("MapDistance(%S,%S,%S)", mpTmpSeg->fnObj.sFullPathQuoted(),
																											 sNull,
																											 fnTmpThies.sFullNameQuoted()); 
						mpTmpDist = Map(fnTmpDist, sExprThies);
						mpTmpDist->Calc();
						mpTmpDist->fErase = true;

						mpTmpThies = Map(fnTmpThies);
						mpTmpThies->fErase = true;
					}	
					else
					{
						//Compute the distance map only 
						String sExprDist("MapDistance(%S)", mpTmpSeg->fnObj.sFullPathQuoted()); 
						mpTmpDist = Map(fnTmpDist, sExprDist);
						mpTmpDist->Calc();
						mpTmpDist->fErase = true;
					}
				}
					break;
			case dmtPolygon:
				{
					//Rasterize the polygon map 
					FileName fnTmpPol = FileName::fnUnique(m_mp->fnObj);
					String sExprPol("MapRasterizePolygon(%S, %S)", m_pm->fnObj.sFullPathQuoted(), 
						                                        m_mp->gr()->fnObj.sFullPathQuoted());
					mpTmpPol = Map(fnTmpPol, sExprPol);
					mpTmpPol->Calc();
					mpTmpPol->fErase = true;
				}
					break;
		}
		
		vector<RealBuf> vDem;  //vector for the output DEM
		vDem.resize(iLines());
		trq.SetText(TR("Calculating Optimized DEM"));
		RangeReal rrInput  = m_mp->rrMinMax();
		double rMax = rrInput.rHi();
		double rMin = rrInput.rLo();
		long iRow = 0;
		for ( ;iRow < iLines(); iRow++)
		{
			vector<double> vDemVal;
			RealBuf rbDem; // = vDem[iRow];
			RealBuf rbDist;
			LongBuf lbPol;
			
			vDemVal.resize(0);
			rbDem.Size(iCols()); 
			mp->GetLineVal(iRow, rbDem);

			if (m_DrainageMapType == dmtSegment)
			{		
				rbDist.Size(iCols());
				mpTmpDist->GetLineVal(iRow, rbDist);
			}
			else
			{
				lbPol.Size(iCols());
				mpTmpPol->GetLineRaw(iRow, lbPol);
			}
			for(long iCol = 0; iCol < iCols(); iCol++)
			{	
				switch (m_DrainageMapType)
				{		
					case dmtSegment:
						if (m_IsUseTable) 
						{
							long iRaw = mpTmpThies->iRaw(RowCol(iRow, iCol));
							m_rBufdist = colBuffer->rValue(iRaw);
							m_rSmooth = colSmoothDrop->rValue(iRaw);
							m_rSharp = colSharpDrop->rValue(iRaw);
						}
						if (rbDist[iCol] == 0 && rbDist[iCol] != rUNDEF) {
							//on the segment
							rbDem[iCol] = rbDem[iCol] - m_rSmooth - m_rSharp;
						}
						else if (rbDist[iCol] <= m_rBufdist && rbDist[iCol] != rUNDEF) 
							rbDem[iCol] = rbDem[iCol]	- m_rSmooth * ((m_rBufdist - rbDist[iCol]) / m_rBufdist);
						break;
					case dmtPolygon:
						if (lbPol[iCol] != iUNDEF)
							rbDem[iCol] -= m_rSharp;
						break;
				}
				vDemVal.push_back(rbDem[iCol]);
			}	
			//put the computed DEM to vDEM for the output map  
			vDem[iRow].Size(iCols());
			vDem[iRow] = rbDem;

			vector<double>::iterator pos;
			pos = max_element(vDemVal.begin(), vDemVal.end());
			rMax = *pos > rMax ? *pos : rMax ;
			pos = min_element(vDemVal.begin(), vDemVal.end());
			rMin = *pos < rMin ? *pos : rMin;
			if (trq.fUpdate(iRow, iLines())) return false;
		}
		//returns the position of the first element with max/min.
		ValueRange vrOutput = ValueRange(rMin,rMax,m_mp->dvrs().rStep());
		SetDomainValueRangeStruct(vrOutput);		
		CreateMapStore();
		trq.fUpdate(iRow, iLines());
		WriteData(vDem);
		vDem.resize(0);
		return true;
}

bool MapDEMOptimization::WriteData(vector<RealBuf>& vData)
{
	//write DEM to disk 
	trq.SetText(TR("Write output map"));
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		ptr.PutLineVal(iRow, vData[iRow]);
		if (trq.fUpdate(iRow, iLines())) return false;	
	}
	trq.fUpdate(iLines(), iLines());
	return true;
}

MapDEMOptimization::~MapDEMOptimization()
{

}
