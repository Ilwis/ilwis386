#pragma once

class FieldColor;

ILWIS::NewDrawer *createLineDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

	struct LineProperties : public GeneralDrawerProperties {
		LineProperties(double t=1.0, int st=0xFFFF, Color clr=Color(0,0,0)) : thickness(t),linestyle(st),drawColor(clr), ignoreColor(false) {}
		LineProperties(LineProperties *lp) { thickness=lp->thickness; linestyle=lp->linestyle;drawColor=lp->drawColor;ignoreColor=lp->ignoreColor;}

		double thickness;
		int linestyle;
		Color drawColor;
		bool ignoreColor;
	};

	class _export LineDrawer : public SimpleDrawer {
	public:
		LineDrawer(ILWIS::DrawerParameters *parms);
		~LineDrawer();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setSpecialDrawingOptions(int option, bool add, vector<Coord>* coords);
		virtual void addCoords(const vector<Coord>& v,int options=0);
		GeneralDrawerProperties *getProperties();

	protected:
		LineDrawer(DrawerParameters *parms, const String& name);
		bool findSelectedPoint(const Coord& c) const;
		void clear();
		void shareVertices(vector<Coord *>& coords);
		void drawSelectedFeature(CoordinateSequence *points, const CoordBounds& cbZoom, bool is3D) const;

		vector<CoordinateSequence *> lines;
		vector<Coord> selectedCoords;
		CoordBounds cb;
		LineProperties lproperties;

	};





}