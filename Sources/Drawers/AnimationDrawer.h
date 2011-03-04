#pragma once

class MultiColumnSelector;
class TimeGraphSlider;
class FieldStringList;
class FieldOneSelectTextOnly;
class ValueSlicerSlider;
class FieldLister;
class FieldRealSliderEx;

#define ID_AnimationBar 6007

ILWIS::NewDrawer *createAnimationDrawer(ILWIS::DrawerParameters *parms);
namespace ILWIS{
	class BoxDrawer;
	class AnimationSlicing;
	class FeatureLayerDrawer;
	class SetDrawer;


	class _export AnimationDrawer : public AbstractMapDrawer {
	public:
		AnimationDrawer(DrawerParameters *parms);
		virtual ~AnimationDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap, int options=0);
		void *getDataSource() const;
		bool draw(int drawerIndex , const CoordBounds& cbArea) const;
		void timedEvent(UINT _timerid);
		String description() const;
		virtual void inactivateOtherPalettes(Palette * palette);
		//void setcheckRpr(void *value, LayerTreeView *tree);
		void updateLegendItem();
		vector<int>& getActiveMaps() { return activeMaps;}
		void animationDefaultView();
		void setTransparency(double v);
		void setMapIndex(int ind);
		double getInterval() const { return interval; }
		bool getUseTime() const { return useTime; }
		void setInterval(double intv) { interval = intv; }
		String getTimeColumn() const { return colTime; }
		void setTimeStep(ILWIS::Duration dur) { timestep = dur; }
		int getTimerId() const { return timerid; }
		void setIndex(int ind) { index = ind;}
		void setTimerId(int tid) { timerid = tid; }
		void setUseTime(bool yesno) { useTime = yesno; }
		int getMapIndex() const { return mapIndex;}
		static int getTimerIdCounter();

		static int timerIdCounter;

	protected:
		enum SourceType{sotUNKNOWN, sotFEATURE, sotMAPLIST, sotOBJECTCOLLECTION};
		virtual String iconName(const String& subtype="?") const;
		void removeSelectionDrawers() ;
		bool timerPerIndex() ;
		bool timerPerTime() ;
		void addSelectionDrawers(const Representation& rpr);
		RangeReal getMinMax(const MapList& mlist) const;
		SetDrawer *createIndexDrawer(const BaseMap& basemap,ILWIS::DrawerParameters& dp, PreparationParameters* pp);
		String timeString(const MapList& mpl,int index);
		void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const;
		double interval;
		UINT timerid;
		IlwisObject *datasource;
		SourceType sourceType;
		vector<String> names;
		vector<int> activeMaps;
		FeatureLayerDrawer *featurelayer;
		int index;
		int mapIndex;
		bool loop;
		bool useTime;
		String colTime;
		clock_t last;
		vector<Palette*> paletteList;
		Representation rpr;
		//DisplayOptionsLegend *doLegend;

		ILWIS::Duration timestep;
		CCriticalSection csAccess;

		void addSetDrawer(const BaseMap& basemap, ILWIS::PreparationParameters *pp, ILWIS::SetDrawer *rsd, const String& name="", bool post=false);
	};


	/*class AnimationSlicing : public DisplayOptionsForm2 {
	public:
		AnimationSlicing(CWnd *par, AnimationDrawer *gdr);
	private:
		FieldOneSelectTextOnly *fldSteps;
		int createSteps(Event*);
		void shutdown(int iReturn);
		String steps;
		ValueSlicerSlider *vs;
		int saveRpr(Event *ev);
		FileName fnRpr;

	};

	class AnimationSelection : public DisplayOptionsForm2 {
	public:
		AnimationSelection(CWnd *par, AnimationDrawer *gdr);
	private:
		void shutdown(int iReturn);
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
	};*/

}