#pragma once

class MultiColumnSelector;
class TimeGraphSlider;
class FieldStringList;
class FieldOneSelectTextOnly;
class ValueSlicerSlider;

//#define ID_TIME_TICK WM_USER+345

ILWIS::NewDrawer *createAnimationDrawer(ILWIS::DrawerParameters *parms);
namespace ILWIS{

	class _export AnimationDrawer : public AbstractMapDrawer {
		friend class AnimationTiming;
		friend class AnimationSourceUsage;
		friend class AnimationControl;
	public:
		AnimationDrawer(DrawerParameters *parms);
		virtual ~AnimationDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap, int options=0);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		bool draw(bool norecursion , const CoordBounds& cbArea) const;
		void timedEvent(UINT _timerid);
		String description() const;
		
	protected:
		enum SourceType{sotUNKNOWN, sotFEATURE, sotMAPLIST, sotOBJECTCOLLECTION};
		virtual String iconName(const String& subtype="?") const;
		void animationTiming(CWnd *w);
		void animationControl(CWnd *w);
		void animationSourceUsage(CWnd *parent);
		void setMapIndex(int index);
		void animationSlicing(CWnd *parent);
		void animationSelection(CWnd *parent);
		void setTransparency(double v);
		bool timerPerIndex() ;
		bool timerPerTime() ;
		double interval;
		UINT timerid;
		IlwisObject *datasource;
		SourceType sourceType;
		vector<String> names;
		FeatureLayerDrawer *featurelayer;
		static int timerIdCounter;
		int index;
		int mapIndex;
		bool loop;
		bool useTime;
		String colTime;

		ILWIS::Duration timestep;
		CCriticalSection csAccess;
		AnimationControl *animcontrol;

		void addSetDrawer(const BaseMap& basemap, ILWIS::PreparationParameters *pp, ILWIS::SetDrawer *rsd, const String& name="");
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
		TimeGraphSlider *graphSlider;
		FieldGroup *fgTime;
		FieldColumn *fcol;
		FieldColumn *fcolTime;
		FieldTime *ftime;
		StaticText *st;
		CheckBox *cbTime;
		String colName;
		FieldReal *frtime;
		FlatIconButton *fbBegin; 
		int begin(Event  *ev);
		int end(Event  *ev);
		int pause(Event  *ev);
		int run(Event  *ev);
		int stop(Event  *ev);
		int changeTimeColumn(Event *);
		int setTimingMode(Event *ev);
		bool initial;

		DECLARE_MESSAGE_MAP();
	};

	class AnimationSlicing : public DisplayOptionsForm2 {
	public:
		AnimationSlicing(CWnd *par, AnimationDrawer *gdr);
	private:
		FieldOneSelectTextOnly *fldSteps;
		int createSteps(Event*);
		String steps;
		ValueSlicerSlider *vs;

	};

	class AnimationSelection : public DisplayOptionsForm2 {
	public:
		AnimationSelection(CWnd *par, AnimationDrawer *gdr);
	private:
		FieldOneSelectTextOnly *fldSteps;
		int createSteps(Event*);
		String steps;
		ValueSlicerSlider *vs;

	};

	class AnimationSourceUsage: public DisplayOptionsForm2
	{
	public:
		AnimationSourceUsage(CWnd *par, AnimationDrawer *gdr);
		int exec();
	private:
		RadioGroup *rg;
		MultiColumnSelector *mcs;
		int columnUsage;
	};



}