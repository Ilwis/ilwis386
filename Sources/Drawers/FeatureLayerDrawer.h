#pragma once

ILWIS::NewDrawer *createFeatureLayerDrawer(ILWIS::DrawerContext *c);

namespace ILWIS{
class FeatureLayerDrawer : public AbstractDrawer {
	public:
		ILWIS::NewDrawer *createFeatureLayerDrawer(ILWIS::DrawerContext *c);

		FeatureLayerDrawer(DrawerContext *context);
		virtual ~FeatureLayerDrawer();
		virtual void prepare(PreparationType t=ptALL,CDC *dc = 0);
		void setDataSource(void *bm);
	protected:
		BaseMap basemap;

	};
}