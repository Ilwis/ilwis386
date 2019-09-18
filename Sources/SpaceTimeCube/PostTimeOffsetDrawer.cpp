#include "Headers\toolspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "PostTimeOffsetDrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createPostTimeOffsetDrawer(DrawerParameters *parms) {
	return new PostTimeOffsetDrawer(parms);
}

PostTimeOffsetDrawer::PostTimeOffsetDrawer(DrawerParameters *parms)
: ComplexDrawer(parms,"PostTimeOffsetDrawer")
{
}

PostTimeOffsetDrawer::~PostTimeOffsetDrawer() {
}

void PostTimeOffsetDrawer::prepare(PreparationParameters *pp) {
}

bool PostTimeOffsetDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	if ( !isActive())
		return false;
	glPopMatrix();

	return true;
}
