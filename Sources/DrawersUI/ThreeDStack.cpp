#include "Client\Headers\formelementspch.h"
#include "engine\base\Round.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "drawers\linedrawer.h"
#include "DrawersUI\ThreeDStack.h"
#include "DrawersUI\ThreeDGlobalTool.h"
#include "Engine\Drawers\ZValueMaker.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

ILWIS::NewDrawer *createCursor3DDrawer(DrawerParameters *parms) {
	return new Cursor3DDrawer(parms);
}

Cursor3DDrawer::Cursor3DDrawer(DrawerParameters *parms) : 
LineDrawer(parms,"Cursor3DDrawer")
{
}

Cursor3DDrawer::~Cursor3DDrawer() {
}


bool Cursor3DDrawer::draw( const CoordBounds& cbArea) const{
	return LineDrawer::draw(cbArea);	
}

void Cursor3DDrawer::prepare(PreparationParameters *p){
	LineDrawer::prepare(p);
}
//-----------------------------------------------------------------------------------
DrawerTool *createThreeDStack(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ThreeDStack(zv, view, drw);
}

ThreeDStack::ThreeDStack(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
DrawerTool("ThreeDStack",zv, view, drw),cursor(0)
{
	stay = true;
}

ThreeDStack::~ThreeDStack() {
	//if (tree && htiNode != 0)
	//	tree->GetTreeCtrl().DeleteItem(htiNode);
	//htiNode = 0;
}

bool ThreeDStack::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	bool ok =  dynamic_cast<ThreeDGlobalTool *>(tool) != 0;
	if ( ok)
		parentTool = tool;

	return ok;
}

void ThreeDStack::OnLButtonUp(UINT nFlags, CPoint point) {
	mouseCrd = drawer->getRootDrawer()->screenToWorld(RowCol(point.y, point.x));
	if(cursor) {
		vector<Coord> crds;
		crds.push_back(Coord(mouseCrd.x, mouseCrd.y, mouseCrd.z));
		crds.push_back(Coord(mouseCrd.x, mouseCrd.y, 0));
		cursor->addCoords(crds,1);
		if ( cursor && cursor->isActive())
			drawer->getRootDrawer()->getDrawerContext()->doDraw();
	}
}

HTREEITEM ThreeDStack::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&ThreeDStack::setthreeDStack);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&ThreeDStack::changeDistances);
	htiNode =  insertItem(TR("3D Stack"),"Stack",item,0);
	DrawerTool::configure(htiNode);

	CoordBounds cbLimits = drawer->getRootDrawer()->getCoordBoundsView();
	distance = 0.25 * (cbLimits.width() + cbLimits.height())/ (2.0 * drawer->getRootDrawer()->getDrawerCount());


	return htiNode;
}

void ThreeDStack::changeDistances() {
	ValueRange range(0, rRound(distance * 3), distance / 10);
	new ThreeDStackForm(tree, (ComplexDrawer *)drawer, this, range,&distance);
}

void ThreeDStack::setthreeDStackMarker(void *v, HTREEITEM) {
	bool use = *(bool *)v;
	if ( use) {
		if ( cursor && cursor->isActive())
			cursor->setActive(true);
		else {
			DrawerParameters dp(drawer->getRootDrawer(), drawer);
			cursor = new Cursor3DDrawer(&dp);
			drawer->getRootDrawer()->addPostDrawer(728,cursor); 
			vector<Coord> crds;
			crds.push_back(Coord(mouseCrd.x, mouseCrd.y, mouseCrd.z));
			crds.push_back(Coord(mouseCrd.x, mouseCrd.y, 0));
			cursor->addCoords(crds,1);
		}
		tree->GetDocument()->mpvGetView()->addTool(this, getId());
	} else {
		cursor->setActive(false);
		tree->GetDocument()->mpvGetView()->noTool(getId());
	}
}

