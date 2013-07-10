#include "Client\Headers\AppFormsPCH.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\FormElements\DatFileLister.h"
#include "Client\Forms\frmmapcr.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "SEBSUI\FormMapSebs.h"
#include "Headers\Hs\Applications.hs"

InfoUIVector* getCommandInfoUI(ILWIS::Module *module) {
	InfoUIVector *infosui = new InfoUIVector();

	(*infosui).push_back(CommandHandlerUI::createCommandInfo("sebs","",Cmdmapsebs,TR("&SEBS Tools..&SEBS"),TR("Retrieve surface bio-geophysical parameters"),"ExePnt16IcoL",".tbt",4792,TR("Retrieve surface bio-geophysical parameters")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("landsurfacetemperature","",Cmdlandsurfacetemperature,TR("&SEBS Tools.Pre-processing.Land Surface &Temperature Computation"),TR("Compute land surface temperature"),"ExeMap16IcoL",".mpr",5062,TR("Compute land surface temperature")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("emissivity","",Cmdemissivity,TR("&SEBS Tools.Pre-processing.Land Surface &Emissivity Computation"),TR("Compute land surface temperature"),"ExeMap16IcoL",".mpr",5062,TR("Compute land surface temperature")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("albedo","",Cmdalbedo,TR("&SEBS Tools.Pre-processing.Land Surface &Albedo Computation"),TR("Compute land surface albedo"),"ExeMap16IcoL",".mpr",5062,TR("Compute land surface albedo")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("smac","",Cmdsmac,TR("&SEBS Tools.Pre-processing.&Atmospheric Correction (SMAC)"),TR("Atmospheric correction"),"ExeMap16IcoL",".mpr",5062,TR("Atmospheric correction")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("rawdata2radiance","",Cmdrawdata2radiance,TR("&SEBS Tools.Pre-processing.&Raw to radiances/reflectance (MODIS)"),TR("Convert raw data into radiances"),"ExeMap16IcoL",".mpr",5062,TR("Convert raw data into radiances")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("brightnesstemperature","",Cmdbrightnesstemperature,TR("&SEBS Tools.Pre-processing.&Brightness Temperature Computation"),TR("Compute brightness temperature"),"ExeMap16IcoL",".mpr",5062,TR("Compute brightness temperature")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("et0","",Cmdmapeto,TR("&SEBS Tools..&Reference ETo"),TR("FAO reference ETo calculation"),"ExeMap16IcoL",".tbt",4792,TR("FAO reference ETo estimation")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("soilmoisture","",Cmdsoilmoisture,TR("&SEBS Tools..&Soil moisture estimation using ASCAT L1 data"),TR("Soil moisture estimation using ASCAT L1 data"),"ExeMap16IcoL",".tbt",4792,TR("Soil moisture estimation using ASCAT L1 data")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("radiance2reflectance","",Cmdmapradiance2reflectance,TR("&SEBS Tools.Pre-processing.&Radiances to reflectance (ASTER)"),TR("Convert radiances to reflectance"),"ExeMap16IcoL",".mpr",5052,TR("Convert radiance to reflectance")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("watervapour","",Cmdwatervapour,"&SEBS Tools.Pre-processing.&Water Vapour Computation","Compute water vapour","ExeMap16IcoL",".mpr",5062,"Compute water vapour"));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("soilmoisture_ascat_l2","",Cmdsoilmoisture_ascat_l2,TR("&SEBS Tools..&ASCAT L2 soil moisture products"),TR("Soil moisture from ASCAT L1 data"),"ExeMap16IcoL",".tbt",4792,TR("ASCAT L2 soil moisture products")));
	
	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("SEBS UI", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
