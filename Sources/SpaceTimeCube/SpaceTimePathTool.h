#pragma once

ILWIS::DrawerTool *createSpaceTimePathTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class SpaceTimePathTool : public DrawerTool {
	public:
		SpaceTimePathTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~SpaceTimePathTool();
		String getMenuString() const;
	protected:
		void displayOptionEdges();
	};

	class EdgesForm : public DisplayOptionsForm {
	public:
		EdgesForm(CWnd *wPar, ComplexDrawer *dr,HTREEITEM hti);
		void apply();
		int setEdges(Event *ev);
	private:
		int setNrEdges(Event *ev);
		int nrEdges;
		FieldIntSliderEx *slider;
		HTREEITEM htiNrEdges;
		FieldRangeReal *fldNrEdges;
};


}