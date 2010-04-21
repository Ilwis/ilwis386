#pragma once

ILWIS::NewDrawer *createPointDrawer(ILWIS::DrawerContext *c);

namespace ILWIS{

class PointDrawer : public FeatureDrawer {
	public:
		PointDrawer(ILWIS::DrawerContext *c);
		virtual void draw(bool norecursion = false);
		void prepare(PreparationType t=ptALL,CDC *dc = 0);
	protected:
		Coord cNorm;

	};
}