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

 Created on: 2007-28-8
 ***************************************************************/
// MapSEBS.cpp: implements algorithms specific to the Surface Energy Balance System (SEBS)
// to calculate the evaporative fraction from satellite earth observation data and meteorological data.
//////////////////////////////////////////////////////////////////////
/* SEBS model
   August 2007, by Lichun Wang
   updated 2011, by Lichun Wang
*/
#include "SEBS\MapSEBS.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"
#include "SEBS\kb_1.h"
#include "SEBS\EnergyBalance.h"

using namespace std;

IlwisObjectPtr * createMapSEBS(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapSEBS::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapSEBS(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static Table VerifyAttributes(Map mpMap)
{
	
	Table tblAtt = mpMap->tblAtt();
	if (!tblAtt.fValid())
		throw ErrorNoAttTable(mpMap->fnObj);
	if (!tblAtt[String("Z0m")].fValid())
		ColumnNotFoundError(tblAtt->fnObj, String("Z0m"));
	return tblAtt;
}

MapSEBS::MapSEBS(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	fNeedFreeze = true;
	sFreezeTitle = TR("Surface Energy Balance System (SEBS)");
	//htpFreeze = "ilwisapp\flow_accumulation_algorithm.htm";

	ReadElement("RelativeEvaporationFraction", "LandSurfaceTemperature", m_mpLST);
	ReadElement("RelativeEvaporationFraction", "Emissivity", m_mpEmis);
	ReadElement("RelativeEvaporationFraction", "Albedo", m_mpAlbedo);
	ReadElement("RelativeEvaporationFraction", "NDVI", m_mpNDVI);
	CompitableGeorefs(fn, mp, m_mpEmis);
	CompitableGeorefs(fn, mp, m_mpAlbedo);
	CompitableGeorefs(fn, mp, m_mpNDVI);
	ReadElement("RelativeEvaporationFraction", "UseLeafAreaIndex", m_USE_LAI);
	if (m_USE_LAI){
		ReadElement("RelativeEvaporationFraction", "LeafAreaIndexMap", m_mpLAI);
		CompitableGeorefs(fn, mp, m_mpLAI);
	}
	ReadElement("RelativeEvaporationFraction", "UseVegetationProportion", m_USE_Pv);
	if (m_USE_Pv){
		ReadElement("RelativeEvaporationFraction", "VegetationProportion", m_mpPv);
		CompitableGeorefs(fn, mp, m_mpPv);
	}
	
	ReadElement("RelativeEvaporationFraction", "USESunZenithAngleMap", m_use_Sza);
	if (m_use_Sza){
		ReadElement("RelativeEvaporationFraction", "SunZenithAngle", m_mpSunZenithAngle);
		CompitableGeorefs(fn, mp, m_mpSunZenithAngle);
		m_rSza = -1;
	}
	else{
		ReadElement("RelativeEvaporationFraction", "AvgSunZenithAngle", m_rSza);
		m_sSza = "nomap";
	}
	ReadElement("RelativeEvaporationFraction", "USEDemMap", m_use_Dem);
	if (m_use_Dem){
		ReadElement("RelativeEvaporationFraction", "DEM", m_mpDEM);
		CompitableGeorefs(fn, mp, m_mpDEM);
	}
	else
		ReadElement("RelativeEvaporationFraction", "AvgSunZenithAngle", m_rDEM);
	ReadElement("RelativeEvaporationFraction", "USEDayNumber", m_use_daynumber);
    if (m_use_daynumber)
		ReadElement("RelativeEvaporationFraction", "DayNumber", m_daynumber);
	else{
		ReadElement("RelativeEvaporationFraction", "Month", m_month);
		ReadElement("RelativeEvaporationFraction", "Day", m_day);
		ReadElement("RelativeEvaporationFraction", "Year", m_year);
	}
	ReadElement("RelativeEvaporationFraction", "ReferenceHeight", m_Z_ref);
	ReadElement("RelativeEvaporationFraction", "PBLHeight", m_hi);

    ReadElement("RelativeEvaporationFraction", "UseHumidityMap", m_useQ_ref_map);
	if (m_useQ_ref_map){
		ReadElement("RelativeEvaporationFraction", "HumidityMap", m_mpQ_ref);
		CompitableGeorefs(fn, mp, m_mpQ_ref);
	}
	else
		ReadElement("RelativeEvaporationFraction", "Humidity", m_Q_ref);
	ReadElement("RelativeEvaporationFraction", "UseWindSpeedMap", m_useU_ref_map);
	if (m_useU_ref_map){
		ReadElement("RelativeEvaporationFraction", "WindSpeedMap", m_mpU_ref);
		CompitableGeorefs(fn, mp, m_mpU_ref);
	}
	else
		ReadElement("RelativeEvaporationFraction", "WindSpeed", m_U_ref);
	ReadElement("RelativeEvaporationFraction", "UseAirTemperatureMap", m_useT_ref_map);
	if(m_useT_ref_map){
		ReadElement("RelativeEvaporationFraction", "AirTemperatureMap", m_mpT_ref);
		CompitableGeorefs(fn, mp, m_mpT_ref);
	}
	else
		ReadElement("RelativeEvaporationFraction", "AirTemperature", m_T_ref);
	ReadElement("RelativeEvaporationFraction", "UsePressureMap", m_useP_ref_map);
	if (m_useP_ref_map){
		ReadElement("RelativeEvaporationFraction", "PressureMap", m_mpP_ref);
		CompitableGeorefs(fn, mp, m_mpP_ref);
	}
	else
		ReadElement("RelativeEvaporationFraction", "Pressure", m_P_ref);
	ReadElement("RelativeEvaporationFraction", "UseSurfacePressureMap", m_useP_sur_map);
	if(m_useP_sur_map){
		ReadElement("RelativeEvaporationFraction", "SurfacePressureMap", m_mpP_sur);
		CompitableGeorefs(fn, mp, m_mpP_sur);
	}
	else
		ReadElement("RelativeEvaporationFraction", "SurfacePressure", m_P_sur);
	ReadElement("RelativeEvaporationFraction", "HorizontalVisibility", m_Visi);
	ReadElement("RelativeEvaporationFraction", "CalculateDownwardSolarRadiation", m_USE_Sdwn);
	ReadElement("RelativeEvaporationFraction", "DownwardSolarRadiation", m_S_dwn);
	ReadElement("RelativeEvaporationFraction", "UseDownwardSolarRadiationMap", m_use_SdwnMap);
	if (m_use_SdwnMap){
		ReadElement("RelativeEvaporationFraction", "SdwnMap", m_mpSdwn);
		CompitableGeorefs(fn, mp, m_mpSdwn);
	}
	ReadElement("RelativeEvaporationFraction", "USELandUseMap", m_USE_LUM);
	if (m_USE_LUM){
		ReadElement("RelativeEvaporationFraction", "LandUseMap", m_mpLandUse);
		//Table tbl = VerifyAttributes(m_mpLandUse);
		CompitableGeorefs(fn, mp, m_mpLandUse);
	}
	ReadElement("RelativeEvaporationFraction", "USECanopyHeight", m_USE_hc);
	if (m_USE_hc){
		ReadElement("RelativeEvaporationFraction", "CanopyHeightMap", m_mpHc);
		CompitableGeorefs(fn, mp, m_mpHc);
	}
	ReadElement("RelativeEvaporationFraction", "USEDisplacementHeight", m_USE_d0);
	if (m_USE_d0){
		ReadElement("RelativeEvaporationFraction", "DisplacementHeightMap", m_mpD0);
		CompitableGeorefs(fn, mp, m_mpD0);
	}

	ReadElement("RelativeEvaporationFraction", "USEMeanAirTemperatureMap", m_useTa_avg_map);
	if (m_useTa_avg_map){
		ReadElement("RelativeEvaporationFraction", "MeanAirTemperatureMap", m_mpTa_avg);
		CompitableGeorefs(fn, mp, m_mpTa_avg);
	}
	else
		ReadElement("RelativeEvaporationFraction", "MeanAirTemperature", m_Ta_avg);

	ReadElement("RelativeEvaporationFraction", "USESunshineHoursMap", m_useN_map);
	if (m_useN_map){
		ReadElement("RelativeEvaporationFraction", "SunshineHoursMap", m_mpN);
		CompitableGeorefs(fn, mp, m_mpN);
	}
	else
		ReadElement("RelativeEvaporationFraction", "SunshineHours", m_rN);
	ReadElement("RelativeEvaporationFraction", "USEKB", m_USE_KB);
	ReadElement("RelativeEvaporationFraction", "USEKBMAP", m_fKBmap);
	if (m_USE_KB){
		if(m_fKBmap){
			ReadElement("RelativeEvaporationFraction", "KB", m_mpKB);
			CompitableGeorefs(fn, mp, m_mpKB);
        }
		else 
		    ReadElement("RelativeEvaporationFraction", "KB", m_kb);
	}
  
}

MapSEBS::MapSEBS(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpLST,
				 const Map& mpEmis,
                 const Map& mpAlbedo,
				 const Map& mpNDVI,
                 bool useLAI,String sLAI,
                 bool usePv,String sPv,
				 bool use_Sza,String sSza,double rSza,
				 bool use_Dem,String sDem,double rDem,
                 bool use_daynumber,long daynumber,long mm,long dd,long yy,
				 double Z_ref,
				 double hi,
				 bool useQ_ref_map,String sQ_ref,double Q_ref,
				 bool useU_ref_map,String sU_ref,double U_ref,
				 bool useT_ref_map,String sT_ref,double T_ref,
				 bool useP_ref_map,String sP_ref,double P_ref,
				 bool useP_sur_map,String sP_sur,double P_sur,
				 double Visi,
				 bool use_Sdwn,
				 double S_dwn,
				 bool use_LUM,String sLUM,bool use_Hc,String sHc,bool use_D0,String sD0,
				 bool use_SdwnMap,String sDwn,
				 bool useTa_avg_map,String sTa_avg,double Ta_avg,
				 bool useN_map,String sN,double rN,
				 bool useKB, double rKB, String sKB, bool useKBMap)
: MapFromMap(fn, p, mpLST),
	m_mpLST(mpLST),
	m_mpEmis(mpEmis),
    m_mpAlbedo(mpAlbedo),
	m_USE_LAI(useLAI),
	m_sLAI(sLAI),
	m_USE_Pv(usePv),
	m_sPv(sPv),
	m_mpNDVI(mpNDVI),
	m_use_Sza(use_Sza),m_sSza(sSza),
	m_rSza(rSza),m_use_Dem(use_Dem),
	m_sDEM(sDem),m_rDEM(rDem),
	m_use_daynumber(use_daynumber),m_daynumber(daynumber),m_month(mm),m_day(dd),m_year(yy),
	m_Z_ref(Z_ref),
	m_hi(hi),
	m_useQ_ref_map(useQ_ref_map),m_sQ_ref(sQ_ref),m_Q_ref(Q_ref),
	m_useU_ref_map(useU_ref_map),m_sU_ref(sU_ref),m_U_ref(U_ref),
	m_useT_ref_map(useT_ref_map),m_sT_ref(sT_ref),m_T_ref(T_ref),
	m_useP_ref_map(useP_ref_map),m_sP_ref(sP_ref),m_P_ref(P_ref),
	m_useP_sur_map(useP_sur_map),m_sP_sur(sP_sur),m_P_sur(P_sur),
	m_Visi(Visi),
	m_USE_Sdwn(use_Sdwn),m_S_dwn(S_dwn),
	m_USE_LUM(use_LUM),m_sLUM(sLUM),m_USE_hc(use_Hc),m_sHc(sHc),m_USE_d0(use_D0),m_sD0(sD0),
	m_use_SdwnMap(use_SdwnMap),m_sDwn(sDwn),
	m_useTa_avg_map(useTa_avg_map),m_sTa_avg(sTa_avg),m_Ta_avg(Ta_avg),
	m_useN_map(useN_map),m_sN(sN),m_rN(rN),
	m_USE_KB(useKB), m_kb(rKB), m_sKB_map(sKB),m_fKBmap(useKBMap)

{
	DomainValueRangeStruct dv(0,10,0.0001);
	SetDomainValueRangeStruct(dv);
	//Verify compatible geo-reference
	CompitableGeorefs(fn, mp, m_mpEmis);
	CompitableGeorefs(fn, mp, m_mpAlbedo);
	objdep.Add(m_mpEmis);
	objdep.Add(m_mpAlbedo);
	if (m_USE_LAI){
		m_mpLAI = Map(sLAI, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpLAI);
		objdep.Add(m_mpLAI);
	}
	if (m_USE_Pv){
		m_mpPv = Map(sPv, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpPv);
		objdep.Add(m_mpPv);
	}
	CompitableGeorefs(fn, mp, m_mpNDVI);
	objdep.Add(m_mpNDVI);
	
	if(m_use_Dem){
		m_mpDEM = Map(sDem, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpDEM);
		objdep.Add(m_mpDEM);
	}
	if(m_use_Sza){
		m_mpSunZenithAngle = Map(sSza, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpSunZenithAngle);
		objdep.Add(m_mpSunZenithAngle);
	}
	if(m_USE_LUM){
		m_mpLandUse = Map(sLUM, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpLandUse);
		//Table tbl = VerifyAttributes(m_mpLandUse);
		objdep.Add(m_mpLandUse);
	}

	if(m_USE_hc){
		m_mpHc = Map(sHc, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpHc);
		objdep.Add(m_mpHc);
	}
	if(m_USE_d0){
		m_mpD0 = Map(sD0, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpD0);
		objdep.Add(m_mpD0);
	}

	if(m_useQ_ref_map){
		m_mpQ_ref = Map(sQ_ref, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpQ_ref);
		objdep.Add(m_mpQ_ref);
	}

	if (m_useU_ref_map){
		m_mpU_ref = Map(sU_ref, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpU_ref);
		objdep.Add(m_mpU_ref);
	}

	if(m_useT_ref_map){
		m_mpT_ref = Map(sT_ref, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpT_ref);
		objdep.Add(m_mpT_ref);
	}
	
	if (m_useP_ref_map){
		m_mpP_ref = Map(sP_ref, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpP_ref);
		objdep.Add(m_mpP_ref);
	}
	
	if(m_useP_sur_map){
		m_mpP_sur = Map(sP_sur, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpP_sur);
		objdep.Add(m_mpP_sur);
	}

	if(m_use_SdwnMap){
		m_mpSdwn = Map(sDwn, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpSdwn);
		objdep.Add(m_mpSdwn);
	}
	if(useTa_avg_map){ //mean air temperature
		m_mpTa_avg = Map(sTa_avg, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpTa_avg);
		objdep.Add(m_mpTa_avg);
	}
	if(useN_map){ //sunshine hours per day
		m_mpN = Map(sN, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpN);
		objdep.Add(m_mpN);
	}

	if(useKB){ //kb map
		if(useKBMap){
		  m_mpKB = Map(sKB, fn.sPath());
		  CompitableGeorefs(fn, mp, m_mpKB);
		  objdep.Add(m_mpKB);
        }
	}
	
	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "Surface Energy Balance System (SEBS)";
}

MapSEBS::~MapSEBS()
{

}

const char* MapSEBS::sSyntax() {
  
  return "MapSEBS(MapLST,MapEmis,MapAlbedo, MapNDVI,\n"
			"true|false,MapLAI, \n"
			"true|false,MapVegetationProportion(Pv), \n"
			"true|false,MapSZA,AvgValueSza, \n"
			"true|false,MapDEM,AvgValueDEM, \n"
			"true|false, daynumber, month,day,year, \n"
			"Z_ref, \n"
			"hi, \n"
			"true|false, HumidityMap,  Q_ref,\n"
			"true|false, WindSpeedMap, U_ref,\n"
			"true|false, AirTemperatureMap, T_ref,\n"
			"true|false, PressureRefMap, P_ref,\n"
			"true|false, PressureSurMap, P_sur,\n"
			"Visi, \n"
			"true|false,S_dwnVal, \n"
			"true|false,MapSurfaceRoughness, \n"
			"true|false,MapCanopyHeight, \n"
			"true|false,MapDisplacementHeight, \n"
			"true|false,sdwnMap, \n"
			"true|false, MeanAirTemperatureMap, MeanAirTemperatureMapVal,\n"
			"true|false, SunshineHoursPerDayMap, SunshineHoursPerDayVal\n"
			"true|false,KB, KBMap, true|false)\n";
}

MapSEBS* MapSEBS::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 55 )
      ExpressionError(sExpr, sSyntax());

    Map mpLST(as[0], fn.sPath());
	Map mpEmis(as[1], fn.sPath());
	Map mpAlbedo(as[2],fn.sPath());
	Map mpNDVI(as[3],fn.sPath());

	bool use_lai = as[4].fVal();
	String sLai = as[5].sVal();
	bool use_Pv = as[6].fVal();
	String sPv = as[7].sVal();
	
	bool use_Sza = as[8].fVal();
	String sSza = as[9].sVal();
	double rSza = as[10].rVal();
	bool use_Dem = as[11].fVal();
	String sDem = as[12].sVal();
	double rDem = as[13].rVal();
	bool use_daynumber = as[14].fVal();
	long daynumber = as[15].iVal();
	long month = as[16].iVal();
	long day = as[17].iVal();
	long year = as[18].iVal();
	double Z_ref = as[19].rVal();
	double hi = as[20].rVal();

	bool useQ_ref_map = as[21].fVal();
	String sQ_ref = as[22].sVal();
	double Q_ref = as[23].rVal();
	bool useU_ref_map = as[24].fVal();
	String sU_ref = as[25].sVal();
	double U_ref = as[26].rVal();
	bool useT_ref_map = as[27].fVal();
	String sT_ref = as[28].sVal();
	double T_ref = as[29].rVal();
	bool useP_ref_map = as[30].fVal();
	String sP_ref = as[31].sVal();
	double P_ref = as[32].rVal();
	bool useP_sur_map = as[33].fVal();
	String sP_sur = as[34].sVal();
	double P_sur = as[35].rVal();
	double Visi = as[36].rVal();
	bool calc_Sdwn = as[37].fVal();
	double S_dwn = as[38].rVal();
	bool use_lum = as[39].fVal();
	String sLum = as[40].sVal();
	bool use_hc = as[41].fVal();
	String sHc = as[42].sVal();
	bool use_d0 = as[43].fVal();

	String sD0 = as[44].sVal();
	bool use_sdwnMap = as[45].fVal();
	String sdwnMap = as[46].sVal();
	
	bool useTa_avg_map = as[47].fVal();
	String sTa_avg = as[48].sVal();
	double Ta_avg = as[49].rVal();

	bool useN_map = as[50].fVal();
	String sN = as[51].sVal();
	double rN = as[52].rVal();

	bool use_kb = as[53].fVal();
	double rKB = as[54].rVal();
	String sKB = as[55].sVal();
	bool useKBMap = as[56].fVal();
		
	return new MapSEBS(fn, p, 
				   mpLST,
				   mpEmis,
                   mpAlbedo,
				   mpNDVI,
				   use_lai,sLai,
				   use_Pv,sPv,
				   use_Sza,sSza,rSza,
				   use_Dem,sDem,rDem,
				   use_daynumber,daynumber,month,day,year,
				   Z_ref,
				   hi,
				   useQ_ref_map,sQ_ref,Q_ref,
				   useU_ref_map,sU_ref,U_ref,
				   useT_ref_map,sT_ref,T_ref,
				   useP_ref_map,sP_ref,P_ref,
				   useP_sur_map,sP_sur,P_sur,
				   Visi,
				   calc_Sdwn,S_dwn,
				   use_lum,sLum,use_hc,sHc,use_d0,sD0,
				   use_sdwnMap,sdwnMap,
				   useTa_avg_map,sTa_avg,Ta_avg,
				   useN_map,sN,rN,
				   use_kb, rKB,sKB, useKBMap);
}

