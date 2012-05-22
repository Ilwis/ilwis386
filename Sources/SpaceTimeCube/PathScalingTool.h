#pragma once

ILWIS::DrawerTool *createPathScalingTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class AnimationDrawer;

class PathScalingTool : public DrawerTool {
public:
	PathScalingTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~PathScalingTool();
	String getMenuString() const;
protected:
	void setScaling();
	Table tbl;


};

class PathScalingForm : public DisplayOptionsForm {
	public:
	PathScalingForm(CWnd *wPar, FeatureLayerDrawer *dr, const Table& tbl);
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