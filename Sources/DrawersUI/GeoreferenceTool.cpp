#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "GeoReferenceTool.h"
#include "Client\FormElements\fldgrf.h"
#include "DrawersUI\GeometryTool.h"


DrawerTool *createGeoReferenceTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new GeoReferenceTool(zv, view, drw);
}

GeoReferenceTool::GeoReferenceTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("GeoReferenceTool",zv, view, drw)
{
}

GeoReferenceTool::~GeoReferenceTool() {
}

bool GeoReferenceTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	return dynamic_cast<GeometryTool *>(tool) != 0;
}

HTREEITEM GeoReferenceTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&GeoReferenceTool::displayOptionGeoReference);
	htiNode = insertItem(TR("GeoReference"),".grf", item);

	return htiNode;
}

void GeoReferenceTool::displayOptionGeoReference() {
	new GeoReferenceForm(tree, (ComplexDrawer *)drawer);
}

String GeoReferenceTool::getMenuString() const {
	return TR("GeoReference");
}

//---------------------------------------------------
GeoReferenceForm::GeoReferenceForm(CWnd *wPar, ComplexDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"GeoReference")
{
	GeoRef gr = dr->getRootDrawer()->getGeoReference();
	if (gr.fValid())
		name = gr->fnObj.sFullPath();
	fldGrf = new FieldGeoRefExisting(root,TR("GeoReference"),&name);
	create();
}

int GeoReferenceForm::setGeoReference(Event *ev) {
	apply();
	return 1;
}

FormEntry *GeoReferenceForm::CheckData() {
	fldGrf->StoreData();
	FileName fn(name);
	if ( !fn.fExist())
		return fldGrf;
	/*
	GeoRef grf(fn);
	bool fConvertPossible = grf->fConvertFrom(drw->getRootDrawer()->getGeoReference());
	if ( !fConvertPossible)
		return fldGrf;
	*/
	return 0;
}
void  GeoReferenceForm::apply() {
	fldGrf->StoreData();
	FileName fn(name);
	GeoRef grf(fn);
	//bool fConvertPossible = grf->fConvertFrom(drw->getRootDrawer()->getGeoReference());
	//if ( fConvertPossible) {
		drw->getRootDrawer()->setGeoreference(grf);
		view->ClearTree();
		PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER | NewDrawer::ptNEWCSY);
		drw->getRootDrawer()->prepare(&pp);
		view->OnUpdate(NULL, 2, NULL); // for now limit the change to the LayerTreeView; if it appears that "drawer" is remembered in more places, call UpdateAllViews(0, 2)
	//}

	updateMapView();
}
