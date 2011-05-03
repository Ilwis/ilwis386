#pragma once

ILWIS::DrawerTool *createLayerDrawerTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class LayerDrawerTool : public DrawerTool {
	public:
		LayerDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~LayerDrawerTool();
	protected:


	};

}