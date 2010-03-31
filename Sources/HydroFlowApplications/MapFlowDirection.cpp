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

 Created on: 2009-07-8
 ***************************************************************/
// MapFlowDirection.cpp: implementation of the MapFlowDirection class.
//
//////////////////////////////////////////////////////////////////////

//#include "ParallelFlowDirectionAlgorithm.h"
//#include "MapFlowDirection.h"
//#include "AdditionalHeaders\Htp\ilwisapp.htp"
//#include "AdditionalHeaders\Err\ilwisapp.err"
//#include "AdditionalHeaders\hs\map.hs"

//using namespace std;
#include "HydroFlowApplications\ParallelFlowDirectionAlgorithm.h"
#include "HydroFlowApplications\MapFlowDirection.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

using namespace std;

IlwisObjectPtr * createMapFlowDirection(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapFlowDirection::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapFlowDirection(fn, (MapPtr &)ptr);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapFlowDirection::MapFlowDirection(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	String sMethod;
	ReadElement("MapFlowDirection", "Method", sMethod);
	if (fCIStrEqual(sMethod, "slope"))
			m_fmMethods = fmSlope;
		else
			m_fmMethods = fmHeight;

	m_fParallel = false;
	String sPara;
	ReadElement("MapFlowDirection", "Parallel", sPara);
	if (fCIStrEqual(sPara, "yes"))
		m_fParallel = true;

	fNeedFreeze = true;
	sFreezeTitle = "MapFlowDirection";
	htpFreeze = htpFlowDirectionT;
}

MapFlowDirection::MapFlowDirection(const FileName& fn, MapPtr& p, const Map& mp, 
		 FlowMethod sMethod, bool fParallel)
	: MapFromMap(fn, p, mp), m_fmMethods(sMethod), m_fParallel(fParallel)
{
	SetDomainValueRangeStruct(Domain("FlowDirection"));  
  fNeedFreeze = true;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.Store(); // store domain and att table info
}

MapFlowDirection::~MapFlowDirection()
{
}

const char* MapFlowDirection::sSyntax() {
  return "MapFlowDirection(DEM [,Slope|Height [, Parallel]])";
}

MapFlowDirection* MapFlowDirection::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 3))
      ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  FlowMethod  sMethod;
  if (iParms > 1) {
    String sMtd = as[1].sVal();
    if (!(fCIStrEqual(sMtd, "slope") || fCIStrEqual(sMtd, "height")))
      ExpressionError(sExpr, sSyntax());
		if (fCIStrEqual(sMtd, "slope"))
			sMethod = fmSlope;
		else
			sMethod = fmHeight;
  }
	else
		sMethod = fmSlope;
	bool fParallel = false;
	if (iParms > 2) {
		String para = as[2].sVal();
		fParallel = fCIStrEqual(para, "parallel");
	}
  return new MapFlowDirection(fn, p, mp, sMethod, fParallel);
}

void MapFlowDirection::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapFlowDirection");
	String sMethod;
	switch (m_fmMethods)
	{
		case fmSlope:
			sMethod = "Slope";
			break;
		case fmHeight:
			sMethod = "Height";
			break;
	}
  WriteElement("MapFlowDirection", "Method", sMethod);
  WriteElement("MapFlowDirection", "Parallel", m_fParallel);

}

String MapFlowDirection::sExpression() const
{
  
	String sMethod;
	switch (m_fmMethods)
	{
		case fmSlope:
			sMethod = "Slope";
			break;
		case fmHeight:
			sMethod = "Height";
			break;
	}
	if (m_fParallel) 
		sMethod &= ", Parallel";

	String sExp("MapFlowDirection(%S,%S)", mp->sNameQuoted(true, fnObj.sPath()),
           sMethod);
	return sExp;
}

bool MapFlowDirection::fDomainChangeable() const
{
  return false;
}

bool MapFlowDirection::fGeoRefChangeable() const
{
  return false;
}

