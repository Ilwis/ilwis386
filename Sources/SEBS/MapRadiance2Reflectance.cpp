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
// MapRadiance2Reflectance.cpp
//
//////////////////////////////////////////////////////////////////////
/* SEBS model
   November 2008, by Lichun Wang
   Convert (ASTER) radiance values to reflectance, by Lichun Wang  
*/
#include "Engine\Applications\MAPFMAP.H"
#include "SEBS\MapRadiance2Reflectance.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\hs\map.hs"

using namespace std;

IlwisObjectPtr * createMapRadiance2Reflectance(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapRadiance2Reflectance::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapRadiance2Reflectance(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapRadiance2Reflectance::MapRadiance2Reflectance(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	String sSensor;
	String daynumber,band;
    ReadElement("MapRadiance2Reflectance", "Sensor", m_Sensor);
	ReadElement("MapRadiance2Reflectance", "Radiance", m_mpRad);
	ReadElement("MapRadiance2Reflectance", "SolarzenithAngle", m_sza);
	ReadElement("MapRadiance2Reflectance", "DayNumber", daynumber);
	m_daynumber=daynumber.iVal();
	if (fCIStrEqual(m_Sensor.toLower(), "aster")){
		ReadElement("MapRadiance2Reflectance", "Band", band);
		m_band = band.iVal();
	}
	else
		ReadElement("MapRadiance2Reflectance", "ESUN", m_esun);
	fNeedFreeze = true;
	sFreezeTitle = TR("Converting Radiance Values to Reflectance");
}

MapRadiance2Reflectance::MapRadiance2Reflectance(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRad,
				 short daynumber,
				 double sza,
				 String sensor,
				 short band
				 )
: MapFromMap(fn, p, mpRad),
	m_mpRad(mpRad),
	m_daynumber(daynumber),
	m_sza(sza),
	m_Sensor(sensor),
	m_band(band)
{
	DomainValueRangeStruct dv(0,100,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	objdep.Add(m_mpRad);
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = TR("Converting Radiance Values to Reflectance");
}

MapRadiance2Reflectance::MapRadiance2Reflectance(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRad,
				 short daynumber,
				 double sza,
				 double esun
				 )
: MapFromMap(fn, p, mpRad),
	m_mpRad(mpRad),
	m_daynumber(daynumber),
	m_sza(sza),
	m_esun(esun),
	m_Sensor("any")
{
	DomainValueRangeStruct dv(0,200,0.001);
	SetDomainValueRangeStruct(dv);
	//init();
	objdep.Add(m_mpRad);
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = TR("Converting Radiance Values to Reflectance");
}

MapRadiance2Reflectance::~MapRadiance2Reflectance()
{

}

const char* MapRadiance2Reflectance::sSyntax() {
	
  return "MapRadiance2Reflectance(RadianceMap,DayNumber,SZA,ESUN), or \n"
         "MapRadiance2Reflectance(RadianceMap,DayNumber,SZA,ASTER,BandNumber)"; 
}

MapRadiance2Reflectance* MapRadiance2Reflectance::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 4 && iParms != 5 )
      ExpressionError(sExpr, sSyntax());

  Map mpRad(as[0], fn.sPath());
  short daynumber = as[1].iVal();
  double sza = as[2].rVal();
  double esun;
  if (iParms == 4){
	esun = 	as[3].rVal();
	return new MapRadiance2Reflectance(fn, p, mpRad,daynumber,sza,esun);
  }
  else if (iParms == 5){
    String sensor = as[3].sVal();
	if (!fCIStrEqual(sensor.toLower(), "aster"))
		ExpressionError(sExpr, sSyntax());
	short band = as[4].iVal();
	return new MapRadiance2Reflectance(fn, p, mpRad,daynumber,sza,sensor,band);
  }
  return NULL;
}

void MapRadiance2Reflectance::Store()
{
  MapFromMap::Store();
  String sSensor;
  WriteElement("MapFromMap", "Type", "MapRadiance2Reflectance");
  WriteElement("MapRadiance2Reflectance", "Sensor", m_Sensor);
  WriteElement("MapRadiance2Reflectance", "Radiance", m_mpRad);
  WriteElement("MapRadiance2Reflectance", "DayNumber", m_daynumber);
  WriteElement("MapRadiance2Reflectance", "SolarzenithAngle", m_sza);
  if (fCIStrEqual(m_Sensor.toLower(), "aster")){
	WriteElement("MapRadiance2Reflectance", "Band", m_band);
  }
  else
	WriteElement("MapRadiance2Reflectance", "ESUN", m_esun);
}

String MapRadiance2Reflectance::sExpression() const
{
  
	String sensor;
	String sExp;
	if (fCIStrEqual(sensor.toLower(), "aster")){
		sExp = String("MapRadiance2Reflectance(%S,%li,%g,%S,%li)", 
						mp->sNameQuoted(true, fnObj.sPath()),
						m_daynumber,
						m_sza,
						m_Sensor,
						m_band);
	}
	else{
		sExp = String("MapRadiance2Reflectance(%S,%li,%g,%g)", 
						mp->sNameQuoted(true, fnObj.sPath()),
						m_daynumber,
						m_sza,
						m_esun);
	}
	return sExp;
}

bool MapRadiance2Reflectance::fDomainChangeable() const
{
  return false;
}

bool MapRadiance2Reflectance::fGeoRefChangeable() const
{
  return false;
}

bool MapRadiance2Reflectance::fFreezing()
{
	trq.SetText(TR("Converting Radiance Values to Reflectance"));

	RealBuf rBufRad;
	RealBuf rBufOutput;
	rBufRad.Size(iCols());
	rBufOutput.Size(iCols());
	 
	// esun for ASTER band
	if (fCIStrEqual(m_Sensor.toLower(), "aster")){
		if (m_band == 1)
			m_esun=1845.99;
		else if(m_band == 2)
			m_esun=1555.74;
		else if(m_band == 3)
			m_esun=1119.47;
		else if(m_band == 4)
			m_esun=231.25;
		else if(m_band == 5)
			m_esun=79.81;
		else if(m_band == 6)
			m_esun=74.99;
		else if(m_band == 7)
			m_esun=68.66;
		else if(m_band == 8)
			m_esun=59.74;
		else if(m_band == 9)
			m_esun=56.92;
	}
	
	double pi=3.14159;
	double d=1-0.01672*cos((pi/180)*0.9856*(m_daynumber-4)); //sun to earth distance
	for (long iRow = 0; iRow< iLines(); iRow++ )
	{
		m_mpRad->GetLineVal(iRow, rBufRad);
		for (long iCol = 0; iCol< iCols(); iCol++){
			double Lrad = rBufRad[iCol];
			double Rtoa = (pi*Lrad*pow(d,2))/(m_esun*cos(m_sza*pi/180));
			rBufOutput[iCol] = Rtoa;
		}
		// write the result
		ptr.PutLineVal(iRow, rBufOutput);
		trq.fUpdate(iRow, iLines());	
	}
	trq.fUpdate(iLines(), iLines());
	return true;
}

