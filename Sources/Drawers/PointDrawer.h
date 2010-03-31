#pragma once

ILWIS::NewDrawer *createPointDrawer(ILWIS::DrawerContext *c);

namespace ILWIS{

class PointDrawer : public FeatureDrawer {
	public:
		PointDrawer(ILWIS::DrawerContext *c);
		virtual void draw();
		void prepare(PreparationType t=ptALL);
	protected:
		Coord cNorm;

	};
}