bool MapFlowDirection::fFreezing()
{
	m_vDem.resize(iLines());  //allocate memory for the input DEM 
	m_vFlow.resize(iLines());  //allocate memory for the output flows 
	InitPars();
	trq.SetTitle(sFreezeTitle);
	trq.SetText(SMAPTextReadingDEM);
	trq.Start();
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		//read DEM into the created vector vDem in memory
		m_vDem[iRow].Size(iCols()); 
		mp->GetLineVal(iRow, m_vDem[iRow]);
		m_vFlow[iRow].Size(iCols());

		for (long iCol = 0; iCol < iCols(); iCol++)
		{
			m_vFlow[iRow][iCol] = 0;
		}
		if (trq.fUpdate(iRow, iLines())) return false;	
	}
	trq.fUpdate(iLines(), iLines());

	trq.SetText(SMAPTextCalculatingFlowDirection);

	if (m_fParallel) {
		FlowDirectionAlgorithm fda(&trq);
		String method = "height";
		if (m_fmMethods == FlowMethod::fmSlope)
			method = "slope";
		m_vFlow = fda.calculate(m_vDem, method);
	} else {
		double rMax;
		int iCout=0;
		long iRow = 0;
		for (; iRow < iLines(); ++iRow)
		//for (long iRow = 0; iRow < iLines(); ++iRow)
		{
			RealBuf& rbDem = m_vDem[iRow];
			for(long iCol = 0; iCol < iCols(); ++iCol)
			{	
				if ((iRow == 0) || (iRow == iLines()-1) || 
						(iCol == 0 ) || (iCol == iCols() - 1)){
					//make an edge cell undef
					//bFlowDirection[iCol] = 0;
					m_vFlow[iRow][iCol] = 0;
				}
				else if (rbDem[iCol] == rUNDEF) {
					//assign 0 to the cell with undefined value 
					//bFlowDirection[iCol] = 0;
					m_vFlow[iRow][iCol] = 0;
				}
				else
				{		//if (rbDem[iCol] != rUNDEF) {
						//Calculate slope/height diff. dh from 8-neighbors for the specified cell
						//vValue - a vector containing slope/dh values for its 8 neighbors 
						vector<double> vValue;
						FillArray(iRow, iCol, vValue);
						
						//finds positions for elements with max value
						vector<int> vPos;
						rMax = rFindMaxLocation(vValue, vPos, iCout);
						
						//examine the positions with max. value
						//perform assigning flow direction algorithm.
						m_vFlow[iRow][iCol] = iLookUp(rMax, iCout,vPos);
				}
			}
			if (trq.fUpdate(iRow, iLines())) return false;
		}
		trq.fUpdate(iRow, iLines());
		
		//Ste flows to the flat areas
		TreatFlatAreas();
		
		m_vDem.resize(0);
		m_vFlag.resize(0);
	}
	//write the computed flow direction for the output map  
	for (long iRow = 0; iRow < iLines(); ++iRow)
	{
		for (long iCol = 0; iCol < iCols(); ++iCol)
		{
			if (m_vFlow[iRow][iCol] == 9)
				m_vFlow[iRow][iCol] = 0;
		}
		ByteBuf& bFlow = m_vFlow[iRow];
		ptr.PutLineRaw(iRow, bFlow);
	}

	//final clean up
	m_vFlow.resize(0); 
	return true;
}

