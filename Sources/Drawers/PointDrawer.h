#pragma once

ILWIS::NewDrawer *createPointDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class PointDrawer : public FeatureDrawer {
	public:
		PointDrawer(ILWIS::DrawerParameters *parms);
		virtual void draw(bool norecursion = false);
		void prepare(PreparationType t=ptALL,CDC *dc = 0);
	protected:
		Coord cNorm;

	};
}