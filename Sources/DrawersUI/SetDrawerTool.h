#pragma once

ILWIS::DrawerTool *createSetDrawerTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class SetDrawerTool : public DrawerTool {
	public:
		SetDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~SetDrawerTool();
	protected:


	};

}