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
// MapDrainageNetworkExtraction.cpp: implementation of class MapDrainageNetworkExtraction.
//
//////////////////////////////////////////////////////////////////////

#include "HydroFlowApplications\MapDrainageNetworkExtraction.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * ccccc(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>()) {
	return NULL;
}
IlwisObjectPtr * createMapDrainageNetworkExtraction(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parm ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapDrainageNetworkExtraction::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapDrainageNetworkExtraction(fn, (MapPtr &)ptr);
}

using namespace std;

void MapDrainageNetworkExtraction::Init()
{
		SetDomainValueRangeStruct(Domain("bool"));  
		fNeedFreeze = true;
		sFreezeTitle = "MapDrainageNetworkExtraction";
		htpFreeze = htpNetworkExtractionT;

		switch (m_tmMethods)
		{
			case tmMap:
				objdep.Add(m_mp);
        objdep.Add(m_mpFlowDir);
				break;
		}

		if (!fnObj.fValid())
    objtime = objdep.tmNewest();
		ptr.Store(); // store domain	
	
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapDrainageNetworkExtraction::MapDrainageNetworkExtraction(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	String sThreshold;
  ReadElement("MapDrainageNetworkExtraction", "Threshold", sThreshold);
  if (sThreshold.iVal() != iUNDEF)
	{	
			m_tmMethods = tmValue;
			m_iThreshold = sThreshold.iVal(); 
	}	
	else
	{
			m_tmMethods = tmMap;
			ReadElement("MapDrainageNetworkExtraction", "FlowDirection", m_mpFlowDir);
			FileName fnMap = FileName(sThreshold);
			m_mp = Map(fnMap);
	}
	Init();
}

MapDrainageNetworkExtraction::MapDrainageNetworkExtraction(const FileName& fn, 
																	 MapPtr& p,
																	 const Map& mp,
																	 ThresholdMethod tmMethod,
																	 const Map& thmp, 
																	 const Map& mpFlowDir)
: MapFromMap(fn, p, mp), m_mp(thmp), m_tmMethods(tmMethod), m_mpFlowDir(mpFlowDir)
{
	Init();
}

MapDrainageNetworkExtraction::MapDrainageNetworkExtraction(const FileName& fn, 
																	 MapPtr& p,
																	 const Map& mp,
																	 ThresholdMethod tmMethod,
																	 long iThreshold) 
: MapFromMap(fn, p, mp), m_iThreshold(iThreshold), m_tmMethods(tmMethod)
{
	Init();
}

MapDrainageNetworkExtraction::~MapDrainageNetworkExtraction()
{
}

const char* MapDrainageNetworkExtraction::sSyntax() {
  return "MapDrainageNetworkExtraction(FlowAccumulationMap, Stream Threshold Value) or \n"
         "MapDrainageNetworkExtraction(FlowAccumulationMap, Variable Threshold Map, FlowDirectionMap)";
}

MapDrainageNetworkExtraction* MapDrainageNetworkExtraction::create(const FileName& fn, 
																					 MapPtr& p, 
																					 const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 2)
      ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  ThresholdMethod tmMethod;
	bool fIncompGeoRef = false;
	
	// Determing if sThreshold is a numeric value 
	if (as[1].iVal() != iUNDEF)
	{
			tmMethod = tmValue;
			long iThreshold = as[1].iVal();
			return new MapDrainageNetworkExtraction(fn, p, mp, tmMethod, iThreshold);
	}
	else 
	{
      if (iParms <3 )
        ExpressionError(sExpr, sSyntax());
			tmMethod = tmMap;
			Map thmap(as[1], fn.sPath());
			if (mp->gr()->fGeoRefNone() && thmap->gr()->fGeoRefNone())
					fIncompGeoRef = mp->rcSize() != thmap->rcSize();
			else
				fIncompGeoRef = mp->gr() != thmap->gr();
			if (fIncompGeoRef)
				throw ErrorIncompatibleGeorefs(mp->gr()->sName(true, fn.sPath()),
                     thmap->gr()->sName(true, fn.sPath()), fn, errMapDrainageNetworkExtraction);
  
			Map mpFlowDir(as[2],fn.sPath());
			Domain dm = mpFlowDir->dm();
			if (!(dm.fValid() && (fCIStrEqual(dm->fnObj.sFileExt(), "FlowDirection.dom"))))
				throw ErrorObject(WhatError(SMAPErrInvalidDomain_S, errMapDrainageNetworkExtraction)); 

			if (mp->gr()->fGeoRefNone() && thmap->gr()->fGeoRefNone())
					fIncompGeoRef = mp->rcSize() != mpFlowDir->rcSize();
			else
					fIncompGeoRef = mp->gr() != mpFlowDir->gr();
			if (fIncompGeoRef)
					throw ErrorIncompatibleGeorefs(mp->gr()->sName(true, fn.sPath()),
                mpFlowDir->gr()->sName(true, fn.sPath()), fn, errMapDrainageNetworkExtraction);

			return new MapDrainageNetworkExtraction(fn, p, mp, tmMethod, thmap, mpFlowDir);
	}

}

