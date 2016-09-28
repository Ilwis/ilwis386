#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "client\formelements\fldlist.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Engine\Domain\Dmvalue.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "DrawersUI\ThreeDTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "Drawers\RasterLayerDrawer.h"

using namespace ILWIS;

DrawerTool *createThreeDTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ThreeDTool(zv, view, drw);
}

ThreeDTool::ThreeDTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : DrawerTool("3DTool", zv, view, drw)
{
	active = false;
}

ThreeDTool::~ThreeDTool() {
}

bool ThreeDTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *layerDrawert = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *adrwt = dynamic_cast<SetDrawerTool *>(tool);
	if ( !layerDrawert && !adrwt)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM ThreeDTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	htiNode = insertItem("3D properties","3D",item);
	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&ThreeDTool::displayZOption3D);
	insertItem("Data source", ".mpv",item);
	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&ThreeDTool::displayZScaling);
	insertItem("Scaling", "ScaleBar",item);
	if ( drawer->getType() != "RasterLayerDrawer") {
		item = new DisplayOptionTreeItem(tree,htiNode,drawer);
		item->setCheckAction(this, 0, (DTSetCheckFunc)&ThreeDTool::setExtrusion);
		item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&ThreeDTool::extrusionOptions);
		insertItem("Extrusion","Extrusion",item, drawer->getSpecialDrawingOption(NewDrawer::sdoExtrusion));
	}
	DrawerTool::configure(htiNode);

	return htiNode;
}

void ThreeDTool::displayZOption3D() {
	new DisplayZDataSourceForm(tree, (ComplexDrawer *)drawer);
}

void ThreeDTool::setExtrusion(void *value, HTREEITEM) {
	bool v = *(bool *)value;
	int opt = drawer->getSpecialDrawingOption() & NewDrawer::sdoExtrusion;
	if ( opt == 0 && v)
		drawer->setSpecialDrawingOptions(NewDrawer::sdoExtrusion | NewDrawer::sdoTOCHILDEREN | NewDrawer::sdoFilledPlain,true);
	else
		drawer->setSpecialDrawingOptions(NewDrawer::sdoExtrusion | NewDrawer::sdoTOCHILDEREN, v);
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void ThreeDTool::extrusionOptions() {
	new ExtrusionOptions(tree, (ComplexDrawer *)drawer);
}

void ThreeDTool::displayZScaling() {
	new ZDataScaling(tree, (ComplexDrawer *)drawer);
}


String ThreeDTool::getMenuString() const {
	return TR("3D Properties");
}

//------------------------------------------------------
DisplayZDataSourceForm::DisplayZDataSourceForm(CWnd *wPar, ComplexDrawer *dr) : 
DisplayOptionsForm(dr,wPar,TR("3D Options")), sourceIndex(0) 
{
	root->SetCallBack((NotifyProc)&DisplayZDataSourceForm::initForm);
	LayerDrawer *layerDrawer = dynamic_cast<LayerDrawer *>(drw);
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drw);
	if ( adrw) {
		layerDrawer = (LayerDrawer *)adrw->getDrawer(0);
		SpatialDataDrawer *absdrw = (SpatialDataDrawer *)layerDrawer->getParentDrawer();
		bmp.SetPointer(absdrw->getBaseMap());
	} else {
		SpatialDataDrawer *fdrw = (SpatialDataDrawer *)layerDrawer->getParentDrawer();
		bmp.SetPointer(fdrw->getBaseMap());
	}
	if ( bmp->fTblAtt())
		attTable = bmp->tblAtt();
	mapName = bmp->fnObj.sRelative();
	sourceIndex = (int)layerDrawer->getZMaker()->getSourceType();
	rg = new RadioGroup(root,TR("Data Source"),&sourceIndex);
	new RadioButton(rg,TR("None"));
	rfeature = new RadioButton(rg,TR("Feature value"));
	zCoord = new RadioButton(rg,TR("Z coordinate"));
	RadioButton *rbMap = new RadioButton(rg,TR("Raster Map"));
	fmap = new FieldMap(rbMap,"",&mapName, new MapListerDomainType(dmVALUE|dmIMAGE));
	rbMaplist = new RadioButton(rg,TR("Maplist"));
	fmaplist = new FieldMapList(rbMaplist, "", &mapName,new MapListerDomainType(dmVALUE|dmIMAGE)); 
	rbTable = new RadioButton(rg,TR("Attribute column"));
	FieldColumn *fcol = new FieldColumn(rbTable,"",attTable,&colName,dmVALUE | dmIMAGE);

	rg->SetIndependentPos();

	create();	
}

int DisplayZDataSourceForm::initForm(Event *ev) {
	if ( GetSafeHwnd()) {
		zCoord->Disable();
		if ( !attTable.fValid())
			rbTable->Disable();
		LayerDrawer *layerDrawer = dynamic_cast<LayerDrawer *>(drw);
		RasterLayerDrawer *rldrw = dynamic_cast<RasterLayerDrawer *>(layerDrawer);
		if ( rldrw) {
			rfeature->Disable();
		}
		SetDrawer *adrw = dynamic_cast<SetDrawer *>(drw);
		if ( !adrw) {
			rbMaplist->Disable();
		}
		if ( bmp.fValid() && bmp->use3DCoordinates())
			zCoord->Enable();
	}
	return 1;
}

