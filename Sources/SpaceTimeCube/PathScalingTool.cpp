#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "drawers\pointdrawer.h"
#include "PathScalingTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\PointSymbolizationTool.h"
#include "SpaceTimePathDrawer.h"

DrawerTool *createPathScalingTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new PathScalingTool(zv, view, drw);
}

PathScalingTool::PathScalingTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: PointScalingTool(zv, view, drw)
{
	type = "PathScalingTool";
}

bool PathScalingTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !ldrwt )
		return false;
	SpaceTimePathDrawer *pdrw = dynamic_cast<SpaceTimePathDrawer *>(drawer);
	if ( !pdrw)
		return false;

	SpatialDataDrawer *spdrw = (SpatialDataDrawer *)(drawer->getParentDrawer());
	BaseMapPtr *bmptr = spdrw->getBaseMap();
	parentTool = tool;
	if ( bmptr->dm()->pdv()) {
		return true;
	}
	if ( bmptr->fTblAtt()) {
		Table tblAtt = bmptr->tblAtt();
		if ( tblAtt.fValid()) {
			for(int i = 0; i < tblAtt->iCols(); ++i) {
				if ( tblAtt->col(i)->dm()->pdv()) {
					tbl = tblAtt;
					return true;
				}
			}
		}
	}
	return false;
}

