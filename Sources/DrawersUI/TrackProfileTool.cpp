#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\flddat.h"
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
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Map\Segment\Seg.h"
#include "Client\FormElements\FieldListView.h"
#include "TrackProfileGraph.h"
#include "Drawers\CollectionDrawer.h"
#include "Engine\Domain\DmValue.h"
#include "DrawersUI\TrackProfileTool.h"
#include "headers\constant.h"

using namespace ILWIS;

//------------------------------------------------------
TrackMarker::TrackMarker(DrawerParameters *parms,TrackLine *_line) : 
PointDrawer(parms,"TrackMarker"), line(_line)
{
}

TrackMarker::~TrackMarker() {
}


bool TrackMarker::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	if (!isActive())
		return false;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	PointProperties *pprops = (PointProperties *)( const_cast<TrackMarker *>(this)->getProperties());
	if ( pprops && line) {
		LineProperties *lprops = (LineProperties *)line->getProperties();
		if ( lprops)
			pprops->drawColor = lprops->drawColor;
	}
	PointDrawer::draw(drawLoop, cbArea);
	glDisable(GL_BLEND);
	if (getRootDrawer()->is3D())
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	return true;
}

void TrackMarker::prepare(PreparationParameters *p){
	properties.symbol = "crosshair";
	properties.scale = 1.5;
	setSpecialDrawingOptions(NewDrawer::sdoExtrusion, true);
	p->props.symbolType = properties.symbol;
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


bool TrackLine::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	LineDrawer::draw(drawLoop, cbArea);
	if (getRootDrawer()->is3D())
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
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
	stay = true;
	graphForm = 0;
	working = false;
	fDown = false;
	line = 0;
	point = 0;
	fSet = false;
}

TrackProfileTool::~TrackProfileTool() {
	drawer->getRootDrawer()->setTopDrawer(0);
	if ( line)
		drawer->getRootDrawer()->removeDrawer(line->getId(), true);
	if (point)
		drawer->getRootDrawer()->removeDrawer(point->getId(), true);
	for(int i=0; i < sources.size(); ++i) 
		delete sources[i];
	sources.clear();
	if (graphForm) {
		graphForm->tool = 0; // prevent the graphForm from cleaning up the tool, since the tool is hereby "gone".
		if (graphForm->m_hWnd != 0 && IsWindow(graphForm->m_hWnd)) {
			graphForm->wnd()->PostMessage(WM_CLOSE);
		}
	}
}

void TrackProfileTool::clear() {
}

bool TrackProfileTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);
	if (!layerDrawerTool && !setDrawerTool)
		return false;

	NewDrawer *nddr = setDrawerTool ? setDrawerTool->getDrawer() : layerDrawerTool->getDrawer()->getParentDrawer();
	SpatialDataDrawer *sddr = dynamic_cast<SpatialDataDrawer *>(nddr);
	if(!sddr)
		return false;

	fSet = (setDrawerTool != 0);

	Domain dm;
	dm.SetPointer(sddr->getSourceSupportObject(IlwisObject::iotDOMAIN));
	if (!dm.fValid() )
		return false;

	bool usable= dm->pdv() || dm->pdc() || dm->pdid() || dm->pdi() || dm->pdbool();
	if (!usable)
		return false;

	parentTool = tool;
	return true;
}

HTREEITEM TrackProfileTool::configure( HTREEITEM parentItem) {
	if (!line) {
		DrawerParameters dp(drawer->getRootDrawer(), drawer);
		line = new TrackLine(&dp);
		line->setActive(false);
		drawer->getRootDrawer()->addPostDrawer(733,line);
	}

	if (!point) {
		DrawerParameters dp2(drawer->getRootDrawer(), drawer);
		point = new TrackMarker(&dp2, line);
		point->setActive(false);
		PreparationParameters pp(NewDrawer::ptRENDER);
		point->prepare(&pp);
		drawer->getRootDrawer()->addPostDrawer(734,point);
	}

	checkItem = new DisplayOptionTreeItem(tree,parentItem,drawer);
	checkItem->setDoubleCickAction(this,(DTDoubleClickActionFunc)&TrackProfileTool::displayOptionAddList);
	checkItem->setCheckAction(this,0, (DTSetCheckFunc)&TrackProfileTool::setcheckTool);
	htiNode = insertItem(TR("Track Profile"),"Track",checkItem,working);

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
			if (line && point) {
				line->setActive(true); // point will show automatically if xIndex and yIndex haven't changed
				mpvGetView()->Invalidate();
			}
		}
	}
	else {
		tree->GetDocument()->mpvGetView()->noTool(getId());
		if ( graphForm) {
			graphForm->ShowWindow(SW_HIDE);
			if (line && point) {
				line->setActive(false);
				point->setActive(false);
				//coords.clear();
				//setCoords();
			}
			mpvGetView()->Invalidate();
		}
	}
}

