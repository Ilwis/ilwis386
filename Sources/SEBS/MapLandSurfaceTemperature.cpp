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

 Created on: Nov, 2007 by Lichun Wang
 ***************************************************************/
/* MapLandSurfaceTemperature.cpp: Land surface temperature (LST) retrieval using 
   Split Window method by Jose A. Sobrino */
//////////////////////////////////////////////////////////////////////
/* SEBS pre-processing model
   November 2007, by Lichun Wang
*/
#include "SEBS\MapLandSurfaceTemperature.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

using namespace std;

IlwisObjectPtr * createMapLandSurfaceTemperature(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapLandSurfaceTemperature::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapLandSurfaceTemperature(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapLandSurfaceTemperature::MapLandSurfaceTemperature(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	fNeedFreeze = true;
	sFreezeTitle = SMAPTextComputeLandSurfaceTemperature;
	ReadElement("LandSurfaceTemperature", "Sensor", m_sSensor);
	ReadElement("LandSurfaceTemperature", "BrightnessTemperature12", m_mpBtemp12);
	ReadElement("LandSurfaceTemperature", "BrightnessTemperature11", m_mpBtemp11);
	ReadElement("LandSurfaceTemperature", "EmissivityMap", m_mpEmissivity);
	ReadElement("LandSurfaceTemperature", "EmissivityDifferenceMap", m_mpEmissivityDif);
	ReadElement("LandSurfaceTemperature", "InputWaterVapor", m_sInWaterVapor);
	CompitableGeorefs(fn, mp, m_mpBtemp11);
	CompitableGeorefs(fn, mp, m_mpEmissivity);
	CompitableGeorefs(fn, mp, m_mpEmissivityDif);
	if (m_sInWaterVapor.length() > 0 && !fCIStrEqual(m_sInWaterVapor, "nomap")){ 
		Map mpWaterVapor(m_sInWaterVapor, fn.sPath());
		CompitableGeorefs(fn, mp, mpWaterVapor);
	}
}

MapLandSurfaceTemperature::MapLandSurfaceTemperature(
				 const FileName& fn, 
				 MapPtr& p,
				 String sSensor,
				 const Map& mpBtemp12,
				 const Map& mpBtemp11,
				 const Map& mpEmissivity,
				 const Map& mpEmissivityDif,
				 String sInWaterVapor
				 )
: MapFromMap(fn, p, mpBtemp12),
	m_sSensor(sSensor),
	m_mpBtemp12(mpBtemp12),
	m_mpBtemp11(mpBtemp11),
	m_mpEmissivity(mpEmissivity),
	m_mpEmissivityDif(mpEmissivityDif),
	m_sInWaterVapor(sInWaterVapor)
{
	DomainValueRangeStruct dv(-400,400,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	CompitableGeorefs(fn, mp, m_mpBtemp11);
	CompitableGeorefs(fn, mp, m_mpEmissivity);
	CompitableGeorefs(fn, mp, m_mpEmissivityDif);
	objdep.Add(m_mpBtemp12);
	objdep.Add(m_mpBtemp11);
	objdep.Add(m_mpEmissivity);
	objdep.Add(m_mpEmissivityDif);
	if (sInWaterVapor.length() > 0 && !fCIStrEqual(m_sInWaterVapor, "nomap")){ 
		Map mpWaterVapor(sInWaterVapor, fn.sPath());
		CompitableGeorefs(fn, mp, mpWaterVapor);
		objdep.Add(mpWaterVapor);
    }
    
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "Compute Land Surface Temperature";
}

MapLandSurfaceTemperature::~MapLandSurfaceTemperature()
{

}

const char* MapLandSurfaceTemperature::sSyntax() {
  
  return "MapLandSurfaceTemperature(MODIS|ASTER|AATSR,RedBand,NearInfrared,Emissivity,EmisDifference,WaterVapor)";
}

MapLandSurfaceTemperature* MapLandSurfaceTemperature::create(const FileName& fn, MapPtr& p, const String& sExpr)
{

  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 6 )
      ExpressionError(sExpr, sSyntax());

	String sSensor = as[0].sVal();
  	Map mpBtemp12(as[1], fn.sPath());
	Map mpBtemp11(as[2], fn.sPath());
	Map mpEmissivity(as[3], fn.sPath());
	Map mpEmissivityDif(as[4], fn.sPath());
	String sInWaterVapor = as[5].sVal();
	return new MapLandSurfaceTemperature(fn, p, 
		sSensor,
		mpBtemp12,
		mpBtemp11,
		mpEmissivity,
		mpEmissivityDif,
		sInWaterVapor
		);
}

void MapLandSurfaceTemperature::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapLandSurfaceTemperature");
  WriteElement("LandSurfaceTemperature", "Sensor", m_sSensor);
  WriteElement("LandSurfaceTemperature", "BrightnessTemperature12", m_mpBtemp12);
  WriteElement("LandSurfaceTemperature", "BrightnessTemperature11", m_mpBtemp11);
  WriteElement("LandSurfaceTemperature", "EmissivityMap", m_mpEmissivity);
  WriteElement("LandSurfaceTemperature", "EmissivityDifferenceMap", m_mpEmissivityDif);
  WriteElement("LandSurfaceTemperature", "InputWaterVapor", m_sInWaterVapor);
}
  

String MapLandSurfaceTemperature::sExpression() const
{
  
	String sExp("MapLandSurfaceTemperature(%S,%S,%S,%S,%S,%S)", 
				m_sSensor,
				mp->sNameQuoted(false, fnObj.sPath()), 
				m_mpBtemp11->sNameQuoted(true),
				m_mpEmissivity->sNameQuoted(true),
				m_mpEmissivityDif->sNameQuoted(true),
				m_sInWaterVapor
				);
  return sExp;
}

bool MapLandSurfaceTemperature::fDomainChangeable() const
{
  return false;
}

bool MapLandSurfaceTemperature::fGeoRefChangeable() const
{
  return false;
}

void MapLandSurfaceTemperature::ReadBrightnessTemperatureMaps()
{
    trq.SetText(SMAPTextComputeBrightnessTemperature);
	RealBuf rBuf1;
	RealBuf rBuf2;
	rBuf1.Size(iCols());
	rBuf2.Size(iCols());
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_mpBtemp12->GetLineVal(iRow, rBuf1);
		m_mpBtemp11->GetLineVal(iRow, rBuf2);
		for (long iCol = 0; iCol< iCols(); iCol++ ){
			inputBand1[iRow*iCols()+iCol] = rBuf1[iCol];
			inputBand2[iRow*iCols()+iCol] = rBuf2[iCol];
		}
		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
}

bool MapLandSurfaceTemperature::fFreezing()
{
	int width = iCols();
	int height = iLines();
	// x and y size of the window for taking the average temperature
	const int windowSize = 3; // prefer odd number

	// for the temperature, allocate memory to load the whole band at once
	inputBand1.Size(width * height);
	inputBand2.Size(width * height);

	// for the emissivity, emissivity difference and water vapor maps, allocate memory for single scan lines
	RealBuf rBufEmis;
	RealBuf rBufEmisDif;
	RealBuf rBufWaterVapor;
	rBufEmis.Size(iCols());
	rBufEmisDif.Size(iCols());
	rBufWaterVapor.Size(iCols());

	// allocate memory for single scan lines of the output
	RealBuf outputLST;
	outputLST.Size(iCols());
	
	// for these bands, read all at once
	ReadBrightnessTemperatureMaps();
    
	trq.SetTitle(sFreezeTitle);
	trq.SetText(SMAPTextComputeLandSurfaceTemperature);
	trq.Start();
	Map mpInWaterVapor;
	bool fInWaterVapor = false;
	if(m_sInWaterVapor.length() > 0 && !fCIStrEqual(m_sInWaterVapor, "nomap")){
		FileName fnInWaterVapor = FileName(m_sInWaterVapor,fnObj.sPath());
		mpInWaterVapor = Map(m_sInWaterVapor,fnObj.sPath());
		fInWaterVapor = true;
	}

	for (int y = 0; y < height; y++) {
		// read these bands by scanline
		m_mpEmissivity->GetLineVal(y, rBufEmis); 
		m_mpEmissivityDif->GetLineVal(y, rBufEmisDif); 
		if (fInWaterVapor)
			mpInWaterVapor->GetLineVal(y, rBufWaterVapor); 

		// calculate the y-range of a "window" for taking the average
		// temperature
		int y1 = y - windowSize / 2;
		int y2 = y1 + windowSize - 1;
		if (y1 < 0)
			y1 = 0;
		if (y2 >= height)
			y2 = height - 1;

		// process the complete scanline
		//
		for (int x = 0; x < width; x++) {
			RowCol rc;
			rc.Row = y;
			rc.Col = x;
			Coord cd = mp->gr()->cConv(rc);
			// calculate the x-range of a "window" for taking the
			// average temperature
			int x1 = x - windowSize / 2;
			int x2 = x1 + windowSize - 1;
			if (x1 < 0)
				x1 = 0;
			if (x2 >= width)
				x2 = width - 1;
			// calculate the average temperature
			double top_nad = 0.0;
			double bot_nad = 0.0;
			double T12_nad_avg = 0.0;
			double T11_nad_avg = 0.0;
			double T12_nads = 0.0;
			double T11_nads = 0.0;
			int count = 0;
			for (int p = x1; p <= x2; ++p)
				for (int q = y1; q <= y2; ++q) {
					T12_nads += inputBand1[p + q * width];
					T11_nads += inputBand2[p + q * width];
					++count;
				}
			T12_nad_avg = T12_nads / count;
			T11_nad_avg = T11_nads / count;
			for (int i = x1; i <= x2; ++i)
				for (int j = y1; j <= y2; ++j) {
					top_nad += (inputBand1[i + j * width] - T12_nad_avg)
						* (inputBand2[i + j * width] - T11_nad_avg);
					bot_nad += pow(
							(inputBand2[i + j * width] - T11_nad_avg), 2);
				}
			double tau_ratio_nad = top_nad / bot_nad;
				
			//LST retrieval using Split Window method by Jose A. Sobrino
			double btemp1;
			double btemp2;
			btemp1 =  inputBand1[y*width+x];
			btemp2 =  inputBand2[y*width+x];
			if ((btemp2 - btemp1) < 0)
				btemp2 = btemp1;
			double W;
			if (fInWaterVapor)	
				W = rBufWaterVapor[x]; 
			else
				W = 13.73 - 13.662 * tau_ratio_nad; //water vapour content by by Zhao Liang and bob Su 2003, INT. J. Remote Sensing
			double emissivity = rBufEmis[x];
			double dEmissivity = rBufEmisDif[x];
			double LST;
			if ((fCIStrEqual(m_sSensor, "aatsr"))){
				LST = btemp2 + (1.97 + 0.2 * W) *(btemp2 - btemp1) -
					(0.26 - 0.08 * W) * (btemp2 - btemp1)*(btemp2 - btemp1) + 
					(0.02 - 0.67 * W) + (64.5 - 7.35 * W) * (1 - emissivity) -
					(119 - 20.4 * W) * dEmissivity;
			}
			else if ((fCIStrEqual(m_sSensor, "modis"))){
				LST = btemp1 + 1.02 + 1.79*(btemp1 - btemp2) +
					1.2*(btemp1 - btemp2)*(btemp1 - btemp2) + 
					(34.83-0.68 * W)*(1-emissivity) + (-73.27-5.19*W)*dEmissivity;
			}
			else if ((fCIStrEqual(m_sSensor, "aster"))){
				//ASTER band 13 and 14 are used for the LST estimates
				//LST = btemp13 + a1* (btemp13 - btemp14) + a2* pow((btemp13-btemp14),2) + a0
				//	 +(a3 + a4* W)*(1-emissivity) + (a5 + a6*W)*dEmissivity;
				//Equation above may be simplified, if the emissivity dependence is neglected
				if (fInWaterVapor) 
					LST = btemp1 + 4.8257* (btemp1 - btemp2) + 0.5816* pow((btemp1-btemp2),2) + 0.2665
					 +(35.01 + 1.33* W)*(1-emissivity) + (-282.25 + 33.77*W)*dEmissivity;
				else
					LST = btemp1 + 4.8257* (btemp1 - btemp2) + 0.5816* pow((btemp1-btemp2),2) + 0.2665;

			}
			outputLST[x] = LST;
		}
		// write the result
		ptr.PutLineVal(y, outputLST);
		trq.fUpdate(y, iLines());	
		
	}	
	trq.fUpdate(iLines(), iLines());
	return true;
}

void MapLandSurfaceTemperature::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
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


