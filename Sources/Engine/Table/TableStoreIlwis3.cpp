#include "Headers\Toolspch.h"
#include "Engine\Base\System\MemoryManager.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\COLINFO.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\TableStoreIlwis3.h"
#include "Engine\Table\NewColumn.h"
#include "Engine\Table\Colbinar.h"

using namespace ILWIS;
#define S(a) String(a).c_str()

TableStoreIlwis3::TableStoreIlwis3() : records(0), rowCount(0), colCount(0), strings(0){
	loaderOnly = true;
}

TableStoreIlwis3::TableStoreIlwis3(const FileName& fn, TablePtr& p) : TableStore(fn,p), records(0), rowCount(0), colCount(0), strings(0)
{
	for(int i = 0 ; i < iCols(); ++i) {
		static_cast<NewColumn *>(ac[i].ptr())->setTable(i, this);
	}
	load(fn,"");
}

TableStoreIlwis3::~TableStoreIlwis3(){
	if ( !loaderOnly && ptr.fChanged)
		store(ptr.fnObj,"");

	delete strings;
	for(long r = 0; r < getRowCount(); ++r) {
		for(long c = 0; c < columnInfo.size(); ++c) {
			const ColumnInfo& info = columnInfo.at(c);
			if ( info.getNewIndex() != iUNDEF) {
				char *p = records + r * recordSize + info.getOffset();
				if ( info.getColumnType() == ColumnInfo::ctBINARY && info.isSharedValue() == false){
					delete [] p; 
				}
			}
		}
	}
	delete [] records;
}

void TableStoreIlwis3::setColumnInfo(const FileName& fnODF, bool& simpleDataTypes, int cnt) {
	vector<int> newIndexs;
	// these indexes must be restored
	for(int col = 0 ; col < columnInfo.size(); ++col) {
		newIndexs.push_back(columnInfo[col].getNewIndex());
	}
	columnInfo.resize(cnt);
	recordSize = 0;
	for(int col = 0 ; col < cnt; ++col) {
		columnInfo.at(col).Read(fnODF,col);
		columnInfo.at(col).setOffset(recordSize);
		columnInfo.at(col).setIndex(col);
		int oldSz = recordSize;
		switch (columnInfo.at(col).getColumnType()) {
			case ColumnInfo::ctRAW:
				recordSize+=4; break;
			case ColumnInfo::ctSTRING: // stored as pointer
			case ColumnInfo::ctBINARY: // stored as pointer
				recordSize+=4; 
				simpleDataTypes = false;
				break;
			case ColumnInfo::ctCRD: 
				recordSize += 16; break;
			case ColumnInfo::ctCRD3D:
				recordSize+=24;break;
			case ColumnInfo::ctREAL:
				recordSize+=8; break;
		}
		int sz = recordSize - oldSz;
		columnInfo.at(col).setFieldSize(sz);
		if ( newIndexs.size() > col)
			columnInfo.at(col).setNewIndex(newIndexs.at(col));
	}
	for(int i =0; i < getColCount(); ++i) {
		columnsByName[columnInfo.at(i).sName()] = i;
	}
}

void TableStoreIlwis3::readData(char *memblock) {
	long posFile = 128;
	long posMem = 0;
	double c33 ; long t33; double v33;
	for(long r = 0; r < getRowCount(); ++r) {
		int stringColCount = 0;
		for(long c = 0; c < getColCount(); ++c) {
			const ColumnInfo& info = columnInfo.at(c);
			char *p = records + r * recordSize + info.getOffset();
			switch(info.getColumnType()) {
				case ColumnInfo::ctRAW: 
					*(long *)p = *(long *)(memblock + posFile);
					t33 = *(long *)p;
					posFile += 4;
					break;
				case ColumnInfo::ctREAL:
					*(double *)p = *(double *)(memblock + posFile);
					v33 = *(double *)p;
					posFile += 8;
					break;
				case ColumnInfo::ctCRD:
					memcpy(p,memblock + posFile,16);
					c33= *(double *)p;
					posFile += 16;
					break;
				case ColumnInfo::ctCRD3D:
					memcpy(p,memblock + posFile,24);
					posFile += 24;
					break;
				case ColumnInfo::ctSTRING:
					{
						long len = 0;
						long stringIndex = stringColCount * r;
						*(long *)p = (long)readString(stringIndex, memblock + posFile, len);
						posFile+= len;
						++stringColCount;
					}
					break;
				case ColumnInfo::ctBINARY:
					long cnt = 0;
					*(long *)p = (long)readCoordList(memblock + posFile, cnt);
					posFile += cnt;
					break;
			}
		}
	}
}

