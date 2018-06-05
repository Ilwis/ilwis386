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


#ifndef ILWSEGMAP_H
#define ILWSEGMAP_H
#include "Engine\Map\basemap.h"
#include "Engine\Map\Feature.h"
#include "Engine\Base\mask.h"
#include <Geos.h>

struct segtype;
class SegmentMapStore;
class SegmentMapVirtual;
class Tranquilizer;

namespace ILWIS {
	class _export Segment : public geos::geom::LineString, public Feature
{
public:
	Segment(QuadTree *tree, geos::geom::LineString *seg=NULL);
	virtual ~Segment();
	//CoordBounds		crdBounds() const; // new 3.0
	long			iBegin() const;   // begin node  - id in node table
	long			iEnd() const;	   // end node	  - id in node table
	Coord			crdBegin() ; //3.0
	Coord			crdEnd() ; //3.0
	double			rAzim(bool fEnd) const;  // azimuth at begin or end
	//void			GetCoords(long& iNr, CoordBuf& crdBuf, bool fResize = false) const; // 3.0
	void			PutCoords(long iNr, const CoordBuf& crdBuf) ; //3.0
	void			PutCoords(CoordinateSequence* sq);
	//static void		Tunnel(long& iNr, CoordinateSequence*, double rTunnelTolerance);
	void			Clip(const CoordBounds& cbClip, Array<CoordBuf>& acrdBuf) const;
	void			Densify(const double rDistance, CoordBuf& crdBuf) const;
	bool			fValid() const;
	bool			fInMask(const DomainValueRangeStruct& dvrs, const Mask&) const;

	virtual			FeatureType getType() const;
	virtual	void    getBoundaries(std::vector<geos::geom::CoordinateSequence*>& boundaries) const;
	long			nearSection(const Coord& crd, double delta, double& dist);


	double			rLength() const;
	virtual void	segSplit(long iAfter, Coord crd, ILWIS::Segment **seg=NULL) = 0; // 3.0
	CoordSystem		cs() const;// { return sm->ptr.cs(); }
	bool			fSelfCheck(long& iFirst, long& iSecond, Coord& crdAt); //3.0
	// checks on crossings and overlaps with itself. Returns true if ok.
	// when it returns false:
	// - self crossing: iFirst, iSecond and rcAt will be set for calls to segSplit()
	// - self overlay: iFirst will be -1. rcAt will be set on overlay start
	bool			fIntersects(bool& fOverlay, long& iAft, Coord& crdAt,ILWIS::Segment* s2, long& iAft2, Tranquilizer& trq);
	// checks if there is an intersection with s2, if so see below.
	//bool			fNoIntersections(bool& fOverlay, long& iAft, Coord& crdAt,ILWIS::Segment& s2, long& iAft2, Tranquilizer&); // 3.0
	//bool			fNoIntersections(const String& sMask, bool& fOverlay, long& iAft, Coord& crdAt, ILWIS::Segment& s2, long& iAft2, Tranquilizer&); //3.0
	// checks on crossings and overlaps with later segments. Returns true if ok.
	// when it returns false:
	// - hidden intersection: iAft and rcAt will be set for a call to segSplit. fOverlay=false
	//   s2 is the other segment, iAft2 is the position in the other segment
	// - overlay with other segment: same, fOverlay = true.
	Geometry		*clone() const=0;
protected:
	void			Split(long iAfter, Coord crdAt, CoordBuf& crdBufBefore, CoordBuf& crdBufAfter);
	bool			fAcceptDeleted;
	static          geos::geom::GeometryFactory * factory;
};

	class LSegment : public ILWIS::Segment {
public:
	LSegment(QuadTree *tree, geos::geom::LineString *seg=NULL);
	virtual long	iValue()const;
	virtual double	rValue() const;
	virtual String	sValue(const DomainValueRangeStruct& dvs, short iWidth=0, short iDec=0) const;
	virtual void	PutVal(long);
	virtual void	PutVal(double);
	virtual void	PutVal(const DomainValueRangeStruct& dvs, const String&);
	virtual void    segSplit(long iAfter, Coord crdAt, ILWIS::Segment **seg=NULL);
	Geometry		*clone() const;
private:
	long value;
};

class RSegment : public ILWIS::Segment {
public:
	RSegment(QuadTree *tree, geos::geom::LineString *seg=NULL);
	virtual long	iValue()const;
	virtual double	rValue() const;
	virtual String	sValue(const DomainValueRangeStruct& dvs, short iWidth=0, short iDec=0) const;
	virtual void	PutVal(long);
	virtual void	PutVal(double);
	virtual void	PutVal(const DomainValueRangeStruct& dvs, const String&);
	virtual void    segSplit(long iAfter, Coord crdAt, ILWIS::Segment **seg=NULL);
	Geometry		*clone() const;
private:
	double value;
};
};


