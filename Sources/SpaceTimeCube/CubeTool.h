#pragma once

ILWIS::DrawerTool *createCubeTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class SpaceTimePathDrawer;

	class CubeTool : public DrawerTool {
	public:
		CubeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~CubeTool();
		String getMenuString() const;
	protected:
		void displayOptionTemporalSource();
		void makeActive(void *v, HTREEITEM);
		SpaceTimePathDrawer *spaceTimePathDrawer;
		NewDrawer *pointMapDrawer;
	private:
		void replaceDrawer(NewDrawer * oldDrw, NewDrawer * newDrw);
	};

}