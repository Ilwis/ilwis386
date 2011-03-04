#pragma once

ILWIS::DrawerTool *createTransparencyTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class TransparencyTool : public DrawerTool {
	public:
		TransparencyTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~TransparencyTool();
		String getMenuString() const;
	protected:
		void displayOptionTransparency();


	};

class TransparencyForm : public DisplayOptionsForm {
	public:
	TransparencyForm(CWnd *wPar, ComplexDrawer *dr,HTREEITEM hti);
	void apply(); 
private:
	int setTransparency(Event *ev);

	int transparency;
	FieldIntSliderEx *slider;
	HTREEITEM htiTransparent;
};

}