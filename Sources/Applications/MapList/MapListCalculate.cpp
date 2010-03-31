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
    return "MapListCalculate(mapapplicationstring,first_band,last_band,maplist,maplist...)";
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
	int iNr;
	ReadElement("MapListCalculate", "NrMaplists", iNr);
	for (int i = 0; i < iNr; ++i)
	{
		String sSection ("MapList%d", i);
		MapList ml;
	  ReadElement("MapListCalculate", sSection.scVal(), ml);
		m_vml.push_back(ml);
	}
  Init();
}

MapListCalculate::MapListCalculate(const FileName& fn, MapListPtr& ptr, const String& sExpr)
: MapListVirtual(fn, ptr, true)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 4)
    ExpressionError(sExpr, sSyntax());  
  sCalcExpr = as[0];
	m_iFirstBand = as[1].iVal();
	m_iLastBand = as[2].iVal();
	for (int i = 3; i < iParms; ++i)
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
	WriteElement("MapListCalculate", "NrMaplists", (int)(m_vml.size()));
	for (int i = 0; i < m_vml.size(); ++i)
	{
		String sSection ("MapList%d", i);
		WriteElement("MapListCalculate", sSection.scVal(), m_vml[i]);
	}
}

void MapListCalculate::Init()
{
  sFreezeTitle = "MapListCalculate";
}

String MapListCalculate::sExpression() const
{
	String sExpr ("MapListCalculate(%S,%d,%d", sCalcExpr, m_iFirstBand, m_iLastBand);
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
			
    trq.SetText(SMPLTextConstructing);
    
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
					String sPattern ("@%d", j + 1); // ranges from @1 to @...
					int iPos = sExpr.find(sPattern);
					if (iPos == String::npos)
						break;
					String sLeft = sExpr.sLeft(iPos);
					String sRight = &sExpr[iPos+sPattern.length()];
					sExpr = String("%S%S%S", sLeft, sMap, sRight);
				}
			}
			String sMapName("%S_%i", fnObj.sFile, i+1);
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
		trq.SetText(SMPLTextCalculatingMap);
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
