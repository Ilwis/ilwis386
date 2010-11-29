#pragma once

#include "Engine\Table\NewTableStore.h"

namespace ILWIS {

	class TableStoreIlwis3 : public NewTableStore  {
	public:
		TableStoreIlwis3();
		~TableStoreIlwis3();
		virtual void load(const FileName& fnODF, const String& prfix);
		virtual void load();
		virtual void store();
		//virtual void getRecord(int row, NewRecord& rc) const;
		virtual void get(int row, int column, double& v ) const;
		virtual void get(int row, int column, Coord& c) const;
		virtual void get(int row, int column, String& s) const;
	private:
		char *moveTo(int row, int column, const  ColumnInfo& fld) const;
		String *readString(char *mem);
		char *readCoordList(char *mem, long& count);
		void setColumnInfo(const FileName& fnODF, bool& simpleDataTypes);
		void readData(char *memblock);
		char * records;
		int recordSize;
		vector<ColumnInfo> columnInfo;
		FileName fnData;
		ObjectTime timStore;
	};
}