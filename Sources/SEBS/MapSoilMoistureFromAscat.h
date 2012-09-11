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
#ifndef ILWMAPSMASCAC_H
#define ILWMAPSMASCAC_H

//#include "RasterApplication\mapfmap.h"
//#include "afx.h"
//#include "BasicDataStructures\MapList.h"
//#include "BasicDataStructures\ObjectCollection.h"
#include "Engine\Applications\MAPFMAP.H"
IlwisObjectPtr * createMapSoilMoistureFromAscat(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class MapSoilMoistureFromAscat : public MapFromMap  
{
friend MapFromMap;

public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapSoilMoistureFromAscat* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	MapSoilMoistureFromAscat(const FileName& fn, MapPtr& p);
	MapSoilMoistureFromAscat(const FileName& fn, 
			MapPtr& p,
			const Map& sand_mp,
			const Map& clay_mp,
			const Map& fc_mp,
			const Map& lc_mp,
			String ascat_filepath
			);
protected:
	virtual void Store();
	~MapSoilMoistureFromAscat();
private:	
	struct ModelTiePoint {
		double rCol;
		double rRow;
		double rX;
		double rY;
	};
	FileName m_fnObject;
	FileName fnObjectName(const String& sExt, String,int iBand);
	void fillAdjcentCells(long row, long col, int lines, int cols);
	Map m_mpClay;
	Map m_mpFc;
	Map m_mpLc;
	//Map m_mpDEM;
	bool m_fmpAod;
	//void GetTiePoints(long iNrTiePoints, ModelTiePoint* amtp);
	//GeoRef	grf(CoordSystem& csy, RowCol rc);
	double calculate(double s0, double s1, double s3, double t0,double t1, double t2,
					 double sand,double clay,double LAI,
					 double Fc,double Fcs, double Fcc, double Fcf,int lc,long ,long); 
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
	void getCoefs(File& f, double& c1, double& c2, double& c3, double& c4);
	String m_ascat_path;
	//GeoRefCTPplanar* m_gcp;
	short nRasterYSize;
	short nRasterXSize;
	int   swath_indicator; //left=0 or right=1)
	//vector<RealBuf> m_vMv;
	vector<RealBuf> m_vS1;
	vector<RealBuf> m_vS2;
	vector<RealBuf> m_vS3;
	vector<RealBuf> m_vT1;
	vector<RealBuf> m_vT2;
	vector<RealBuf> m_vT3;
	RealBuf m_rBufSr;
	RealBuf m_rBufTal;
	RealBuf m_rBufF_ref;
	RealBuf m_rBufWv;
	/*RealBuf m_rBufSr_1;
	RealBuf m_rBufSr_2;
	RealBuf m_rBufF_ref1;
	RealBuf m_rBufF_ref2;
	*/
	LongBuf m_iBufLc;
	//vector<RealBuf> m_vDEM;
	//vector<RealBuf> m_vSr;
};

#endif // ILWMAPSMASCAC_H
