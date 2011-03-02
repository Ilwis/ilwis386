#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "drawers\linedrawer.h"
#include "Drawers\GridDrawer.h"
#include "Drawers\Grid3DTool.h"
#include "Drawers\GridTool.h"
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
	htiNode = insertItem("3D Grid","3D",item,gdr->is3D());

	DrawerTool::configure(htiNode);

	return htiNode;
}

void Grid3DTool::displayOptionGrid3D() {
	new Grid3DOptions(tree, (GridDrawer *)drawer);
}

void Grid3DTool::grid3D(void *v) {
	bool threeD = *(bool *)v;
	GridDrawer *gdr = (GridDrawer *)drawer;
	gdr->set3D(threeD);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	drawer->prepare(&pp);
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

String Grid3DTool::getMenuString() const {
	return TR("Grid 3D");
}

//-----------------------------------------------------------
Grid3DOptions::Grid3DOptions(CWnd *par, GridDrawer *gdr) :
DisplayOptionsForm(gdr, par, TR("3D Grid options")), zDist(gdr->getZSpacing()),planeColor(gdr->getPlaneColor()), numPlanes(gdr->getNumberOfPlanes())
{
	hasgrid = (gdr->getMode() & GridDrawer::mGRID) != 0;
	hasplane = (gdr->getMode() & GridDrawer::mPLANE) != 0;
	iscube = (gdr->getMode() & GridDrawer::mMARKERS) != 0;
	hasaxis = (gdr->getMode() & GridDrawer::mAXIS) != 0;
	hasverticals = (gdr->getMode() & GridDrawer::mVERTICALS) != 0;
	frDistance = new FieldReal(root, TR("Vertical Distance"), &zDist, ValueRange(0.0,10000));
	frPlanes = new FieldInt(root, TR("Number of planes"), &numPlanes, ValueRange(1,10));
	new StaticText(root,TR("Appearance"));
	fg = new FieldGroup(root);
	cbgrid = new CheckBox(fg,TR("Grid"), &hasgrid);
	cbverticals = new CheckBox(fg,TR("Verticals"), &hasverticals);
	cbverticals->Align(cbgrid, AL_AFTER);
	cbplane = new CheckBox(fg,TR("Plane"), &hasplane);
	cbplane->Align(cbverticals, AL_AFTER);
	cbcube = new CheckBox(fg,TR("Cube"),&iscube);
	cbcube->SetCallBack((NotifyProc)&Grid3DOptions::uncheckRest);
	cbcube->Align(cbplane, AL_AFTER);
	fg->SetIndependentPos();
	FieldColor *fc = new FieldColor(cbplane,TR("Color"), &planeColor);
	fc->Align(fg, AL_UNDER);
	fc->SetIndependentPos();
	create();
}

int Grid3DOptions::uncheckRest(Event *ev) {
	cbcube->StoreData();
	if ( iscube) {
		cbgrid->SetVal(false);
		cbverticals->SetVal(false);
		cbplane->SetVal(false);
		((GridDrawer *)drw)->clear();
	}
	return 1;
}

void  Grid3DOptions::apply() {
	frDistance->StoreData();
	cbgrid->StoreData();
	cbplane->StoreData();
	cbcube->StoreData();
	cbverticals->StoreData();
	frPlanes->StoreData();
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
	gdr->setZSpacing(zDist);
	gdr->setPlaneColor(planeColor);
	gdr->setNumberOfplanes(numPlanes);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	drw->prepare(&pp);
	updateMapView();
}
