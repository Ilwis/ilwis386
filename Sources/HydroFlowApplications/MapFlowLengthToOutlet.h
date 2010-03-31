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
// MapFlowLengthToOutlet.h: interface for the MapSlopeLengths class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ILWMAPFLOWLENGTH2OUTLET_H
#define ILWMAPFLOWLENGTH2OUTLET_H

#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Table\Col.h"
#include "Engine\Map\Point\PNT.H"

#define	sDownstreamCoord  "DownstreamCoord"
#define	sUpstreamCoord  "UpstreamCoord"
#define sUpstreamID "UpstreamLinkID"

IlwisObjectPtr * createMapFlowLength2Outlet(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class MapFlowLength2Outlet : public MapFromMap  
{
	friend MapFromMap;
public:
	static const char* sSyntax();
	virtual bool fFreezing();
	virtual String sExpression() const;
	static MapFlowLength2Outlet* create(const FileName& fn, MapPtr& p, const String& sExpr);
	MapFlowLength2Outlet(const FileName& fn, MapPtr& p);
	virtual bool fDomainChangeable() const;
	virtual bool fGeoRefChangeable() const;
protected:
	virtual void Store();
	MapFlowLength2Outlet(const FileName& fn, MapPtr& p,
										const Map& mpdrainageNetwork,
										const Map& mpFlow); 
	 ~MapFlowLength2Outlet();
	
private:
	vector<LongBuf>   m_vDrainageMap;   
	vector<ByteBuf>   m_vFlowDir;      //vector for input flow direction 
	vector<RealBuf>		m_vOutput_s;    //for lengths to stream output
	RowCol m_rcUpstream;
	vector<byte> m_vFlowNum;
	vector<byte> m_vReceiveNum;
	Map m_mpFlow;
	void init();
	bool IsEdgeCell(long iRow, long iCol);
	void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
	void Lengths2Outlet(long iStreamID, RowCol rc, bool);
	bool fLatLonCoords();
	bool fEllipsoidalCoords();
	double rDistance(Coord cd1, Coord cd2);
};

#endif // ILWMAPFLOWLENGTH2OUTLET_H
