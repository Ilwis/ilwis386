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
/* Column, ColumnPtr
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    7 May 98    5:00 pm
*/

#ifndef ILWCOL_H
#define ILWCOL_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\tbl.h"
#include "Engine\Base\objdepen.h"

class Column;
class ColumnPtr;
class DATEXPORT ColumnStore;
class TableStore;
class DATEXPORT ColumnVirtual;
class ColumnCoordBuf;
class DATEXPORT BinMemBlock;
class IlwisObjectVirtual;

class ColumnPtr: public IlwisObjectPtr
{
friend class TablePtr;
friend class TableStore;
friend class Column;
friend class ColumnStore;
friend class DATEXPORT ColumnStoreBase; // access to dvs
friend class ColumnVirtual;
friend class ObjectDependency;

public:
	virtual              String sName(bool = false, const String& = "") const;
	virtual              String sNameQuoted(bool = false, const String& = "") const;
	virtual void         Store();
	virtual String       sType() const;
	long                 iOffset() const 
	                           { return _iOffset; }
	long                 iRecs() const 
	                           { return _iRecs; }
	const Domain&        dm() const 
	                           { return dvs.dm(); }
	const Domain&        dmKey() const 
	                           { return _dmKey; }
	long _export         iRaw(long iKey) const;
	long _export         iValue(long iKey) const;
	double _export       rValue(long iKey) const;
	String _export       sValue(long iKey, short iWidth=-1, short iDec=-1) const;
	Coord _export        cValue(long iKey) const;
	CoordinateSequence  _export         *iGetValue(long iRec, bool fResize = false) const;
	void _export         GetBufRaw(ByteBuf&, long iKey=0, long iNr=0) const;
	void _export         GetBufRaw(IntBuf& , long iKey=0, long iNr=0) const;
	void _export         GetBufRaw(LongBuf&, long iKey=0, long iNr=0) const;
	void _export         GetBufVal(LongBuf&, long iKey=0, long iNr=0) const;
	void _export         GetBufVal(RealBuf&, long iKey=0, long iNr=0) const;
	void _export         GetBufVal(StringBuf&, long iKey=0, long iNr=0, short iWid=0, short iDec=-1) const;
	void _export         GetBufVal(CoordBuf&, long iKey, long iNr) const;
	void _export         PutRaw(long iKey, long iRaw);
	void _export         PutVal(long iKey, long iVal);
	void _export         PutVal(long iKey, double rVal);
	void _export         PutVal(long iKey, const Coord& cVal);
	void _export         PutVal(long iKey, const String& sVal);
	void _export         PutVal(long iKey, const CoordinateSequence *seq, long iSz);
	void _export         PutBufRaw(const ByteBuf&, long iKey=0, long iNr=0);
	void _export         PutBufRaw(const IntBuf& , long iKey=0, long iNr=0);
	void _export         PutBufRaw(const LongBuf&, long iKey=0, long iNr=0);
	void _export         PutBufVal(const LongBuf&, long iKey=0, long iNr=0);
	void _export         PutBufVal(const RealBuf&, long iKey=0, long iNr=0);
	void _export         PutBufVal(const StringBuf&, long iKey=0, long iNr=0);
	void _export         PutBufVal(const CoordBuf&, long iKey, long iNr);
	void _export         DeleteRec(long iStartRec, long iRecs = 1);
	void _export         AppendRec(long iRecs = 1);
	void _export         Fill();
	void                 SetMinMax(const RangeInt& ri);
	void _export         SetMinMax(const RangeReal& rr);
	void                 StoreTime(); // stores object time in obj def. file section for column
	void _export         GetVal(long iKey, BinMemBlock&) const;
	void _export         PutVal(long iKey, const BinMemBlock&);
	void _export         CalcMinMax();
	void _export		 CalcMinMaxSelection(const vector<long>& rows, RangeReal& rrMinMax, double& sum, double& stdev, double& avg) ;
	void _export Replace(const String& sExpression);
	void _export         BreakDependency(); // only keeps calculated result, if nothing has been calculated it calculates first
	void                 CreateColumnStore();
	void  _export        SetOwnedByTable(bool f=true);
	virtual void _export GetNewestDependentObject(String& sObjName, ObjectTime& tmNewer) const;
	virtual void         DependencyNames(Array<String>& asNames) const; // read from file
	virtual void         Rename(const String& sNewColName);
	virtual void _export Updated();
	virtual void         SetDomainValueRangeStruct(const DomainValueRangeStruct&);
	virtual void         SetValueRange(const ValueRange&); // dangerous function!
	virtual void _export Calc(bool fMakeUpToDate=false); // calculates the result;
	virtual void         DeleteCalc(); // deletes calculated result
	StoreType            st() const
	                           { return dvs.st(); }
	bool                 fMonotone(bool fAscending=true) const;
	bool _export         fOwnedByTable() const;
	bool _export         fMergeDomainForCalc(Domain& dm, const String& sExpr);
	bool                 fUseReals() const     
	                           { return dvs.fUseReals(); }
	bool                 fRealValues() const   
	                           { return dvs.fRealValues(); }
	bool                 fRawAvailable() const 
	                           { return dvs.fRawAvailable(); }
	bool                 fRawIsValue() const   
	                           { return dvs.fRawIsValue(); }
	bool                 fValues() const       
	                           { return dvs.fValues(); }
	bool                 fCoords() const       
	                           { return dvs.fCoords(); }
	const ValueRange&    vr() const 
	                           { return dvs.vr(); }
	RangeReal _export    rrMinMax();
	RangeInt _export     riMinMax();
	double _export       rMean();
	double _export       rStdDev();
	double _export       rSum();
	bool _export         fBinary() const;
	bool _export         fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
	virtual bool         fDependent() const; // returns true if it's dependent column
	virtual bool         fCalculated() const; // returns true if a calculated result exists
	virtual bool         fDefOnlyPossible() const; // returns true if data can be retrieved without complete calculation (on the fly)
	bool _export         fUniqueValues() const;
	BinMemBlock          binValue(long iKey) const;
	String _export       sTableAndColumnName(const String& sRelPath);
	String _export       sSection() const;        
	virtual String       sObjectSection() const;
	Domain _export       dmFromStrings(const FileName& fnDom, DomainType dmt);
	virtual DomainInfo   dminf() const;
	DomainValueRangeStruct _export dvrsDefault(const String& sExpression) const;
	const DomainValueRangeStruct&  dvrs() const 
	                           { return dvs; }
	virtual String _export  sExpression() const;
	ColumnCoordBuf _export *pcbuf(); //used to access brgin and end of coordbuf
	virtual void _export    DoNotUpdate();

