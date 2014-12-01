#include "Client\Headers\formelementspch.h"
#include "client\formelements\fldlist.h"
#include "Client\FormElements\fldcol.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 

#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\LayerDrawer.h"

#include "Drawers\FeatureLayerDrawer.h"

#include "TimePositionBar.h"
#include "SpaceTimeCubeTool.h"
#include "SpaceTimeElementsDrawer.h"
#include "SpaceTimePathTool.h"
#include "SpaceTimePathDrawer.h"
#include "StationsDrawer.h"
#include "SpaceTimeRoseDrawer.h"
#include "CubeDrawer.h"
#include "PreTimeOffsetDrawer.h"
#include "PostTimeOffsetDrawer.h"
#include "TimeZoomTool.h"
#include "CubeElementsTool.h"
#include "PathScalingTool.h"
#include "TimeProfileTool.h"
#include "ParallelCoordinatePlotTool.h"

using namespace ILWIS;

DrawerToolInfoVector *createDrawerTool() {
	DrawerToolInfoVector *infos = new DrawerToolInfoVector();
	infos->push_back(new DrawerToolInfo("SpaceTimeCubeTool",createSpaceTimeCubeTool));
	infos->push_back(new DrawerToolInfo("CubeElementsTool",createCubeElementsTool));
	infos->push_back(new DrawerToolInfo("SpaceTimePathTool",createSpaceTimePathTool));
	infos->push_back(new DrawerToolInfo("TimeZoomTool",createTimeZoomTool));
	infos->push_back(new DrawerToolInfo("PathScalingTool",createPathScalingTool));
	infos->push_back(new DrawerToolInfo("TimeProfileTool",createTimeProfileTool));
	infos->push_back(new DrawerToolInfo("ParallelCoordinatePlotTool",createParallelCoordinatePlotTool));
	return infos;
}

DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("PointLayerDrawer","Cube:<stp>",createSpaceTimePathDrawer));
	infos->push_back(new DrawerInfo("PointLayerDrawer","Cube:<stations>",createStationsDrawer));
	infos->push_back(new DrawerInfo("PointLayerDrawer","Cube:<rose>",createSpaceTimeRoseDrawer));
	infos->push_back(new DrawerInfo("CubeDrawer","Cube",createCubeDrawer));
	infos->push_back(new DrawerInfo("PreTimeOffsetDrawer","Cube",createPreTimeOffsetDrawer));
	infos->push_back(new DrawerInfo("PostTimeOffsetDrawer","Cube",createPostTimeOffsetDrawer));
	infos->push_back(new DrawerInfo("SpaceTimeElementsDrawer","Cube",createSpaceTimeElementsDrawer));
	return infos;
}

void moduleInit(ILWIS::Module *module) {
	getEngine()->getContext()->ComHandler()->AddCommand("stccommand", executeStcCommand);
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("SpaceTimeCube", fnModule,ILWIS::Module::mi38,"1.3");
	module->addMethod(ILWIS::Module::ifDrawerTools, (void *)createDrawerTool);
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer);
	module->addMethod(ILWIS::Module::ifInit, (void *)moduleInit);
	return module;
}
