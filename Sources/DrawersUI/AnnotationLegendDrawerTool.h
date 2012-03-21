#pragma once

class FieldColor;
class FieldListView;

ILWIS::DrawerTool *createAnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
namespace ILWIS {
	class AnnotationLegendDrawer;

	class AnnotationLegendDrawerTool : public DrawerTool {
	public:
		AnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnnotationLegendDrawerTool();
		String getMenuString() const;
		void clear();
	protected:
		void setPosition();
		void setAppearance();
		void makeActive(void *v, HTREEITEM );
		AnnotationLegendDrawer *legend;
	};

	class LegendPosition : public DisplayOptionsForm2 {
		public:
		LegendPosition(CWnd *wPar, AnnotationLegendDrawer *dr);
		//void apply(); 
	private:
		int setPosition(Event *ev) ;
		int setOrientation(Event *ev);
		
		FieldIntSliderEx *sliderV;
		FieldIntSliderEx *sliderH;
		FieldIntSliderEx *sliderS;
		RadioGroup *rg;
		FieldInt *fiColumns;
		int x,y;
		int orientation;
		int cols;
	};

	class LegendAppearance : public DisplayOptionsForm {
	public:
		LegendAppearance(CWnd *wPar, AnnotationLegendDrawer *dr);
	private:
		void apply();
		bool useBgColor;
		Color bgColor;
		bool drawBoundary;
		FieldColor *fc;
		FieldListView *fview;
		CheckBox *cbColor, *cbBoundary;
		FieldReal *fldScale;
		FieldString *fldTitle;
		double scale;
		String title;
		//vector<long> raws;
	};

}