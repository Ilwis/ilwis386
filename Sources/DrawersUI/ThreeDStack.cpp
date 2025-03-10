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
#include "drawers\pointdrawer.h"
#include "DrawersUI\ThreeDStack.h"
#include "DrawersUI\ThreeDGlobalTool.h"
#include "Engine\Drawers\ZValueMaker.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

ILWIS::NewDrawer *createCursor3DDrawer(DrawerParameters *parms) {
	return new Cursor3DDrawer(parms);
}

Cursor3DDrawer::Cursor3DDrawer(DrawerParameters *parms) : 
PointDrawer(parms,"Cursor3DDrawer")
{
}

Cursor3DDrawer::~Cursor3DDrawer() {
}


bool Cursor3DDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	if (cNorm.z == 0) {
		getRootDrawer()->setZIndex(1 + getRootDrawer()->getZIndex());
		glDepthRange(0.01 - getRootDrawer()->getZIndex() * 0.0005, 1.0 - getRootDrawer()->getZIndex() * 0.0005);
	}
	PointDrawer::draw(drawLoop, cbArea);
	glDisable(GL_BLEND);

	return true;
}

void Cursor3DDrawer::prepare(PreparationParameters *p){
	properties.symbol = "crosshair";
	setSpecialDrawingOptions(NewDrawer::sdoExtrusion, true);
	PointDrawer::prepare(p);
}
//-----------------------------------------------------------------------------------
DrawerTool *createThreeDStack(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ThreeDStack(zv, view, drw);
}

ThreeDStack::ThreeDStack(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
DrawerTool("ThreeDStack",zv, view, drw),cursor(0),offset(0),distance(0),fMouseDown(false),htiNodeCursor(0)
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

void ThreeDStack::OnLButtonDown(UINT nFlags, CPoint point) {
	fMouseDown = true;
	if ( nFlags & MK_CONTROL)
		return;

	mouseCrd = drawer->getRootDrawer()->screenToWorld(RowCol(point.y, point.x));
	if ( drawer->getRootDrawer()->getMapCoordBounds().fContains(mouseCrd)) {
		if(cursor) {
			double offset = rUNDEF;
			for(int i = 0; i < drawer->getRootDrawer()->getDrawerCount(); ++i) {
				ComplexDrawer *cdr  = dynamic_cast<ComplexDrawer *>(drawer->getRootDrawer()->getDrawer(i));
				if ( !cdr)
					continue;
				for(int j = 0; j < cdr->getDrawerCount(); ++j ){
					ComplexDrawer *cdr2 = dynamic_cast<ComplexDrawer *>(cdr->getDrawer(j));
					if (!cdr2)
						continue;
					offset = max(offset, cdr2->getZMaker()->getOffset());
				}
			}
			mouseCrd.z = offset * 1.05; // slightly above the top map
			cursor->setCoord(mouseCrd);	
			drawer->getRootDrawer()->getDrawerContext()->doDraw();
		}
	}
}

void ThreeDStack::OnLButtonUp(UINT nFlags, CPoint point) {
	fMouseDown = false;
	if ( nFlags & MK_CONTROL)
		return;

	mouseCrd = drawer->getRootDrawer()->screenToWorld(RowCol(point.y, point.x));
	if ( drawer->getRootDrawer()->getMapCoordBounds().fContains(mouseCrd)) {
		if(cursor) {
			double offset = rUNDEF;
			for(int i = 0; i < drawer->getRootDrawer()->getDrawerCount(); ++i) {
				ComplexDrawer *cdr  = dynamic_cast<ComplexDrawer *>(drawer->getRootDrawer()->getDrawer(i));
				if ( !cdr)
					continue;
				for(int j = 0; j < cdr->getDrawerCount(); ++j ){
					ComplexDrawer *cdr2 = dynamic_cast<ComplexDrawer *>(cdr->getDrawer(j));
					if (!cdr2)
						continue;
					offset = max(offset, cdr2->getZMaker()->getOffset());
				}
			}
			mouseCrd.z = offset * 1.05; // slightly above the top map
			cursor->setCoord(mouseCrd);	
			drawer->getRootDrawer()->getDrawerContext()->doDraw();
		}
	}
}

void ThreeDStack::OnMouseMove(UINT nFlags, CPoint point) {
	if ( nFlags & MK_CONTROL)
		return;
	if (!fMouseDown)
		return;

	mouseCrd = drawer->getRootDrawer()->screenToWorld(RowCol(point.y, point.x));
	if ( drawer->getRootDrawer()->getMapCoordBounds().fContains(mouseCrd)) {
		if(cursor) {
			double offset = rUNDEF;
			for(int i = 0; i < drawer->getRootDrawer()->getDrawerCount(); ++i) {
				ComplexDrawer *cdr  = dynamic_cast<ComplexDrawer *>(drawer->getRootDrawer()->getDrawer(i));
				if ( !cdr)
					continue;
				for(int j = 0; j < cdr->getDrawerCount(); ++j ){
					ComplexDrawer *cdr2 = dynamic_cast<ComplexDrawer *>(cdr->getDrawer(j));
					if (!cdr2)
						continue;
					offset = max(offset, cdr2->getZMaker()->getOffset());
				}
			}
			mouseCrd.z = offset * 1.05; // slightly above the top map
			cursor->setCoord(mouseCrd);	
			drawer->getRootDrawer()->getDrawerContext()->doDraw();
		}
	}
}

HTREEITEM ThreeDStack::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&ThreeDStack::setthreeDStack);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&ThreeDStack::changeDistances);
	htiNode =  insertItem(TR("3D Stack"),"Stack",item,0);
	DrawerTool::configure(htiNode);

	CoordBounds cbLimits = drawer->getRootDrawer()->getCoordBoundsView();
	distance = 0.5 * (cbLimits.width() + cbLimits.height())/ (2.0 * drawer->getRootDrawer()->getDrawerCount());

	return htiNode;
}

