#pragma once

ILWIS::NewDrawer *createPointDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class SVGElement;

class PointDrawer : public SimpleDrawer {
	public:
		PointDrawer(ILWIS::DrawerParameters *parms);
		~PointDrawer();
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setDrawColor(const Color& col);
		Color getDrawColor() const;
		void setSymbol(const String& sym);
		String getSymbol() const;
		void shareVertices(vector<Coord *>& coords);

	protected:
		PointDrawer(DrawerParameters *parms, const String& name);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent) ;

		Coord cNorm;
		Color drawColor;
		SVGElement *drw;
		String drwId;
		String symbol;
	};
}