void TableStoreIlwis3::calcStoreSizeNeeded(vector<unsigned long>& recordLocations) const {
	unsigned long fixedSize = 0;
	vector<unsigned long> variabelSizes(rowCount, 0);
	for(int i = 0; i < columnInfo.size(); ++i) {
		const ColumnInfo& info = columnInfo.at(i);
		ColumnInfo::ColumnType ct = info.getColumnType();
		if (  ct == ColumnInfo::ctRAW)
			fixedSize +=  4;
		else if ( ct == ColumnInfo::ctREAL)
			fixedSize +=  8;
		else if ( ct == ColumnInfo::ctCRD)
			fixedSize +=  16;
		else if ( ct == ColumnInfo::ctCRD3D)
			fixedSize +=  24;
	}
	for(int j = 0; j < rowCount; ++j) {
		unsigned long sz = fixedSize;
		for(int i = 0; i < columnInfo.size(); ++i) {
			const ColumnInfo& info = columnInfo.at(i);
			ColumnInfo::ColumnType ct = info.getColumnType();
			if ( ct == ColumnInfo::ctSTRING) {
				const Column& col = ptr.col(i);
				sz += col->sValue(j+1,0,0).size() + 1;

			}else if ( ct == ColumnInfo::ctBINARY) {
				const Column col = ptr.col(i);
				const BinMemBlock &bm = col->binValue(j+1); 
				sz += bm.iSize();
			}
		}
		recordLocations[j] = sz + (j > 0 ? recordLocations[j-1] : 0);	
	}
}

void TableStoreIlwis3::writeData(ofstream& file, int r, int c) {
	long iv;
	double rv;
	Coord crd;
	//const ColumnInfo& info = columnInfo.at(c);
	Column col = ac[c];
	StoreType st = col->dvrs().st();
	bool usesRaw = col->dvrs().fRawAvailable();
	if ( usesRaw) {
		iv = col->iRaw(r);
		file.write((const char *)&iv,4);
	}
	else if ( st == stREAL) {
		rv = col->rValue(r);
		file.write((const char *)&rv,8);
	}
	else if(st == stCRD) { 
		crd = col->cValue(r);
		file.write((const char *)&(crd.x), 8);
		file.write((const char *)&(crd.y), 8);
	}
	else if ( st == stCRD3D) {
		crd = col->cValue(r);
		file.write((const char *)&(crd.x), 8);
		file.write((const char *)&(crd.y), 8);
		file.write((const char *)&(crd.z), 8);
	}
	else if ( st == stSTRING) {
		String s = col->sValue(r);
		file.write(s.c_str(), s.size() +1);
	}
	//	case ColumnInfo::ctBINARY:
	//		long cnt = 0;
	//		*(long *)p = (long)readCoordList(memblock + posFile, cnt);
	//		posFile += cnt;
	//		break;
	//}

}

