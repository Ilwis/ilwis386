#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\CollectionDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\CollectionTool.h"
#include "Client\Base\Framewin.h"
#include "Client\Mapwindow\MapWindow.h"

using namespace ILWIS;


DrawerTool *createCollectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new CollectionTool(zv, view, drw);
}

CollectionTool::CollectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	SetDrawerTool("CollectionTool",zv, view, drw)
{
	for(int i=0; i < 4; ++i) 
		visible[i] = 1;
}

CollectionTool::~CollectionTool() {
}

void CollectionTool::clear() {
}

bool CollectionTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<CollectionDrawer *>(drw) != 0;
}

HTREEITEM CollectionTool::configure( HTREEITEM parentItem) {
	DisplayOptionTree *item = new DisplayOptionTree(tree,parentItem,drawer, this);
	htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv",(LayerTreeItem *)item);
	int nrSegMaps, nrRasMaps, nrPolMaps, nrPntMaps;
	nrSegMaps = nrRasMaps = nrPolMaps = nrPntMaps = 0;
	ObjectCollection *oc = (ObjectCollection *)drawer->getDataSource();
	if (!oc)
		return parentItem;
	for(int i = 0; i < (*oc)->iNrObjects(); ++i) {
		FileName fn = (*oc)->fnObject(i);
		if ( IOTYPE(fn) == IlwisObject::iotSEGMENTMAP)
			++nrSegMaps;
		if ( IOTYPE(fn) == IlwisObject::iotPOLYGONMAP)
			++nrPolMaps;
		if ( IOTYPE(fn) == IlwisObject::iotPOINTMAP)
			++nrPntMaps;
		if ( IOTYPE(fn) == IlwisObject::iotRASMAP)
			++nrRasMaps;
	}
	HTREEITEM htiNodeGroup = insertItem(htiNode, TR("Collective tools"), ".ioc");
	if ( nrPntMaps > 0) {
		createLayersNode(htiNodeGroup,TR("Point layers"), ".mpp","PointSymbolizationTool",0);
	}
	if ( nrSegMaps > 0) {
		createLayersNode(htiNodeGroup,TR("Segment layers"), ".mps","LineSetTool",1);
	}
	if ( nrPolMaps > 0) {
		createLayersNode(htiNodeGroup,TR("Polygon layers"), ".mpa","PolygonSetTool",2);
	}
	if ( nrRasMaps > 0) {
		createLayersNode(htiNodeGroup,TR("Raster layers"), ".mpr","LayerDrawerTool",3);
	}
	//DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tree,htiNode,drawer);
	htiNode = insertItem(htiNode, TR("Individual tools"), "Setcol",0);
	

	//DrawerTool::configure(htiNode);

	return parentItem;
}

void CollectionTool::createLayersNode(HTREEITEM htiNodeGroup, const String& name, const String& icon, const String& tool, int index) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,htiNodeGroup,drawer);
	item->setCheckAction(this,0, (DTSetCheckFunc)&CollectionTool::setcheckLayer);
	layerItems[index] = insertItem(TR(name), icon,item,1);
	DrawerTool *dt = DrawerTool::createTool(tool, getDocument()->mpvGetView(),tree,drawer);
	if ( dt) {
		addTool(dt);
		dt->configure(layerItems[index]);
	}
}

void CollectionTool::setcheckLayer(void *w, HTREEITEM item) {
	bool yesno = *(bool *)w;
	for(int i = 0; i < 4; ++i) {
		if (layerItems[i] == item)
			visible[i] = yesno;
	}


	CollectionDrawer *colDrawer = (CollectionDrawer *)drawer;
	for(int i=0; i < colDrawer->getDrawerCount(); ++i) {
		if ( colDrawer->getDrawer(i)->getType() == "PolygonLayerDrawer") {
			colDrawer->getDrawer(i)->setActive(visible[2] != 0);
		} else if (colDrawer->getDrawer(i)->getType() == "LineLayerDrawer") {
			colDrawer->getDrawer(i)->setActive(visible[1] != 0);
		} else if (colDrawer->getDrawer(i)->getType() == "PointLayerDrawer") {
			colDrawer->getDrawer(i)->setActive(visible[0] != 0);
		} else if (colDrawer->getDrawer(i)->getType() == "RasterLayerDrawer") {
			colDrawer->getDrawer(i)->setActive(visible[3] != 0);
		}
	}
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

String CollectionTool::getMenuString() const {
	return TR("Set");
}

