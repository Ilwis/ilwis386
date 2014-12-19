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
DisplayOptionsForm(dr,wPar,TR("Rotation")), tbl(_tbl), fcColumn(0), clockwise(true)
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
	if ( tbl.fValid() || bmptr->dm()->pdv()) {
		if ( !props->stretchRange.fValid()) {
			if ( tbl.fValid()) {
				if ( props->stretchColumn == "") {
					for(int i=0 ; i < tbl->iCols(); ++i) {
						if ( tbl->col(i)->dm()->pdv()) {
							props->stretchColumn = tbl->col(i)->sName();
							props->stretchRange = tbl->col(i)->dvrs().rrMinMax();
							break;
						}
					}
				} else {
					Column  col = tbl->col(props->stretchColumn);
					props->stretchRange = col->rrMinMax();

				}
			}else
				props->stretchRange = bmptr->rrMinMax();
		}
		if ( tbl.fValid()) {
			fcColumn = new FieldColumn(root, TR("Attribute Column"), tbl, &(inf.rotationColumn), dmVALUE);
			fcColumn->SetCallBack((NotifyProc)&PointDirectionForm::ColValCallBack);
		}
		inf.rr = props->stretchRange;

		new FieldBlank(root);

		frr = new FieldRangeReal(root, TR("Rotation range"), &(inf.rr));
		cbClockwise = new CheckBox(root,TR("Clockwise"),&(inf.clockwise));
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