void MapDrainageNetworkExtraction::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapDrainageNetworkExtraction");
	String sMethod;
  switch (m_tmMethods)
	{
		case tmValue:
			WriteElement("MapDrainageNetworkExtraction", "Threshold", m_iThreshold);
			break;
		case tmMap:
      WriteElement("MapDrainageNetworkExtraction", "Threshold", m_mp);
      WriteElement("MapDrainageNetworkExtraction", "FlowDirection", m_mpFlowDir);
      break;
	}
}

String MapDrainageNetworkExtraction::sExpression() const
{
  
	
	switch (m_tmMethods)
	{
		case tmValue:

			return String("MapDrainageNetworkExtraction(%S,%li)", 
											mp->fnObj.sRelativeQuoted(),
											m_iThreshold);
			break;
		case tmMap:
			return String("MapDrainageNetworkExtraction(%S,%S, %S)", 
										mp->fnObj.sRelativeQuoted(),
										m_mp->sNameQuoted(true ), 
										m_mpFlowDir->sNameQuoted(true));
			break;
		default: // this case will not occur .. this is only to keep the compiler happy
			return String("MapDrainageNetworkExtraction");
	}
}

bool MapDrainageNetworkExtraction::fDomainChangeable() const
{
  return false;
}

bool MapDrainageNetworkExtraction::fGeoRefChangeable() const
{
  return false;
}

