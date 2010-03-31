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
#include "HydroFlowApplications\TblHortonPlots.h"


//extern "C" _export vector<ApplicationInfo *>* getApplicationInfo());

InfoVector* getApplicationInfo() {

	InfoVector *infos = new InfoVector();

	infos->push_back(ApplicationMap::newApplicationInfo(createMapFlowDirection,"MapFlowDirection"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapFlowAccumulation,"MapFlowAccumulation"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapDEMOptimization,"MapDEMOptimization"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapTopologicalOptimization,"MapTopologicalOptimization"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapVariableThresholdComputation,"MapVariableThresholdComputation"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapFillSinks,"MapFillSinks"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapDrainageNetworkExtraction,"MapDrainageNetworkExtraction"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapDrainageNetworkOrdering,"MapDrainageNetworkOrdering"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapOverlandFlowLength,"MapOverlandFlowLength"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapFlowLength2Outlet,"MapFlowLength2Outlet"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapCatchmentExtraction,"MapCatchmentExtraction"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapCatchmentMerge,"MapCatchmentMerge"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableHortonPlots,"TableHortonPlots"));

	return infos;

}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Hydrological flow Applications", "IlwisHydroFlowApps.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifGetAppInfo, (void *)getApplicationInfo);  

	return module;
}



