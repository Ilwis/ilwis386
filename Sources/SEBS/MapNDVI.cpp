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

 Created on: 2007-22-11
 ***************************************************************/
//This operation calculates the land surface emissivity, optionally you can 
//also obtain an emissivity difference map, a NDVI and a vegetation proportion (Pv) map.  
//
//////////////////////////////////////////////////////////////////////
/* SEBS pre-processing model
   November 2007, by Lichun Wang
*/
#include "SEBS\MapNDVI.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

using namespace std;

IlwisObjectPtr * createMapEmissivity(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapEmissivity::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapEmissivity(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapEmissivity::MapEmissivity(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	fNeedFreeze = true;
	sFreezeTitle = SMAPTextComputeEmissivity;
	ReadElement("Emissivity", "Sensor", m_sSensor);
	ReadElement("Emissivity", "RedBand", m_mpRedBand);
	ReadElement("Emissivity", "NearInfraredBand", m_mpNIR);
	CompitableGeorefs(fn, mp, m_mpNIR);
	ReadElement("Emissivity", "AlbedoMap", m_sAlbedoMap);
	if (m_sAlbedoMap.length() > 0){ 
		Map mpAlbedo(m_sAlbedoMap, fn.sPath());
		CompitableGeorefs(fn, mp, mpAlbedo);
    }
	ReadElement("Emissivity", "NDVI", m_sNdviMap);
	ReadElement("Emissivity", "EmissivityDifferenceMap", m_sEmissivityDifMap);
	ReadElement("Emissivity", "VegetationProportionMap", m_sPvMap);
}

MapEmissivity::MapEmissivity(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRed,
				 const Map& mpNIR,
				 String sAlbedoMap,
				 String sSensor,
				 String sNdviMap,
				 String sEmissivityDifMap,
				 String sPvMap
				 )
: MapFromMap(fn, p, mpRed),
	m_mpRedBand(mpRed),
	m_mpNIR(mpNIR),
	m_sSensor(sSensor),
	m_sAlbedoMap(sAlbedoMap),
	m_sNdviMap(sNdviMap),
	m_sEmissivityDifMap(sEmissivityDifMap),
	m_sPvMap(sPvMap)
{
	DomainValueRangeStruct dv(-10,10,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	CompitableGeorefs(fn, mp, m_mpNIR);
	objdep.Add(m_mpRedBand);
	objdep.Add(m_mpNIR);
	if (sAlbedoMap.length() > 0){ 
		Map mpAlbedo(sAlbedoMap, fn.sPath());
		CompitableGeorefs(fn, mp, mpAlbedo);
		objdep.Add(mpAlbedo);
    }
    if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "Compute Surface Emissivity";
}

MapEmissivity::~MapEmissivity()
{

}

const char* MapEmissivity::sSyntax() {
  
  return "MapEmissivity(InputRedBand,InputNearInfrared, InputAlbedo, SENSOR,OutputNDVI, OutputEmissivityDifMap, OutputPvMap)";
}

MapEmissivity* MapEmissivity::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 7 )
      ExpressionError(sExpr, sSyntax());

  	Map mpRed(as[0], fn.sPath());
	Map mpNIR(as[1], fn.sPath());
	String sAlbedoMap = as[2].sVal();
	String sSensor = as[3].sVal();
	String sNdviMap = as[4].sVal(); 
	String sEmissivityDifMap = as[5].sVal(); 
	String sPvMap = as[6].sVal(); 
	return new MapEmissivity(fn, p, mpRed,mpNIR,sAlbedoMap,sSensor,sNdviMap,sEmissivityDifMap, sPvMap);
}

void MapEmissivity::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapEmissivity");
  WriteElement("Emissivity", "RedBand", m_mpRedBand);
  WriteElement("Emissivity", "NearInfraredBand", m_mpNIR);
  WriteElement("Emissivity", "Sensor", m_sSensor);
  WriteElement("Emissivity", "AlbedoMap", m_sAlbedoMap);
  WriteElement("Emissivity", "NDVI", m_sNdviMap);
  WriteElement("Emissivity", "EmissivityDifferenceMap", m_sEmissivityDifMap);
  WriteElement("Emissivity", "VegetationProportionMap", m_sPvMap);
}