void MapSEBS::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapSEBS");
  WriteElement("RelativeEvaporationFraction", "LandSurfaceTemperature", m_mpLST);
  WriteElement("RelativeEvaporationFraction", "Emissivity", m_mpEmis);
  WriteElement("RelativeEvaporationFraction", "Albedo", m_mpAlbedo);
  WriteElement("RelativeEvaporationFraction", "NDVI", m_mpNDVI);

  WriteElement("RelativeEvaporationFraction", "USELeafAreaIndex", m_USE_LAI);
  if (m_USE_LAI)
		WriteElement("RelativeEvaporationFraction", "LeafAreaIndexMap", m_mpLAI);
  WriteElement("RelativeEvaporationFraction", "UseVegetationProportion", m_USE_Pv);
  if (m_USE_Pv)
		WriteElement("RelativeEvaporationFraction", "VegetationProportion", m_mpPv);
  	
  WriteElement("RelativeEvaporationFraction", "USESunZenithAngleMap", m_use_Sza);
  if (m_use_Sza){
  	WriteElement("RelativeEvaporationFraction", "SunZenithAngle", m_mpSunZenithAngle);
  }
  else{
  	WriteElement("RelativeEvaporationFraction", "AvgSunZenithAngle", m_rSza);
  }

  WriteElement("RelativeEvaporationFraction", "USEDemMap", m_use_Dem);
  if (m_use_Dem)
	WriteElement("RelativeEvaporationFraction", "DEM", m_mpDEM);
  else
	WriteElement("RelativeEvaporationFraction", "AvgSunZenithAngle", m_rDEM);
  WriteElement("RelativeEvaporationFraction", "USEDayNumber", m_use_daynumber);
  if (m_use_daynumber)
	WriteElement("RelativeEvaporationFraction", "DayNumber", m_daynumber);
  else{
	WriteElement("RelativeEvaporationFraction", "Month", m_month);
	WriteElement("RelativeEvaporationFraction", "Day", m_day);
	WriteElement("RelativeEvaporationFraction", "Year", m_year);
  }	

  WriteElement("RelativeEvaporationFraction", "ReferenceHeight", m_Z_ref);
  WriteElement("RelativeEvaporationFraction", "PBLHeight", m_hi);

  WriteElement("RelativeEvaporationFraction", "UseHumidityMap", m_useQ_ref_map);
  WriteElement("RelativeEvaporationFraction", "UseWindSpeedMap", m_useU_ref_map);
  WriteElement("RelativeEvaporationFraction", "UseAirTemperatureMap", m_useT_ref_map);
  WriteElement("RelativeEvaporationFraction", "UsePressureMap", m_useP_ref_map);
  WriteElement("RelativeEvaporationFraction", "UseSurfacePressureMap", m_useP_sur_map);
  	if (m_useQ_ref_map)
		WriteElement("RelativeEvaporationFraction", "HumidityMap", m_mpQ_ref);
	else
		WriteElement("RelativeEvaporationFraction", "Humidity", m_Q_ref);
	
	if (m_useU_ref_map)
		WriteElement("RelativeEvaporationFraction", "WindSpeedMap", m_mpU_ref);
	else
		WriteElement("RelativeEvaporationFraction", "WindSpeed", m_U_ref);
	
	if(m_useT_ref_map)
		WriteElement("RelativeEvaporationFraction", "AirTemperatureMap", m_mpT_ref);
	else
		WriteElement("RelativeEvaporationFraction", "AirTemperature", m_T_ref);
	
	WriteElement("RelativeEvaporationFraction", "USEMeanAirTemperatureMap", m_useTa_avg_map);
	if (m_useP_ref_map)
		WriteElement("RelativeEvaporationFraction", "PressureMap", m_mpP_ref);
	else
		WriteElement("RelativeEvaporationFraction", "Pressure", m_P_ref);
	
	if(m_useP_sur_map)
		WriteElement("RelativeEvaporationFraction", "SurfacePressureMap", m_mpP_sur);
	else
		WriteElement("RelativeEvaporationFraction", "SurfacePressure", m_P_sur);
		

  WriteElement("RelativeEvaporationFraction", "HorizontalVisibility", m_Visi);	
  WriteElement("RelativeEvaporationFraction", "CalculateDownwardSolarRadiation", m_USE_Sdwn);
  WriteElement("RelativeEvaporationFraction", "DownwardSolarRadiation", m_S_dwn);
  WriteElement("RelativeEvaporationFraction", "USELandUseMap", m_USE_LUM); //surface roughtness
  WriteElement("RelativeEvaporationFraction", "USECanopyHeight", m_USE_hc);
  WriteElement("RelativeEvaporationFraction", "USEDisplacementHeight", m_USE_d0);
  if (m_USE_LUM)
		WriteElement("RelativeEvaporationFraction", "LandUseMap", m_mpLandUse);
  if (m_USE_hc)
		WriteElement("RelativeEvaporationFraction", "CanopyHeightMap", m_mpHc);
  if (m_USE_d0)
		WriteElement("RelativeEvaporationFraction", "DisplacementHeightMap", m_mpD0);
  WriteElement("RelativeEvaporationFraction", "UseDownwardSolarRadiationMap", m_use_SdwnMap);
  if (m_use_SdwnMap){
	WriteElement("RelativeEvaporationFraction", "SdwnMap", m_mpSdwn);
		
  }

  WriteElement("RelativeEvaporationFraction", "USEMeanAirTemperatureMap", m_useTa_avg_map);
  if (m_useTa_avg_map)
	WriteElement("RelativeEvaporationFraction", "MeanAirTemperatureMap", m_mpTa_avg);
  else
	WriteElement("RelativeEvaporationFraction", "MeanAirTemperature", m_Ta_avg);

  WriteElement("RelativeEvaporationFraction", "USESunshineHoursMap", m_useN_map);
  if (m_useN_map)
	WriteElement("RelativeEvaporationFraction", "SunshineHoursMap", m_mpN);
  else
	WriteElement("RelativeEvaporationFraction", "SunshineHours", m_rN);
  WriteElement("RelativeEvaporationFraction", "USEKB", m_USE_KB);
  WriteElement("RelativeEvaporationFraction", "USEKBMAP", m_fKBmap);
  if(m_USE_KB){
	if(m_fKBmap) 
		WriteElement("RelativeEvaporationFraction", "KB", m_mpKB);
    else 
		WriteElement("RelativeEvaporationFraction", "KB", m_kb);
  }
}

