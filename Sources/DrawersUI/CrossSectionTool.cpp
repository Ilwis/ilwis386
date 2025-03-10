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
#include "Client\FormElements\FieldListView.h"
#include "CrossSectionGraph.h"
#include "Engine\Drawers\ComplexDrawer.h" 
#include "Engine\Drawers\SimpleDrawer.h"
#include "drawers\pointdrawer.h"
#include "DrawersUI\CrossSectionTool.h"
#include "headers\constant.h"

using namespace ILWIS;
ProbeMarkers::ProbeMarkers(DrawerParameters *parms) : 
ComplexDrawer(parms,"ProbeMarkers")
{
}

ProbeMarkers::~ProbeMarkers() {
}


void ProbeMarkers::prepare(PreparationParameters *p){
	//properties.symbol = "crosshair";
	//setSpecialDrawingOptions(NewDrawer::sdoExtrusion, true);
	ComplexDrawer::prepare(p);
}

void ProbeMarkers::addMarker(const Coord& crd) {
	DrawerParameters dp(this->getRootDrawer(), this);
	int currentNr = getDrawerCount();
	Color clr = Representation::clrPrimary(currentNr == 0 ? 1 : currentNr + 3);

	PointDrawer *pdrw = new PointDrawer(&dp);
	PointProperties *prop = (PointProperties *)pdrw->getProperties();
	prop->symbol = "crosshair";
	prop->drawColor = clr;
	pdrw->setCoord(crd);
	PreparationParameters pp(NewDrawer::ptRENDER);
	pp.props.symbolType = prop->symbol;
	pdrw->prepare(&pp);
	addDrawer(pdrw);
}

void ProbeMarkers::setLastMarkerCoord(const Coord& crd) {
	int sz = drawers.size();
	if (sz > 0) {
		PointDrawer * pdrw = (PointDrawer *)(drawers[sz - 1]);
		pdrw->setCoord(crd);
	}
}

DrawerTool *createCrossSectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new CrossSectionTool(zv, view, drw);
}

CrossSectionTool::CrossSectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("CrossSectionTool",zv, view, drw)
, fDown(false)
{
	active = false;
	stay = true;
	graphForm = 0;
	working = false;
	markers = 0;
}

CrossSectionTool::~CrossSectionTool() {
	if ( markers)
		drawer->getRootDrawer()->removeDrawer(markers->getId(), true);
	sources.clear();
	if (graphForm) {
		graphForm->tool = 0; // prevent the graphForm from cleaning up the tool, since the tool is hereby "gone".
		if (graphForm->m_hWnd != 0 && IsWindow(graphForm->m_hWnd)) {
			graphForm->wnd()->PostMessage(WM_CLOSE);
		}
	}
}

void CrossSectionTool::clear() {
}

bool CrossSectionTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);
	if (!layerDrawerTool)
		return false;
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(layerDrawerTool->getDrawer());
	if ( !ldrw)
		return false;

	SpatialDataDrawer *sddr = dynamic_cast<SpatialDataDrawer *>(ldrw->getParentDrawer());
	if ( !sddr)
		return false;
	Domain dm;
	dm.SetPointer(sddr->getSourceSupportObject(IlwisObject::iotDOMAIN));
	if (!dm.fValid() )
		return false;

	bool usable= dm->pdv() || dm->pdi() || dm->pdbool();
	if (!usable)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM CrossSectionTool::configure( HTREEITEM parentItem) {
	if (!markers) {
		DrawerParameters dp(drawer->getRootDrawer(), drawer);
		markers = new ProbeMarkers(&dp);
		markers->setActive(false);
		drawer->getRootDrawer()->addPostDrawer(732,markers);
	}

	checkItem = new DisplayOptionTreeItem(tree,parentItem,drawer);
	checkItem->setDoubleCickAction(this,(DTDoubleClickActionFunc)&CrossSectionTool::displayOptionAddList);
	checkItem->setCheckAction(this,0, (DTSetCheckFunc)&CrossSectionTool::setcheckTool);
	htiNode = insertItem(TR("Layer Profile"),"CrossSection",checkItem,working);

	DrawerTool::configure(htiNode);

	return htiNode;
}

