#pragma once

ILWIS::NewDrawer *createLineEditDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

	class _export LineEditDrawer : public LineDrawer {
	public:
		LineEditDrawer(DrawerParameters *parms);
		virtual ~LineEditDrawer();
		virtual void prepare(PreparationParameters *pp);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		bool draw(bool norecursion , const CoordBounds& cbArea) const;
		
	protected:


	};
}