#include "Client\Headers\AppFormsPCH.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Applications\MapListVirtual.h"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Client\Forms\frmtblcr.h"
#include "ColumnAttributeFromMapListForm.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Applications\colVIRT.H"
#include "TableAttributeFromMapList.h"
#include "MaplistFiltering\ColumnAttributeFromMapList.h"
#include "Engine\Map\Raster\Map.h"
#include "MapListCondensing.h"
#include "FormMapListCondensing.h"
#include "TableAttributeFromMapListForm.h"


//extern "C" _export vector<CommandInfo *>* getCommandInfo());

InfoVector* getCommandInfo() {

	InfoVector *infos = new InfoVector();
	infos->push_back(CommandMap::newCommandInfo(createTableMapListAttributeFrom,"TableMapListAttributeFrom",metadataTableMapListAttributeFrom));
	infos->push_back(CommandMap::newCommandInfo(createColumnAttributeFromMapList,"ColumnAttributeFromMapList"));
	infos->push_back(CommandMap::newCommandInfo(createMapListCondensing,"MapListCondensing"));


	return infos;

}

extern "C" _export InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("colattrfrommpl","",createColumnAttributeFromMapList,TR("&Raster Operations..Attribute column from map"),TR("Create attribute column"),"ExeTbl16IcoL",".tbl",0,TR("Create attribute column from maplist statistical information")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("tblattrfrommpl","",createTableFromMapList,TR("&Table Operations..Attribute column from map"),TR("Create attribute column"),"ExeTbl16IcoL",".tbl",0,TR("Create attribute column from maplist statistical information")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("aggmaplist","",CmdMapListCondensing,TR("&Raster Operations..Condense Map List"),TR("Condenses Maplist"),".mpl",".mpl",0,TR("Condenses a maplist to a smaller maplist using an selected aggregation method")));

	return infosui;
}


extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Filtering and Aggregation of MapLists", fnModule,ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getCommandInfo);
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI); 


	return module;
}



