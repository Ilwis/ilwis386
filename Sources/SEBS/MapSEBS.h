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
// MapSEBS.h: interface for the MapSEBS class.
//
//////////////////////////////////////////////////////////////////////
/* MapSEBS model
   August 2007,by Lichun Wang
*/
#ifndef ILWMAPSEBSMODEL_H
#define ILWMAPSEBSMODEL_H

#include "Engine\Applications\MAPFMAP.H"

#define m_PI 3.14159265358979323846

IlwisObjectPtr * createMapSEBS(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapSEBS : public MapFromMap  
{
friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapSEBS* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	MapSEBS(const FileName& fn, MapPtr& p,
				 const Map& mpLST,  //Land surface temperature
				 const Map& mpEmis, //Emissivity
                 const Map& mpAlbedo, //Land surface albedo
				 const Map& mpNDVI,
				 bool useLAI,
				 String sLAI,
                 bool usePv,
				 String sPv,
				 bool use_Sza,
				 String sSza,
				 double rSza,
				 bool use_DEM,
				 String sDEM,
				 double rDEM,
				 bool use_daynumber,
				 long daynumber, //Julian Day Number
				 long mm,
				 long dd,
				 long yy,
				 double Z_ref,
				 double hi,
				 bool useQ_ref_map,
				 String sQ_ref,
				 double Q_ref,
				 bool useU_ref_map,	
				 String sU_ref,
				 double U_ref,
				 bool useT_ref_map,			
				 String sT_ref,
				 double T_ref,
				 bool useP_ref_map,				
				 String sP_ref,
				 double P_ref,
				 bool useP_sur_map,				
				 String sP_sur,
				 double P_sur,
				 double Visi,
				 bool calc_Sdwn,
				 double S_dwn,
				 bool use_LUM,
				 String sLUM,
				 bool use_Hc,
				 String sHc,
				 bool use_D0,
				 String sD0,
				 bool use_SdwnMap,
				 String sDwn,
				 bool useTa_avg_map,String sTa_avg,double Ta_avg,
				 bool useN_map,String sN,double rN);

	MapSEBS(const FileName& fn, MapPtr& p);
protected:
	virtual void Store();

	~MapSEBS();
private:
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
	long  JulianDayNumber(int ,int, int);
	Map m_mpLST;
	Map m_mpEmis;
	Map m_mpAlbedo;
	Map m_mpLAI;
	Map m_mpPv;
	Map m_mpNDVI;
	Map m_mpSunZenithAngle;
	Map m_mpDEM;
	Map m_mpLandUse; //surface roughtness 
	Map m_mpHc; //Canopy height   
	Map m_mpD0; //Displacement height
	Map m_mpQ_ref;
	Map m_mpU_ref;
	Map m_mpT_ref;
	Map m_mpP_ref;
	Map m_mpP_sur;
	Map m_mpSdwn;
	String m_sLAI;
	String m_sPv;
	String m_sNDVI;
	String m_sSza;
	String m_sDEM;
	String m_sLUM; //surface roughness  
	String m_sHc;  //Canopy height
	String m_sD0;  //displacement height
	double m_Z_ref; //Reference height (m)
	double m_hi; //Height of the PBL (m)
	bool m_useQ_ref_map;
	String m_sQ_ref;
	double m_Q_ref; //Specific humidity at reference height (kg/kg)
	bool m_useU_ref_map;
	String m_sU_ref;
	double m_U_ref; //Wind speed at reference height (m/s)
	bool m_useT_ref_map;
	String m_sT_ref;
	double m_T_ref; //Air temperature at reference height (C)
	bool m_useP_ref_map;
	String m_sP_ref;
	double m_P_ref; //Pressure at reference height (Pa)
	bool m_useP_sur_map;
	String m_sP_sur;
	double m_P_sur; //Pressure at surface (Pa)
	double m_S_dwn; //Downward solar radiation
	double m_Visi; //Horizontal visibility (km)
	long m_daynumber;
	long m_month;
	long m_day;
	long m_year;
	bool m_use_daynumber;
	bool m_USE_LAI;
	bool m_USE_Pv;
	bool m_USE_NDVI;
	bool m_use_Sza;
	bool m_use_Dem;
	bool m_USE_LUM; //surface roughness 
	bool m_USE_hc; //Canopy height
	bool m_USE_d0; //displacement height
	bool m_USE_Sdwn;
	bool m_use_SdwnMap;
	String m_sDwn;	
	double m_rDEM;
	double m_rSza;
	bool m_useN_map;
	String m_sN;
	double m_rN;
	Map m_mpN;
	bool m_useTa_avg_map;
	String m_sTa_avg;
	double m_Ta_avg;
	Map m_mpTa_avg;
	bool fLatLonCoords();
};

#endif // ILWMAPSEBSMODEL_H

