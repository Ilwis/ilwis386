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

	(*infosui).push_back(CommandHandlerUI::createCommandInfo("rawdata2radiance","",Cmdrawdata2radiance,TR("&SEBS Tools@01.Pre-processing.&Raw data to Radiance"),TR("Raw data to Radiance"),"ExeMap16IcoL",".mpr",5062,TR("Convert raw data to radiance values")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("radiance2reflectance","",Cmdmapradiance2reflectance,TR("&SEBS Tools@02.Pre-processing.&Radiance to TOA Reflectance"),TR("Radiance to TOA Reflectance"),"ExeMap16IcoL",".mpr",5052,TR("Convert radiance to reflectance (ASTER)")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("smac","",Cmdsmac,TR("&SEBS Tools@03.Pre-processing.&Atmospheric Correction (SMAC)"),TR("Atmospheric correction (SMAC)"),"ExeMap16IcoL",".mpr",5062,TR("Perform atmospheric correction")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("albedo","",Cmdalbedo,TR("&SEBS Tools@04.Pre-processing.&Land Surface Albedo"),TR("Land Surface Albedo"),"ExeMap16IcoL",".mpr",5062,TR("Compute land surface albedo")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("brightnesstemperature","",Cmdbrightnesstemperature,TR("&SEBS Tools@05.Pre-processing.&Brightness Temperature"),TR("Brightness Temperature"),"ExeMap16IcoL",".mpr",5062,TR("Compute brightness temperature")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("emissivity","",Cmdemissivity,TR("&SEBS Tools@06.Pre-processing.&Land Surface Emissivity"),TR("Land Surface Emissivity"),"ExeMap16IcoL",".mpr",5062,TR("Compute land surface emissivity")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("watervapour","",Cmdwatervapour,"&SEBS Tools@07.Pre-processing.&Water Vapour Computation",TR("Water Vapour Computation"),"ExeMap16IcoL",".mpr",5062,"Compute atmospheric water vapour"));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("landsurfacetemperature","",Cmdlandsurfacetemperature,TR("&SEBS Tools@08.Pre-processing.&Land Surface Temperature"),TR("Land Surface Temperature"),"ExeMap16IcoL",".mpr",5062,TR("Compute land surface temperature")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("sebs","",Cmdmapsebs,TR("&SEBS Tools@09.SEBS processing.&Surface Energy Balance System (SEBS)"),TR("Surface Energy Balance System (SEBS)"),"ExeMap16IcoL",".mpr",4792,TR("Retrieve surface bio-geophysical parameters")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("et0","",Cmdmapeto,TR("&SEBS Tools@10.SEBS related.&Reference ETo"),TR("Reference ETo"),"ExeMap16IcoL",".mpr",4792,TR("Compute FAO reference ETo estimation")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("soilmoisture","",Cmdsoilmoisture,TR("&SEBS Tools@11.Flux related.&Soil Moisture Estimation using ASCAT L1 data"),TR("Soil Moisture Estimation using ASCAT L1 data"),"ExeMap16IcoL",".mpr",4792,TR("Soil moisture estimation using ASCAT L1 data")));
	(*infosui).push_back(CommandHandlerUI::createCommandInfo("soilmoisture_ascat_l2","",Cmdsoilmoisture_ascat_l2,TR("&SEBS Tools@12.Flux related.&Soil Moisture Products from ASCAT L2"),TR("Soil Moisture Products from ASCAT L2"),"ExeMap16IcoL",".mpr",4792,TR("ASCAT L2 soil moisture products")));
	
	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("SEBS UI", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfoUI, (void *)getCommandInfoUI);  

	return module;
}
