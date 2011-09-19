#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Raster\Map.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\LineLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "drawers\linefeaturedrawer.h"
#include "drawers\PointDrawer.h"
#include "drawers\PointFeatureDrawer.h"
#include "Engine\Base\Algorithm\triangulation\gpc.h"
#include "drawers\polygondrawer.h"
#include "drawers\polygonfeaturedrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\CanvasBackgroundDrawer.h"
#include "Drawers\RasterDataDrawer.h"
#include "Drawers\RasterDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Drawers\GridDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\Boxdrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "LayerDrawerTool.h"
#include "RepresentationTool.h"
#include "LegendTool.h"
#include "ColorTool.h"
#include "InteractiveRepresentationTool.h"
#include "StretchTool.h"
#include "PolygonSetTool.h"
#include "LineStyleTool.h"
#include "NonRepresentationTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "AnimationTool.h"
#include "DrawersUI\CollectionTool.h"
//#include "AnimationControlTool.h"
#include "AnimationTimeSelection.h"
#include "ThreeDTool.h"
#include "GridTool.h"
#include "ThreeDGlobalTool.h"
#include "AttributeTool.h"
#include "HighLightTool.h"
#include "BackgroundTool.h"
#include "PointSymbolizationTool.h"
#include "Grid3DTool.h"
#include "TransparencyTool.h"
#include "LineSetTool.h"
#include "FeatureSetEditor.h"
#include "FeatureSetEditor2.h"
#include "PointSetEditor.h"
#include "AnimationManagementTool.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "AnimationManagement.h"
#include "DrawersUI\ThreeDStack.h"
#include "GraticuleTool.h"
#include "LineSetEditor2.h"
#include "DrawersUI\GlobalTool.h"
#include "DistanceMeasurer.h"
#include "DrawersUI\CrossSectionTool.h"
#include "DrawersUI\TrackProfileTool.h"
#include "DrawersUI\PointScalingTool.h"
#include "DrawersUI\AnnotationDrawerTool.h"
#include "DrawersUI\AnnotationLegendDrawerTool.h"
#include "DrawersUI\PointDirectionTool.h"
#include "DrawersUI\HovMollerTool.h"
#include "InfoTool.h"


using namespace ILWIS;


DrawerToolInfoVector *createDrawerTool() {
	DrawerToolInfoVector *infos = new DrawerToolInfoVector();

	infos->push_back(new DrawerToolInfo("RepresentationTool",createRepresentationTool));
	infos->push_back(new DrawerToolInfo("LegendTool",createLegendTool));
	infos->push_back(new DrawerToolInfo("LayerDrawerTool",createLayerDrawerTool));
	infos->push_back(new DrawerToolInfo("ColorTool",createColorTool));
	infos->push_back(new DrawerToolInfo("InteractiveRepresentationTool",createInteractiveRepresentationTool));
	infos->push_back(new DrawerToolInfo("StretchTool",createStretchTool));
	infos->push_back(new DrawerToolInfo("PolygonSetTool",createPolygonSetTool));
	infos->push_back(new DrawerToolInfo("LineStyleTool",createLineStyleTool));
	infos->push_back(new DrawerToolInfo("NonRepresentationTool",createNonRepresentationToolTool));
	infos->push_back(new DrawerToolInfo("AnimationTool",createAnimationTool));
	
	infos->push_back(new DrawerToolInfo("AnimationTimeSelectionTool",createAnimationTimeSelectionTool));
	infos->push_back(new DrawerToolInfo("3DTool",createThreeDTool));
	infos->push_back(new DrawerToolInfo("GridTool",createGridTool));
	infos->push_back(new DrawerToolInfo("3DGlobal",createThreeDGlobalTool));
	infos->push_back(new DrawerToolInfo("AttributeTool",createAttributeTool));
	infos->push_back(new DrawerToolInfo("HighlightTool",createHighLightTool));
	infos->push_back(new DrawerToolInfo("BackgroundTool",createBackgroundTool));
	infos->push_back(new DrawerToolInfo("PointSymbolizationTool",createPointSymbolizationTool));
	infos->push_back(new DrawerToolInfo("Grid3DTool",createGrid3DTool));
	infos->push_back(new DrawerToolInfo("TransparencyTool",createTransparencyTool));
	infos->push_back(new DrawerToolInfo("LineSetTool",createLineSetTool));
	infos->push_back(new DrawerToolInfo("PointSetEditor",createPointSetEditor));
	infos->push_back(new DrawerToolInfo("LineSetEditor2",createLineSetEditor2));
	infos->push_back(new DrawerToolInfo("AnimationManagement",createAnimationManagementTool));
	infos->push_back(new DrawerToolInfo("CollectionTool",createCollectionTool));
	infos->push_back(new DrawerToolInfo("ThreeDStack",createThreeDStack));
	infos->push_back(new DrawerToolInfo("GraticuleTool",createGraticuleTool));
	infos->push_back(new DrawerToolInfo("GlobalTool",createGlobalTool));
	infos->push_back(new DrawerToolInfo("DistanceMeasurer",createMeasurerTool));
	infos->push_back(new DrawerToolInfo("CrossSectionTool",createCrossSectionTool));
	infos->push_back(new DrawerToolInfo("TrackProfileTool",createTrackProfileTool));
	infos->push_back(new DrawerToolInfo("PointScalingTool",createPointScalingTool));
	infos->push_back(new DrawerToolInfo("AnnotationDrawerTool",createAnnotationDrawerTool));
	infos->push_back(new DrawerToolInfo("PointDirectionTool",createPointDirectionTool));
	infos->push_back(new DrawerToolInfo("InfoTool",createInfoTool));
	infos->push_back(new DrawerToolInfo("HovMollerTool",createHovMollerTool));

	return infos;
}

extern "C" _export void moduleInit(ILWIS::Module *module) {
	if ( AnimationTool::animManagement == 0 ) {
			AnimationTool::animManagement = new AnimationPropertySheet();
			AnimationRun *page = new AnimationRun(*AnimationTool::animManagement);
			AnimationTool::animManagement->AddPage(page);
			AnimationSynchronization *page2 = new AnimationSynchronization(*AnimationTool::animManagement);
			AnimationTool::animManagement->AddPage(page2);
			AnimationProgress *page3 = new AnimationProgress(*AnimationTool::animManagement);
			AnimationTool::animManagement->AddPage(page3);
			RealTimePage *page4 = new RealTimePage(*AnimationTool::animManagement);
			AnimationTool::animManagement->AddPage(page4);
			AnimationTool::animManagement->Create(0,WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME );
			//AnimationTool::animManagement->ShowWindow(SW_HIDE);

	}

}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("DrawersUI", "IlwisDrawersUI.dll",ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifDrawerTools, (void *)createDrawerTool);
	module->addMethod(ILWIS::Module::ifInitUI, (void *)moduleInit);

	return module;
}