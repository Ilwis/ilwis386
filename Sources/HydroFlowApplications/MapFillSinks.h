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
// MapFillSinks.h: interface for the MapFillSinks class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ILWMAPFILLSINKS_H
#define ILWMAPFILLSINKS_H

//#include "MAPFMAP.H"
#include "Engine\Applications\MAPFMAP.H"

IlwisObjectPtr * createMapFillSinks(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class MapFillSinks : public MapFromMap  
{
	friend MapFromMap;
	enum FillMethod {fmFill , fmCut};
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapFillSinks* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapFillSinks(const FileName& fn, MapPtr& p);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
protected:
	virtual void Store();
	MapFillSinks(const FileName& fn, MapPtr& p, const Map& map, FillMethod sMethod);
  ~MapFillSinks();
private:
	FillMethod m_fmMethods;
	vector<RealBuf> m_vDEM;  //vector for input/output DEM 
	vector<LongBuf> m_vFlag;
	vector<RowCol> m_vSinks;
	long m_sinkPixels;
	long m_sinkPixelsThreshold;
    double m_sinkHeight;
	long m_iFlag; //flag each of cells in the sink contributing area 
	void SingleSinkFill();
	void GroupSinksFill();
	double getCutValue(RowCol rcOutlet);
	void GetNeighborCells(long iRow, long iCol, vector<double>&);
	bool fLocateInitialSink(RowCol& rc);
	void FindSinkContArea(RowCol rc);
	void FindSinkContArea2(RowCol rcInitSink);
	bool IsEdgeCell(long iRow, long iCol);
	void FlagAdjaCell(RowCol rc, vector<RowCol>&);
	bool fIdentifyOutletCell(RowCol rcInitSink, RowCol& rcOutlet);
	bool IsPotentialOutlet(long iRow, long iCol);
	void DepresFill(RowCol rcOutlet);
	void FlatAreaFlag(RowCol rcOutlet);
	void CutTerrain(RowCol rcOutlet);
	bool IsUndef(long iRow, long iCol);
	void FlagNeighbors(long iRow, long iCol);
};

#endif // ILWMAPFILLSINKS_H