void TrackProfileTool::uncheckTool() {
	graphForm = 0;
	if (line && point) {
		line->setActive(false);
		point->setActive(false);
		coords.clear();
		setCoords();
	}
	mpvGetView()->Invalidate();
	if (htiNode) {
		if (checkItem)
			checkItem->SwitchCheckBox(false);
		CTreeCtrl& tc = tree->GetTreeCtrl();
		tc.SetCheck(htiNode, false);
	}
}

String TrackProfileTool::getMenuString() const {
	return TR("Track Profile");
}

void TrackProfileTool::displayOptionAddList() {
	if (graphForm)
		new ChooseTrackProfileForm(graphForm, (LayerDrawer *)drawer, this);
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
		if (!fSet && (( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) || ( IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION ))) { // unpack maplist or objectcollection and add every individual map
			if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
				MapList mpl (obj->fnObj);
				for (int i = 0; i < mpl->iSize(); ++i) {
					Map mp = mpl[i];
					if ( isUnique(mp->fnObj)) {
						TrackDataSource *src = new TrackDataSource(mp);
						sources.push_back(src);
						if (graphForm)
							graphForm->addSource(src->getSource());
					}					
				}
			} else { // objectcollection
				ObjectCollection oc (obj->fnObj);
				for (int i = 0; i < oc->iNrObjects(); ++i) {
					IlwisObject io = oc->ioObj(i);
					if ( IOTYPEBASEMAP(io->fnObj) && isUnique(io->fnObj)) {
						TrackDataSource *src = new TrackDataSource(io);
						sources.push_back(src);
						if (graphForm)
							graphForm->addSource(src->getSource());
					}					
				}
			}
		} else if ( isUnique(obj->fnObj)) {
			TrackDataSource *src = new TrackDataSource(obj);
			sources.push_back(src);
			if (graphForm)
				graphForm->addSource(src->getSource());
		}
	}
}

void TrackProfileTool::setCoords() {
	line->addCoords(coords, 1);
}

void TrackProfileTool::setCoords(const vector<Coord>& crds) {
	coords = crds;
	line->addCoords(coords, 1);
	mpvGetView()->Invalidate();
}

void TrackProfileTool::OnMouseMove(UINT nFlags, CPoint pnt)
{
	if ( !line->isActive())
		return;

	if (fDown) {
		SetCursor(zCursor(IDC_CROSS));
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
		tree->GetTreeCtrl().SetCheck(htiNode);
	}
}

void TrackProfileTool::OnLButtonUp(UINT nFlags, CPoint pnt)
{
	if ( !line->isActive()) {
		line->setActive(true);
		point->setActive(false);
		if (graphForm)
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
			drawer->getRootDrawer()->addPostDrawer(733,line);
			if (graphForm) {
				graphForm->setTrack(coords);
				graphForm->ShowWindow(SW_SHOW);
			}
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
			FileName fn = sources[j]->getSource()->fnObj;
			//if ( fn == fnDrw) {
				sources[j]->updateIndex(cdrw->getCurrentIndex());
			//}
		}
		if ( graphForm)
			graphForm->update();
	}
}

void TrackProfileTool::updateCbStretch() {
	if (graphForm)
		graphForm->updateCbStretch();
}

void TrackProfileTool::setActiveMode(bool yesno) {
	DrawerTool::setActiveMode(yesno);
}
//-------------------------------------------------------------------
TrackDataSource::TrackDataSource(const IlwisObject& obj) {
	addSource(obj);
}

