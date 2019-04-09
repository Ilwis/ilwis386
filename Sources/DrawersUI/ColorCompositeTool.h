#pragma once

ILWIS::DrawerTool *createColorCompositeTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldRealSliderEx;

namespace ILWIS {
	class ColorCompositeDrawer;

	class ColorCompositeTool : public DrawerTool {
	public:
		ColorCompositeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~ColorCompositeTool();
		String getMenuString() const;
		void clear();
		void addBands();
	protected:
		void stretchCC1();
		void stretchCC2();
		void stretchCC3();
		void displayOptionCC();
	};

	class SetBandsForm : public DisplayOptionsForm {
	public:
		SetBandsForm(CWnd *wPar, ColorCompositeTool *ccTool);
		void apply();
	private:
		FieldOneSelectString *fm1, *fm2, *fm3;
		FieldInt *fi1, *fi2, *fi3;
		CheckBox *cb;
		long v1,v2,v3;
		int e1,e2,e3;
		bool exception;
		vector<String> names;
		ColorCompositeTool *ccTool;

		int setExc(Event *ev);


	};

	class SetStretchCCForm : public DisplayOptionsForm2 {
	public:
		SetStretchCCForm(CWnd *wPar, ColorCompositeDrawer *dr,int index);
		void apply(); 
	private:
		FormEntry *CheckData();

		FieldRealSliderEx *sliderLow;
		FieldRealSliderEx *sliderHigh;
		const RangeReal rrAllowedRange;
		const double rStep;
		double low, high;
		int index;

		int check(Event *);

	};

}

