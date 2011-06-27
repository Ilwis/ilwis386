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
	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setCheckAction(this, 0, (DTSetCheckFunc)&ThreeDTool::setExtrusion);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&ThreeDTool::extrusionOptions);
	insertItem("Extrusion","Extrusion",item, drawer->getSpecialDrawingOption(NewDrawer::sdoExtrusion));
	//insertItem("Axis", "Axis");
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
		drawer->setSpecialDrawingOptions(NewDrawer::sdoExtrusion | NewDrawer::sdoTOCHILDEREN | NewDrawer::sdoFilled,true);
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
	rg = new RadioGroup(root,TR("Data Source"),&sourceIndex);
	new RadioButton(rg,"Self");
	RadioButton *rbMap = new RadioButton(rg,TR("Raster Map"));
	fmap = new FieldMap(rbMap,"",&mapName, new MapListerDomainType(dmVALUE|dmIMAGE));
	rbMaplist = new RadioButton(rg,TR("Maplist"));
	fmaplist = new FieldMapList(rbMaplist, "", &mapName,new MapListerDomainType(dmVALUE|dmIMAGE)); 
	//if ( attTable.fValid()) {
		rbTable = new RadioButton(rg,TR("Attribute column"));
		FieldColumn *fcol = new FieldColumn(rbTable,"",attTable,&colName,dmVALUE | dmIMAGE);
	//}


	rg->SetIndependentPos();


	create();
	
}

int DisplayZDataSourceForm::initForm(Event *ev) {
	if ( GetSafeHwnd()) {
		if ( !attTable.fValid())
			rbTable->Disable();
		LayerDrawer *layerDrawer = dynamic_cast<LayerDrawer *>(drw);
		SetDrawer *adrw = dynamic_cast<SetDrawer *>(drw);
		if ( !adrw) {
			rbMaplist->Disable();
		}
	}
	return 1;
}

void DisplayZDataSourceForm::apply() {
	rg->StoreData();

	ILWIS::LayerDrawer *layerDrawer = dynamic_cast<ILWIS::LayerDrawer *>(drw);
	SetDrawer *setDrawer = dynamic_cast<SetDrawer *>(drw);
	if ( setDrawer) {
		setDrawer->getZMaker()->setRange(setDrawer->getStretchRangeReal());
		for(int i = 0 ; i < setDrawer->getDrawerCount(); ++i) {
			RasterLayerDrawer *layerDrawer = (RasterLayerDrawer *)setDrawer->getDrawer(i);
			MapList mpl;
			if ( sourceIndex == 0) {
				mpl = *((MapList *)(setDrawer->getDataSource())); 
			} else if ( sourceIndex == 1){
				updateDrawer(layerDrawer, BaseMap(FileName(mapName)));
				continue;
			} else if ( sourceIndex == 2){
				mpl = MapList(FileName(mapName));
			}
			Map mp = mpl[i];
			updateDrawer(layerDrawer, mp);
			/*RangeReal tempRange = mp->dvrs().rrMinMax();
			if ( tempRange.fValid()) {
				RangeReal rr = adrw->getZMaker()->getRange();
				rr += tempRange.rLo();
				rr += tempRange.rHi();
				adrw->getZMaker()->setRange(rr);
			}*/
		}
	} else {
		updateDrawer( layerDrawer, BaseMap(mapName));
	}

	updateMapView();
}

void DisplayZDataSourceForm::updateDrawer(LayerDrawer *layerDrawer, const BaseMap& basemap) {
	if ( mapName != "" && sourceIndex < 3) {
		layerDrawer->getZMaker()->setDataSourceMap(basemap);
		PreparationParameters pp(NewDrawer::pt3D);
		layerDrawer->prepare(&pp);
	} else if ( colName != "" && sourceIndex == 3) {
		layerDrawer->getZMaker()->setTable(attTable,colName);
		PreparationParameters pp(NewDrawer::pt3D);
		layerDrawer->prepare(&pp);
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

	if (dr->getZMaker()->getRange().fValid()) { 
		range = dr->getZMaker()->getRange();
		double rMin = min(-range.rHi(), range.rHi() * 2);
		double rMax = max(-range.rHi(), range.rHi() * 2);
		ValueRangeReal vr( rMin,rMax,range.rWidth()/20);
		zoffset -= abs(range.rLo());
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
	double rMin = min(-range.rHi(), range.rHi() * 2);
	double rMax = max(-range.rHi(), range.rHi() * 2);
	ValueRangeReal vr( rMin,rMax,range.rWidth()/20);
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
	RangeReal rr = drw->getZMaker()->getRange();
	drw->getZMaker()->setOffset(zoffset + rr.rLo());
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drw);
	if ( adrw) {
		rr = adrw->getZMaker()->getRange();
		for(int i = 0 ; i < adrw->getDrawerCount(); ++i) {
			LayerDrawer *layerDrawer = (LayerDrawer *)adrw->getDrawer(i);
			layerDrawer->getZMaker()->setRange(rr);
			layerDrawer->getZMaker()->setZScale(zscale/100.0);
			layerDrawer->getZMaker()->setOffset(zoffset + rr.rLo());
		}
	}
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

	transparency = 100 *(1.0-fdr->getExtrusionTransparency());
	if ( (fdr->getSpecialDrawingOption() &  NewDrawer::sdoOpen) != 0)
		line = 0;
	if ( (fdr->getSpecialDrawingOption() &  NewDrawer::sdoFilled) != 0)
		line = 1;
	if ( (fdr->getSpecialDrawingOption() &  NewDrawer::sdoFootPrint) != 0)
		line = 2;
	rg = new RadioGroup(root, TR("Appearence"),&line);
	new RadioButton(rg, TR("Line"));
	new RadioButton(rg,TR("Filled"));
	new RadioButton(rg,TR("Footprint"));
	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	slider->SetCallBack((NotifyProc)&ExtrusionOptions::setTransparency);
	slider->setContinuous(true);

	create();

}

int ExtrusionOptions::setTransparency(Event *ev) {
	slider->StoreData();
	//((LayerDrawer *)drw)->extrTransparency = 1.0 - (double)transparency/100.0;
	((LayerDrawer *)drw)->setExtrustionTransparency(1.0 - (double)transparency/100.0);
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
	return 1;
}
void ExtrusionOptions::apply() {
	rg->StoreData();
	slider->StoreData();
	LayerDrawer *fdr = (LayerDrawer *)drw;
	SetDrawer *animDrw = dynamic_cast<SetDrawer *>(drw);
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			LayerDrawer *fdr = (LayerDrawer *)animDrw->getDrawer(i);
			setFSDOptions(fdr);
		}
	}
	else {
		setFSDOptions(fdr);
	}

	updateMapView();

}

void ExtrusionOptions::setFSDOptions(LayerDrawer *fsd) {
	if ( line == 0) {
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, true );
	    ((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, false );
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilled, false);
	}
	if (line == 1) {
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilled, true);
	    ((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, false );
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, false);
	}
	if (line == 2){
	    ((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, false );
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilled, false);
		((LayerDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, true);
	}
	((LayerDrawer *)drw)->setExtrustionTransparency(1.0 - (double)transparency/100.0);

}