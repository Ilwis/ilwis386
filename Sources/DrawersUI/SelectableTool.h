#pragma once

ILWIS::DrawerTool *createSelectableTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class BaseMapPtr;
class AreaSelector;

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
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnEscape();
		virtual void Stop();
	protected:
		void setSelectable(void *v, HTREEITEM);
		void FeatureAreaSelected(CRect rect);
		bool fCtrl;
		bool fShift;
		BaseMapPtr *bmapptr;
		vector<long> * selectedRaws;
		AreaSelector *as;

	};


}

