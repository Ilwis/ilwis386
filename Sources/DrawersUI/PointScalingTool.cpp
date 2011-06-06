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
		for(int i = 0; i < tblAtt->iCols(); ++i) {
			if ( tblAtt->col(i)->dm()->pdv()) {
				tbl = tblAtt;
				return true;
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
	new PointScalingForm(tree, (PointLayerDrawer *)drawer, tbl);
}

String PointScalingTool::getMenuString() const {
	return TR("Size Scaling");
}

//---------------------------------------------------
PointScalingForm::PointScalingForm(CWnd *wPar, PointLayerDrawer *dr, const Table& _tbl):
DisplayOptionsForm(dr,wPar,TR("Scaling")), tbl(_tbl), fcColumn(0), rrScale(RangeReal(0.1,10)), scaleModel(0), stretchModel(0)
{
	props = (PointProperties *)dr->getProperties();
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
		fcColumn = new FieldColumn(root, TR("Attribute Column"), tbl, &(props->stretchColumn), dmVALUE);
		fcColumn->SetCallBack((NotifyProc)&PointScalingForm::ColValCallBack);
	}

    frr = new FieldRangeReal(root, SDCUiStretch, &(props->stretchRange));

   // fri = new FieldRangeReal(root, SDCUiSize, &rrScale, ValueRange(0.1,10));

	RadioGroup* rgLinLog = new RadioGroup(root, "", &stretchModel, true);
    rgLinLog->Align(frr, AL_UNDER);
    rgLinLog->SetIndependentPos();
    new RadioButton(rgLinLog, SDCUiLinear);
    new RadioButton(rgLinLog, SDCUiLogarithmic);
	new RadioButton(rgLinLog, TR("None"));
    RadioGroup* rgRadiusArea = new RadioGroup(root, "", &scaleModel, true);
    rgRadiusArea->SetIndependentPos();
    new RadioButton(rgRadiusArea, SDCUiRadius);
    new RadioButton(rgRadiusArea, SDCUiArea);

	create();
}

int PointScalingForm::ColValCallBack(Event*) {
	if ( !fcColumn)
		return -1;

    String sOldCol = sCol;
    fcColumn->StoreData();
    if (sOldCol != sCol) {
      Column col = tbl->col(sCol);
      frr->SetVal(col->rrMinMax());
    }
    return 1;
  }
void PointScalingForm::apply(){
	root->StoreData();
	if ( stretchModel == 0) {
		props->scaleMode = PointProperties::sLINEAR;
	} else if (stretchModel == 1)
		props->scaleMode = PointProperties::sLOGARITHMIC;
	else
		props->scaleMode = PointProperties::sNONE;
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drw->prepare(&pp);

	updateMapView();
}

