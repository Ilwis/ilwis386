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

InfoUIVector* getApplicationInfoUI() {
	InfoUIVector *infosui = new InfoUIVector();

	(*infosui).push_back(CommandHandlerUI::createApplicationInfo("sebs","",Cmdmapsebs,SMENUmapsebs,SMENUSmapsebs,"ExePnt16IcoL",".tbt",4792,SMENUDmapsebs));
	(*infosui).push_back(CommandHandlerUI::createApplicationInfo("landsurfacetemperature","",Cmdlandsurfacetemperature,SMENUlandsurfacetemperature,SMENUSlandsurfacetemperature,"ExeMap16IcoL",".mpr",5062,SMENUDlandsurfacetemperature));
	(*infosui).push_back(CommandHandlerUI::createApplicationInfo("emissivity","",Cmdemissivity,SMENUemissivity,SMENUSemissivity,"ExeMap16IcoL",".mpr",5062,SMENUDemissivity));
	(*infosui).push_back(CommandHandlerUI::createApplicationInfo("albedo","",Cmdalbedo,SMENUalbedo,SMENUSalbedo,"ExeMap16IcoL",".mpr",5062,SMENUDalbedo));
	(*infosui).push_back(CommandHandlerUI::createApplicationInfo("smac","",Cmdsmac,SMENUsmac,SMENUSsmac,"ExeMap16IcoL",".mpr",5062,SMENUDsmac));
	(*infosui).push_back(CommandHandlerUI::createApplicationInfo("rawdata2radiance","",Cmdrawdata2radiance,SMENUrawdata2radiance,SMENUSrawdata2radiance,"ExeMap16IcoL",".mpr",5062,SMENUSrawdata2radiance));
	(*infosui).push_back(CommandHandlerUI::createApplicationInfo("brightnesstemperature","",Cmdbrightnesstemperature,SMENUbrightnesstemperature,SMENUSbrightnesstemperature,"ExeMap16IcoL",".mpr",5062,SMENUDbrightnesstemperature));
	(*infosui).push_back(CommandHandlerUI::createApplicationInfo("et0","",Cmdmapeto,SMENUFaoEto,SMENUSFaoEto,"ExeMap16IcoL",".tbt",4792,SMENUDFaoEto));
	(*infosui).push_back(CommandHandlerUI::createApplicationInfo("radiance2reflectance","",Cmdmapradiance2reflectance,SMENURad2Reflec,SMENUSRad2Reflec,"ExeMap16IcoL",".mpr",5052,SMENUDRad2Reflec));

	return infosui;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("SEBS UI", "SEBSUI.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifGetAppInfoUI, (void *)getApplicationInfoUI);  

	return module;
}
