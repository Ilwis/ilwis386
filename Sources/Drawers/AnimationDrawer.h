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
		FieldReal *frSecondsPerFrame;
	};

	class AnimationControl: public DisplayOptionsForm
	{
	public:
		AnimationControl(CWnd *par, AnimationDrawer *gdr);
		void apply();
	private:
	};

	class AnimationSourceUsage: public DisplayOptionsForm
	{
	public:
		AnimationSourceUsage(CWnd *par, AnimationDrawer *gdr);
		void apply();
	private:
		RadioGroup *rg;
		MultiColumnSelector *mcs;
		int columnUsage;
	};


}