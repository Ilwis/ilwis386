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
	bool isDataLayer;


	};

class TransparencyForm : public DisplayOptionsForm {
	public:
	TransparencyForm(CWnd *wPar, ComplexDrawer *dr,HTREEITEM hti, bool isDataLayer = false);
	void apply(); 
private:
	int setTransparency(Event *ev);

	int transparency;
	FieldIntSliderEx *slider;
	HTREEITEM htiTransparent;
	FieldRangeReal *fldTranspValue;
	CheckBox *cb;
	bool isDataLayer;
	RangeReal transpValues;
	RangeReal oldRange;
	bool useTV;
};

}