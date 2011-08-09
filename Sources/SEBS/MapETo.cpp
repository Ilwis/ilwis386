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

 Created on: 2009-26-01
 ***************************************************************/
// MapETo.cpp: implements reference ETo defined by FAO 56
//////////////////////////////////////////////////////////////////////
/* ETo model
   Febuary 2009, by Lichun Wang
*/
#include "Engine\Applications\MAPFMAP.H"
#include "SEBS\MapETo.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\hs\map.hs"
#include "Engine\SpatialReference\Coordsys.h"

using namespace std;

IlwisObjectPtr * createMapETo(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapETo::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapETo(fn, (MapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MapETo::MapETo(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
	fNeedFreeze = true;
	sFreezeTitle = TR("Reference ETo");
	//htpFreeze = "ilwisapp\flow_accumulation_algorithm.htm";

	ReadElement("ReferenceETo", "WindSpeed", m_mpU);
	ReadElement("ReferenceETo", "UseVaporPressure", m_useEa);
	if (m_useEa){
		ReadElement("ReferenceETo", "use_Max_Ea_Map", m_useEa_max_map);
		if(m_useEa_max_map){
			ReadElement("ReferenceETo", "Max_Ea_Map", m_mpEa_max);
			CompitableGeorefs(fn, mp, m_mpEa_max);
			m_rEa_max = -1;
		}
		else
			ReadElement("ReferenceETo", "Max_Ea_Val", m_rEa_max);
		ReadElement("ReferenceETo", "use_Min_Ea_Map", m_useEa_min_map);
		if(m_useEa_min_map){
			ReadElement("ReferenceETo", "Min_Ea_Map", m_mpEa_min);
			CompitableGeorefs(fn, mp, m_mpEa_min);
			m_rEa_min = -1;
		}
		else 
			ReadElement("ReferenceETo", "Min_Ea_Val", m_rEa_min);
	}
	else{
		ReadElement("ReferenceETo", "use_RH_Max_Min", m_useRH_max_min);
		if(m_useRH_max_min){
			ReadElement("ReferenceETo", "use_Max_RH_Map", m_useRH_max_map);
			if(m_useRH_max_map){
				ReadElement("ReferenceETo", "Max_RH_Map", m_mpRH_max);
				CompitableGeorefs(fn, mp, m_mpRH_max);
				m_rRH_max = -1;
			}
			else
				ReadElement("ReferenceETo", "Max_RH_Val", m_rRH_max);
			ReadElement("ReferenceETo", "use_Min_RH_Map", m_useRH_min_map);
			if(m_useRH_min_map){
				ReadElement("ReferenceETo", "Min_RH_Map", m_mpRH_min);
				CompitableGeorefs(fn, mp, m_mpRH_min);
				m_rRH_min = -1;
			}
			else
				ReadElement("ReferenceETo", "Min_RH_Val", m_rRH_min);
		}
		else{
			ReadElement("ReferenceETo", "use_Avg_RH_Map", m_useRH_avg_map);
			if(m_useRH_avg_map){
				ReadElement("ReferenceETo", "Avg_RH_Map", m_mpRH_avg);
				CompitableGeorefs(fn, mp, m_mpRH_avg);
				m_rRH_avg = -1;
			}
			else
				ReadElement("ReferenceETo", "Avg_RH_Val", m_rRH_avg);
		}
	}
	ReadElement("ReferenceETo", "use_Max_T_Map", m_useT_max_map);
	if(m_useT_max_map){
		ReadElement("ReferenceETo", "Max_T_Map", m_mpT_max);
		CompitableGeorefs(fn, mp, m_mpT_max);
		m_rT_max = -1;
	}
	else
		ReadElement("ReferenceETo", "Max_T_Val", m_rT_max);
	ReadElement("ReferenceETo", "use_Min_T_Map", m_useT_min_map);
	if(m_useT_min_map){
		ReadElement("ReferenceETo", "Min_T_Map", m_mpT_min);
		CompitableGeorefs(fn, mp, m_mpT_min);
		m_rT_min = -1;
	}
	else
		ReadElement("ReferenceETo", "Min_T_Val", m_rT_min);
	ReadElement("ReferenceETo", "use_Avg_T_Map", m_useT_avg_map);
	if(m_useT_avg_map){
		ReadElement("ReferenceETo", "Avg_T_Map", m_mpT_avg);
		CompitableGeorefs(fn, mp, m_mpT_avg);
		m_rT_avg = -1;
	}
	else
		ReadElement("ReferenceETo", "Avg_T_Val", m_rT_avg);
	ReadElement("ReferenceETo", "use_Rn", m_useRn);
	if(m_useRn){
		ReadElement("ReferenceETo", "use_Rn_Map", m_useRn_map);
		if(m_useRn_map){
			ReadElement("ReferenceETo", "Rn_Map", m_mpRn);
			CompitableGeorefs(fn, mp, m_mpRn);
			m_rRn = -1;
		}
		else
			ReadElement("ReferenceETo", "Rn_Val", m_rRn);
	}
	else{
		ReadElement("ReferenceETo", "use_Tran_Map", m_useTran_map);
		if(m_useTran_map){
			ReadElement("ReferenceETo", "Tran_Map", m_mpTran);
			CompitableGeorefs(fn, mp, m_mpTran);
			m_rTran = -1;
		}
		else
			ReadElement("ReferenceETo", "Tran_Val", m_rTran);
	}
	ReadElement("ReferenceETo", "use_Height_Map", m_useHeight_map);
	if(m_useHeight_map){
		ReadElement("ReferenceETo", "Height_Map", m_mpHeight);
		CompitableGeorefs(fn, mp, m_mpHeight);
		m_rHeight = -1;
	}
	else
		ReadElement("ReferenceETo", "Height_Val", m_rHeight);
	
	ReadElement("ReferenceETo", "DayNumber", m_daynumber);
	ReadElement("ReferenceETo", "Ra_map", m_sRa);
}
MapETo::MapETo(const FileName& fn, MapPtr& p,
				 const Map& mpU,  //average daily wind speed [m/s]
				 bool use_Ea_max_map,String sEa_max,double rEa_max, //daily actual water vapor pressure map 
				 bool use_Ea_min_map,String sEa_min,double rEa_min, 
				 bool use_T_max_map,String sT_max, double rT_max,
				 bool use_T_min_map, String sT_min, double rT_min, //daily air temperature [Celsius]
				 bool use_T_avg_map,String sT_avg,double rT_avg, //daily air temperature [Celsius]
				 bool useRn,
				 bool useRn_map,String sRn, double rRn,
				 bool use_height_map, String sHeight, double rHeight,
				 long daynumber, //Julian Day Number
				 String Ra)
: MapFromMap(fn, p, mpU),
	m_mpU(mpU),
	m_useEa(true),
	m_useEa_max_map(use_Ea_max_map),m_sEa_max(sEa_max),m_rEa_max(rEa_max),
	m_useEa_min_map(use_Ea_min_map),m_sEa_min(sEa_min),m_rEa_min(rEa_min),
	m_useT_max_map(use_T_max_map),m_sT_max(sT_max),	m_rT_max(rT_max),
	m_useT_min_map(use_T_min_map),m_sT_min(sT_min),m_rT_min(rT_min),
	m_useT_avg_map(use_T_avg_map),m_sT_avg(sT_avg),m_rT_avg(rT_avg),
	m_useRn(useRn),
	m_useRn_map(useRn_map),m_sRn(sRn),m_rRn(rRn),
	m_useTran_map(useRn_map),m_sTran(sRn),m_rTran(rRn),
	m_useHeight_map(use_height_map),m_sHeight(sHeight),m_rHeight(rHeight),
	m_daynumber(daynumber),
	m_sRa(Ra)
{
	DomainValueRangeStruct dv(0,2000,0.0001);
	SetDomainValueRangeStruct(dv);
	//Verify compatible geo-reference
	objdep.Add(m_mpU);
	if (use_Ea_max_map){
		m_mpEa_max = Map(sEa_max, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpEa_max);
		objdep.Add(m_mpEa_max);
	}
	if (use_Ea_min_map){
		m_mpEa_min = Map(sEa_min, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpEa_min);
		objdep.Add(m_mpEa_min);
	}
	if (use_T_max_map){
		m_mpT_max = Map(sT_max, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpT_max);
		objdep.Add(m_mpT_max);
	}
	if (use_T_min_map){
		m_mpT_min = Map(sT_min, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpT_min);
		objdep.Add(m_mpT_min);
	}
	if (use_T_avg_map){
		m_mpT_avg = Map(sT_avg, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpT_avg);
		objdep.Add(m_mpT_avg);
	}
	if(useRn){
		if (useRn_map){
			m_mpRn = Map(sRn, fn.sPath());
			CompitableGeorefs(fn, mp, m_mpRn);
			objdep.Add(m_mpRn);
		}
	}
	else{
		if (m_useTran_map){
			m_mpTran = Map(m_sTran, fn.sPath());
			CompitableGeorefs(fn, mp, m_mpTran);
			objdep.Add(m_mpTran);
		}
	}
	if (use_height_map){
			m_mpHeight = Map(sHeight, fn.sPath());
			CompitableGeorefs(fn, mp, m_mpHeight);
			objdep.Add(m_mpHeight);
	}

	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "Reference ETo";
}

MapETo::MapETo(const FileName& fn, MapPtr& p,
				 const Map& mpU,  //average daily wind speed [m/s]
				 bool useRH_max_min,
				 bool use_RH_max_map,String sRH_max,double rRH_max,//maximum relative humidity map[%] 
				 bool use_RH_min_map,String sRH_min,double rRH_min,//relative humidity map [%] 
				 bool use_RH_avg_map,String sRH_avg,double rRH_avg,
				 bool use_T_max_map,String sT_max,double rT_max,
				 bool use_T_min_map,String sT_min,double rT_min,
				 bool use_T_avg_map,String sT_avg,double rT_avg,  //average daily air temperature [Celsius]
                 bool useRn,
				 bool useRn_map,String sRn,double rRn,
				 bool use_height_map, String sHeight, double rHeight,
				 long daynumber, //Julian Day Number
				 String Ra)
: MapFromMap(fn, p, mpU),
	m_mpU(mpU),
	m_useEa(false),m_useRH_max_min(useRH_max_min),
	m_useRH_max_map(use_RH_max_map),m_sRH_max(sRH_max),m_rRH_max(rRH_max),
	m_useRH_min_map(use_RH_min_map),m_sRH_min(sRH_min),m_rRH_min(rRH_min),
	m_useRH_avg_map(use_RH_avg_map),m_sRH_avg(sRH_avg),m_rRH_avg(rRH_avg),
	m_useT_max_map(use_T_max_map),m_sT_max(sT_max),m_rT_max(rT_max),
	m_useT_min_map(use_T_min_map),m_sT_min(sT_min),m_rT_min(rT_min),
	m_useT_avg_map(use_T_avg_map),m_sT_avg(sT_avg),m_rT_avg(rT_avg),
	m_useRn(useRn),
	m_useRn_map(useRn_map),m_sRn(sRn),m_rRn(rRn),
	m_useTran_map(useRn_map),m_sTran(sRn),m_rTran(rRn),
	m_useHeight_map(use_height_map),m_sHeight(sHeight),m_rHeight(rHeight),
	m_daynumber(daynumber),
	m_sRa(Ra)
{

	DomainValueRangeStruct dv(0,10,0.0001);
	SetDomainValueRangeStruct(dv);
	//Verify compatible geo-reference
	objdep.Add(m_mpU);
	if(useRH_max_min){
		if (use_RH_max_map){
			m_mpRH_max = Map(sRH_max, fn.sPath());
			CompitableGeorefs(fn, mp, m_mpRH_max);
			objdep.Add(m_mpRH_max);
		}
		if (use_RH_min_map){
			m_mpRH_min = Map(sRH_min, fn.sPath());
			CompitableGeorefs(fn, mp, m_mpRH_min);
			objdep.Add(m_mpRH_min);
		}
	}
	else{
		if (use_RH_avg_map){
			m_mpRH_avg = Map(sRH_avg, fn.sPath());
			CompitableGeorefs(fn, mp, m_mpRH_avg);
			objdep.Add(m_mpRH_avg);
		}
	}
	if (use_T_max_map){
		m_mpT_max = Map(sT_max, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpT_max);
		objdep.Add(m_mpT_max);
	}
	if (use_T_min_map){
		m_mpT_min = Map(sT_min, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpT_min);
		objdep.Add(m_mpT_min);
	}
	if (use_T_avg_map){
		m_mpT_avg = Map(sT_avg, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpT_avg);
		objdep.Add(m_mpT_avg);
	}
	if(useRn){
		if (useRn_map){
			m_mpRn = Map(sRn, fn.sPath());
			CompitableGeorefs(fn, mp, m_mpRn);
			objdep.Add(m_mpRn);
		}
	}
	else{
		if (m_useTran_map){
			m_mpTran = Map(m_sTran, fn.sPath());
			CompitableGeorefs(fn, mp, m_mpTran);
			objdep.Add(m_mpTran);
		}
	}
	if (use_height_map){
		m_mpHeight = Map(sHeight, fn.sPath());
		CompitableGeorefs(fn, mp, m_mpHeight);
		objdep.Add(m_mpHeight);
	}

	if (!fnObj.fValid())
     objtime = objdep.tmNewest();
	ptr.Store(); 
	fNeedFreeze = true;
	sFreezeTitle = "Reference ETo";
}

MapETo::~MapETo()
{

}

const char* MapETo::sSyntax() {
  
  return "MapETo(MapU2,\n"
			"true|false,Max_Ea_map, Max_Ea_Val, \n"
			"true|false,Min_Ea_map, Min_Ea_Val, \n"
			"true|false,Max_T_map, Max_T_Val, \n"
			"true|false,Min_T_map, Min_T_Val, \n"
			"true|false,Avg_T_map, Avg_T_Val, \n"
			"true|false,\n"
			"true|false,Rn_map|Tran_map, Rn_val|Tran_val, \n"
			"true|false,Height_map, Height_Val, \n"
			"daynumber, \n"
			"Ra_map) or\n"

		  "MapETo(MapU2,\n"
			"true|false,\n"
			"true|false,Max_RH_map, Max_RH_Val, \n"
			"true|false,Min_RH_map, Min_RH_Val, \n"
			"true|false,Mean_RH_map, Mean_RH_Val,\n"
			"true|false,Max_T_map, Max_T_Val, \n"
			"true|false,Min_T_map, Min_T_Val, \n"
			"true|false,Avg_T_map, Avg_T_Val, \n"
			"true|false,\n"
			"true|false,Rn_map|Tran_map, Rn_val|Tran_val,\n"
			"true|false,Height_map, Height_Val,\n"
			"daynumber, \n"
			"Ra_map)\n";
}

MapETo* MapETo::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 25 && iParms != 29 )
      ExpressionError(sExpr, sSyntax());
  Map mpU2(as[0], fn.sPath());
  if (iParms == 25 ){
	  bool use_max_Ea_map = as[1].fVal();
	  String sEa_max = as[2].sVal();
	  double rEa_max = as[3].rVal();	
	  bool use_min_Ea_map = as[4].fVal();
	  String sEa_min = as[5].sVal();
	  double rEa_min = as[6].rVal();	
	  bool use_max_T_map = as[7].fVal();
	  String sT_max = as[8].sVal();
	  double rT_max = as[9].rVal();
	  bool use_min_T_map = as[10].fVal();
	  String sT_min = as[11].sVal();
	  double rT_min = as[12].rVal();
	  bool use_avg_T_map = as[13].fVal();
	  String sT_avg = as[14].sVal();
	  double rT_avg = as[15].rVal();
	  bool use_Rn = as[16].fVal();	
	  bool use_Rn_map = as[17].fVal();
	  String sRn = as[18].sVal();
	  double rRn = as[19].rVal();
	  bool use_height_map = as[20].fVal();
	  String sHeight = as[21].sVal();
	  double rHeight = as[22].rVal();
	  long daynumber = as[23].iVal();
	  String sTran = as[24].sVal();
	  return new MapETo(fn, p, mpU2,
						use_max_Ea_map,sEa_max,rEa_max,
						use_min_Ea_map,sEa_min,rEa_min,
						use_max_T_map,sT_max,rT_max,
						use_min_T_map,sT_min,rT_min,
						use_avg_T_map,sT_avg,rT_avg,
						use_Rn,	
						use_Rn_map,sRn,rRn,
						use_height_map,sHeight,rHeight,
						daynumber,
						sTran);
  }						
  else{
	  bool use_RH_max_min = as[1].fVal();
	  bool use_max_RH_map = as[2].fVal();
	  String sRH_max = as[3].sVal();
	  double rRH_max = as[4].rVal();	
	  bool use_min_RH_map = as[5].fVal();
	  String sRH_min = as[6].sVal();
	  double rRH_min = as[7].rVal();	
	  bool use_avg_RH_map = as[8].fVal();
	  String sRH_avg = as[9].sVal();
	  double rRH_avg = as[10].rVal();	
	  bool use_max_T_map = as[11].fVal();
	  String sT_max = as[12].sVal();
	  double rT_max = as[13].rVal();
	  bool use_min_T_map = as[14].fVal();
	  String sT_min = as[15].sVal();
	  double rT_min = as[16].rVal();
	  bool use_avg_T_map = as[17].fVal();
	  String sT_avg = as[18].sVal();
	  double rT_avg = as[19].rVal();
	  bool use_Rn = as[20].fVal();	
	  bool use_Rn_map = as[21].fVal();
	  String sRn = as[22].sVal();
	  double rRn = as[23].rVal();
	  bool use_height_map = as[24].fVal();
	  String sHeight = as[25].sVal();
	  double rHeight = as[26].rVal();
	  long daynumber = as[27].iVal();
	  String sTran = as[28].sVal();
	  return new MapETo(fn, p, mpU2,
						use_RH_max_min,
						use_max_RH_map,sRH_max,rRH_max,
						use_min_RH_map,sRH_min,rRH_min,
						use_avg_RH_map,sRH_avg,rRH_avg,
						use_max_T_map,sT_max,rT_max,
						use_min_T_map,sT_min,rT_min,
						use_avg_T_map,sT_avg,rT_avg,
						use_Rn,	
						use_Rn_map,sRn,rRn,
						use_height_map,sHeight,rHeight,
						daynumber,
						sTran);
  }
  	
}

void MapETo::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapETo");
  WriteElement("ReferenceETo", "WindSpeed", m_mpU);
	WriteElement("ReferenceETo", "UseVaporPressure", m_useEa);
	if (m_useEa){
		WriteElement("ReferenceETo", "use_Max_Ea_Map", m_useEa_max_map);
		if(m_useEa_max_map)
			WriteElement("ReferenceETo", "Max_Ea_Map", m_mpEa_max);
		else
			WriteElement("ReferenceETo", "Max_Ea_Val", m_rEa_max);
		WriteElement("ReferenceETo", "use_Min_Ea_Map", m_useEa_min_map);
		if(m_useEa_min_map)
			WriteElement("ReferenceETo", "Min_Ea_Map", m_mpEa_min);
		else
			WriteElement("ReferenceETo", "Min_Ea_Val", m_rEa_min);
	}
	else{
		WriteElement("ReferenceETo", "use_RH_Max_Min", m_useRH_max_min);
		if(m_useRH_max_min){
			WriteElement("ReferenceETo", "use_Max_RH_Map", m_useRH_max_map);
			if(m_useRH_max_map)
				WriteElement("ReferenceETo", "Max_RH_Map", m_mpRH_max);
			else
				WriteElement("ReferenceETo", "Max_RH_Val", m_rRH_max);
			WriteElement("ReferenceETo", "use_Min_RH_Map", m_useRH_min_map);
			if(m_useRH_min_map)
				WriteElement("ReferenceETo", "Min_RH_Map", m_mpRH_min);
			else
				WriteElement("ReferenceETo", "Min_RH_Val", m_rRH_min);
		}
		else{
			WriteElement("ReferenceETo", "use_Avg_RH_Map", m_useRH_avg_map);
			if(m_useRH_avg_map)
				WriteElement("ReferenceETo", "Avg_RH_Map", m_mpRH_avg);
			else
				WriteElement("ReferenceETo", "Avg_RH_Val", m_rRH_avg);
		}
	}
	WriteElement("ReferenceETo", "use_Max_T_Map", m_useT_max_map);
	if(m_useT_max_map)
		WriteElement("ReferenceETo", "Max_T_Map", m_mpT_max);
	else
		WriteElement("ReferenceETo", "Max_T_Val", m_rT_max);
	WriteElement("ReferenceETo", "use_Min_T_Map", m_useT_min_map);
	if(m_useT_min_map)
		WriteElement("ReferenceETo", "Min_T_Map", m_mpT_min);
	else
		WriteElement("ReferenceETo", "Min_T_Val", m_rT_min);
	WriteElement("ReferenceETo", "use_Avg_T_Map", m_useT_avg_map);
	if(m_useT_avg_map)
		WriteElement("ReferenceETo", "Avg_T_Map", m_mpT_avg);
	else
		WriteElement("ReferenceETo", "Avg_T_Val", m_rT_avg);
	WriteElement("ReferenceETo", "use_Rn", m_useRn);
	if(m_useRn){
		WriteElement("ReferenceETo", "use_Rn_Map", m_useRn_map);
		if(m_useRn_map)
			WriteElement("ReferenceETo", "Rn_Map", m_mpRn);
		else
			WriteElement("ReferenceETo", "Rn_Val", m_rRn);
	}
	else{
		WriteElement("ReferenceETo", "use_Tran_Map", m_useTran_map);
		if(m_useTran_map)
			WriteElement("ReferenceETo", "Tran_Map", m_mpTran);
		else
			WriteElement("ReferenceETo", "Tran_Val", m_rTran);
	}
	WriteElement("ReferenceETo", "use_Height_Map", m_useHeight_map);
	if(m_useHeight_map){
		WriteElement("ReferenceETo", "Height_Map", m_mpHeight);
	}
	else
		WriteElement("ReferenceETo", "Height_Val", m_rHeight);
	
	WriteElement("ReferenceETo", "DayNumber", m_daynumber);
	WriteElement("ReferenceETo", "Ra_map", m_sRa);
}

