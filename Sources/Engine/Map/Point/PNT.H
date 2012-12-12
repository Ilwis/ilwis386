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


#ifndef ILWPOINTMAP_H
#define ILWPOINTMAP_H
#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\Col.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Base\mask.h"
#include "Engine\Map\Point\ilwPoint.h"

class DATEXPORT PointMapStore;
class DATEXPORT PointMapVirtual;
class ForeignFormat;
class IlwisObjectVirtual;

class _export PointMapPtr : public BaseMapPtr
{
	friend class PointMapStore;
	friend class PointMapVirtual;
	friend class PointMap;
	friend class PointMapImport;
	friend class BaseMapPtr;
	static PointMapPtr  *create(const FileName&);
	static PointMapPtr  *create(const FileName&, const String& sExpression);
protected:
	PointMapPtr(const FileName& fn, bool fCreate=false);
	 PointMapPtr(const FileName& fn, const CoordSystem& cs, 
		const CoordBounds& cb, const DomainValueRangeStruct& dvs, long iPoints);
	 PointMapPtr(const FileName& fnPointMap, LayerInfo li);
	
	 ~PointMapPtr();            
public:
	virtual void  Store();
	void Load();
	void  Export(const FileName& fn) const;
	virtual String  sType() const;
	virtual vector<long>    viRaw(const Coord&, double rPrx=rUNDEF) const;
	virtual vector<long>    viValue(const Coord&, double rPrx=rUNDEF) const;
	virtual vector<double>  vrValue(const Coord&, double rPrx=rUNDEF) const;
	virtual vector<String>  vsValue(const Coord&, short iWidth=-1, short iDec=-1, double rPrx=rUNDEF) const;
	long  iFeatures() const;
	long  iRaw(long iRec) const;
	long  iValue(long iRec) const;
	double  rValue(long iRec) const;
	String  sValue(long iRec, short iWidth=-1, short iDec=-1) const;
	Coord  cValue(long iRec) const;
	bool  fPntInMask(long iRec, const Mask&) const;
	long  iRec(const String&) const;
	long  iRec(long iRaw) const;
	long  iRec(const Coord&, double rPrx=rUNDEF) const;
	Coord  cValue(const String& sValue) const;
	void  Bounds(Coord& crdMin, Coord& crdMax) const;
	virtual void  CalcMinMax();
	virtual void  Rename(const FileName& fnNew);
	virtual void  GetObjectDependencies(Array<FileName>& afnObjDep);
	virtual bool  fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
	void  SetDomainValueRangeStruct(const DomainValueRangeStruct& dvrs);
	virtual void  KeepOpen(bool f=true, bool force = false);
	// next group is moved from PointMapStore to this level (they work on member pms):
	void PutRaw(long iRec, long iRaw);
	void PutVal(long iRec, double rValue);
	Geometry *getFeature(long i) const;
	void  PutVal(long iRec, const String& sValue);
	void  PutVal(long iRec, const Coord&);
	void  Delete(long iRec);
	long  iAdd(long iRecs);
	long  iAddVal(const Coord&, const String& sValue);
	long  iAddVal(const Coord&, double rValue);
	long  iAddRaw(const Coord&, long iRaw);
	void  PutBufRaw(const CoordBuf&, const LongBuf&);
	// next group are reimplemented (from IlwisObjectPtr):
	virtual void  GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	virtual String  sExpression() const;
	virtual bool  fDependent() const; // returns true if it's dependent point map
	virtual bool  fCalculated() const; // returns true if a calculated result exists
	//  virtual bool fUpToDate() const; // returns true if an up to dat calculate result exists
	virtual bool  fDefOnlyPossible() const; // returns true if data can be retrieved without complete calculation (on the fly)
	virtual void  Calc(bool fMakeUpToDate=false); // calculates the result;
	// fMakeUpToDate is true it first updates the dependencies
	virtual void  DeleteCalc(); // deletes calculated result     
	void  BreakDependency(); // only keeps calculated result, if nothing has been calculated it calculates first
	void CreatePointMapStore();
	void  UndoAllChanges();
	virtual void  GetObjectStructure(ObjectStructure& os);
	virtual void  DoNotUpdate();
	 IlwisObjectVirtual *pGetVirtualObject() const;
	virtual void    SetCoordSystem(const CoordSystem&); // dangerous function!
	Feature *newFeature(geos::geom::Geometry *g=NULL);
	virtual vector<Geometry *> getFeatures(Coord crd, double rPrx=rUNDEF);
	bool removeFeature(const String& id, const vector<int>& selectedCoords=vector<int>());
	Geometry *getFeatureById(const String& id) const;
	vector<Feature *> getFeatures(const CoordBounds& cb, bool complete=true) const;
	RangeReal getZRange(bool force=false) ;
private:  
	void OpenPointMapVirtual(); // load PointMapVirtual and set member pmv
	PointMapStore* pms;
	PointMapVirtual* pmv;
};

class _export PointMap: public BaseMap
{
public:
   PointMap();
   PointMap(const FileName& fn);
   PointMap(const FileName& fn, const String& sExpression);
	 PointMap(const FileName& fnPointMap, LayerInfo inf);
   PointMap(const FileName& fn, const CoordSystem&, 
           const CoordBounds&, const DomainValueRangeStruct& dvs);  
   PointMap(const String& sExpression);
   PointMap(const String& sExpression, const String& sPath);
   PointMap(const PointMap& mp);
  void operator = (const PointMap& mp) { SetPointer(mp.pointer()); }
  PointMapPtr* ptr() const { return static_cast<PointMapPtr*>(pointer()); }
  PointMapPtr* operator -> () const { return ptr(); }
  void  Export(const FileName& fn) const;
  static FileName fnFromExpression(const String& sExpr);
	
};

inline PointMap undef(const PointMap&) { return PointMap(); }

#endif




