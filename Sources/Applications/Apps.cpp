#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Map\Raster\Map.h"
#include "Applications\Raster\MAPFILTR.H"
#include "Applications\Raster\MAPCLASS.H"
#include "Applications\Raster\MAPCOLSP.H"
#include "Applications\Raster\SEGRAS.H"
#include "Applications\Raster\POLRAS.H"
#include "Applications\Raster\MAPARN.H"
#include "Applications\Raster\MAPDENSF.H"
#include "Applications\Raster\MAPAGGR.H"
#include "Applications\Raster\MAPMRROT.H"
#include "Applications\Raster\MAPHECK.H"
#include "Applications\Raster\MAPCLUST.H"
#include "Applications\Raster\MapMaplistStatistics.h"
#include "Applications\Raster\MAPCLCMP.H"
#include "Applications\Raster\MAPDIST.H"
#include "Applications\Raster\MAPSUB.H"
#include "Applications\Raster\MAPTHIES.H"
#include "Applications\Raster\MAPCROSS.H"
#include "Applications\Raster\MAPSLICE.H"
#include "Applications\Raster\MAPSTRCH.H"
#include "Applications\Raster\Mappntrs.h"
#include "Applications\Raster\MAPNRPNT.H"
#include "Applications\Raster\MAPMVAVG.H"
#include "Applications\Raster\MAPMVSRF.H"
#include "Applications\Raster\MAPTRNDS.H"
#include "Applications\Raster\Mapkrig.h"
#include "Applications\Raster\Mapresmp.h"
#include "Applications\Raster\MAP3D.H"
#include "Applications\Raster\MAPITPCN.H"
#include "Applications\Raster\MAPATTRB.H"
#include "Applications\Raster\MPSEGDNS.H"
#include "Applications\Raster\MAPCOKRI.H"
#include "Applications\Raster\variosrf.h"
#include "Applications\Raster\variosfm.h"
#include "Applications\Raster\MapComputedElsewhere.h"
#include "Applications\Raster\MapKrigingFromRaster.h"
#include "Applications\Raster\MapVisibility.h"
#include "Applications\Raster\MAPITER.H"
#include "Applications\Raster\MAPCALC.H"
#include "Applications\Table\COLAGGR.H"
#include "Applications\Table\COLSVM.H"
#include "Applications\Table\COLCUM.H"
#include "Applications\Table\COLLSF.H"
#include "Applications\Table\TblBursaWolf.h"
#include "Applications\Table\Tblcrvar.h"
#include "Applications\Table\tblchdom.h"
#include "Applications\Table\Tblcross.h"
#include "Applications\Table\TblGlue.h"
#include "Applications\Table\TBLTRNSP.H"
#include "Applications\Table\Spatcorr.h"
#include "Applications\Table\SEGDIR.H"
#include "Applications\Table\PATTANAL.H"
#include "Applications\Table\Neighpol.h"
#include "Applications\MapList\MapListApplic.h"
#include "Applications\MapList\MapListCalculate.h"
#include "Applications\MapList\MapListMatrixMultiply.h"
#include "Applications\Raster\MapAggregateMapList.h"
#include "Applications\Raster\Mapglue.h"
#include "Applications\Table\COLCALC.H"
#include "Applications\MapList\MapListChangeDetection.h"
#include "Applications\Table\TableCreateTimeColumn.h"
#include "Applications\Table\AUTCSEMV.H"

//extern "C" _export vector<ApplicationInfo *>* getApplicationInfo());

extern "C" _export void testApplicationInfo(ApplicationInfo* name) {
	name[0].createFunction = createMapAttribute;
	name[1].name = "MapAttribute";
}
InfoVector* getApplicationInfo() {

	InfoVector *infos = new InfoVector();

	infos->push_back(ApplicationMap::newApplicationInfo(createMapListChangeDetection,"MapListChangeDetection"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapAggregateMapList,"MapAggregateMapList"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapAttribute,"MapAttribute",metadataMapAtrribute));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapListMatrixMultiply,"MapListMatrixMultiply"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapVariogramSurfaceRas,"MapVariogramSurfaceRas"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapListCalculate,"MapListCalculate"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapListApplic,"MapListApplic"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapGlue,"MapGlue"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapFilter,"MapFilter"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapClassify,"MapClassify"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapColorSep,"MapColorSeparation"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapColorSep,"MapColorSep"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapRasterizeSegment,"MapRasterizeSegment"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapRasterizePolygon,"MapRasterizePolygon"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapAreaNumbering,"MapAreaNumbering"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapDensify,"MapDensify"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapAggregate,"MapAggregate___"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapMirrorRotate,"MapMirrorRotate"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapHeckbert,"MapHeckbert"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapCluster,"MapCluster"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapMaplistStatistics,"MapMaplistStatistics"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapColorComp,"MapColorComp___"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapDistance,"MapDistance"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapSubMap,"MapSubMap___"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapThiessen,"MapThiessen"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapCross,"MapCross"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapSlicing,"MapSlicing", metadataSlicing));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapStretch,"MapStretch___"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapRasterizePoint,"MapRasterizePoint___"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapNearestPoint,"MapNearestPoint"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapMovingAverage,"MapMovingAverage"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapMovingSurface,"MapMovingSurface"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapTrendSurface,"MapTrendSurface"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapResample,"MapResample"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapApply3D,"MapApply3D"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapInterpolContour,"MapInterpolContour"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapSegmentDensity,"MapSegmentDensity"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapKriging,"MapKriging___"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapCoKriging,"MapCoKriging"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapVariogramSurfacePnt,"MapVariogramSurfacePnt"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapComputedElsewhere,"MapComputedElsewhere"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapKrigingFromRaster,"MapKrigingFromRaster"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapVisibility,"MapVisibility"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapIterator,"MapIterator"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapIterator,"MapIterProp"));
	infos->push_back(ApplicationMap::newApplicationInfo(createMapCalculate,"MapCalculate"));
	infos->push_back(ApplicationMap::newApplicationInfo(createColumnCalculate,"ColumnCalculate"));
	infos->push_back(ApplicationMap::newApplicationInfo(createColumnAggregate,"ColumnAggregate___"));
	infos->push_back(ApplicationMap::newApplicationInfo(createColumnAggregate,"agg___"));
	infos->push_back(ApplicationMap::newApplicationInfo(createColumnSemiVariogram,"ColumnSemiVariogram"));
	infos->push_back(ApplicationMap::newApplicationInfo(createColumnCumulative,"ColumnCumulative"));
	infos->push_back(ApplicationMap::newApplicationInfo(createColumnLeastSquaresFit,"ColumnLeastSquaresFit"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableCrossVarioGram,"TableCrossVarioGram"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableBursaWolf,"TableBursaWolf"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableChangeDomain,"TableChangeDomain"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableCross,"TableCross"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableGlue,"TableGlue"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableGlue,"TableGlue"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableSpatCorr,"TableSpatCorr"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableSegDir,"TableSegDir"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTablePattAnal,"TablePattAnal"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableNeighbourPol,"TableNeighbourPol"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableTranspose,"TableTranspose"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableAutoCorrSemiVar,"TableAutoCorrSemiVar"));
	infos->push_back(ApplicationMap::newApplicationInfo(createTableCreateTimeColumn,"TableCreateTimeColumn"));

	return infos;

}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Rastermap and Table Applications", "IlwisApp.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifGetAppInfo, (void *)getApplicationInfo);  

	return module;
}



