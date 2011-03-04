#pragma once

ILWIS::DrawerTool *createLegendTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

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
	HTREEITEM htiLeg;
};
}
