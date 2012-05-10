struct sqlite3;

namespace ILWIS {
	struct QValue {
		QValue() { pResult = 0; }
		String columnName;
		String sResult;
		void *pResult;
	};

	typedef vector<QValue> ResultRow;
	//typedef vector<ResultRow> QueryResults; 

	class QueryResults : public vector<ResultRow> 
	{
		public:
			int column(const String& name);
			String get(const String& colName, int rec);
		private:
			map<String, int> colMapping;
	};

	class Database {
	public:
		Database(const String& _location, const String& _id);
		virtual ~Database();
		String getId() const;
		String getLocation() const;
		virtual bool executeQuery(const String& query, QueryResults& results, vector<String>& errors=vector<String>()) = 0;
		virtual bool executeStatement(const String& statement)=0;
		virtual bool isValid() const;
		static Database * create(const String& type, const String& name, const String& id);
		virtual bool open_connection() = 0;
		virtual bool close_connection() { return false;}
	protected:
		bool valid;

	private:
		String id;
		String location;
	};

	class SpatialLite : public Database {
	public:
		SpatialLite(const String& location, const String& id);
		bool executeQuery(const String& query, QueryResults& results, vector<String>& errors=vector<String>());
		bool executeStatement(const String& statement);
		bool isValid() const;
		virtual bool open_connection() ;
		virtual bool close_connection();
	private:
		sqlite3* db;

	};
}