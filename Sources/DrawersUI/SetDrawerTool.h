#pragma once

namespace ILWIS {

	class SetDrawerTool : public DrawerTool {
	public:
		SetDrawerTool(const String& name, ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~SetDrawerTool();
		String getMenuString() const;
		void clear();
	protected:
	};

}