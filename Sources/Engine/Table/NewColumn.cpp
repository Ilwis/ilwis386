#include "Engine\Domain\dmclass.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dminfo.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\System\MemoryManager.h"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Table\COLSTRNG.H"
#include "Engine\Table\Colbinar.h"
#include "Engine\Table\COLCOORD.H"
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Table\tblinfo.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Scripting\Script.h"
#include "Engine\Base\Tokbase.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\TableStoreIlwis3.h"
#include "Engine\Table\NewColumn.h"


NewColumn::NewColumn(const Table& tblDflt, const String& sColName, bool fCreate) : ColumnPtr(tblDflt, sColName,fCreate) {
	table = dynamic_cast<TableStoreIlwis3 *>(tblDflt->pts);
}

NewColumn::NewColumn(const Table& tblDflt, const String& sColName, const DomainValueRangeStruct& dvs) : ColumnPtr(tblDflt, sColName, dvs) {
	table = dynamic_cast<TableStoreIlwis3 *>(tblDflt->pts);
}

long NewColumn::iRecs() const {
	return table->getRowCount();
}

long NewColumn::iRaw(long iKey) const {
	double v = rUNDEF; 
	if (0 != pcv)
		return pcv->iComputeRaw(iKey - iOffset());

	table->get(iKey  - iOffset(), colIndex,v);
	return v;
}

double NewColumn::rValue(long iKey) const{
	if (!dvs.fValues())
		return rUNDEF;
	if (dvs.fUseReals()) {
		double v;
		if (0 != pcv)
			return pcv->rComputeVal(iKey);

		table->get(iKey - 1, colIndex,v); // memory block is zero based, not 1 based
		return v;
	}
	long raw = iRaw(iKey);
	return dvs.rValue(raw);
}

long NewColumn::iValue(long iKey) const{
	if (!dvs.fValues())
		return iUNDEF;
	if (dvs.fUseReals()) {
		double v;
		if (0 != pcv)
			return pcv->rComputeVal(iKey);

		table->get(iKey , colIndex,v);
		return v;
	}
	long raw = iRaw(iKey);
	return dvs.rValue(raw);
	return iUNDEF;
}

String NewColumn::sValue(long iKey, short iWidth, short iDec) const{
	clock_t time1 = clock();

	if (dvs.fUseReals()) {
		double val = rValue(iKey);
		return dvs.sValue(val, iWidth, iDec);
	} else if ( dvs.fRawAvailable()) {
		long raw = iRaw(iKey);
		return dvs.sValueByRaw(raw, iWidth, iDec);
	} else if ( dvs.dm()->dmt() == dmtSTRING) {
		String s;
		table->get(iKey - iOffset(), colIndex, s);
		return s;
	} else if ( dvs.dm()->dmt() == dmtCOORD) {
		Coord c;
		table->get(iKey - iOffset(), colIndex, c);
		return dvs.dm()->pdcrd()->sValue(c, iWidth, iDec);
	} else if ( dvs.dm()->dmt() == dmtCOORDBUF) {
		return "TO BE DONE";
	}
	return sUNDEF;

}

Coord NewColumn::cValue(long iKey) const {
	Coord c;
	if (0 != pcv)
		return pcv->cComputeVal(iKey);

	table->get(iKey, colIndex,c);
	return c;
}

CoordinateSequence *NewColumn::iGetValue(long iRec, bool fResize) const{
	CoordinateSequence *seq = NULL;
	table->get(iRec, colIndex,&seq);

	return seq;
}

void NewColumn::GetBufRaw(ByteBuf& b, long iKey, long iNr) const{
	getBufRaw(b, iKey,iNr);
}

void NewColumn::GetBufRaw(IntBuf& b, long iKey, long iNr) const{
	getBufRaw(b,iKey, iNr);
}

void NewColumn::GetBufRaw(LongBuf& b, long iKey, long iNr) const{
	getBufRaw(b,iKey, iNr);
}

void NewColumn::GetBufVal(LongBuf& b, long iKey, long iNr) const{
	getBufRaw(b,iKey, iNr);
	if (!dvs.fRawIsValue())  {
		for (int i = 0; i < iNr; ++i)
			b[i] = dvs.iValue(b[i]);
	}
}

