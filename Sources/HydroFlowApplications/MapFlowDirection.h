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
// MapFlowDirection.h: interface for the MapFlowDirection class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ILWMAPFLOWDIRECTION_H
#define ILWMAPFLOWDIRECTION_H

//#include "RasterApplication\mapfmap.h"
#include "Engine\Applications\MAPFMAP.H"
#include "LargeVector.h"

IlwisObjectPtr * createMapFlowDirection(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class MapFlowDirection : public MapFromMap  
{
	friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapFlowDirection* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapFlowDirection(const FileName& fn, MapPtr& p);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
protected:
	enum FlowMethod {fmSlope , fmHeight };
	virtual void Store();
	MapFlowDirection(const FileName& fn, MapPtr& p, const Map& map, FlowMethod sMethod, bool);
	~MapFlowDirection();
private:
	FlowMethod m_fmMethods;
	bool       m_fParallel;
	LargeVector<RealBuf> m_vDem;  //vector for the input DEM 
	LargeVector<ByteBuf> m_vFlow;
	LargeVector<LongBuf> m_vFlag;	//flag for the flat areas 
	vector<RowCol> m_vFlat;	//store a continuous flat area  	
	vector<byte> m_vDirection; 
	vector<byte> m_vFlowSelf; 
	void FillArray(long iRow, long iCol, vector<double>&);
	double rComputeSlope(double rCurH, double rNbH, int iPos);
	double rComputeHeightDifference(double rCurH, double rNbH);
	bool isEven(int elem);
	double rFindMaxLocation(vector<double>&, vector<int>&, int& iCout);
	long iLookUp(double rMax, int iCout,vector<int>&);
	bool isInOneEdge(int iPos1, int iPos2, int iPos3, vector<int>&);
	long m_ContFlat;
	void TreatFlatAreas();
	void LocateOutlets(RowCol rcInitFlat, vector<RowCol>&);
	void SetFlowsInFlatArea(vector<RowCol>& vOutlets);
	void CheckNeighbors(RowCol rc,vector<RowCol>&);
	void InitPars();
};

#endif // ILWMAPFLOWDIRECTION_H
