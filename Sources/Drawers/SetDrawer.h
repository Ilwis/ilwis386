#pragma once

namespace ILWIS{
	class LayerDrawer;
	class SetDrawer;

	class _export SetDrawer : public SpatialDataDrawer {
	public:
		SetDrawer(DrawerParameters *parms, const String& title);
		virtual ~SetDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap, int options=0);
		bool draw(int drawerIndex, const DrawLoop drawLoop, const CoordBounds& cbArea) const;
		String description() const;
		void updateLegendItem();
		void setTransparency(double v);
		RangeReal getStretchRangeReal() const { return rrMinMax; }
		void setStretchRangeReal(const RangeReal& rr);
		bool isSet() const { return true; }


	protected:
		enum SourceType{sotUNKNOWN, sotFEATURE, sotMAPLIST, sotOBJECTCOLLECTION};
		virtual String iconName(const String& subtype="?") const;
		RangeReal getStretchRangeReal(const MapList& mlist) const;
		RangeReal getStretchRangeReal(const ObjectCollection& oc) const;
		void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const;
		ILWIS::LayerDrawer *createIndexDrawer(int index, const BaseMap& basemap,ILWIS::DrawerParameters& dp, PreparationParameters* pp);
		SourceType sourceType;
		Representation rpr;
		RangeReal rrMinMax;

		CCriticalSection csAccess;

		void addLayerDrawer(int index, const BaseMap& basemap, ILWIS::PreparationParameters *pp, ILWIS::LayerDrawer *rsd, const String& name="", bool post=false);
	};

}