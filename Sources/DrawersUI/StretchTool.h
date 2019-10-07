#pragma once

ILWIS::DrawerTool *createStretchTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldRealSliderEx;

namespace ILWIS {
class StretchTool : public DrawerTool {
public:
	StretchTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	String getMenuString() const;
private:
	RangeReal getBaseRange() const;
	void displayOptionStretch();
	HTREEITEM htiLower;
	HTREEITEM htiUpper;
};

class SetStretchValueForm : public DisplayOptionsForm2 {
	public:
	SetStretchValueForm(CWnd *wPar, NewDrawer *dr, const RangeReal& _baserr, const RangeReal& _currentrr, double rStep, HTREEITEM _htiLower, HTREEITEM _htiUpper);
	void apply(); 
private:
	int logStretching(Event *);
	RangeReal rr;
	FieldRealSliderEx *sliderLow;
	FieldRealSliderEx *sliderHigh;
	CheckBox *cb;
	double low, high;
	bool inRace;
	bool fStarting;
	bool logStretch;
	HTREEITEM htiLower;
	HTREEITEM htiUpper;
	int check(Event *);
};
}