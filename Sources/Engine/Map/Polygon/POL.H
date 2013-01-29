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
 */

#pragma once

#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Feature.h"
#include "Engine\Map\Feature.h"
#include <Geos.h>

typedef ArrayLarge<Coord>  CoordArrayLarge;

class DATEXPORT PolygonMapStore;
class DATEXPORT PolygonMapVirtual;
class IlwisObjectVirtual;
struct LayerInfo;
class  SegmentEditor;
struct poltype;

namespace ILWIS {
class _export Polygon : public geos::geom::Polygon, public Feature
{
public:
	Polygon(QuadTree *tree);
	Polygon(QuadTree *tree, geos::geom::Polygon *pol);
	void				addBoundary(LinearRing *ring);
	void				addHole(LinearRing * ring);
	//CoordBounds			cbBounds() const; // bounding rectangle
	Coord				crdGravity() const; // center of gravity
	bool				fValid() const;
	bool				fInMask(const DomainValueRangeStruct& dvrs, const Mask&) const;
	bool				fContains(Coord) const;  // 3.0
	double				rArea() const;
	double				rLen() const;
	Coord				crdFindPointInPol() const; // a nicely located point;
	Coord				crdFindPointInPol2() const; // a 100% certain point not ending up in holes or similar
	virtual				FeatureType getType() const;
	virtual	void		getBoundaries(vector<CoordinateSequence*>& boundaries) const;
	virtual Coord centroid() const;

protected:
	String				sName(short iWidth=0, short iDec=-1) const;
	int					PutName(const String&);
	virtual Geometry *	copy(ILWIS::Polygon *) const;
private:
	LinearRing			*makeRing(const LineString *line) const;
};

class LPolygon : public ILWIS::Polygon {
public:
	LPolygon(QuadTree *tree, geos::geom::Polygon *pol=NULL);
	virtual long	iValue()const;
	virtual double	rValue() const;
	virtual String	sValue(const DomainValueRangeStruct& dvrs, short iWidth=-1, short iDec=-1) const;
	virtual void	PutVal(long);
	virtual void	PutVal(double);
	virtual void	PutVal(const DomainValueRangeStruct& dvs, const String& sV);
	Geometry *		clone() const;
private:
	long value;
};

class RPolygon : public ILWIS::Polygon {
public:
	RPolygon(QuadTree *tree, geos::geom::Polygon *pol=NULL);
	virtual long	iValue()const;
	virtual double	rValue() const;
	virtual String	sValue(const DomainValueRangeStruct& dvrs, short iWidth=-1, short iDec=-1) const;
	virtual void	PutVal(long);
	virtual void	PutVal(double);
	virtual void	PutVal(const DomainValueRangeStruct& dvs, const String& sV);
	Geometry *		clone() const;
private:
	double value;
};
};

class _export PolygonMapPtr: public BaseMapPtr
{
	friend class DATEXPORT ILWIS::Polygon;
	friend class DATEXPORT PolygonMap;
	friend class DATEXPORT PolygonMapVirtual;
	friend class DATEXPORT PolygonMapStore;
	friend class DATEXPORT PolygonMapImport;
	friend class BaseMapPtr;
	friend class SegmentEditor;
public:
	virtual void  Store();
	void Load();
	void Export(const FileName& fn) const;

	virtual void    SetDomainValueRangeStruct(const DomainValueRangeStruct&);
	virtual void    SetValueRange(const ValueRange&);
	
	virtual String  sType() const;
	virtual vector<long>    viRaw(const Coord&, double rPrx=rUNDEF) const;
	virtual vector<long>    viValue(const Coord&, double rPrx=rUNDEF) const;
	virtual vector<double>  vrValue(const Coord&, double rPrx=rUNDEF) const;
	virtual vector<String>  vsValue(const Coord&, short iWidth=-1, short iDec=-1, double rPrx=rUNDEF) const;
	
	long  iFeatures() const;
	long  iPolDeleted() const;