class _export SegmentMapPtr : public BaseMapPtr
{
	friend class ILWIS::Segment;
	friend class Topology;
	friend class SegmentMap;
	friend class SegmentMapVirtual;
	friend class SegmentMapStore;
	friend class PolygonMapStore;
	friend class SegmentMapImport;
	friend class SegmentEditor;
	friend class BaseMapPtr;
	static SegmentMapPtr  *create(const FileName&);
	static SegmentMapPtr* create(const FileName&, const String& sExpression);
protected:
	 SegmentMapPtr(const FileName&, bool fCreate=false);
	 SegmentMapPtr(const FileName& fn, const CoordSystem& cs,
						  const CoordBounds& cb, const DomainValueRangeStruct& dvs);
	 SegmentMapPtr(const FileName& fnSegmentMap,LayerInfo li) ;
	 ~SegmentMapPtr();              
public:
	virtual void    Store();
	void            Export(const FileName& fn) const;
	virtual String  sType() const;
	virtual vector<long>    viRaw(const Coord&, double rPrx=rUNDEF) const;
	virtual vector<double>  vrValue(const Coord&, double rPrx=rUNDEF) const;
	virtual vector<long>    viValue(const Coord&, double rPrx=rUNDEF) const;
	virtual vector<String>  vsValue(const Coord&, short iWidth=-1, short iDec=-1, double rPrx=rUNDEF) const;
	long            iFeatures() const;	    // nr segments in map
	long            iSegDeleted() const;	    // nr of deleted segments in map
	void                   SetNumberOfSegments(long iNr);
	void                   SetNumberOfCoords(long iNr);
	void                   SetNumberOfDeletedSegments(long iNr);
	virtual void    SetDomainValueRangeStruct(const DomainValueRangeStruct&);
	virtual void    SetValueRange(const ValueRange&);
	ILWIS::Segment  *seg(const Coord&, double rPrx=rUNDEF) const;
	
	ILWIS::Segment  *segFirst(long& index, bool fAcceptDeleted=false) const;
	ILWIS::Segment  *segFirst(bool fAcceptDeleted=false) const;
	ILWIS::Segment  *segLast() const;
	ILWIS::Segment  *getFeature(const String&);
	Geometry		*getFeature(long i) const;
	Geometry		*getTransformedFeature(long iRec, const CoordSystem& csy) const	;
	virtual vector<Geometry *> getFeatures(Coord crd, double rPrx=rUNDEF);
	Geometry *getFeatureById(FeatureID id) const;
	bool removeFeature(FeatureID id, const vector<int>& selectedCoords=vector<int>());
	bool            fSatisfyCondition(const ILWIS::Segment& seg) const;
	
	void     SetAlfa(const CoordBounds& cb);
	virtual RangeReal getZRange(bool force=false) ;	
	
	//long            iNode(Coord) const; //3.0
	Coord    crdNode(Coord) const; // 3.0
	Coord    crdCoord(Coord, ILWIS::Segment** seg, long& iNr, double rPrx, bool fAcceptDeleted = false) const; //3.0
	Coord    crdPoint(Coord, ILWIS::Segment** seg, long& iAft, double rPrx, bool fAcceptDeleted = false) const; //3.0
	bool            fSegExist(const ILWIS::Segment& segNew, Tranquilizer* trq=0);
	bool            fSegExist(long iCrdNew, const CoordBuf& crdBufNew, const CoordBounds& mmBoundsNew, Tranquilizer* trq=0); //3.0
	
	virtual void  CalcMinMax();
	virtual void  Rename(const FileName& fnNew);
	virtual void  GetObjectDependencies(Array<FileName>& afnObjDep);
	virtual bool  fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
	virtual void  KeepOpen(bool f=true, bool force = false);

	// next group is moved from SegmentMapStore
	ILWIS::Segment*  newFeature(long iSegNr);
	Feature*  newFeature(geos::geom::Geometry *line=NULL);
    vector<Feature *> getFeatures(const CoordBounds& cb, bool complete=true) const;

	// next group are reimplemented (from IlwisObjectPtr):
	virtual void    GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	virtual String  sExpression() const;
	virtual bool    fDependent() const; // returns true if it's dependent point map
	virtual bool    fCalculated() const; // returns true if a calculated result exists

	virtual bool    fDefOnlyPossible() const; // returns true if data can be retrieved without complete calculation (on the fly)
	// calculates the result; fMakeUpToDate is true it first updates the dependencies
	virtual void    Calc(bool fMakeUpToDate=false); 
	
	virtual void    DeleteCalc(); // deletes calculated result     
	void            BreakDependency(); // only keeps calculated result, if nothing has been calculated it calculates first
	void                   CreateSegmentMapStore();
	void            RemoveRedundantNodes(Tranquilizer&);
	bool            fCheckForPolygonize(const String& sMask, bool fAutoCorrect, Tranquilizer&);
	const SegmentMapStore* sms() const { return pms; } // to use in segras for check
	void            UndoAllChanges();
	void            GetObjectStructure(ObjectStructure& os);
	virtual void    SetCoordSystem(const CoordSystem&); // dangerous function!		
	void            DoNotUpdate();		
	void            Pack();
	
private:			 
	void Load();
	void OpenSegmentMapVirtual(); // load SegmentMapVirtual and set member pmv
	SegmentMapStore* pms;
	SegmentMapVirtual* pmv;
	long _iSeg, _iCrd, _iSegDeleted;
public:
	double rSnapDist, rTunnelWidth;
};


class _export SegmentMap: public BaseMap
{
public:
   SegmentMap();
   SegmentMap(const FileName& fn);
	 SegmentMap(const FileName& fnSegmentMap,LayerInfo li);
   SegmentMap(const String& sExpression);
   SegmentMap(const String& sExpression, const String& sPath);
   SegmentMap(const FileName& fn, const String& sExpression);
   SegmentMap(const FileName& fn, const CoordSystem&,
             const CoordBounds&, const DomainValueRangeStruct& dvs);
  SegmentMap(const SegmentMap& sm): BaseMap(sm.pointer()){}
  void operator = (const SegmentMap& sm) { SetPointer(sm.pointer()); }
  SegmentMapPtr* ptr() const { return static_cast<SegmentMapPtr*>(pointer()); }
  SegmentMapPtr* operator -> () const { return ptr(); }
  void  Export(const FileName& fn) const;
};

inline SegmentMap undef(const SegmentMap&) { return SegmentMap(); }

#endif // ILWSEGMAP_H
