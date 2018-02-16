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
// MapDrainageNetworkExtraction.h: interface for the MapDrainageNetworkExtraction class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ILWDRAINAGENETWOREXTRACTION_H
#define ILWDRAINAGENETWOREXTRACTION_H

#include "Engine\Applications\MAPFMAP.H"
#include "LargeVector.h"

IlwisObjectPtr * createMapDrainageNetworkExtraction(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

#define sConstructDrainage  "TRUE"
#define sNotConstructDrainage  "FALSE"

class MapDrainageNetworkExtraction : public MapFromMap  
{
	friend MapFromMap;
	enum ThresholdMethod {tmValue , tmMap };
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapDrainageNetworkExtraction* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapDrainageNetworkExtraction(const FileName& fn, MapPtr& p);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
protected:
	virtual void Store();
	MapDrainageNetworkExtraction(const FileName& fn, MapPtr& p, const Map& map, 
															 ThresholdMethod thMethod,	
															 const Map& thmap,
															 const Map& mpFlowDir);
															 
	MapDrainageNetworkExtraction(const FileName& fn, MapPtr& p, const Map& map, 
															 ThresholdMethod thMethod, 
															 long iThreshold);
															 
	~MapDrainageNetworkExtraction();
	Map m_mp;
	long m_iThreshold;
private:
	void Init();
	ThresholdMethod m_tmMethods;
	LargeVector<LongBuf> m_vFacc;  //vector for the input flow accumulation map 
	LargeVector<ByteBuf> m_vOutput;
	LargeVector<LongBuf> m_vThreshold;	//the minimum contributing area per cell
	LargeVector<ByteBuf> m_vFlow;
	String m_sFlowMap;
	Map m_mpFlowDir;
	bool IsEdgeCell(long iRow, long iCol);
	bool IsDeadendLine(long& iRow, long& iCol);
};

#endif // ILWMAPFLOWDIRECTION_H
