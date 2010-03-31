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
/* $Log: /ILWIS 3.0/RasterStorage/Map.h $
 * 
 * 18    10/29/01 12:58p Martin
 * fHasPyramidFile is nu const geworden
 * 
 * 17    8/23/01 17:19 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 16    6/19/01 11:06a Martin
 * maps can now tell how many pyramid layers they have (to ensure that not
 * an impossible layer is used)
 * 
 * 15    6/14/01 11:49a Martin
 * added support for deleting pyramid files
 * 
 * 14    6/13/01 4:08p Martin
 * map can check if it has a pyramid file
 * 
 * 13    6/05/01 4:28p Martin
 * added interface for creating pyramid files and accessing the layers in
 * a file
 * 
 * 12    23-01-01 11:03a Martin
 * useas is passed as a parm with the constructor
 * 
 * 11    10-01-01 4:25p Martin
 * link through of the the StoreAs function in MapStore
 * 
 * 10    21-12-00 10:05a Martin
 * added the pGetVirtualObject() function to access the virtual object
 * embedded in the object. 
 * 
 * 9     23/11/00 10:43 Willem
 * Added st() virtual function: the StoreType is now retrieved from
 * MapStore
 * 
 * 8     11-09-00 11:34a Martin
 * added function for GetObjectStructure
 * 
 * 7     17-03-00 9:54a Martin
 * extra constructors for foreignformat support
 * 
 * 6     17-01-00 11:30 Wind
 * added proximity to iValue(const Coord& ..) etc.
 * 
 * 5     2-12-99 16:41 Hendrikse
 * made void _export OpenMapVirtual()  public
 * needed for use by MapComputedElsewhere
 * 
 * 4     9/29/99 11:05a Wind
 * added support for maps thata are a band in a maplist without mpr and
 * objectinfo in maplist
 * 
 * 3     3/08/99 2:27p Martin
 * //->/*
 * 
 * 2     3/08/99 2:09p Martin
 * exports added
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/11 16:44:21  Wim
// Added SetValueRange
//
/* Map
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   24 Apr 98    5:05 pm
*/

#ifndef ILWMAP_H
#define ILWMAP_H
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\Patch.h"
#include "Engine\SpatialReference\Gr.h"

enum MapFormat { mfLine, mfPatch, mfQuad };
enum InterpolMethod { imNEARNEIGHB, imBILINEAR, imBICUBIC };

class DATEXPORT Map;
class DATEXPORT MapStore;
class MapVirtual;
class APPEXPORT Tranquilizer;
class ForeignFormat;
struct LayerInfo;
class IlwisObjectVirtual;

class MapPtr : public BaseMapPtr
{
	friend class BaseMapPtr;
	friend class MapFromMap; // access to _gr
	friend class DATEXPORT MapStore;
	friend class DATEXPORT MapVirtual;
	friend class Map;
	friend class DATEXPORT MapImport;
	static MapPtr _export *create(const FileName&);
	static MapPtr* create(const FileName&, const String& sExpression);
	virtual void _export CalcMinMax();
protected:
	_export MapPtr(const FileName&, bool fCreate=false, long iBandNr = iUNDEF);
	_export MapPtr(const FileName& fn, const GeoRef& gr, const RowCol& rcSize, const DomainValueRangeStruct& dvs, 
		MapFormat mf=mfLine, long iBandNr=iUNDEF, bool fUseAs=false);
	_export MapPtr(const FileName& fn, const LayerInfo& inf);
	inline bool fInside(const RowCol& rc) const;
public:
	virtual String _export sName(bool fExt = false, const String& sDirRelative = "") const;
    virtual String _export sNameQuoted(bool fExt = false, const String& sDirRelative = "") const;
	
	virtual void _export Store();
	virtual String _export sType() const;
    virtual StoreType _export st() const;

	virtual _export ~MapPtr();
	void _export Export(const FileName& fn);
	void _export InitName(const FileName& fn);
	const GeoRef& gr() const { return _gr; }
	RowCol rcSize() const { return _rcSize; }
	virtual void  _export SetSize(RowCol rc);
	long iLines() const { return rcSize().Row; }
	long iCols() const { return rcSize().Col; }
	void _export SetGeoRef(const GeoRef&); // dangerous function!
	virtual void _export SetValueRange(const ValueRange&); // dangerous function!
	MapFormat mf() const { return _mf; }
	void _export FillWithUndef();
	// Get one pixel value
	long   _export iRaw(RowCol) const;
	long   _export iValue(RowCol) const;
	double _export rValue(RowCol) const;
	String _export sValue(RowCol, short iWidth=-1, short iDec=-1) const;
	long   _export iRaw(const Coord&, double rPrx=rUNDEF) const;
	long   _export iValue(const Coord&, double rPrx=rUNDEF) const;
	double _export rValue(const Coord&, double rPrx=rUNDEF) const;
	String _export sValue(const Coord&, short iWidth=-1, short iDec=-1, double rPrx=rUNDEF) const;
	
	// Get a line
	void _export GetLineRaw(long iLine, ByteBuf&, long iFrom=0, long iNum=0, int iPyrLayer = 0) const;
	void _export GetLineRaw(long iLine, IntBuf& , long iFrom=0, long iNum=0, int iPyrLayer = 0) const;
	void _export GetLineRaw(long iLine, LongBuf&, long iFrom=0, long iNum=0, int iPyrLayer = 0) const;
	void _export GetLineVal(long iLine, LongBuf&, long iFrom=0, long iNum=0, int iPyrLayer = 0) const;
	void _export GetLineVal(long iLine, RealBuf&, long iFrom=0, long iNum=0, int iPyrLayer = 0) const;
	
