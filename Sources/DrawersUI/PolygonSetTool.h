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
	void displayOptionSimplify();
	void setActiveAreas(void *w, HTREEITEM hti);
	void setActiveBoundaries(void *w, HTREEITEM hti);

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

class PolygonSimplificationForm : public DisplayOptionsForm {
	public:
	PolygonSimplificationForm(CWnd *wPar, ComplexDrawer *dr);
	void apply(); 
private:
	bool simplify;
	bool boundaries;
};

}