void NewColumn::GetBufVal(RealBuf& b, long iKey, long iNr) const{
	if (iNr == 0 || iNr > b.iSize())
		iNr = b.iSize();
	if (!dvs.fValues() || !dvs.fRawAvailable()) {
		for (int i = 0; i < iNr; ++i)
			b[i] = rUNDEF;
		return;
	}
	if ( fRawAvailable()) {
		LongBuf lb(b.iSize());
		getBufRaw(lb, iKey, iNr);
		if (dvs.fRawIsValue())
			for (int i = 0; i < iNr; ++i)
				b[i] = doubleConv(lb[i]);
		else
			for (int i = 0; i < iNr; ++i)
				b[i] = dvs.rValue(lb[i]);
	} else {
		if (iNr == 0 || iNr > b.iSize())
			iNr = b.iSize();
		int i = 0;
		for (long j = iKey; i < iNr; ++j, ++i)
			b[i] = rValue(j);
	}
}
void NewColumn::GetBufVal(StringBuf& b, long iKey, long iNr, short iWid, short iDec) const{
	if (iNr == 0 || iNr > b.iSize())
		iNr = b.iSize();
	LongBuf lb(b.iSize());
	getBufRaw(lb, iKey, iNr);
	for (int i = 0; i < iNr; ++i)
		b[i] = dvs.sValueByRaw(lb[i], iWid, iDec);
}

void NewColumn::GetBufVal(CoordBuf& b, long iKey, long iNr) const{
	if (iNr == 0 || iNr > b.iSize())
		iNr = b.iSize();
	for (int i = 0; i < iNr; ++i)
		b[i] = cValue(i);
}

void NewColumn::PutRaw(long iKey, long iRaw){
	table->put(iKey - iOffset(),colIndex,iRaw);
	if ( ptrTbl->fUseAs() )
		ptrTbl->PutLongInForeign(sName(), iKey, iRaw);	
}
void NewColumn::PutVal(long iKey, long iVal){
}
void NewColumn::PutVal(long iKey, double rVal){
	if (dvs.fUseReals()) {
		table->put(iKey - iOffset(), colIndex, rVal);
	} else if ( dvs.fRawAvailable()) {
		PutRaw(iKey, dvs.iRaw(rVal));
	} 
}
void NewColumn::PutVal(long iKey, const Coord& cVal){
	table->put(iKey - 1, colIndex, cVal);
}

void NewColumn::PutVal(long iKey, const String& sVal){
  if ( ptrTbl->fUseAs() && !fLoadingForeignData)
		ptrTbl->PutStringInForeign(sName(), iKey, sVal);	

	if (dvs.fUseReals()) {
		PutVal(iKey, dvs.rValue(sVal));
	} else if ( dvs.fRawAvailable()) {
		PutRaw(iKey, dvs.iRaw(sVal));
	} else if ( dvs.dm()->dmt() == dmtSTRING) {
		table->put(iKey - iOffset(), colIndex, sVal); 	
	} else if ( dvs.dm()->dmt() == dmtCOORD) {
		Coord c = dvs.dm()->pdcrd()->cValue(sVal);
		PutVal(iKey, c);
	} else if ( dvs.dm()->dmt() == dmtCOORDBUF) {
		return ; //TO BE DONE;
	}
}
void NewColumn::PutVal(long iKey, const CoordBuf& crdBuf, long iSz){
}
void NewColumn::PutBufRaw(const ByteBuf&, long iKey, long iNr){
}
void NewColumn::PutBufRaw(const IntBuf& , long iKey, long iNr){
}
void NewColumn::PutBufRaw(const LongBuf&, long iKey, long iNr){
}
void NewColumn::PutBufVal(const LongBuf&, long iKey, long iNr){
}
void NewColumn::PutBufVal(const RealBuf&, long iKey, long iNr){
}
void NewColumn::PutBufVal(const StringBuf&, long iKey, long iNr){
}
void NewColumn::PutBufVal(const CoordBuf&, long iKey, long iNr){
}
void NewColumn::DeleteRec(long iStartRec, long iRecs){
}

void NewColumn::AppendRec(long iRecs) {
	table->iRecNew(iRecs);
}

void NewColumn::Fill() {
	for(int i=0; i < table->getRowCount(); ++i) {
		table->put(i, colIndex, rUNDEF);
	}
}

void NewColumn::GetVal(long iKey, BinMemBlock&) const{
	MessageBox(0,"I am used GETVAL","", MB_OK);
}

void NewColumn::PutVal(long iKey, const BinMemBlock&){
	MessageBox(0,"I am used PUTVAL","", MB_OK);
}

void NewColumn::Rename(const String& sNewColName)
{
  const_cast<String&>(sNam) = sNewColName;
  table->rename(colIndex, sNewColName);
}

void NewColumn::setTable(int index, TableStoreIlwis3 *tb) {
	colIndex = index;
	table = tb;
}