String MapSEBS::sExpression() const
{
  
	String sExp("MapSEBS(%S,%S,%S,%S,%li,%S,%li,%S,%li,%S,%g,%li,%S,%g,%li,%li,%li,%li,%li,%g,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%g,%li,%g,%li,%S,%li,%S,%li,%S,%li,%S,%li,%S,%g,%li,%S,%g,%li,%g,%S,%li)", 
										mp->sNameQuoted(false, fnObj.sPath()), 
										m_mpEmis->sNameQuoted(true),
										m_mpAlbedo->sNameQuoted(true),
										m_mpNDVI->sNameQuoted(true),
										m_USE_LAI,m_sLAI,
										m_USE_Pv,m_sPv,
										m_use_Sza,m_sSza,m_rSza,
										m_use_Dem,m_sDEM,m_rDEM,
										m_use_daynumber,m_daynumber,m_month,m_day,m_year,
										m_Z_ref,
										m_hi,
										m_useQ_ref_map,m_sQ_ref,m_Q_ref,
										m_useU_ref_map,m_sU_ref,m_U_ref,
										m_useT_ref_map,m_sT_ref,m_T_ref,
										m_useP_ref_map,m_sP_ref,m_P_ref,
										m_useP_sur_map,m_sP_sur,m_P_sur,
										m_Visi,
										m_USE_Sdwn,m_S_dwn,
										m_USE_LUM,m_sLUM,m_USE_hc,m_sHc,m_USE_d0,m_sD0,
										m_use_SdwnMap,m_sDwn,
										m_useTa_avg_map,m_sTa_avg,m_Ta_avg,
										m_useN_map,m_sN,m_rN,
										m_USE_KB,m_kb,m_sKB_map,m_fKBmap);
  return sExp;
}

