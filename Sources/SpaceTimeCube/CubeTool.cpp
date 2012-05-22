#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "client\formelements\fldlist.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
//#include "Client\Ilwis.h"
//#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\MapPaneView.h"
#include "CubeTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "SpaceTimePathDrawer.h"
#include "Drawers\RasterLayerDrawer.h"


DrawerTool *createCubeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new CubeTool(zv, view, drw);
}

CubeTool::CubeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("CubeTool",zv, view, drw)
, spaceTimePathDrawer(0)
, pointMapDrawer(drw->getParentDrawer())
{
}

CubeTool::~CubeTool() {
	//if (spaceTimePathDrawer)
	//	drawer->getRootDrawer()->removeDrawer(spaceTimePathDrawer->getId(), true);
	if (spaceTimePathDrawer) {
		replaceDrawer(spaceTimePathDrawer, pointMapDrawer);
		delete spaceTimePathDrawer;
		drawer->setActive(true);
	}
	//drawer->setActive(true);
}

bool CubeTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	return false;

	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);
	if ( !ldrwt && !setDrawerTool)
		return false;
	PointLayerDrawer *pdrw;
	if ( setDrawerTool) {
		NewDrawer *drw = setDrawerTool->getDrawer();
		pdrw = dynamic_cast<PointLayerDrawer *>(((SetDrawer *)(drw))->getDrawer(0));
	} else
		pdrw = dynamic_cast<PointLayerDrawer *>(drawer);
	if ( !pdrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM CubeTool::configure( HTREEITEM parentItem) {
	DrawerParameters dp(drawer->getRootDrawer(), drawer->getParentDrawer());
	spaceTimePathDrawer = new SpaceTimePathDrawer(&dp);
	spaceTimePathDrawer->setActive(false);
	BaseMapPtr * bmp = ((SpatialDataDrawer*)(drawer->getParentDrawer()))->getBaseMap();
	BaseMap basemap;
	basemap.SetPointer(bmp);
	spaceTimePathDrawer->getZMaker()->setSpatialSource(basemap, ((SpatialDataDrawer*)(drawer->getParentDrawer()))->getRootDrawer()->getMapCoordBounds());
	spaceTimePathDrawer->getZMaker()->setDataSourceMap(basemap);
	//drawer->getRootDrawer()->addDrawer(spaceTimePathDrawer);

	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	// item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CubeTool::displayOptionTransparency);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CubeTool::displayOptionTemporalSource);
	item->setCheckAction(this,0,(DTSetCheckFunc )&CubeTool::makeActive);
	htiNode = insertItem(TR("Space Time Path"), "SpaceTimePath", item, 0);
	return htiNode;
}

void CubeTool::makeActive(void *v, HTREEITEM) {
	bool yesno = *(bool*)v;
	//drawer->setActive(!yesno);
	if (yesno) {
		drawer->setActive(false);
		replaceDrawer(pointMapDrawer, spaceTimePathDrawer);
	} else {
		replaceDrawer(spaceTimePathDrawer, pointMapDrawer);
		drawer->setActive(true);
	}
	//spaceTimePathDrawer->setActive(yesno);
		//BaseMapPtr * basemapPtr = ((SpatialDataDrawer*)drawer)->getBaseMap();
		//pp.csy = basemapPtr->cs();
		//if ( ((SpatialDataDrawer*)drawer)->getName() == "Unknown")
		//	spaceTimePathDrawer->setName(((SpatialDataDrawer*)drawer)->getName());
		//spaceTimePathDrawer->setRepresentation(basemapPtr->dm()->rpr()); //  default choice
		//BaseMap bmp(basemap);
		//spaceTimePathDrawer->addDataSource(basemapPtr);
		//spaceTimePathDrawer->getParentDrawer();
	if (yesno && !spaceTimePathDrawer->fIsPrepared()) {
		LayerDrawer *layerDrawer;
		SetDrawer *adrw = dynamic_cast<SetDrawer *>(drawer);
		if ( adrw)
			layerDrawer = (LayerDrawer *)adrw->getDrawer(0);
		else
			layerDrawer = dynamic_cast<LayerDrawer *>(drawer);

		PreparationParameters pp(NewDrawer::ptGEOMETRY, layerDrawer->getCoordSystem());
		spaceTimePathDrawer->prepare(&pp);
	}
	mpvGetView()->Invalidate();	
}

void CubeTool::replaceDrawer(NewDrawer * oldDrw, NewDrawer * newDrw)
{
	ComplexDrawer * rootDrawer = drawer->getRootDrawer();
	oldDrw->setActive(false);
	int index = rootDrawer->getDrawerIndex(oldDrw);
	if (iUNDEF != index) {
		rootDrawer->removeDrawer(oldDrw->getId(), false);
		rootDrawer->insertDrawer(index, newDrw);
		newDrw->setActive(true);
	}
	// replace the drawer-pointer in its tools-list
}

void CubeTool::displayOptionTemporalSource() {
	
}

String CubeTool::getMenuString() const {
	return TR("SpaceTimePath");
}
