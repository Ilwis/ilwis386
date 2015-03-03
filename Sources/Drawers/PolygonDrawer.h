#pragma once

ILWIS::NewDrawer *createPolygonDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

struct _export PolygonProperties : public LineProperties {
	PolygonProperties();
	PolygonProperties(PolygonProperties *);
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);

	String hatchName;
};

class _export PolygonDrawer : public SimpleDrawer {
	//class LineDrawer;

	public:
		PolygonDrawer(ILWIS::DrawerParameters *parms);
		~PolygonDrawer();
		virtual bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setDrawColor(const Color& col);
		LineDrawer *getBoundaryDrawer();
		void setBoundaryDrawer(LineDrawer * _boundary);
		void areasActive(bool yesno);
		void boundariesActive(bool active);
		void setAreaAlpha(double v);
		void setlineStyle(int st);
		void setlineThickness(double th);
		void setLineColor(const Color& clr);
		virtual void select(bool yesno);
		GeneralDrawerProperties *getProperties();
		CoordBounds getBounds() const { return cb; }

	protected:
		virtual void setSpecialDrawingOptions(int option, bool add, const vector<int>& coords=vector<int>()) ;
		PolygonDrawer(DrawerParameters *parms, const String& name);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent) ;
		LineDrawer *boundary;

		vector<pair<unsigned int, vector<Coord>>> triangleStrips;
		Color drawColor;
		Color backgroundColor; // for hatching background
		CoordBounds cb;
		bool showArea;
		bool showBoundary;
		double areaAlpha;
		PolygonProperties properties;
		const byte *hatch;
		const byte *hatchInverse;
	};
}