void MapFlowDirection::FillArray(long iRow, long iCol, 
																 vector<double>& vValue)
{
	//For the specified cell[iRow, iCol], compute the slope/dh values among its 8-neighbors
	//return the calculated slope/dh in vector vValue
	//iNbRow - row number for the specified neighbor
	//iNbCol - column number for the specified neighbor
	//rCurH - elevation for the specified cell[iRow, iCol] 
	//rNb8H - elevation for the specified neighbor' cell[iNbRow, iNbCol]
	//rValue - slope / height difference
	//location number
	//	-------
	//	|6|7|8|
	//	-------
	//	|5| |1|
	//	-------
	//	|4|3|2|
	//	-------
	//
	long iNbRow, iNbCol;
	double rCurH, rNb8H, rValue;
	rCurH = m_vDem[iRow][iCol];
	for(int i= 0; i<8; i++)
	{
		switch (i)
		{
			case 0: {	//location 1/E
				iNbRow = iRow;
				iNbCol = iCol+1;
			}
			break;
			case 1: { //location 2/SE 
				iNbRow = iRow+1;
				iNbCol = iCol+1;
			}
			break;
			case 2: {	//location 3/S
				iNbRow = iRow+1;
				iNbCol = iCol;
			}
			break;
			case 3:{ //Location 4/SW
				iNbRow = iRow+1;
				iNbCol = iCol-1;
			}
			break;
			case 4:{	//location 5/W
				iNbRow = iRow;
				iNbCol = iCol-1;
			}
			break;
			case 5:{	//location 6/NW 
				iNbRow = iRow-1;
				iNbCol = iCol-1;
			}
			break;
			case 6:{	//location 7/N
				iNbRow = iRow-1;
				iNbCol = iCol;
			}
			break;
			case 7:{	//location 8/NE
				iNbRow = iRow-1;
				iNbCol = iCol+1;
			}
			break;
		}
		rNb8H = m_vDem[iNbRow][iNbCol]; 
		if (rNb8H == rUNDEF)
			vValue.push_back(rUNDEF); //never flow to undefied cell	
		else{
			switch(m_fmMethods)
			{
				case fmSlope:
					rValue = rComputeSlope(rCurH, rNb8H,i+1);
					break;
				case fmHeight:
					rValue = rComputeHeightDifference(rCurH,rNb8H);
					break;
			}
			vValue.push_back(rValue); 
		}
	}
}

double MapFlowDirection::rComputeSlope(double rCurH, 
																			 double rNbH, 
																			 int iPos)
{
		//The slope is calculated by subscribing the neighbor's value from the center
		//distance 1.14 is concerned for diagonal cells 
		double rVal;
		if (isEven(iPos))
			rVal = (rCurH - rNbH)/1.41;
		else
			rVal = rCurH - rNbH;
		return rVal;
}

double MapFlowDirection::rComputeHeightDifference(double rCurH, double rNbH)
{
		return rCurH - rNbH;
}

bool MapFlowDirection::isEven(int elem)
{
		return elem % 2 == 0;
}

double MapFlowDirection::rFindMaxLocation(vector<double>& vValue, vector<int>& vPos, int& iCout)
{
		//finds the maximum value in the input vector
		//returns the maximum value, number of elements with max.
		//returns posision(s) for the element(s) with max. in a vector
		
		vector<double>::iterator pos;
		
		//returns the position of the first element with max. in vValue
		pos = max_element(vValue.begin(), vValue.end());
		double rMax = *pos;

		//count number of elements with max
		iCout = count(vValue.begin(), vValue.end(), rMax);

		//find the first element with max value
		pos = find(vValue.begin(), vValue.end(), rMax);
		int iIndex = pos - vValue.begin();

		while (pos != vValue.end())
		{
				vPos.push_back(iIndex);  //push it into a vector
				pos = find(++pos, vValue.end(), rMax);
				iIndex = pos - vValue.begin();
		}
		return rMax;
}

long MapFlowDirection::iLookUp(double rMax, 
															 int iCout,
															 vector<int>& vPos)
{
		//rMax - max. value in slope / height diff.
		//iCout - number of positions with max. elements in vPos
		//vPos - positions with elements having max. value
		//iPos - location which the target cell flows to
	
		//This function examines the positons for elements with max value
		//then assigns flow direction accordinngly
		long iPos;
		if (rMax <= 0)		
			iPos = 9; //a sink or flat cell
		else if (iCout < 3)
			iPos = vPos[0] + 1;
		else{
			//examine the positions of elements with max.
			//if three cells are adjacent, assign it to flow to the center
			//else flow to the first elem. in vector vPos for positions
			if (isInOneEdge(0,1,7, vPos))
				iPos = 1;
			else if (isInOneEdge(1,2,3, vPos))
				iPos = 3;
			else if (isInOneEdge(3,4,5, vPos))
				iPos = 5;
			else if (isInOneEdge(5,6,7, vPos))
				iPos = 7;
			else
				iPos = vPos[0] + 1;
		}
		return iPos;
}

