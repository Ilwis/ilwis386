
#pragma once

namespace ILWIS	{

	class FeatureLayerDrawer;

	class _export FeatureDrawer : public AbstractDrawer {
	public:
		FeatureDrawer(DrawerParameters *parms, const String& ty);
		~FeatureDrawer();
		void setDataSource(void *p);
		void prepare(PreparationParameters *pp);
	protected:
//		virtual void setColor(const BaseMap& mp, ILWIS::FeatureLayerDrawer *fdr);

		Feature *feature;
		Color color1;
		Color color2;
	};

}