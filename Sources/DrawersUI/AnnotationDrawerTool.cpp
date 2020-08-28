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
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Drawers\ColorCompositeDrawer.h"
//#include "AnnotationBorderTool.h"

using namespace ILWIS;

DrawerTool *createAnnotationDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnnotationDrawerTool(zv, view, drw);
}

AnnotationDrawerTool::AnnotationDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool(TR("AnnotationDrawerTool"),zv, view, drw)
{
	active = false;
}

AnnotationDrawerTool::~AnnotationDrawerTool() {
}

void AnnotationDrawerTool::clear() {
	DrawerTool::clear();
}

bool AnnotationDrawerTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);

	if (!layerDrawerTool && !setDrawerTool)
		return false;
	ComplexDrawer * cdrw = (ComplexDrawer*)drawer;
	if (cdrw->isSet()) {
		if (cdrw->getDrawerCount() > 0) {
			BaseMap *bmp = (BaseMap *)(cdrw->getDrawer(0)->getDataSource());
			if (!bmp || !bmp->fValid() || ((*bmp)->dm()->pdc() == 0 && (*bmp)->dm()->pdcol() == 0 && ((*bmp)->dm()->pdv() == 0 || (*bmp)->dm()->pdbool() != 0))) {
				ColorCompositeDrawer *rdrw = 0;
				if ( (rdrw = dynamic_cast<ColorCompositeDrawer *>(tool->getDrawer())) == 0)
					return false;
				if ( !rdrw->isColorComposite()) // accept ColorComposite
					return false;
			}
		} else
			return false; // empty maplist/objectcollection: no legend needed
	} else {
		BaseMap *bmp = (BaseMap *)tool->getDrawer()->getDataSource();
		if (!bmp || !bmp->fValid() || ((*bmp)->dm()->pdc() == 0 && (*bmp)->dm()->pdcol() == 0 && ((*bmp)->dm()->pdv() == 0 || (*bmp)->dm()->pdbool() != 0))) {
			ColorCompositeDrawer *rdrw = 0;
			if ( (rdrw = dynamic_cast<ColorCompositeDrawer *>(tool->getDrawer())) == 0)
				return false;
			if ( !rdrw->isColorComposite()) // accept ColorComposite
				return false;
		}
	}
	parentTool = tool;
	return true;
}

HTREEITEM AnnotationDrawerTool::configure( HTREEITEM parentItem) {
	htiNode = insertItem(parentItem, TR("Annotations"),"Annotation");
	DrawerTool::configure(htiNode);
	return htiNode;
}

String AnnotationDrawerTool::getMenuString() const {
	return "Annotations";
}

