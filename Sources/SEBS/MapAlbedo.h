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
// MapAlbedo.h: interface for the MapAlbedo class.
//
//////////////////////////////////////////////////////////////////////
/* MapSEBS model
   November 2007,by Lichun Wang
*/
#ifndef ILWMAPALBEDO_H
#define ILWMAPALBEDO_H

#include "Engine\Applications\MAPFMAP.H"

#define m_PI 3.14159265358979323846

IlwisObjectPtr * createMapAlbedo(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

#define m_PI 3.14159265358979323846

class MapAlbedo: public MapFromMap  
{
friend MapFromMap;
	enum Method {mUsing2Bands, mModis, mAatsr,mAster};
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapAlbedo* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	MapAlbedo(const FileName& fn, MapPtr& p);
	
	MapAlbedo(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRed,
				 const Map& mpNIR,
				 Method md
				 );
	//Modis or Aster
	MapAlbedo(const FileName& fn, 
				 MapPtr& p,
				 const Map& mpRed,
				 const Map& mpNIR,
				 Method md,
				 const Map& mpBand3,
				 const Map& mpBand4,
				 const Map& mpBand5,
				 const Map& mpBand7
				);
protected:
	virtual void Store();
	~MapAlbedo();
private:
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
	Method m_mMethod;
	Map m_mpRedBand;
	Map m_mpNIR;
	Map m_mpBand3;
	Map m_mpBand4;
	Map m_mpBand5;
	Map m_mpBand7;
};



#endif // ILWMAPALBEDO_H
