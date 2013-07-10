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
/* FormMapSebs  
   Sep 2007, by Lichun Wang
*/
#pragma once

LRESULT Cmdmapeto(CWnd *wnd, const String& s);
LRESULT Cmdmapsebs(CWnd *wnd, const String& s);
LRESULT Cmdrawdata2radiance(CWnd *wnd, const String& s);
LRESULT Cmdbrightnesstemperature(CWnd *wnd, const String& s);
LRESULT Cmdemissivity(CWnd *wnd, const String& s);
LRESULT Cmdlandsurfacetemperature(CWnd *wnd, const String& s);
LRESULT Cmdalbedo(CWnd *wnd, const String& s);
LRESULT Cmdsmac(CWnd *wnd, const String& s);
LRESULT Cmdwatervapour(CWnd *wnd, const String& s);
LRESULT Cmdmapradiance2reflectance(CWnd *wnd, const String& s);
LRESULT Cmdsoilmoisture(CWnd *wnd, const String& s);
LRESULT Cmdsoilmoisture_ascat_l2(CWnd *wnd, const String& s);

class FormMapSebs : public FormMapCreate  
{
public:
  _export FormMapSebs(CWnd* mw, const char* sPar);
private:  
  int exec();
  
  int m_iMethod;
  String m_sMapLST;
  String m_sMapEmis;
  String m_sMapAlbedo;
  String m_sMapLai;
  String m_sMapPv;
  String m_sMapNDVI;
  String m_sMapSza;
  String m_sMapDem;
  String m_sLUM;
  String m_sHc;
  String m_sD0;
  String m_sQ_ref;
  String m_sU_ref;
  String m_sT_ref;
  String m_sP_ref;
  String m_sP_sur;
  bool m_fQ_ref;
  bool m_fU_ref;
  bool m_fT_ref;
  bool m_fP_ref;
  bool m_fP_sur;
  CheckBox *m_chkQ_ref;
  CheckBox *m_chkU_ref;
  CheckBox *m_chkT_ref;
  CheckBox *m_chkP_ref;
  CheckBox *m_chkP_sur;
  FieldReal* m_fldQ_ref;
  FieldReal* m_fldU_ref;
  FieldReal* m_fldT_ref;
  FieldReal* m_fldP_ref;
  FieldReal* m_fldP_sur;
  	
  double m_Z_ref;
  double m_hi;
  double m_Q_ref;
  double m_U_ref;
  double m_T_ref;
  double m_P_ref;
  double m_P_sur;
  bool m_fS_dwn;
  double m_S_dwn;
  CheckBox *m_chkSdwn;	
  bool m_fSdwnMap;
  String m_sSdwnMap;
  CheckBox *m_chkSdwnMap;
  FieldDataType *m_fldSdwn;
  int CalcSdwnOnSelect(Event *);
  int SdwnMapOnSelect(Event *);
  double m_Visi;
  FieldReal* m_flVisi;
  bool m_fLUM; //surface roughness
  bool m_fHc;//Vegetation height
  bool m_fD0; //displacement height
  bool m_fLai;
  bool m_fPv;
  bool m_fSza;
  double m_rSza;
  CheckBox *m_chkSza;
  int MapOnSelect(Event *);
  FieldReal* m_fldSza;
  bool m_fDem;
  double m_rDem;
  CheckBox *m_chkDem;
  int DemMapOnSelect(Event *);
  FieldReal* m_fldDem;

  bool m_fDaynumber;
  CheckBox *m_chkDaynumber;
  int m_iDaynumber;
  FieldInt* m_fldMonth;	
  FieldInt* m_fldDay;	
  FieldInt* m_fldYear;
  
  int m_iMonth;	
  int m_iDay;	
  int m_iYear;

  bool m_fTa_avg_map;
  CheckBox *m_chkTa_avg_map;
  FieldReal* m_fldTa_avg;
  String m_sTa_avg;
  double m_Ta_avg;

  bool m_fN_s_map;
  CheckBox *m_chkN_s_map;
  FieldReal* m_fldN_s;
  String m_sN_s;
  double m_N_s;

