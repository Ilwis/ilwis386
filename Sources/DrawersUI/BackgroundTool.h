#pragma once

ILWIS::DrawerTool *createBackgroundTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class BackgroundTool : public DrawerTool {
	public:
		BackgroundTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~BackgroundTool();
		String getMenuString() const;
	protected:
		void displayOptionOutsideColor();
		void displayOptionInsideColor();


	};

	class SetColorForm : public DisplayOptionsForm {
	public:
		SetColorForm(const String& title, CWnd *wPar, CanvasBackgroundDrawer *dr, Color& color);
		void apply();
	private:
		FieldColor *fc;
		Color c;
		Color& clr;

	};
}