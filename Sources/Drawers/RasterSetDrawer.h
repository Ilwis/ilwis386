#pragma once

ILWIS::NewDrawer *createRasterSetDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

	class _export RasterSetDrawer : public SetDrawer {
	public:
		RasterSetDrawer(DrawerParameters *parms);
		virtual ~RasterSetDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap, int options=0);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		bool draw(bool norecursion , const CoordBounds& cbArea) const;
		
	protected:


	};
}