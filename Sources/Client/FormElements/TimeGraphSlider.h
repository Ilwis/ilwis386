#pragma once

class TimeGraphSlider;

#define ID_TIME_TICK WM_USER+345
#define ID_NOTIFY_ME WM_USER+346

class TimeGraph : public CStatic, public BaseZapp {
public:
   TimeGraph(TimeGraphSlider *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
   void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   void OnLButtonUp(UINT nFlags, CPoint point);
   void setIndex(int index);
   int OnToolHitTest(CPoint point, TOOLINFO *pTI) const;
	void OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);
	void PreSubclassWindow() ;
	void setThreshold(double v, bool above = true);
	double getThreshold() const;
	void setLinkedWindow(CWnd *);

   DECLARE_MESSAGE_MAP();
private:
	TimeGraphSlider *fldGraph;
	CPoint timePoint;
	CToolTipCtrl* toolTip;
	double threshold;
	bool isAbove;
	CWnd *toBeNotified;
	vector<bool> marked;;
};

class _export TimeGraphSlider : public FormEntry {
	friend class TimeGraph;
public:
	TimeGraphSlider(FormEntry* par, RangeInt defaultRange);
	void setSourceTable(const Table& tbl);
	void setSourceColumn(const String& sName);
	void setRecordRange(const RangeInt& rng);
	void create();
	void setIndex(int index);
	void setTimeInterval(ILWIS::TimeInterval in);
	void setTimes(const Column& col);
	TimeGraph *timegraph;
	void setThreshold(double v, bool above = true);
	double getThreshold() const;
	void setLinkedWindow(CWnd *);
private:

	Table sourceTable;
	RangeInt recordRange;
	Column timeCol;
	ILWIS::TimeInterval interval;
	String sourceColumn;
};