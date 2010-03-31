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
/* $Log: /ILWIS 3.0/PolygonMap/POL.H $
 * 
 * 21    8/23/01 17:07 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 20    1/03/01 14:31 Willem
 * PolygonMap and PolygonMapStore now set DomainValueRangeStruct or
 * ValueRange of the proper TablePolygon Column as well to have the domain
 * of the polygonmap and that of the TablePolygon Column synchronized..
 * 
 * 19    21-12-00 10:06a Martin
 * added the pGetVirtualObject() function to access the virtual object
 * embedded in the object. 
 * 
 * 18    12-09-00 8:41a Martin
 * added 
 * 
 * 17    11-09-00 10:54a Martin
 * added functionfor GetObjectStructure and DoNotUpdate
 * 
 * 16    1-09-00 10:33 Koolhoven
 * Made SetCoordSystem() virtual so that PolygonMap can also set the
 * coordsystem of its internal segmentmap
 * 
 * 15    25-07-00 10:28a Martin
 * added acces function and member to indicate that a polygonmap is (non)
 * topological
 * 
 * 14    27-04-00 14:59 Koolhoven
 * Added UndoAllChanges()
 * 
 * 13    1-03-00 12:43p Martin
 * handling of polygon maps as foreign format (phase 1)
 * 
 * 12    20-01-00 10:49a Martin
 * 
 * 11    17-01-00 11:30 Wind
 * added proximity to iValue(const Coord& ..) etc.
 * 
 * 10    17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 9     10-12-99 3:14p Martin
 * removed internal coordinates and used true coords
 * 
 * 8     10-12-99 11:49a Martin
 * removed internal coordinates and replaced them by true coords
 * 
 * 7     21-07-99 14:45 Koolhoven
 * exports
 * 
 * 6     21-07-99 12:27 Koolhoven
 * exports
 * 
 * 5     17-06-99 1:09p Martin
 * added s for import/export dll
 * 
 * 4     7-06-99 15:56 Koolhoven
 * Header comment
 * 
 * 
 * 3     7-06-99 11:06 Koolhoven
 * Added exports
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/04 14:42:03  Wim
// Added ILWIS::Polygon::fInMask(mask)
//
/* PolygonMap
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    4:39 pm
*/

#pragma once

#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Feature.h"
#include "Engine\Map\Feature.h"
#include "Headers\geos.h"

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
	Polygon();
	Polygon(geos::geom::Polygon *pol);
	void				addBoundary(LinearRing *ring);
	void				addHole(LinearRing * ring);
	CoordBounds			cbBounds() const; // bounding rectangle
	Coord				crdGravity() const; // center of gravity
	bool				fValid() const;
	bool				fInMask(const DomainValueRangeStruct& dvrs, const Mask&) const;
	bool				fContains(Coord) const;  // 3.0
	double				rArea() const;
	double				rLen() const;
	Coord				crdFindPointInPol() const; // a nicely located point;
	Coord				crdFindPointInPol2() const; // a 100% certain point not ending up in holes or similar
	virtual				FeatureType getType() const;

protected:
	String				sName(short iWidth=0, short iDec=-1) const;
	int					PutName(const String&);
	virtual Geometry *	copy(ILWIS::Polygon *) const;
private:
	LinearRing			*makeRing(const LineString *line) const;
};

class LPolygon : public ILWIS::Polygon {
public:
	LPolygon(geos::geom::Polygon *pol=NULL);
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
	RPolygon(geos::geom::Polygon *pol=NULL);
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
	Geometry	*getTransformedFeature(long iRec, const CoordSystem& csy) const;
	
	virtual void  CalcMinMax();
	virtual void  Rename(const FileName& fnNew);
	virtual void  GetObjectDependencies(Array<FileName>& afnObjDep);
	virtual bool  fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
	virtual void  KeepOpen(bool f=true);

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

