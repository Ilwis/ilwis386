#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Map\Raster\Map.h"
#include "PolygonApplications\POLATTRB.H"
#include "PolygonApplications\POLFRMSG.H"
#include "geos/operation/polygonize/Polygonizer.h"
#include "PolygonApplications\POLFRMRS.H"
#include "PolygonApplications\POLLABEL.H"
#include "PolygonApplications\POLTRNSF.H"
#include "PolygonApplications\POLMASK.H"
#include "PolygonApplications\POLNUMB.H"
#include "PolygonApplications\PolBuffer.H"
#include "PolygonApplications\ConvexHull.H"
#include "PolygonApplications\PolygonMapGrid.h"
#include "PolygonApplications\Union.H"
#include "PolygonApplications\Difference.H"
#include "PolygonApplications\SymDifference.H"
#include "PolygonApplications\Intersect.H"
#include "PolygonApplications\polRelate.H"

																	   
//extern "C" _export vector<ApplicationInfo *>* getApplicationInfo());

InfoVector* getApplicationInfo() {	  

	InfoVector *infos = new InfoVector();   

	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapAttribute,"PolygonMapAttribute"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapFromSegment,"PolygonMapFromSegment"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapFromRas,"PolygonMapFromRas"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapLabels,"PolygonMapLabels"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapTransform,"PolygonMapTransform"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapMask,"PolygonMapMask"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapNumbering,"PolygonMapNumbering"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapGrid,"PolygonMapGrid"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapBuffer,"PolygonMapBuffer"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapConvexHull,"PolygonMapConvexHull"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapIntersect,"PolygonMapIntersect"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapUnion,"PolygonMapUnion"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapDifference,"PolygonMapDifference"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapSymetricDifference,"PolygonMapSymetricDifference"));
	infos->push_back(ApplicationMap::newApplicationInfo(createPolygonMapRelate,"PolygonMapRelate"));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Polygonmap Applications", "IlwisPoygonApps.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifGetAppInfo, (void *)getApplicationInfo);  

	return module;
}



