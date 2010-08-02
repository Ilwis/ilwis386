#include "Client\Headers\formelementspch.h"
#include "Client\Ilwis.h"
#include "Drawer_n.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h"
#include "Engine\Map\Feature.h"
#include "Engine\Map\Basemap.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"

using namespace ILWIS;

FeatureDrawer::FeatureDrawer(DrawerParameters *parms, const String& ty) : SimpleDrawer(parms, ty){
	red = 0;
	green = 0.69;
	blue = 0.08;
}

FeatureDrawer::~FeatureDrawer() {
}

void FeatureDrawer::addDataSource(void *p,int options){ 
	feature = (Feature *)p;
}


void FeatureDrawer::prepare(PreparationParameters *p) {
	SimpleDrawer::prepare(p);
}

HTREEITEM FeatureDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	return parent;
}

void FeatureDrawer::draw(bool norecursion) {
	
}

void FeatureDrawer::setColor(const Color& color) {
	red = (double)color.red() / 255.0;
	green = (double)color.green() / 255.0;
	blue = (double)color.blue() / 255.0;
}

void FeatureDrawer::setOpenGLColor() const {
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glColor4f(red,green,blue, getTransparency());
}