void ThreeDStack::update() {

	if (!htiNode)
		return;

	tree->DeleteAllItems(htiNode, true);
	stackStatus.clear();
	ComplexDrawer *rootDrawer = (ComplexDrawer *)drawer->getRootDrawer();
	DisplayOptionTreeItem *item;
	/*item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&ThreeDStack::setthreeDStackMarker);
	insertItem(TR("3D Cursor"),"info",item,0);*/

	for(int i = 0 ; i < rootDrawer->getDrawerCount(); ++i) {
		NewDrawer *drw = rootDrawer->getDrawer(i);
		item = new DisplayOptionTreeItem(tree,htiNode,drawer);
		HTREEITEM it = 0;
		if ( drw->getType() == "RasterDataDrawer") {
			it = insertItem(drw->getName(),".mpr",item,1);
		} else if ( drw->getType() == "FeatureDataDrawer") {
			BaseMapPtr *bmp = ((FeatureDataDrawer *)drw)->getBaseMap();
			if ( IOTYPE(bmp->fnObj) == IlwisObject::iotSEGMENTMAP)
				it = insertItem(drw->getName(),".mps",item,1);
			if ( IOTYPE(bmp->fnObj) == IlwisObject::iotPOLYGONMAP)
				it = insertItem(drw->getName(),".mpa",item,1);
			if ( IOTYPE(bmp->fnObj) == IlwisObject::iotPOINTMAP)
				it = insertItem(drw->getName(),".mpp",item,1);
			if ( IOTYPE(bmp->fnObj) == IlwisObject::iotMAPLIST)
				it = insertItem(drw->getName(),".mpl",item,1);
			if ( IOTYPE(bmp->fnObj) == IlwisObject::iotOBJECTCOLLECTION)
				it = insertItem(drw->getName(),".ioc",item,1);
		} else {
			continue;
		}
		item->setCheckAction(this,0,(DTSetCheckFunc)&ThreeDStack::setIndividualStatckItem);
		stackStatus.push_back(StackInfo(it, true));

	}
}

void ThreeDStack::updateLayerDistances() {
	RootDrawer *rootDrawer = drawer->getRootDrawer();
	double zoffset = 0;
	int n = rootDrawer->getDrawerCount();

	for(int i = 0 ; i < n; ++i) {
		if ( stackStatus.size() > 0) {
			if ( !stackStatus[i].status)
				continue;
		}

		SpatialDataDrawer *drw = dynamic_cast<SpatialDataDrawer *>(rootDrawer->getDrawer(i));
		if ( !drw)
			continue;
		for(int j = 0 ; j < drw->getDrawerCount(); ++j) {
			ComplexDrawer *cdrw = (ComplexDrawer *)drw->getDrawer(j);
			cdrw->getZMaker()->setOffset(zoffset,true);
		}
		zoffset += distance;
	}
}

void ThreeDStack::setthreeDStack(void *v, HTREEITEM) {
	bool useStack = *(bool *)v;
	if ( !useStack)
		distance = 0;
	else {
		if ( distance == 0) {
			CoordBounds cbLimits = drawer->getRootDrawer()->getCoordBoundsView();
			distance = 0.25 * (cbLimits.width() + cbLimits.height())/ (2.0 * drawer->getRootDrawer()->getDrawerCount());
		}
	}

	update();
	updateLayerDistances();

	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void ThreeDStack::setIndividualStatckItem(void *v, HTREEITEM it) {
	bool useStack = *(bool *)v;
	for(int i=0; i < stackStatus.size(); ++i) {
		if ( stackStatus[i].item = it) {
			stackStatus[i].status = useStack;
			break;
		}
	}
	updateLayerDistances();
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

//---------------------------------------------------

ThreeDStackForm::ThreeDStackForm(CWnd *wPar, ComplexDrawer *dr, ILWIS::ThreeDStack *_stack, const ValueRange& _range, double *_distance) : 
DisplayOptionsForm(dr,wPar,"ThreeDStack"),
distance(_distance),
range(_range),
stck(_stack)
{
	slider = new FieldRealSliderEx(root,TR("Stack layer distance"), distance, range,true);
	slider->SetCallBack((NotifyProc)&ThreeDStackForm::setThreeDStack);
	slider->setContinuous(true);
	create();
}

int ThreeDStackForm::setThreeDStack(Event *ev) {
	apply();
	return 1;
}

void  ThreeDStackForm::apply() {
	if ( initial) return;
	slider->StoreData();

	SetDrawer *setdrw = dynamic_cast<SetDrawer *>(drw);
	if ( setdrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < setdrw->getDrawerCount(); ++i) {
			ComplexDrawer *cdrw = (ComplexDrawer *)drw;
			cdrw->getZMaker()->setOffset(*distance,true);
			//cdrw->prepareChildDrawers(&pp);
		}
	}
	else {
		stck->updateLayerDistances();
	}


	updateMapView();

}