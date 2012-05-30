#pragma once

ILWIS::DrawerTool *createTimeZoomTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldIntSlider;

namespace ILWIS {

	class SpaceTimeCube;
	class TimeBounds;
	class TimeZoomForm;

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
	};

	class TimeZoomForm : public DisplayOptionsForm {
	public:
		TimeZoomForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM hti, SpaceTimeCube * _stc, TimeZoomTool & _timeZoomTool);
		virtual void apply();
		virtual void OnOK();
		virtual void OnCancel();
	private:
		int sliderFromCallBack(Event *ev);
		int sliderToCallBack(Event *ev);
		void calcSliderFromMinMax();
		void calcMinMaxFromSlider();
		void SetNewValues();
		int timeZoomFrom;
		int timeZoomTo;
		FieldIntSlider * sliderFrom;
		FieldIntSlider * sliderTo;
		HTREEITEM htiTimeZoom;
		FieldRangeReal *fldTimeFrom;
		FieldRangeReal *fldTimeTo;
		FieldTime * ftFrom;
		FieldTime * ftTo;
		ILWIS::Time tMin;
		ILWIS::Time tMax;
		SpaceTimeCube * stc;
		TimeZoomTool & timeZoomTool;
		bool fInCallback;
		const double maxSlider;
};


}