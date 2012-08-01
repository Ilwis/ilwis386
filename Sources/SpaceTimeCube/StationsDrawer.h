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
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& currentSection);
		virtual void drawObjects(const int steps, GetHatchFunc getHatchFunc) const;
		String getInfo(const Coord& c) const;
	private:
	};
}