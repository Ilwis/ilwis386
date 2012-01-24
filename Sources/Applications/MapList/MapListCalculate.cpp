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
// MapListCalculate.cpp: implementation of the MapListCalculate class.
//
//////////////////////////////////////////////////////////////////////

#include "Applications\MapList\MapListCalculate.h"
#include "Headers\Hs\maplist.hs"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace {
  const char* sSyntax() 
  {
    return "MapListCalculate(mapapplicationstring,first_band,last_band,namingConvention,maplist,maplist...)";
  }
}

IlwisObjectPtr * createMapListCalculate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)new MapListCalculate(fn, (MapListPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapListCalculate(fn, (MapListPtr &)ptr);
}

MapListCalculate::MapListCalculate(const FileName& fn, MapListPtr& ptr)
: MapListVirtual(fn, ptr, false)
{
  ReadElement("MapListCalculate", "CalculateExpression", sCalcExpr);
	ReadElement("MapListCalculate", "FirstBand", m_iFirstBand);
	ReadElement("MapListCalculate", "LastBand", m_iLastBand);
	ReadElement("MapListCalculate", "OriginalNames", keepOriginalNames);
	int iNr;
	ReadElement("MapListCalculate", "NrMaplists", iNr);
	for (int i = 0; i < iNr; ++i)
	{
		String sSection ("MapList%d", i);
		MapList ml;
	  ReadElement("MapListCalculate", sSection.c_str(), ml);
		m_vml.push_back(ml);
	}
  Init();
}

MapListCalculate::MapListCalculate(const FileName& fn, MapListPtr& ptr, const String& sExpr)
: MapListVirtual(fn, ptr, true)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 5)
    ExpressionError(sExpr, sSyntax());  
  sCalcExpr = as[0];
	m_iFirstBand = as[1].iVal();
	m_iLastBand = as[2].iVal();
	keepOriginalNames = as[3].fVal();
	for (int i = 4; i < iParms; ++i)
		m_vml.push_back(MapList(FileName(as[i])));

	CheckExpression();

  Init();
}

MapListCalculate::~MapListCalculate()
{
}

void MapListCalculate::Store()
{
  MapListVirtual::Store();
  WriteElement("MapListVirtual", "Type", "MapListCalculate");
  WriteElement("MapListCalculate", "CalculateExpression", sCalcExpr);
	WriteElement("MapListCalculate", "FirstBand", m_iFirstBand);
	WriteElement("MapListCalculate", "LastBand", m_iLastBand);
	WriteElement("MapListCalculate", "OriginalNames", keepOriginalNames);
	WriteElement("MapListCalculate", "NrMaplists", (int)(m_vml.size()));
	for (int i = 0; i < m_vml.size(); ++i)
	{
		String sSection ("MapList%d", i);
		WriteElement("MapListCalculate", sSection.c_str(), m_vml[i]);
	}
}

void MapListCalculate::Init()
{
  sFreezeTitle = "MapListCalculate";
}

String MapListCalculate::sExpression() const
{
	String sExpr ("MapListCalculate(%S,%d,%d,%d", sCalcExpr, m_iFirstBand, m_iLastBand, keepOriginalNames);
	for (int i = 0; i < m_vml.size(); ++i)
		sExpr += String (",%S", m_vml[i]->sName());
	sExpr += ")";
  return sExpr;
}

void MapListCalculate::CheckExpression()
{
	if (m_vml.size() == 0)
		throw ErrorObject(WhatError("At least one MapList expected",0), WhereError());
	
	// here we know m_vml.size() > 0
	trq.SetText("Checking source maplists");

	for (int i = 0; i < m_vml.size(); ++i)
		if (!m_vml[i].fValid())
			throw ErrorObject(WhatError("Invalid Maplist",0), WhereError(m_vml[i]->sName()));

	// here we know all maplists are valid

	if (m_iFirstBand == iUNDEF)
		m_iFirstBand = m_vml[0]->iLower();
	if (m_iLastBand == iUNDEF)
		m_iLastBand = m_vml[0]->iUpper();
	if ((m_iFirstBand < m_vml[0]->iLower()) || (m_iLastBand > m_vml[0]->iUpper()))
		throw ErrorObject(WhatError("Number of maps requested exceeds maps in Maplist",0), WhereError(m_vml[0]->sName()));

	// here we know that the requested bands (from first to last) are OK

	int iMaps = m_vml[0]->iSize();

	for (int i = 1; i < m_vml.size(); ++i)
		if (m_vml[i]->iSize() != iMaps)
			throw ErrorObject(WhatError("Number of maps in Maplist differs from first Maplist",0), WhereError(m_vml[i]->sName()));

	// now we know all maplists are same size
}