void TableStoreIlwis3::Store() {
	TableStore::Store();
	store(ptr.fnObj,ptr.sSectionPrefix);
}
void TableStoreIlwis3::store(const FileName& fnODF, const String& prfix){

	String prefix = prfix == "" ? "" : prfix + ":";
	clock_t time1 = clock();
	ObjectInfo::WriteElement(S(prefix + "Table"),"Columns",fnODF, (long)ac.size());
	ObjectInfo::WriteElement(S(prefix + "Table"),"Records",fnODF, getRowCount());
	ObjectInfo::WriteElement(S(prefix + "TableStore"),"Records",fnODF, getRowCount());
	ObjectInfo::ReadElement(S(prefix + "TableStore"), "Data", fnODF, fnData);
	bool simpleDataTypes = true;

	bool allSimple = true;
	// setColumnInfo will reset column size, but we need to keep the old values as this is the value correct for the memory
	// block. The new size will include size of columns that are in the old structure.
	long oldRecordSize = recordSize; 
	setColumnInfo(fnODF,allSimple,ac.size());
	recordSize = oldRecordSize;
	for(int col = 0 ; col < ac.size(); ++col) {
		ObjectInfo::WriteElement(S(prefix + "TableStore"),String("Col%d",col).c_str(),fnODF, ac[col]->sName());
	}


	ofstream file (fnData.sFullPath().c_str(), ios::out|ios::binary);
	if (!file.is_open())
		return;


	char sHeader[128];
	memset(sHeader, 0, 128);
	strcpy(sHeader, "ILWIS 2.00 Table\r\n\032");
	file.write(sHeader,128);

	bool allNewStruct = true;
	for(long c = 0; c < ac.size(); ++c) {
		NewColumn *col = dynamic_cast<NewColumn *>(ac[c].ptr());

		if (!col)
			allNewStruct = false;
	}
	for(long c = 0; c < columnInfo.size(); ++c) {
		if ( columnInfo[c].isDeleted()) {
			allNewStruct = false;
		}
	}
	if ( allNewStruct && allSimple) {
		file.write(records,recordSize * getRowCount());
	} else {
		vector<unsigned long> recordLocations(rowCount,0);
		calcStoreSizeNeeded(recordLocations);
		unsigned long oldSize = 0;
		unsigned long j = 0;
		unsigned long step = 30000;
		while( j < rowCount) {
			int rows = min(step, recordLocations.size() - j);
			unsigned long sz = recordLocations[j + rows - 1];
			char *writebuffer = new char[sz - oldSize];
			fillBuffer(writebuffer, j, rows);
			file.write(writebuffer,sz - oldSize);
			j+= step;
			delete [] writebuffer;
			oldSize = sz;
		}
	}

	file.close();
}

void TableStoreIlwis3::fillBuffer(char *writebuffer, unsigned long startRow, unsigned long rows) {

	//const ColumnInfo& info = columnInfo.at(c);
	int loc = 0;
	for( unsigned long row = startRow + 1; row <= startRow + rows; ++row) {
		for(int i = 0; i < columnInfo.size(); ++i) {
			const ColumnInfo& info = columnInfo.at(i);
			ColumnInfo::ColumnType ct = info.getColumnType();
			Column col = ptr.col(info.sName());
			if (  ct == ColumnInfo::ctRAW){
				(*(long *)(writebuffer + loc)) = col->iRaw(row);
				loc += 4;
			}
			else if ( ct == ColumnInfo::ctREAL) {
				(*(double *)(writebuffer + loc)) = col->rValue(row);
				loc += 8;
			}
			else if ( ct == ColumnInfo::ctCRD){
				Coord crd = col->cValue(row);
				(*(double *)(writebuffer + loc)) = crd.x;
				(*(double *)(writebuffer + loc + 8)) = crd.y;
				loc += 16;
			}
			else if ( ct == ColumnInfo::ctCRD3D){
				Coord crd = col->cValue(row);
				(*(double *)(writebuffer + loc)) = crd.x;
				(*(double *)(writebuffer + loc + 8)) = crd.y;
				(*(double *)(writebuffer + loc + 16)) = crd.z;
				loc += 24;
			} else if (  ct == ColumnInfo::ctSTRING){
				 String s = col->sValue(row,0,0);
				 strcpy(writebuffer + loc,s.c_str());
				 loc += s.size() + 1;
			} else if (  ct == ColumnInfo::ctBINARY){
				 BinMemBlock& bm = col->binValue(row);
				 memcpy(writebuffer + loc, bm.ptr(), bm.iSize());
				 loc += bm.iSize();
			}

		}
	}
	//	case ColumnInfo::ctBINARY:
	//		long cnt = 0;
	//		*(long *)p = (long)readCoordList(memblock + posFile, cnt);
	//		posFile += cnt;
	//		break;
	//}

}

