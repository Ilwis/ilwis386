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
// MapFillSinks.cpp: implementation of the MapFillSinks class.
//
//////////////////////////////////////////////////////////////////////
/* MapFillSinks
   Copyright Ilwis System Development ITC
   August 2003, by Lichun Wang
*/

//#include "MapFillSinks.h"
//#include "AdditionalHeaders\Htp\ilwisapp.htp"
//#include "AdditionalHeaders\Err\ilwisapp.err"
//#include "AdditionalHeaders\hs\map.hs"
#include "HydroFlowApplications\MapFillSinks.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapFillSinks(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapFillSinks::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapFillSinks(fn, (MapPtr &)ptr);
}

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MapFillSinks::MapFillSinks(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	String sMethod;
	ReadElement("MapFillSinks", "Method", sMethod);
	if (fCIStrEqual(sMethod, "fill"))
			m_fmMethods = fmFill;
		else
			m_fmMethods = fmCut;
	
	fNeedFreeze = true;
	sFreezeTitle = "MapFillSinks";
	htpFreeze = "ilwisapp\\fill_sinks_algorithm.htm";
}

MapFillSinks::MapFillSinks(const FileName& fn, MapPtr& p, const Map& mp, FillMethod sMethod)
: MapFromMap(fn, p, mp), m_fmMethods(sMethod)
{
		fNeedFreeze = true;
		if (!fnObj.fValid())
			objtime = objdep.tmNewest();
		ptr.Store(); // store domain and att table info
}

MapFillSinks::~MapFillSinks()
{

}

const char* MapFillSinks::sSyntax() {
	return "MapFillSinks(DEM [,fill|cut])";	 
}

MapFillSinks* MapFillSinks::create(const FileName& fn, MapPtr& p, const String& sExpr)
{

	
	Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 1 || iParms > 3) 
		ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  FillMethod  sMethod;
  if (iParms > 1) {
    String sMtd = as[1].sVal();
    if (!(fCIStrEqual(sMtd, "fill") || fCIStrEqual(sMtd, "cut")))
      ExpressionError(sExpr, sSyntax());
		if (fCIStrEqual(sMtd, "fill"))
			sMethod = fmFill;
		else
			sMethod = fmCut;
  }
  else
		sMethod = fmFill;	
  
  return new MapFillSinks(fn, p, mp, sMethod);
}

void MapFillSinks::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapFillSinks");
  String sMethod;
  switch (m_fmMethods)
  {
		case fmFill:
			sMethod = "fill";
			break;
		case fmCut:
			sMethod = "cut";
			break;
	}
  WriteElement("MapFillSinks", "Method", sMethod);
}

String MapFillSinks::sExpression() const
{
  
	String sMethod;
	switch (m_fmMethods)
	{
		case fmFill:
			sMethod = "fill";
			break;
		case fmCut:
			sMethod = "cut";
			break;
	}
  String sExp("MapFillSinks(%S,%S)", mp->sNameQuoted(true, fnObj.sPath()),sMethod);
  return sExp;
}

bool MapFillSinks::fDomainChangeable() const
{
  return false;
}

bool MapFillSinks::fGeoRefChangeable() const
{
  return false;
}

