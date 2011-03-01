#pragma once

ILWIS::NewDrawer *createRasterLayerDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

	class _export RasterLayerDrawer : public AbstractMapDrawer {
	public:
		RasterLayerDrawer(DrawerParameters *parms);
		virtual ~RasterLayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap, int options=0);
		bool draw(bool norecursion , const CoordBounds& cbArea) const;
		
	protected:
		void addSetDrawer(const BaseMap& basemap, ILWIS::PreparationParameters *pp, ILWIS::SetDrawer *rsd, const String& name="");
	};
}