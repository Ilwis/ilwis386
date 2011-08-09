#pragma once

ILWIS::DrawerTool *createInfoTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class InfoTool : public DrawerTool {
	public:
		InfoTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~InfoTool();
		String getMenuString() const;
	protected:
		void setInfo(void *v, HTREEITEM);


	};


}

