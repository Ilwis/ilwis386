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
	void updateRepresentations();
	void setRprBase(const Representation& rprB);
  /* int OnToolHitTest(CPoint point, TOOLINFO *pTI) const;
   void OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);*/

   DECLARE_MESSAGE_MAP();
private:
	void drawRprBase(LPDRAWITEMSTRUCT lpDIS, const CRect rct);
	ValueSlicerSlider *fldslicer;
	vector<int> ylimits;
	CPoint activePoint;
	Representation rprBase;
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
	void setHighColor(Color c);
	void setLowColor(Color c);
	void setBoundColor(int index, Color c);
	void setRprBase(const Representation& rprB);
	Representation getRpr() const;
	
private:
	void init();
	Color nextColor(int i);
	ValueSlicer *valueslicer;
	ValueRange valrange;
	ILWIS::SetDrawer *drawer;
	ILWIS::DrawingColor *drawingcolor;
	RepresentationGradual *rprgrad;
	Representation rpr;
	vector<double> bounds;
	int selectedIndex;
	Color lowColor;
	Color highColor;
	Representation rprBase;
};


//----------------------------
class SlicingStepColor : public FormWithDest {
public:
	SlicingStepColor(CWnd *par, Color *col);

	//int exec();
};