bool MapFlowDirection::isInOneEdge(int iPos1, int iPos2, int iPos3, vector<int>& vPos)
{
		bool fCondition1 = find(vPos.begin(),vPos.end(),iPos1) != vPos.end();
		bool fCondition2 = find(vPos.begin(),vPos.end(),iPos2) != vPos.end();
		bool fCondition3 = find(vPos.begin(),vPos.end(),iPos3) != vPos.end();

		return fCondition1 && fCondition2 && fCondition3;

}

void MapFlowDirection::TreatFlatAreas()
{
		trq.SetText(SMapTextFlatArea);		
		m_vFlag.resize(iLines());

		//m_vFlag is initialized to zero, first
		for (long iRow = 0; iRow < iLines(); iRow++)
		{
			m_vFlag[iRow].Size(iCols());
			for (long iCol = 0; iCol < iCols(); iCol++)
			{
				m_vFlag[iRow][iCol] = 0;
			}
		}
		
		/*---For each area in the flat areas: 
		  ---Identify outlets in the flat area.
		  ---Examine the neighbors in the flat area to the outlets.
		  ---If a neighbor is in the flat area and
		     has not been assigned a flow direction, its flow is set to the outlet.
		  ---Locate neighbors without flow direction, in the flat area, 
			---to the neighbors that was evaluated in the previous step.
			---Set their flows to the neighbors with flow direction.
			---The procedure is repeated untill all the cells in 
			---the flat area have been evaluated.
		*/

		RowCol rcInitFlat;	
		m_ContFlat = 0;
		//for (long iRow = 1; iRow < iLines()-1; ++iRow)
		long iRow = 1;
		for (; iRow < iLines()-1; ++iRow)
		{
			for(long iCol = 1; iCol < iCols()-1; ++iCol)
			{
				if (m_vFlow[iRow][iCol] == 9)	//cell in flat area
				{
					vector<RowCol> vOutlets;
					vOutlets.resize(0);
					m_vFlat.resize(0);
					rcInitFlat.Row = iRow;
					rcInitFlat.Col = iCol;
					LocateOutlets(rcInitFlat, vOutlets);

					if (vOutlets.size() > 0)
					{
						SetFlowsInFlatArea(vOutlets);
					}	
					else
					{
						for (vector<RowCol>::iterator pos = m_vFlat.begin(); 
								 pos < m_vFlat.end(); ++pos)
						{		
								RowCol rc (*pos);
								m_vFlow[rc.Row][rc.Col] = 0;	//make it UNDEF
						}	
					}
				}
			}
			trq.fUpdate(iLines(), iLines());
		}	
		trq.fUpdate(iRow, iLines());
}

void MapFlowDirection::LocateOutlets(RowCol rcInitFlat, 
																		 vector<RowCol>& vOutlets)
{
		//--Locate and Flag a continous flat area to m_vFlag
		//--Locate and push the outlets to the flat area to vOutlets
		
		vector<RowCol> vNbs;
		vector<RowCol> vStarters;	
		vStarters.push_back(rcInitFlat);
		
		double rHeight = m_vDem[rcInitFlat.Row][rcInitFlat.Col];
		m_ContFlat++;

		do
		{
			vNbs.resize(0);
			for (vector<RowCol>::iterator pos = vStarters.begin(); 
																		pos < vStarters.end(); ++pos)
			{
				RowCol rcNb;
				m_vFlag[pos->Row][pos->Col] = m_ContFlat;
				m_vFlat.push_back(*pos);
				for (int i=-1; i<=1; ++i) 			//Evaluate neighbors in flat area 
				{				
					rcNb.Row = pos->Row + i;						
					for(int j=-1; j<=1; ++j)		
					{
						rcNb.Col = pos->Col + j;
						if (m_vFlag[rcNb.Row][rcNb.Col] != m_ContFlat) //has not been flagged
						{
							if (m_vFlow[rcNb.Row][rcNb.Col] == 9) 				 //in flat area
							{
									m_vFlag[rcNb.Row][rcNb.Col] = m_ContFlat;	
									m_vFlat.push_back(rcNb);
									vNbs.push_back(rcNb);
							}	
							else if((m_vDem[rcNb.Row][rcNb.Col] == rHeight) &&		
										(m_vFlow[rcNb.Row][rcNb.Col] != 0))
							{
									m_vFlag[rcNb.Row][rcNb.Col] = m_ContFlat;	
									vOutlets.push_back(rcNb);		
							}
						}
					}
				}		
			}
			vStarters.swap(vNbs);
		}while(vNbs.size() != 0);
}

