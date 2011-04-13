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

																	   
//extern "C" _export vector<CommandInfo *>* getCommandInfo());

InfoVector* getCommandInfo() {	  

	InfoVector *infos = new InfoVector();   

	infos->push_back(CommandMap::newCommandInfo(createPolygonMapAttribute,"PolygonMapAttribute"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapFromSegment,"PolygonMapFromSegment"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapFromRas,"PolygonMapFromRas"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapLabels,"PolygonMapLabels"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapTransform,"PolygonMapTransform"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapMask,"PolygonMapMask"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapNumbering,"PolygonMapNumbering"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapGrid,"PolygonMapGrid"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapBuffer,"PolygonMapBuffer"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapConvexHull,"PolygonMapConvexHull"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapIntersect,"PolygonMapIntersect"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapUnion,"PolygonMapUnion"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapDifference,"PolygonMapDifference"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapSymetricDifference,"PolygonMapSymetricDifference"));
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapRelate,"PolygonMapRelate"));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Polygonmap Applications", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getCommandInfo);  

	return module;
}