	// Get a patch
	void _export GetPatchRaw(RowCol, BytePatch&) const;
	void _export GetPatchRaw(RowCol, IntPatch& ) const;
	void _export GetPatchRaw(RowCol, LongPatch&) const;
	void _export GetPatchVal(RowCol, LongPatch&) const;
	void _export GetPatchVal(RowCol, RealPatch&) const;
	// next group is moved from MapStore to this level (they work on member pms):
	void _export PutRaw(RowCol rc, long iRaw);
	void _export PutVal(RowCol rc, double rVal);
	void _export PutVal(RowCol rc, const String& sVal);
	void _export PutRaw(const Coord& crd, long iRaw); 
	void _export PutRaw(const Coord& crd, double rVal); 
	void _export PutVal(const Coord& crd, const String& sVal); 
	void _export PutLineRaw(long iLine, const ByteBuf&, long iFrom=0, long iNum=0);
	void _export PutLineRaw(long iLine, const IntBuf&,  long iFrom=0, long iNum=0);
	void _export PutLineRaw(long iLine, const LongBuf&, long iFrom=0, long iNum=0);
	void _export PutLineVal(long iLine, const LongBuf&, long iFrom=0, long iNum=0);
	void _export PutLineVal(long iLine, const RealBuf&, long iFrom=0, long iNum=0);
	void _export PutPatchRaw(RowCol rc, const BytePatch&);
	void _export PutPatchRaw(RowCol rc, const IntPatch& );
	void _export PutPatchRaw(RowCol rc, const LongPatch&);
	void _export PutPatchVal(RowCol rc, const LongPatch&);
	void _export PutPatchVal(RowCol rc, const RealPatch&);
	////  
	bool _export fPatchByte(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fPatchInt(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fPatchLong(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fPatchReal(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fUnPatchByte(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fUnPatchInt(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fUnPatchLong(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fUnPatchReal(MapPtr* mpPatch, const String& sTitle, Tranquilizer& trq);
	virtual void _export Rename(const FileName& fnNew);
	virtual void _export GetObjectDependencies(Array<FileName>& afnObjDep);
	virtual bool _export fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
	virtual void _export KeepOpen(bool f=true);
	void _export Replace(const String& sExpression);
	// next group are reimplemented (from IlwisObjectPtr):
	virtual void _export GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	virtual String _export sExpression() const;
	virtual bool _export fDependent() const; // returns true if it's dependent point map
	virtual bool _export fCalculated() const; // returns true if a calculated result exists
	//  virtual bool fUpToDate() const; // returns true if an up to dat calculate result exists
	virtual bool _export fDefOnlyPossible() const; // returns true if data can be retrieved without complete calculation (on the fly)
	virtual void _export Calc(bool fMakeUpToDate=false); // calculates the result;
	// fMakeUpToDate is true it first updates the dependencies
	virtual void _export DeleteCalc(); // deletes calculated result     
	void _export BreakDependency(); // only keeps calculated result, if nothing has been calculated it calculates first
	void _export CreateMapStore();
	DomainValueRangeStruct _export dvrsDefault(const String& sExpression) const;
	bool _export fMergeDomainForCalc(Domain& dm, const String& sExpr);
	InterpolMethod interpolMethod() const { return im; }
	void SetInterpolMethod(InterpolMethod intmeth) { im = intmeth; }
	bool _export fMapInMapList() const;
	void _export OpenMapVirtual(); // load MapVirtual and set member pmv
	void _export GetObjectStructure(ObjectStructure& os);
	void _export DoNotUpdate();
	_export IlwisObjectVirtual *pGetVirtualObject() const;
	_export void StoreAs(const String& sExpression);
	_export void CreatePyramidLayer();
	_export bool fHasPyramidFile() const;
	_export void DeletePyramidFile();
	_export int  iNoOfPyramidLayers();
private:  
	GeoRef _gr;
	RowCol _rcSize;
	MapFormat _mf;
	// Interpolation:
	InterpolMethod im;
	double rValueBiLinear(double rRow, double rCol) const;
	double rValueBiCubic(double rRow, double rCol) const;
	double rBiCubicResult(long iRow, long iCol, double rDCol) const;
	double rBiCubicPolynom(double rPar[], double rDeltaRow) const;
	bool   fResolveRealUndefs(double rPar[]) const;
	// Store and Virtual:
	MapStore* pms;
	MapVirtual* pmv;
	long iBandNr;
};


class Map: public BaseMap
{
	friend class MapPtr;
public:
	_export Map();
	_export Map(const FileName& fn);
	_export Map(const FileName& fnMpl, long iBandNr);
	_export Map(const FileName& fn, const GeoRef& gr, const RowCol& rc, const DomainValueRangeStruct& dvs,
		const MapFormat mf = mfLine, long iBandNr=iUNDEF, bool fUseAs=false);
	_export Map(const FileName& fn, const String& sExpression);
	_export Map(const String& sExpression);
	_export Map(const String& sExpression, const String& sPath);
	_export Map(const Map& mp);
	_export Map(const FileName& fnObj, const LayerInfo& _inf);
	void operator = (const Map& mp) { SetPointer(mp.pointer()); }
	MapPtr* ptr() const { return static_cast<MapPtr*>(pointer()); }
	MapPtr* operator -> () const { return ptr(); }
	void _export Export(const FileName& fn) ;
	FileName fnFromExpression(const String& sExpr);
	static int _export iColorType(const String& sColor);
};

inline Map undef(const Map&)
{
	return Map();
}

inline bool MapPtr::fInside(const RowCol& rc) const
{
	return (rc.Row >= 0) && (rc.Col >= 0) &&
			(rc.Row < rcSize().Row) && (rc.Col < rcSize().Col);
}

#endif




