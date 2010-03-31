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
// MapETo.h: interface for the MapETo class.
//
//////////////////////////////////////////////////////////////////////
/* MapETo      
   March 2009,by Lichun Wang
*/
#ifndef ILWMAPETo_H
#define ILWMAPETo_H

#define m_PI 3.14159265358979323846

IlwisObjectPtr * createMapETo(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapETo : public MapFromMap  
{
friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapETo* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	MapETo(const FileName& fn, MapPtr& p);
	MapETo(const FileName& fn, MapPtr& p,
				 const Map& mpU,  //average daily wind speed [m/s]
				 bool use_Ea_max_map,
				 String sEa_max, //maximum daily actual water vapor pressure map
				 double rEa_max, 
				 bool use_Ea_min_map,
				 String sEa_min, //minimum daily actual water vapor pressure map
				 double rEa_min, 
				 bool use_T_max_map, //maximum daily air temperature [Celsius]
				 String sT_max,
				 double rT_max,
				 bool use_T_min_map, //minimum daily air temperature [Celsius]
				 String sT_min,
				 double rT_min,
				 bool use_T_avg_map, //average daily air temperature [Celsius]
				 String sT_avg,
				 double rT_avg,
                 bool useRn,
				 bool useRn_map,
				 String sRn_map,
				 double rRn_val,
				 bool useHeight_map, String sHeight, double rHeight,
				 long daynumber, //Julian Day Number
				 String Ra
				 );	// 
	MapETo(const FileName& fn, MapPtr& p,
				 const Map& mpU,  //average daily wind speed [m/s]
				 bool useRH_max_min,
				 bool use_RH_max_map,
				 String sRH_max, //maximum relative humidity map[%]
				 double rRH_max, 
				 bool use_RH_min_map,
				 String sRH_min, //relative humidity map [%]
				 double rRH_min, 
				 bool use_RH_avg_map,
				 String sRH_avg, //average daily relative humidity map [%]
				 double rRH_avg,
				 bool use_T_max_map, //maximum daily air temperature [Celsius]
				 String sT_max,
				 double rT_max,
				 bool use_T_min_map, //minimum daily air temperature [Celsius]
				 String sT_min,
				 double rT_min,
				 bool use_T_avg_map, //average daily air temperature [Celsius]
				 String sT_avg,
				 double rT_avg,
                 bool useRn,
				 bool useRn_map,
				 String sRn_map,
				 double rRn_val,
				 bool useHeight_map, String sHeight, double rHeight,
				 long daynumber, //Julian Day Number
				 String Ra);	// terrestrial incoming daily solar radiation
protected:
	virtual void Store();				 
	~MapETo();
private:
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
	Map m_mpU;
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
	bool m_useTran_map;
	bool m_useHeight_map;
	Map m_mpEa_max;
	Map m_mpEa_min;
	Map m_mpRH_max;
	Map m_mpRH_min;
	Map m_mpRH_avg;
	Map m_mpT_max; 
	Map m_mpT_min;
	Map m_mpT_avg;
	Map m_mpRn;
	Map m_mpTran;
	Map m_mpRa;
	Map m_mpHeight;
	String m_sEa_max;
	String m_sEa_min;
	String m_sRH_max;
	String m_sRH_min;
	String m_sRH_avg;
	String m_sT_max; 
	String m_sT_min;
	String m_sT_avg;
	String m_sRn;
	String m_sTran;
	String m_sHeight;
	double m_rEa_max;
	double m_rEa_min;
	double m_rRH_max;
	double m_rRH_min;
	double m_rRH_avg;
	double m_rT_max; 
	double m_rT_min;
	double m_rT_avg;
	double m_rRn;
	double m_rTran;
	double m_rHeight;
	String m_sRa;
	long m_daynumber;
};

#endif // ILWMAPETo_H
