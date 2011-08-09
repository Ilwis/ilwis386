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
// MapBrightnessTemperature.cpp: Calculates brightness temperature.
//
//////////////////////////////////////////////////////////////////////
/* SEBS pre-processing model
   Sepertember 2007, by Lichun Wang
*/
#include "SEBS\MapBrightnessTemperature.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

using namespace std;
IlwisObjectPtr * createMapBrightnessTemperature(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapBrightnessTemperature::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapBrightnessTemperature(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapBrightnessTemperature::MapBrightnessTemperature(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	fNeedFreeze = true;
	sFreezeTitle = TR("Compute Brightness Temperature");
	ReadElement("BrightnessTemperature", "Sensor", m_sSensor);
	Sensor sensor;
	if (fCIStrEqual(m_sSensor, "modis"))
			sensor = stModis;
	else if (fCIStrEqual(m_sSensor, "aster"))
			sensor = stAster;
	else
			sensor = stOther;
	switch(sensor)
	{
		case stModis:
		case stAster:
			ReadElement("BrightnessTemperature", "RadianceMap1", m_mpRadiance1);
			ReadElement("BrightnessTemperature", "RadianceMap2", m_mpRadiance2);
			ReadElement("BrightnessTemperature", "OuputBTMap2", m_sBTmap2);
			break;
		case stOther:
			ReadElement("BrightnessTemperature", "RadianceMap1", m_mpRadiance1);
			ReadElement("BrightnessTemperature", "WaveLength", m_rWavelength);
			break;
	}
}

MapBrightnessTemperature::MapBrightnessTemperature(const FileName& fn, 
				 MapPtr& p,
				 String sSensor,
				 const Map& mpRadiance1,
				 double rWavelength
				 )
: MapFromMap(fn, p, mpRadiance1),
	m_sSensor(sSensor),
	m_mpRadiance1(mpRadiance1),
	m_rWavelength(rWavelength)
{
	DomainValueRangeStruct dv(-1000,1000,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	objdep.Add(m_mpRadiance1);
	
    if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = TR("Compute Brightness Temperature");
}
MapBrightnessTemperature::MapBrightnessTemperature(const FileName& fn, 
				 MapPtr& p,
				 String sSensor,
				 const Map& mpRadiance1,
				 const Map& mpRadiance2,
				 String sBTMap
				 )
: MapFromMap(fn, p, mpRadiance1),
	m_sSensor(sSensor),
	m_mpRadiance1(mpRadiance1),
	m_mpRadiance2(mpRadiance2),
	m_sBTmap2(sBTMap)
{
	DomainValueRangeStruct dv(-1000,1000,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	objdep.Add(m_mpRadiance1);
	//objdep.Add(m_mpRadiance2);
	
    if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = TR("Compute Brightness Temperature");
}

MapBrightnessTemperature::~MapBrightnessTemperature()
{

}

const char* MapBrightnessTemperature::sSyntax() {
  
  return "MapBrightnessTemperature(other,RadianceMap,Waveleghth), or \n"
		  "MapBrightnessTemperature(aster,TIR_Band13,TIR_Band14,OutputBTMap14), or \n"	
			"MapBrightnessTemperature(modis,RadianceMap31,RadianceMap32, OutputBTMap32)\n";

}

MapBrightnessTemperature* MapBrightnessTemperature::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms < 3 )
      ExpressionError(sExpr, sSyntax());
	
    String sSensor = as[0].sVal();
	if (!(fCIStrEqual(sSensor, "modis") || !fCIStrEqual(sSensor, "other") || !fCIStrEqual(sSensor, "aster") ))
      ExpressionError(sExpr, sSyntax());
	if (fCIStrEqual(sSensor, "modis") || fCIStrEqual(sSensor, "aster")){
		Map mpRadiance1(as[1], fn.sPath());
		Map mpRadiance2(as[2], fn.sPath());
		String sBTMap2 = as[3].sVal();

		return new MapBrightnessTemperature(fn, p, sSensor,mpRadiance1,mpRadiance2,sBTMap2);
	}
	else{
		Map mpRadiance(as[1], fn.sPath());
		double rWavelength = as[2].rVal();
		return new MapBrightnessTemperature(fn, p, sSensor,mpRadiance,rWavelength);
    }
}

void MapBrightnessTemperature::Store()
{
	MapFromMap::Store();
	WriteElement("MapFromMap", "Type", "MapBrightnessTemperature");
	Sensor sensor;
	WriteElement("BrightnessTemperature", "Sensor", m_sSensor);
	if (fCIStrEqual(m_sSensor, "modis"))
		sensor = stModis;
	else if (fCIStrEqual(m_sSensor, "aster"))
		sensor = stAster;
    
    else
		sensor = stOther;
	switch (sensor)
	{
		case stModis:
		case stAster:
			WriteElement("BrightnessTemperature", "RadianceMap1", m_mpRadiance1);
			WriteElement("BrightnessTemperature", "RadianceMap2", m_mpRadiance2);
			WriteElement("BrightnessTemperature", "OuputBTMap2", m_sBTmap2);
			break;
		case stOther:
			WriteElement("BrightnessTemperature", "RadianceMap1", m_mpRadiance1);
			WriteElement("BrightnessTemperature", "WaveLength", m_rWavelength);
			break;
	}
}

String MapBrightnessTemperature::sExpression() const
{
	String sExp;
	if (fCIStrEqual(m_sSensor, "modis") || fCIStrEqual(m_sSensor, "aster"))
		sExp = String("MapBrightnessTemperature(%S,%S,%S,%S)", 
				m_sSensor,
				m_mpRadiance1->sNameQuoted(false, fnObj.sPath()),
				m_mpRadiance2->sNameQuoted(false, fnObj.sPath()),
				m_sBTmap2);
    else
		sExp = String("MapBrightnessTemperature(%S,%S,%g)",
				m_sSensor,
				m_mpRadiance1->sNameQuoted(false, fnObj.sPath()), 
				m_rWavelength);
	return sExp;
}

bool MapBrightnessTemperature::fDomainChangeable() const
{
  return false;
}

bool MapBrightnessTemperature::fGeoRefChangeable() const
{
  return false;
}

bool MapBrightnessTemperature::fFreezing()
{
	trq.SetText(TR("Compute Brightness Temperature"));
	RealBuf rBuf1_in;
	RealBuf rBuf2_in;
	RealBuf rBuf1_out;
	RealBuf rBuf2_out;
	
	rBuf1_in.Size(iCols());
	rBuf2_in.Size(iCols());
	rBuf1_out.Size(iCols());
	rBuf2_out.Size(iCols());
	double rWavelength1;
	double rWavelength2;
	Map mpBTM2;
	if (fCIStrEqual(m_sSensor, "modis") || fCIStrEqual(m_sSensor, "aster")){
		if (fCIStrEqual(m_sSensor, "modis")){
		
			rWavelength1 = 11.017; 
			rWavelength2 = 12.032;
		}
		if (fCIStrEqual(m_sSensor, "aster")){
		
			rWavelength1 = 10.6; 
			rWavelength2 = 11.3;
		}
		FileName fnBTmap(m_sBTmap2, fnObj);
		fnBTmap.sExt = ".mpr";
		mpBTM2 = Map(fnBTmap, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(-1000,1000,0.001));
	}
	else{
		rWavelength1 = m_rWavelength;
	}
	double c1 = 3.741775 * pow(10.0, -22);  //first radiance constant  W-m^3-u^-1
	double c2 = 0.0143877; // second radiance constant m-K
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_mpRadiance1->GetLineVal(iRow, rBuf1_in);
		if (fCIStrEqual(m_sSensor, "modis") || fCIStrEqual(m_sSensor, "aster")){
			m_mpRadiance2->GetLineVal(iRow, rBuf2_in);
		}
		for (long iCol = 0; iCol< iCols(); iCol++){
			rBuf1_out[iCol] = c2/(rWavelength1*pow(10.0,-6)*log(c1/(pow(rWavelength1*pow(10.0,-6),5)*m_PI*rBuf1_in[iCol])+1));	
			rBuf1_out[iCol]=rBuf1_out[iCol]*log(2.71828);
			if (fCIStrEqual(m_sSensor, "modis") || fCIStrEqual(m_sSensor, "aster")){
				rBuf2_out[iCol] = c2/(rWavelength2*pow(10.0,-6)*log(c1/(pow(rWavelength2*pow(10.0,-6),5)*m_PI*rBuf2_in[iCol])+1));	
				rBuf2_out[iCol]=rBuf2_out[iCol]*log(2.71828);
				double btm2 = rBuf2_out[iCol];
			}
		}
		// write the result
		ptr.PutLineVal(iRow, rBuf1_out);
		if (fCIStrEqual(m_sSensor, "modis") || fCIStrEqual(m_sSensor, "aster")){
			mpBTM2->PutLineVal(iRow, rBuf2_out);
		}
		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
	return true;
}



