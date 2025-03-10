#pragma once

class FieldColor;
class FieldListView;

ILWIS::DrawerTool *createAnnotationScaleBarDrawerTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
namespace ILWIS {
	class AnnotationScaleBarDrawer;

	class AnnotationScaleBarDrawerTool : public DrawerTool {
	public:
		AnnotationScaleBarDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnnotationScaleBarDrawerTool();
		String getMenuString() const;
	protected:
		void setPosition();
		void makeActive(void *v, HTREEITEM );
		AnnotationScaleBarDrawer *scaleDrawer;
	};

	class ScaleBarPosition : public DisplayOptionsForm {
		public:
		ScaleBarPosition(CWnd *wPar, AnnotationScaleBarDrawer *dr);
	private:
		int setPosition(Event *ev);
		int UseKilometersChanged(Event *);
		void apply();
		
		FieldIntSliderEx *sliderV;
		FieldIntSliderEx *sliderH;
		FieldReal *fldSize;
		FieldString *fldUnit;
		FieldInt *fldTicks;
		FieldInt *fldNumbers;
		CheckBox *cbLine;
		CheckBox *cbDivide;
		CheckBox *cbMultiLabels;
		CheckBox *cbUseKilometers;
		FieldReal *fontScale;
		int ticks;
		int x,y;
		double sz;
		String unit;
		bool km;
		bool multiLabels;
		bool line;
		bool divideFirstInterval;
		int num;
		double fscale;
	};
}