#pragma once

ILWIS::DrawerTool *createLineSetTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class LineSetTool : public DrawerTool {
	public:
		LineSetTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~LineSetTool();
		String getMenuString() const;
	protected:


	};

}