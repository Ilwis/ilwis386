#include "Headers\toolspch.h"
#include "TableSelection.h"

TableSelection::TableSelection() {
}

void TableSelection::setSize(const RowCol& tblSize) {
	if ( tblSize.Row != rows.size() || tblSize.Col != cols.size()) {
		rows.resize(tblSize.Row ,false);
		cols.resize(tblSize.Col, false);
	}
}

bool TableSelection::fContains(const RowCol& rc) const {
	if (!boundary.fContains(rc))
		return false;
	return rows[rc.Row - 1] && cols[rc.Col];
}

bool TableSelection::fContainsRow(long row) const{
	return rows[row - 1];
}

void TableSelection::selectRows(const vector<long>& selrows) {
	if (selrows.size() > 0) {
		long maxrow = -1;
		long minrow = 1000000000;

		for(long x = 0; x < cols.size(); ++x) {
			cols[x] = true;
		}

		for(int i = 0 ; i < selrows.size(); ++i) {
			long row = selrows[i];
			if ( row < rows.size() && row != iUNDEF) {
				rows[row] = true;
				maxrow = max(row + 1, maxrow);
				minrow = min(row + 1, minrow);
			}
		}
		boundary = MinMax(RowCol(minrow,0L), RowCol(maxrow, cols.size() -1L));
	} else
		reset();
}

void TableSelection::selectBlock(const RowCol& bottom, const RowCol& top) {

	if ( bottom.Row == 1 && top.Row == 0 && bottom.Col != iUNDEF) { // special case. empty table with rows, used for copying
		boundary = MinMax(RowCol(0L,bottom.Col), RowCol(0L,bottom.Col));
		cols[bottom.Col] = true;
		return;

	} else if ( top.Col >= cols.size() || (top.Row - 1) >= rows.size()) 
		return;

	boundary = MinMax(bottom, top);
	if ( boundary.fUndef()) {
		return;
	}
	for(long x = 0; x < cols.size(); ++x) {
		cols[x] = (x >= bottom.Col && x <= top.Col);
	}

	for(long y = 0; y < rows.size(); ++y)
		rows[y] = (y >= (bottom.Row - 1) && y <= (top.Row - 1));

}

bool TableSelection::fValid() const {
	return rows.size() != 0 && cols.size() != 0;
}

void TableSelection::setMinCol(long c) {

	if ( c >= cols.size() || c < 0)
		return;
	if (boundary.MinCol() == iUNDEF || boundary.MinCol() == -iUNDEF) {
		for (long x = 0; x < c; ++x)
			cols[x] = false;
		cols[c] = true;
	} else if (c < boundary.MinCol()) {
		for (long x = c; x < boundary.MinCol(); ++x)
			cols[x] = true;
	} else {
		for (long x = boundary.MinCol(); x < c; ++x)
			cols[x] = false;
	}
	boundary.MinCol() = c;
}

void TableSelection::setMaxCol(long c) {

	if ( c >= cols.size() || c < 0)
		return;

	if (boundary.MaxCol() == iUNDEF) {
		for (long x = c + 1; x < cols.size(); ++x)
			cols[x] = false;
		cols[c] = true;
	} else if (c < boundary.MaxCol()) {
		for (long x = c + 1; x <= boundary.MaxCol(); ++x)
			cols[x] = false;
	} else {
		for (long x = boundary.MaxCol() + 1; x <= c; ++x)
			cols[x] = true;
	}
	boundary.MaxCol() = c;
}

void TableSelection::setMinRow(long r) {

	if ( r > rows.size() || r < 1)
		return;
	if (boundary.MinRow() == iUNDEF || boundary.MinRow() == -iUNDEF) {
		for (long y = 0; y < r - 1; ++y)
			rows[y] = false;
		rows[r - 1] = true;
	} else if (r < boundary.MinRow()) {
		for (long y = r - 1; y < boundary.MinRow() - 1; ++y)
			rows[y] = true;
	} else {
		for (long y = boundary.MinRow() - 1; y < r - 1; ++y)
			rows[y] = false;
	}
	boundary.MinRow() = r;
}

void TableSelection::setMaxRow(long r) {

	if ( r > rows.size() || r < 1)
		return;
	if (boundary.MaxRow() == iUNDEF) {
		for (long y = r; y < rows.size() - 1; ++y)
			rows[y] = false;
		rows[r - 1] = true;
	} else if (r < boundary.MaxRow()) {
		for (long y = r; y < boundary.MaxRow(); ++y)
			rows[y] = false;
	} else {
		for (long y = boundary.MaxRow(); y < r; ++y)
			rows[y] = true;
	}
	boundary.MaxRow() = r;
}

long TableSelection::minCol() const {
	return boundary.MinCol();
}

long TableSelection::maxCol() const {
	return boundary.MaxCol();
}

long TableSelection::minRow() const {
	return boundary.MinRow();
}

long TableSelection::maxRow() const {
	return boundary.MaxRow();
}

void TableSelection::reset() {

	boundary = MinMax();
	for(long x = 0; x < cols.size(); ++x)
		cols[x] = false;

	for(long y = 0; y < rows.size(); ++y)
		rows[y] = false;
}

MinMax TableSelection::mm() const {
	return boundary;
}

TableSelection& TableSelection::operator+=(const TableSelection& sel) {
	
	if ( sel.cols.size() != cols.size() || sel.rows.size() != rows.size())
		return *this;

	boundary += sel.boundary;

	for(long x = 0; x < cols.size(); ++x)
		cols[x] = sel.cols[x] || cols[x];

	for(long y = 0; y < rows.size(); ++y)
		rows[y] = sel.rows[y] || rows[y];	
	return *this;
}

long TableSelection::findRow(long r) const{
	int count = 0;
	for(int i = 0; i < rows.size(); ++i) {
		if ( rows[i]) {
			if ( r == count)
				return i;
			++count;
		}

	}
	return -1;
}

