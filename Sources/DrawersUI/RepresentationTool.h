#pragma once

ILWIS::DrawerTool *createRepresentationTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class RepresentationTool : public DrawerTool {
public:
	RepresentationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	~RepresentationTool();
private:
	void displayOptionSubRpr();
	Representation rpr;
};

class RepresentationToolForm : public DisplayOptionsForm {
public:
	RepresentationToolForm(CWnd *wPar, ILWIS::LayerDrawer *dr, SetDrawer *adr, DrawerTool *t);
	void apply(); 
private:
	String rpr;
	FieldRepresentation *fldRpr;
	LayerDrawer *layerDrawer;
	SetDrawer *animDrw;
	DrawerTool *tool;
};
}
