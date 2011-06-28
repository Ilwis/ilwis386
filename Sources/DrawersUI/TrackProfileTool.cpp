#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Drawers\LayerDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\SetDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\ComplexDrawer.h" 
#include "drawers\linedrawer.h"
#include "drawers\pointdrawer.h"
#include "Client\FormElements\FieldListView.h"
#include "TrackProfileGraph.h"
#include "DrawersUI\TrackProfileTool.h"
#include "headers\constant.h"

using namespace ILWIS;

//------------------------------------------------------
TrackMarker::TrackMarker(DrawerParameters *parms) : 
PointDrawer(parms,"TrackMarker")
{
}

TrackMarker::~TrackMarker() {
}


bool TrackMarker::draw( const CoordBounds& cbArea) const{
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	PointDrawer::draw(cbArea);
	glDisable(GL_BLEND);

	return true;
}

void TrackMarker::prepare(PreparationParameters *p){
	properties.symbol = "crosshair";
	setSpecialDrawingOptions(NewDrawer::sdoExtrusion, true);
	PointDrawer::prepare(p);
}
//------------------------------------------------------
TrackLine::TrackLine(DrawerParameters *parms) : 
LineDrawer(parms,"TrackLine")
{
	setSupportingDrawer(true);
}

TrackLine::~TrackLine() {
}


bool TrackLine::draw( const CoordBounds& cbArea) const{
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	LineDrawer::draw(cbArea);
	return true;
}

void TrackLine::prepare(PreparationParameters *p){
	LineDrawer::prepare(p);
}
//--------------------------------------------------

DrawerTool *createTrackProfileTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new TrackProfileTool(zv, view, drw);
}


TrackProfileTool::TrackProfileTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("TrackProfileTool",zv, view, drw)
{
	active = false;
	graphForm = false;
	stay = true;
	graphForm = 0;
	working = false;
	fDown = false;
	line = 0;
	point = 0;
}

TrackProfileTool::~TrackProfileTool() {
	if ( line)
		drawer->getRootDrawer()->removeDrawer(line->getId(), true);
	if (point)
		drawer->getRootDrawer()->removeDrawer(point->getId(), true);
	for(int i=0; i < sources.size(); ++i) 
		delete sources[i];
	sources.clear();
}

void TrackProfileTool::clear() {
}

bool TrackProfileTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);
	if (!layerDrawerTool && !setDrawerTool)
		return false;
	//LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(layerDrawerTool->getDrawer());
	//if ( !ldrw)
	//	return false;
	parentTool = tool;
	return true;
}

HTREEITEM TrackProfileTool::configure( HTREEITEM parentItem) {
	DrawerParameters dp(drawer->getRootDrawer(), drawer);
	line = new TrackLine(&dp);
	line->setActive(false);
	drawer->getRootDrawer()->addPostDrawer(730,line);

	DrawerParameters dp2(drawer->getRootDrawer(), drawer);
	point = new TrackMarker(&dp2);
	point->setActive(false);
	PreparationParameters pp(NewDrawer::ptRENDER);
	point->prepare(&pp);
	drawer->getRootDrawer()->addPostDrawer(731,point);

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&TrackProfileTool::displayOptionAddList);
	item->setCheckAction(this,0, (DTSetCheckFunc)&TrackProfileTool::setcheckTool);
	htiNode = insertItem(TR("Track Profile"),"Track",item,0);

	DrawerTool *dt = DrawerTool::createTool("LineStyleTool", getDocument()->mpvGetView(),tree, line);
	if ( dt) {
		addTool(dt);
		dt->configure(htiNode);
	}

	SpatialDataDrawer *spdr = dynamic_cast<SpatialDataDrawer *>(drawer->getParentDrawer());
	if ( !spdr)
		spdr = dynamic_cast<SpatialDataDrawer *>(drawer);
	IlwisObject *obj = (IlwisObject *)spdr->getDataSource();
	addSource((*obj)->fnObj);

	DrawerTool::configure(htiNode);

	return htiNode;
}

