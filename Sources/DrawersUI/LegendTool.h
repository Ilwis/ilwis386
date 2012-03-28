#pragma once

ILWIS::DrawerTool *createLegendTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
class ColorSelector;

namespace ILWIS {

class LineLayerDrawer;

class LegendTool : public DrawerTool {
public:
	LegendTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	~LegendTool();
	HTREEITEM configure( HTREEITEM parentItem);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	void update();
	String getMenuString() const;
private:
	void insertLegendItemsValue(const Representation& rpr, const DomainValueRangeStruct& dvs);
	void insertLegendItemsClass(const Representation& rpr);
	void displayOptionRpr();
	HTREEITEM htiLeg;
};

class LineRprForm : public DisplayOptionsForm {
	public:
	LineRprForm(CWnd *wPar, LayerDrawer *dr, RepresentationClass* rc, long raw);
	void apply(); 
private:
  RepresentationClass* rcl;  
  ColorSelector* cs;
  int CustomColor(Event *);
  long iRaw;
  Color col;
  Line line;

};
}
