#include "Client\Headers\formelementspch.h"
#include "client\formelements\fldlist.h"
#include "Client\FormElements\fldcol.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 

#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\LayerDrawer.h"

#include "Drawers\FeatureLayerDrawer.h"

#include "CubeTool.h"
#include "TimePositionBar.h"
#include "SpaceTimeCubeTool.h"
#include "SpaceTimePathDrawer.h"
#include "PathScalingTool.h"
#include "CubeDrawer.h"
#include "PreTimeOffsetDrawer.h"
#include "PostTimeOffsetDrawer.h"

using namespace ILWIS;

DrawerToolInfoVector *createDrawerTool() {
	DrawerToolInfoVector *infos = new DrawerToolInfoVector();
	infos->push_back(new DrawerToolInfo("SpaceTimeCubeTool",createSpaceTimeCubeTool));
	infos->push_back(new DrawerToolInfo("CubeTool",createCubeTool));
	infos->push_back(new DrawerToolInfo("PathScalingTool",createPathScalingTool));
	return infos;
}

DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("PointLayerDrawer","Cube",createSpaceTimePathDrawer));
	infos->push_back(new DrawerInfo("CubeDrawer","Cube",createCubeDrawer));
	infos->push_back(new DrawerInfo("PreTimeOffsetDrawer","Cube",createPreTimeOffsetDrawer));
	infos->push_back(new DrawerInfo("PostTimeOffsetDrawer","Cube",createPostTimeOffsetDrawer));
	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("SpaceTimeCube", fnModule,ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifDrawerTools, (void *)createDrawerTool);
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer);  
	return module;
}