void TableStoreIlwis3::load(const FileName& fnODF, const String& prfix){
	String prefix = prfix == "" ? "" : prfix + ":";

	clock_t time1 = clock();
	int count;
	ObjectInfo::ReadElement(S(prefix + "Table"),"Columns",fnODF, count);
	setColCount(count);
	ObjectInfo::ReadElement(S(prefix + "Table"),"Records",fnODF, count);
	setRowCount(count);
	ObjectInfo::ReadElement(S(prefix + "TableStore"), "Data", fnODF, fnData);
	recordSize = 0;
	bool simpleDataTypes = true;

	setColumnInfo(fnODF, simpleDataTypes, getColCount());
	int numStringColumns = 0;
	for(int i=0; i < columnInfo.size(); ++i) {
		columnInfo[i].setNewIndex(i);
		if ( columnInfo[i].dmt() == dmtSTRING ) {
			++numStringColumns;

		}
	}
	//stringBlock = new String[numStringColumns * getRowCount()];
	if ( numStringColumns > 0)
		strings = new MemoryManager<String>(10000);


	ifstream file (fnData.sFullPath().c_str(), ios::in|ios::binary|ios::ate);
	if (!file.is_open())
		return; // throw some error
	long size = file.tellg();
	char *memblock = new char [size];
	if ( memblock == 0)
		throw ErrorObject(TR("Couldnt allocate memory for table"));
	file.seekg (0, ios::beg);
	file.read (memblock, size);
	file.close();

	records = new char [ recordSize * getRowCount()];
	memset(records, 0,recordSize * getRowCount());

	Loaded(true);
	if ( !simpleDataTypes) {
		readData(memblock);
	} else {
		memcpy(records, memblock+128,size - 128);
	}

	long time2 = clock();
	TRACE(String("loaded %S in %f ms\n", fnODF.sRelative(), ((double)time2-time1)/1000.0).c_str());

	delete[] memblock;
}
char *TableStoreIlwis3::readString(long index, char *mem, long& strlen) {
	String *txt = strings->allocate();
	unsigned long pp = (unsigned long)txt;
	///String *txt =   new String();
	char c;
	while((c = *(mem) ) != 0) {
		(*txt)+= c;
		++strlen;
		++mem;
	}
	++strlen; //closing /0

	return (char *)txt;
}

char *TableStoreIlwis3::readCoordList(char *mem, long& count) {
	count = *(long *)mem;
	long noCoords = count / 16;
	CoordinateSequence *seq = new CoordinateArraySequence(noCoords);
	for(int i=0; i < noCoords; ++i) {
		double x = *(double *)(mem + 4 + i * sizeof(double) * 2);
		double y = *(double *)(mem + 4 + sizeof(double) * (i * 2 + 1));
		seq->setAt(Coordinate(x,y), i);
	}
	count +=4;
	return (char *)seq;
}


//void TableStoreIlwis3::getRecord(int row, NewRecord& rc) const{
//}

void TableStoreIlwis3::put(int row, int column, const Coord& crd) {
	int c = columnInfo[column].getNewIndex();
	if ( c == iUNDEF)
		return;
	if ( row >= rowCount || c >= colCount)
		return;
	const ColumnInfo& field = columnInfo.at(c);
	char *p = moveTo(row, column);
	if ( p == 0)
		return;

	if ( field.getColumnType() == ColumnInfo::ctCRD) {
		*(double *)p = crd.x;
		p += sizeof(double);
		*(double *)p = crd.y;
	
	} if ( field.getColumnType() == ColumnInfo::ctCRD3D) {
		p += sizeof(double);
		*(double *) p = crd.z;
	}
}

void TableStoreIlwis3::put(int row, int column, const String& s) {
	int c = columnInfo[column].getNewIndex();
	if ( c == iUNDEF)
		return;
	if ( row >= rowCount || c >= colCount)
		return;
	const ColumnInfo& field = columnInfo.at(c);
	if ( field.getColumnType() == ColumnInfo::ctSTRING) {
		char *p = moveTo(row, column);
		if ( p == 0)
			return;

		String *s2 = new String(s);
		*(long *)p = (long)s2;
	}
}