String MapETo::sExpression() const
{
  if(m_useEa)
	return String("MapETo(%S,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%li,%S,%g,%li,%S,%g,%li,%S)", 
										m_mpU->sNameQuoted(true),
										m_useEa_max_map,m_sEa_max,m_rEa_max,
										m_useEa_min_map,m_sEa_min,m_rEa_min,
										m_useT_max_map,m_sT_max,m_rT_max,
										m_useT_min_map,m_sT_min,m_rT_min,
										m_useT_avg_map,m_sT_avg,m_rT_avg,
										m_useRn,
										m_useRn_map,m_sRn,m_rRn,
										m_useHeight_map,m_sHeight,m_rHeight,
										m_daynumber,
										m_sRa);
										
  else
	  return String("MapETo(%S,%li,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%S,%g,%li,%li,%S,%g,%li,%S,%g,%li,%S)", 
										m_mpU->sNameQuoted(true),
										m_useRH_max_min,
										m_useRH_max_map,m_sRH_max,m_rRH_max,
										m_useRH_min_map,m_sRH_min,m_rRH_min,
										m_useRH_avg_map,m_sRH_avg,m_rRH_avg,
										m_useT_max_map,m_sT_max,m_rT_max,
										m_useT_min_map,m_sT_min,m_rT_min,
										m_useT_avg_map,m_sT_avg,m_rT_avg,
										m_useRn,
										m_useRn_map,m_sRn,m_rRn,
										m_useHeight_map,m_sHeight,m_rHeight,
										m_daynumber,
										m_sRa);

}

