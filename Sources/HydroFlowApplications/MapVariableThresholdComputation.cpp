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
// MapVariableThresholdComputation.cpp: implementation of class MapVariableThresholdComputation
//
//////////////////////////////////////////////////////////////////////

#include "HydroFlowApplications\MapVariableThresholdComputation.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapVariableThresholdComputation(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapVariableThresholdComputation::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapVariableThresholdComputation(fn, (MapPtr &)ptr);
}

using namespace std;

static bool isEven(int elem)
{
		return elem % 2 == 0;
}

static void SplitString(String s, vector<ThresholdValues> &results)
{
	Array<String> as;
	Split(s, as, ",");
	results.clear();

	for (unsigned int i=0; i < as.size(); i++)
	{
    if( (i % 2) == 0)
    {
      ThresholdValues res;
      res.UpperRelief = as[i].rVal();
      res.AccValue = as[i+1].rVal();
		  results.push_back(res);
    }
	}
}

void MapVariableThresholdComputation::Init()
{
		fNeedFreeze = true;
		sFreezeTitle = "MapVariableThresholdComputation";
		htpFreeze = htpVariableThresholdComputationT;

		if (!fnObj.fValid())
      objtime = objdep.tmNewest();
		ptr.Store(); // store domain	
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapVariableThresholdComputation::MapVariableThresholdComputation(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	ReadElement("MapVariableThresholdComputation", "FilterSize", m_iFilterSize);
  ReadElement("MapVariableThresholdComputation", "NrOfClasses", m_iClasses);
  ReadElement("MapVariableThresholdComputation", "ThresholdString", m_sThreshold);
  ReadElement("MapVariableThresholdComputation", "ReliefMap", m_fReliefMap);
	Init();
}

MapVariableThresholdComputation::MapVariableThresholdComputation(const FileName& fn, 
																	 MapPtr& p,
																	 const Map& mp,
																	 int iSize,
																	 int iClasses, 
																	 String sThreshold,
                                   bool fReliefMap)
: MapFromMap(fn, p, mp), m_iFilterSize(iSize), m_iClasses(iClasses), m_sThreshold(sThreshold), m_fReliefMap(fReliefMap)
{
	Init();
}


MapVariableThresholdComputation::~MapVariableThresholdComputation()
{
}

const char* MapVariableThresholdComputation::sSyntax() {
  return "MapVariableThresholdComputation(DEM, FilterSize, NrOfClasses, \"ThresholdString\", true | false)";
}

MapVariableThresholdComputation* MapVariableThresholdComputation::create(const FileName& fn, 
																					 MapPtr& p, 
																					 const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 5)
      ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  int iFilterSize = as[1].iVal();
  int iClasses = as[2].iVal();
	String sThreshold = as[3];
  
  if ((iFilterSize < 3) )  //must be odd values
		throw ErrorObject(WhatError(String(SMAPErrInvalidFilterSize.scVal(), as[1].iVal()),
                                errMapVariableThresholdComputation));
  if ((iClasses < 1) )  //must be positive values
		throw ErrorObject(WhatError(String(SMAPErrInvalidNrofClasses.scVal(), as[2].iVal()),
                                errMapVariableThresholdComputation));
  if (isEven(iFilterSize))
    iFilterSize += 1;
  Array<String> asThreshold;
	Split(sThreshold, asThreshold, ",");
  int iSize = asThreshold.size()/2;
  if (iClasses != iSize)
      throw ErrorObject(WhatError(String("Invalid threshold values", as[3].sVal()),
                                errMapVariableThresholdComputation));

  bool fReliefMap = as[4].fVal(); 
	return new MapVariableThresholdComputation(fn, p, mp, iFilterSize, iClasses, sThreshold, fReliefMap);
}

void MapVariableThresholdComputation::Store()
{
  MapFromMap::Store();
  if (isEven(m_iFilterSize))
    m_iFilterSize += 1;
  WriteElement("MapFromMap", "Type", "MapVariableThresholdComputation");
	WriteElement("MapVariableThresholdComputation", "DEM", mp);
	WriteElement("MapVariableThresholdComputation", "FilterSize", m_iFilterSize);
  WriteElement("MapVariableThresholdComputation", "NrOfClasses", m_iClasses);
  WriteElement("MapVariableThresholdComputation", "ThresholdString", m_sThreshold);
  WriteElement("MapVariableThresholdComputation", "ReliefMap", m_fReliefMap);
}

String MapVariableThresholdComputation::sExpression() const
{
  return String("MapVariableThresholdComputation(%S,%li,%li,\"%S\", %li)", 
                                              mp->sNameQuoted(false, fnObj.sPath()),
                                              m_iFilterSize,
                                              m_iClasses,
                                              m_sThreshold,
                                              m_fReliefMap);
}

bool MapVariableThresholdComputation::fDomainChangeable() const
{
  return false;
}

bool MapVariableThresholdComputation::fGeoRefChangeable() const
{
  return false;
}