void  TableStoreIlwis3::put(int row, int column, double v){
	int c = columnInfo[column].getNewIndex();
	if ( c == iUNDEF)
		return;
	if ( row >= rowCount || c >= colCount)
		return;
	const ColumnInfo& field = columnInfo.at(c);
	char *p = moveTo(row, column);
	if ( p == 0)
		return;

	switch(field.getColumnType()) {
		case ColumnInfo::ctRAW:
			*(long *)p = v == rUNDEF ? iUNDEF : (long)v;
			break;
		case ColumnInfo::ctREAL:
			*(double *)p = v;
	}
}

void TableStoreIlwis3::get(int row, int column, double& v ) const {
	int c = columnInfo[column].getNewIndex();
	if ( c == iUNDEF)
		return;
	if ( row >= rowCount || c > columnInfo.size()) {
		v = rUNDEF;
		return;
	}

	const ColumnInfo& field = columnInfo.at(c);
	v = rUNDEF;
	char *p = moveTo(row, c);
	if ( p == 0)
		return;

	switch(field.getColumnType()) {
		case ColumnInfo::ctRAW:{

			long raw = p != 0 ? *(long *) p : iUNDEF;
			v = raw;
							   }
							   break;
		case ColumnInfo::ctREAL:
			v = *(double *)p; break;
	}
}

void TableStoreIlwis3::get(int row, int column, Coord& crd) const {
	int c = columnInfo[column].getNewIndex();
	if ( c == iUNDEF)
		return;
	if ( row >= rowCount || c >= colCount) {
		crd= crdUNDEF;
		return;
	}
	const ColumnInfo& field = columnInfo.at(c);
	StoreType st = field.st();
	char *p = moveTo(row, column);
	if ( p == 0)
		return;

	crd.x = *(double *)p;
	crd.y = *(double *)(p + sizeof(double));
	crd.z =  st == stCRD3D ? *(double *)(p + sizeof(double)*2) : rUNDEF;
}

void TableStoreIlwis3::get(int row, int column, String& s) const {
	int c = columnInfo[column].getNewIndex();
	if ( c == iUNDEF)
		return;
	if ( row >= rowCount || c >= colCount) {
		s = "";
		return;
	}
	char *p = moveTo(row, column);
	if ( p == 0)
		return;
	s = **(String **)p;
}

void TableStoreIlwis3::get(int row, int column, CoordinateSequence **seq) const {
	int c = columnInfo[column].getNewIndex();
	if ( c == iUNDEF)
		return;
	if ( row >= rowCount || c >= colCount)
		return;
	char *p = moveTo(row, column);
	if ( p == 0)
		return;

	CoordinateSequence *s = (CoordinateSequence *)(*(long *)p);
	(*seq) = s;
}

inline char *TableStoreIlwis3::moveTo(int row, int col) const{
	return (char *)(records + row * recordSize + columnInfo[col].getOffset());
}

bool TableStoreIlwis3::fUsesReals(int column) const{
	//int c = columnInfo[column].getNewIndex();
	//if ( c == iUNDEF)
	//	return;
	return columnInfo.at(column).dvrs().fUseReals();
}

void TableStoreIlwis3::sharedValue(int column, bool yesno) {
	columnInfo.at(column).sharedValue(yesno);
}

bool TableStoreIlwis3::isShared(int column) const {
	return columnInfo.at(column).isSharedValue();
}

//-------OLD INTERFACE --------------------------------
long TableStoreIlwis3::iCol(const String& name) const{
	map<String, long>::const_iterator cur = columnsByName.find(name);
	if ( cur != columnsByName.end())
		return (*cur).second;
	return -1; // undef would be more logical, but older code expects -1
}

