#pragma once


ILWIS::NewDrawer *createFeatureLayerDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class SetDrawer;

class FeatureLayerDrawer : public AbstractMapDrawer {
	public:

		FeatureLayerDrawer(DrawerParameters *parms);
		virtual ~FeatureLayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		void getFeatures(vector<Feature *>& features) const;
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);

	protected:
		void addSetDrawer(const BaseMap& basemap, ILWIS::PreparationParameters *pp, ILWIS::SetDrawer *fsd, const String& name="");

	};

}