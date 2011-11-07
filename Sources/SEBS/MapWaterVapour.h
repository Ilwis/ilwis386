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

 Created on: 2011-02 by Lichun Wang
 ***************************************************************/
// MapAlbedo.h: interface for the MapAlbedo class.
//
//////////////////////////////////////////////////////////////////////
/* MapSEBS model
   Feb 2011,by Lichun Wang
*/
#ifndef ILWMAPWATERVAPOUR_H
#define ILWMAPWATERVAPOUR_H

#include "Engine\Applications\MAPFMAP.H"

#define m_PI 3.14159265358979323846

IlwisObjectPtr * createMapWaterVapour(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

#define m_PI 3.14159265358979323846

class MapWaterVapour : public MapFromMap  
{
friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapWaterVapour* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	MapWaterVapour(const FileName& fn, MapPtr& p);
	
	MapWaterVapour(const FileName& fn, 
					MapPtr& p,
					String sSensor,
					const Map& mpBand2,
					const Map& mpBand17,
					const Map& mpBand18,
					const Map& mpBand19
					);
	MapWaterVapour(const FileName& fn, 
					MapPtr& p,
					String sSensor,
					const Map& mpBand1,  //12um
					const Map& mpBand2	 //11um
					);
    ~MapWaterVapour();
protected:
	virtual void Store();

private:
	String m_sSensor;
	Map m_mpBand1;  //12 um brightness temperature
	Map m_mpBand2;  //11 um brightness temperature
	Map m_mpBand17;	  //band 17 of Modis center at 0.905um, bandwidth 0.03um
	Map m_mpBand18;	  //band 18 of Modis center at 0.936um, bandwidth 0.01um
	Map m_mpBand19;	  //band 19 of Modis center at 0.94um, bandwidth 0.05um
	
	void ReadBrightnessTemperatureMaps();
	RealBuf  inputBand1; //Brightness temperature 12
	RealBuf  inputBand2; //Brightness temperature 11
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
};

#endif // ILWMapWaterVapour_H
