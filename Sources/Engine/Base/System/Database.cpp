#include "toolspch.h"
#include "SpatialDB\headers\spatialite\sqlite3.h"
#include "Engine\Base\System\Database.h"

using namespace ILWIS;

int QueryResults::column(const String& name) {
	if ( size() == 0)
		return iUNDEF;
	if ( at(0).size() == 0)
		return iUNDEF;
	if (colMapping.size() == 0) {
		for(int i =0; i < at(0).size(); ++i)
			colMapping[at(0)[i].columnName] = i;
	}

	map<String,int>::const_iterator cur = colMapping.find(name);
	if ( cur != colMapping.end())
		return (*cur).second;

	return iUNDEF;
}

String QueryResults::get(const String& colName, int rec) {
	int col = column(colName);
	if ( col != iUNDEF) {
		if ( rec < size()) {
			return at(rec)[col].sResult;
		}
	}
	return sUNDEF;

}
//-------------------------------------------
Database::Database(const String& _location, const String& _id) : location(_location), id(_id),valid(false) {
}

Database::~Database() {
	close_connection();
}

String Database::getId() const {
	return id;
}

String Database::getLocation() const {
	return location;
}

Database *Database::create(const String& type, const String& location, const String& id) {
	return new SpatialLite(location, id); // more in the future
}

bool Database::isValid() const {
	return valid;
}

//-----------------------------------------------
int callback(void *retData, int noOfColumns, char **argv, char **azColName){
	QueryResults *qr = (QueryResults *)retData;
	if ( noOfColumns > 0) {
		ResultRow data(noOfColumns);
		for(int i =0; i < noOfColumns; ++i) {
			String sc(azColName[i]);
			String cv(argv[i]);
			data[i].columnName = sc;
			data[i].sResult = cv;

		}
		qr->push_back(data);
	}

	return 0;
}

SpatialLite::SpatialLite(const String& _location, const String& _id) : Database(_location, _id) {
	valid = open_connection();
}

bool SpatialLite::open_connection() {
	return sqlite3_open_v2(getLocation().c_str(), &db,SQLITE_OPEN_FULLMUTEX,0) == SQLITE_OK;
}

bool SpatialLite::close_connection() {
	return sqlite3_close(db) == SQLITE_OK;

}
bool SpatialLite::executeQuery(const String& query, QueryResults& results, vector<String>& errors){
	if (!isValid())
		return false;
	results.clear();

	char* db_err = 0;

	bool isOK = sqlite3_exec(db, query.c_str(), callback, &results, &db_err) == SQLITE_OK;
	if ( db_err) {
		String err(db_err);
		MessageBox(0,err.c_str(), "Bah", MB_OK);
		sqlite3_free(db_err);
	}

	return  isOK;
}

bool SpatialLite::executeStatement(const String& statement){
	if (!isValid())
		return false;

	char* db_err = 0;
	bool isOK = sqlite3_exec(db, statement.c_str(), NULL, 0, &db_err) == SQLITE_OK;
	if ( db_err) {
		String err(db_err);
		sqlite3_free(db_err);
	}
	return isOK;
}

bool SpatialLite::isValid() const{
	return Database::isValid(); 
}



