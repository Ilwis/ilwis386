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

InfoVector* getApplicationInfo() {

	InfoVector *infos = new InfoVector();
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapAttribute,"SegmentMapAttribute"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapFromRasAreaBnd,"SegmentMapFromRasAreaBnd"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapTunneling,"SegmentMapTunneling"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapLabels,"SegmentMapLabels"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapMask,"SegmentMapMask"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapPolBoundaries,"SegmentMapPolBoundaries"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapFromRasValueBnd,"SegmentMapFromRasValueBnd"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapTransform,"SegmentMapTransform"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapSubMap,"SegmentMapSubMap"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapDensifyCoords,"SegmentMapDensifyCoords"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapGlue,"SegmentMapGlue"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapNumbering,"SegmentMapNumbering"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapIntersect,"SegmentMapIntersect"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapUnion,"SegmentMapUnion"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapDifference,"SegmentMapDifference"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapSymetricDifference,"SegmentMapSymetricDifference"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapVoronoi,"SegmentMapVoronoi"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapRelate,"SegmentMapRelate"));
	infos->push_back(ApplicationMap::newApplicationInfo(createSegmentMapTIN,"SegmentMapTIN"));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Segmentmap applications", "IlwisSegmentApps.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifGetAppInfo, (void *)getApplicationInfo);  

	return module;
}