bool MapListCalculate::fFreezing()
{ 
  try {
    SetSize(0);

		CheckExpression();
			
    trq.SetText(TR("Constructing"));
    
		int iMaps = m_iLastBand - m_iFirstBand + 1;

		if (iMaps <= 0)
			throw ErrorObject(WhatError("Last band nr. must be greater or equal to first band nr.",1), WhereError());
		
		SetSize(iMaps);

		for (int i = 0; i < iMaps; ++i)
		{
			if (trq.fUpdate(i, iMaps)) 
			{
				SetSize(0);
				return false;
			}
			String sExpr = sCalcExpr;
			for (int j = m_vml.size() - 1; j >= 0; --j) // start from up and go downwards: e.g. first substitute @12 and then @1
			{
				// substitute the @ signs
				String sMap = m_vml[j]->map(i+m_iFirstBand)->sName();
				for (;;)
				{
					String sPattern ("[", j + 1); // ranges from @1 to @...
					int ind1 = sExpr.find("[");
					if (ind1 == String::npos)
						break;
					int ind2 = sExpr.find("]", ind1);
					if ( ind2 == string::npos)
						break;
					if ( ind1 - ind2 > 3)
						throw ErrorObject(TR("Syntax error in expression"));
					String n = sExpr.substr(ind1 + 1,ind2 - ind1 - 1);
					n = n.sTrimSpaces();
					int val = n.iVal();
					String sLeft, sRight;
					String sM;
					if ( val == iUNDEF || val > 0) {
						throw ErrorObject(TR("Syntax error in expression"));
					}
					else {
						sLeft = sExpr.sLeft(ind1);
						sRight = &sExpr[ind2 + 1];
						int indMap = i + m_iFirstBand + val;
						sM = String("%S_%i", keepOriginalNames ? fnObj.sFile + "_" + sMap : fnObj.sFile, indMap + 1);
						if ( indMap < 0) {
							int ind = sRight.find_first_of("+-/*");
							if ( ind != string::npos)
								sExpr = sRight.substr(ind + 1);
						} else {
							sExpr = String("%S%S%S", sLeft, sM, sRight);
						}
					}
					
				}
				for (;;)
				{
					String sPattern ("@%d", j + 1); // ranges from @1 to @...
					int iPos = sExpr.find(sPattern);
					if (iPos == String::npos)
						break;
					String sLeft = sExpr.sLeft(iPos);
					String sRight = &sExpr[iPos+sPattern.length()];
					sExpr = String("%S%S%S", sLeft, sMap, sRight);
				}
			}
			String sMapName("%S_%i", keepOriginalNames ? fnObj.sFile + "_" + m_vml[0]->map(i)->sName()  : fnObj.sFile, i+1);
			FileName fnMap(sMapName);
			map(i) = Map(fnMap,sExpr);

			if (map(i).fValid())
			{
				if (dvrsMaps.dm().fValid())
					map(i)->SetDomainValueRangeStruct(dvrsMaps);
			}
			else
				throw ErrorObject(WhatError(String("Map nr. %d in the output map list is invalid.",i),0), WhereError());
			// Note the error code 0 in the throw, thus the message is not shown to the user.
			// I consider the message received from the calculator (in Map(fnMap,sExpr)) sufficient.
		}

		if (trq.fUpdate(iMaps, iMaps)) 
		{
			SetSize(0);
			return false;
		}
		ptr.SetGeoRef(map(0)->gr());
		trq.SetText(TR("Calculating Map"));
		for (int i = 0; i < iMaps; ++i) 
		{
			if (trq.fUpdate(i, iMaps)) 
				return false;
			map(i)->Calc();
			if (!map(i)->fCalculated())
				return false;
		}
  }
  catch (ErrorObject& err)
  {
    err.Show();
    SetSize(1); // this and next line have a reason: if fFreezing does not return a mpl that is fValid, it enters an infinite loop.
		map(0) = m_vml[0]->map(m_iFirstBand);
    return false;
  }
  return true;
}



