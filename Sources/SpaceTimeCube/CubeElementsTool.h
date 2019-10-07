#pragma once

ILWIS::DrawerTool *createCubeElementsTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {
	struct CubeElement;

	class AnimationDrawer;

	class CubeElementsTool : public DrawerTool {
	public:
		CubeElementsTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~CubeElementsTool();
		String getMenuString() const;
	protected:
		void changeCube();
		void changeTicks();
		void changeCoordinates();
		void changeLabels();
		void setCubeVisibility(void *value, HTREEITEM);
		void setTicksVisibility(void *value, HTREEITEM);
		void setVisibility(const String& element, bool value);
		int getVisibility(const String& element);
		void elementForm(const String& element);
		void setCoordVisibility(void *value, HTREEITEM);
		void setLabelVisibility(void *value, HTREEITEM);
	bool isDataLayer;


	};

class CubeElementsForm : public DisplayOptionsForm {
	public:
	CubeElementsForm(CWnd *wPar, ComplexDrawer *dr,HTREEITEM hti, CubeElement& elem);
	void apply(); 
private:
	int dummy;
	int setTransparency(Event *ev);
	FieldIntSliderEx *slider;
	FieldColor *fc;
	int transparency;
	CubeElement& element;

};

}