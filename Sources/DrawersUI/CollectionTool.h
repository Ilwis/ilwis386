#pragma once

ILWIS::DrawerTool *createCollectionTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class CollectionTool : public SetDrawerTool {
	public:
		CollectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~CollectionTool();
		String getMenuString() const;
		void clear();
	protected:
		void createLayersNode(HTREEITEM htiNodeGroup, const String& name, const String& icon, const String& tool, int index);
		void setcheckLayer(void *w, HTREEITEM item);
		HTREEITEM layerItems[4];
		int visible[4];
	};

}