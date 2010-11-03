#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\Module.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Engine\Map\basemap.h"
#include "Client\ilwis.h"
#include "Client\Editors\map\FeatureSetEditor.h"
#include "PointSetEditor.h"


using namespace ILWIS;


FSEditors *createMapEditors() {
	FSEditors *infos = new FSEditors();
	infos->push_back(new FeatureSetEditorInfo("PointSetEditor","ilwis38",createPointSetEditor));
	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("MapEditors", "MapEditors.dll",ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifMapEditors, (void *)createMapEditors);  

	return module;
}