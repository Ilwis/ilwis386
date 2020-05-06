#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Map\Raster\Map.h"
#include "HydroFlowApplications\MapFlowDirection.h"
#include "HydroFlowApplications\MapFlowAccumulation.h"
#include "HydroFlowApplications\MapDEMOptimization.h"
#include "HydroFlowApplications\MapTopologicalOptimization.h"
#include "HydroFlowApplications\MapVariableThresholdComputation.h"
#include "HydroFlowApplications\MapFillSinks.h"
#include "HydroFlowApplications\MapDrainageNetworkExtraction.h"
#include "HydroFlowApplications\MapDrainageNetworkOrdering.h"
#include "HydroFlowApplications\MapSlopeLengths.h"
#include "HydroFlowApplications\MapFlowLengthToOutlet.h"
#include "HydroFlowApplications\MapCatchmentExtraction.h"
#include "HydroFlowApplications\MapCatchmentMerge.h"
#include "HydroFlowApplications\TblFlowPathLonsitudinalProfile.h"
#include "HydroFlowApplications\TblHortonPlots.h"


//extern "C" _export vector<CommandInfo *>* getCommandInfo());

InfoVector* getCommandInfo() {

	InfoVector *infos = new InfoVector();

	infos->push_back(CommandMap::newCommandInfo(createMapFlowDirection,"MapFlowDirection"));
	infos->push_back(CommandMap::newCommandInfo(createMapFlowAccumulation,"MapFlowAccumulation"));
	infos->push_back(CommandMap::newCommandInfo(createMapDEMOptimization,"MapDEMOptimization"));
	infos->push_back(CommandMap::newCommandInfo(createMapTopologicalOptimization,"MapTopologicalOptimization"));
	infos->push_back(CommandMap::newCommandInfo(createMapVariableThresholdComputation,"MapVariableThresholdComputation"));
	infos->push_back(CommandMap::newCommandInfo(createMapFillSinks,"MapFillSinks"));
	infos->push_back(CommandMap::newCommandInfo(createMapDrainageNetworkExtraction,"MapDrainageNetworkExtraction"));
	infos->push_back(CommandMap::newCommandInfo(createMapDrainageNetworkOrdering,"MapDrainageNetworkOrdering"));
	infos->push_back(CommandMap::newCommandInfo(createMapOverlandFlowLength,"MapOverlandFlowLength"));
	infos->push_back(CommandMap::newCommandInfo(createMapFlowLength2Outlet,"MapFlowLength2Outlet"));
	infos->push_back(CommandMap::newCommandInfo(createMapCatchmentExtraction,"MapCatchmentExtraction"));
	infos->push_back(CommandMap::newCommandInfo(createMapCatchmentMerge,"MapCatchmentMerge"));
	infos->push_back(CommandMap::newCommandInfo(createTableFlowPathLongitudinalProfile,"tblFlowPathLongitudinalProfile"));
	infos->push_back(CommandMap::newCommandInfo(createTableHortonPlots,"TableHortonPlots"));

	return infos;

}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Hydrological flow Applications", fnModule,ILWIS::Module::mi37,"1.1");
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getCommandInfo);  

	return module;
}



