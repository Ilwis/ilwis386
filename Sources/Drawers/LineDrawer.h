#pragma once

ILWIS::NewDrawer *createLineDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class LineDrawer : public FeatureDrawer {
	public:
		LineDrawer(ILWIS::DrawerParameters *parms);
		~LineDrawer();
		virtual void draw(bool norecursion = false);
		void prepare(PreparationParameters *);
	protected:
		LineDrawer(DrawerParameters *parms, const String& name);

		CoordinateSequence *points;
		CoordBounds cb;

	};
}