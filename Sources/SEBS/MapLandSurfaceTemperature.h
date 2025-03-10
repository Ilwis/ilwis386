/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
// MapLandSurfacetemperature.h: interface for the MapLandSurfacetemperature class.
//
//////////////////////////////////////////////////////////////////////
/* SEBS pre-processing model
   November 2007,by Lichun Wang
*/
#ifndef ILWMapLandSurfacetemperature_H
#define ILWMapLandSurfacetemperature_H

#include "Engine\Applications\MAPFMAP.H"

#define m_PI 3.14159265358979323846

IlwisObjectPtr * createMapLandSurfaceTemperature(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapLandSurfaceTemperature : public MapFromMap  
{
friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapLandSurfaceTemperature* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	virtual void Store();
	MapLandSurfaceTemperature(const FileName& fn, MapPtr& p);
	
	MapLandSurfaceTemperature(const FileName& fn, 
							MapPtr& p,
							String sSensor,
							const Map& mpBtemp12,
							const Map& mpTtemp11,
							const Map& mpEmis,
							const Map& mpEmisDif,
							String sInWaterVapor);
protected:							
    ~MapLandSurfaceTemperature();
private:
	String m_sSensor;
	Map m_mpBtemp12;  //12 um brightness temperature
	Map m_mpBtemp11;  //11 um brightness temperature
	Map m_mpEmissivity;	  //emissivity map 	
	Map m_mpEmissivityDif;  //emissivity difference map 
	String m_sInWaterVapor; //input file for water vapor content 
	void ReadBrightnessTemperatureMaps();
	RealBuf  inputBand1; //Brightness temperature at wavelength 12um
	RealBuf  inputBand2; //Brightness temperature 11 at wavelength 11um
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
};



#endif // ILWMapLandSurfacetemperature_H
