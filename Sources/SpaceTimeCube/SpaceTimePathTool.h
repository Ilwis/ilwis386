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
		void changeFootprint();
		void changeXT();
		void changeXY();
		void changeYT();
		void setSpaceTimePathVisibility(void *value, HTREEITEM);
		void setFootprintVisibility(void *value, HTREEITEM);
		void setXTVisibility(void *value, HTREEITEM);
		void setXYVisibility(void *value, HTREEITEM);
		void setYTVisibility(void *value, HTREEITEM);
		void setVisibility(const String& element, bool value);
		void elementForm(const String& element);
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

	class SpaceTimeElementsForm : public DisplayOptionsForm {
		public:
		SpaceTimeElementsForm(CWnd *wPar, ComplexDrawer *dr,HTREEITEM hti, PathElement& elem);
		void apply(); 
	private:
		int dummy;
		int setTransparency(Event *ev);
		FieldIntSliderEx *slider;
		FieldColor *fc;
		int transparency;
		PathElement& element;
	};
}