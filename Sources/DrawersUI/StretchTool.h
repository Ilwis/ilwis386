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
	RangeReal getBaseRange() const ;
	void displayOptionStretch();
};

class SetStretchValueForm : public DisplayOptionsForm2 {
	public:
	SetStretchValueForm(CWnd *wPar, NewDrawer *dr, const RangeReal& _baserr, const RangeReal& _currentrr, double rStep);
	void apply(); 
private:
	RangeReal rr;
	FieldRealSliderEx *sliderLow;
	FieldRealSliderEx *sliderHigh;
	double low, high;

	int check(Event *);

};
}