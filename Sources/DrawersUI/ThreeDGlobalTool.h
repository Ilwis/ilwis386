#pragma once

ILWIS::DrawerTool *createThreeDGlobalTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class ThreeDGlobalTool : public DrawerTool {
	public:
		ThreeDGlobalTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~ThreeDGlobalTool();
	protected:
		void setthreeD(void *v, HTREEITEM it) ;


	};

}