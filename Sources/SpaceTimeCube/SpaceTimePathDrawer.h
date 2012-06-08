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
		virtual void drawObjects(const int steps, GetHatchFunc getHatchFunc) const;
		vector<Feature *> features;
	};
}