void ThreeDStack::changeDistances() {
	ValueRange range(0, rRound(distance * 3), distance / 10);
	new ThreeDStackForm(tree, (ComplexDrawer *)drawer, this, range,&distance,&offset);
}

void ThreeDStack::setthreeDStackMarker(void *v, HTREEITEM) {
	bool yesno = *(bool *)v;
	setActive(yesno); // noone else will set us active/inactive
	MapPaneView *view = tree->GetDocument()->mpvGetView();
	if (yesno) {
		view->noTool();
		if (!view->addTool(this, getId()))
			view->changeStateTool(getId(), true);
		if (cursor) {
			if (!cursor->isActive())
				cursor->setActive(true);
		} else {
			DrawerParameters dp(drawer->getRootDrawer(), drawer);
			cursor = new Cursor3DDrawer(&dp);
			cursor->setCoord(mouseCrd);
			((PointProperties*)cursor->getProperties())->drawColor = Color(0,0,0);
			PreparationParameters pp(NewDrawer::ptRENDER);
			cursor->prepare(&pp);
			drawer->getRootDrawer()->addPostDrawer(728,cursor);
		}
		drawer->getRootDrawer()->getZMaker()->setThreeDPossible(true);
	} else {
		drawer->getRootDrawer()->getZMaker()->setThreeDPossible(false);
		cursor->setActive(false);
		view->changeStateTool(getId(), false);
	}
}

void ThreeDStack::Stop()
{
	if ( cursor->isActive()) {
		cursor->setActive(false);
	}

	if (tree->m_hWnd && htiNodeCursor)
		tree->GetTreeCtrl().SetCheck(htiNodeCursor, false);
}

void ThreeDStack::update() {

	if (!htiNode)
		return;

	tree->DeleteAllItems(htiNode, true);
	stackStatus.clear();
	ComplexDrawer *rootDrawer = (ComplexDrawer *)drawer->getRootDrawer();
	DisplayOptionTreeItem *item;
	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&ThreeDStack::setthreeDStackMarker);
	htiNodeCursor = insertItem(TR("3D Cursor"),"info",item,0);

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
		} else if ( drw->getType() == "AnimationDrawer") {
			it = insertItem(drw->getName(),"Animation",item,1);
		} else if ( drw->getType() == "CollectionDrawer") {
				it = insertItem(drw->getName(),"Set",item,1);
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
			cdrw->getZMaker()->setOffset(zoffset + offset,true);
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
		if ( stackStatus[i].item == it) {
			stackStatus[i].status = useStack;
			break;
		}
	}
	updateLayerDistances();
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

//---------------------------------------------------

ThreeDStackForm::ThreeDStackForm(CWnd *wPar, ComplexDrawer *dr, ILWIS::ThreeDStack *_stack, const ValueRange& _range, double *_distance, double *_offset) : 
DisplayOptionsForm2(dr,wPar,"ThreeDStack"),
distance(_distance),
range(_range),
stck(_stack),
offset(_offset)
{
	sliderDistance = new FieldRealSliderEx(root,TR("Stack layer distance"), distance, range,false);
	sliderDistance->SetCallBack((NotifyProc)&ThreeDStackForm::setThreeDStack);
	sliderDistance->setContinuous(true);

	ValueRange vrr(RangeReal(-range->rrMinMax().rHi(),range->rrMinMax().rHi()),range->rStep());
	sliderOffset = new FieldRealSliderEx(root,TR("Stack layer offset"), offset, vrr, false);
	sliderOffset->SetCallBack((NotifyProc)&ThreeDStackForm::setThreeDStack);
	sliderOffset->setContinuous(true);
	create();
}

int ThreeDStackForm::setThreeDStack(Event *ev) {
	apply();
	return 1;
}

void  ThreeDStackForm::apply() {
	if ( initial) return;
	sliderDistance->StoreData();
	sliderOffset->StoreData();

	SetDrawer *setdrw = dynamic_cast<SetDrawer *>(drw);
	if ( setdrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < setdrw->getDrawerCount(); ++i) {
			ComplexDrawer *cdrw = (ComplexDrawer *)drw;
			cdrw->getZMaker()->setOffset(*distance + *offset,true);
			//cdrw->prepareChildDrawers(&pp);
		}
	}
	else {
		stck->updateLayerDistances();
	}


	updateMapView();

}