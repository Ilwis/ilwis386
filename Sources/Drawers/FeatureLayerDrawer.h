#pragma once

ILWIS::NewDrawer *createFeatureLayerDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{
class FeatureLayerDrawer : public AbstractDrawer {
	public:
		ILWIS::NewDrawer *createFeatureLayerDrawer(DrawerParameters *parms);

		FeatureLayerDrawer(DrawerParameters *parms);
		virtual ~FeatureLayerDrawer();
		virtual void prepare(PreparationType t=ptALL,CDC *dc = 0);
		void setDataSource(void *bm);
	protected:
		BaseMap basemap;

	};
}