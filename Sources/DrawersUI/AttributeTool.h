#pragma once

ILWIS::DrawerTool *createAttributeTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class AnimationDrawer;

class AttributeTool : public DrawerTool {
public:
	AttributeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~AttributeTool();
	String getMenuString() const ;
	void update();
protected:
	void setcheckattr(void *value, HTREEITEM item);

	SetChecks *attrCheck;
	HTREEITEM lasthit;
};

}