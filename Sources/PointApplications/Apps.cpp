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


//extern "C" _export vector<CommandInfo *>* getCommandInfo());

InfoVector* getCommandInfo() {

	InfoVector *infos = new InfoVector();

	infos->push_back(CommandMap::newCommandInfo(createPointMapFromRas,"PointMapFromRas"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapAttribute,"PointMapAttribute")); 
	infos->push_back(CommandMap::newCommandInfo(createPointMapSeg,"PointMapSegDist")); 
	infos->push_back(CommandMap::newCommandInfo(createPointMapSeg,"PointMapSegNodes")); 
	infos->push_back(CommandMap::newCommandInfo(createPointMapSeg,"PointMapSegCoords")); 
	infos->push_back(CommandMap::newCommandInfo(createPointMapPolLabels,"PointMapPolLabels")); 
	infos->push_back(CommandMap::newCommandInfo(createPointMapTransform,"PointMapTransform"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapSubMap,"PointMapSubMap"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapGlue,"PointMapGlue"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapMask,"PointMapMask"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapNumbering,"PointMapNumbering"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapFromTable,"PointMapFromTable"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapIntersect,"PointMapIntersect"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapUnion,"PointMapUnion"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapDifference,"PointMapDifference"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapSymetricDifference,"PointMapSymetricDifference"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapPointInSegment,"PointMapPointInSegment"));
	infos->push_back(CommandMap::newCommandInfo(createPointMapRelate,"PointMapRelate"));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Pointmap Applications", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getCommandInfo);  

	return module;
}



