#pragma once

ILWIS::DrawerTool *createAnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
namespace ILWIS {

	class AnnotationLegendDrawerTool : public DrawerTool {
	public:
		AnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnnotationLegendDrawerTool();
		String getMenuString() const;
		void clear();
	protected:
	};

}