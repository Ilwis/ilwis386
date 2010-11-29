#pragma once;

namespace ILWIS {

	class NewTableStore  {
	public:
		NewTableStore();
		virtual ~NewTableStore() {};
		virtual void store()=0;
		virtual void load()=0;
		virtual void get(int row, int column, double& v ) const=0;
		virtual void get(int row, int column, Coord& c) const=0;
		virtual void get(int row, int column, String& s) const=0;
		virtual long index(const String& colName) const;
		long getRowCount() const;
		long getColCount() const;
	protected:
		void setRowCount(long r);
		void setColCount(long c);
		map<String, long> columnsByName;
	private:
		int rowCount;
		int colCount;


	};
}