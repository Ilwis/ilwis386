#pragma once

ILWIS::NewDrawer *createRasterDataDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

	class _export RasterDataDrawer : public SpatialDataDrawer {
	public:
		RasterDataDrawer(DrawerParameters *parms);
		virtual ~RasterDataDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap, int options=0);
		bool draw(int drawerIndex , const CoordBounds& cbArea) const;
		
	protected:
		void addLayerDrawer(const BaseMap& basemap, ILWIS::PreparationParameters *pp, ILWIS::LayerDrawer *rsd, const String& name="");
	};
}