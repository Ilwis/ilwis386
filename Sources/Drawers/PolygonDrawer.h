#pragma once

ILWIS::NewDrawer *createPolygonDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class _export PolygonDrawer : public SimpleDrawer {
	//class LineDrawer;

	public:
		PolygonDrawer(ILWIS::DrawerParameters *parms);
		~PolygonDrawer();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
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
		virtual void setSpecialDrawingOptions(int option, bool add, const vector<int>& coords=vector<int>()) ;
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