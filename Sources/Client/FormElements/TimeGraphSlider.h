#pragma once

class TimeGraphSlider;

#define ID_TIME_TICK WM_USER+345

class TimeGraph : public CStatic, public BaseZapp {
public:
   TimeGraph(TimeGraphSlider *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
   void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   void OnLButtonUp(UINT nFlags, CPoint point);
   void setIndex(int index);

   DECLARE_MESSAGE_MAP();
private:
	TimeGraphSlider *fldGraph;
	CPoint timePoint;

};

class _export TimeGraphSlider : public FormEntry {
	friend class TimeGraph;
public:
	TimeGraphSlider(FormEntry* par);
	void setSourceTable(const Table& tbl);
	void setSourceColumn(const String& sName);
	void setRecordRange(const RangeInt& rng);
	void create();
	void setIndex(int index);
private:
	TimeGraph *timegraph;
	Table sourceTable;
	RangeInt recordRange;
	String sourceColumn;
};