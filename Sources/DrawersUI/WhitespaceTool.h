#pragma once

ILWIS::DrawerTool *createwhitespacetool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class WhiteSpaceTool : public DrawerTool {
	public:
		WhiteSpaceTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~WhiteSpaceTool();
		String getMenuString() const;
	protected:
		void displayOptionWhiteSpace();

	};

class WhiteSpaceForm : public DisplayOptionsForm {
	public:
	WhiteSpaceForm(CWnd *wPar, ComplexDrawer *dr);
	void apply(); 
private:
	FieldReal* frTop, *frBot, *frLeft, *frRight;

	Extension ext;


};

}