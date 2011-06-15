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

 Created on: 2007-28-9
 ***************************************************************/
// MapSI2Radiance.cpp: converts digital number (DN) of MODIS channels into 
// radiance or reflectance.
//
//////////////////////////////////////////////////////////////////////
/* SEBS pre-processing model
   Sepertember 2007, by Lichun Wang
*/
#include "SEBS\MapSI2Radiance.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

using namespace std;

IlwisObjectPtr * createMapSI2Radiance(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapSI2Radiance::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapSI2Radiance(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapSI2Radiance::MapSI2Radiance(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	fNeedFreeze = true;
	sFreezeTitle = SMAPTextRaw2Radiance;
	//htpFreeze = "ilwisapp\flow_accumulation_algorithm.htm";

	ReadElement("Radiance", "RawDataMap", m_mpRawData);
	ReadElement("Radiance", "Scale", m_rScale);
	ReadElement("Radiance", "Offset", m_rOffset);
}

MapSI2Radiance::MapSI2Radiance(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRawMap,
				 double rScale,
				 double rOffset
                 )
: MapFromMap(fn, p, mpRawMap),
	m_mpRawData(mpRawMap),
	m_rScale(rScale),
	m_rOffset(rOffset)
	
{
	DomainValueRangeStruct dv(-1e10,1e10,0.00001);
	SetDomainValueRangeStruct(dv);
	//init();
	objdep.Add(m_mpRawData);
	
    if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "Convert raw data into radiances";
}

MapSI2Radiance::~MapSI2Radiance()
{

}

const char* MapSI2Radiance::sSyntax() {
  
  return "MapSI2Radiance(RawDataMap,RadianceScale,RadianceOffset)";
}

MapSI2Radiance* MapSI2Radiance::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 3 )
      ExpressionError(sExpr, sSyntax());

  	Map mpRawData(as[0], fn.sPath());
	double rScale = as[1].rVal();
	double rOffset = as[2].rVal();
	
	return new MapSI2Radiance(fn, p, mpRawData,
				   rScale,
				   rOffset
				   );
}

void MapSI2Radiance::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapSI2Radiance");
  WriteElement("Radiance", "RawDataMap", m_mpRawData);
  WriteElement("Radiance", "Scale", m_rScale);
  WriteElement("Radiance", "Offset", m_rOffset);
}

String MapSI2Radiance::sExpression() const
{
  
	String sExp("MapSI2Radiance(%S,%g,%g)", 
				mp->sNameQuoted(false, fnObj.sPath()), 
				m_rScale,
				m_rOffset);
  return sExp;
}

bool MapSI2Radiance::fDomainChangeable() const
{
  return false;
}

bool MapSI2Radiance::fGeoRefChangeable() const
{
  return false;
}

bool MapSI2Radiance::fFreezing()
{
	trq.SetText(SMAPTextRaw2Radiance);
	RealBuf rBuf1;
	RealBuf rBuf2;
	rBuf1.Size(iCols());
	rBuf2.Size(iCols());
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		mp->GetLineVal(iRow, rBuf1);
		for (long iCol = 0; iCol< iCols(); iCol++){
			rBuf2[iCol] = m_rScale*(rBuf1[iCol]-m_rOffset);	
		}
		// write the result
		ptr.PutLineVal(iRow, rBuf2);
		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
	return true;
}

