#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
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
#include "drawers\linedrawer.h"
#include "drawers\pointdrawer.h"
#include "Client\FormElements\FieldListView.h"
#include "HovMollerGraph.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Client\FormElements\objlist.h"
#include "DrawersUI\HovMollerTool.h"
#include "headers\constant.h"

using namespace ILWIS;

//------------------------------------------------------
HTrackMarker::HTrackMarker(DrawerParameters *parms) : 
PointDrawer(parms,"HTrackMarker")
{
}

HTrackMarker::~HTrackMarker() {
}


bool HTrackMarker::draw( const CoordBounds& cbArea) const{
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	PointDrawer::draw(cbArea);
	glDisable(GL_BLEND);
	if (getRootDrawer()->is3D())
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	return true;
}

void HTrackMarker::prepare(PreparationParameters *p){
	properties.symbol = "crosshair";
	setSpecialDrawingOptions(NewDrawer::sdoExtrusion, true);
	PointDrawer::prepare(p);
}
//------------------------------------------------------
HTrackLine::HTrackLine(DrawerParameters *parms) : 
LineDrawer(parms,"HTrackLine")
{
	setSupportingDrawer(true);
}

HTrackLine::~HTrackLine() {
}


bool HTrackLine::draw( const CoordBounds& cbArea) const{
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	LineDrawer::draw(cbArea);
	if (getRootDrawer()->is3D())
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	return true;
}

void HTrackLine::prepare(PreparationParameters *p){
	LineDrawer::prepare(p);
}
//--------------------------------------------------

DrawerTool *createHovMollerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new HovMollerTool(zv, view, drw);
}


HovMollerTool::HovMollerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("HovMollerTool",zv, view, drw)
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

HovMollerTool::~HovMollerTool() {
	drawer->getRootDrawer()->setTopDrawer(0);
	if ( line)
		drawer->getRootDrawer()->removeDrawer(line->getId(), true);
	if (point)
		drawer->getRootDrawer()->removeDrawer(point->getId(), true);
}

void HovMollerTool::clear() {
}

bool HovMollerTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	if (!layerDrawerTool)
		return false;

	parentTool = tool;
	return true;
}

HTREEITEM HovMollerTool::configure( HTREEITEM parentItem) {
	DrawerParameters dp(drawer->getRootDrawer(), drawer);
	line = new HTrackLine(&dp);
	line->setActive(false);
	drawer->getRootDrawer()->addPostDrawer(730,line);

	DrawerParameters dp2(drawer->getRootDrawer(), drawer);
	point = new HTrackMarker(&dp2);
	point->setActive(false);
	PreparationParameters pp(NewDrawer::ptRENDER);
	point->prepare(&pp);
	drawer->getRootDrawer()->addPostDrawer(731,point);

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&HovMollerTool::displayOptionAddList);
	item->setCheckAction(this,0, (DTSetCheckFunc)&HovMollerTool::setcheckTool);
	htiNode = insertItem(TR("HovMoller Diagram"),"Track",item,0);

	DrawerTool *dt = DrawerTool::createTool("LineStyleTool", getDocument()->mpvGetView(),tree, line);
	if ( dt) {
		addTool(dt);
		dt->configure(htiNode);
	}

	//SpatialDataDrawer *spdr = dynamic_cast<SpatialDataDrawer *>(drawer->getParentDrawer());
	//if ( !spdr)
	//	spdr = dynamic_cast<SpatialDataDrawer *>(drawer);
	//IlwisObject *obj = (IlwisObject *)spdr->getDataSource();
	//addSource((*obj)->fnObj);

	DrawerTool::configure(htiNode);

	return htiNode;
}

