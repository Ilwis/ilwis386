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

#ifndef ILWSEGSTORE_H
#define ILWSEGSTORE_H
#include "Engine\Map\Segment\Seg.h"
#include "Geos\Geos.h"

class APPEXPORT Tranquilizer;
class SegmentMapPtr;

namespace geos {
	namespace index {
		namespace quadtree{
		class Quadtree;
		}
	}
}

class _export SegmentMapStore	: public MultiLineString
{
	friend class SegmentMapPtr;

public:
	SegmentMapStore(const FileName&, SegmentMapPtr& p, bool fCreate=false, bool fForeign=false);
	virtual ~SegmentMapStore();

	void                 Export(const FileName& fn) const;
	virtual void		 Store();
	static void          UnStore(const FileName& fnObj);

	long                 iSeg() const { return ptr._iSeg; }
	long                 iSegDeleted() const { return ptr._iSegDeleted; }
	long                 iCrd() const { return ptr._iCrd; }

	ILWIS::Segment*     segFirst(long& index, bool fAcceptDeleted=false) const;
	ILWIS::Segment*     segLast() const;
	ILWIS::Segment*     seg(long) const;
	ILWIS::Segment*	 seg(const String& sV);
	//ILWIS::Segment*     newFeature(long iSegNr);
	Feature*		    newFeature(geos::geom::Geometry *line = NULL);
	Geometry			*getTransformedFeature(long iRec, const CoordSystem& csy) const	;
	void				removeFeature(const String& id, const vector<int>& selectedCoords=vector<int>());

	//long                 iNode(Coord) const; // 3.0
	//Coord                crdNode(Coord) const;
	Coord                crdCoord(Coord, ILWIS::Segment**, long&) const; //3.0
	Coord                crdPoint(Coord, ILWIS::Segment**, long& iAft, double rPrx) const;  // 3.0

	bool                 fSegExist(const ILWIS::Segment& segNew, Tranquilizer* trq=0) const;
	bool                 fSegExist(const CoordinateSequence *crdBufNew, 
                                const CoordBounds& crdBoundsNew, Tranquilizer* trq) const;

	void 				 GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	bool				 fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
	void				 KeepOpen(bool f =true);
	void				 Flush();
	void				 Updated();

	virtual void		 SetErase(bool f=true);

	const DomainValueRangeStruct& dvrs() const { return ptr.dvrs(); }
	Domain               dm() const { return ptr.dm(); }
	StoreType            st() const { return ptr.st(); }
	/*virtual void         SetDomainValueRangeStruct(const DomainValueRangeStruct&);
	virtual void         SetValueRange(const ValueRange&);*/

	bool fUseReals() const     { return ptr.fUseReals(); }
	bool fRealValues() const   { return ptr.fRealValues(); }
	bool fRawAvailable() const { return ptr.fRawAvailable(); }
	bool fRawIsValue() const   { return ptr.fRawIsValue(); }
	bool fValues() const       { return ptr.fValues(); }

	void				DoNotUpdate();	
	void				Pack();
	
protected:
	SegmentMapPtr& ptr;  // the SegmentMapPtr of which this is a member
	void SetCoordSystem(const CoordSystem& cs);	
	ObjectTime timStore; // time of last change in data
	geos::index::quadtree::Quadtree *spatialIndex;
	
private:
	void Truncate(); // remove all contents, make empty
	
	int iStatus;
	String sCode;
	bool fErase;
};

#endif // ILWSEGSTORE_H




