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
// MapAlbedo.cpp: implementation of the MapAlbedo class.
//
//////////////////////////////////////////////////////////////////////
/* SEBS model
   November 2007, by Lichun Wang
*/
#include "SEBS\MapWaterVapour.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

using namespace std;

IlwisObjectPtr * createMapWaterVapour(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapWaterVapour::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapWaterVapour(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapWaterVapour::MapWaterVapour(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	fNeedFreeze = true;
	sFreezeTitle = "Compute Atmospheric Water Vapour";
	ReadElement("MapWaterVapour", "Sensor", m_sSensor);
	if ((fCIStrEqual(m_sSensor, "modis"))){
		ReadElement("MapWaterVapour", "Band2", m_mpBand2);
		ReadElement("MapWaterVapour", "Band17", m_mpBand17);
		ReadElement("MapWaterVapour", "Band18", m_mpBand18);
		ReadElement("MapWaterVapour", "Band19", m_mpBand19);
		CompitableGeorefs(fn, mp, m_mpBand17);
		CompitableGeorefs(fn, mp, m_mpBand18);
		CompitableGeorefs(fn, mp, m_mpBand19);
	}
	else if ((fCIStrEqual(m_sSensor, "aatsr"))){
		ReadElement("MapWaterVapour", "Band1", m_mpBand1);
		ReadElement("MapWaterVapour", "Band2", m_mpBand2);
		CompitableGeorefs(fn, mp, m_mpBand2);
	}
}

MapWaterVapour::MapWaterVapour(
				 const FileName& fn, 
				 MapPtr& p,
				 String sSensor,
				 const Map& mpBand2,
				 const Map& mpBand17,
				 const Map& mpBand18,
				 const Map& mpBand19
				 )
: MapFromMap(fn, p, mpBand2),
	m_sSensor(sSensor),
	m_mpBand2(mpBand2),
	m_mpBand17(mpBand17),
	m_mpBand18(mpBand18),
	m_mpBand19(mpBand19)
{
	DomainValueRangeStruct dv(-400,400,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	CompitableGeorefs(fn, mp, m_mpBand17);
	CompitableGeorefs(fn, mp, m_mpBand18);
	CompitableGeorefs(fn, mp, m_mpBand19);
	objdep.Add(m_mpBand17);
	objdep.Add(m_mpBand18);
	objdep.Add(m_mpBand19);
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "Compute Atmospheric Water Vapour";
}

MapWaterVapour::MapWaterVapour(
				 const FileName& fn, 
				 MapPtr& p,
				 String sSensor,
				 const Map& mpBand1,
				 const Map& mpBand2
				 )
: MapFromMap(fn, p, mpBand1),
	m_sSensor(sSensor),
	m_mpBand1(mpBand1),
	m_mpBand2(mpBand2)
{
	DomainValueRangeStruct dv(-400,400,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	CompitableGeorefs(fn, mp, m_mpBand2);
	objdep.Add(m_mpBand2);
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "Compute Atmospheric Water Vapour";
}

MapWaterVapour::~MapWaterVapour()
{

}

const char* MapWaterVapour::sSyntax() {
  
  return "MapWaterVapour(MODIS, Band2, Band17, Band18, Band19), or \n"
         "MapWaterVapour(AATSR,Band1,Band2)"; 
}

MapWaterVapour* MapWaterVapour::create(const FileName& fn, MapPtr& p, const String& sExpr)
{

  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 3 )
	  ExpressionError(sExpr, sSyntax());
  String sSensor = as[0].sVal();
  if ((fCIStrEqual(sSensor, "aatsr"))){
	  Map mpBand1(as[1], fn.sPath());
	  Map mpBand2(as[2], fn.sPath());
	  return new MapWaterVapour(fn, p, 
		sSensor,
		mpBand1,
		mpBand2
		);
  }
  else if((fCIStrEqual(sSensor, "modis"))){
	if (iParms < 5 )
	  ExpressionError(sExpr, sSyntax());
	Map mpBand2(as[1], fn.sPath());
	Map mpBand17(as[2], fn.sPath());
	Map mpBand18(as[3], fn.sPath());
	Map mpBand19(as[4], fn.sPath());
	return new MapWaterVapour(fn, p, 
		sSensor,
		mpBand2,
		mpBand17,
		mpBand18,
		mpBand19);
  }	
}

void MapWaterVapour::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapWaterVapour");
  WriteElement("MapWaterVapour", "Sensor", m_sSensor);
  if ((fCIStrEqual(m_sSensor, "modis"))){
	WriteElement("MapWaterVapour", "Band2", m_mpBand2);
	WriteElement("MapWaterVapour", "Band17", m_mpBand17);
	WriteElement("MapWaterVapour", "Band18", m_mpBand18);
	WriteElement("MapWaterVapour", "Band19", m_mpBand19);
  }
  else if ((fCIStrEqual(m_sSensor, "aatsr"))){
	WriteElement("MapWaterVapour", "Band1", m_mpBand1);
	WriteElement("MapWaterVapour", "Band2", m_mpBand2);
	
  }
}
  
String MapWaterVapour::sExpression() const
{
	String sExp;
	if((fCIStrEqual(m_sSensor, "modis"))){
		sExp = String("MapWaterVapour(%S,%S,%S,%S,%S)", 
				m_sSensor,
				mp->sNameQuoted(false, fnObj.sPath()), 
				m_mpBand17->sNameQuoted(true),
				m_mpBand18->sNameQuoted(true),
				m_mpBand19->sNameQuoted(true)
				);
	}
    else if((fCIStrEqual(m_sSensor, "aatsr"))){
		sExp = String("MapWaterVapour(%S,%S,%S)", 
				m_sSensor,
				mp->sNameQuoted(false, fnObj.sPath()), 
				m_mpBand2->sNameQuoted(true)
				);
	}
  
	
	return sExp;
}

bool MapWaterVapour::fDomainChangeable() const
{
  return false;
}

bool MapWaterVapour::fGeoRefChangeable() const
{
  return false;
}

void MapWaterVapour::ReadBrightnessTemperatureMaps()
{
    trq.SetText("Reading brightness temperature files");
	RealBuf rBuf1;
	RealBuf rBuf2;
	rBuf1.Size(iCols());
	rBuf2.Size(iCols());
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_mpBand1->GetLineVal(iRow, rBuf1);
		m_mpBand2->GetLineVal(iRow, rBuf2);
		for (long iCol = 0; iCol< iCols(); iCol++ ){
			inputBand1[iRow*iCols()+iCol] = rBuf1[iCol];
			inputBand2[iRow*iCols()+iCol] = rBuf2[iCol];
		}
		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
}

bool MapWaterVapour::fFreezing()
{
	int width = iCols();
	int height = iLines();
	// x and y size of the window for taking the average temperature
	const int windowSize = 3; // prefer odd number

	// for near IR channels of MODIS used for water vapour retrieval
	RealBuf inputBand17;
	RealBuf inputBand18;
	RealBuf inputBand19;
	
	// for the temperature of aatsr, allocate memory to load the whole band at once
	if((fCIStrEqual(m_sSensor, "aatsr"))){
		inputBand1.Size(width * height);
		inputBand2.Size(width * height);
		ReadBrightnessTemperatureMaps();
    }
	else if((fCIStrEqual(m_sSensor, "modis"))){
		// allocate memory for single scan lines
		inputBand2.Size(width);
		inputBand17.Size(width);
		inputBand18.Size(width);
		inputBand19.Size(width);
	}

	// allocate memory for single scan lines of the output
	RealBuf outputWaterVapour;
	outputWaterVapour.Size(iCols());
	
	trq.SetTitle(sFreezeTitle);
	trq.SetText("Compute Atmospheric Water Vapour");
	trq.Start();
	
	for (int y = 0; y < height; y++) {
		if((fCIStrEqual(m_sSensor, "modis"))){
			// read these bands of MODIS by scanline
			m_mpBand2->GetLineVal(y, inputBand2); 
			m_mpBand17->GetLineVal(y, inputBand17); 
			m_mpBand18->GetLineVal(y, inputBand18); 
			m_mpBand19->GetLineVal(y, inputBand19); 
		}
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
		  if((fCIStrEqual(m_sSensor, "aatsr"))){
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
			//water vapour content retrieval by Su 2003, INT. J. Remote Sensing	
			double W = 13.73 - 13.662 * tau_ratio_nad; 
			outputWaterVapour[x] = W;
		  }
		  else if((fCIStrEqual(m_sSensor, "modis"))){
			//Atmospheric water vapour retrieval from radiance measurements,Sobrino.2003

			//calculate ratios for MODIS channels 17,18,19
			double G17 = inputBand17[x]/inputBand2[x];   
			double G18 = inputBand18[x]/inputBand2[x];   
			double G19 = inputBand19[x]/inputBand2[x];   

			//Water vapour values of MODIS channels 17,18 and 19
			double W17 = 26.314-54.434*G17+28.449*pow(G17,2);
			double W18 = 5.012-23.017*G18+27.884*pow(G18,2);
			double W19 = 9.446-26.887*G19+19.914*pow(G19,2);

			//Weight factors of MODIS channels 17,18,and 19
			double f17 = 0.192;
			double f18 = 0.453;
			double f19 = 0.355;

			//Total water vapour from MODIS images
			double W = f17*W17+f18*W18+f19*W19;
			outputWaterVapour[x] = W;
		  }	
		}
		// write the result
		ptr.PutLineVal(y, outputWaterVapour);
		trq.fUpdate(y, iLines());	
		
	}	
	trq.fUpdate(iLines(), iLines());
	return true;
}

void MapWaterVapour::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
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






