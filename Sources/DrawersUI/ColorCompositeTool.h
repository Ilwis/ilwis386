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
		void stretchCC1();
		void stretchCC2();
		void stretchCC3();
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

	class SetStretchCCForm : public DisplayOptionsForm2 {
	public:
		SetStretchCCForm(CWnd *wPar, RasterLayerDrawer *dr,int index);
		void apply(); 
	private:
		FormEntry *CheckData();

		RangeReal rr;
		FieldRealSliderEx *sliderLow;
		FieldRealSliderEx *sliderHigh;
		double low, high;
		int index;

		int check(Event *);

	};

}

