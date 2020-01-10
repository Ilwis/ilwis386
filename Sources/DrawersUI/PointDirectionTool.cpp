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
#include "DrawersUI\PointDirectionTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\PointSymbolizationTool.h"
#include "Drawers\SetDrawer.h"
#include "Headers\Hs\Drwforms.hs"


DrawerTool *createPointDirectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new PointDirectionTool(zv, view, drw);
}

PointDirectionTool::PointDirectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("PointDirectionTool", zv, view, drw)
{
}

PointDirectionTool::~PointDirectionTool() {
}

bool PointDirectionTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	PointSymbolizationTool *pst = dynamic_cast<PointSymbolizationTool *>(tool);
	if ( !pst)
		return false;
	SpatialDataDrawer *spdrw = ((ComplexDrawer*)drawer)->isSet() ? static_cast<SpatialDataDrawer *>(drawer) : static_cast<SpatialDataDrawer *>(drawer->getParentDrawer());
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

HTREEITEM PointDirectionTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&PointDirectionTool::setScaling); 
	htiNode = insertItem(TR("Direction"),"Direction",item);
	DrawerTool::configure(htiNode);

	return htiNode;
}

void PointDirectionTool::setScaling() {
	new PointDirectionForm(tree, (PointLayerDrawer *)drawer, tbl);
}

String PointDirectionTool::getMenuString() const {
	return TR("Direction");
}

//---------------------------------------------------
PointDirectionForm::PointDirectionForm(CWnd *wPar, PointLayerDrawer *dr, const Table& _tbl):
DisplayOptionsForm(dr,wPar,TR("Rotation")), tbl(_tbl), fcColumn(0), fRotate(false), clockwise(true)
{
	SpatialDataDrawer *spdrw =  ((ComplexDrawer*)dr)->isSet() ? (SpatialDataDrawer*)dr : (SpatialDataDrawer *)(dr->getParentDrawer());
	BaseMapPtr *bmptr = spdrw->getBaseMap();
	if (dr->isSet()) {
		props = (PointProperties *)dr->getDrawer(0)->getProperties();
		inf = ((PointLayerDrawer*)dr->getDrawer(0))->getRotationInfo();
	} else {
		props = (PointProperties *)dr->getProperties();
		inf = dr->getRotationInfo();
	}
	fRotate = inf.rr.fValid();
	if ( tbl.fValid()) { // attribute table
		if (inf.rotationColumn == "" || !tbl->col(inf.rotationColumn).fValid()) { // overwrite inf.rr, regardless of its current stretch; it is based on a non-existing column
			for(int i = 0; i < tbl->iCols(); ++i) {
				if ( tbl->col(i)->dm()->pdv()) {
					inf.rotationColumn = tbl->col(i)->sName();
					inf.rr = tbl->col(i)->rrMinMax();
					break;
				}
			}
		} else if (!inf.rr.fValid()) {
			inf.rr = tbl->col(inf.rotationColumn)->rrMinMax();
		}
		CheckBox * cbRotate = new CheckBox(root, TR("Rotate"), &fRotate);
		fcColumn = new FieldColumn(cbRotate, TR("Attribute Column"), tbl, &(inf.rotationColumn), dmVALUE);
		fcColumn->SetCallBack((NotifyProc)&PointDirectionForm::ColValCallBack);
		fcColumn->Align(cbRotate, AL_UNDER);
		FieldBlank * fb = new FieldBlank(cbRotate);
		fb->Align(fcColumn, AL_UNDER);
		frr = new FieldRangeReal(cbRotate, TR("Rotation range"), &(inf.rr));
		frr->Align(fb, AL_UNDER);
		cbClockwise = new CheckBox(cbRotate,TR("Clockwise"),&(inf.clockwise));
		cbClockwise->Align(frr, AL_UNDER);
	} else if ( bmptr->dm()->pdv()) { // value-pointmap (no attribute table)
		if (!inf.rr.fValid())
			inf.rr = bmptr->rrMinMax();
		CheckBox * cbRotate = new CheckBox(root, TR("Rotate"), &fRotate);
		frr = new FieldRangeReal(cbRotate, TR("Rotation range"), &(inf.rr));
		frr->Align(cbRotate, AL_UNDER);
		cbClockwise = new CheckBox(cbRotate,TR("Clockwise"),&(inf.clockwise));
		cbClockwise->Align(frr, AL_UNDER);
	}

	create();
}

int PointDirectionForm::ColValCallBack(Event*) {
	if ( !fcColumn)
		return -1;

    String sOldCol = inf.rotationColumn;
    fcColumn->StoreData();
    if (sOldCol != inf.rotationColumn) {
      Column col = tbl->col(inf.rotationColumn);
      frr->SetVal(col->rrMinMax());
    }
    return 1;
}

void PointDirectionForm::apply(){
	root->StoreData();
	if (!fRotate)
		inf.rr = RangeReal();
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	if (drw->isSet()) {
		for(int i = 0; i < drw->getDrawerCount(); ++i) {
			PointLayerDrawer *psdrw = (PointLayerDrawer *) (drw->getDrawer(i));
			PointProperties *oldprops = (PointProperties *)psdrw->getProperties();
			oldprops->set(props);
			psdrw->setRotationInfo(inf);
			psdrw->prepare(&pp);
		}
	} else {
		PointLayerDrawer *pdr = (PointLayerDrawer *)drw;
		pdr->setRotationInfo(inf);
		drw->prepare(&pp);
	}

	updateMapView();
}

