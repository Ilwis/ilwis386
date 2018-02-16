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
// MapFlowAccumulation.h: interface for the MapFlowAccumulation class.
//
//////////////////////////////////////////////////////////////////////
/* FlowAccumulation
   Copyright Ilwis System Development ITC
   April 2003,by Lichun Wang
*/
#ifndef ILWMAPFLOWACCUMULATION_H
#define ILWMAPFLOWACCUMULATION_H

#include "Engine\Applications\MAPFMAP.H"
#include "LargeVector.h"

IlwisObjectPtr * createMapFlowAccumulation(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class MapFlowAccumulation : public MapFromMap  
{
friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapFlowAccumulation* create(const FileName& fn, MapPtr& p, const String& sExpr);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
	MapFlowAccumulation(const FileName& fn, MapPtr& p);
protected:
	virtual void Store();
	MapFlowAccumulation(const FileName& fn, MapPtr& p, const Map& map);
  ~MapFlowAccumulation();
private:
	LargeVector<ByteBuf> m_vFlowDirection;  //vector for input flow direction 
	LargeVector<LongBuf> m_vFlowAcc; //vector for output flow accumulation 
	long iFlowAcc(long iRow, long iCol); //accumulated flow value for each cell 
};

#endif // ILWMAPFLOWACCUMULATION_H