  bool m_fKB;
  CheckBox *m_chkKB;
  FieldReal* m_fldKB;
  double m_kb;
  bool m_fKB_s_map;
  CheckBox *m_chkKB_s_map; 
  FieldReal* m_fldKB_s;
  FieldDataType *m_fldKB_s_map;  
  String m_sKB_s_map;
  int kbCalOnSelect(Event *);
  int kbMapOnSelect(Event *);

  int DaynumberOnSelect(Event *);
};

class FormMapETo : public FormMapCreate  
{
public:
  _export FormMapETo(CWnd* mw, const char* sPar);
private:  
  int exec();
    int m_iEa_RH;
	int m_iRn_E;
  	bool m_useEa;
	bool m_useEa_max_map;
	bool m_useEa_min_map;
	bool m_useRH_max_min;
	bool m_useRH_max_map;
	bool m_useRH_min_map;
	bool m_useRH_avg_map;
	bool m_useT_max_map;
	bool m_useT_min_map;
	bool m_useT_avg_map;
	bool m_useRn;
	bool m_useRn_map;
	bool m_useE_map;
	bool m_useHeight_map;
	bool m_fRaMap;
	String m_sEa_max;
	String m_sEa_min;
	String m_sRH_max;
	String m_sRH_min;
	String m_sRH_avg;
	String m_sT_max; 
	String m_sT_min;
	String m_sT_avg;
	String m_sRn;
	String m_sE;
	String m_sHeight;
	String m_sU2;
	double m_rEa_max;
	double m_rEa_min;
	double m_rRH_max;
	double m_rRH_min;
	double m_rRH_avg;
	double m_rT_max; 
	double m_rT_min;
	double m_rT_avg;
	double m_rRn;
	double m_rE;
	double m_rHeight;
	String m_sRa;
	long m_daynumber;

	//RadioGroup* rg_Ea;
	

	CheckBox *m_chkEa;
	CheckBox *m_chkEa_max_map;
	CheckBox *m_chkEa_min_map;
	CheckBox *m_chkT_avg;
	CheckBox *m_chkRH_max_min;
	CheckBox *m_chkRH_max_map;
	CheckBox *m_chkRH_min_map;
	CheckBox *m_chkRH_avg_map;
	CheckBox *m_chkT_max_map;
	CheckBox *m_chkT_min_map;
	CheckBox *m_chkT_avg_map;
	CheckBox *m_chkRn;
	CheckBox *m_chkRn_map;
	CheckBox *m_chkE_map;
	CheckBox *m_chkDem_map;

	FieldDataType *m_fdtEa_max_map;
	FieldDataType *m_fdtEa_min_map;
	FieldDataType *m_fdtRH_max_map;
	FieldDataType *m_fdtRH_min_map;
	FieldDataType *m_fdtRH_avg_map;
	FieldDataType *m_fdtT_max_map;
	FieldDataType *m_fdtT_min_map;
	FieldDataType *m_fdtT_avg_map;
	FieldDataType *m_fdtRn_map;
	FieldDataType *m_fdtE_map;
	FieldDataType *m_fdtDEM_map;

	FieldReal* m_fldEa_max;
	FieldReal* m_fldEa_min;
	FieldReal* m_fldT_max;
	FieldReal* m_fldT_min;
	FieldReal* m_fldT_avg;
	FieldReal* m_fldRH_max;
	FieldReal* m_fldRH_min;
	FieldReal* m_fldRH_avg;
	FieldReal* m_fldRn;
	FieldReal* m_fldE;
	FieldReal* m_fldDem;
	RadioGroup* rg_Ea_RH;
	RadioGroup* rg_Rn_E;
	int MapEaRHOnSelect(Event *);
	int MapEaMaxOnSelect(Event *);
	int MapEaMinOnSelect(Event *);
	int MapRHMaxMinOnSelect(Event *);
	int MapRHMaxOnSelect(Event *);
	int MapRHMinOnSelect(Event *);
	int MapRHAvgOnSelect(Event *);
	int MapTAvgOnSelect(Event *);
	int MapTMaxOnSelect(Event *);
	int MapTMinOnSelect(Event *);
	int MapRnEOnSelect(Event *);
	int MapRnOnSelect(Event *);
	int MapEOnSelect(Event *);
	int MapDemOnSelect(Event *);
/*int CalcSdwnOnSelect(Event *);
	int SdwnMapOnSelect(Event *);
	int MapOnSelect(Event *);
	int DemMapOnSelect(Event *);*/
};

