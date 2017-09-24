#pragma once

ILWIS::DrawerTool *createAnnotationBorderTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;
	class AnnotationBorderDrawer;

	class AnnotationBorderTool : public DrawerTool {
	public:
		AnnotationBorderTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnnotationBorderTool();
		String getMenuString() const;
	protected:
		void makeActive(void *v, HTREEITEM );
		void displayOptionAnnotationBorder();

		AnnotationBorderDrawer *border;


	};

class AnnotationBorderForm : public DisplayOptionsForm {
	public:
	AnnotationBorderForm(CWnd *wPar, AnnotationBorderDrawer *dr);
	void apply(); 
private:
	int step;
	int num;
	bool neatline;
	bool ticks;	
};

}