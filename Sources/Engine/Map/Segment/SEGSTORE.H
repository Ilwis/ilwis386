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
/* $Log: /ILWIS 3.0/SegmentMap/SEGSTORE.H $
 * 
 * 15    8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 14    1/03/01 14:32 Willem
 * The DomainValueRangeStruct and ValueRange for the segment map now also
 * set the ValueRange of the proper TableSegment Column as well to have
 * the domain of the segmentmap and that of the TableSegment Column
 * synchronized.
 * 
 * 13    13-02-01 8:28a Martin
 * when changing the coordsystem of a map the coordsystem of the internal
 * tables (columns) is also changed
 * 
 * 12    24/01/01 16:33 Willem
 * Added code to let Ilwis2.x data file be correctly set to R/O and R/W
 * 
 * 11    24-11-00 12:29 Koolhoven
 * Added Pack() option for segments
 * 
 * 10    11/01/00 2:01p Martin
 * removed superflous GetObjectStructure. They are all handled in the
 * relevant ilwisobject
 * 
 * 9     11-09-00 11:30a Martin
 * added function for objectStructure 
 * 
 * 8     16-06-00 20:49 Koolhoven
 * use Column members of SegmentMapStore instead of enum ColumnTypes with
 * indirections
 * 
 * 7     16-06-00 2:57p Martin
 * made detsructor virtual
 * 
 * 6     28-02-00 8:49a Martin
 * Segmentmap can now read foreign formats
 * 
 * 5     7-02-00 17:04 Wind
 * solved problems with readonly maps and their conversion to format 3.0
 * 
 * 4     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 3     10-12-99 11:53a Martin
 * //->/*
 * 
 * 2     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
// Revision 1.3  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.2  1997/09/19 15:51:54  Wim
// Added buffer alSegBuf
//
/* SegmentMapStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   19 Sep 97   11:14 am
*/

#ifndef ILWSEGSTORE_H
#define ILWSEGSTORE_H
#include "Engine\Map\Segment\Seg.h"
#include "Headers\geos.h"

class APPEXPORT Tranquilizer;
class SegmentMapPtr;

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

	long                 iNode(Coord) const; // 3.0
	Coord                crdNode(Coord) const;
	Coord                crdCoord(Coord, ILWIS::Segment**, long&) const; //3.0
	Coord                crdPoint(Coord, ILWIS::Segment**, long& iAft, bool fAcceptDeleted) const;  // 3.0

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
	
private:
	void Truncate(); // remove all contents, make empty
	
	int iStatus;
	String sCode;
	bool fErase;
};

#endif // ILWSEGSTORE_H




