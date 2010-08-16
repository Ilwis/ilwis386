#pragma once

class FieldColor;

ILWIS::NewDrawer *createLineDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{
//
//enum LineDspType { ldtNone,
//                   ldtSingle, ldtDouble, ldtTriple,
//                   ldtDot, ldtDash, ldtDashDot, ldtDashDotDot,
//                   ldtBlocked, ldtSymbol
//                 };

class LineDrawer : public SimpleDrawer {
	public:
		LineDrawer(ILWIS::DrawerParameters *parms);
		~LineDrawer();
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setDrawColor(const Color& col);
		void setThickness(float t);

	protected:
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		LineDrawer(DrawerParameters *parms, const String& name);
		void clear();

		vector<CoordinateSequence *> lines;
		Color drawColor;
		Feature *feature;
		CoordBounds cb;
		float thickness;
	};

}