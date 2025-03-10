#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\SetDrawer.h"
#include "Engine\Domain\Dmvalue.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include "Drawers\ThreeDTool.h"
#include "Drawers\SetDrawerTool.h"
#include "Drawers\AnimationTool.h"

using namespace ILWIS;

DrawerTool *createThreeDTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ThreeDTool(zv, view, drw);
}

ThreeDTool::ThreeDTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : DrawerTool("ThreeDTool", zv, view, drw)
{
	active = false;
}

ThreeDTool::~ThreeDTool() {
}

bool ThreeDTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	if ( !sdrwt && !adrwt)
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

void ThreeDTool::setExtrusion(void *value) {
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
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drw);
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( adrw) {
		sdrw = (SetDrawer *)adrw->getDrawer(0);
		bmp.SetPointer(((BaseMap *)sdrw->getDataSource())->ptr());
	} else {
		AbstractMapDrawer *fdrw = (AbstractMapDrawer *)sdrw->getParentDrawer();
		bmp.SetPointer(fdrw->getBaseMap());
	}
	if ( bmp->fTblAtt())
		attTable = bmp->tblAtt();
	rg = new RadioGroup(root,TR("Data Source"),&sourceIndex);
	new RadioButton(rg,"Self");
	RadioButton *rbMap = new RadioButton(rg,TR("Raster Map"));
	fmap = new FieldMap(rbMap,"",&mapName, new MapListerDomainType(dmVALUE|dmIMAGE));

	if ( attTable.fValid()) {
		RadioButton *rbTable = new RadioButton(rg,TR("Attribute column"));
		FieldColumn *fcol = new FieldColumn(rbTable,"",attTable,&colName,dmVALUE | dmIMAGE);
	}

	rg->SetIndependentPos();


	create();
	
}


void DisplayZDataSourceForm::apply() {
	rg->StoreData();
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drw);
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( adrw) {
		for(int i = 0 ; i < adrw->getDrawerCount(); ++i) {
			SetDrawer *sdrw = (SetDrawer *)adrw->getDrawer(i);
			updateDrawer(sdrw);
		}
	} else {
		updateDrawer(sdrw);
	}

	updateMapView();
}

void DisplayZDataSourceForm::updateDrawer(SetDrawer *sdrw) {
	if ( sourceIndex == 0) {
		sdrw->getZMaker()->setDataSourceMap(bmp);
		PreparationParameters pp(NewDrawer::pt3D);
		sdrw->prepare(&pp);
	}
	else if ( mapName != "" && sourceIndex == 1) {
		sdrw->getZMaker()->setDataSourceMap(BaseMap(FileName(mapName)));
		PreparationParameters pp(NewDrawer::pt3D);
		sdrw->prepare(&pp);
	} else if ( colName != "" && sourceIndex == 2) {
		sdrw->getZMaker()->setTable(attTable,colName);
		PreparationParameters pp(NewDrawer::pt3D);
		sdrw->prepare(&pp);
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
		RangeReal rr = dr->getZMaker()->getRange();
		ValueRangeReal vr(- ( rr.rHi() + rr.rLo()), rr.rWidth());
		zoffset -= rr.rLo();
		sliderOffset = new FieldRealSliderEx(root,"Z Offset", &zoffset,vr,true);
		sliderOffset->SetCallBack((NotifyProc)&ZDataScaling::settransforms);
		sliderOffset->setContinuous(true);
	}
	create();
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
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( adrw) {
		for(int i = 0 ; i < adrw->getDrawerCount(); ++i) {
			SetDrawer *sdrw = (SetDrawer *)adrw->getDrawer(i);
			sdrw->getZMaker()->setZScale(zscale/100.0);
			sdrw->getZMaker()->setOffset(zoffset + rr.rLo());
		}
	}
	updateMapView();
}
//----------------------------------------------------------------
ExtrusionOptions::ExtrusionOptions(CWnd *p, ComplexDrawer *drw) :
DisplayOptionsForm(drw, p, TR("Extrusion options") )
{
	SetDrawer *fdr = (SetDrawer *)drw;
	AnimationDrawer *animdrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( animdrw)
		fdr = ((SetDrawer *)animdrw->getDrawer(0));

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
	//((SetDrawer *)drw)->extrTransparency = 1.0 - (double)transparency/100.0;
	((SetDrawer *)drw)->setExtrustionTransparency(1.0 - (double)transparency/100.0);
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
	return 1;
}
void ExtrusionOptions::apply() {
	rg->StoreData();
	slider->StoreData();
	SetDrawer *fdr = (SetDrawer *)drw;
	AnimationDrawer *animDrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			SetDrawer *fdr = (SetDrawer *)animDrw->getDrawer(i);
			setFSDOptions(fdr);
		}
	}
	else {
		setFSDOptions(fdr);
	}

	updateMapView();

}

void ExtrusionOptions::setFSDOptions(SetDrawer *fsd) {
	if ( line == 0) {
		((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, true );
	    ((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, false );
		((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilled, false);
	}
	if (line == 1) {
		((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilled, true);
	    ((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, false );
		((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, false);
	}
	if (line == 2){
	    ((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoOpen, false );
		((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilled, false);
		((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFootPrint, true);
	}
	((SetDrawer *)drw)->setExtrustionTransparency(1.0 - (double)transparency/100.0);

}