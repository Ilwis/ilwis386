#include "Headers\toolspch.h"
#include "Engine\Base\System\module.h"
#include "Engine\Base\DataObjects\Version.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Map\Raster\Map.h"
#include "SegmentApplications\SEGATTRB.H"
//#include "SegmentApplications\SEGCLEAN.H"
#include "SegmentApplications\SEGTUNNL.H"
#include "SegmentApplications\SEGLABEL.H"
#include "SegmentApplications\SEGMASK.H"
#include "SegmentApplications\SEGPLBND.H"
#include "SegmentApplications\CNFRMRSV.H"
#include "SegmentApplications\SEGTRNSF.H"
#include "SegmentApplications\SEGSUBMP.H"
#include "SegmentApplications\SEGDENSF.H"
#include "SegmentApplications\SEGGLUE.H"
#include "SegmentApplications\SEGNUMB.H"
#include "SegmentApplications\SegBound.H"
#include "SegmentApplications\SGFRMRSP.H"
#include "SegmentApplications\segUnion.H"
#include "SegmentApplications\segDifference.H"
#include "SegmentApplications\segSymDifference.H"
#include "SegmentApplications\segIntersect.H"
#include "SegmentApplications\segVoronoi.H"
#include "SegmentApplications\segRelate.H"
#include "SegmentApplications\segTin.h"

InfoVector* getCommandInfo() {

	InfoVector *infos = new InfoVector();
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapAttribute,"SegmentMapAttribute"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapFromRasAreaBnd,"SegmentMapFromRasAreaBnd"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapTunneling,"SegmentMapTunneling"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapLabels,"SegmentMapLabels"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapMask,"SegmentMapMask"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapPolBoundaries,"SegmentMapPolBoundaries"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapFromRasValueBnd,"SegmentMapFromRasValueBnd"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapTransform,"SegmentMapTransform"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapSubMap,"SegmentMapSubMap"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapDensifyCoords,"SegmentMapDensifyCoords"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapGlue,"SegmentMapGlue"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapNumbering,"SegmentMapNumbering"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapIntersect,"SegmentMapIntersect"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapUnion,"SegmentMapUnion"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapDifference,"SegmentMapDifference"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapSymetricDifference,"SegmentMapSymetricDifference"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapVoronoi,"SegmentMapVoronoi"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapRelate,"SegmentMapRelate"));
	infos->push_back(CommandMap::newCommandInfo(createSegmentMapTIN,"SegmentMapTIN"));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Segmentmap applications", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getCommandInfo);  

	return module;
}