bool MapDrainageNetworkExtraction::fFreezing()
{
	m_vFacc.resize(iLines());  //allocate memory for the input Flow Accumulation  
	if (m_tmMethods == tmMap)
		m_vThreshold.resize(iLines());  //allocate memory for the threshold
	
	if (m_tmMethods == tmMap )
		m_vFlow.resize(iLines());

	m_vOutput.resize(iLines());  //allocate memory for the output map  

	trq.SetTitle(sFreezeTitle);
	trq.SetText(SMAPTextDrainageNetworkExtraction);
	trq.Start();

	long iRow, iCol;
	for ( iRow = 0; iRow< iLines(); iRow++ )
	{
		//reading Flow Accumulation 
		m_vFacc[iRow].Size(iCols()); 
		mp->GetLineVal(iRow, m_vFacc[iRow]);

		if (m_tmMethods == tmMap)
		{
			m_vThreshold[iRow].Size(iCols()); 
			m_mp->GetLineVal(iRow, m_vThreshold[iRow]);
		}

		if (m_tmMethods == tmMap )
		{
			m_vFlow[iRow].Size(iCols()); 
			m_mpFlowDir->GetLineRaw(iRow, m_vFlow[iRow]);
		}
		
		m_vOutput[iRow].Size(iCols());

		for ( iCol = 0; iCol < iCols(); iCol++)
		{
			m_vOutput[iRow][iCol] = 0;
		}
		if (trq.fUpdate(iRow, iLines())) return false;	
	}
	trq.fUpdate(iLines(), iLines());

	for (iRow = 0; iRow < iLines(); ++iRow)
	{
		LongBuf& lbFacc = m_vFacc[iRow];
		LongBuf& lbThreshold = m_vThreshold[iRow];
		for(long iCol = 0; iCol < iCols(); ++iCol)
		{	
			if (lbFacc[iCol] == iUNDEF || (m_tmMethods == tmMap && lbThreshold[iCol] == iUNDEF)) {
				m_vOutput[iRow][iCol] = 0;
			}
			else if (m_tmMethods == tmMap && lbFacc[iCol] > lbThreshold[iCol])
			{		
				m_vOutput[iRow][iCol] = 2;	
			}
			else if (m_tmMethods == tmValue && lbFacc[iCol] > m_iThreshold)
			{		
				m_vOutput[iRow][iCol] = 2;	
			}
			else
				m_vOutput[iRow][iCol] = 1;
		}
		if (trq.fUpdate(iRow, iLines())) return false;
	}
	trq.fUpdate(iRow, iLines());
	
	m_vFacc.resize(0);
	m_vThreshold.resize(0);

	//Construct dead-end drainage lines, if stream threshold map is used
	if (m_tmMethods == tmMap )
	{
		trq.SetText(SMAPTextConstructDrainage);
		for (iRow = 1; iRow < iLines() - 1; ++iRow)
		{
			for( iCol = 1; iCol < iCols() - 1; ++iCol)
			{
				if (m_vOutput[iRow][iCol] == 2 && m_vFlow[iRow][iCol] != 0 )
				{
				  long in = iRow;
					long jn = iCol;
					while (IsDeadendLine( in, jn))
					{
						m_vOutput[in][jn] = 2;	
					}
				}
			}
			if (trq.fUpdate(iRow, iLines())) return false;
		}
	}	

	//write the computed draiange network to the output map  
	for (iRow = 0; iRow < iLines(); ++iRow)
	{
		ByteBuf& bFlow = m_vOutput[iRow];
		ptr.PutLineRaw(iRow, bFlow);
	}

	//final clean up
	m_vOutput.resize(0); 
	return true;
}

bool MapDrainageNetworkExtraction::IsEdgeCell(long iRow, long iCol)
{
		if (iRow == 0 || iRow == iLines() - 1 ||
				iCol == 0 || iCol == iCols() - 1)
				return true;
		else
				return false;
}

bool MapDrainageNetworkExtraction::IsDeadendLine(long& iRow, long& iCol)
{
	int iFlow = m_vFlow[iRow][iCol];
	long in, jn;
	bool fCondition;
	switch (iFlow)
	{
			case 1: {
				in = iRow;
				jn = iCol+1;
				fCondition = (m_vOutput[in][jn] != 2);
			}
			break;
			case 2: {
				in = iRow+1;
				jn = iCol+1;
				fCondition = (m_vOutput[in][jn] != 2);
			}
			break;
			case 3:	{
				in = iRow+1;
				jn = iCol;
				fCondition = (m_vOutput[in][jn] != 2);
			}
			break;
			case 4:	{
				in = iRow+1;
				jn = iCol-1;
				fCondition = (m_vOutput[in][jn] != 2);
			}
			break;
			case 5:	{
				in = iRow;
				jn = iCol-1;
				fCondition = (m_vOutput[in][jn] != 2);
			}
			break;
			case 6:	{
				in = iRow-1;
				jn = iCol-1;
				fCondition = (m_vOutput[in][jn] != 2);
			}
			break;
			case 7:	{
				in = iRow-1;
				jn = iCol;
				fCondition = (m_vOutput[in][jn] != 2);
			}
			break;
			case 8:	{
				in = iRow-1;
				jn = iCol+1;
				fCondition = (m_vOutput[in][jn] != 2);
			}
			break;
      default:
        in = iRow;
				jn = iCol;
        fCondition = false;
				
	}
	iRow = in;
	iCol = jn;
	if ((m_vOutput[in][jn] == 0) || (IsEdgeCell(in,jn)) )
		fCondition = false;

	return fCondition;
}