bool MapFillSinks::fFreezing()
{

	m_sinkPixelsThreshold = 2; //TO initiallize!!!
		/*Two vector m_vSinksFilled and m_vSinksFlagged are needed
		m_vDEM -- 1. stores the input DEM, 
											2. modified by depression filling algorithm,
											3. it will be wirtten to the output map

		m_vFlag - 1. Each element is initialled to zero; 
										  2. Elements in depres.contributing area are flagged,
										     when the the depres. area are defined;
											3. Elements in flat area flagged to -1*/
		
		m_vDEM.resize(iLines());  //allocate memory for vectors 
		m_vFlag.resize(iLines());
		m_vSinks.resize(0);
		trq.SetTitle(sFreezeTitle);
		trq.SetText(TR("Reading DEM"));
		trq.Start();
		for (long iRow = 0; iRow < iLines(); iRow++)
		{
				//reading DEM  
				m_vDEM[iRow].Size(iCols()); 
				mp->GetLineVal(iRow, m_vDEM[iRow]);

				m_vFlag[iRow].Size(iCols());
				for (long iCol = 0; iCol < iCols(); iCol++)
				{
					m_vFlag[iRow][iCol] = 0;
				}
				if (trq.fUpdate(iRow, iLines())) return false;	
		}
		trq.fUpdate(iLines(), iLines());

		m_iFlag = 0;
		//Fill single-cell sinks
		SingleSinkFill();

		//Fill sinks based on the user specified threshold
		if (m_fmMethods == fmCut)
			GroupSinksFill();

		//scan DEM seeking an initial sink
		RowCol rcSink;
		m_iFlag = 0;
		if (m_fmMethods == fmFill)
			trq.SetText(TR("Fill sinks"));
		else
			trq.SetText("Cut terrain");
		for (long iRow = 1; iRow < iLines()-1; ++iRow)
		{
			rcSink.Row = iRow;	
			for (long iCol = 1; iCol < iCols()-1; ++iCol)  
			{
				rcSink.Col = iCol;
				if (fLocateInitialSink(rcSink))
				{
						/*increment flag variable, locate the extent of the 
						contributing sink area*/
						m_iFlag++; //increment by the number of init. sink
						FindSinkContArea(rcSink);	
						
						//Identify outlet cell
						RowCol rcOutlet;
						if (fIdentifyOutletCell(rcSink, rcOutlet))
						{
							/*if the elevation of outlet is greater than that of the
							 *initial sink then fill the depressions, otherwise
							 *if they are equal to, no filling is needed*/
						   if (m_fmMethods == fmFill){	
							  if (m_vDEM[rcOutlet.Row][rcOutlet.Col] > 
									m_vDEM[rcSink.Row][rcSink.Col])
							  {
									DepresFill(rcOutlet);
							  }
							  else
									FlatAreaFlag(rcOutlet);
						   }
						   else
							  CutTerrain(rcOutlet);	
						}
						else
							FlatAreaFlag(rcOutlet);
						m_vSinks.clear();
				}
			}
			if (trq.fUpdate(iRow, iLines())) return false;
		}
		trq.SetText(TR("Write output map"));

		for (long iRow = 0; iRow < iLines(); iRow++)
		{
				ptr.PutLineVal(iRow, m_vDEM[iRow]);
				if (trq.fUpdate(iRow, iLines())) return false;	
		}
		trq.fUpdate(iLines(), iLines());
		//Final clean up
		m_vDEM.resize(0);
		m_vFlag.resize(0);
		m_vSinks.resize(0);
		return true;
}

void MapFillSinks::SingleSinkFill()
{
		/*Single-cell sink:height is lower than neighbors
		  Filling by raising its elevation to the lowest neighbor*/
		vector<double>::iterator pos;
		
		trq.SetText(TR("Fill single-cell depressions"));
		for (long iRow = 0; iRow < iLines(); ++iRow)
		{
			for (long iCol = 0; iCol < iCols(); ++iCol)  
			{
					//flag the cells in teh edge or adjacent to UNDEF
					if (IsEdgeCell(iRow, iCol))		
							m_vFlag[iRow][iCol] = -2;
					else if (m_vDEM[iRow][iCol] == rUNDEF)
							FlagNeighbors(iRow, iCol);
			}
			trq.fUpdate(iRow, iLines());	
		}
		
		for (long iRow = 1; iRow < iLines() - 1; ++iRow)
		{
			for (long iCol = 1; iCol < iCols() - 1; ++iCol)  
			{
					if (!IsUndef(iRow, iCol))
					{	
							vector<double> vNbs;
							GetNeighborCells(iRow, iCol, vNbs);
							pos = min_element(vNbs.begin(), vNbs.end());
							double rMin = *pos;
							if (m_vDEM[iRow][iCol] < rMin)
										m_vDEM[iRow][iCol] = rMin;
					}
			}
			trq.fUpdate(iRow, iLines());	
		}
}

