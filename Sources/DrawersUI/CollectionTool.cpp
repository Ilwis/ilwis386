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
#include "Engine\Base\DataObjects\ObjectCollection.h"
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
}

CollectionTool::~CollectionTool() {
}

void CollectionTool::clear() {
}

bool CollectionTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 
	CollectionDrawer *coldrw = dynamic_cast<CollectionDrawer *>(drw);
	if ( !coldrw)
		return false;
	ObjectCollection *oc = (ObjectCollection *)coldrw->getDataSource();
	if ( !oc)
		return false;
	return true;
	
}

HTREEITEM CollectionTool::configure( HTREEITEM parentItem) {
	DisplayOptionTree *item = new DisplayOptionTree(tree,parentItem,drawer, this);
	htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv",(LayerTreeItem *)item);
	ObjectCollection *oc = (ObjectCollection *)drawer->getDataSource();
	type = IOTYPE((*oc)->fnObject(0));
	if (!oc)
		return parentItem;
	if ( type == IlwisObject::iotPOINTMAP) {
		createNode(htiNode,"PointSymbolizationTool");
	}
	if ( type == IlwisObject::iotSEGMENTMAP) {
		createNode(htiNode,"LineSetTool");
	}
	if ( type == IlwisObject::iotPOLYGONMAP) {
		createNode(htiNode,"PolygonSetTool");
	}
	if ( type == IlwisObject::iotRASMAP) {
		createNode(htiNode, "StretchTool");
		createNode(htiNode, "InteractiveRepresentationTool");
	}
	createNode(htiNode, "ColorTool");
	createNode(htiNode, "3DTool");

	return parentItem;
}

void CollectionTool::createNode(HTREEITEM hti, const String& name) {
	DrawerTool *dt = DrawerTool::createTool(name, getDocument()->mpvGetView(),tree,drawer);
	if (dt->isToolUseableFor(this)) {
		addTool(dt);
		dt->configure(hti);
	} else
		delete dt;
}

void CollectionTool::setcheckLayer(void *w, HTREEITEM item) {
	bool yesno = *(bool *)w;


	CollectionDrawer *colDrawer = (CollectionDrawer *)drawer;
	for(int i=0; i < colDrawer->getDrawerCount(); ++i) {
		colDrawer->getDrawer(i)->setActive(yesno);
	}
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

String CollectionTool::getMenuString() const {
	return TR(".ioc");
}