bool MapSEBS::fDomainChangeable() const
{
  return false;
}

bool MapSEBS::fGeoRefChangeable() const
{
  return false;
}

bool MapSEBS::fFreezing()
{
  	int width = iCols();
	int height = iLines();

	double mm=m_S_dwn;

	//NDVI factors for vegetation fractional coverage Pv, from Sobrino method
	const double NDVImin = 0.2;
	const double NDVImax = 0.5;
	
	RealBuf inputLST;
	RealBuf inputEmsi;
	RealBuf inputAlbedo;
	RealBuf inputBandLai;
	RealBuf inputBandPv;
	RealBuf inputBandNDVI;
	RealBuf inputBandSza;
	RealBuf inputBandLat;
	RealBuf inputBandDem;

	RealBuf inputQ_ref;
	RealBuf inputU_ref;
	RealBuf inputT_ref;
	RealBuf inputP_ref;
	RealBuf inputP_sur;
	RealBuf inputBandLanduse;
	RealBuf inputBandHc;
	RealBuf inputBandD0;
	RealBuf inputSdwn;
	RealBuf inputTa_avg;
	RealBuf inputN_s;
	RealBuf inputKB_map;

	RealBuf outputBand1;  
	RealBuf outputBand2;   
	RealBuf outputBand3;
	RealBuf outputBand4;
	RealBuf outputBand5;
	RealBuf outputBand6;
	RealBuf outputBand7;
	RealBuf outputBand8;
	RealBuf outputBand9;

	RealBuf outputBand10;
	RealBuf outputBand11;
	RealBuf outputBand12;
	RealBuf outputBand13;
	RealBuf outputBand14;
	RealBuf outputBand15;
	RealBuf outputBand16;


	// allocate memory for single scan lines of the inputs
	inputLST.Size(width);
	inputEmsi.Size(width);
	inputAlbedo.Size(width);
	inputBandLai.Size(width);
	inputBandPv.Size(width);
	inputBandNDVI.Size(width);
	inputBandSza.Size(width);
	inputBandDem.Size(width);
	inputBandLat.Size(width);
	inputBandLanduse.Size(width);
	inputBandHc.Size(width);
	inputBandD0.Size(width);
	inputQ_ref.Size(width);
	inputU_ref.Size(width);
	inputT_ref.Size(width);
	inputP_ref.Size(width);
	inputP_sur.Size(width);
	inputSdwn.Size(width);
	inputTa_avg.Size(width);
	inputN_s.Size(width);
	inputKB_map.Size(width);

	// allocate memory for single scan lines of the outputs
	outputBand1.Size(width); //Actual evaporation
	outputBand2.Size(width); //Latent heat flux
	outputBand3.Size(width); //Net radiation flux density Rn
	outputBand4.Size(width); //Soil heat flux density
	outputBand5.Size(width); //Sensible heat flux at dry limit Hdry 
	outputBand6.Size(width); //Sensible heat flux at wet limit Hwet 
	outputBand7.Size(width); //Sensible heat flux  
	outputBand8.Size(width); //evaporation fraction
	outputBand9.Size(width); //Daily evaporation
	//Temperal testing files
	outputBand10.Size(width); 
	outputBand11.Size(width); 
	outputBand12.Size(width); 
	outputBand13.Size(width); 
	outputBand14.Size(width); 
	outputBand15.Size(width); 
	//outputBand16.Size(width); 

	//Create output maps
	Domain dmValue("value.dom");
	DomainValueRangeStruct dv(-999999999,999999999,0.00001);
	
	FileName fnRn = FileName("sebs_Rn",fnObj);
	fnRn.sExt = ".mpr";
	fnRn = FileName::fnUnique(fnRn);
	Map mpRn = Map(fnRn, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(0,1367,0.001));

	FileName fnG0 = FileName("sebs_G0",fnObj);
	fnG0.sExt = ".mpr";
	fnG0 = FileName::fnUnique(fnG0);
	Map mpG0 = Map(fnG0, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(0,1200,0.001));

	FileName fnH_dry = FileName("sebs_H_dry",fnObj);
	fnH_dry.sExt = ".mpr";
	fnH_dry = FileName::fnUnique(fnH_dry);
	Map mpH_dry = Map(fnH_dry, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(0,2000,0.001));

	FileName fnH_wet = FileName("sebs_H_wet",fnObj);
	fnH_wet.sExt = ".mpr";
	fnH_wet = FileName::fnUnique(fnH_wet);
	Map mpH_wet = Map(fnH_wet, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(-2000,2000,0.001));

	FileName fnH_i = FileName("sebs_H_i",fnObj);
	fnH_i.sExt = ".mpr";
	fnH_i = FileName::fnUnique(fnH_i);
	Map mpH_i = Map(fnH_i, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(-1000,1000,0.001));
	
	FileName fndaily_evap = FileName("sebs_daily_evap",fnObj);
	fndaily_evap.sExt = ".mpr";
	fndaily_evap = FileName::fnUnique(fndaily_evap);
	Map mpdaily_evap = Map(fndaily_evap, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(-1,10,0.001));

	FileName fnact_evap = FileName("sebs_evap_relative",fnObj);
	fnact_evap.sExt = ".mpr";
	fnact_evap = FileName::fnUnique(fnact_evap);
	Map mpact_evap = Map(fnact_evap, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(-1,10,0.001));
	
	FileName fnLE = FileName("sebs_LE",fnObj);
	fnLE.sExt = ".mpr";
	fnLE = FileName::fnUnique(fnLE);
	Map mpLE = Map(fnLE, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(-1200,1200,0.001));

	//Temperal testing files
	FileName fnT0ta = FileName("sebs_T0ta",fnObj);
	fnT0ta.sExt = ".mpr";
	fnT0ta = FileName::fnUnique(fnT0ta);
	Map mpT0ta = Map(fnT0ta, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnC_i = FileName("sebs_C_i",fnObj);
	fnC_i.sExt = ".mpr";
	fnC_i = FileName::fnUnique(fnC_i);
	Map mpC_i = Map(fnC_i, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnZ0h = FileName("sebs_Z0h",fnObj);
	fnZ0h.sExt = ".mpr";
	fnZ0h = FileName::fnUnique(fnZ0h);
	Map mpZ0h = Map(fnZ0h, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnZ0m = FileName("sebs_Z0m",fnObj);
	fnZ0m.sExt = ".mpr";
	fnZ0m = FileName::fnUnique(fnZ0m);
	Map mpZ0m = Map(fnZ0m, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnLAI = FileName("sebs_LAI",fnObj);
	fnLAI.sExt = ".mpr";
	fnLAI = FileName::fnUnique(fnLAI);
	Map mpLAI = Map(fnLAI, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnKB = FileName("sebs_kb",fnObj);
	fnKB.sExt = ".mpr";
	fnKB = FileName::fnUnique(fnKB);
	Map mpKB = Map(fnKB, mp->gr(), mp->gr()->rcSize(), dv);

	/*FileName fnre_i = FileName("sebs_re_i",fnObj);
	fnre_i.sExt = ".mpr";
	fnre_i = FileName::fnUnique(fnre_i);
	Map mpre_i = Map(fnre_i, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnT0ta = FileName("sebs_T0ta",fnObj);
	fnT0ta.sExt = ".mpr";
	fnT0ta = FileName::fnUnique(fnT0ta);
	Map mpT0ta = Map(fnT0ta, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnZ0h = FileName("sebs_Z0h",fnObj);
	fnZ0h.sExt = ".mpr";
	fnZ0h = FileName::fnUnique(fnZ0h);
	Map mpZ0h = Map(fnZ0h, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnrhoa = FileName("sebs_rhoa",fnObj);
	fnrhoa.sExt = ".mpr";
	fnrhoa = FileName::fnUnique(fnrhoa);
	Map mprhoa = Map(fnrhoa, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnrhoacp = FileName("sebs_rhoacp",fnObj);
	fnrhoacp.sExt = ".mpr";
	fnrhoacp = FileName::fnUnique(fnrhoacp);
	Map mprhoacp = Map(fnrhoacp, mp->gr(), mp->gr()->rcSize(), dv);

	FileName fnC_i = FileName("sebs_C_i",fnObj);
	fnC_i.sExt = ".mpr";
	fnC_i = FileName::fnUnique(fnC_i);
	Map mpC_i = Map(fnC_i, mp->gr(), mp->gr()->rcSize(), dv);*/

	// placeholders for the single values of the inner for-loop
	double C_d = 0.2; //Foliage drag coefficient
	double Ct = 0.01; //Heat transfer coefficient

	kb_1 kb = kb_1(m_Z_ref, C_d, Ct);
	EnergyBalance eb = EnergyBalance();
		
	// loop over all scanlines
	trq.SetTitle(sFreezeTitle);
	trq.SetText(TR("Generating SEBS maps..."));
	trq.Start();

	int _julianday;
	if (m_use_daynumber)
		_julianday = m_daynumber; //193;
	else{
		long jdn2 = JulianDayNumber(m_month, m_day, m_year);
		long jdn1 = JulianDayNumber(1, 1, m_year);
		_julianday = jdn2 - jdn1 + 1;
	}
	for (int y = 0; y < height; y++) {
			// read these bands by scanline
			mp->GetLineVal(y, inputLST); 
			m_mpEmis->GetLineVal(y, inputEmsi); 
			m_mpAlbedo->GetLineVal(y, inputAlbedo); 
			m_mpNDVI->GetLineVal(y, inputBandNDVI); 
			if (m_USE_LAI) {
				m_mpLAI->GetLineVal(y, inputBandLai);
			}
			if (m_USE_Pv) {
				m_mpPv->GetLineVal(y, inputBandPv);
			}
			if (m_use_Sza)
			  m_mpSunZenithAngle->GetLineVal(y, inputBandSza);
			if (m_use_Dem)
			  m_mpDEM->GetLineVal(y, inputBandDem);
			if (m_USE_LUM) {
				m_mpLandUse->GetLineVal(y, inputBandLanduse);
			}
			if (m_USE_hc) {
				m_mpHc->GetLineVal(y, inputBandHc);
			}
			if (m_USE_d0) {
				m_mpD0->GetLineVal(y, inputBandD0);
			}
			if(m_useQ_ref_map)
				m_mpQ_ref->GetLineVal(y, inputQ_ref);
			if(m_useU_ref_map)
				m_mpU_ref->GetLineVal(y, inputU_ref);
			if(m_useT_ref_map)
				m_mpT_ref->GetLineVal(y, inputT_ref);
			if(m_useP_ref_map)
				m_mpP_ref->GetLineVal(y, inputP_ref);
			if(m_useP_sur_map)
				m_mpP_sur->GetLineVal(y, inputP_sur);
			if(m_use_SdwnMap)
				m_mpSdwn->GetLineVal(y, inputSdwn);

			if(m_useTa_avg_map)
				m_mpTa_avg->GetLineVal(y, inputTa_avg);
			if(m_useN_map)
				m_mpN->GetLineVal(y, inputN_s);
			if(m_USE_KB && m_fKBmap)
				 m_mpKB->GetLineVal(y, inputKB_map);
			for (int x = 0; x < width; x++) {
				if(m_useQ_ref_map)
					m_Q_ref = inputQ_ref[x]; //0.06; 
				if(m_useU_ref_map)
					m_U_ref = inputU_ref[x]; //2
				if(m_useT_ref_map)
					m_T_ref = inputT_ref[x]; //25
				if(m_useP_ref_map)
					m_P_ref = inputP_ref[x]; //100000
				if(m_useP_sur_map)
					m_P_sur = inputP_sur[x]; //100100; 

				RowCol rc;
				rc.Row = y;
				rc.Col = x;
				Coord cd = mp->gr()->cConv(rc);

				double Fc; //vegetation fractional coverage or Fractional vegetation cover = 0.5
				double LAI;
				double NDVI = inputBandNDVI[x];
				double emissivity = inputEmsi[x];
				double albedoValue = inputAlbedo[x];
				double LST = inputLST[x];

				if (NDVI <= 0)
					NDVI = 0.0001;
				else if (NDVI >= 1)
					NDVI = 1;	
				if (m_USE_LAI) 
					LAI = inputBandLai[x];
				else if (NDVI == rUNDEF)
					LAI = rUNDEF;
				else	//use a simple formular for LAI= f(NDVI), Su (1996)
					LAI = sqrt(NDVI*(1.0+NDVI)/(1.0-NDVI+ 1.e-6));
				
				if(m_USE_Pv)
					Fc = inputBandPv[x];
				else{
					Fc=(1-exp(-0.5*LAI));
				}
								
				double hMin = 0.0012;
				double hMax = 2.5;

				double emis_air; //emissivity of the atmosphere
				double LWd; //downward longwave radiation
				
				if (m_T_ref != rUNDEF){
					emis_air = 9.2 * pow(10.0,-6) * pow((m_T_ref + 273.15),2);
					LWd = emis_air * sigma * pow(m_T_ref+273.15,4); 
				}
				else{
					emis_air = rUNDEF;
					LWd = rUNDEF;
				}
				
				//eccentricity (sun earth distance)
				double _day_angle = 2*m_PI *(_julianday-1)/365;
				double e0 = 1.00011+0.034221*cos(_day_angle)+0.00128*sin(_day_angle);
				e0 = e0+0.000719*cos(2*_day_angle)+0.000077*sin(2*_day_angle);
				double dem;
				if (m_use_Dem)
					dem = inputBandDem[x];
				else
					dem = m_rDEM;
				if (dem < 0) 
				 dem = 0;
				//calculate downward shortwave radiation
				double rSza;
				if (m_use_Sza)
					rSza = inputBandSza[x];
				else
					rSza = m_rSza;
				if(m_use_SdwnMap)
					m_S_dwn = inputSdwn[x];
				else if (!m_USE_Sdwn){
					//Get atmospheric transmissivity
					double BR0 = 0.0116;
					double Ba0 = log(50.0)/m_Visi-BR0;
					double Ba55 = 0.0030765;
					double H1 = 5.5/log(Ba0/Ba55);
					double HBa0 = H1 * Ba0;
					double HBa55 = 0.05 - H1 *Ba55;
					//tau is Aerosol optical thickness
					double tau = 0.42*(HBa0/exp(dem/(1000*H1))+HBa55);
					//tran_c is transmission coefficient  
					double trans = exp((-1/cos(rSza*m_PI/180))*tau);
					//trans = 0.75; //set default value for the entire area
					if (rSza == rUNDEF || dem ==rUNDEF)
						m_S_dwn = rUNDEF;
					else
						m_S_dwn = 1367*e0*cos(rSza*m_PI/180)*trans;
				}
				double z0hM = 0;
				double h = 0;
				double d0 = 0;
				
				if (!m_USE_hc){
					double ndvi_max = 0.8; //h=2.5
					double ndvi_min = 0.0; //h = 0.0012
					//vegetation height
					h = hMin + (hMax-hMin)/(ndvi_max-ndvi_min)*(NDVI-ndvi_min); 
					if (h >= m_Z_ref/2.0 )
						h = m_Z_ref/2.0;
				}	
				else{
					h = inputBandHc[x];
				}
				if (m_USE_LUM)
					z0hM = inputBandLanduse[x];
				if (m_USE_d0)
					d0 = inputBandD0[x];
				if (h<0.0001)
					h = 0.0001;
				if (d0<0.0001)
					d0 = 0.0001;
				bool kb_p;
				if (NDVI < 0.2) //for bare soil
					kb_p =1;
				else
					kb_p = 0;
				double kb_val=2.5;
				if(m_USE_KB){
					if(m_fKBmap)
				      kb_val=inputKB_map[x];
					else
                      kb_val = m_kb;
				}
				kb.calculate(Fc, LAI, z0hM, h,d0,m_USE_LUM,m_USE_hc,m_USE_d0,m_U_ref,m_P_ref,m_T_ref,
							 LST, m_Q_ref, kb_p, m_USE_KB,kb_val);
				h = kb.getHc();
				if (!m_USE_LUM)
					z0hM = kb.getZ0(); //Roughness height for heat transfer (m), 0.02
				d0 = kb.getD();
				double z0h = kb.getZ0h(); //Roughness height for momentum transfer (m), 0.2
				eb.calculate(m_Z_ref, m_hi, d0, z0hM, z0h, Fc, m_U_ref, m_T_ref, m_P_ref, m_Q_ref, 
						LST, m_P_sur, m_S_dwn, LWd, albedoValue, emissivity,m_USE_LAI,LAI);
					
				//calculating daily evaporation
				LatLon ll;
				double lat;
				if (mp->gr()->cs()->fCoord2LatLon()) {
					ll = mp->gr()->cs()->llConv(cd);
					lat = ll.Lat*m_PI/180;
					//double y= cd.Y;
				}
				else{
					lat = cd.y;
					lat = lat*m_PI/180;
				}

				//calculating intermediate results, some formulas are from Campbell & Norman, 1998.
				//Here, we use the average PBL and surface tempeature
				double t_c = log((m_hi-d0)/z0h)/log((m_Z_ref-d0)/z0h);
				double t_s = m_T_ref + 273.15;
				//double t_s = 25 + 273.15;
				double t_pbl_A = (LST)*(1.0-t_c)+t_s*t_c;
				double T0 = (LST)/pow((1.0-dem/44331.0), 1.5029);
				t_pbl_A = t_pbl_A/pow(1.0-dem/44331.0, 1.5029);
				double T_0pbl = 0.5*(T0+t_pbl_A); //mean potaaential temperature
				double Tcn = T_0pbl - 273.15; //mean PBL temperature converted to 0C
								
				//--------------------
				/*double esat = 0.6108*exp(17.27*Tcn/(237.3+Tcn));//vapor pressure
				//double esat = 611.0*exp(17.502*Tcn/(Tcn+240.97));
				double hr_pbl = m_Q_ref; //humidity at reference height
				double eact = hr_pbl*esat; //actual vapour presure
				double em_air = 0.34-0.14*sqrt(0.001*esat); //atmospheric emissivity
				double Tae = sqrt(em_air) * pow(t_pbl_A,2);
				double T0e = sqrt(emissivity) * pow(T0, 2);
				//L24- daily net longwave radiation
				double L24 = sigma * (Tae+T0e)*(Tae-T0e);
				double tau_t = exp(-tau*2.0); //supposing sun zenith is 60 as an average value
				double solar_decl = 0.409*sin(0.0172*_julianday-1.39); //solar declination
				double tdecl = tan(solar_decl);
				double Ws = acos(-tdecl * tan(lat)); //sunset hour angle in radiance
				double D_es = 1.0+0.033*cos(0.0172*_julianday); // earth - sun distance
				//K24 - daily incoming global radiation
				double K24 = 435.2 * tau_t * D_es * (Ws * sin(lat) *
						sin(solar_decl) + cos(lat) * 
						cos(solar_decl) * sin(Ws));
				double qstar = (1.0 - albedoValue) *K24 + L24; //net radiation Rn
				//lamda=lamda*Row
				//lamda-latent heat of vaporazation (j kgm^-1)
				//Row - density of water (Kgm^-1)
				double lambda = 2.501-0.02361*(t_pbl_A-273);
				if (abs(lambda) < 1.0e-25)
					lambda = 1.0e-25;
				double evapfr = eb.getEvap_fr();
				double Edaily = 0.0864 * evapfr * qstar / lambda; */
				//---------------------------------------


				//method by handbook of hydrology C4.8
				//---------------------------------------------------
				double solar_decl = 0.4093*sin((2*m_PI/365)*_julianday-1.405); //solar declination P4.31
				double tdecl = tan(solar_decl);
				//lat positive for the northern, negtive for the southern hemisphere
				double Ws = acos(-tdecl * tan(lat)); //sunset hour angle in radiance
				double D_es = 1.0+0.033*cos((2*m_PI/365)*_julianday); // earth - sun distance
				double N = 24*Ws/m_PI; //total day length 
				// sunshine hours per day
				double n_s;
				if (m_useN_map)
					n_s = inputN_s[x];
				else
					n_s = m_rN;
				double Ta;
				if (m_useTa_avg_map)
					Ta = inputTa_avg[x];
				else
					Ta = m_Ta_avg;
				//double n_s = N*0.9; //real sunshine hours
				double Q = m_Q_ref; //specific humidity at reference height
				//Enet-net emissivity can be estimated using equation below: 
				double Ae=0.34;
				double Be=-0.14;
				double Enet=-0.02+0.261*exp(-7.77*pow(10.0,-4)*pow(m_T_ref,2));
				//actual vapour presure kPa can be estimated based on net emissivity
				double eact = pow((Enet-Ae)/Be,2);//actual vapour presure kPa
				//double eact1 = Q*m_P_sur*0.001/(0.622+0.378*Q); //actual vapour presure kPa
				//the extraterrestrial solar radiation mm /day 
				double S0 = 15.392*D_es*(Ws*sin((lat))*sin(solar_decl)+
							cos((lat))*cos(solar_decl)*sin(Ws));    //mm/day
				S0 = S0*2.47; //in MJ.m^2/day
				//latent heat of vaporation MJkg^^-1  
				//double le = 2.501-0.002361*(t_pbl_A-273); //Ts surface temperature  
				double le = 2.501-0.002361*(LST-273.15);//Ts surface temperature  
				double ro = 4.903*pow(10.0,-9); //Stefan-Boltzmann constant
				double Rn = (0.25+0.5*n_s/N)*(1-albedoValue)*S0-
							(0.9*n_s/N+0.1)*(0.34-0.14*sqrt(eact))*ro*pow((Ta+273.15),4); //in MJ/m^2 /day Tcn
				double Rnl = (0.9*n_s/N+0.1)*(0.34-0.14*sqrt(eact))*ro*pow((Ta+273.15),4);
				double evapfr = eb.getEvap_fr();
				double Edaily = evapfr * Rn / (le); //MJ /m^2 /day 
				//--------------------------------------------------------

				double LE = eb.getRn()-eb.getG0()-eb.getH_i();
				outputBand1[x] = eb.getXevap(); 
				outputBand2[x] = LE; //turbulent latent heat flux
				outputBand3[x] = eb.getRn();    // Net radiation
				outputBand4[x] = eb.getG0();    // Soil heat flux
				outputBand5[x] = eb.getH_dry(); // Sensible heat flux at dry limit
				outputBand6[x] = eb.getH_wet(); // Sensible heat flux at wet limit
				outputBand7[x] = eb.getH_i();   // Sensible heat flux at dry limit
				outputBand8[x] = eb.getEvap_fr(); // Evaporation fraction
				outputBand9[x] = Edaily; //daily evaporation
				//Temperal outputs
				outputBand10[x] = LAI; 
				outputBand11[x] = z0h;
				outputBand12[x] = z0hM; 
				outputBand13[x] = eb.getT_T0ta(); 
				outputBand14[x] = eb.getT_C_i();
				outputBand15[x] = kb.getKB();
				/*outputBand10[x] = eb.getT_re_wet(); 
				outputBand11[x] = eb.getT_re_i(); 
				outputBand12[x] = eb.getT_T0ta(); 
				outputBand13[x] = z0h; 
				outputBand14[x] = d0; //eb.getT_rhoa(); 
				outputBand15[x] = z0hM; //eb.getT_rhoacp(); 
				outputBand16[x] = h; //eb.getT_C_i();*/ 
			}

			// write the results
			ptr.PutLineVal(y, outputBand8);
			mpLE->PutLineVal(y, outputBand2);
			mpRn->PutLineVal(y, outputBand3);
			mpG0->PutLineVal(y, outputBand4);
			mpH_dry->PutLineVal(y, outputBand5);
			mpH_wet->PutLineVal(y, outputBand6);
			mpH_i->PutLineVal(y, outputBand7);
			mpdaily_evap->PutLineVal(y, outputBand9);
			mpact_evap->PutLineVal(y,outputBand1);
			
			mpLAI->PutLineVal(y,outputBand10);
			mpZ0h->PutLineVal(y,outputBand11);
			mpZ0m->PutLineVal(y,outputBand12);
			mpT0ta->PutLineVal(y,outputBand13);
			mpC_i->PutLineVal(y,outputBand14);
			mpKB->PutLineVal(y,outputBand15);	

			// Notify process listeners about processing progress and
			// check whether or not processing shall be terminated
			trq.fUpdate(y, iLines());
		}
		trq.fUpdate(iLines(), iLines());
	return true;
}

bool MapSEBS::fLatLonCoords()
{
	CoordSystemLatLon* csll = mp->cs()->pcsLatLon();
	return (0 != csll);
}


void MapSEBS::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
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

long MapSEBS::JulianDayNumber(int month,int day, int year)
{
	double a  = (14 - month)/12;
	double y =  year + 4800 - a;
    double m = month + 12* a -3;
	long jdn = day +  (153*m+2)/5 + 365*y  +y/4 -y/100 + y/400 - 32045;
	return jdn;
}
