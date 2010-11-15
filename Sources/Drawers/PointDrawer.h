#pragma once

#include "Client\Mapwindow\Drawers\SVGElements.h"

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
		static String defaultSymbol;
	protected:
		PointDrawer(DrawerParameters *parms, const String& name);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent) ;
		void calcSize();

		void drawRectangle(const SVGAttributes& attributes, double z) const;
		void drawEllipse(const SVGAttributes& attributes, double z) const;
		void drawLine(const SVGAttributes& attributes, double z) const;
		void drawPolygon(const SVGAttributes& attributes, double z) const;
		void drawPath(const SVGAttributes& attributes, double z) const;

		Coord cNorm;
		Color drawColor;
		const SVGElement *element;
		String symbol;
		double width, height;
	};
}