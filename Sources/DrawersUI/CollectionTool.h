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
		void setcheckLayer(void *w, HTREEITEM item);
		IlwisObject::iotIlwisObjectType type;
	};

}