void DisplayZDataSourceForm::apply() {
	rg->StoreData();

	SetDrawer *setDrawer = dynamic_cast<SetDrawer *>(drw);
	if ( setDrawer) {
		RangeReal rrMinMax;
		for(int i = 0 ; i < setDrawer->getDrawerCount(); ++i) {
			LayerDrawer *layerDrawer = (LayerDrawer *)setDrawer->getDrawer(i);
			layerDrawer->getZMaker()->setSourceType((ZValueMaker::SourceType)sourceIndex);
			Map mp;
			if ( sourceIndex == 1) {
				MapList mpl = *((MapList *)(setDrawer->getDataSource())); 
				mp = mpl[i];
			} else if ( sourceIndex == 3){
				if (i == 0)
					updateDrawer(layerDrawer, BaseMap(FileName(mapName)));
				else
					layerDrawer->getZMaker()->addRange(BaseMap(FileName(mapName)));
				continue;
			} else if ( sourceIndex == 4){
				MapList mpl = MapList(FileName(mapName));
				mp = mpl[i];
			}
			if (i == 0)
				updateDrawer(layerDrawer, mp);
			else
				layerDrawer->getZMaker()->addRange(mp);
			rrMinMax += layerDrawer->getZMaker()->getZRange();
		}
		setDrawer->getZMaker()->setRange(rrMinMax);
		for(int i = 0 ; i < setDrawer->getDrawerCount(); ++i) {
			LayerDrawer *layerDrawer = (LayerDrawer *)setDrawer->getDrawer(i);
			PreparationParameters pp(NewDrawer::pt3D);
			layerDrawer->prepare(&pp);
		}
	} else {
		LayerDrawer * layerDrawer = dynamic_cast<ILWIS::LayerDrawer *>(drw);
		if (sourceIndex >= 0 && sourceIndex <= 2)
			updateDrawer( layerDrawer, bmp);
		else if (sourceIndex >= 3 && sourceIndex <= 4)
			updateDrawer( layerDrawer, BaseMap(mapName));
		else if (sourceIndex == 5)
			updateDrawer( layerDrawer, bmp);
		PreparationParameters pp(NewDrawer::pt3D);
		layerDrawer->prepare(&pp);
	}

	updateMapView();
}

void DisplayZDataSourceForm::updateDrawer(LayerDrawer *layerDrawer, const BaseMap& basemap) {
	layerDrawer->getZMaker()->setSourceType((ZValueMaker::SourceType)sourceIndex);
	if (sourceIndex >= 1 && sourceIndex <= 4) {
		layerDrawer->getZMaker()->setDataSourceMap(basemap);
	} else if ( colName != "" && sourceIndex == 5) {
		FeatureLayerDrawer * featureLayerDrawer = (FeatureLayerDrawer*)layerDrawer;
		layerDrawer->getZMaker()->setTable(((BaseMap*)featureLayerDrawer->getDataSource())->ptr()->tblAtt(),colName);
	}
}

//--------------------------------
ZDataScaling::ZDataScaling(CWnd *wPar, ComplexDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"Scaling and offset"),
zscale(dr->getZMaker()->getZScale() * 100),
zoffset(dr->getZMaker()->getOffset()),
sliderOffset(0) {
	sliderScale = new FieldRealSliderEx(root,"Z Scaling", &zscale,ValueRange(0,1000),true);
	sliderScale->SetCallBack((NotifyProc)&ZDataScaling::settransforms);
	sliderScale->setContinuous(true);

	if (dr->getZMaker()->getSetZRange().fValid()) { 
		range = dr->getZMaker()->getSetZRange();
		zoffset = dr->getZMaker()->getOffset();
		double rMin = min(-range.rHi(), range.rHi() * 2);
		double rMax = max(-range.rHi(), range.rHi() * 2);
		ValueRangeReal vr( rMin,rMax,range.rWidth()/20);
		if ( zoffset == rUNDEF) {
			zoffset -= abs(range.rLo());
		} else {
			CoordBounds cbLimits = dr->getZMaker()->getBounds();
			double zMaxSizeEstimate = (cbLimits.width() + cbLimits.height())/ 2.0;
			zoffset = zoffset * range.rWidth() / ( 0.25 * zMaxSizeEstimate);
		}
		frr = new FieldRangeReal(root, TR("Detected Value range"), &range);
		frr->SetCallBack((NotifyProc)&ZDataScaling::updateOffset);
		sliderOffset = new FieldRealSliderEx(root,"Z Offset", &zoffset,vr,true);
		sliderOffset->SetCallBack((NotifyProc)&ZDataScaling::settransforms);
		sliderOffset->setContinuous(true);
	}
	create();
}

int ZDataScaling::updateOffset(Event *ev) {
	frr->StoreData();
	double rMin = min(-range.rHi(), range.rHi() * 2.0);
	double rMax = max(-range.rHi(), range.rHi() * 2.0);
	ValueRangeReal vr( rMin,rMax,range.rWidth()/20.0);
	zoffset -= abs(range.rLo());
	sliderOffset->setValueRange(vr); 
	drw->getZMaker()->setRange(range);
	apply();
	return 1;
}

