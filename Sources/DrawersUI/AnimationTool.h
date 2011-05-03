#pragma once

ILWIS::DrawerTool *createAnimationTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {
	class AnimationDrawer;
	class AnimationPropertySheet;

	class AnimationTool : public SetDrawerTool {
	public:
		AnimationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnimationTool();
		static AnimationPropertySheet *getManagement() ;
		String getMenuString() const;
		static AnimationPropertySheet *animManagement;
		void clear();
	protected:
		void animationDefaultView();
	};

}