bool MapETo::fDomainChangeable() const
{
  return false;
}

bool MapETo::fGeoRefChangeable() const
{
  return false;
}

bool MapETo::fFreezing()
{
  	int width = iCols();
	int height = iLines();

	RealBuf inputU;
	RealBuf inputEa_Max;
	RealBuf inputEa_Min;
	RealBuf inputT_Max;
	RealBuf inputT_Min;
	RealBuf inputT_Avg;
	RealBuf inputRH_Max;
	RealBuf inputRH_Min;
	RealBuf inputRH_Avg;
	RealBuf inputRn;
	RealBuf inputTran;
	RealBuf inputHeight;
	
	RealBuf outputBand1;  
	RealBuf outputBand2;   
	
	// allocate memory for single scan lines of the inputs
	inputU.Size(width);
	inputEa_Max.Size(width);
	inputEa_Min.Size(width);
	inputT_Max.Size(width);
	inputT_Min.Size(width);
	inputT_Avg.Size(width);
	inputRH_Max.Size(width);
	inputRH_Min.Size(width);
	inputRH_Avg.Size(width);
	inputRn.Size(width);
	inputTran.Size(width);
	inputHeight.Size(width);
	
	// allocate memory for single scan lines of the outputs
	outputBand1.Size(width); //ETo
	outputBand2.Size(width); //Ra
	Map mpRa;
	bool fRaMap=false;
	//Create output maps
	if(m_sRa.length() > 0 && !fCIStrEqual(m_sRa, "nomap"))
	{
		FileName fnRa = FileName(m_sRa,fnObj);
		mpRa = Map(fnRa, mp->gr(), mp->gr()->rcSize(), DomainValueRangeStruct(0,1367,0.001));
		outputBand2.Size(iCols());
		fRaMap = true;
	}
	double pi = 3.1415926;
	// loop over all scanlines
	trq.SetTitle(sFreezeTitle);
	trq.SetText(TR("Generating reference ETo map..."));
	trq.Start();

	for (int y = 0; y < height; y++) {
		// read these bands by scanline
		mp->GetLineVal(y, inputU); 
		if(m_useEa){
			if (m_useEa_max_map) 
				m_mpEa_max->GetLineVal(y, inputEa_Max);
			if (m_useEa_min_map) 
				m_mpEa_min->GetLineVal(y, inputEa_Min);
		}
		else{
			if (m_useRH_max_min){
			  if (m_useRH_max_map) 
				m_mpRH_max->GetLineVal(y, inputRH_Max);
			  if (m_useRH_min_map) 
				m_mpRH_min->GetLineVal(y, inputRH_Min);
			}
			else
				if (m_useRH_avg_map) 
				 m_mpRH_avg->GetLineVal(y, inputRH_Avg);
		}
		if (m_useT_max_map) 
			m_mpT_max->GetLineVal(y, inputT_Max);
		if (m_useT_min_map) 
			m_mpT_min->GetLineVal(y, inputT_Min);
		if (m_useT_avg_map) 
			m_mpT_avg->GetLineVal(y, inputT_Avg);
		if(m_useRn){
			if (m_useRn_map) 
				m_mpRn->GetLineVal(y, inputRn);
		}
		else
			if (m_useTran_map)
			m_mpTran->GetLineVal(y, inputTran);

		if (m_useHeight_map) 
				m_mpHeight->GetLineVal(y, inputHeight);
	
		for (int x = 0; x < width; x++) {
				RowCol rc;
				rc.Row = y;
				rc.Col = x;
				Coord cd = mp->gr()->cConv(rc);
				//calculating daily evaporation
				LatLon ll;
				double lat;
				if (mp->gr()->cs()->fCoord2LatLon()) {
					ll = mp->gr()->cs()->llConv(cd);
					lat = ll.Lat*m_PI/180;
				}
				else{
					lat = cd.y;
					lat = lat*m_PI/180;
				}
				//for testing only 
				//lat = 50.8*m_PI/180; 

				double rEa_max, rEa_min,rEa; //daily actual water vapor pressure [kPa]
				double rRH_max, rRH_min; //relative humidity [%]
				double rT_max, rT_min, rT_avg; //air temperature
				double rRn; //incoming net radiation at crop surface [MJm^-2/day]
				double rTran; //overall daily average shortwave transmissivity 
				double rETo;
				if(m_useT_max_map)
					rT_max=inputT_Max[x];
				else
					rT_max = m_rT_max;
				if(m_useT_min_map)
					rT_min=inputT_Min[x];
				else
					rT_min = m_rT_min;
				if(m_useT_avg_map)
					rT_avg=inputT_Avg[x];
				else
					rT_avg = m_rT_avg;

				double Es_Tmax= 0.6108*exp((17.27*rT_max)/(rT_max+237.3));
				double Es_Tmin= 0.6108*exp((17.27*rT_min)/(rT_min+237.3));
				double Es=0.5*(Es_Tmax+Es_Tmin); //[kPa]

				double Gsc = 0.082; //solar constant [MJ m_^-2 per day]
				double dr = 1+0.033*cos((2*pi/365)*m_daynumber); //inverse of the relative distance earth sun
				double s_de= 0.409*sin((2*pi/365)*m_daynumber-1.39); //solar declination [rad]
				double Ws = acos(-tan(lat) * tan(s_de)); //sunset hour angle in radiance
				double rX = 1-pow(tan(lat),2)*pow(tan(s_de),2);
				if (rX <=0)
					rX=0.00001;
				//daily terrestrial incoming solar radiation [MJ m^-2 /day]
				double rRa = (24*60/pi)*Gsc*dr*(Ws*sin(lat)*sin(s_de)+cos(lat)*cos(s_de)*sin(Ws)); 
				
				if(m_useEa){
					if(m_useEa_max_map)
						rEa_max = inputEa_Max[x];
					else
						rEa_max = m_rEa_max;
					if(m_useEa_min_map)
						rEa_min = inputEa_Min[x];
					else
						rEa_min = m_rEa_min;
					rEa = (rEa_max+rEa_min)/2;
				}	
				else{
					double RHavg;
					if(!m_useRH_max_min){
						if(m_useRH_avg_map)
							RHavg = inputRH_Avg[x];
						else
							RHavg = m_rRH_avg;
					}
					else{
						if(m_useRH_max_map)
							rRH_max = inputRH_Max[x];
						else
							rRH_max = m_rRH_max;
						if(m_useRH_min_map)
							rRH_min = inputRH_Min[x];
						else
							rRH_min = m_rRH_min;
						
					}
					if(!m_useRH_max_min)
						rEa = 0.01*RHavg*((Es_Tmax+Es_Tmin)/2); 
					else	
						rEa = (Es_Tmin*(rRH_max/100)+Es_Tmax*(rRH_min/100))/2; 
				}
				if(m_useRn){
					if(m_useRn_map)
						rRn=inputRn[x];
					else
						rRn = m_rRn; //daily transmissivity of the atmosphere
				}
				else{ //sorrogatory input if Rn is not available
					if(m_useTran_map)
						rTran = inputTran[x];
					else
						rTran = m_rTran;
					/*double Gsc = 0.082; //solar constant [MJ m_^-2 per day]
					double dr = 1+0.033*cos((2*pi/365)*m_daynumber); //inverse of the relative distance earth sun
					double s_de= 0.409*sin((2*pi/365)*m_daynumber-1.39); //solar declination [rad]
					double Ws = acos(-tan(lat) * tan(s_de)); //sunset hour angle in radiance
					double rX = 1-pow(tan(lat),2)*pow(tan(s_de),2);
					if (rX <=0)
						rX=0.00001;
					//Ws = pi/
					//daily terrestrial incoming solar radiation [MJ m^-2 /day]
					rRa = (24*60/pi)*Gsc*dr*(Ws*sin(lat)*sin(s_de)+cos(lat)*cos(s_de)*sin(Ws)); */
					double Rs = rTran*rRa;
					double alf = 0.23; // the albedo for grass (ETo)
					double Rns = (1-alf)*Rs; //short wave radiation
					if(m_useT_max_map)
						rT_max = inputT_Max[x];
					else
						rT_max = m_rT_max;
					if(m_useT_min_map)
						rT_min = inputT_Min[x];
					else
						rT_min = m_rT_min;
					if(m_useT_avg_map)
						rT_avg = inputT_Avg[x];
					else
						rT_avg = m_rT_avg;
					double delta = 4.903*pow(10.0,-9);//MJoule K^-4 m_^-2 per day
					double Rso = (0.75+2*pow(10.0,-5)*rTran)*rRa;
					//daily net long wave radiation [MJ m_^-2 /day]
					double Rnl=-delta*((pow((rT_max+273.16),4)+pow((rT_min+273.15),4))/2)*(0.34-0.14*sqrt(rEa))*(1.35*Rs/Rso-0.35);
					rRn=Rns+Rnl;
				}
				double rHeight;
				if(m_useHeight_map)
					rHeight = inputHeight[x];
				else
					rHeight = m_rHeight;
					
				double gama = 0.665*pow(10.0,-3)*101.3*pow(((293-0.0065*rHeight)/293),5.26);//psychrometric constant [kPa °C-1].

				double sv= 4098*(0.6108*exp(17.27*rT_avg/(rT_avg+ 237.3)))/pow((rT_avg+237.3),2); //slope vapour pressure curve [kPa °C-1],
				double u2=inputU[x];
				rETo=(0.408*sv*rRn+gama*(900/(rT_avg+273))*u2*(Es-rEa))/(sv+gama*(1+0.34*u2));

				//where rETo reference evapotranspiration [mm day-1],
				//Rn net radiation at the crop surface [MJ m-2 day-1],
				//G soil heat flux density [MJ m-2 day-1],
				//rT_avg mean daily air temperature at 2 m height [°C],
				//u2 wind speed at 2 m height [m s-1],
				//es saturation vapour pressure [kPa],
				//ea actual vapour pressure [kPa],
				//es - ea saturation vapour pressure deficit [kPa],
				//sv slope vapour pressure curve [kPa °C-1],
				//gama psychrometric constant [kPa °C-1].

				outputBand1[x] = rETo;
				if(fRaMap)
					outputBand2[x] = rRa; //turbulent latent heat flux
			}

			// write the results
			ptr.PutLineVal(y, outputBand1);
			if(fRaMap)
				mpRa->PutLineVal(y, outputBand2);
			// Notify process listeners about processing progress and
			// check whether or not processing shall be terminated
			trq.fUpdate(y, iLines());
		}
		trq.fUpdate(iLines(), iLines());
	return true;
}

void MapETo::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
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


