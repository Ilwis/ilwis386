#pragma once

#include "DrawersUI\PointScalingTool.h"

ILWIS::DrawerTool *createPathScalingTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class PathScalingTool : public PointScalingTool {
public:
	PathScalingTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *drw);
};

}