void TrackDataSource::addSource(const IlwisObject& obj) {
	currentIndex = 0;
	if ( IOTYPEBASEMAP(obj->fnObj)) {
		bmp = BaseMap(obj->fnObj);
		dm = bmp->dm();
	} else if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
		mpl = MapList(obj->fnObj);
		dm = mpl[0]->dm();
	} else if ( IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION ) {
		oc = ObjectCollection(obj->fnObj);
		dm = oc->ioObj(0)->dminf().dm();
	}
}

ILWIS::LayerDrawer *TrackDataSource::getLayerDrawer(ILWIS::NewDrawer *ndr) const{

	ILWIS::ComplexDrawer *cdr = (ILWIS::ComplexDrawer *)ndr;
	ILWIS::LayerDrawer *ldr = 0;
	if ( !cdr->isSet()) {
		ldr = (ILWIS::LayerDrawer *)cdr;

	} else {
		ILWIS::SetDrawer *spdr = (ILWIS::SetDrawer *)ndr;
		int activeIndex = spdr->getCurrentIndex();
		if ( activeIndex != iUNDEF)
			ldr = (ILWIS::LayerDrawer *)spdr->getDrawer(activeIndex);

	}
	return ldr;
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

RangeReal TrackDataSource::getRange() const{
	return overruleRange;
}

void TrackDataSource::setRange(const RangeReal& rng) {
	overruleRange = rng;
}

void TrackDataSource::updateIndex(long ind){
	currentIndex = ind;
}

Domain TrackDataSource::domain() const {
	return dm;
}

//-------------------------------------------------------------------
ChooseTrackProfileForm::ChooseTrackProfileForm(CWnd *wPar, LayerDrawer *dr, TrackProfileTool *t) : 
	DisplayOptionsForm2(dr,wPar,TR("Add data source"),fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON | fbsOKHASCLOSETEXT),
	tool(t)
{
	fm = new FieldDataType(root,TR("Data source"),&name, ".mpr.mps.mpa.mpp.mpl",true);
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
DisplayOptionsForm2(dr,wPar,TR("Track Profile Graph"),fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON), graph(0), tool(t), yStretch(false)
{
	vector<FLVColumnInfo> v;
	v.push_back(FLVColumnInfo("Source", 220));
	v.push_back(FLVColumnInfo("Distance", 70));
	v.push_back(FLVColumnInfo("Value range", 130, true));
	v.push_back(FLVColumnInfo("Value", 100));
	graph = new TrackProfileGraphEntry(root,t);
	cbStretch = new CheckBox(root, "Independent Scaling", &yStretch);
	cbStretch->SetCallBack((NotifyProc)&TrackProfileGraphFrom::stretchClicked);
	FieldListView *view = new FieldListView(root,v);
	graph->setListView(view);
	FieldGroup *grbuttons = new FieldGroup(root);
	PushButton *pb1 = new PushButton(grbuttons,TR("Load track"),(NotifyProc)&TrackProfileGraphFrom::loadTrack);
	PushButton *pb2 = new PushButton(grbuttons,TR("Save track"), (NotifyProc)&TrackProfileGraphFrom::saveTrack);
	PushButton *pb3 = new PushButton(grbuttons,TR("Open track as Table"), (NotifyProc)&TrackProfileGraphFrom::openAsTable);
	pb2->Align(pb1, AL_AFTER);
	pb3->Align(pb2, AL_AFTER);
	grbuttons->SetIndependentPos();
	create();
	view->setItemChangedCallback(graph, (NotifyItemChangedProc)&TrackProfileGraphEntry::setOverruleRange);
	view->setContextMenuCallback(graph, (NotifyContextMenuProc)&TrackProfileGraphEntry::onContextMenu);
	cbStretch->disable();
}

int TrackProfileGraphFrom::openAsTable(Event *ev){
	if ( graph) {
		graph->openAsTable();
	}
	return 1;
}

class LoadTrackForm: public FormWithDest
{
public:
	LoadTrackForm(CWnd* parent, String* sName)
		: FormWithDest(parent, TR("Load track"))
	{
		new FieldDataTypeLarge(root, sName, ".mps");
		//      SetMenHelpTopic(htpOpenSegmentMap);
		create();
	}
};
int TrackProfileGraphFrom::loadTrack(Event *ev) {

	String sMap;
	LoadTrackForm frm(this, &sMap);
	if (!frm.fOkClicked()) 
		return 1;

	if ( sMap == "") 
		return 1;

	FileName fnSeg(sMap);
	SegmentMap smp(fnSeg);

	if ( !smp.fValid())
		return 1;

	if (smp->iFeatures() != 1) {
		throw ErrorObject(TR("Track segmentmap may only contain 1 segment"));
	}

	Segment *seg = CSEGMENT(smp->getFeature(0));
	if (!seg)
		return 1;

	const CoordinateSequence *seq = seg->getCoordinatesRO();
	vector<Coord> coords;
	for(int j = 0; j < seq->size(); ++j) {
		Coord c = seq->getAt(j);
		Coord crd = drw->getRootDrawer()->getCoordinateSystem()->cConv(smp->cs(),c);
		coords.push_back(crd);
	}
	setTrack(coords);
	tool->setCoords(coords);

	return 1;
}

int TrackProfileGraphFrom::stretchClicked(Event *)
{
	cbStretch->StoreData();
	if (graph) {
		graph->setYStretch(yStretch);
	}
	return 0;  
}

void TrackProfileGraphFrom::updateCbStretch() {
	if (tool->sources.size() > 1)
		cbStretch->enable();
	else
		cbStretch->disable();
}

class SegmentMapNameForm : public FormWithDest {
public:
	SegmentMapNameForm(CWnd *par,String *name) : FormWithDest(par,TR("Save as Segmentmap"),fbsSHOWALWAYS | fbsMODAL) {
		new FieldString(root,TR("Segmentmap name"),name);
		//create();
	}

	int exec() {
		FormWithDest::exec();
		return 1;
	}
};
int TrackProfileGraphFrom::saveTrack(Event *ev) {
	if ( trackCoords.size() == 0)
		return 0;

	SpatialDataDrawer *spdrw = dynamic_cast<SpatialDataDrawer *>(drw->getParentDrawer());
	if(!spdrw)
		return 0;

	BaseMapPtr *bmp = spdrw->getBaseMap();
	if ( !bmp)
		return 0;
	String fname("Profile_Track");
	if ( SegmentMapNameForm(this, &fname).DoModal() == IDOK) {
		FileName fnSeg(fname,".mps");
		Domain dom(fnSeg, 1, dmtUNIQUEID, "Seg");
		CoordSystem csy = bmp->cs();
		CoordBounds bnds = bmp->cb();
		SegmentMap smp(fnSeg,csy,bnds, DomainValueRangeStruct(dom));
		Segment *seg = CSEGMENT(smp->newFeature());
		CoordinateSequence *sq = new CoordinateArraySequence();
		for(int i=0; i < trackCoords.size(); ++i) {
			sq->add(trackCoords[i]);
		}
		seg->PutCoords(sq);
		seg->PutVal(1L);
	}

	return 1;
}

void TrackProfileGraphFrom::setTrack(const vector<Coord>& crds) {
	if ( graph) {
		trackCoords.clear();
		trackCoords = crds;
		graph->setTrack(crds);
		graph->setIndex(iUNDEF,0,Coord());
	}
	if (tool)
		tool->setMarker(Coord());
}

void TrackProfileGraphFrom::addSource(const IlwisObject& bmp) {
	if ( graph)
		graph->addSource(bmp);
	updateCbStretch();
}

void TrackProfileGraphFrom::reset() {
	if ( graph)
		graph->setIndex(iUNDEF,0,Coord());
	if (tool)
		tool->setMarker(Coord());
}

void TrackProfileGraphFrom::update() {
	if ( graph)
		graph->update();
}

void TrackProfileGraphFrom::shutdown(int iReturn) {
	if (tool)
		tool->uncheckTool();
	DisplayOptionsForm2::shutdown(iReturn);
}