void CrossSectionTool::setcheckTool(void *w, HTREEITEM item) {
	working = *(bool *)w;
	setActive(working); // noone else will set us active/inactive
	if ( working) {
		mpvGetView()->noTool(ID_ZOOMIN);
		mpvGetView()->noTool(ID_ZOOMOUT);
		mpvGetView()->noTool(ID_PANAREA);
		if (!mpvGetView()->addTool(this, getId()))
			mpvGetView()->changeStateTool(getId(), true);
		if (!graphForm) {
			graphForm = new CrossSectionGraphFrom(tree, (LayerDrawer *)drawer, sources, this);
			for(int i = 0; i < sources.size(); ++i) {
				graphForm->addSourceSet(sources[i]);
			}
		} else {
			graphForm->ShowWindow(SW_SHOW);
			if (markers) {
				markers->setActive(true);
				mpvGetView()->Invalidate();
			}
		}
		if ( sources.size() == 0)
			displayOptionAddList();

		for(int i = 0; i < sources.size(); ++i)
			graphForm->addSourceSet(sources[i]);
	}
	else {
		mpvGetView()->changeStateTool(getId(), false);
		if ( graphForm) {
			graphForm->ShowWindow(SW_HIDE);
			if (markers) {
				markers->setActive(false);
				//markers->clear();
				//graphForm->reset();
				mpvGetView()->Invalidate();
			}
		}
	}
}

void CrossSectionTool::uncheckTool() {
	graphForm = 0;
	if (markers) {
		markers->setActive(false);
		markers->clear();
	}
	mpvGetView()->Invalidate();
	if (htiNode) {
		if (checkItem)
			checkItem->SwitchCheckBox(false);
		CTreeCtrl& tc = tree->GetTreeCtrl();
		tc.SetCheck(htiNode, false);
	}
}

String CrossSectionTool::getMenuString() const {
	return TR("Layer Profile");
}

void CrossSectionTool::displayOptionAddList() {
	if (graphForm)
		new ChooseCrossSectionForm(graphForm, (LayerDrawer *)drawer, this);
}

bool CrossSectionTool::isUnique(const FileName& fn) {
	
	for(int i=0; i < sources.size(); ++i) {
		if ( sources.at(i)->fnObj == fn)
			return false;
	}
	return true;
}

void CrossSectionTool::addSource(const FileName& fn) {
	if ( IOTYPE(fn) == IlwisObject::iotMAPLIST) {
		MapList mpl(fn);
		if( mpl.fValid()) {
			if ( isUnique(mpl->fnObj)) {
				sources.push_back(mpl);
				if (graphForm)
					graphForm->addSourceSet(mpl);
			}
		}
	} else if (IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc(fn);
		if( oc.fValid()) {
			if ( isUnique(oc->fnObj)) {
				sources.push_back(oc);
				if (graphForm)
					graphForm->addSourceSet(oc);
			}
		}
	}
}

void CrossSectionTool::OnLButtonDown(UINT nFlags, CPoint pnt) {
	fDown = true;
	short state = ::GetKeyState(VK_CONTROL);
	Coord c1 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(pnt.y, pnt.x));
	markers->setActive(true);
	if ( (state & 0x8000)) {
		if ( graphForm)
			graphForm->addCoord(c1);
		markers->addMarker(c1);
	} else {
		markers->clear();
		if ( graphForm) {
			graphForm->reset();
			graphForm->addCoord(c1);
		}
		markers->addMarker(c1);
	}
	mpvGetView()->Invalidate();
}

void CrossSectionTool::OnMouseMove(UINT nFlags, CPoint point) {
	if ( mpvGetView()->iActiveTool == ID_ZOOMIN) // during zooming, no message handling
		return;

	if (fDown) {
		Coord c1 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(point.y, point.x));
		markers->setLastMarkerCoord(c1);
		if ( graphForm) {
			graphForm->setLastCoord(c1);
		}
		mpvGetView()->Invalidate();
	}
}

void CrossSectionTool::OnLButtonUp(UINT nFlags, CPoint point) {
	fDown = false;
	if ( mpvGetView()->iActiveTool == ID_ZOOMIN) // during zooming, no message handling
		return;

	Coord c1 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	markers->setLastMarkerCoord(c1);
	if ( graphForm) {
		graphForm->setLastCoord(c1);
	}
	mpvGetView()->Invalidate();
}

void CrossSectionTool::updateCbStretch() {
	if (graphForm)
		graphForm->updateCbStretch();
}

void CrossSectionTool::setCustomRange() {
	if (graphForm)
		graphForm->setCustomRange();
}

//-------------------------------------------------------------------
ChooseCrossSectionForm::ChooseCrossSectionForm(CWnd *wPar, LayerDrawer *dr, CrossSectionTool *t) : 
	DisplayOptionsForm2(dr,wPar,TR("Add data source"),fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON | fbsOKHASCLOSETEXT),
	tool(t)
{
	fm = new FieldDataType(root,TR("Data source"),&name, ".mpl.ioc",true);
	new PushButton(root, TR("Add"),(NotifyProc) &ChooseCrossSectionForm::addSource);
	create();
}


int ChooseCrossSectionForm::addSource(Event *ev) {
	fm->StoreData();
	if ( name != "") {
		tool->addSource(FileName(name));
	}
	return 1;
}

