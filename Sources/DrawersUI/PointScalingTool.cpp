#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "Engine\Drawers\SVGLoader.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "drawers\pointdrawer.h"
#include "DrawersUI\PointScalingTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\PointSymbolizationTool.h"
#include "Drawers\SetDrawer.h"
#include "Headers\Hs\Drwforms.hs"


DrawerTool *createPointScalingTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new PointScalingTool(zv, view, drw);
}

PointScalingTool::PointScalingTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("PointScalingTool", zv, view, drw)
{
}

PointScalingTool::~PointScalingTool() {
}

bool PointScalingTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	PointSymbolizationTool *pst = dynamic_cast<PointSymbolizationTool *>(tool);
	if ( !pst)
		return false;
	SpatialDataDrawer *spdrw = (SpatialDataDrawer *)(drawer->getParentDrawer());
	BaseMapPtr *bmptr = spdrw->getBaseMap();
	parentTool = tool;
	if ( bmptr->dm()->pdv()) {
		return true;
	}
	if ( bmptr->fTblAtt()) {
		Table tblAtt = bmptr->tblAtt();
		if ( tblAtt.fValid()) {
			for(int i = 0; i < tblAtt->iCols(); ++i) {
				if ( tblAtt->col(i)->dm()->pdv()) {
					tbl = tblAtt;
					return true;
				}
			}
		}
	}
	return false;
}

HTREEITEM PointScalingTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&PointScalingTool::setScaling); 
	htiNode = insertItem(TR("Stretching"),"Scale",item);
	DrawerTool::configure(htiNode);

	return htiNode;
}

void PointScalingTool::setScaling() {
	new PointScalingForm(tree, (FeatureLayerDrawer *)drawer, tbl);
}

String PointScalingTool::getMenuString() const {
	return TR("Size Scaling");
}

//---------------------------------------------------
PointScalingForm::PointScalingForm(CWnd *wPar, FeatureLayerDrawer *dr, const Table& _tbl):
DisplayOptionsForm(dr,wPar,TR("Scaling")), tbl(_tbl), fcColumn(0), rrScale(RangeReal(0.1,10)), fStretch(false), scaleModel(0), stretchModel(0), fDisableCallBacks(true)
{
	props = (PointProperties *)dr->getProperties();
	fStretch = (props->scaleMode != PointProperties::sNONE);
	stretchModel = (props->scaleMode == PointProperties::sLOGARITHMIC) ? 1 : 0;
	scaleModel = (props->radiusArea == PointProperties::sAREA) ? 1 : 0;

	fmscale = new FieldReal(root,TR("Scale Exaggeration"),&(props->exaggeration)); 
	CheckBox * cbStretch = new CheckBox(root, TR("&Stretch"), &fStretch);

	if ( tbl.fValid()) {
		if ( !props->stretchRange.fValid()) {
			for(int i=0 ; i < tbl->iCols(); ++i) {
				if ( tbl->col(i)->dm()->pdv()) {
					props->stretchColumn = tbl->col(i)->sName();
					props->stretchRange = tbl->col(i)->dvrs().rrMinMax();
					break;
				}
			}
		}
		fcColumn = new FieldColumn(cbStretch, TR("Attribute Column"), tbl, &(props->stretchColumn), dmVALUE);
		fcColumn->Align(cbStretch, AL_UNDER);
		fcColumn->SetCallBack((NotifyProc)&PointScalingForm::ColValCallBack);
	}

    frr = new FieldRangeReal(cbStretch, TR("&Range"), &(props->stretchRange));
	if (fcColumn != 0)
		frr->Align(fcColumn, AL_UNDER);
	else
		frr->Align(cbStretch, AL_UNDER);   
	RadioGroup* rgLinLog = new RadioGroup(cbStretch, "", &stretchModel, true);
	rgLinLog->Align(frr, AL_UNDER);
    rgLinLog->SetIndependentPos();
    new RadioButton(rgLinLog, TR("&Linear"));
    new RadioButton(rgLinLog, TR("Lo&garithmic"));
    RadioGroup* rgRadiusArea = new RadioGroup(cbStretch, "", &scaleModel, true);
	rgRadiusArea->Align(rgLinLog, AL_UNDER);
    rgRadiusArea->SetIndependentPos();
    new RadioButton(rgRadiusArea, TR("&Radius"));
    new RadioButton(rgRadiusArea, TR("&Area"));

	create();

	fDisableCallBacks = false;
}

int PointScalingForm::ColValCallBack(Event*) {
	if ( fDisableCallBacks || !fcColumn)
		return -1;

    fcColumn->StoreData();
    Column col = tbl->col(props->stretchColumn);
	if ( col.fValid())
		frr->SetVal(col->rrMinMax());
    return 1;
}

void PointScalingForm::apply(){
	root->StoreData();
	fmscale->StoreData();
	if (!fStretch)
		props->scaleMode = PointProperties::sNONE;
	else if ( stretchModel == 0)
		props->scaleMode = PointProperties::sLINEAR;
	else if (stretchModel == 1)
		props->scaleMode = PointProperties::sLOGARITHMIC;
	else
		props->scaleMode = PointProperties::sNONE;
	if (scaleModel == 0)
		props->radiusArea = PointProperties::sRADIUS;
	else if (scaleModel == 1)
		props->radiusArea = PointProperties::sAREA;
	else
		props->radiusArea = PointProperties::sRADIUS;
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	RepresentationProperties rprop;
	rprop.symbolType = props->symbol;
	pp.props = &rprop;
	drw->prepare(&pp);

	updateMapView();
}

