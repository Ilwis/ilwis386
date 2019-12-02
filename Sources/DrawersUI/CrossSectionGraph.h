#pragma once

class CrossSectionGraphEntry;

//#define ID_TIME_TICK WM_USER+345

namespace ILWIS {
class CrossSectionTool;
}

class CrossSectionGraph : public CStatic, public BaseZapp {
	friend class CrossSectionGraphEntry;
public:
   CrossSectionGraph(CrossSectionGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
   void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   void OnLButtonDown(UINT nFlags, CPoint point);
   void OnMouseMove(UINT nFlags, CPoint point);
   void OnLButtonUp(UINT nFlags, CPoint point);
   void setIndex(int index);
   void recomputeValues();
	void PreSubclassWindow();
	void OnContextMenu(CWnd* pWnd, CPoint point) ;
		void saveAsTbl();
		void saveAsSpectrum();

   DECLARE_MESSAGE_MAP();
private:
	void DrawMarker(int xposOld, int xpos);
	BaseMap getBaseMap(long i, long m);
	int getNumberOfMaps(long i);
	CrossSectionGraphEntry *fldGraph;
	vector<vector<vector<double> > >values;
	CToolTipCtrl* toolTip;
	CRect rct;
	bool yStretch;
	bool fDown;
	int markerXposOld;
};

class _export CrossSectionGraphEntry : public FormEntry {
	friend class CrossSectionGraph;
public:
	CrossSectionGraphEntry(FormEntry* par, vector<IlwisObject>& sources, const CoordSystem& cys, ILWIS::CrossSectionTool *t);
//	void addSourceSet(const IlwisObject& obj);
	void setRecordRange(const RangeInt& rng);
	void create();
	void addCoord(const Coord& crd);
	void setLastCoord(const Coord& crd);
	void setYStretch(bool stretch);
	void setListView(FieldListView *v);
	RangeReal getRange(long i);
	ILWIS::CrossSectionTool *tool;
	void fillList();
	void update();
	void reset();
	void onContextMenu(CWnd* pWnd, CPoint point);
	CrossSectionGraph *graph() {return crossSectionGraph;}

private:
	const DomainValueRangeStruct & getDvrs(int i) const;
	bool isUnique(const FileName& fn);
	vector<IlwisObject>& sources;
	vector<Coord> crdSelect;
	FieldListView *listview;
	vector<RangeReal> ranges;
	vector<int> currentIndex;
	CrossSectionGraph *crossSectionGraph;
	CoordSystem csy;
};