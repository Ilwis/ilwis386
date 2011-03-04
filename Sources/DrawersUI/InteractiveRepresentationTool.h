#pragma once

ILWIS::DrawerTool *createInteractiveRepresentationTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class InteractiveRepresentationTool : public DrawerTool {
public:
	InteractiveRepresentationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~InteractiveRepresentationTool();
	String getMenuString() const;
protected:
	void rasterSlicing();


};

class InterActiveSlicing : public DisplayOptionsForm2 {
public:
	InterActiveSlicing(CWnd *par, NewDrawer *gdr);
private:
	FieldOneSelectTextOnly *fldSteps;
	int createSteps(Event*);
	String steps;
	ValueSlicerSlider *vs;
	int saveRpr(Event *ev);
	FileName fnRpr;

};

}