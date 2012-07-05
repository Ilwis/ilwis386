#pragma once

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
//class TableStoreIlwis3;

class TableStoreIlwis3;


class _export NewColumn : public ColumnPtr
{
friend class TablePtr;
friend class TableStore;
friend class Column;
friend class ColumnStore;
friend class DATEXPORT ColumnStoreBase; // access to dvs
friend class ColumnVirtual;
friend class ColumnPtr;


public:
	//void Store();
	void setTable(int index, TableStoreIlwis3 *tb);
	long iRecs() const ;
	long iRaw(long iKey) const;
	long iValue(long iKey) const;
	double rValue(long iKey) const;
	String sValue(long iKey, short iWidth=-1, short iDec=-1) const;
	Coord cValue(long iKey) const;
	CoordinateSequence *iGetValue(long iRec, bool fResize = false) const;
	void GetBufRaw(ByteBuf&, long iKey=0, long iNr=0) const;
	void GetBufRaw(IntBuf& , long iKey=0, long iNr=0) const;
	void GetBufRaw(LongBuf&, long iKey=0, long iNr=0) const;
	void GetBufVal(LongBuf&, long iKey=0, long iNr=0) const;
	void GetBufVal(RealBuf&, long iKey=0, long iNr=0) const;
	void GetBufVal(StringBuf&, long iKey=0, long iNr=0, short iWid=0, short iDec=-1) const;
	void GetBufVal(CoordBuf&, long iKey, long iNr) const;
	void          PutRaw(long iKey, long iRaw);
	void          PutVal(long iKey, long iVal);
	void          PutVal(long iKey, double rVal);
	void          PutVal(long iKey, const Coord& cVal);
	void          PutVal(long iKey, const String& sVal);
	void          PutVal(long iKey, const CoordBuf& crdBuf, long iSz);
	void          PutBufRaw(const ByteBuf&, long iKey=0, long iNr=0);
	void          PutBufRaw(const IntBuf& , long iKey=0, long iNr=0);
	void          PutBufRaw(const LongBuf&, long iKey=0, long iNr=0);
	void          PutBufVal(const LongBuf&, long iKey=0, long iNr=0);
	void          PutBufVal(const RealBuf&, long iKey=0, long iNr=0);
	void          PutBufVal(const StringBuf&, long iKey=0, long iNr=0);
	void          PutBufVal(const CoordBuf&, long iKey, long iNr);
	void          DeleteRec(long iStartRec, long iRecs = 1);
	void AppendRec(long iRecs = 1);
	void Fill();
	void GetVal(long iKey, BinMemBlock&) const;
	void PutVal(long iKey, const BinMemBlock&);
	virtual void Rename(const String& sNewColName);
	ColumnCoordBuf  *pcbuf(); //used to access brgin and end of coordbuf

	//void          Loaded(bool fValue);
	//bool          fIsLoaded() const;
	//void          SetLoadingForeignData(bool fYesNo);
	//bool          fGetLoadingForeignData() const;
	//void          GetObjectStructure(ObjectStructure& os);
	//void          GetObjectDependencies(Array<FileName>& afnObjDep);
	// IlwisObjectVirtual *pGetVirtualObject() const;


protected:
	NewColumn(const Table& tblDflt, const String& sColName, bool fCreate=false);
	NewColumn(const Table& tblDflt, const String& sColName, const DomainValueRangeStruct& dvs);

private:

	TableStoreIlwis3 *table;
	long colIndex;

	template<class T> void getBufRaw(Buf<T>& b, long iKey, long iNr) const {
	  if (iNr == 0 || iNr > b.iSize())
		  iNr = b.iSize();
	  int i = 0;
	  for (long j = iKey; i < iNr; ++j, ++i)
		b[i] = iRaw(j);
	}
};

