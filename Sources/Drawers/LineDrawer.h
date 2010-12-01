#pragma once

class FieldColor;

ILWIS::NewDrawer *createLineDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{


class LineDrawer : public SimpleDrawer {
	public:
		LineDrawer(ILWIS::DrawerParameters *parms);
		~LineDrawer();
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setDrawColor(const Color& col);
		void setThickness(float t);
		void setLineStyle(int style);

	protected:
		LineDrawer(DrawerParameters *parms, const String& name);
		bool findSelectedPoint(const Coord& c) const;
		void setSpecialDrawingOptions(SpecialDrawingOptions option, bool add, vector<Coord>* coords);
		void clear();
		void shareVertices(vector<Coord *>& coords);
		void drawSelectedFeature(CoordinateSequence *points, const CoordBounds& cbZoom, bool is3D) const;

		vector<CoordinateSequence *> lines;
		vector<Coord> selectedCoords;
		Color drawColor;
		CoordBounds cb;
		float thickness;
		int linestyle;
	};





}