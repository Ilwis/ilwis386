#pragma once

ILWIS::DrawerTool *createPointSymbolizationTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class AnimationDrawer;

class PointSymbolizationTool : public DrawerTool {
public:
	PointSymbolizationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~PointSymbolizationTool();
	String getMenuString() const;
protected:
	void setSymbolization();


};

class PointSymbolizationForm : public DisplayOptionsForm {
	public:
	PointSymbolizationForm(CWnd *wPar, PointLayerDrawer *dr);
	void apply(); 
private:
	vector<string> names;
	String name;
	long selection;
	long t3dOr;
	FieldOneSelectString *fselect;
	FieldDataType *fdSelect;
	FieldReal *fiThick;
	CheckBox *f3d;
	FieldReal *frScale;
	PointProperties *props;



};


}