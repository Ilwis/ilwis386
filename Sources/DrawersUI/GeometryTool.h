#pragma once

ILWIS::DrawerTool *createGeometryTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class GeometryTool : public DrawerTool {
	public:
		GeometryTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~GeometryTool();
		String getMenuString() const;
	};
}