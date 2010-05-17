#include "headers\toolspch.h"
#include "Drawer_n.h"
#include "Engine\Map\Feature.h"
#include "Engine\Map\Basemap.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"

using namespace ILWIS;

FeatureDrawer::FeatureDrawer(DrawerParameters *parms, const String& ty) : AbstractDrawer(parms, ty){
	//clr = Color(0,176,20);
 //   clr0 = Color(168,168,168);
	//IlwisSettings settings("DefaultSettings");
	//clr0 = settings.clrValue("NoColor", clr0);
	//clr = settings.clrValue("YesColor", clr);
}

FeatureDrawer::~FeatureDrawer() {
}

void FeatureDrawer::setDataSource(void *p){
	feature = (Feature *)p;
}
