#pragma once

ILWIS::DrawerTool *createThreeDStack(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class Cursor3DDrawer;

	struct StackInfo{
		StackInfo(HTREEITEM it, bool v) : item(it), status(v) {}
		HTREEITEM item;
		bool status;
	};

	class ThreeDStack : public DrawerTool {
		friend class ThreeDStackForm;

	public:
		ThreeDStack(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~ThreeDStack();
		virtual void OnLButtonUp(UINT nFlags, CPoint point);
	protected:
		void changeDistances();
		void setthreeDStackMarker(void *v, HTREEITEM);
		void setIndividualStatckItem(void *v, HTREEITEM);
		virtual void update();
		void updateLayerDistances();
		void setthreeDStack(void *v, HTREEITEM it) ;
		vector<StackInfo> stackStatus;
		double distance;
		double offset;
		Cursor3DDrawer *cursor;
		Coord mouseCrd;


	};

	class Cursor3DDrawer : public PointDrawer {
	public:
		Cursor3DDrawer(ILWIS::DrawerParameters *parms);
		~Cursor3DDrawer();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		private:
	};

	class ThreeDStackForm : public DisplayOptionsForm2 {
	public:
		ThreeDStackForm(CWnd *wPar, ComplexDrawer *dr, ThreeDStack *st, const ValueRange& _range, double *_distance, double *_offset);
		void apply(); 
	private:
		int setThreeDStack(Event *ev);

		int ThreeDStack;
		FieldRealSliderEx *sliderDistance;
		FieldRealSliderEx *sliderOffset;
		double *distance;
		double *offset;
		const ValueRange& range;
		ILWIS::ThreeDStack *stck;
	};

}