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

 ***************************************************************/
/* MapSMAC
   January 2008,by Lichun Wang
*/
#ifndef ILWMAPSMAC_H
#define ILWMAPSMAC_H

#include "Engine\Applications\MAPFMAP.H"

IlwisObjectPtr * createMapSMAC(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapSMAC : public MapFromMap  
{
friend MapFromMap;

public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapSMAC* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	// A structure used to store atomoshperic coefficients
	MapSMAC(const FileName& fn, MapPtr& p);
	
	MapSMAC(const FileName& fn, 
			MapPtr& p,
			const Map& ref_mp,
			String coef_file,
			bool fOpticalDepthMap, String OpticalDepth,
			bool fWaterVaporMap, String WaterVapor,
			bool fOzoneContentMap, String OzoneContent,
			bool fSurfacePressureMap, String SurfacePressure,
			bool fSolarZenithAngleMap, String SolarZenithAngle,
			bool fSolarAzimutAngleMap, String SolarAzimutAngle,
			bool fViewZenithAngleMap, String ViewZenithAngle,
			bool fViewAzimutAngleMap, String ViewAzimutAngle);
	
	MapSMAC(const FileName& fn, 
			MapPtr& p,
			const Map& ref_mp,
			String coef_file,
			bool fOpticalDepthMap, String OpticalDepth,
			bool fWaterVaporMap, String WaterVapor,
			bool fOzoneContentMap, String OzoneContent,
			bool fSurfacePressureMap, String SurfacePressure,
			bool fSolarZenithAngleMap, String SolarZenithAngle,
			bool fSolarAzimutAngleMap, String SolarAzimutAngle,
			bool fViewZenithAngleMap, String ViewZenithAngle,
			bool fViewAzimutAngleMap, String ViewAzimutAngle,
			bool fCloudMaskMap, String CloudMask);	
				 
	
protected:
	virtual void Store();
	
    ~MapSMAC();
public:
	struct coef_atoms{
		double ah2o, nh2o;
		double ao3,  no3; 
		double ao2,  no2,  po2;
		double aco2, nco2, pco2;
		double ach4, nch4, pch4;
		double ano2, nno2, pno2;
		double aco,  nco,  pco;
		double a0u, a1u, a2u ;
		double a0s, a1s, a2s, a3s;
		double a0T, a1T, a2T, a3T;
		double taur,sr;
		double a0taup, a1taup ;
		double wo, gc;
		double a0P,a1P,a2P,a3P;
		double a4P,a5P;
		double Resa1,Resa2;
		double Resa3,Resa4;
		double Resr1, Resr2, Resr3;
		double Rest1,Rest2;
		double Rest3,Rest4;
	};
	coef_atoms ca;
private:
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
	void getCoefs(File& f, double& c1, double& c2, double& c3, double& c4);
	double applySMAC(double tetas, double tetav, double phis, double phiv, double uh2o, double uo3, double taup550, double pression, double r_toa);
	String m_coef_filename;
	
	Map mp_r_toa;
	Map mp_uh2o;
	Map mp_uo3;
	Map mp_taup550;
	Map mp_pression;
	Map mp_tetas;
	Map mp_phis;
	Map mp_tetav;
	Map mp_phiv;
	Map mp_cloudmask;

	String s_taup550;
	String s_uh2o;
	String s_uo3;
	String s_pression;
	String s_tetas;
	String s_phis;
	String s_tetav;
	String s_phiv;
	String s_cloudmask;

	bool is_mp_uh2o;
	bool is_mp_uo3;
	bool is_mp_taup550;
	bool is_mp_pression;
	bool is_mp_tetas;
	bool is_mp_phis;
	bool is_mp_tetav;
	bool is_mp_phiv;
	bool is_mp_cloudmask;
};


#endif // ILWMAPSMAC_H
