#pragma once

class CrossSectionGraphEntry;

#define ID_TIME_TICK WM_USER+345

class CrossSectionGraph : public CStatic, public BaseZapp {
public:
   CrossSectionGraph(CrossSectionGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
   void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   void OnLButtonUp(UINT nFlags, CPoint point);
   void setIndex(int index);
   int OnToolHitTest(CPoint point, TOOLINFO *pTI) const;
	void OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);
	void PreSubclassWindow() ;

   DECLARE_MESSAGE_MAP();
private:
	BaseMap getBaseMap(long i, long m);
	int getNumberOfMaps(long i);

	CrossSectionGraphEntry *fldGraph;
	vector<vector<double> > values;
	CToolTipCtrl* toolTip;
};

class _export CrossSectionGraphEntry : public FormEntry {
	friend class CrossSectionGraph;
public:
	CrossSectionGraphEntry(FormEntry* par);
	void addSourceSet(const IlwisObject& obj);
	void setRecordRange(const RangeInt& rng);
	void create();
	void setCoord( const Coord& crd);
	void setListView(FieldListView *v);
	RangeReal getRange(long i);
	void setIndex(int sourceIndex, double value);

private:
	bool isUnique(const FileName& fn);
	vector<IlwisObject> sources;
	Coord crdSelect;
	FieldListView *listview;
	vector<RangeReal> ranges;
	int currentIndex;
	CrossSectionGraph *crossSectionGraph;	
};