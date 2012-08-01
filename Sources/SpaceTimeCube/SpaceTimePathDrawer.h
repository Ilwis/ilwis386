#pragma once

#include "SortableDrawer.h"
#include "GroupableDrawer.h"
#include "SpaceTimeDrawer.h"

ILWIS::NewDrawer *createSpaceTimePathDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class _export SpaceTimePathDrawer : public SpaceTimeDrawer, public SortableDrawer, public GroupableDrawer {
	public:
		ILWIS::NewDrawer *createSpaceTimePathDrawer(DrawerParameters *parms);

		SpaceTimePathDrawer(DrawerParameters *parms);
		virtual ~SpaceTimePathDrawer();
		virtual void prepare(PreparationParameters *parms);

	protected:
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& currentSection);
		virtual void drawObjects(const int steps, GetHatchFunc getHatchFunc) const;
		virtual String getInfo(const Coord& c) const;
		vector<Feature *> features;
		vector<long> *objectStartIndexes;
	};
}