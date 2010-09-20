#pragma once

ILWIS::NewDrawer *createSVGSetDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

	class _export SVGSetDrawer : public SetDrawer {
	public:
		SVGSetDrawer(DrawerParameters *parms);
		virtual ~SVGSetDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap, int options=0);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		bool draw(bool norecursion , const CoordBounds& cbArea) const;
		
	protected:


	};
}