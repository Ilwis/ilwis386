#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\Module.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Engine\Map\basemap.h"
#include "Client\ilwis.h"
#include "FeatureSetEditor.h"
#include "MapEditors\LineSetEditor.h"
#include "PointSetEditor.h"


using namespace ILWIS;


BMEditors *createMapEditors() {
	BMEditors *infos = new BMEditors();
	infos->push_back(new BaseMapEditorInfo("PointSetEditor","ilwis38",createPointSetEditor));
	infos->push_back(new BaseMapEditorInfo("LineSetEditor","ilwis38",createLineSetEditor));
	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("MapEditors", "MapEditors.dll",ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifMapEditors, (void *)createMapEditors);  

	return module;
}