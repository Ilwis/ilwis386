#include "Headers\Toolspch.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\COLINFO.H"
#include "Engine\Table\TableStoreIlwis3.h"

using namespace ILWIS;
#define S(a) String(a).c_str()

TableStoreIlwis3::TableStoreIlwis3() : records(0){
}
TableStoreIlwis3::~TableStoreIlwis3(){
	for(long r = 0; r < getRowCount(); ++r) {
		for(long c = 0; c < getColCount(); ++c) {
			const ColumnInfo& info = columnInfo.at(c);
			char *p = records + r * recordSize + info.getOffset();
			if ( info.getColumnType() == ColumnInfo::ctBINARY && info.isSharedValue() == false){
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
	double c33 ; long t33; double v33;
	for(long r = 0; r < getRowCount(); ++r) {
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
					*(long *)p = *(long *)readString(memblock + posFile);
					posFile+= ((String *)p)->size() + 1;
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

void TableStoreIlwis3::load() {
}

bool TableStoreIlwis3::load(const FileName& fnODF, const String& prfix){
	String prefix = prfix == "" ? "" : prfix + ":";
	ObjectInfo::ReadElement(S(prefix + "TableStore"), "Data", fnODF, fnData);
	ifstream file (fnData.sFullPath().c_str(), ios::in|ios::binary|ios::ate);
	if (!file.is_open())
		return false; // no binary file thus no table store or corrupt / not yet calculated

	clock_t time1 = clock();
	int count;
	ObjectInfo::ReadElement(S(prefix + "Table"),"Columns",fnODF, count);
	setColCount(count != shUNDEF ? count : 0);
	ObjectInfo::ReadElement(S(prefix + "Table"),"Records",fnODF, count);
	setRowCount(count != shUNDEF ? count : 0);
	recordSize = 0;
	bool simpleDataTypes = true;

	setColumnInfo(fnODF, simpleDataTypes);

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
	TRACE(String("loaded %S in %f ms\n", fnODF.sRelative(), ((double)time2-time1)/1000.0).c_str());

	delete[] memblock;
	return true;
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
	count = *(long *)mem;
	long noCoords = count / 16;
	CoordinateSequence *seq = new CoordinateArraySequence(noCoords);
	for(int i=0; i < noCoords; ++i) {
		double x = *(double *)(mem + 4 + i * sizeof(double) * 2);
		double y = *(double *)(mem + 4 + sizeof(double) * (i * 2 + 1));
		seq->setAt(Coordinate(x,y), i);
	}
	//*(long *)block = count;
	//memcpy(block+4,mem+4,count);
	count +=4;
	return (char *)seq;
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
				
			long raw = p != 0 ? *(long *) p : iUNDEF;
				//if ( raw == 0)
				//	v = rUNDEF;
				//double rVal = raw + field.getValueOffset();
				//v = rVal * field.getStep();
				v = raw;
		    }
		    break;
		case ColumnInfo::ctREAL:
			v = *(double *)p; break;
	}
}

void TableStoreIlwis3::get(int row, int column, Coord& c) const {
	const ColumnInfo& field = columnInfo.at(column);
	StoreType st = field.st();
	char *p = moveTo(row, column, field);
	c.x = *(double *)p;
	c.y = *(double *)(p + sizeof(double));
	c.z =  st == stCRD3D ? *(double *)(p + sizeof(double)*2) : rUNDEF;
}

void TableStoreIlwis3::get(int row, int column, String& s) const {
	const  ColumnInfo& field = columnInfo.at(column);
	char *p = moveTo(row, column, field);
	s = **(String **)p;
}

void TableStoreIlwis3::get(int row, int column, CoordinateSequence **seq) const {
	const ColumnInfo& field = columnInfo.at(column);
	char *p = moveTo(row, column, field);
	double *a1 = (double *)(records);
	double *a2 = (double *)(records+8);
	double *a3 = (double *)(records+16);
	double *a4 = (double *)(records+24);
	double *a5 = (double *)(records+32);
	CoordinateSequence *s = (CoordinateSequence *)(*(long *)p);
	(*seq) = s;

/*	long size = *(long *)block;
	double x = *(double *)(block + sizeof(long));
	double y = *(double *)(block + sizeof(long) + sizeof(double));
	long noCoords = size / 16;
	(*seq) = new CoordinateArraySequence(noCoords);
	for(int i=0; i < noCoords; ++i) {
		Coord c(
		(*seq)->getAt(*(double *)(block));
	}*/
	

}

inline char *TableStoreIlwis3::moveTo(int row, int column, const  ColumnInfo& fld) const{
	return (char *)(records + row * recordSize + fld.getOffset());
}

bool TableStoreIlwis3::fUsesReals(int column) const{
	return columnInfo.at(column).dvrs().fUseReals();
}

void TableStoreIlwis3::sharedValue(int column, bool yesno) {
	columnInfo.at(column).sharedValue(yesno);
}

bool TableStoreIlwis3::isShared(int column) const {
	return columnInfo.at(column).isSharedValue();
}







