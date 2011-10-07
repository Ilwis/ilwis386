#pragma once

ILWIS::DrawerTool *createColorCompositeTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {
	class RasterLayerDrawer;

	class ColorCompositeTool : public DrawerTool {
	public:
		ColorCompositeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~ColorCompositeTool();
		String getMenuString() const;
		void clear();
	protected:
		void displayOptionCC();
	};

	class SetBandsForm : public DisplayOptionsForm {
	public:
		SetBandsForm(CWnd *wPar, RasterLayerDrawer *drw);
		void apply();
	private:
		FieldMap *fm1, *fm2, *fm3;
		String band1, band2, band3;


	};

}