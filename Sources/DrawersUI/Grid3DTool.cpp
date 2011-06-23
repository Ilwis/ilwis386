#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "drawers\linedrawer.h"
#include "Drawers\GridDrawer.h"
#include "DrawersUI\Grid3DTool.h"
#include "DrawersUI\GridTool.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createGrid3DTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new Grid3DTool(zv, view, drw);
}

Grid3DTool::Grid3DTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("Grid3DTool",zv, view, drw)
{
}

Grid3DTool::~Grid3DTool() {
}

bool Grid3DTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	return dynamic_cast<GridTool *>(tool) != 0;
}

HTREEITEM Grid3DTool::configure( HTREEITEM parentItem) {
	GridDrawer *gdr = (GridDrawer *)drawer;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree, parentItem, drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&Grid3DTool::displayOptionGrid3D);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&Grid3DTool::grid3D);
	htiNode = insertItem("3D Grid","3D",item,gdr->is3DGrd());

	DrawerTool::configure(htiNode);

	return htiNode;
}

void Grid3DTool::displayOptionGrid3D() {
	new Grid3DOptions(tree, (GridDrawer *)drawer);
}

void Grid3DTool::grid3D(void *v, HTREEITEM ) {
	bool threeD = *(bool *)v;
	GridDrawer *gdr = (GridDrawer *)drawer;
	gdr->set3DGrid(threeD);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	drawer->prepare(&pp);
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

String Grid3DTool::getMenuString() const {
	return TR("Grid 3D");
}

//-----------------------------------------------------------
Grid3DOptions::Grid3DOptions(CWnd *par, GridDrawer *gdr) :
DisplayOptionsForm(gdr, par, TR("3D Grid options"))
{
	hasgrid = (gdr->getMode() & GridDrawer::mGRID) != 0;
	hasplane = (gdr->getMode() & GridDrawer::mPLANE) != 0;
	iscube = (gdr->getMode() & GridDrawer::mMARKERS) != 0;
	hasaxis = (gdr->getMode() & GridDrawer::mAXIS) != 0;
	hasverticals = (gdr->getMode() & GridDrawer::mVERTICALS) != 0;
	new StaticText(root,TR("Appearance"));
	fg = new FieldGroup(root);
	cbgrid = new CheckBox(fg,TR("Grid"), &hasgrid);
	cbverticals = new CheckBox(fg,TR("Verticals"), &hasverticals);
	cbplane = new CheckBox(fg,TR("Plane"), &hasplane);
	cbcube = new CheckBox(fg,TR("Cube"),&iscube);
	cbcube->SetCallBack((NotifyProc)&Grid3DOptions::uncheckRest);
	fg->SetIndependentPos();
	FieldGroup *fg2 = new FieldGroup(cbplane, true);
	fg2->Align(cbplane, AL_AFTER);
	create();
}

int Grid3DOptions::uncheckRest(Event *ev) {
	cbcube->StoreData();
	if ( iscube) {
		cbgrid->SetVal(false);
		cbverticals->SetVal(false);
		cbplane->SetVal(false);
		((GridDrawer *)drw)->clear();
		//updateMapView();
	}
	return 1;
}

void  Grid3DOptions::apply() {
	if (initial) return;

	cbgrid->StoreData();
	cbplane->StoreData();
	cbcube->StoreData();
	cbverticals->StoreData();
	GridDrawer *gdr = ((GridDrawer *)drw);
	int mode = 0;
	if ( hasgrid)
		mode |= GridDrawer::mGRID;
	if (hasplane)
		mode |= GridDrawer::mPLANE;
	if ( iscube)
		mode |= GridDrawer::mCUBE;
	if ( hasverticals)
		mode |= GridDrawer::mVERTICALS;
	gdr->setMode(mode);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	drw->prepare(&pp);
	updateMapView();
}
