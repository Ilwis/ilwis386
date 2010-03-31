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
/* $Log: /ILWIS 3.0/PointMap/PNT.H $
 * 
 * 18    15-10-03 13:53 Retsios
 * Added function to fill the pointmap from a buffer (performance reason)
 * 
 * 17    10-01-03 11:02 Hendrikse
 * added  SetCoordSystem(const CoordSystem&); cto be used by property
 * sheet when user changes csy of pointmap. It calls
 * pointmapstore::SetCoordSystem(const CoordSystem&); to ensure that the
 * coordinate column also gets the new csy as domain
 * 
 * 16    9/26/02 12:25p Lichun
 * exported iAdd function.
 * 
 * 15    8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 14    5/03/01 10:34 Willem
 * Removed superfluous member _iPoints from PointMapPtr: iPnt() now gets
 * the number of points from the PointMapStore or PointMapVirtual
 * 
 * 13    21-12-00 10:23a Martin
 * added access function for the virtualobject
 * 
 * 12    8-09-00 4:40p Martin
 * added function GetObjectStructure and DoNotUpdate
 * 
 * 11    27-04-00 12:46 Koolhoven
 * Added function UndoAllChanges()
 * and internal help funciton Load()
 * 
 * 10    28-02-00 8:49a Martin
 * changes to do with multiple shapes in one layer. The LayerInfo object
 * is now passed as parm
 * 
 * 9     21-02-00 4:38p Martin
 * Pntmap works now with PointMapStoreForeignFormat for non ilwis formats
 * 
 * 8     15-02-00 8:55a Martin
 * Added changes for use of foreign formats
 * 
 * 7     17-01-00 11:29 Wind
 * added proximity to iValue(const Coord& ..) etc.
 * 
 * 6     12-01-00 17:12 Wind
 * removed proximity (moved to basemap)
 * 
 * 5     15-07-99 18:39 Koolhoven
 * externals
 * 
 * 4     17-06-99 1:09p Martin
 * added s for import/export dll
 * 
 * 3     7-06-99 13:14 Koolhoven
 * Header comment
 * 
 * 2     7-06-99 11:09 Koolhoven
 * Added exports
// Revision 1.4  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.3  1997/08/26 11:04:41  martin
// SetDomaiRangeStruct added as function
//
// Revision 1.2  1997/08/04 15:01:37  Wim
// Added fPntInMask()
//
/* PointMap
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  MS   26 Aug 97   11:54 am
*/

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
	virtual void  KeepOpen(bool f=true);
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




