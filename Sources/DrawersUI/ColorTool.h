#pragma once

ILWIS::DrawerTool *createColorTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class ColorTool : public DrawerTool {
public:
	ColorTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~ColorTool();
	void setcheckRpr(void *value, HTREEITEM item);
	SetChecks *getColorCheck() { return colorCheck;}
	String getMenuString() const;
	void clear();
protected:
	SetChecks *colorCheck;


};

}