void TrackProfileTool::setcheckTool(void *w, HTREEITEM item) {
	working = *(bool *)w;
	if ( working) {
		tree->GetDocument()->mpvGetView()->addTool(this, getId());
		if (!graphForm) {
			graphForm = new TrackProfileGraphFrom(tree, (LayerDrawer *)drawer, this);
			for(int i = 0; i < sources.size(); ++i) {
				graphForm->addSource(sources[i]->getSource());
			}
		} else {
			graphForm->ShowWindow(SW_SHOW);
		}
	}
	else {
		tree->GetDocument()->mpvGetView()->noTool(getId());
		if ( graphForm) {
			graphForm->ShowWindow(SW_HIDE);
			if (line && point) {
				line->setActive(false);
				point->setActive(false);
				coords.clear();
				setCoords();
			}
			mpvGetView()->Invalidate();
		}
	}
}

String TrackProfileTool::getMenuString() const {
	return TR("Track Profile");
}

void TrackProfileTool::displayOptionAddList() {
	new ChooseTrackProfileForm(tree, (LayerDrawer *)drawer, this);
}

bool TrackProfileTool::isUnique(const FileName& fn) {
	
	for(int i=0; i < sources.size(); ++i) {
		if ( sources.at(i)->getSource()->fnObj == fn)
			return false;
	}
	return true;
}

void TrackProfileTool::addSource(const FileName& fn) {
	IlwisObject obj = IlwisObject::obj(fn);
	if( obj.fValid()) {
		if ( isUnique(obj->fnObj)) {
			TrackDataSource *src = new TrackDataSource(obj);
			sources.push_back(src);
			if (graphForm)
				graphForm->addSource(src->getMap());
		}
	}
}

void TrackProfileTool::setCoords() {
	line->addCoords(coords, 1);
}

void TrackProfileTool::OnMouseMove(UINT nFlags, CPoint pnt)
{
	if ( !line->isActive())
		return;

	if (fDown) {
		SetCursor(zCursor(IDC_CROSS));
		mpvGetView()->Invalidate();
		Coord c1 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(pnt.y, pnt.x));
		coords[coords.size() - 1] = c1;
		setCoords();
		mpvGetView()->Invalidate();
	}
}

void TrackProfileTool::OnLButtonDown(UINT nFlags, CPoint pnt) {
	short state = ::GetKeyState(VK_CONTROL);
	if ( !(state & 0x8000)) {
		if ( line->isActive() && fDown == false) {
			line->setActive(false);
			coords.clear();
			setCoords();
			point->setActive(false);
			mpvGetView()->Invalidate();
		}
	}
}

void TrackProfileTool::OnLButtonUp(UINT nFlags, CPoint pnt)
{
	if ( !line->isActive()) {
		line->setActive(true);
		point->setActive(false);
		graphForm->reset();
		Coord c1 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(pnt.y, pnt.x));
		coords.push_back(c1);
		Coord c2 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(pnt.y, pnt.x));
		coords.push_back(c2);
		tree->GetDocument()->mpvGetView()->setBitmapRedraw(true);
		fDown = true;
		setCoords();
	} else {
		short state = ::GetKeyState(VK_CONTROL);
		if ( state & 0x8000) {
			coords.push_back(coords[coords.size() - 1]);
		} else {
			setCoords();
			fDown = false;
			tree->GetDocument()->mpvGetView()->setBitmapRedraw(false);
			graphForm->setTrack(coords);
			graphForm->ShowWindow(SW_SHOW);
		}
	}
	mpvGetView()->Invalidate();
}

