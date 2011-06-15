#pragma once

ILWIS::DrawerTool *createAnnotationDrawerTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnnotationDrawerTool : public DrawerTool {
	public:
		AnnotationDrawerTool( ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnnotationDrawerTool();
		String getMenuString() const;
		void clear();
	protected:
	};

}