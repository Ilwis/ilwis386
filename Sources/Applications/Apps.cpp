#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\Grid.h"
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
#include "Applications\Raster\MapColorFromRpr.h"
#include "Applications\Raster\MapParallaxCorrection.h"
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
#include "Applications\MapList\MapListExpand.h"
#include "Applications\Raster\Mapresmp2.h"
#include "Applications\Table\COLumntotime.H"
#include "Applications\Table\coltimefromcolumns.h"
#include "Applications\Table\TableNetworkDistance.h"
#include "Applications\Raster\MAPTOATTRB.H"


//extern "C" _export vector<CommandInfo *>* getCommandInfo());

extern "C" _export void testCommandInfo(CommandInfo* name) {
	name[0].createFunction = createMapAttribute;
	name[1].name = "MapAttribute";
}
InfoVector* getCommandInfo() {

	InfoVector *infos = new InfoVector();
		infos->push_back(CommandMap::newCommandInfo(createMapResample2,"MapResample2"));

	infos->push_back(CommandMap::newCommandInfo(createMapListExpand,"MapListExpand",metadataMapListExpand));
	infos->push_back(CommandMap::newCommandInfo(createMapListChangeDetection,"MapListChangeDetection",metadataMapListChangeDetection));
	infos->push_back(CommandMap::newCommandInfo(createMapAggregateMapList,"MapAggregateMapList"));
	infos->push_back(CommandMap::newCommandInfo(createMapAttribute,"MapAttribute",metadataMapAtrribute));
	infos->push_back(CommandMap::newCommandInfo(createMapListMatrixMultiply,"MapListMatrixMultiply",metadataMapListMatrixMultiply));
	infos->push_back(CommandMap::newCommandInfo(createMapVariogramSurfaceRas,"MapVariogramSurfaceRas",metadataMapVariogramSurfaceRas));
	infos->push_back(CommandMap::newCommandInfo(createMapListCalculate,"MapListCalculate"));
	infos->push_back(CommandMap::newCommandInfo(createMapListApplic,"MapListApplic"));
	infos->push_back(CommandMap::newCommandInfo(createMapGlue,"MapGlue",metadataMapGlue));
	infos->push_back(CommandMap::newCommandInfo(createMapFilter,"MapFilter",metadataMapFilter));
	infos->push_back(CommandMap::newCommandInfo(createMapClassify,"MapClassify",metadataMapClassify));
	infos->push_back(CommandMap::newCommandInfo(createMapColorSep,"MapColorSeparation",metadataMapColorSep));
	infos->push_back(CommandMap::newCommandInfo(createMapColorSep,"MapColorSep",metadataMapColorSep));
	infos->push_back(CommandMap::newCommandInfo(createMapRasterizeSegment,"MapRasterizeSegment",metadataMapRasterizeSegment));
	infos->push_back(CommandMap::newCommandInfo(createMapRasterizePolygon,"MapRasterizePolygon",metadataMapRasterizePolygon));
	infos->push_back(CommandMap::newCommandInfo(createMapAreaNumbering,"MapAreaNumbering",metadataMapAreaNumbering));
	infos->push_back(CommandMap::newCommandInfo(createMapDensify,"MapDensify",metadataMapDensify));
	infos->push_back(CommandMap::newCommandInfo(createMapAggregate,"MapAggregate___",metadataMapAggregate));
	infos->push_back(CommandMap::newCommandInfo(createMapMirrorRotate,"MapMirrorRotate",metadataMapMirrorRotate));
	infos->push_back(CommandMap::newCommandInfo(createMapHeckbert,"MapHeckbert",metadataMapHeckbert));
	infos->push_back(CommandMap::newCommandInfo(createMapCluster,"MapCluster",metadataMapCluster));
	infos->push_back(CommandMap::newCommandInfo(createMapMaplistStatistics,"MapMaplistStatistics"));
	infos->push_back(CommandMap::newCommandInfo(createMapColorComp,"MapColorComp___",metadataMapColorComp));
	infos->push_back(CommandMap::newCommandInfo(createMapDistance,"MapDistance",metadataMapDistance));
	infos->push_back(CommandMap::newCommandInfo(createMapSubMap,"MapSubMap___",metadataMapSubMap));
	infos->push_back(CommandMap::newCommandInfo(createMapThiessen,"MapThiessen",metadataMapThiessen));
	infos->push_back(CommandMap::newCommandInfo(createMapCross,"MapCross",metadataMapCross));
	infos->push_back(CommandMap::newCommandInfo(createMapSlicing,"MapSlicing", metadataMapSlicing));
	infos->push_back(CommandMap::newCommandInfo(createMapStretch,"MapStretch___",metadataMapStretch));
	infos->push_back(CommandMap::newCommandInfo(createMapRasterizePoint,"MapRasterizePoint___", metadataMapRasterizePoint));
	infos->push_back(CommandMap::newCommandInfo(createMapNearestPoint,"MapNearestPoint",metadataMapNearestPoint));
	infos->push_back(CommandMap::newCommandInfo(createMapMovingAverage,"MapMovingAverage",metadataMovingAverage));
	infos->push_back(CommandMap::newCommandInfo(createMapMovingSurface,"MapMovingSurface",metadataMapMovingSurface));
	infos->push_back(CommandMap::newCommandInfo(createMapTrendSurface,"MapTrendSurface",metadataMapTrendSurface));
	infos->push_back(CommandMap::newCommandInfo(createMapResample,"MapResample",metadataMapResample));
	infos->push_back(CommandMap::newCommandInfo(createMapToAttribute,"MapToAttribute",metadataMapToAtrribute));
	infos->push_back(CommandMap::newCommandInfo(createMapApply3D,"MapApply3D"));
	infos->push_back(CommandMap::newCommandInfo(createMapInterpolContour,"MapInterpolContour",metadataMapInterpolContour));
	infos->push_back(CommandMap::newCommandInfo(createMapSegmentDensity,"MapSegmentDensity",metadataMapSegmentDensity));
	infos->push_back(CommandMap::newCommandInfo(createMapKriging,"MapKrigingSimple",metadataMapKrigingSimple));
	infos->push_back(CommandMap::newCommandInfo(createMapKriging,"MapKrigingOrdinary",metadataMapKrigingOrdinary));
	infos->push_back(CommandMap::newCommandInfo(createMapCoKriging,"MapCoKriging"));
	infos->push_back(CommandMap::newCommandInfo(createMapVariogramSurfacePnt,"MapVariogramSurfacePnt"));
	infos->push_back(CommandMap::newCommandInfo(createMapComputedElsewhere,"MapComputedElsewhere"));
	infos->push_back(CommandMap::newCommandInfo(createMapKrigingFromRaster,"MapKrigingFromRaster"));
	infos->push_back(CommandMap::newCommandInfo(createMapVisibility,"MapVisibility"));
	infos->push_back(CommandMap::newCommandInfo(createMapIterator,"MapIterator",metadataMapIter));
	infos->push_back(CommandMap::newCommandInfo(createMapIterator,"MapIterProp"));
	infos->push_back(CommandMap::newCommandInfo(createMapCalculate,"MapCalculate"));
	infos->push_back(CommandMap::newCommandInfo(createMapColorFromRpr,"MapColorFromRpr"));
	infos->push_back(CommandMap::newCommandInfo(createMapParallaxCorrection,"MapParallaxCorrection"));
	infos->push_back(CommandMap::newCommandInfo(createColumnCalculate,"ColumnCalculate"));
	infos->push_back(CommandMap::newCommandInfo(createColumnAggregate,"ColumnAggregate___"));
	infos->push_back(CommandMap::newCommandInfo(createColumnAggregate,"agg___"));
	infos->push_back(CommandMap::newCommandInfo(createColumnToTime,"ColumnToTime"));
	infos->push_back(CommandMap::newCommandInfo(createColumnSemiVariogram,"ColumnSemiVariogram"));
	infos->push_back(CommandMap::newCommandInfo(createColumnCumulative,"ColumnCumulative"));
	infos->push_back(CommandMap::newCommandInfo(createColumnLeastSquaresFit,"ColumnLeastSquaresFit"));
	infos->push_back(CommandMap::newCommandInfo(createTableCrossVarioGram,"TableCrossVarioGram"));
	infos->push_back(CommandMap::newCommandInfo(createTableBursaWolf,"TableBursaWolf"));
	infos->push_back(CommandMap::newCommandInfo(createTableChangeDomain,"TableChangeDomain"));
	infos->push_back(CommandMap::newCommandInfo(createTableCross,"TableCross"));
	infos->push_back(CommandMap::newCommandInfo(createTableGlue,"TableGlue"));
	infos->push_back(CommandMap::newCommandInfo(createTableGlue,"TableGlue"));
	infos->push_back(CommandMap::newCommandInfo(createTableSpatCorr,"TableSpatCorr"));
	infos->push_back(CommandMap::newCommandInfo(createTableSegDir,"TableSegDir"));
	infos->push_back(CommandMap::newCommandInfo(createTablePattAnal,"TablePattAnal"));
	infos->push_back(CommandMap::newCommandInfo(createTableNeighbourPol,"TableNeighbourPol"));
	infos->push_back(CommandMap::newCommandInfo(createTableTranspose,"TableTranspose"));
	infos->push_back(CommandMap::newCommandInfo(createTableAutoCorrSemiVar,"TableAutoCorrSemiVar"));
	infos->push_back(CommandMap::newCommandInfo(createTableCreateTimeColumn,"TableCreateTimeColumn"));
	infos->push_back(CommandMap::newCommandInfo(createTableNetworkDistance,"Table2DimNetworkDistance"));
	infos->push_back(CommandMap::newCommandInfo(createColumnTimeFromColumns,"ColumnTimeFromColumns"));
	

	return infos;

}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Rastermap and Table Applications", fnModule,ILWIS::Module::mi38,"1.4");
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getCommandInfo);  

	return module;
}



