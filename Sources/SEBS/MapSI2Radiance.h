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
// MapSI2Radiance.h: interface for the MapSI2Radiance class.
//
//////////////////////////////////////////////////////////////////////
/* SEBS pre-processing model
   September 2007,by Lichun Wang
*/
#ifndef ILWMAPSI2RADIANCE_H
#define ILWMAPSI2RADIANCE_H

#include "Engine\Applications\MAPFMAP.H"

#define m_PI 3.14159265358979323846

IlwisObjectPtr * createMapSI2Radiance(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapSI2Radiance : public MapFromMap  
{
friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapSI2Radiance* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapSI2Radiance(const FileName& fn, MapPtr& p);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
protected:
	virtual void Store();
	
	MapSI2Radiance(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRawData,
				 double rScale,
				 double rOffset
				 );
    ~MapSI2Radiance();
private:
	Map m_mpRawData;
	double m_rScale; 
	double m_rOffset; 
};

#endif // ILWMAPSI2RADIANCE_H
