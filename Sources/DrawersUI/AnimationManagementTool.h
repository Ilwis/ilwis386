#pragma once

ILWIS::DrawerTool *createAnimationManagementTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class AnimationManagementTool : public DrawerTool {
	public:
		AnimationManagementTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnimationManagementTool();
		void timedEvent(UINT timerid);
	protected:
		void animationManagement();


	};

}