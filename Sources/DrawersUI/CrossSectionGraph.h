#pragma once

class CrossSectionGraphEntry;

//#define ID_TIME_TICK WM_USER+345

class CrossSectionGraph : public CStatic, public BaseZapp {
	friend class CrossSectionGraphEntry;
public:
   CrossSectionGraph(CrossSectionGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
   void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   void OnLButtonUp(UINT nFlags, CPoint point);
   void setIndex(int index);
	void PreSubclassWindow() ;
	void OnContextMenu(CWnd* pWnd, CPoint point) ;
		void saveAsTbl();
		void saveAsSpectrum();

   DECLARE_MESSAGE_MAP();
private:
	BaseMap getBaseMap(long i, long m);
	int getNumberOfMaps(long i);
	CrossSectionGraphEntry *fldGraph;
	vector<vector<vector<double> > >values;
	CToolTipCtrl* toolTip;
};

class _export CrossSectionGraphEntry : public FormEntry {
	friend class CrossSectionGraph;
public:
	CrossSectionGraphEntry(FormEntry* par, vector<IlwisObject>& sources, const CoordSystem& cys);
//	void addSourceSet(const IlwisObject& obj);
	void setRecordRange(const RangeInt& rng);
	void create();
	void setCoord( const Coord& crd);
	void setListView(FieldListView *v);
	RangeReal getRange(long i);
	void fillList();
	void update();
	void reset();
	CrossSectionGraph *graph() {return crossSectionGraph;}

private:
	bool isUnique(const FileName& fn);
	vector<IlwisObject>& sources;
	vector<Coord> crdSelect;
	FieldListView *listview;
	vector<RangeReal> ranges;
	vector<int> currentIndex;
	CrossSectionGraph *crossSectionGraph;
	CoordSystem csy;
};