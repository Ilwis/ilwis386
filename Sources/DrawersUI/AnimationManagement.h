#pragma once

class MapCompositionDoc;
class FieldOneSelect;
class FieldRealSliderEx;
class TimeGraphSlider;
class FieldColor;

#define ID_AnimationBar 67305

class CGLToMovie;
class FieldListView;

namespace ILWIS {

	struct AnimationProperties;
	class AnimationDrawer;
	class AnimationBar;

	struct AnimationProperties {
		AnimationProperties() : drawer(0), mdoc(0), animBar(0) {}
		AnimationProperties(const AnimationProperties& p) : drawer(p.drawer), mdoc(p.mdoc), animBar(p.animBar) {}
		AnimationDrawer *drawer;
		MapCompositionDoc *mdoc;
		AnimationBar *animBar;
	};

	class AnimationBar : public CToolBar
	{
	public:
		AnimationBar();
		virtual ~AnimationBar();
		virtual void OnUpdateCmdUI(CFrameWnd*, BOOL);
		void Create(CWnd* pParent,const AnimationProperties& anim); 
		void updateTime(/*const AnimationProperties* props*/ );
		String setTimeString(/*const AnimationProperties* props*/);
		HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		void setMarked(bool yesno);
		LRESULT OnChangeColor(WPARAM wp, LPARAM lp) ;
		LRESULT OnUpdateAnimMessages(WPARAM p1, LPARAM p2);
	protected:
		afx_msg void OnSetFocus();
		afx_msg void OnKillFocus();

		CEdit ed;
		CFont fnt;
		bool fActive;
		bool isMarked;
		CBrush red;
		AnimationProperties animation;

		DECLARE_MESSAGE_MAP()
	};

	class AnimationPropertySheet : public CPropertySheet
	{
	public:
		enum Pages{pRun=1, pSynchornization=2, pAttributes=4, pTimedEvent=16, pProgress=32, pAVI=64, pRemove=128,  pAll=32767};
		AnimationPropertySheet();

		BOOL OnInitDialog();
		void addAnimation(const AnimationProperties& props);
		void setActiveAnimation(AnimationDrawer * drw);
		void removeAnimation(AnimationDrawer * drw);
		AnimationProperties* findAnimationProps(AnimationDrawer * drw);
		AnimationProperties* getActiveAnimation();
		AnimationProperties* getAnimation(int i);
		int getAnimationCount() const;
		int getActiveIndex() const;
		void OnSysCommand(UINT nID, LPARAM p);
		void OnClose();
		void OnDestroy();
		LRESULT command(WPARAM, LPARAM);
	
	private:
		vector<AnimationProperties> animations;
		int activeIndex;
		CCriticalSection cs;

		DECLARE_MESSAGE_MAP( );
	};

	class AnimationRun : public FormBasePropertyPage {
	public:
		AnimationRun(AnimationPropertySheet& sheet);
		~AnimationRun();
		int changeActive(Event *ev);
		void timed();
		void refreshTimer();
	private:
		void stopAvi();
		void startAvi();
		void UpdateUIState();
		FlatIconButton *fiPause;
		FlatIconButton *fiRun;
		FieldOneSelect *foAnimations;
		FieldRealSliderEx *sliderFps;
		FieldString *fldAviName;
		CheckBox *cbAvi;
		long animIndex;
		double fps;
		bool saveToAvi;
		CGLToMovie *movieRecorder;
		String fnAvi;

		AnimationPropertySheet	&propsheet;
		virtual int DataChanged(Event*);
		int framePlus(Event *ev);
		int frameMinus(Event *ev);
		int speed(Event *ev);
		int begin(Event *ev);
		int end(Event *ev);
		int pause(Event *ev);
		int run(Event *ev);
		int stop(Event *ev);
		int checkAvi(Event *ev);
	};

	class AnimationSynchronization : public FormBasePropertyPage {
	public:
		AnimationSynchronization(AnimationPropertySheet& sheet);
	private:
		BOOL OnInitDialog();
		FieldOneSelect *foSlave1;
		FieldReal *fiSlaveStep;
		FieldGroup *fgMaster,*fgSlaveIndex, *fgSlaveTime;
		StaticText *stMaster;
		FieldInt *fiSlave1I;
		FieldListView *listview;
		int offset1;
		long choiceMaster, choiceSlave1;
		double step1;
		AnimationPropertySheet	&propsheet;
		bool initial;
		int year, month, day, hour, minute;

		int synchronize(Event*);
		int DataChanged(Event*ev) ;
		void setTimerPerIndex(FormEntry *anchor);
		void setTimerPerTime(FormEntry *anchor);
	};
	class GraphPropertyForm;

	class AnimationProgress : public FormBasePropertyPage {
		public:
			AnimationProgress(AnimationPropertySheet& sheet);
		private:

			int DataChanged(Event*ev);
			int changeColumn(Event *ev);
			LRESULT OnTimeTick( WPARAM wParam, LPARAM lParam );
			LRESULT OnCleanForm( WPARAM wParam, LPARAM lParam );
			AnimationPropertySheet	&propsheet;
			TimeGraphSlider *graphSlider;
			FieldGroup *fgMaster;
			FieldColumn *fcol;
			StaticText *stMaster;
			int graphProperties(Event *ev);
			GraphPropertyForm *form;

			String colName;
			Table tbl;
			
		DECLARE_MESSAGE_MAP();
	};

	class RealTimePage : public FormBasePropertyPage {
	public:
		RealTimePage(AnimationPropertySheet& sheet, AnimationRun * _animationRun);
	private:
		AnimationPropertySheet	&propsheet;
		FieldGroup *fgTime;
		FieldGroup *fgMaster;
		StaticText *stMaster;
		CheckBox *cbTime;
		FieldColumn *fcolTime;
		FieldInt *fiYr, *fiMonth, *fiDay, *fiHour, *fiMinute;
		Table tbl;
		bool useTimeAttribute;
		int year, month, day, hour, minute;
		String timeColName;

		int DataChanged(Event*ev);
		int setTimingMode(Event *ev);
		int changeDuration(Event *ev);
		int changeTimeColumn(Event *e);
		void setTimeElements(FormEntry *entry);
		double calcNiceStep(Duration time);
		AnimationRun * animationRun;
	};

	class GraphPropertyForm : public DisplayOptionsForm {
	public:
		GraphPropertyForm(CWnd *wPar,TimeGraphSlider *slider, const Column& col, AnimationProperties *adr);
	private:
		void apply();
		void OnClose();
		FormEntry *CheckData();
		void calcMed(const Column& col);

		FieldReal *frr;
		Column col;
		TimeGraphSlider *graph;
		AnimationProperties *props;
		int type;
		double threshold;
		Color color;
		int changeColor(Event *ev);
		FieldColor *fc;

		DECLARE_MESSAGE_MAP();
	};
}

