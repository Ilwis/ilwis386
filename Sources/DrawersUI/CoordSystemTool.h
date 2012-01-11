#pragma once

ILWIS::DrawerTool *createCoordSystemTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class CoordSystemTool : public DrawerTool {
	public:
		CoordSystemTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~CoordSystemTool();
		String getMenuString() const;
	protected:
		void displayOptionCoordSystem();


	};

class CoordSystemForm : public DisplayOptionsForm {
	public:
	CoordSystemForm(CWnd *wPar, ComplexDrawer *dr);
	void apply(); 
private:
	int setCoordSystem(Event *ev);
	FormEntry *CheckData();
	FieldCoordSystem *fldCsy;

	String name;

};

}