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

 Created on: 2007-28-9
 ***************************************************************/
// MapBrightnessTemperature.h: interface for the MapBrightnessTemperature class.
//
//////////////////////////////////////////////////////////////////////
/* SEBS pre-processing model
   September 2007,by Lichun Wang
*/
#ifndef ILWMAPBRIGHTNESSTEMPERATURE_H
#define ILWMAPBRIGHTNESSTEMPERATURE_H

#include "Engine\Applications\MAPFMAP.H"

#define m_PI 3.14159265358979323846

IlwisObjectPtr * createMapBrightnessTemperature(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapBrightnessTemperature : public MapFromMap  
{
friend MapFromMap;
enum Sensor {stModis , stAster, stOther };
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapBrightnessTemperature* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	MapBrightnessTemperature(const FileName& fn, MapPtr& p);
	
	MapBrightnessTemperature(const FileName& fn, 
				 MapPtr& p,
				 String sSensor, 
				 const Map& mpRadiance,
				 double rWavelength
				 );
	MapBrightnessTemperature(const FileName& fn, 
				 MapPtr& p,
				 String sSensor,
				 const Map& mpRadiance1,
				 const Map& mpRadiance2,
				 String sBTmap32
				 );
protected:
	virtual void Store();
    ~MapBrightnessTemperature();
private:
	Map m_mpRadiance1;
	Map m_mpRadiance2;
	String m_sBTmap2;
	double m_rWavelength;
	String m_sSensor;
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
};


#endif // ILWMAPBRIGHTNESSTEMPERATURE_H
