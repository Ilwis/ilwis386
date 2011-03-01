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
	PointSymbolizationForm(CWnd *wPar, PointSetDrawer *dr);
	void apply(); 
private:
	vector<string> names;
	long selection;
	int thick;
	double scale;
	FieldOneSelectString *fselect;
	FieldInt *fiThick;
	FieldReal *frScale;


};


}