void HovMollerTool::setcheckTool(void *w, HTREEITEM item) {
	working = *(bool *)w;
	if ( working) {
		tree->GetDocument()->mpvGetView()->addTool(this, getId());
		if (!graphForm) {
			graphForm = new HovMollerGraphFrom(tree, (LayerDrawer *)drawer, this);
			graphForm->addSource(source.getSource());
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

String HovMollerTool::getMenuString() const {
	return TR("Hovemoller Diagram");
}

void HovMollerTool::displayOptionAddList() {
	new ChooseHovMollerForm(tree, (LayerDrawer *)drawer, this);
}

void HovMollerTool::setSource(const FileName& fn) {
	MapList mpl(fn);
	if( mpl.fValid()) {
		source.setSource(mpl);
		if (graphForm)
			graphForm->addSource(mpl);
	}
}

void HovMollerTool::setCoords() {
	line->addCoords(coords, 1);
}

void HovMollerTool::OnMouseMove(UINT nFlags, CPoint pnt)
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

void HovMollerTool::OnLButtonDown(UINT nFlags, CPoint pnt) {
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

void HovMollerTool::OnLButtonUp(UINT nFlags, CPoint pnt)
{
	if ( !line->isActive()) {
		line->setActive(true);
		point->setActive(false);
		graphForm->reset();
		Coord c1 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(pnt.y, pnt.x));
		coords.push_back(c1);
		Coord c2 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(pnt.y, pnt.x));
		coords.push_back(c2);
		drawer->getRootDrawer()->removeDrawer(line->getId(), false);
		tree->GetDocument()->rootDrawer->setTopDrawer(line);
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
			tree->GetDocument()->rootDrawer->setTopDrawer(0);
			tree->GetDocument()->mpvGetView()->setBitmapRedraw(false);
			drawer->getRootDrawer()->addPostDrawer(730,line);
			graphForm->setTrack(coords);
			graphForm->ShowWindow(SW_SHOW);
		}
	}
	mpvGetView()->Invalidate();
}

void HovMollerTool::setMarker(const Coord& crd) {
	if (!point->isActive()) {
		point->setActive(true);
	}
	if (crd.fUndef())
		point->setActive(false);
	point->setCoord(crd);
	mpvGetView()->Invalidate();
}

void HovMollerTool::setActiveMode(bool yesno) {
	DrawerTool::setActiveMode(yesno);
	setcheckTool(&yesno, 0);

}
//-------------------------------------------------------------------
HovMollerDataSource::HovMollerDataSource() {

}

void HovMollerDataSource::setSource(const MapList& obj) {
	currentIndex = 0;
	mpl = MapList(obj->fnObj);
}

MapList HovMollerDataSource::getSource() const{
	return mpl;
}

void HovMollerDataSource::updateIndex(long ind){
	currentIndex = ind;
}

//-------------------------------------------------------------------
ChooseHovMollerForm::ChooseHovMollerForm(CWnd *wPar, LayerDrawer *dr, HovMollerTool *t) : 
	DisplayOptionsForm2(dr,wPar,TR("Add data source"),fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON | fbsOKHASCLOSETEXT),
	tool(t)
{
	fm = new FieldDataType(root,TR("Data source"),&name, ".mpl",true);
	new PushButton(root, TR("Add"),(NotifyProc) &ChooseHovMollerForm::addSource);
	create();
}


int ChooseHovMollerForm::addSource(Event *ev) {
	fm->StoreData();
	if ( name != "") {
		tool->setSource(FileName(name));
	}
	return 1;
}

//========================================================================
HovMollerGraphFrom::HovMollerGraphFrom(CWnd *wPar, LayerDrawer *dr,HovMollerTool *t) :
DisplayOptionsForm2(dr,wPar,TR("Hovmoller Diagram"),fbsBUTTONSUNDER | fbsNOCANCELBUTTON|fbsHIDEONCLOSE), graph(0), initial(true)
{
	if ( t->source.getSource().fValid())
		name = t->source.getSource()->fnObj.sFullPath();
	graph = new HovMollerGraphEntry(root,t);
	fm = new FieldDataType(root,TR("MapList"),&name, ".mpl",true);
	fm->SetCallBack((NotifyProc)&HovMollerGraphFrom::setSource);
	fcTime = new FieldColumn(root, TR("Time column"),tbl,&columnName,dmTIME);
	fcTime->SetCallBack((NotifyProc)&HovMollerGraphFrom::setTimeColumn);
	SetCallBack((NotifyProc)&HovMollerGraphFrom::init);
	create();
	ShowWindow(SW_HIDE);
}

int HovMollerGraphFrom::setTimeColumn(Event *ev) {
	if ( graph && columnName != "") {
		graph->setTimeColumn(columnName);
	}
	return 1;
}

int HovMollerGraphFrom::init(Event *ev) {
	if ( GetSafeHwnd() && initial) {
		fcTime->Hide();
		initial = false;
	}
	return 1;
}

FormEntry *HovMollerGraphFrom::CheckData() {
	return 0;
}

int HovMollerGraphFrom::setSource(Event *ev) {
	fm->StoreData();
	if ( name != "") {
		FileName fn(name);
		MapList mpl(fn);
		graph->setSource(mpl);
		if ( mpl->fTblAtt()) {
			Table tblAtt = mpl->tblAtt();
			for(int i = 0; i < tblAtt->iCols(); ++i) {
				if ( tblAtt->col(i)->dm()->pdtime()) {
					fcTime->Show();
					fcTime->FillWithColumns(&tblAtt);
					columnName = tblAtt->col(i)->sName();
					tbl = tblAtt;
					fcTime->SetVal(columnName);
					graph->setTimeColumn(columnName);
				}
			}
		}
	}

	return 1;
}

void HovMollerGraphFrom::setTrack(const vector<Coord>& crds) {
	if ( graph)
		graph->setTrack(crds);
}

void HovMollerGraphFrom::addSource(const MapList& mpl) {
	if ( graph)
		graph->setSource(mpl);
}

void HovMollerGraphFrom::reset() {

}

void HovMollerGraphFrom::update() {
	if ( graph)
		graph->update();
}
