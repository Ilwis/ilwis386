#pragma once

class ValueSlicerSlider;

namespace ILWIS{
	class DrawingColor;
}

#define ID_TIME_TICK WM_USER+345

class ValueSlicer : public CStatic, public BaseZapp {
public:
   ValueSlicer(ValueSlicerSlider *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
   void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   void OnLButtonUp(UINT nFlags, CPoint point);
   void OnLButtonDown(UINT nFlags, CPoint point);
   void OnLButtonDblClk(UINT nFlags, CPoint point);
   void OnMouseMove(UINT nFlags, CPoint point);
   	CRect makeDrawRect();
  /* int OnToolHitTest(CPoint point, TOOLINFO *pTI) const;
   void OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);*/

   DECLARE_MESSAGE_MAP();
private:
	ValueSlicerSlider *fldslicer;
	vector<int> ylimits;
	CPoint activePoint;
};

class _export ValueSlicerSlider : public FormEntry {
	friend class ValueSlicer;
public:
	ValueSlicerSlider(FormEntry* par, ILWIS::SetDrawer *sdrw);
	~ValueSlicerSlider();
	void setValueRange(const ValueRange& tbl);
	void setBound(int index, double up);
	void setDrawer(ILWIS::SetDrawer *drwc);
	void setNumberOfBounds(int n);
	void create();
	bool isValid() const;
	
private:
	void init();
	ValueSlicer *valueslicer;
	ValueRange valrange;
	ILWIS::SetDrawer *drawer;
	ILWIS::DrawingColor *drawingcolor;
	RepresentationGradual *rprgrad;
	Representation rpr;
	vector<double> bounds;
	int selectedIndex;
};

//----------------------------
class SlicingStepColor : public FormWithDest {
public:
	SlicingStepColor(CWnd *par, Color *col);

	//int exec();
};