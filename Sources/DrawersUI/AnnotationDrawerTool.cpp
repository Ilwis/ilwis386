#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\SetDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\AnnotationDrawerTool.h"
#include "DrawersUI\AnnotationLegendDrawerTool.h"
#include "DrawersUI\GlobalTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\LayerDrawerTool.h"

using namespace ILWIS;

DrawerTool *createAnnotationDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnnotationDrawerTool(zv, view, drw);
}

AnnotationDrawerTool::AnnotationDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool(TR("AnnotationDrawerTool"),zv, view, drw)
{
	active= false;
}

AnnotationDrawerTool::~AnnotationDrawerTool() {
}

void AnnotationDrawerTool::clear() {
}

bool AnnotationDrawerTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);
	if (!layerDrawerTool)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM AnnotationDrawerTool::configure( HTREEITEM parentItem) {
	htiNode = insertItem(parentItem, TR("Annotations"),"Annotation");
	DrawerTool *dt = new AnnotationLegendDrawerTool(mpv,tree,drawer);
	if ( dt) {
		addTool(dt);
		dt->configure(htiNode);
	}
	DrawerTool::configure(htiNode);


	return htiNode;
}

String AnnotationDrawerTool::getMenuString() const {
	return "Annotations";
}

