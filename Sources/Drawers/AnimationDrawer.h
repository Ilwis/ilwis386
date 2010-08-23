#pragma once

namespace ILWIS{
	NewDrawer *createAnimationDrawer(DrawerParameters *parms);

	class _export AnimationDrawer : public ComplexDrawer {
	public:
		AnimationDrawer(DrawerParameters *parms);
		virtual ~AnimationDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap, int options=0);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		bool draw(bool norecursion , const CoordBounds& cbArea) const;
		
	protected:
		double interval;


	};
}