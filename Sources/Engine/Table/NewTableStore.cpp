#include "Headers\Toolspch.h"
#include "Engine\Table\NewTableStore.h"

using namespace ILWIS;

NewTableStore::NewTableStore() : rowCount(0), colCount(0) {
}

long NewTableStore::getRowCount() const {
	return rowCount;
}

long NewTableStore::getColCount() const {
	return colCount;
}

inline long NewTableStore::index(const String& colName) const {
	map<String, long>::const_iterator cur;
	if ( ( cur = columnsByName.find(colName)) != columnsByName.end())
		return (*cur).second;
	return iUNDEF;
}

void NewTableStore::setRowCount(long r) {
	rowCount = r;
}

void NewTableStore::setColCount(long c) {
	colCount = c;
}
