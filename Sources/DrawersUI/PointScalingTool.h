#pragma once

ILWIS::DrawerTool *createPointScalingTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class _export PointScalingTool : public DrawerTool {
public:
	PointScalingTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~PointScalingTool();
	String getMenuString() const;
protected:
	void setScaling();
	Table tbl;


};

class PointScalingForm : public DisplayOptionsForm {
	public:
	PointScalingForm(CWnd *wPar, FeatureLayerDrawer *dr, const Table& tbl);
	void apply(); 
private:
	int ColValCallBack(Event*);

	FieldColumn *fcColumn;
	FieldRangeReal *frr;
	FieldRangeReal *fri;
	FieldReal *fmscale;

	int stretchModel;
	int scaleModel;
	String sCol;
	Table tbl;
	RangeReal rrScale;
	RangeReal rrStretch;
	PointProperties *props;



};


}