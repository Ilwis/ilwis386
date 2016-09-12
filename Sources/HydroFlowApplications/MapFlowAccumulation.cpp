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
// MapFlowAccumulation.cpp: implementation of the MapFlowAccumulation class.
//
//////////////////////////////////////////////////////////////////////
/* FlowAccumulation
   Copyright Ilwis System Development ITC
   April 2003, by Lichun Wang
*/
#include "HydroFlowApplications\MapFlowAccumulation.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapFlowAccumulation(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapFlowAccumulation::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapFlowAccumulation(fn, (MapPtr &)ptr);
}

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MapFlowAccumulation::MapFlowAccumulation(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
		fNeedFreeze = true;
		sFreezeTitle = "MapFlowAccumulation";
		htpFreeze = "ilwisapp\\flow_accumulation_algorithm.htm";

}

MapFlowAccumulation::MapFlowAccumulation(const FileName& fn, MapPtr& p,
  const Map& mp)
: MapFromMap(fn, p, mp)
{
	DomainValueRangeStruct dv(0,999999999,1);
	SetDomainValueRangeStruct(dv);
	fNeedFreeze = true;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	ptr.Store(); // store domain info
}

MapFlowAccumulation::~MapFlowAccumulation()
{

}

const char* MapFlowAccumulation::sSyntax() {
  return "MapFlowAccumulation(FlowDirectionMap)";
}

MapFlowAccumulation* MapFlowAccumulation::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 1 || iParms > 1)
      ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
	Domain dm = mp->dm();
	if (!(dm.fValid() && (fCIStrEqual(dm->fnObj.sFileExt(), "FlowDirection.dom"))))
			throw ErrorObject(WhatError(TR("Use an input map with domain FlowDirection   "), errMapFlowAccumulation)); 

	return new MapFlowAccumulation(fn, p, mp);
}

void MapFlowAccumulation::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapFlowAccumulation");
	WriteElement("Display", "Stretching", "Logarithmic");
}

String MapFlowAccumulation::sExpression() const
{
  
	String sExp("MapFlowAccumulation(%S)", mp->sNameQuoted(false, fnObj.sPath()));
  return sExp;
}

bool MapFlowAccumulation::fDomainChangeable() const
{
  return false;
}

bool MapFlowAccumulation::fGeoRefChangeable() const
{
  return false;
}

bool MapFlowAccumulation::fFreezing()
{
	m_vFlowDirection.resize(iLines());  //allocate memory for input flow direction 
	m_vFlowAcc.resize(iLines()); //allocate memory for output flow acc. 
	trq.SetTitle(sFreezeTitle);
	trq.SetText(TR("Reading Flow Direction Map"));
	trq.Start();
	long iRow;
	for (iRow = 0; iRow < iLines(); iRow++)
	{
		//read flow direction into the created vector vFlowDirection in memory
		m_vFlowDirection[iRow].Size(iCols()); 
		mp->GetLineRaw(iRow, m_vFlowDirection[iRow]);

		//allocate memory for output flow accumulation
		m_vFlowAcc[iRow].Size(iCols()); 
		if (trq.fUpdate(iRow, iLines()))
			return false;	
	}
	trq.fUpdate(iLines(), iLines());
	
	trq.SetText(TR("Calculating Flow Accumulation"));

	for (iRow = 0; iRow < iLines(); iRow++)
	{
		for (long iCol = 0; iCol < iCols(); iCol++)
		{
			if (m_vFlowDirection[iRow][iCol] == 0)
			{
				//Call the recursive function at each outlet cell.
				//Then the function recusively calls itsself for all cells that flow to the outlet
				//cell in location(iCol,iCol). 
				iFlowAcc(iRow,iCol);
			}
		}
		if (trq.fUpdate(iRow, iLines()))
			return false;	
	}
	trq.fUpdate(iLines(), iLines());

	//write flow accumulation for the output map
	trq.SetText(TR("Writing Flow Accumulation Map"));
  for (iRow = 0; iRow < iLines(); iRow++ )
	{
		ptr.PutLineVal(iRow, m_vFlowAcc[iRow]);
		if (trq.fUpdate(iRow, iLines())) return false;
	}
	trq.fUpdate(iLines(), iLines());
	m_vFlowDirection.resize(0); 
	m_vFlowAcc.resize(0); 
	return true;
}
long MapFlowAccumulation::iFlowAcc(long iRow, long iCol)
{	
	//For the specified cell in loaction[iRow, iCol], 
	//checks whether its neighboring cells flow to it,
	//If true, calls the function recursively, and
	//stores the accumulated flow to output flow acc. vector in location[iRow,iCol]   
	//The recursion stops when it reaches a cell that has no flow to itsself 
	//location number
	//	-------
	//	|6|7|8|
	//	-------
	//	|5| |1|
	//	-------
	//	|4|3|2|
	//	-------
	// 
	long iAcc = 1;
	bool isFlow; //determine if the neighboring cell flows to cell in location[iRow,iCol] 
	long in, jn;  
	for (int iNr = 1; iNr < 9; iNr++)
	{
		isFlow = false;
		switch (iNr)
		{
				case 1: {	//East
					if (iCol != iCols()-1)
					{	
						in = iRow;
						jn = iCol+1;
						isFlow = (m_vFlowDirection[in][jn] == 5);
					}
				}
				break;
				case 2: { //South East 
					if (iCol != iCols()-1 && iRow != iLines()-1)
					{
						in = iRow+1;
						jn = iCol+1;
						isFlow = (m_vFlowDirection[in][jn] == 6);
					}
				}
				break;
				case 3: {	//South
					if (iRow != iLines()-1)
					{
						in = iRow+1;
						jn = iCol;
						isFlow = (m_vFlowDirection[in][jn] == 7);
					}
				}
				break;
				case 4:{ //South West
					if (iCol != 0 && iRow != iLines()-1)
					{
						in = iRow+1;
						jn = iCol-1;
						isFlow = (m_vFlowDirection[in][jn] == 8);
					}
				}
				break;
				case 5:{	//West
					if (iCol != 0 )
					{
						in = iRow;
						jn = iCol-1;
						isFlow = (m_vFlowDirection[in][jn] == 1);
					}
				}
				break;
				case 6:{	//North West 
					if (iCol != 0 && iRow != 0)
					{
						in = iRow-1;
						jn = iCol-1;
						isFlow = (m_vFlowDirection[in][jn] == 2);
					}
				}
				break;
				case 7:{	//North
					if (iRow != 0)
					{
						in = iRow-1;
						jn = iCol;
						isFlow = (m_vFlowDirection[in][jn] == 3);
					}
				}
				break;
				case 8:{	//North East
					if (iCol != iCols()-1 && iRow != 0)
					{
						in = iRow-1;
						jn = iCol+1;
						isFlow = (m_vFlowDirection[in][jn] == 4);
					}
				}
				break;
		}
		if (isFlow)
			iAcc += iFlowAcc(in, jn);
		m_vFlowAcc[iRow][iCol] = iAcc;
	}
	return iAcc;
}
