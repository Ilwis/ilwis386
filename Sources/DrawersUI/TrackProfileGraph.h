#pragma once

class TrackProfileGraphEntry;

#define ILWM_UPDATE_ANIM (WM_APP+232)

namespace ILWIS {
class TrackProfileTool;
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

class TrackProfileGraph : public CStatic, public BaseZapp {
	friend class TrackProfileGraphEntry;
public:
   TrackProfileGraph(TrackProfileGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
   void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   void OnLButtonDown(UINT nFlags, CPoint point);
   void OnMouseMove(UINT nFlags, CPoint point);
   void OnLButtonUp(UINT nFlags, CPoint point);
   void setIndex(int index);
   int OnToolHitTest(CPoint point, TOOLINFO *pTI) const;
	void OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);
	void PreSubclassWindow() ;
	void setTrack( const vector<Coord>& crds);
	void OnContextMenu(CWnd* pWnd, CPoint point) ;

   DECLARE_MESSAGE_MAP();
private:
	void DrawMarker(int xposOld, int xpos, CRect & rect);
	BaseMap getBaseMap(long i, long m);
	int getNumberOfMaps(long i);
	LRESULT tick(WPARAM wp, LPARAM lp);
	void saveAsCsv();
	void saveAsTbl();
	RangeReal getRange(int i) const;
	Domain getDomain(int i) const;
	const DomainValueRangeStruct & getDvrs(int i) const;
	double getValue(int i, const BaseMap& bmp, const Coord& crd) const;
	Color getColor(int i, const BaseMap&bmp, long iRaw) const;
	ILWIS::LayerDrawer *getLayerDrawer(ILWIS::NewDrawer *cdr) const ;

	TrackProfileGraphEntry *fldGraph;
	vector<vector<GraphInfo> > values;
	vector<LocInfo> track;
	CToolTipCtrl* toolTip;
	bool yStretch;
	bool fDown;
	int markerXposOld;
};

class _export TrackProfileGraphEntry : public FormEntry {
	friend class TrackProfileGraph;
public:
	TrackProfileGraphEntry(FormEntry* par, ILWIS::TrackProfileTool *t);
	void addSource(const IlwisObject& obj);
	void setRecordRange(const RangeInt& rng);
	void create();
	void setTrack( const vector<Coord>& crds);
	void setYStretch(bool stretch);
	void setListView(FieldListView *v);
	RangeReal getRange(long i);
	void setIndex(int sourceIndex, double value, const Coord& crd);
	ILWIS::TrackProfileTool *tool;
	void setOverruleRange(int col, int row, const String& value);
	void update();
	void openAsTable();
	void onContextMenu(CWnd* pWnd, CPoint point);

private:
	FieldListView *listview;
	int currentIndex;
	TrackProfileGraph *graph;
};