void TableStoreIlwis3::DeleteRec(long iStartRec, long iRecs){

	if ( strings) {
		for(int i=0; i < columnInfo.size(); ++i) {
			if ( columnInfo[i].dmt() == dmtSTRING && columnInfo[i].getNewIndex() >= 0) {
				for(int j = iStartRec; j < iStartRec + iRecs; ++j) {
					char *p = moveTo(j - 1, i);
					String *s = *(String **)p;
					strings->deallocate(s);				
				}

			}
		}
	}

	unsigned long start = recordSize * (iStartRec - 1); // assume its 1 based
	unsigned long end = start + recordSize * iRecs;
	unsigned long oldSize = recordSize * getRowCount();
	unsigned long newSize = oldSize - ( end - start);
	char * recordsNew = new char [newSize ];
	memset(recordsNew, 0, newSize);
	memcpy(recordsNew,records, start);
	memcpy(recordsNew + start, records + end, oldSize - end);
	delete [] records;
	records = recordsNew;

	setRowCount(getRowCount() - iRecs);
	ptr.fChanged = true;
	TableStore::DeleteRec(iStartRec, iRecs);
}

long TableStoreIlwis3::iRecNew(long iRecs ){
	TableStore::iRecNew(iRecs);

	unsigned long oldSize = recordSize * getRowCount();
	unsigned long sz = recordSize * (getRowCount() + 1);
	char * recordsNew = new char [sz ];
	memset(recordsNew,0,sz);
	memcpy(recordsNew, records, oldSize);
	delete [] records;
	records = recordsNew;
	setRowCount(getRowCount() + iRecs);
	ptr.fChanged = true;

	return getRowCount();
}

//const Column& TableStoreIlwis3::colNew(const String& scol, const Domain& dm, const ValueRange& vr){
//	return ColumnPtr::create(Table(ptr.fnObj),scol,DomainValueRangeStruct(dm,vr));
//}
//
//const Column& TableStoreIlwis3::colNew(const String&, const DomainValueRangeStruct& dvs){
//	return ColumnPtr::create(Table(ptr.fnObj),scol,dvs);
//}


//void  TableStoreIlwis3::RemoveCol(Column&){
//}

bool TableStoreIlwis3::fWriteAvailable() const{
	return true;
}
void TableStoreIlwis3::DoNotUpdate(){
	TableStore::DoNotUpdate();
}

void TableStoreIlwis3::Loaded(bool fValue){
	TableStore::Loaded(fValue);
}

long TableStoreIlwis3::getRowCount() const {
	return rowCount;
}

long TableStoreIlwis3::getColCount() const {
	return colCount;
}

inline long TableStoreIlwis3::index(const String& colName) const {
	map<String, long>::const_iterator cur;
	if ( ( cur = columnsByName.find(colName)) != columnsByName.end())
		return (*cur).second;
	return iUNDEF;
}

void TableStoreIlwis3::setRowCount(long r) {
	rowCount = r;
}

void TableStoreIlwis3::setColCount(long c) {
	colCount = c;
	//offsets.resize(c);
	//for(int i = 0; i < c; ++i)
	//	offsets[i] = i;
}

void TableStoreIlwis3::rename(int colIndex, const String& colName){
	for(map<String,long>::iterator cur = columnsByName.begin(); cur != columnsByName.end(); ++cur) {
		if ( (*cur).second == colIndex) {
			columnsByName.erase((*cur).first);
			break;
		}

	}
	columnsByName[colName] = colIndex;
}

void  TableStoreIlwis3::RemoveCol(Column& c) {
	TableStore::RemoveCol(c);
	for(int i = 0; i < columnInfo.size(); ++i) {
		if ( columnInfo[i].sName() == c->sName()) {
			ColumnInfo& inf = columnInfo[i];
			inf.setDeleted(true);
			break;

		}
	}
	int index = columnsByName[c->sName()];
	columnsByName.erase(c->sName());


	for(map<String,long>::iterator cur = columnsByName.begin(); cur != columnsByName.end(); ++cur) {
		if ( (*cur).second > index) {
			(*cur).second--;

			break;
		}

	}	
	colCount--;
}

const Column& TableStoreIlwis3::AddCol(const Column& c) {
	const Column& col = TableStore::AddCol(c);

	
	return col;
}







