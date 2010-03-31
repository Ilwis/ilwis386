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
/* MapStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  24 Jun 97    5:08 pm
*/

#ifndef ILWMAPSTORE_H
#define ILWMAPSTORE_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\DataObjects\Patch.h"

class APPEXPORT Tranquilizer;
class DATEXPORT Column;

class DATEXPORT MapStore;
class ForeignFormat;
struct LayerInfo;

class DATEXPORT MapStoreBase
{
	friend DATEXPORT MapStore;
protected:
	MapStoreBase(const FileName& fnDat, MapStore& mps, const RowCol& rcSize, 
		           const DomainValueRangeStruct& dvrs);
	MapStoreBase(const FileName& fnDat, MapStore& mps);
	virtual ~MapStoreBase();
	static MapStoreBase* create(const FileName& fnDat, MapStore& mps, const RowCol& rcSize, const DomainValueRangeStruct& dvrs);
	// Get/Put one pixel value
	virtual long iRaw(RowCol) const;
	virtual long iValue(RowCol) const;
	virtual double rValue(RowCol) const;
	virtual void PutRaw(RowCol, long) =0;
	virtual void PutVal(RowCol, double);
	virtual void PutVal(RowCol, const String&);
	// Get/Put a line
	virtual void GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum, int iPyrLayer = 0) const =0;
	virtual void GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum, int iPyrLayer = 0) const =0;
	virtual void GetLineRaw(long iLine, LongBuf&, long iFrom, long iNum, int iPyrLayer = 0) const =0;
	virtual void GetLineVal(long iLine, LongBuf&, long iFrom, long iNum, int iPyrLayer = 0) const =0;
	virtual void GetLineVal(long iLine, RealBuf&, long iFrom, long iNum, int iPyrLayer = 0) const =0;
	virtual void PutLineRaw(long iLine, const ByteBuf&, long iFrom, long iNum) =0;
	virtual void PutLineRaw(long iLine, const IntBuf&,  long iFrom, long iNum) =0;
	virtual void PutLineRaw(long iLine, const LongBuf&, long iFrom, long iNum) =0;
	virtual void PutLineVal(long iLine, const LongBuf&, long iFrom, long iNum) =0;
	virtual void PutLineVal(long iLine, const RealBuf&, long iFrom, long iNum) =0;
	virtual void GetPatchRaw(RowCol rc, BytePatch&) const =0;
	virtual void GetPatchRaw(RowCol rc, IntPatch& ) const =0;
	virtual void GetPatchRaw(RowCol rc, LongPatch&) const =0;
	virtual void GetPatchVal(RowCol rc, LongPatch&) const =0;
	virtual void GetPatchVal(RowCol rc, RealPatch&) const =0;
	virtual void PutPatchRaw(RowCol rc, const BytePatch&) =0;
	virtual void PutPatchRaw(RowCol rc, const IntPatch& ) =0;
	virtual void PutPatchRaw(RowCol rc, const LongPatch&) =0;
	virtual void PutPatchVal(RowCol rc, const LongPatch&) =0;
	virtual void PutPatchVal(RowCol rc, const RealPatch&) =0;
	virtual void Store() {};
	void KeepOpen(bool f =true);
	void SetErase(bool f =true);
	virtual StoreType st() const = 0;
	virtual String sType() const = 0;
	long iLines() const { return rcSize.Row; }
	long iCols() const { return rcSize.Col; }
	bool fInside(const RowCol& rc) const;
	File* file;
	File* filePyramid;
	DomainValueRangeStruct dvs;
	const ValueRange& vr() const { return dvs.vr(); }
	const Domain& dm() const { return dvs.dm(); }
	double rPredominant(double rV1, double rV2, double rV3, double rV4);
	void CreatePyramidLayers(const FileName& fn);	
	bool fHasPyramidFile();
	void DeletePyramidFile();
	int iNoOfPyramidLayers();

protected:
	void InitializePyramids(const FileName& fnPyr);
	virtual void IterateCreatePyramidLayer(int iPyrLayer, long &iLastFilePos, Tranquilizer *trq) {}	
	RowCol rcSize;
	bool& fChanged;
	vector<long> iPyramidLayerOffset;
};

inline bool MapStoreBase::fInside(const RowCol& rc) const
{
	return (rc.Row >= 0) && (rc.Col >= 0) &&
			(rc.Row < iLines()) && (rc.Col < iCols());
}

class DATEXPORT MapStore//: public MapPtr
{
	friend class DATEXPORT MapPtr;
	friend class DATEXPORT MapVirtual;
	friend class DATEXPORT MapImport;
	friend class DATEXPORT MapStoreBase;
	friend class _export MapWithHeader;
public:
	MapStore(const FileName&, MapPtr&);
	_export MapStore(const FileName&, MapPtr&, const GeoRef&, const RowCol& rcSize, 
		               const DomainValueRangeStruct&, const MapFormat mf=mfLine, bool fUseAs=false);
	MapStore(const FileName& fn, MapPtr& p, const LayerInfo& inf);
	_export ~MapStore();
	void Export(const FileName& fn) const;
	void Rename(const FileName& fnNew);
	void _export Store();
	static void UnStore(const FileName& fnObj);
	void StoreAs(const String& sExpression);
	long iRaw(RowCol rc) const
    { return mpsb->iRaw(rc); }
	long iValue(RowCol rc) const
    { return mpsb->iValue(rc); }
	double rValue(RowCol rc) const
    { return mpsb->rValue(rc); }
	void GetLineRaw(long iLine, ByteBuf& buf, long iFrom=0, long iNum=0, int iPyrLayer = 0) const
    { mpsb->GetLineRaw(iLine, buf, iFrom, iNum, iPyrLayer); }
	void GetLineRaw(long iLine, IntBuf& buf, long iFrom=0, long iNum=0, int iPyrLayer = 0) const
    { mpsb->GetLineRaw(iLine, buf, iFrom, iNum, iPyrLayer); }
	void GetLineRaw(long iLine, LongBuf& buf, long iFrom=0, long iNum=0, int iPyrLayer = 0) const
    { mpsb->GetLineRaw(iLine, buf, iFrom, iNum, iPyrLayer); }
	void GetLineVal(long iLine, LongBuf& buf, long iFrom=0, long iNum=0, int iPyrLayer = 0) const
    { mpsb->GetLineVal(iLine, buf, iFrom, iNum, iPyrLayer); }
	void GetLineVal(long iLine, RealBuf& buf, long iFrom=0, long iNum=0, int iPyrLayer = 0) const
    { mpsb->GetLineVal(iLine, buf, iFrom, iNum, iPyrLayer); }
	