String MapEmissivity::sExpression() const
{
  
	String sExp("MapEmissivity(%S,%S,%S,%S,%S,%S,%S)", 
				mp->sNameQuoted(false, fnObj.sPath()), 
				m_mpNIR->sNameQuoted(true),
				m_sAlbedoMap,
				m_sSensor,
				m_sNdviMap,
				m_sEmissivityDifMap,
				m_sPvMap);
  return sExp;
}

bool MapEmissivity::fDomainChangeable() const
{
  return false;
}

bool MapEmissivity::fGeoRefChangeable() const
{
  return false;
}

bool MapEmissivity::fFreezing()
{
	trq.SetText("SAFTitleComputeEmissivity");
	RealBuf rBufRed;
	RealBuf rBufNIR;
	RealBuf rBufAlbedo;
	RealBuf rBufNDVI;
    RealBuf rBufEmissivity;
	RealBuf rBufEmissivityDif;
	RealBuf rBufPv;
	rBufRed.Size(iCols());
	rBufNIR.Size(iCols());
	rBufEmissivity.Size(iCols());
	Map mpNdvi;
	Map mpAlbedo;
	Map mpEmissivityDif;
	Map mpPv;
	bool fNdviMap = false;
	bool fEmissivityDifMap = false;
	bool fPvMap = false;
	bool fAlbedoMap = false;
	if(m_sNdviMap.length() > 0)
	{
		FileName fnNdvi = FileName(m_sNdviMap,fnObj);
		mpNdvi = Map(fnNdvi, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(-10,10,0.001));
		rBufNDVI.Size(iCols());
		fNdviMap = true;
	}
	if(m_sEmissivityDifMap.length() > 0 && !fCIStrEqual(m_sEmissivityDifMap, "nomap"))
	{
		FileName fnEmissivityDif = FileName(m_sEmissivityDifMap,fnObj);
		mpEmissivityDif = Map(fnEmissivityDif, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(-10,10,0.001));
		rBufEmissivityDif.Size(iCols());
		fEmissivityDifMap = true;
	}
	if(m_sPvMap.length() > 0 && !fCIStrEqual(m_sPvMap, "nomap"))
	{
		FileName fnPv = FileName(m_sPvMap,fnObj);
		mpPv = Map(fnPv, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(0,1,0.001));
		rBufPv.Size(iCols());
		fPvMap = true;
	}
	if(m_sAlbedoMap.length() > 0){
		FileName fnAlbedo = FileName(m_sAlbedoMap,fnObj.sPath());
		mpAlbedo = Map(m_sAlbedoMap,fnObj.sPath());
		rBufAlbedo.Size(iCols());
		fAlbedoMap = true;
	}
	
	//NDVI factors for vegetation fractional coverage Pv, from Sobrino method
	const double NDVImin = 0.2;
	const double NDVImax = 0.5;
	// weight factors for albedo (Valiente et al., 1995)
	const double c4 = 0.035;
	const double c5 = 0.545;
	const double c6 = 0.32;
	//Emissivity factors 
	double eSoil1,eSoil2,deSoil1,deSoil2;
	double eMixed1,eMixed2, deMixed1;
	double eVeg1, deVeg1;
	if ((fCIStrEqual(m_sSensor, "modis"))){
		//factors for soil pixels
		eSoil1 = 0.9832;
		eSoil2 = -0.058;
		deSoil1 = 0.0018;
		deSoil2 = -0.06;
		//factors for mixed pixels
		eMixed1 = 0.971;
		eMixed2 = 0.018;
		deMixed1 = 0.006;
		//factors for vegetation pixels
		eVeg1 = 0.985;
		deVeg1= 0.005;
	}
	else if ((fCIStrEqual(m_sSensor, "aatsr"))){
		//factors for soil pixels
		eSoil1 = 0.9825;
		eSoil2 = -0.051;
		deSoil1 = -0.0001;
		deSoil2 = -0.041;
		//factors for mixed pixels
		eMixed1 = 0.971;
		eMixed2 = 0.018;
		deMixed1 = 0.006;
		//factors for vegetation pixels
		eVeg1 = 0.990;
		deVeg1= 0;
	}
	double emissivity = 0;
	double albedoValue;
	double dEmissivity = 0.0; //spectral emissivity difference
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_mpRedBand->GetLineVal(iRow, rBufRed);
		m_mpNIR->GetLineVal(iRow, rBufNIR);
		if (fAlbedoMap)
			mpAlbedo->GetLineVal(iRow, rBufAlbedo); 
		for (long iCol = 0; iCol< iCols(); iCol++){

			double rRed = rBufRed[iCol];
			double rNIR = rBufNIR[iCol];
			/*if (fCIStrEqual(m_sSensor, "aatsr") || fCIStrEqual(m_sSensor, "aster")){
				if (rNIR != rUNDEF || rRed != rUNDEF){
					rRed = rRed *0.01;
					rNIR = rNIR *0.01;
				}
			}*/
			double rNdvi;
			if (rNIR == rUNDEF || rRed == rUNDEF)
				rNdvi = rUNDEF;
			else if((rNIR+rRed)<=0.0001 )
				rNdvi = 0;
			else
				rNdvi = (rNIR-rRed)/(rNIR+rRed);
			double Pv;
			if (fAlbedoMap)
				albedoValue = rBufAlbedo[iCol];
			else
				albedoValue = c4 + c5 * rRed+c6*rNIR ; // Valiente et
			if (rNdvi == rUNDEF){
				emissivity = rUNDEF;
				dEmissivity = rUNDEF;;
				Pv=rUNDEF;
			}
			else if (rNdvi < 0.2){ //for bare soil pixels
				emissivity = eSoil1 + eSoil2 * rRed; // /100;
				dEmissivity = deSoil1 + deSoil2 * rRed; // /100;
				Pv=0;
			}
			else if (rNdvi >= 0.2 && rNdvi <= 0.5){ //for mixed pixels
				Pv = pow((rNdvi - NDVImin),2) / ((NDVImax - NDVImin)*(NDVImax - NDVImin));
				emissivity = eMixed1 + eMixed2 * Pv;
				dEmissivity = deMixed1 * (1 - Pv);
			}	
			else if (rNdvi > 0.5){ // fully vegetation pixels
				emissivity = eVeg1 + deVeg1;
				dEmissivity = deVeg1;
				Pv=1;
			}
			if (albedoValue <= 0.035 && rNdvi != rUNDEF){
				emissivity = 0.995; //water area
				dEmissivity = 0.0;
				Pv=0;
				rNdvi = 0.0;
			}
			if ((fCIStrEqual(m_sSensor, "aster")) && (rNdvi != rUNDEF)){
				//estimates according to thermal band13 and 14 emissivity values
				double e13 = 0.968 + 0.022*Pv; 
				double e14 = 0.970 + 0.020*Pv;
				emissivity = 0.5*(e13+e14);
				dEmissivity = e13-e14;
			}
			if (fNdviMap)
				rBufNDVI[iCol] = rNdvi; 
			rBufEmissivity[iCol] = emissivity;
			if (fEmissivityDifMap)
				rBufEmissivityDif[iCol] = dEmissivity; 
			if (fPvMap)
				rBufPv[iCol] = Pv; 
		}
		// write the result
		ptr.PutLineVal(iRow, rBufEmissivity);
		if (fNdviMap)
			mpNdvi->PutLineVal(iRow, rBufNDVI);
		if (fEmissivityDifMap)
			mpEmissivityDif->PutLineVal(iRow, rBufEmissivityDif);
		if (fPvMap)
			mpPv->PutLineVal(iRow, rBufPv);
		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
	return true;
}

void MapEmissivity::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
{
	bool fIncompGeoRef = false;
	if (mp1->gr()->fGeoRefNone() && mp2->gr()->fGeoRefNone())
		  fIncompGeoRef = mp1->rcSize() != mp2->rcSize();
	else
			fIncompGeoRef = mp1->gr() != mp2->gr();
	if (fIncompGeoRef)
			throw ErrorIncompatibleGeorefs(mp1->gr()->sName(true, fn.sPath()),
            mp2->gr()->sName(true, fn.sPath()), fn, 1);

}