//Fill sinks based on the threshold -  Nr. of pixels specified
void MapFillSinks::GroupSinksFill()
{
		//scan DEM seeking an initial sink
		RowCol rcSink;
		
		trq.SetText("Fill sinks based on threshold of sink-pixels");
		for (long iRow = 1; iRow < iLines()-1; ++iRow)
		{
			rcSink.Row = iRow;	
			for (long iCol = 1; iCol < iCols()-1; ++iCol)  
			{
				rcSink.Col = iCol;
				if (fLocateInitialSink(rcSink))
				{
					m_sinkHeight = m_vDEM[rcSink.Row][rcSink.Col];
					/*increment flag variable, locate the extent of the 
					contributing sink area*/
					m_iFlag++; //increment by the number of init. sink
					FindSinkContArea2(rcSink);
					if (m_sinkPixels <= m_sinkPixelsThreshold)
					{	
						//Identify outlet cell
						RowCol rcOutlet;
						if (fIdentifyOutletCell(rcSink, rcOutlet))
						{
							/*if the elevation of outlet is greater than that of the
							 *initial sink then fill the depressions, otherwise
							 *if they are equal to, no filling is needed*/
							if (m_vDEM[rcOutlet.Row][rcOutlet.Col] > 
								m_vDEM[rcSink.Row][rcSink.Col])
							{
								DepresFill(rcOutlet);
							}
							else
								FlatAreaFlag(rcOutlet);
						}
						else
							FlatAreaFlag(rcOutlet);
						m_vSinks.clear();
					}
				}
			}
			trq.fUpdate(iRow, iLines());
		}
		for (long iRow = 0; iRow < iLines(); iRow++)
		{
				for (long iCol = 0; iCol < iCols(); iCol++)
				{
					if ( m_vFlag[iRow][iCol] > 0)
						m_vFlag[iRow][iCol] = 0;
				}
		}
}

bool MapFillSinks::IsUndef(long iRow, long iCol)
{
		//check to see if it is a cell with undef 
		if (m_vFlag[iRow][iCol] == -3) 
				return true;
		else 
				return false;
}

void MapFillSinks::FlagNeighbors(long iRow, long iCol)
{
		//flag all neighbors to -1, if it is a cell with undef of elevation
		//or it has an undef neighbor
		for (int i=-1; i<=1; ++i)
		{
			for(int j=-1; j<=1; ++j)
				m_vFlag[iRow+i][iCol+j] = -3;
		}
}

void MapFillSinks::GetNeighborCells(long iRow, long iCol, vector<double>& vNeighbors)
{
		//Store the neighbor's value of elevation 
		vNeighbors.push_back(m_vDEM[iRow][iCol+1]);
		vNeighbors.push_back(m_vDEM[iRow+1][iCol+1]);
		vNeighbors.push_back(m_vDEM[iRow+1][iCol]);
		vNeighbors.push_back(m_vDEM[iRow+1][iCol-1]);
		vNeighbors.push_back(m_vDEM[iRow][iCol-1]);
		vNeighbors.push_back(m_vDEM[iRow-1][iCol-1]);
		vNeighbors.push_back(m_vDEM[iRow-1][iCol]);
		vNeighbors.push_back(m_vDEM[iRow-1][iCol+1]);
}

bool MapFillSinks::fLocateInitialSink(RowCol& rc)
{
		/*check to see if cell in position rc has lower/the same elevation
		as its adjacent cells*/
		
		/*skiping edge cells, or
			cell has a neighbor with an undef value of elevation or
			it is already flagged*/
		if (m_vFlag[rc.Row][rc.Col] == 0)  
		{
			vector<double>::iterator pos;
			vector<double> vNbs;

			GetNeighborCells(rc.Row, rc.Col, vNbs);

			pos = min_element(vNbs.begin(), vNbs.end());
			double rMin = *pos;

			if (m_vDEM[rc.Row][rc.Col] <= rMin)
					return true;
		}
		return false;				
}