void TrackProfileTool::setMarker(const Coord& crd) {
	if (!point->isActive()) {
		point->setActive(true);
	}
	if (crd.fUndef())
		point->setActive(false);
	point->setCoord(crd);
	mpvGetView()->Invalidate();
}
void TrackProfileTool::timedEvent(UINT timerid) {
	for(int i=0; i < drawer->getRootDrawer()->getDrawerCount(); ++i) {
		SpatialDataDrawer *cdrw = dynamic_cast<SpatialDataDrawer *>(drawer->getRootDrawer()->getDrawer(i));
		if (!cdrw)
			continue;
		IlwisObject *obj = (IlwisObject *)cdrw->getDataSource();
		FileName fnDrw = (*obj)->fnObj;
		for(int j = 0; j < sources.size(); ++j) {
			FileName fn = sources[i]->getSource()->fnObj;
			if ( fn == fnDrw) {
				sources[i]->updateIndex(cdrw->getCurrentIndex());
			}
		}
		if ( graphForm)
			graphForm->update();
	}
}

void TrackProfileTool::setActiveMode(bool yesno) {
	DrawerTool::setActiveMode(yesno);
	setcheckTool(&yesno, 0);

}
//-------------------------------------------------------------------
TrackDataSource::TrackDataSource(const IlwisObject& obj) {
	addSource(obj);
}

void TrackDataSource::addSource(const IlwisObject& obj) {
	currentIndex = 0;
	if ( IOTYPEBASEMAP(obj->fnObj)) {
		bmp = BaseMap(obj->fnObj);
	} else if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
		mpl = MapList(obj->fnObj);
	} else if ( IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
		oc = ObjectCollection(obj->fnObj);
	}
}

BaseMap TrackDataSource::getMap(const Coord& crd) const{
	if ( bmp.fValid())
		return bmp;
	if ( mpl.fValid())
		return mpl[currentIndex];
	if ( oc.fValid()) {
		set<String> maps;
		oc->getBaseMaps(crd,maps);
		if ( maps.size() > 0) {
			BaseMap bmp((*maps.begin()));
			return bmp;
		}
	}
	return BaseMap();
}

IlwisObject TrackDataSource::getSource() const{
	if ( bmp.fValid())
		return bmp;
	if ( mpl.fValid())
		return mpl;
	if ( oc.fValid())
		return oc;
	return IlwisObject();
}

void TrackDataSource::updateIndex(long ind){
	currentIndex = ind;
}

//-------------------------------------------------------------------
ChooseTrackProfileForm::ChooseTrackProfileForm(CWnd *wPar, LayerDrawer *dr, TrackProfileTool *t) : 
	DisplayOptionsForm2(dr,wPar,TR("Add data source"),fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON | fbsOKHASCLOSETEXT),
	tool(t)
{
	fm = new FieldDataType(root,TR("Data source"),&name, ".mpr.mps.mpa.mpp",true);
	new PushButton(root, TR("Add"),(NotifyProc) &ChooseTrackProfileForm::addSource);
	create();
}


int ChooseTrackProfileForm::addSource(Event *ev) {
	fm->StoreData();
	if ( name != "") {
		tool->addSource(FileName(name));
	}
	return 1;
}

//========================================================================
TrackProfileGraphFrom::TrackProfileGraphFrom(CWnd *wPar, LayerDrawer *dr,TrackProfileTool *t) :
DisplayOptionsForm2(dr,wPar,TR("Track Profile Graph"),fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON|fbsHIDEONCLOSE), graph(0)
{
	vector<FLVColumnInfo> v;
	v.push_back(FLVColumnInfo("Source", 220));
	v.push_back(FLVColumnInfo("Distance", 70));
	v.push_back(FLVColumnInfo("Value range", 80));
	v.push_back(FLVColumnInfo("Value", 60));
	graph = new TrackProfileGraphEntry(root,t);
	graph->setListView(new FieldListView(root,v));
	create();
	ShowWindow(SW_HIDE);
}


void TrackProfileGraphFrom::setTrack(const vector<Coord>& crds) {
	if ( graph)
		graph->setTrack(crds);
}

void TrackProfileGraphFrom::addSource(const IlwisObject& bmp) {
	if ( graph)
		graph->addSource(bmp);
}

void TrackProfileGraphFrom::reset() {
	if ( graph)
		graph->setIndex(iUNDEF,0,Coord());
}

void TrackProfileGraphFrom::update() {
	if ( graph)
		graph->update();
}
