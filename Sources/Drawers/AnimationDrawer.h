#pragma once

class MultiColumnSelector;

ILWIS::NewDrawer *createAnimationDrawer(ILWIS::DrawerParameters *parms);
namespace ILWIS{

	class _export AnimationDrawer : public ComplexDrawer {
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
		void animationTiming(CWnd *w);
		void animationControl(CWnd *w);
		void animationSourceUsage(CWnd *parent);
		double interval;
		UINT timerid;
		IlwisObject *datasource;
		SourceType sourceType;
		vector<String> names;
		FeatureLayerDrawer *featurelayer;
		static int timerIdCounter;
		int index;
		bool loop;
	};

	class AnimationTiming: public DisplayOptionsForm
	{
	public:
		AnimationTiming(CWnd *par, AnimationDrawer *gdr);
		void apply();
	private:
		int setTiming(Event *ev);
		FieldRealSliderEx *slider;
	};

	class AnimationControl: public DisplayOptionsForm2
	{
	public:
		AnimationControl(CWnd *par, AnimationDrawer *gdr);
	private:
		int begin(Event  *ev);
		int end(Event  *ev);
		int pause(Event  *ev);
		int run(Event  *ev);
		int stop(Event  *ev);
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