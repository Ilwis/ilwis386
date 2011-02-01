#pragma once

ILWIS::NewDrawer *createPolygonDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class PolygonDrawer : public SimpleDrawer {
	//class LineDrawer;

	public:
		PolygonDrawer(ILWIS::DrawerParameters *parms);
		~PolygonDrawer();
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setDrawColor(const Color& col);
		LineDrawer *getBoundaryDrawer();
		void areasActive(bool yesno);
		void boundariesActive(bool active);
		void setTransparencyArea(double v);
		void setlineStyle(int st);
		void setlineThickness(double th);
		void setLineColor(const Color& clr);

	protected:
		virtual void setSpecialDrawingOptions(int option, bool add, vector<Coord>* coords=NULL) ;
		PolygonDrawer(DrawerParameters *parms, const String& name);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent) ;
		LineDrawer *boundary;

		vector<vector<Coord> > triangleStrips;
		Color drawColor;
		CoordBounds cb;
		bool showArea;
		bool showBoundary;
		double areaTransparency;
	};
}