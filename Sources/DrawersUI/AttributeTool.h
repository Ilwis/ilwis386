#pragma once

ILWIS::DrawerTool *createAttributeTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class AnimationDrawer;

class AttributeTool : public DrawerTool {
public:
	AttributeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~AttributeTool();
	String getMenuString() const ;
	void update();
protected:
	void setcheckAttributeTable(void *w, HTREEITEM );
	void setcheckattr(void *value, HTREEITEM item);
	void displayOptionAttColumn();

	SetChecks *attrCheck;
};

	class ChooseAttributeColumnForm : public DisplayOptionsForm {
		public:
		ChooseAttributeColumnForm(CWnd *wPar, LayerDrawer *dr, DrawerTool *t);
		void apply(); 
	private:
		Table attTable;
		String attColumn;
		FieldColumn *fc;
		DrawerTool *tool;
	};


}