	ILWIS::Polygon   *polFirst() const;
	ILWIS::Polygon   *polLast() const;
	virtual vector<Geometry *> getFeatures(Coord crd, double rPrx=rUNDEF);
	Geometry   *getFeature(const String&) const;
	Geometry   *getFeature(long) const;
	Geometry *getFeatureById(const String& id) const;
	Geometry	*getTransformedFeature(long iRec, const CoordSystem& csy) const;
	bool removeFeature(const String& id, const vector<int>& selectedCoords=vector<int>());
	
	virtual void  CalcMinMax();
	virtual void  Rename(const FileName& fnNew);
	virtual void  GetObjectDependencies(Array<FileName>& afnObjDep);
	virtual bool  fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
	virtual void  KeepOpen(bool f=true, bool force = false);

	// next group is moved from PolygonMapStore
	Feature   *newFeature(geos::geom::Geometry *p=NULL);
	void addPolygon(ILWIS::Polygon *pol);

	void  CalcLengthAndAreaAndBounds(long iPol);
	bool         fCompact(PolygonMap& pm, Tranquilizer& trq);

	// next group are reimplemented (from IlwisObjectPtr):
	virtual void    GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	virtual String  sExpression() const;
	virtual bool    fDependent() const; // returns true if it's dependent point map
	virtual bool    fCalculated() const; // returns true if a calculated result exists

	virtual bool  fDefOnlyPossible() const; // returns true if data can be retrieved without complete calculation (on the fly)
	virtual void  Calc(bool fMakeUpToDate=false); // calculates the result;
	// fMakeUpToDate is true it first updates the dependencies
	virtual void  DeleteCalc(); // deletes calculated result     
	void  BreakDependency(); // only keeps calculated result, if nothing has been calculated it calculates first
	void         CreatePolygonMapStore();
	void  UndoAllChanges();
	bool  fTopologicalMap();
	void  TopologicalMap(bool fYes=true);
	void  GetObjectStructure(ObjectStructure& os);
	void  DoNotUpdate();	
	 IlwisObjectVirtual *pGetVirtualObject() const;
     vector<Feature *> getFeatures(const CoordBounds& cb, bool complete=true) const;
	 virtual RangeReal getZRange(bool force=false) ;	
	
protected:
	PolygonMapPtr(const FileName&, bool fCreate=false);
	 PolygonMapPtr(const FileName& fnPolMap, LayerInfo inf);
	 PolygonMapPtr(const FileName&, const CoordSystem&, const CoordBounds&, const DomainValueRangeStruct&);
	 ~PolygonMapPtr();
	
private:
	void OpenPolygonMapVirtual(); // load PolygonMapVirtual and set member pmv
	vector<ILWIS::Polygon *> pol(const Coord&) const;  // point in polygon
	PolygonMapStore* pms;
	PolygonMapVirtual* pmv;
	long _iPol, _iPolDeleted;
	bool fTopoMap;
	static PolygonMapPtr  *create(const FileName&);
	static PolygonMapPtr* create(const FileName&, const String& sExpression);
};

class _export PolygonMap: public BaseMap
{
public:
   PolygonMap();
   PolygonMap(const FileName& fn);
	 PolygonMap(const FileName& fnSegmentMap, LayerInfo li);
   PolygonMap(const String& sExpression);
   PolygonMap(const String& sExpression, const String& sPath);
   PolygonMap(const FileName& fn, const String& sExpression);
   PolygonMap(const FileName& fn, const CoordSystem&,
             const CoordBounds&, const DomainValueRangeStruct& dvs);
//  PolygonMap(const String& sExpression); tto be implemented
   PolygonMap(const PolygonMap& pm);
  void operator = (const PolygonMap& pm)
    { SetPointer(pm.pointer()); }
  PolygonMapPtr* ptr() const { return static_cast<PolygonMapPtr*>(pointer()); }
  PolygonMapPtr* operator -> () const { return ptr(); }
  void  Export(const FileName& fn) const;
  static FileName fnFromExpression(const String& sExpr);
};

