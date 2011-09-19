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
/* PolygonMapStore
   Copyright Ilwis System Development ITC
   april 1995, by Wim Koolhoven
	Last change:  WK    7 Jul 97    1:05 pm
*/

#ifndef ILWPOLMAPSTORE_H
#define ILWPOLMAPSTORE_H
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Geos\Geos.h"

class APPEXPORT Tranquilizer;

bool fContainsComplete(const CoordBounds& cbOuter, const CoordBounds& cbInner);

class _export PolygonMapStore  : public MultiPolygon
{
	friend class DATEXPORT PolygonMapPtr;
	friend class DATEXPORT PolygonMapVirtual;
public:

	
	PolygonMapStore(const FileName&, PolygonMapPtr& p, bool fCreate=false);
	virtual      ~PolygonMapStore();
	void         Export(const FileName& fn) const;
	virtual void Store();
	static void  UnStore(const FileName& fnObj);
	long iPol() { return geometries->size(); }
	ILWIS::Polygon *operator[](long i);

	long				iPolDeleted() const { return ptr._iPolDeleted; }
	ILWIS::Polygon			*polFirst() const;
	ILWIS::Polygon			*polLast() const;
	vector<ILWIS::Polygon *>	pol(const Coord&) const;  // point in polygon
	ILWIS::Polygon			*pol(long) const;
	
	//ILWIS::Polygon			*polNewIsland(const String& sVal, const CoordBuf&); // 3.0
	//ILWIS::Polygon			*polNewIsland(const String& sVal, const CoordArrayLarge&); //3.0
	

	void      CalcLengthAndAreaAndBounds(long iPol);

	void         GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	void         KeepOpen(bool f =true);
	//void         Flush();
	//bool         fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
	void         Updated();
	virtual void SetErase(bool f=true);

	const DomainValueRangeStruct& dvrs() const { return ptr.dvrs(); }
	Domain                        dm() const { return ptr.dm(); }
	StoreType                     st() const { return ptr.st(); }

	bool fUseReals() const     { return ptr.fUseReals(); }
	bool fRealValues() const   { return ptr.fRealValues(); }
	bool fRawAvailable() const { return ptr.fRawAvailable(); }
	bool fRawIsValue() const   { return ptr.fRawIsValue(); }
	bool fValues() const       { return ptr.fValues(); }
	Feature*				newFeature(geos::geom::Geometry *p=NULL);
	Geometry				*getTransformedFeature(long iRec, const CoordSystem& csy) const;
	bool removeFeature(const String& id, const vector<int>& selectedCoords=vector<int>());
	Geometry			*getFeatureById(const String& id) const;
	void					addPolygon(ILWIS::Polygon *pol);
	virtual void			Load() {};
	
protected:
	PolygonMapStore(PolygonMapPtr& p,  const FileName& fn);
	void CalcBounds();
	PolygonMapPtr& ptr;  // the SegmentMapPtr of which this is a member
	const FileName& fnObj; // fnObj from ptr (for convenience)
	ObjectTime timStore; // time of last change in data
    geos::index::quadtree::Quadtree *spatialIndex;
	
private:
	void StoreRing(File& binaryFile, const LineString *ls);
	void getRings(long startIndex, const Table& tblTopology, const Table& tblPolygon, vector<LinearRing*>& rings );
	void AddBoundary(Topology& top, const String& sVal, Tranquilizer* trq=0);
	LinearRing *ReadRing(File&  binaryFile);

		
	//void FindPolygon(Topology topStart,
	//const ArrayLarge<Coord>& crdF,
	//const ArrayLarge<Coord>& crdL,
	//Tranquilizer& trq); // 3.0
	
	int iStatusFlags;
//	void AddLastPol(long id);
//	void RemoveLastPol(long id);
//	long iLastPol[5];
};

#endif // ILWPOLMAPSTORE_H




