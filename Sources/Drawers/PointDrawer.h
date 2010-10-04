#pragma once

ILWIS::NewDrawer *createPointDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class PointDrawer : public SimpleDrawer {
	public:
		PointDrawer(ILWIS::DrawerParameters *parms);
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setDrawColor(const Color& col);
	
	protected:
		PointDrawer(DrawerParameters *parms, const String& name);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent) ;

		Coord cNorm;
		Color drawColor;
	};
}