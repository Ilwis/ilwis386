#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
//#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createAnimationDrawer(DrawerParameters *parms) {
	return new AnimationDrawer(parms);
}

AnimationDrawer::AnimationDrawer(DrawerParameters *parms) : 
	ComplexDrawer(parms,"AnimationDrawer")
{
	setTransparency(rUNDEF);
}

AnimationDrawer::~AnimationDrawer(){
}

void AnimationDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);
}

void AnimationDrawer::addDataSource(void *data, int options){
}

HTREEITEM AnimationDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	return ComplexDrawer::configure(tv,parent);
}

bool AnimationDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	return true;
}
