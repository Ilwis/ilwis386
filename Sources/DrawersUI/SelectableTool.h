#pragma once

ILWIS::DrawerTool *createSelectableTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class BaseMapPtr;

namespace ILWIS {

	class SelectableTool : public DrawerTool {
	public:
		SelectableTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~SelectableTool();
		String getMenuString() const;

		bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		bool OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		void OnLButtonUp(UINT nFlags, CPoint point);
	protected:
		void setSelectable(void *v, HTREEITEM);
		void FeatureAreaSelected(CRect rect);
		bool fCtrl;
		BaseMapPtr *bmapptr;
		vector<long> selectedRaws;


	};


}

