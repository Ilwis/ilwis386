#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Map\Raster\Map.h"
#include "PointApplications\PNTFRMRS.H"
#include "PointApplications\PNTATTRB.H"
#include "PointApplications\Pntpollb.h"
#include "PointApplications\PNTSGDST.H"
#include "PointApplications\PNTTRNSF.H"
#include "PointApplications\PNTSUB.H"
#include "PointApplications\PNTGLUE.H"
#include "PointApplications\PNTMASK.H"
#include "PointApplications\PNTNUMB.H"
#include "PointApplications\pntfrtbl.h"
#include "PointApplications\pntUnion.H"
#include "PointApplications\pntDifference.H"
#include "PointApplications\pntSymDifference.H"
#include "PointApplications\PointInSegment.H"
#include "PointApplications\pntIntersect.H"
#include "PointApplications\pntRelate.H"


//extern "C" _export vector<ApplicationInfo *>* getApplicationInfo());

InfoVector* getApplicationInfo() {

	InfoVector *infos = new InfoVector();

	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapFromRas,"PointMapFromRas"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapAttribute,"PointMapAttribute")); 
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapSeg,"PointMapSegDist")); 
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapSeg,"PointMapSegNodes")); 
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapSeg,"PointMapSegCoords")); 
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapPolLabels,"PointMapPolLabels")); 
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapTransform,"PointMapTransform"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapSubMap,"PointMapSubMap"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapGlue,"PointMapGlue"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapMask,"PointMapMask"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapNumbering,"PointMapNumbering"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapFromTable,"PointMapFromTable"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapIntersect,"PointMapIntersect"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapUnion,"PointMapUnion"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapDifference,"PointMapDifference"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapSymetricDifference,"PointMapSymetricDifference"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapPointInSegment,"PointMapPointInSegment"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPointMapRelate,"PointMapRelate"));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Pointmap Applications", "IlwisPointApps.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifGetAppInfo, (void *)getApplicationInfo);  

	return module;
}



