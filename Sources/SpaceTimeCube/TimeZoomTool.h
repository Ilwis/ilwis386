#pragma once

ILWIS::DrawerTool *createTimeZoomTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldIntSlider;

namespace ILWIS {

	class SpaceTimeCube;
	class TimeBounds;
	class TimeZoomForm;
	class PreTimeOffsetDrawer;

	class TimeZoomTool : public DrawerTool {
	public:
		TimeZoomTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~TimeZoomTool();
		String getMenuString() const;
		void setFormAutoDeleted();
	protected:
		void displayOptionTimeZoom();
		SpaceTimeCube * stc;
		TimeZoomForm * timeZoomForm;
		CoordBounds cbFullExtent;
	};

	class TimeZoomForm : public DisplayOptionsForm {
	public:
		TimeZoomForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM hti, SpaceTimeCube * _stc, CoordBounds & _cbFullExtent, TimeZoomTool & _timeZoomTool);
		virtual void apply();
		virtual void OnOK();
		virtual void OnCancel();
	private:
		int sliderFromCallBack(Event *ev);
		int sliderToCallBack(Event *ev);
		int sliderZoomCallBack(Event *ev);
		int sliderPosCallBack(Event *ev);
		void calcSliderFromMinMax();
		void calcMinMaxFromSlider();
		void calcSliderFromZoom();
		void calcZoomFromSlider();
		void calcSliderFromPos();
		void calcPosFromSlider();
		void SetNewValues();
		void preparePreTimeOffsetDrawers(ComplexDrawer * drw, PreparationParameters * pp);
		int timeZoomFrom;
		int timeZoomTo;
		int iZoom;
		int iXPos;
		int iYPos;
		FieldIntSlider * sliderFrom;
		FieldIntSlider * sliderTo;
		FieldIntSlider * sliderZoom;
		FieldIntSlider * sliderXPos;
		FieldIntSlider * sliderYPos;
		HTREEITEM htiTimeZoom;
		FieldTime * ftFrom;
		FieldTime * ftTo;
		ILWIS::Time tMin;
		ILWIS::Time tMax;
		double zoom;
		double xPos;
		double yPos;
		SpaceTimeCube * stc;
		CoordBounds & cbFullExtent;
		double prevZoom;
		TimeZoomTool & timeZoomTool;
		bool fInCallback;
		const double maxSlider;
};


}