	// Get a patch
	void GetPatchRaw(RowCol rc, BytePatch& pt) const
    { mpsb->GetPatchRaw(rc, pt); }
	void GetPatchRaw(RowCol rc, IntPatch& pt) const
    { mpsb->GetPatchRaw(rc, pt); }
	void GetPatchRaw(RowCol rc, LongPatch& pt) const
    { mpsb->GetPatchRaw(rc, pt); }
	void GetPatchVal(RowCol rc, LongPatch& pt) const
    { mpsb->GetPatchVal(rc, pt); }
	void GetPatchVal(RowCol rc, RealPatch& pt) const
    { mpsb->GetPatchVal(rc, pt); }
    
	void PutRaw(RowCol rc, long iRaw) 
    { mpsb->PutRaw(rc, iRaw); }
	void PutVal(RowCol rc, double rVal)  
    { mpsb->PutVal(rc, rVal); }
	void PutVal(RowCol rc, const String& sVal) 
    { mpsb->PutVal(rc, sVal); }
	void PutRaw(const Coord& crd, long iRaw) 
    { PutRaw(gr()->rcConv(crd), iRaw); }
	void PutRaw(const Coord& crd, double rVal) 
    { PutVal(gr()->rcConv(crd), rVal); }
	void PutVal(const Coord& crd, const String& sVal) 
    { PutVal(gr()->rcConv(crd), sVal); }
	void PutLineRaw(long iLine, const ByteBuf& buf, long iFrom=0, long iNum=0)
    { mpsb->PutLineRaw(iLine, buf, iFrom, iNum); }
	void PutLineRaw(long iLine, const IntBuf& buf, long iFrom=0, long iNum=0)
    { mpsb->PutLineRaw(iLine, buf, iFrom, iNum); }
	void PutLineRaw(long iLine, const LongBuf& buf, long iFrom=0, long iNum=0)
    { mpsb->PutLineRaw(iLine, buf, iFrom, iNum); }
	void PutLineVal(long iLine, const LongBuf& buf, long iFrom=0, long iNum=0)
    { mpsb->PutLineVal(iLine, buf, iFrom, iNum); }
	void PutLineVal(long iLine, const RealBuf& buf, long iFrom=0, long iNum=0)
    { mpsb->PutLineVal(iLine, buf, iFrom, iNum); }
	void PutPatchRaw(RowCol rc, const BytePatch& pt)
    { mpsb->PutPatchRaw(rc, pt); }
	void PutPatchRaw(RowCol rc, const IntPatch& pt)
    { mpsb->PutPatchRaw(rc, pt); }
	void PutPatchRaw(RowCol rc, const LongPatch& pt)
    { mpsb->PutPatchRaw(rc, pt); }
	void PutPatchVal(RowCol rc, const LongPatch& pt)
    { mpsb->PutPatchVal(rc, pt); }
	void PutPatchVal(RowCol rc, const RealPatch& pt)
    { mpsb->PutPatchVal(rc, pt); }
	virtual void CreatePyramidLayers();
	void _export Flush();
	void _export KeepOpen(bool f =true);
	void _export SetErase(bool f=true);
	void _export FillWithUndef();
	void _export Updated();
	void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	bool fPatchByte(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq);
	bool fPatchInt(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq);
	bool fPatchLong(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq);
	bool fPatchReal(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fUnPatchByte(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fUnPatchInt(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fUnPatchLong(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq);
	bool _export fUnPatchReal(MapPtr *mpPatch, const String& sTitle, Tranquilizer& trq);
	void SetSize(RowCol rc);
	bool fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
	const GeoRef& gr() const {
		return ptr.gr();
	}
	long iCols() const { return ptr.iCols(); }
	long iLines() const { return ptr.iLines(); }
	RowCol rcSize() const { return ptr.rcSize(); }
	Domain dm() const { return ptr.dm(); }
	CoordSystem cs() { return ptr.cs(); }
	
	StoreType st() const { return mpsb->st(); }
	
	const DomainValueRangeStruct& dvrs() const { return ptr.dvrs(); }
	const FileName& fnMap() { return fnObj; }
	void GetObjectStructure(ObjectStructure& os);
	bool fHasPyramidFile();
	void DeletePyramidFile();
	int iNoOfPyramidLayers();

private:
	MapStoreBase* mpsb;
	const FileName& fnObj; // fnObj from ptr (for convenience)
	MapPtr& ptr;  // the MapPtr of which this is a member
	Time timStore; // time of last change in data
	bool fUseAs;
};

#endif




