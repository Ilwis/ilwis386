#pragma once

ILWIS::DrawerTool *createPolygonSetTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class PolygonSetTool : public DrawerTool {
public:
	PolygonSetTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~PolygonSetTool();
	String getMenuString() const;
protected:
	void displayOptionTransparencyP();
	void setActiveAreas(void *w);
	void setActiveBoundaries(void *w);

	HTREEITEM htiTransparency;


};

class TransparencyFormP : public DisplayOptionsForm {
	public:
	TransparencyFormP(CWnd *wPar, ComplexDrawer *dr,HTREEITEM hti);
	void apply(); 
private:
	int setTransparency(Event *ev);

	int transparency;
	FieldIntSliderEx *slider;
	HTREEITEM htiTransparent;
};

}