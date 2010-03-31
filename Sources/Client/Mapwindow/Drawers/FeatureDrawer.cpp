#include "headers\toolspch.h"
#include "Drawer_n.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"

using namespace ILWIS;

FeatureDrawer::FeatureDrawer(DrawerContext *context, const String& ty) : AbstractDrawer(context, ty){
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