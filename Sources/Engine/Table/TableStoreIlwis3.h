#pragma once

#include "Engine\Table\NewTableStore.h"


	class _export TableStoreIlwis3 : public TableStore  {
	public:
		TableStoreIlwis3();
		TableStoreIlwis3(const FileName& fn, TablePtr& p);
		~TableStoreIlwis3();
		virtual void load(const FileName& fnODF, const String& prfix);
		virtual void store(const FileName& fnODF, const String& prfix);
		//virtual void getRecord(int row, NewRecord& rc) const;
		virtual void get(int row, int column, double& v ) const;
		virtual void get(int row, int column, Coord& c) const;
		virtual void get(int row, int column, String& s) const;
		virtual void get(int row, int column, CoordinateSequence **seq) const;
		virtual void put(int row, int column, double v);
		virtual void put(int row, int column, const String& s);
		virtual void put(int row, int column, const Coord& c);
		virtual bool fUsesReals(int column) const;
		virtual void sharedValue(int column, bool yesno);
		virtual bool isShared(int col) const;
		virtual long index(const String& colName) const;
		long getRowCount() const;
		long getColCount() const;
		void rename(int colIndex, const String& colName);
		void Store();

		//old interface
		long iCol(const String& sName) const;
		void DeleteRec(long iStartRec, long iRecs=1);
		long iRecNew(long iRecs = 1);
	//	const Column &colNew(const String&, const Domain&, const ValueRange& vr);
	//	const Column &colNew(const String&, const DomainValueRangeStruct& dvs);
		const Column& AddCol(const Column&);
		void  RemoveCol(Column&);
		virtual bool fWriteAvailable() const;
		virtual void DoNotUpdate();
		void Loaded(bool fValue);

	private:
		void setRowCount(long r);
		void setColCount(long c);
		map<String, long> columnsByName;
		void calcStoreSizeNeeded(vector<unsigned long>& recordSize) const;
		void fillBuffer(char *writebuffer, unsigned long startRow, unsigned long rows) ;

		int rowCount;
		int colCount;
		char *moveTo(int row, int col) const;
		char *readString(long index, char *mem,long& strlen);
		char *readCoordList(char *mem, long& count);
		void setColumnInfo(const FileName& fnODF, bool& simpleDataTypes, int cnt);
		void readData(char *memblock);
		void writeData(ofstream& file,int r, int c);
		char * records;
		int recordSize;
		vector<ColumnInfo> columnInfo;
		//vector<long> offsets;
		FileName fnData;
		ObjectTime timStore;
		MemoryManager<String> *strings;
		//char *writebuffer;
	};