void MapFlowDirection::SetFlowsInFlatArea(vector<RowCol>& vOutlets)
{
		vector<RowCol> vNbs;	//Neighbors without flow direction to neighbors  
													//with flow direction in the flat area

		do
		{
			vNbs.resize(0);
			//for (vector<RowCol>::iterator pos = vOutlets.begin(); pos < vOutlets.end(); ++pos)
			vector<RowCol>::iterator pos = vOutlets.begin(); 
			for (;pos < vOutlets.end(); ++pos)
			{
				m_vFlag[pos->Row][pos->Col] = 0;
			}
			for (pos = vOutlets.begin(); pos < vOutlets.end(); ++pos)
			{		
				RowCol rcNb;
				int iNb = 0;
				for (int i=-1; i<=1; ++i) 			//Evaluate neighbors in flat area 
				{				
					rcNb.Row = pos->Row + i;						
					for(int j=-1; j<=1; ++j)		
					{
						rcNb.Col = pos->Col + j;
						if (m_vFlow[rcNb.Row][rcNb.Col] == 9 && 
								m_vFlowSelf[iNb] != m_vFlow[pos->Row][pos->Col] &&
								m_vFlag[rcNb.Row][rcNb.Col] == m_ContFlat) 
							 
						{
							m_vFlow[rcNb.Row][rcNb.Col] = m_vDirection[iNb];	
							vNbs.push_back(rcNb);
						}	
						if((m_vFlag[rcNb.Row][rcNb.Col] == m_ContFlat) &&		
							  !(isEven(iNb)) &&
							  m_vFlowSelf[iNb] != m_vFlow[pos->Row][pos->Col])
						{
									m_vFlow[rcNb.Row][rcNb.Col] = m_vDirection[iNb];	
						}
						iNb++;
					}
				}
			}		
			vOutlets.swap(vNbs);
		}while (vNbs.size() != 0);
}

void MapFlowDirection::InitPars()
{
		//	Location number				Order in m_vDirection
		//	-------								-------	 looping order of the neighbors 	
	  //	|6|7|8|								|0|1|2|
		//	-------								-------
		//	|5| |1|								|3|4|5|
		//	-------								-------
		//	|4|3|2|								|6|7|8|
		//	-------								-------
		//
		m_vDirection.resize(9);
		
		m_vDirection[0] = 2;
		m_vDirection[1] = 3;
		m_vDirection[2] = 4;
		m_vDirection[3] = 1;
		m_vDirection[4] = 0;
		m_vDirection[5] = 5;
		m_vDirection[6] = 8;
		m_vDirection[7] = 7;
		m_vDirection[8] = 6;

		m_vFlowSelf.resize(9);
		m_vFlowSelf[0] = 6;
		m_vFlowSelf[1] = 7;
		m_vFlowSelf[2] = 8;
		m_vFlowSelf[3] = 5;
		m_vFlowSelf[4] = 0;
		m_vFlowSelf[5] = 1;
		m_vFlowSelf[6] = 4;
		m_vFlowSelf[7] = 3;
		m_vFlowSelf[8] = 2;

}