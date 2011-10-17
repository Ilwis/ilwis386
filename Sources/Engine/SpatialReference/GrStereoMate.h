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

 Created on: 2007-02-8
 ***************************************************************/
/* GeoRefStereoMate
   made for StereoFromOrthoImage
   Copyright Ilwis System Development ITC
   september 2001, by Jan Hendrikse,
*/

#include "Engine\SpatialReference\Gr.h"
#include "Engine\Map\Raster\Map.h"
//#include "Engine\SpatialReference\GRCTP.H"

#ifndef GRSTEREOMATE_H
#define GRSTEREOMATE_H

////typedef pair<double,double> PairRowCol;

class GeoRefStereoMate: public GeoRefPtr
{
	friend class GeoRef;
	friend class GeoRefPtr;
	
public:
	_export GeoRefStereoMate(const FileName& fn);
	_export GeoRefStereoMate(const FileName& fn, const GeoRef& grSource, const Map& mpDTM,
		const double rAngl, const double rHeigh,
		const long iColOffset);
	_export ~GeoRefStereoMate();
	bool fValid() const { return _fValid; }
	//virtual String sName(bool fExt = false, const String& sDirRelative = "") const;
	virtual String sType() const;
	virtual double rPixSize() const;
	virtual void Coord2RowCol(const Coord& c, double& rR, double& rC) const;
	virtual void RowCol2Coord(double rR, double rC, Coord& c) const;
	virtual void Store();
	void Init();
	void _export SetRowCol(RowCol rcSize);
	void GetObjectStructure(ObjectStructure& os);
	CoordBounds _export cb() const;
	
	//int _export Compute();
	
	double _export rGetLookAngle() const;
	RowCol rcGetRowColOffSet() const;
	double _export rGetRefHeight() const;
	GeoRef grGetGeoRefSourceMap() const;
	
	void _export SetRowColOffSet(const RowCol rcPiv);
	void _export SetLookAngle(const double rAng);
	void _export SetRefHeight(const double rRefH);
	void _export SetGeoRefSourceMap(const GeoRef grSrc);
	virtual bool fEqual(const IlwisObjectPtr&) const;
	virtual bool fGeoRefNone() const;
	
private:
	double rHeightAccuracy;
	int iMaxHeightCount; 
	RangeReal rrMMDTM;
	CoordBounds cbDTM;
	MinMax mmDTM;
	MinMax mmSourceMap;
	
	Map mapDTM;
	GeoRef grSourceMap;
	double rSourceMapPixSize;
	double rAngle;
	double rRefHeight;
	bool _fValid;
	double rEstimTerrainHeight; // 1st TerrainHeightGuess
	long iColLeftExtra;
	double rTanAngle;
	bool fTransformDTMCoords;
	CoordSystem csDTM;
};


#endif // GRSTEREOMATE_H
