#pragma once

ILWIS::DrawerTool *createOperationTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class OperationTool : public DrawerTool {
	public:
		OperationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~OperationTool();
		String getMenuString() const;
	protected:
		void doOperation();
		void doAction(int options=0);
		void addOperationItems(const String& sExt);

		vector<Action *> actions;
		map<long, Action *> itemActions;
		bool first;
		HTREEITEM htiDummy;
	};

}