#include "Headers\Toolspch.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\COLINFO.H"
#include "Engine\Table\TableStoreIlwis3.h"

using namespace ILWIS;
#define S(a) String(a).scVal()

TableStoreIlwis3::TableStoreIlwis3() : records(0){
}
TableStoreIlwis3::~TableStoreIlwis3(){
	for(long r = 0; r < getRowCount(); ++r) {
		for(long c = 0; c < getColCount(); ++c) {
			const ColumnInfo& info = columnInfo.at(c);
			char *p = records + r * recordSize + info.getOffset();
			if ( info.getColumnType() == ColumnInfo::ctBINARY){
				delete [] p; 
			}
			else if	( info.getColumnType() == ColumnInfo::ctSTRING) {
				delete p;
			}
		}
	}
	delete [] records;
}

void TableStoreIlwis3::setColumnInfo(const FileName& fnODF, bool& simpleDataTypes) {
	columnInfo.resize(getColCount());
	for(int col = 0 ; col < getColCount(); ++col) {
		columnInfo.at(col).Read(fnODF,col);
		columnInfo.at(col).setOffset(recordSize);
		columnInfo.at(col).setIndex(col);
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
	}
	for(int i =0; i < getColCount(); ++i) {
		columnsByName[columnInfo.at(i).sName()] = i;
	}
}

void TableStoreIlwis3::readData(char *memblock) {
	long posFile = 128;
	long posMem = 0;

	for(long r = 0; r < getRowCount(); ++r) {
		for(long c = 0; c < getColCount(); ++c) {
			const ColumnInfo& info = columnInfo.at(c);
			char *p = records + r * recordSize + info.getOffset();
			switch(info.getColumnType()) {
				case ColumnInfo::ctRAW:
					*p = *(long *)(memblock + posFile);
					posFile += 4;
					break;
				case ColumnInfo::ctREAL:
					*p = *(double *)(memblock + posFile);
					posFile += 8;
					break;
				case ColumnInfo::ctCRD:
					memcpy(p,memblock + posFile,16);
					posFile += 16;
					break;
				case ColumnInfo::ctCRD3D:
					memcpy(p,memblock + posFile,24);
					posFile += 24;
					break;
				case ColumnInfo::ctSTRING:
					*p = *(long *)readString(memblock + posFile);
					posFile+= ((String *)p)->size() + 1;
					break;
				case ColumnInfo::ctBINARY:
					long cnt = 0;
					*p = *(long *)readCoordList(memblock + posFile, cnt);
					posFile += cnt;
					break;
			}
		}
	}
}

void TableStoreIlwis3::load() {
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

	setColumnInfo(fnODF, simpleDataTypes);

	ifstream file (fnData.sFullPath().scVal(), ios::in|ios::binary|ios::ate);
	if (!file.is_open())
		return; // throw some error
	long size = file.tellg();
	char *memblock = new char [size];
	file.seekg (0, ios::beg);
	file.read (memblock, size);
	file.close();

	records = new char [ recordSize * getRowCount()];
	memset(records, 0,recordSize * getRowCount());

	if ( !simpleDataTypes) {
		readData(memblock);
	} else {
		memcpy(records, memblock+128,size - 128);
	}

	long time2 = clock();
	TRACE(String("loaded %S in %f ms\n", fnODF.sRelative(), ((double)time2-time1)/1000.0).scVal());

	delete[] memblock;
}
String *TableStoreIlwis3::readString(char *mem) {
	char c;
	String *txt = new String();
	while((c = *(mem) ) != 0) {
		(*txt)+= c;
		++mem;
	}
	return txt;
}

char *TableStoreIlwis3::readCoordList(char *mem, long& count) {
	long sz = *(long *)mem;
	char *block = new char [sz];
	memcpy(block,mem+4,sz);
	return block;
}


void TableStoreIlwis3::store(){
}
//void TableStoreIlwis3::getRecord(int row, NewRecord& rc) const{
//}

void TableStoreIlwis3::get(int row, int column, double& v ) const {
	const ColumnInfo& field = columnInfo.at(column);
	v = rUNDEF;
	char *p = moveTo(row, column, field);
	switch(field.getColumnType()) {
		case ColumnInfo::ctRAW:{
				long raw = *(long *) p;
				if ( raw == 0)
					v = rUNDEF;
				double rVal = raw + field.getValueOffset();
				v = rVal * field.getStep();
		    }
		    break;
		case ColumnInfo::ctREAL:
			v = *(double *)p; break;
	}
}

void TableStoreIlwis3::get(int row, int column, Coord& c) const {
	const ColumnInfo& field = columnInfo.at(column);
	char *p = moveTo(row, column, field);
	c.x = *(double *)p;
	c.y = *(double *)p + sizeof(double);
	c.z = *(double *)p + sizeof(double)*2;
}

void TableStoreIlwis3::get(int row, int column, String& s) const {
	const  ColumnInfo& field = columnInfo.at(column);
	char *p = moveTo(row, column, field);
	s = **(String **)p;
}

inline char *TableStoreIlwis3::moveTo(int row, int column, const  ColumnInfo& fld) const{
	return (char *)(records + row * recordSize + fld.getOffset());
}



