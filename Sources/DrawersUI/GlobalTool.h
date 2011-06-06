#pragma once

ILWIS::DrawerTool *createGlobalTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class GlobalTool : public DrawerTool {
	public:
		GlobalTool(const String& name, ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~GlobalTool();
		String getMenuString() const;
		void clear();
	protected:
	};

}