class FormMapSmac : public FormMapCreate  
{
public:
  _export FormMapSmac(CWnd* mw, const char* sPar);
private:  
  int exec();

  String m_sMapRef;
  String m_sMapOpticalDepth;
  String m_sMapWaterVapor;
  String m_sMapOzoneContent;
  String m_sMapSurfacePressure;
  String m_sMapSolarZenithAngle;
  String m_sMapSolarAzimutAngle;
  String m_sMapViewZenithAngle;
  String m_sMapViewAzimutAngle;
  String m_coef_file;
  String  m_sMapCloudMask;
  bool m_fMapOpticalDepth;
  bool m_fMapWaterVapor;
  bool m_fMapOzoneContent;
  bool m_fMapSurfacePressure;
  bool m_fMapSolarZenithAngle;
  bool m_fMapSolarAzimutAngle;
  bool m_fMapViewZenithAngle;
  bool m_fMapViewAzimutAngle;
  bool m_fMapCloudMask;
  CheckBox *m_chkMapOpticalDepth;
  CheckBox *m_chkMapWaterVapor;
  CheckBox *m_chkMapOzoneContent;
  CheckBox *m_chkMapSurfacePressure;
  CheckBox *m_chkMapSolarZenithAngle;
  CheckBox *m_chkMapSolarAzimutAngle;
  CheckBox *m_chkMapViewZenithAngle;
  CheckBox *m_chkMapViewAzimutAngle;

  FieldReal* m_fldOpticalDepth;
  FieldReal* m_fldWaterVapor;
  FieldReal* m_fldOzoneContent;
  FieldReal* m_fldSurfacePressure;
  FieldReal* m_fldSolarZenithAngle;
  FieldReal* m_fldSolarAzimutAngle;
  FieldReal* m_fldViewZenithAngle;
  FieldReal* m_fldViewAzimutAngle;

  double m_r_taup550;
  double m_r_uh2o;
  double m_r_uo3;
  double m_r_pression;
  double m_r_tetas;
  double m_r_phis;
  double m_r_tetav;
  double m_r_phiv;
  int MapOnSelect(Event *);
};

class FormMapSI2Radiance : public FormMapCreate  
{
public:
  _export FormMapSI2Radiance(CWnd* mw, const char* sPar);
private:  
  int exec();
	
  String m_sMapRawdata;
  double m_rScale;
  double m_rOffset;
};

class FormMapRadiance2Reflectance : public FormMapCreate  
{
public:
  _export FormMapRadiance2Reflectance(CWnd* mw, const char* sPar);
private:  
  int exec();
  FieldReal* m_fldSolarZenithAngle;
  FieldReal* m_fldESUN;
  bool m_fSensor;
  CheckBox *m_chkSensor;
  FieldInt* fldBandnumber;
  String m_sMapRad;
  int m_iDaynumber;
  double m_rSza;
  double m_rEsun;
  String m_sSensor;
  int m_iBandnumber;
  bool m_AddSensorString;
  int SensorOnChange(Event *);
  FieldOneSelectTextOnly *m_fldSensor;
  int SensorOnSelect(Event *);
};

class FormMapBrightnessTemperature : public FormMapCreate  
{
public:
  _export FormMapBrightnessTemperature(CWnd* mw, const char* sPar);
private:  
  int exec();

  FieldOneSelectTextOnly *m_fldSensor;
  int SensorOnChange(Event *);
  bool m_AddString;
  String m_sSensor;
  FieldMap *m_fldBand1;
  FieldMap *m_fldBand2;
  String m_sMapRadiance31;
  String m_sMapRadiance32;
  FieldMap *m_fldBand;
  FieldReal *m_fldWavelength;
  FieldMapCreate *m_fmcBT32;
  String m_sBTMap32;
  String m_sMapRadiance;
  double m_rWavelength;

  FieldMap *m_fldBand13;
  FieldMap *m_fldBand14;
  FieldMapCreate *m_fmcBT14;
  String m_sMapRadiance13;
  String m_sMapRadiance14;
  String m_sBTMap14;
};

