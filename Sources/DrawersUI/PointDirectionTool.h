#pragma once

ILWIS::DrawerTool *createPointDirectionTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class AnimationDrawer;

class PointDirectionTool : public DrawerTool {
public:
	PointDirectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~PointDirectionTool();
	String getMenuString() const;
protected:
	void setScaling();
	Table tbl;


};

class PointDirectionForm : public DisplayOptionsForm {
	public:
	PointDirectionForm(CWnd *wPar, PointLayerDrawer *dr, const Table& tbl);
	void apply(); 
private:
	int ColValCallBack(Event*);

	FieldColumn *fcColumn;
	FieldRangeReal *frr;
	CheckBox *cbClockwise;

	bool clockwise;
	Table tbl;
	PointProperties *props;
	SymbolRotationInfo inf;



};


}