bool MapVariableThresholdComputation::fFreezing()
{
	m_vDem.resize(iLines());  
	m_vOutput.resize(iLines());  

  vector<ThresholdValues> vThreshold;
  SplitString(m_sThreshold, vThreshold);
  
	trq.SetTitle(sFreezeTitle);
	trq.SetText(SMAPTextInitializeMap);
	trq.Start();
  
	long iRow;
	for ( iRow = 0; iRow< iLines(); iRow++ )
	{
		m_vDem[iRow].Size(iCols()); 
		mp->GetLineVal(iRow, m_vDem[iRow]);
    m_vOutput[iRow].Size(iCols());
    for ( long iCol = 0; iCol < iCols(); iCol++)
		{
      m_vOutput[iRow][iCol] = rUNDEF;
    }
		if (trq.fUpdate(iRow, iLines())) return false;	
	}
	trq.fUpdate(iLines(), iLines());
  
  if (isEven(m_iFilterSize))
    m_iFilterSize += 1;
  ComputeInternalRelief();
  ValueRange vrOutput = ValueRange(-9999999.9,9999999.9,mp->dvrs().rStep());
  if(m_fReliefMap)
  {
    FileName fnReliefMap = FileName::fnUnique(fnObj);
    Map mpRelief(fnReliefMap, gr(), rcSize(), vrOutput);
    //Map mpRelief(fnReliefMap, gr(), rcSize(), Domain("FlowDirection"));;
    trq.SetText("Create internal relief map");
	  for (iRow = 0; iRow < iLines(); ++iRow)
	  {
		  RealBuf& rb = m_vOutput[iRow];
		  mpRelief->PutLineVal(iRow, rb);
	  }
  }
  ReClassify(vThreshold);
	m_vDem.resize(0);

  //returns the position of the first element with max/min.
	//ValueRange vrOutput = ValueRange(-9999999.9,9999999.9,mp->dvrs().rStep());
	SetDomainValueRangeStruct(vrOutput);		
	CreateMapStore();

  trq.SetText("Write output map");
	for (iRow = 0; iRow < iLines(); ++iRow)
	{
		RealBuf& rb = m_vOutput[iRow];
		ptr.PutLineVal(iRow, rb);
	}

	//final clean up
	m_vOutput.resize(0); 
	return true;
}

bool MapVariableThresholdComputation::IsEdgeCell(long iRow, long iCol)
{
		if (iRow <= 0 || iRow >= iLines() - 1 ||
				iCol <= 0 || iCol >= iCols() - 1)
				return true;
		else
				return false;
}

void MapVariableThresholdComputation::ComputeInternalRelief()
{
  trq.SetText("Computation of the internal relief");
  int iSize = (m_iFilterSize)/2;
  for ( long iRow = 0; iRow< iLines(); iRow++ )
	{
    for ( long iCol = 0; iCol < iCols(); iCol++)
		{
        if (m_vDem[iRow][iCol] != rUNDEF)
        {
            vector<double> vVal;
            for (int i=-iSize; i<=iSize; ++i)
	          {
		            for (int j=-iSize; j<=iSize; ++j)
		            {
                  RowCol rc;
                  rc.Row = iRow+i;
                  rc.Col = iCol+j;
                  if ((IsEdgeCell(rc.Row, rc.Col) != true) && (m_vDem[rc.Row][rc.Col] != rUNDEF))
                  {
                    vVal.push_back(m_vDem[rc.Row][rc.Col]);
                  }
                }
	          }
            if (vVal.size() > 0)
            {
              vector<double>::iterator pos = max_element(vVal.begin(), vVal.end());
		          double rMax = *pos;
              pos = min_element(vVal.begin(), vVal.end());
		          double rMin = *pos;
              m_vOutput[iRow][iCol] = rMax - rMin;        
            }
         }
         else
             m_vOutput[iRow][iCol] = rUNDEF;
        
		}
    trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
}

void MapVariableThresholdComputation::ReClassify(vector<ThresholdValues> vThreshold)
{
  trq.SetText(SMAPTextVariableThresholdComputation);
  for ( long iRow = 0; iRow< iLines(); iRow++ )
	{
    for ( long iCol = 0; iCol < iCols(); iCol++)
		{
        m_vDem[iRow][iCol] = rUNDEF;
        if (m_vOutput[iRow][iCol] != rUNDEF)
        {
        
          for (int iClass=0; iClass<m_iClasses; ++iClass)
	        {
		         if (m_vOutput[iRow][iCol] <= vThreshold[iClass].UpperRelief)
            {           
              m_vDem[iRow][iCol] = vThreshold[iClass].AccValue;
              break;
            }
          }
        }
    }
    trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());

  trq.SetText(SMAPTextVariableThresholdComputation);
  int iSize = (m_iFilterSize)/2;
  for ( long iRow = 0; iRow< iLines(); iRow++ )
	{
    for ( long iCol = 0; iCol < iCols(); iCol++)
		{
        if (m_vDem[iRow][iCol] != rUNDEF)
        {
          vector<double> vVal;
          vVal.resize(0);
          for (int i=-iSize; i<=iSize; ++i)
	        {
		        for (int j=-iSize; j<=iSize; ++j)
		        {
              RowCol rc;
              rc.Row = iRow+i;
              rc.Col = iCol+j;
              if ((IsEdgeCell(rc.Row, rc.Col) != true) && (m_vDem[rc.Row][rc.Col] != rUNDEF))
              {
                vVal.push_back(m_vDem[rc.Row][rc.Col]);
              }
           }
	        }
          int iCout=0;
          int iMaxCout=0;
          double rValue;
          for (unsigned int i=0; i<vVal.size(); ++i)
          {
            iCout = count(vVal.begin(), vVal.end(), vVal[i]);
            if (iCout > iMaxCout)
            {
              iMaxCout = iCout;
              rValue = vVal[i];
            } 
          }
          m_vOutput[iRow][iCol] = rValue;
        }
        else
          m_vOutput[iRow][iCol] = rUNDEF;
    }
    trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());

}
