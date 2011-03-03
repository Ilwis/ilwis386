#pragma once

ILWIS::DrawerTool *createAnimationTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {
	class AnimationDrawer;

	class AnimationTool : public DrawerTool {
	public:
		AnimationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnimationTool();
		String getMenuString() const;
	protected:
		void animationDefaultView();

	};

}