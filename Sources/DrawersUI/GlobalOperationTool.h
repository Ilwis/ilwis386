#pragma once

ILWIS::DrawerTool *createGlobalOperationTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class GlobalOperationTool : public DrawerTool {
	public:
		GlobalOperationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~GlobalOperationTool();
		String getMenuString() const;
		void doAction(int options=0);
	protected:
		void doOperation();
		void addGlobalOperationItems();

		vector<Action *> actions;
		map<long, Action *> itemActions;
		HTREEITEM htiDummy;
		bool first;
	};

}