int ZDataScaling::settransforms(Event *) {
	apply();
	return 1;
}

void ZDataScaling::apply() {
	sliderScale->StoreData();
	if ( sliderOffset)
		sliderOffset->StoreData();
	drw->getZMaker()->setZScale(zscale/100.0);
	RangeReal rr = drw->getZMaker()->getSetZRange();
	drw->getZMaker()->setOffset(zoffset + rr.rLo());
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drw);
	ComplexDrawer * annotations = (ComplexDrawer *)drw->getRootDrawer()->getDrawer(800, ComplexDrawer::dtPOST);
	if ( annotations) {
		NewDrawer * grid = annotations->getDrawer(200, ComplexDrawer::dtPOST);
		if ( grid) {
			PreparationParameters pp(NewDrawer::ptRENDER);
			grid->prepare(&pp);
		}
	}
	if ( adrw) {
		rr = adrw->getZMaker()->getZRange();
		for(int i = 0 ; i < adrw->getDrawerCount(); ++i) {
			LayerDrawer *layerDrawer = (LayerDrawer *)adrw->getDrawer(i);
			layerDrawer->getZMaker()->setRange(rr);
			layerDrawer->getZMaker()->setZScale(zscale/100.0);
			layerDrawer->getZMaker()->setOffset(zoffset + rr.rLo());
		}
	}
	// Do not call prepare pt3D here, because the xyz OpenGL coordinates do not need recalculation.
	// A simple redraw is sufficient and much faster
	updateMapView();
}
//----------------------------------------------------------------
ExtrusionOptions::ExtrusionOptions(CWnd *p, ComplexDrawer *drw) :
DisplayOptionsForm(drw, p, TR("Extrusion options") )
{
	LayerDrawer *fdr = (LayerDrawer *)drw;
	SetDrawer *animdrw = dynamic_cast<SetDrawer *>(drw);
	if ( animdrw)
		fdr = ((LayerDrawer *)animdrw->getDrawer(0));

	transparency = 100 *(1.0-fdr->getExtrusionAlpha());
	if ( (fdr->getSpecialDrawingOption() &  NewDrawer::sdoOpen) != 0)
		line = 0;
	else if ( (fdr->getSpecialDrawingOption() &  NewDrawer::sdoFilledPlain) != 0)
		line = 1;
	else if ( (fdr->getSpecialDrawingOption() &  NewDrawer::sdoFilledShaded) != 0)
		line = 2;
	else if ( (fdr->getSpecialDrawingOption() &  NewDrawer::sdoFootPrint) != 0)
		line = 3;
	rg = new RadioGroup(root, TR("Appearence"),&line);
	new RadioButton(rg,TR("Line"));
	new RadioButton(rg,TR("Filled"));
	new RadioButton(rg,TR("Filled and Shaded"));
	new RadioButton(rg,TR("Footprint"));
	rg->SetCallBack((NotifyProc)&ExtrusionOptions::setExtrusionMode);
	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	slider->SetCallBack((NotifyProc)&ExtrusionOptions::setTransparency);
	slider->setContinuous(true);

	create();
}

int ExtrusionOptions::setTransparency(Event *ev) {
	slider->StoreData();
	((LayerDrawer *)drw)->setExtrusionAlpha(1.0 - (double)transparency/100.0);
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
	return 1;
}

int ExtrusionOptions::setExtrusionMode(Event *ev) {
	rg->StoreData();
	LayerDrawer *fdr = (LayerDrawer *)drw;
	SetDrawer *animDrw = dynamic_cast<SetDrawer *>(drw);
	if ( animDrw) {
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			LayerDrawer *fdr = (LayerDrawer *)animDrw->getDrawer(i);
			setFSDOptions(fdr);
		}
	}
	else
		setFSDOptions(fdr);
	updateMapView();
	return 1;
}

void ExtrusionOptions::apply() {
	rg->StoreData();
	slider->StoreData();
	LayerDrawer *fdr = (LayerDrawer *)drw;
	SetDrawer *animDrw = dynamic_cast<SetDrawer *>(drw);
	if ( animDrw) {
		//PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			LayerDrawer *fdr = (LayerDrawer *)animDrw->getDrawer(i);
			setFSDOptions(fdr);
		}
	}
	else {
		setFSDOptions(fdr);
	}
	fdr->setExtrusionAlpha(1.0 - (double)transparency/100.0);
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
}

void ExtrusionOptions::setFSDOptions(LayerDrawer *fsd) {
	if ( line == 0) {
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, true);
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilledPlain, false);
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilledShaded, false);
	    ((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, false);
	}
	if (line == 1) {
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, false);
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilledPlain, true);
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilledShaded, false);
	    ((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, false);
	}
	if (line == 2) {
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, false);
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilledPlain, false);
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilledShaded, true);
	    ((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, false);
	}
	if (line == 3){
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, false);
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilledPlain, false);
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilledShaded, false);
	    ((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, true);
	}
}