int ChooseCrossSectionForm::exec() {
	fm->StoreData();
	if ( name != "") {
		tool->addSource(FileName(name));
	}

	return 1;
}

//========================================================================
CrossSectionGraphFrom::CrossSectionGraphFrom(CWnd *wPar, LayerDrawer *dr, vector<IlwisObject>& sources, CrossSectionTool *t) :
DisplayOptionsForm2(dr,wPar,TR("Layer Profile Graph"),fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON,true)
, tool(t)
, yStretch(false)
, iScaleMethod(MAP)
{
	vector<FLVColumnInfo> v;
	v.push_back(FLVColumnInfo("Source", 200));
	v.push_back(FLVColumnInfo("Probe", 40));
	v.push_back(FLVColumnInfo("Index range", 80));
	v.push_back(FLVColumnInfo("Value range", 130, true));
	v.push_back(FLVColumnInfo("Selected index", 90));
	v.push_back(FLVColumnInfo("Value", 100));
	graph = new CrossSectionGraphEntry(root, sources,dr->getRootDrawer()->getCoordinateSystem(),t);
	addResizableFormEntry(graph);
	rgStretch = new RadioGroup(root, "Scale", (int*)&iScaleMethod, true);
	rgStretch->SetIndependentPos();
	new RadioButton(rgStretch, "Map");
	new RadioButton(rgStretch, "Probe");
	new RadioButton(rgStretch, "Custom");
	rgStretch->SetCallBack((NotifyProc)&CrossSectionGraphFrom::scaleChanged);
	cbStretch = new CheckBox(root, "Independent Scaling", &yStretch);
	cbStretch->SetCallBack((NotifyProc)&CrossSectionGraphFrom::stretchClicked);
	cbStretch->Align(rgStretch, AL_AFTER);
	FieldListView * flv = new FieldListView(root,v);
	addResizableFormEntry(flv);
	flv->Align(rgStretch, AL_UNDER);
	graph->setListView(flv);
	FieldGroup *fg = new FieldGroup(root,true);
	PushButton *pb1 = new PushButton(fg,TR("Save as Table"), (NotifyProc)&CrossSectionGraphFrom::saveAsTable);
	PushButton *pb2 = new PushButton(fg,TR("Save as Spectrum"), (NotifyProc)&CrossSectionGraphFrom::saveAsSpectrum);
	pb2->Align(pb1, AL_AFTER);
	create();
	flv->setItemChangedCallback(graph, (NotifyItemChangedProc)&CrossSectionGraphEntry::setOverruleRange);
	flv->setContextMenuCallback(graph, (NotifyContextMenuProc)&CrossSectionGraphEntry::onContextMenu);
	cbStretch->disable();
}

int CrossSectionGraphFrom::saveAsTable(Event *ev) {
	graph->graph()->saveAsTbl();
	return 1;
}

int CrossSectionGraphFrom::saveAsSpectrum(Event *ev) {
	graph->graph()->saveAsSpectrum();
	return 1;
}

void CrossSectionGraphFrom::addSourceSet(const IlwisObject& obj) {
	graph->graph()->recomputeValues();
	graph->update();
	updateCbStretch();
}

void CrossSectionGraphFrom::addCoord(const Coord& crd) {
	graph->addCoord(crd);
}

void CrossSectionGraphFrom::setLastCoord(const Coord& crd) {
	graph->setLastCoord(crd);
}

void CrossSectionGraphFrom::reset() {
	graph->reset();
}

void CrossSectionGraphFrom::shutdown(int iReturn) {
	if (tool)
		tool->uncheckTool();
	DisplayOptionsForm2::shutdown(iReturn);
}

int CrossSectionGraphFrom::stretchClicked(Event *)
{
	cbStretch->StoreData();
	if (graph) {
		graph->setYStretch(yStretch);
	}
	return 0;  
}

int CrossSectionGraphFrom::scaleChanged(Event *)
{
	rgStretch->StoreData();
	if (graph) {
		if (iScaleMethod == MAP) {
			graph->setYStretchOnSamples(false);
		} else if (iScaleMethod == PROBE) {
			graph->setYStretchOnSamples(true);
		} else if (iScaleMethod == CUSTOM) {
			graph->setCustomRange();
			graph->setYStretch(yStretch); // this is only here to trigger an Invalidate()
		}
	}
	return 0;  
}

void CrossSectionGraphFrom::updateCbStretch() {
	if (tool->sources.size() > 1)
		cbStretch->enable();
	else
		cbStretch->disable();
}

void CrossSectionGraphFrom::setCustomRange() {
	if (graph)
		graph->setCustomRange();
	rgStretch->SetVal(CUSTOM);
	rgStretch->StoreData();
}