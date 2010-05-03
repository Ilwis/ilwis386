#include "headers\toolspch.h"
#include "Drawer_n.h"
#include "Engine\Map\Feature.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"

using namespace ILWIS;

FeatureDrawer::FeatureDrawer(DrawerParameters *parms, const String& ty) : AbstractDrawer(parms, ty){
}

FeatureDrawer::~FeatureDrawer() {
}

void FeatureDrawer::setDataSource(void *p){
	feature = (Feature *)p;
}
void FeatureDrawer::setCoordinateSystem(const CoordSystem& cs) {
	csy = cs;
}

void FeatureDrawer::setDomain(const Domain& _dm) {
	dm = _dm;
}