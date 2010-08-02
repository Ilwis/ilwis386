#pragma once


ILWIS::NewDrawer *createFeatureLayerDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class SetDrawer;

class FeatureLayerDrawer : public AbstractMapDrawer {
	public:
		ILWIS::NewDrawer *createFeatureLayerDrawer(DrawerParameters *parms);

		FeatureLayerDrawer(DrawerParameters *parms);
		virtual ~FeatureLayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		String getMask() const;
		void setMask(const String& sM);
		void setSingleColor(const Color& c);
		Color getSingleColor() const;
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		void getFeatures(vector<Feature *>& features) const;

	protected:
		void addSetDrawer(const BaseMap& basemap, ILWIS::PreparationParameters *pp, ILWIS::SetDrawer *fsd, const String& name="");

		String mask;
		Color singleColor;

	};

}