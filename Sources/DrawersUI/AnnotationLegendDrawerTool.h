#pragma once

class FieldColor;
class FieldListView;
class FieldRealSliderEx;

ILWIS::DrawerTool *createAnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
namespace ILWIS {
	class AnnotationLegendDrawer;
	class AnnotationDrawer;

	class AnnotationLegendDrawerTool : public DrawerTool {
	public:
		AnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnnotationLegendDrawerTool();
		String getMenuString() const;
		void clear();
	protected:
		static int count;
		AnnotationDrawer *findAnnotation() const;
		void setPosition();
		void setAppearance();
		void makeActive(void *v, HTREEITEM );
		AnnotationLegendDrawer *legend;
		FileName associatedFile;
	};

	class LegendPosition : public DisplayOptionsForm2 {
		public:
		LegendPosition(CWnd *wPar, AnnotationLegendDrawer *dr);
		//void apply(); 
	private:
		int setPosition(Event *ev) ;
		int setOrientation(Event *ev);
		
		FieldRealSliderEx *sliderV;
		FieldRealSliderEx *sliderH;
		FieldIntSliderEx *sliderS;
		RadioGroup *rg;
		FieldInt *fiColumns;
		double x,y;
		int orientation;
		int cols;
	};

	class LegendAppearance : public DisplayOptionsForm {
	public:
		LegendAppearance(CWnd *wPar, AnnotationLegendDrawer *dr, ComplexDrawer *cdrw);
	private:
		void apply();
		bool useBgColor;
		Color bgColor;
		bool drawBoundary;
		ComplexDrawer *layer;
		FieldColor *fc;
		FieldListView *fview;
		CheckBox *cbColor, *cbBoundary;
		FieldReal *fldScale;
		FieldReal *fontScale;
		FieldString *fldTitle;
		double scale;
		String title;
		double fscale;
		//vector<long> raws;

		double rmin,rmax;
		double rstep;
		RangeReal range;
		DomainValueRangeStruct dvrs;
		FieldReal *fstep, *fmax, *fmin;
	};

}