void MapFillSinks::FindSinkContArea(RowCol rcInitSink)
{
		//*define the extent of sink contributing area		
		vector<RowCol> vStartCells;
		FlagAdjaCell(rcInitSink, vStartCells);
		
		/*identify the outward adjacent cells for each of starting cells
		 *put the located cells in vrcOutwardCells
		 *flag the located cells in vSinkFlagged*/

		vector<RowCol>::iterator pos;
		vector<RowCol> vAdjaCells;
		m_vFlag[rcInitSink.Row][rcInitSink.Col] = m_iFlag;
		m_vSinks.resize(0);
		m_vSinks.push_back(rcInitSink);
		do
		{

			vAdjaCells.resize(0); /*stores all located outward adjacent cells
													to each of starting cell*/
			
			for (pos = vStartCells.begin(); pos < vStartCells.end(); ++pos)
			{
					RowCol rc (*pos);
					m_vSinks.push_back(rc);
					FlagAdjaCell(*pos,vAdjaCells);
			}
			vStartCells.swap(vAdjaCells);
		}while(vAdjaCells.size() != 0); //no more element can be located 
}

void MapFillSinks::FindSinkContArea2(RowCol rcInitSink)
{
		//*define the extent of sink contributing area		
		vector<RowCol> vStartCells;
		m_sinkPixels = 1;
		FlagAdjaCell(rcInitSink, vStartCells);
		
		/*identify the outward adjacent cells for each of starting cells
		 *put the located cells in vrcOutwardCells
		 *flag the located cells in vSinkFlagged*/

		vector<RowCol>::iterator pos;
		vector<RowCol> vAdjaCells;
		m_vFlag[rcInitSink.Row][rcInitSink.Col] = m_iFlag;
		m_vSinks.resize(0);
		m_vSinks.push_back(rcInitSink);
		do
		{

			vAdjaCells.resize(0); /*stores all located outward adjacent cells
													to each of starting cell*/
			
			for (pos = vStartCells.begin(); pos < vStartCells.end(); ++pos)
			{
					RowCol rc (*pos);
					m_vSinks.push_back(rc);
					FlagAdjaCell(*pos,vAdjaCells);
			}
			vStartCells.swap(vAdjaCells);
			if (m_sinkPixels > m_sinkPixelsThreshold)
				break;

		}while(vAdjaCells.size() != 0); //no more element can be located 
}

bool MapFillSinks::IsEdgeCell(long iRow, long iCol)
{
		if (iRow == 0 || iRow == iLines() - 1 ||
				iCol == 0 || iCol == iCols() - 1)
				return true;
		else
				return false;
}

void MapFillSinks::FlagAdjaCell(RowCol rcStartCell, vector<RowCol>& vAdj)
{
	/*check and skip if a cell 
  	*is undefined or  
	*lies at the edge of DEM or
	*is adjacent to an undefined element
	*is already existed in the vector*/

		RowCol adj;	
		for (int i=-1; i<=1; i++)
		{
			for (int j=-1; j<=1; j++)
			{
		
				 adj.Row = rcStartCell.Row + i; //adjacent to starting cell rcCell
				 adj.Col = rcStartCell.Col + j;
				 if (m_vFlag[adj.Row][adj.Col] != m_iFlag &&
					 m_vFlag[adj.Row][adj.Col] > -2 &&
					 m_vDEM[adj.Row][adj.Col] >= m_vDEM[rcStartCell.Row][rcStartCell.Col])
				 {
					 vAdj.push_back(adj);
					 m_vFlag[adj.Row][adj.Col] = m_iFlag;
					 if (m_vDEM[adj.Row][adj.Col] == m_sinkHeight)
						 m_sinkPixels++;
				 }
			}
		}
}


class RowColValueLessClass //compare two elements for min_element algorithm
{
public:
	RowColValueLessClass(vector<RealBuf>& m_vSFd) :
		m_vDEM(m_vSFd)
	{
	}
	bool operator()(RowCol rc1, RowCol rc2)
	{ 
		return m_vDEM[rc1.Row][rc1.Col] < m_vDEM[rc2.Row][rc2.Col];
	}
private:
		vector<RealBuf>& m_vDEM;
};

