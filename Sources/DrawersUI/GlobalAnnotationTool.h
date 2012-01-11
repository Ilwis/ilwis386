#pragma once

ILWIS::DrawerTool *createGlobalAnnotationTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class GlobalAnnotationTool : public DrawerTool {
	public:
		GlobalAnnotationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~GlobalAnnotationTool();
	protected:
		void setthreeD(void *v, HTREEITEM it) ;


	};

}