class FormMapEmissivity : public FormMapCreate  
{
public:
  _export FormMapEmissivity(CWnd* mw, const char* sPar);
private:  
  int exec();
	
  String m_sMapRed;
  String m_sMapNIR;
  String m_sMapAlbedo;
  String m_sMapNdvi;
  String m_sMapEmisDif;
  String m_sMapPv;
  bool m_fNdviMap;
  bool m_fEmisDifMap;
  bool m_fPvMap;
  bool m_fAlbedoMap;
  String m_sSensor;
  bool m_AddSensorString;
  FieldOneSelectTextOnly *m_fldSensor;
  int SensorOnChange(Event *);
};

class FormMapLandSurfaceTemperature : public FormMapCreate  
{
public:
  _export FormMapLandSurfaceTemperature(CWnd* mw, const char* sPar);
private:  
  int exec();
	
  String m_sMapBtemp12;
  String m_sMapBtemp11;
  String m_sMapEmissivity;
  String m_sMapEmisDif;
  String m_sMapInWaterVapor;
  String m_sMapOutWaterVapor;
  bool m_fInWaterVapor;
  bool m_fOutWaterVapor;
  FieldOneSelectTextOnly *m_fldSensor;
  int SensorOnChange(Event *);
  bool m_AddSensorString;
  String m_sSensor;
  StaticText *m_stAATSRBand1;
  StaticText *m_stAATSRBand2;
  StaticText *m_stModisBand31;
  StaticText *m_stModisBand32;
  StaticText *m_stAsterBand13;
  StaticText *m_stAsterBand14;
  
};
class FormMapWaterVapour : public FormMapCreate  
{
public:
  _export FormMapWaterVapour(CWnd* mw, const char* sPar);
private:  
  int exec();
	
  String m_sMapBand1;
  String m_sMapBand2;
  String m_sMapBand17;
  String m_sMapBand18;
  String m_sMapBand19;
  FieldOneSelectTextOnly *m_fldSensor;
  int SensorOnChange(Event *);
  bool m_AddSensorString;
  String m_sSensor;
  StaticText *m_stAATSRBand2;
  StaticText *m_stBand2;
  
  FieldMap *m_fldBand1;
  FieldMap *m_fldBand2;
  FieldMap *m_fldBand17;
  FieldMap *m_fldBand18;
  FieldMap *m_fldBand19;
};

class FormMapAlbedo : public FormMapCreate  
{
public:
  _export FormMapAlbedo(CWnd* mw, const char* sPar);
private:  
  int exec();
	
  String m_sMapBand1;
  String m_sMapBand2;
  String m_sBand3;
  String m_sBand4;
  String m_sBand5;
  String m_sBand7;
  String m_sMethod;
  int m_iMethod;
  FieldOneSelectTextOnly *m_fldMethod;
  int MethodOnChange(Event *);
  bool m_AddString;
  FieldMap *m_fldBand3;
  FieldMap *m_fldBand4;
  FieldMap *m_fldBand5;
  FieldMap *m_fldBand7;

  StaticText *m_stBand1;
  StaticText *m_stBand2;
  StaticText *m_stBand3;
  StaticText *m_stBand4;
  StaticText *m_stBand5;
  StaticText *m_stBand7;

  StaticText *m_stBandAster3;
  StaticText *m_stBandAster5;
  StaticText *m_stBandAster6;
  StaticText *m_stBandAster8;
  StaticText *m_stBandAster9;
  
  StaticText *m_stVis;
  StaticText *m_stNIR; 
};

class FormMapSoilMoisture : public FormMapCreate  
{
public:
  _export FormMapSoilMoisture(CWnd* mw, const char* sPar);
private:  
  int exec();
	
  String m_sMapSand;
  String m_sMapClay;
  String m_sMapFc;
  String m_sMapLc;
  String m_sAscatDir;
};

class FormMapSoilMoisture_l2 : public FormMapCreate  
{
public:
  _export FormMapSoilMoisture_l2(CWnd* mw, const char* sPar);
private:  
  int exec();

  String m_sMapLc;
  String m_sAscatDir;
};

