#pragma once


ILWIS::NewDrawer *createFeatureDataDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class LayerDrawer;

class _export FeatureDataDrawer : public SpatialDataDrawer {
	public:

		FeatureDataDrawer(DrawerParameters *parms);
		virtual ~FeatureDataDrawer();
		virtual void prepare(PreparationParameters *pp);
		void getFeatures(vector<Feature *>& features, int index = 0) const;
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);

	protected:
		void addLayerDrawer(const BaseMap& basemap, ILWIS::PreparationParameters *pp, ILWIS::LayerDrawer *fsd, const String& name="");

	};

}