	long                 _iRecs;
	FileName             fnTbl;
	const String         sNam;
	void _export         Loaded(bool fValue);
	bool _export         fIsLoaded() const;
	void _export         SetLoadingForeignData(bool fYesNo);
	bool _export         fGetLoadingForeignData() const;
	void _export         GetObjectStructure(ObjectStructure& os);
	void _export         GetObjectDependencies(Array<FileName>& afnObjDep);
	_export IlwisObjectVirtual *pGetVirtualObject() const;

	~ColumnPtr();

protected:
	ColumnPtr(const Table& tblDflt, const String& sColName, bool fCreate=false);
	ColumnPtr(const Table& tblDflt, const String& sColName, const DomainValueRangeStruct& dvs);

	int                  ReadEntry(const char* sEntry, String& sValue);
	long                 iReadEntry(const char* sEntry);
	double               rReadEntry(const char* sEntry);
	bool _export         WriteEntry(const char* sEntry, const String& sValue);
	bool _export         WriteEntry(const char* sEntry, const char* sValue);
	bool                 WriteEntry(const char* sEntry, bool fValue);
	bool  _export               WriteEntry(const char* sEntry, long iValue);
	bool                 WriteEntry(const char* sEntry, double rValue);
	RangeInt             _riMinMax;
	RangeReal            _rrMinMax;
	String               sSectionPrefix;
	void                 OpenColumnVirtual(); // load ColumnMapVirtual and set member pcv
	ColumnStore*         pcs;
	ColumnVirtual*       pcv;
	ObjectDependency     objdep;
	bool                 _fOwnedByTable;
	DomainValueRangeStruct  dvs;
	double               _rMean, _rStdDev, _rSum;

private:
	static ColumnPtr*      create(const Table& tbl, const String& sColName);
	static ColumnPtr*      create(const Table& tbl, const String& sColName,
	                           const String& sExpression, const DomainValueRangeStruct& dvs);
	static ColumnPtr*      create(const Table& tbl, const String& sColName,
	                           const DomainValueRangeStruct& dvs);

	Domain                 _dmKey; // DomainClass or DomainValue(min,max)
	long                   _iOffset;
	TablePtr              *ptrTbl;
	bool                  fLoadingForeignData;
};

class Column: public IlwisObject
{
	friend class ColumnPtr;

public:
	_export Column();
	_export Column(const Column& col);
	_export Column(const String& sTableAndColName);
	_export Column(const String& sTableAndColName, const Table& tblDefault);
	_export Column(const Table& tblDflt, const String& sColName);
	_export Column(const Table& tblDflt, const String& sColName, const String& sExpression);
	_export Column(const Table& tblDflt, const String& sColName, const String& sExpression, const Domain& dm);
	_export Column(const Table& tblDflt, const String& sColName, const String& sExpression, const DomainValueRangeStruct& dvs);		
	_export Column(const Table& tblDflt, const String& sColName, const DomainValueRangeStruct& dvs);

	void                      operator = (const Column& col) 
	                              { SetPointer(col.pointer()); }
	ColumnPtr*                ptr() const 
	                              { return static_cast<ColumnPtr*>(pointer()); }
	ColumnPtr*                operator -> () const 
	                              { return ptr(); }
	void                      VirtualToStore();
	bool _export              fUsedInOpenColumns() const;
	static void _export       SplitTableAndColumn(const Table& tblDefault, const String& sTableAndColName, String& sTable, String& sCol);
	static ColumnPtr*         pGet(const Table& tbl, const String& sCol);

private:
	bool                      fInitForConstructor(const String& sTableAndColName, const Table& tblDefault);
	bool                      fInitForConstructor(const Table& tblDflt, const String& sColName,
	                                            const String& sExpression, const Domain& dm);
	bool                      fInitForConstructor(const Table& tblDefault, const String& sColName,
	                              const String& sExpression, const DomainValueRangeStruct& dvr);
	static IlwisObjectPtrList listCol;
};

extern DATEXPORT Column colUNDEF;
inline Column undef(const Column&) { return colUNDEF; }

#endif // ILWCOL_H
