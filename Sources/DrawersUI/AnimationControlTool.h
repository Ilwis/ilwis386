#pragma once

ILWIS::DrawerTool *createAnimationControlTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;
	class AnimationControl;

	class AnimationBar : public CToolBar
	{
	// Construction
	public:
		AnimationBar();
		virtual ~AnimationBar();
		virtual void OnUpdateCmdUI(CFrameWnd*, BOOL);
		void Create(CWnd* pParent); 
		void updateTime(const String& );
	protected:
		afx_msg void OnSetFocus();
		afx_msg void OnKillFocus();

		CEdit ed;
		CFont fnt;
		bool fActive;

		DECLARE_MESSAGE_MAP()
	};


	class AnimationControlTool : public DrawerTool {
	public:
		AnimationControlTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnimationControlTool();
		String getMenuString() const;
	protected:
		void animationControl();
		AnimationControl *animControl;
	};

	class AnimationControl: public DisplayOptionsForm2
	{
	public:
		AnimationControl(CWnd *par, AnimationDrawer *gdr);
	private:
		int setTiming(Event *ev);
		int changeColum(Event *);
		LRESULT OnTimeTick( WPARAM wParam, LPARAM lParam );
		void setSlider(FormEntry *entry);
		void setTimeElements(FormEntry *entry);
		void setMapIndex(int index);
		void shutdown(int iReturn=2);
		double calcNiceStep(Duration time);
		int changeDuration(Event *ev);
		void OnTimer(UINT timerID);
		TimeGraphSlider *graphSlider;
		FieldGroup *fgTime;
		FieldColumn *fcol;
		FieldColumn *fcolTime;
		FieldInt *fiYr, *fiMonth, *fiDay, *fiHour, *fiMinute;
		//FieldTime *ftime;
		StaticText *st;
		CheckBox *cbTime;
		FieldRealSliderEx *sliderFps;
		String colName;
		//FieldReal *frtime;
		FlatIconButton *fbBegin; 
		int begin(Event  *ev);
		int end(Event  *ev);
		int pause(Event  *ev);
		int run(Event  *ev);
		int stop(Event  *ev);
		int speed(Event *ev);
		int changeTimeColumn(Event *);
		int setTimingMode(Event *ev);
		bool initial;
		double fps;
		int year, month, day, hour, minute;
		bool useTimeAttribute;
		String timeColName;
		AnimationBar animBar;


		DECLARE_MESSAGE_MAP();
	};

}