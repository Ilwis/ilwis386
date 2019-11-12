#pragma once

class FieldColor;
class FieldListView;

ILWIS::DrawerTool *createAnnotationNorthArrowTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {
	class AnnotationNorthArrow;

	class AnnotationNorthArrowTool : public DrawerTool {
	public:
		AnnotationNorthArrowTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnnotationNorthArrowTool();
		String getMenuString() const;
	protected:
		void setPosition();
		void setAppearance();
		void makeActive(void *v, HTREEITEM );
		AnnotationNorthArrowDrawer *northDrawer;
	};

	class NorthArrowPosition : public DisplayOptionsForm {
		public:
		NorthArrowPosition(CWnd *wPar, AnnotationNorthArrowDrawer *dr);
	private:
		int setPosition(Event *ev) ;
		int setScale(Event *ev);
		int setArrow(Event *ev);
		void apply();
		
		FieldIntSliderEx *sliderV;
		FieldIntSliderEx *sliderH;
		FieldIntSliderEx *sliderScale;
		FieldOneSelectString *arrowType;
		int ticks;
		int x,y,scale;
		String unit;
		vector<String> names;
		long selection;
	};

	class NorthArrowAppearance : public DisplayOptionsForm {
	public:
		NorthArrowAppearance(CWnd *wPar, AnnotationNorthArrowDrawer *dr);
	private:
		void apply();
		bool useBgColor;
		Color bgColor;
		long selection;
		bool drawBoundary;
		double scale;
		String title;
		vector<String> names;

		//vector<long> raws;
	};

}