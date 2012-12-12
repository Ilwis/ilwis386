#pragma once

struct RowSelect{
	RowSelect(long y1, long x1, long x2) : row(y1), startCol(x1), endCol(x2) {}
	RowSelect() : startCol(0), endCol(0), row(1) {}
	long startCol;
	long endCol;
	long row;
};


class TableSelection {
public:
	TableSelection();
	void setSize(const RowCol& tblSize);
	bool fContains(const RowCol& rc) const;
	bool fContainsRow(long row) const;
	void selectBlock(const RowCol& bottom, const RowCol& top);
	void selectRows(const vector<long>& selrows);
	bool fValid() const;
	void setMinCol(long c);
	void setMaxCol(long c);
	void setMinRow(long r);
	void setMaxRow(long r);
	long minCol() const;
	long maxCol() const;
	long minRow() const;
	long maxRow() const;
	void reset();
	MinMax mm() const;
	TableSelection& operator+=(const TableSelection& sel2);
	const vector<bool>& getRows() const { return  rows; }
	long findRow(long row) const;
private:
	MinMax boundary; // this is the maximum extent of the selection; it doesnt need to be continous
	vector<bool> rows;
	vector<bool> cols;
};

