#pragma once

#include "SpaceTimeDrawer.h"

ILWIS::NewDrawer *createStationsDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class _export StationsDrawer : public SpaceTimeDrawer {
	public:
		ILWIS::NewDrawer *createStationsDrawer(DrawerParameters *parms);

		StationsDrawer(DrawerParameters *parms);
		virtual ~StationsDrawer();

	protected:
		virtual void drawObjects(const int steps, GetHatchFunc getHatchFunc) const;
	private:
	};
}