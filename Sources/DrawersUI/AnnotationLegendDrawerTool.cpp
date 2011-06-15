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
#include "DrawersUI\AnnotationLegendDrawerTool.h"

using namespace ILWIS;

DrawerTool *createAnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnnotationLegendDrawerTool(zv, view, drw);
}

AnnotationLegendDrawerTool::AnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool(TR("AnnotationLegendDrawerTool"), zv, view, drw)
{
}

AnnotationLegendDrawerTool::~AnnotationLegendDrawerTool() {
}

void AnnotationLegendDrawerTool::clear() {
}

bool AnnotationLegendDrawerTool::isToolUseableFor(ILWIS::DrawerTool *drw) { 

	return false;
}

HTREEITEM AnnotationLegendDrawerTool::configure( HTREEITEM parentItem) {
	DrawerTool::configure(htiNode);
	htiNode = insertItem(parentItem, TR("Legend"),"legend");
	return htiNode;
}

String AnnotationLegendDrawerTool::getMenuString() const {
	return "Legend Annotation";
}