bool MapFillSinks::fIdentifyOutletCell(RowCol rcSink, RowCol& rcOutlet)
{
		//Find outlet cell in the rim of the sink contributing area
		vector<RowCol> vOutlets; //potential outlets
		vOutlets.resize(0);
		
		vector<RowCol>::iterator pos = m_vSinks.begin();
		for (; pos < m_vSinks.end(); ++pos)
		//for (vector<RowCol>::iterator pos = m_vSinks.begin(); pos < m_vSinks.end(); ++pos)
		{		
				long iRow = pos->Row;
				long iCol = pos->Col;
				if (IsPotentialOutlet(iRow, iCol))
					 vOutlets.push_back(*pos);
		}
		
		if (vOutlets.size() > 0)
		{
				/*the lowest outlet is selected. 
				 *if two or more potential outlets have the lowest elevation
				 *the one encounted first is selected*/
				RowColValueLessClass rvl(m_vDEM);
				pos = min_element(vOutlets.begin(), vOutlets.end(), rvl);
				rcOutlet = *pos;
				return true;
		}
		else
				return false;
}

double MapFillSinks::getCutValue(RowCol rcOutlet)
{
		//Get the lowerest height of the neighbouring cells of the outlet
		double rHeight = m_vDEM[rcOutlet.Row][rcOutlet.Col];
		for (int i=-1; i<2; ++i)
		{
			for (int j=-1; j<2; ++j)
			{
				if (m_vDEM[rcOutlet.Row+i][rcOutlet.Col+j] < rHeight)
					rHeight = m_vDEM[rcOutlet.Row+i][rcOutlet.Col+j];
			}
		}

		return rHeight;
}

bool MapFillSinks::IsPotentialOutlet(long iRow, long iCol)
{
		/*Check if the current cell lies at the rim of the sink cont. area, and
		 *is higher than a cell, which is outside of the arae.*/
		double rHeight = m_vDEM[iRow][iCol];
		for (int i=-1; i<2; ++i)
		{
			for (int j=-1; j<2; ++j)
			{
					long iFlag = m_vFlag[iRow+i][iCol+j];
					if (iFlag != m_iFlag) 
					{
						if (iFlag == -2)	//flat area lies at the edge of DEM
						{
							if (rHeight >= m_vDEM[iRow+i][iCol+j])
								return true;
						}
						else 
						{
							if (rHeight > m_vDEM[iRow+i][iCol+j])
								return true;
						}
					}
			}
		}

		return false;
}

void MapFillSinks::DepresFill(RowCol rcOutlet)
{
		/*for each cell in the sink cont. area, if it is lower than the 
		 *elevation of the outlet, raise iis elevation to that of outlet.*/
		double rHeight = m_vDEM[rcOutlet.Row][rcOutlet.Col];
		vector<RowCol>::iterator pos;
		for (pos = m_vSinks.begin(); pos < m_vSinks.end(); ++pos)
		{		
				long iRow = pos->Row;
				long iCol = pos->Col;
				if (m_vDEM[iRow][iCol] < rHeight)
				{
					 m_vDEM[iRow][iCol] = rHeight;
				}
				m_vFlag[iRow][iCol] = -1;  //flag the cell within the sink area 
		}
}

void MapFillSinks::CutTerrain(RowCol rcOutlet)
{
		//cell with lowest height to the outlet is selected for
		//breaching and filling in this case
		double cutValue = getCutValue(rcOutlet);
		double rHeight = m_vDEM[rcOutlet.Row][rcOutlet.Col];
		vector<RowCol>::iterator pos;
		for (pos = m_vSinks.begin(); pos < m_vSinks.end(); ++pos)
		{		
				long iRow = pos->Row;
				long iCol = pos->Col;
				if (m_vDEM[iRow][iCol] <= rHeight){
					m_vDEM[iRow][iCol] = cutValue;
					m_vFlag[iRow][iCol] = -1;  //flag the cell within the sink area 
				}
		}
}

void MapFillSinks::FlatAreaFlag(RowCol rcOutlet)
{
		//flag the cells in an existing flat area

		double rHeight = m_vDEM[rcOutlet.Row][rcOutlet.Col];
		vector<RowCol>::iterator pos;
		for (pos = m_vSinks.begin(); pos < m_vSinks.end(); ++pos)
		{
					 m_vFlag[(*pos).Row][(*pos).Col] = -1; 
		}
}