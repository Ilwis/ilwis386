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

 Created on: 2007-11 by Lichun Wang
 ***************************************************************/
// MapRadiance2Refleflectance.h: interface for the MapRadiance2Refleflectance class.
//
//////////////////////////////////////////////////////////////////////
/* MapSEBS model    
   Convert (ASTER) radiance values to reflectance, by Lichun Wang
   November 2008,by Lichun Wang
*/
#ifndef ILWMapRadiance2Reflectance_H
#define ILWMapRadiance2Reflectance_H

IlwisObjectPtr * createMapRadiance2Reflectance(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapRadiance2Reflectance: public MapFromMap  
{
friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapRadiance2Reflectance* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	MapRadiance2Reflectance(const FileName& fn, MapPtr& p);
	
	MapRadiance2Reflectance(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRad,
				 short daynumber,
				 double sza,
				 double esun);
				 
	//Aster
	MapRadiance2Reflectance(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRad,
				 short daynumber,
				 double sza,
				 String sensor,
				 short band);
protected:
	virtual void Store();
	~MapRadiance2Reflectance();
private:
	String m_Sensor;
	Map m_mpRad;
	short m_daynumber;
	double m_sza;
	double m_esun;
	short m_band;
};

#endif 