#pragma once

class HovMollerGraphEntry;

#define ILWM_UPDATE_ANIM (WM_APP+232)

namespace ILWIS {
class HovMollerTool;
const DrawingColor;
}

struct LocInfo {
	LocInfo(const Coord& c=Coord(), double d=rUNDEF) : crd(c), dist(d), marker(false) {}
	Coord crd;
	double dist;
	bool marker;
};

struct GraphInfo {
	GraphInfo(long ind, double v=rUNDEF,const Coord& c=Coord()) : index(ind), value(v), crd(c) {}
	double value;
	Coord crd;
	long index;
};

class HovMollerGraph : public CStatic, public BaseZapp {
	friend class HovMollerGraphEntry;
public:
   HovMollerGraph(HovMollerGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
   void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   void OnLButtonUp(UINT nFlags, CPoint point);
   void setIndex(int index);
   int OnToolHitTest(CPoint point, TOOLINFO *pTI) const;
	void OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);
	void PreSubclassWindow() ;
	void setTrack( const vector<Coord>& crds);
	void setTimeColumn(const String& colName);

   DECLARE_MESSAGE_MAP();
private:
	void setCoordText(int index, const Map& mp, CDC *dc);
	void setTimeText(int index, CDC *dc);
	void setTextDataBlock(CDC *dc);
	HovMollerGraphEntry *fldGraph;
	vector<vector<double> > values;
	vector<RowCol> track;
	vector<Coord> trackCrd; // this is a reduced version of the original track. every coord matches a record in track
	CToolTipCtrl* toolTip;
	Column timeColumn;
	CRect rctInner;
	int xIndex;
	int yIndex;
};

class _export HovMollerGraphEntry : public FormEntry {
	friend class HovMollerGraph;
public:
	HovMollerGraphEntry(FormEntry* par, ILWIS::HovMollerTool *t);
	void setSource(const MapList& obj);
	void setRecordRange(const RangeInt& rng);
	void create();
	void setTrack( const vector<Coord>& crds);
	//RangeReal getRange(long i);
	void setTimeColumn(const String& colName);
//	void setIndex(int sourceIndex, double value, const Coord& crd);
	ILWIS::HovMollerTool *tool;
	void update();

private:
	int currentIndex;
	MapList mpl;
	HovMollerGraph *graph;
};