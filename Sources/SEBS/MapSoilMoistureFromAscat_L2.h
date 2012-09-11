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
   January 2009,by Lichun Wang
*/
#ifndef ILWMAPSMASCAC_L2_H
#define ILWMAPSMASCAC_L2_H

//#include "RasterApplication\mapfmap.h"
//#include "afx.h"
//#include "BasicDataStructures\MapList.h"
//#include "BasicDataStructures\ObjectCollection.h"
#include "Engine\Applications\MAPFMAP.H"
IlwisObjectPtr * createMapSoilMoistureFromAscat_l2(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapSoilMoistureFromAscat_l2 : public MapFromMap  
{
friend MapFromMap;

public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapSoilMoistureFromAscat_l2* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;

	virtual void Store();

	MapSoilMoistureFromAscat_l2(const FileName& fn, MapPtr& p);
	
	MapSoilMoistureFromAscat_l2(const FileName& fn, 
			MapPtr& p,
			const Map& lc_mp,
			String ascat_filepath
			);
	struct ModelTiePoint {
		double rCol;
		double rRow;
		double rX;
		double rY;
	};
protected:
	~MapSoilMoistureFromAscat_l2();
private:
	Map m_mpLc;
	FileName m_fnObject;
	FileName fnObjectName(const String& sExt, String,int iBand);
	void GetTiePoints(long iNrTiePoints, ModelTiePoint* amtp);
	//GeoRef	grf(CoordSystem& csy, RowCol rc);
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
	void getCoefs(File& f, double& c1, double& c2, double& c3, double& c4);
	String m_ascat_path;
	GeoRefCTPplanar* m_gcp;
	short nRasterYSize;
	short nRasterXSize;
	int   swath_indicator; //left=0 or right=1)
};

#endif // ILWMAPSMASCAC_L2_H
