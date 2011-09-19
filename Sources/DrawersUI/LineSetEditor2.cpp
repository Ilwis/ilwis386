#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Segment\Seg.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Client\Mapwindow\PixelInfoBar.h"
#include "Client\Mapwindow\PixelInfoView.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Domain\dmsort.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "FeatureSetEditor2.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "geos/operation/polygonize/Polygonizer.h"
#include "drawers\linedrawer.h"
#include "drawers\linefeaturedrawer.h"
#include "LineSetEditor2.h"
#include "Headers\constant.h"
#include "Headers\Hs\Editor.hs"
#include "Headers\Hs\Appforms.hs"
#include "Headers\Htp\Ilwis.htp"

UndoItem::~UndoItem() {
	delete seq;
	seq = 0;
}
//------------------------------------
PointClickInfo::PointClickInfo(Segment *s,NewDrawer *drw, long index, const Coord& crd) : 
seg(s), 
drawer(drw), 
crdIndex(index), 
crdClick(crd), 
linePoint(false) 
{
}
//-----------------------------------------------------

SelectedSegment::~SelectedSegment() {
}
SelectedSegments::~SelectedSegments(){
	empty();
}

void SelectedSegments::empty() {
	for(int i=0; i <  size(); ++i) {
		if ( at(i)->drawer)
			at(i)->drawer->setSpecialDrawingOptions(NewDrawer::sdoSELECTED,false);
		delete at(i);
	}
	clear();
}

void SelectedSegments::remove(Segment *seg, int index){
	if ( seg == 0) {
		empty();
		return;
	}
	for(int i=0; i <  size(); ++i) {
		if ( at(i)->seg->getGuid() == seg->getGuid() && index == iUNDEF) {
			delete at(i);
			return;
		} else {
			if ( index == -1) {
				at(i)->selectedCoords.clear();
				return;
			}
			SelectedSegment *ss = at(i);
			vector<int>::const_iterator cur = find(ss->selectedCoords.begin(), ss->selectedCoords.end(), index);
			if ( cur != ss->selectedCoords.end()) {
				ss->selectedCoords.erase(cur);
				return;
			}

		}
	}
}

SelectedSegment *SelectedSegments::add(Segment *seg, NewDrawer *drw, int index, bool add){
	if ( seg == 0) {
		empty();
		return 0;
	}
	if ( index == iUNDEF || !add) {
		empty();
	}
	for(int i=0; i <  size(); ++i) {
		if ( at(i)->seg->getGuid() == seg->getGuid()) {
			SelectedSegment *ss = at(i);
			if ( index != iUNDEF) {
				if ( !add ) 
					ss->selectedCoords.clear();

				vector<int>::const_iterator cur = find(ss->selectedCoords.begin(), ss->selectedCoords.end(), index);
				if ( cur == ss->selectedCoords.end())
					ss->selectedCoords.push_back(index);
			}
			return ss;
		}
	}
	SelectedSegment *ss = new SelectedSegment();
	ss->seg = seg;
	ss->drawer = drw;
	if ( index >= 0)
		ss->selectedCoords.push_back(index);
	push_back(ss);

	return ss;
}

BEGIN_MESSAGE_MAP(LineSetEditor2, FeatureSetEditor2)
	ON_COMMAND(ID_UNDO, OnUndo)
	ON_UPDATE_COMMAND_UI(ID_UNDO, OnUpdateUndo)
	ON_COMMAND(ID_REDO, OnRedo)
	ON_UPDATE_COMMAND_UI(ID_UNDO, OnUpdateRedo)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_SELALL, OnSelectAll)
END_MESSAGE_MAP()

//
//

#define sMen(ID) ILWSF("men",ID).c_str()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 

DrawerTool *createLineSetEditor2(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new LineSetEditor2(zv, view, drw);
}

LineSetEditor2::LineSetEditor2(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
FeatureSetEditor2("LineSetEditor2",zv, view, drw),
curSegSplit("EditSplitCursor"),
curSegSplitting("EditSplittingCursor"),
rSnapDistance(0.02),
insertionPoint(iUNDEF)
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");
	editorState = msNONE;
	useAutoClose = false;
	fAutoSnap = true;

}

LineSetEditor2::~LineSetEditor2(){
	while( !undoItems.empty()) {
		delete undoItems.back();
		undoItems.pop_back();
	}
}

String LineSetEditor2::getMenuString() const {
	if ( drawer->inEditMode()) {
		return TR("Exit Segmentmap Editor");
	}
	else{
		return TR("Segmentmap Editor");
	}
}

bool LineSetEditor2::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	LayerDrawerTool *sdrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if (!sdrwt)
		return false;
	ILWIS::LayerDrawer *sdrw = dynamic_cast<ILWIS::LayerDrawer *>(sdrwt->getDrawer());
	if ( !sdrw)
		return false;
	BaseMapPtr *bmp = ((SpatialDataDrawer *)sdrw->getParentDrawer())->getBaseMap();
	if ( !bmp || IOTYPE(bmp->fnObj) != IlwisObject::iotSEGMENTMAP)
		return false;

	if ( bmp->fReadOnly())
		return false;
	if ( drawer->inEditMode())
		active = true;
	parentTool = tool;
	return true;
}

HTREEITEM LineSetEditor2::configure( HTREEITEM parentItem) {
	getSettings();
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&LineSetEditor2::checkSegments);
	htiNode = insertItem(TR("Segmentmap Editor 2"),"EditMap", item);
	FeatureSetEditor2::configure(htiNode);
	DisplayOptionRadioButtonItem *ritem = new DisplayOptionRadioButtonItem(TR("Split"), tree,htiNode,drawer);
	ritem->setState(false);
	ritem->setCheckAction(this,editModeItems,0);
	insertItem(TR("Split"),"Bitmap", ritem);
	item = new DisplayOptionTreeItem(tree,hitInsert,drawer);
	item->setCheckAction(this, 0, (DTSetCheckFunc) &LineSetEditor2::checkAutoClose);
	insertItem(TR("Auto Close"),"AutoClose",item,useAutoClose);
	item = new DisplayOptionTreeItem(tree,hitInsert,drawer);
	item->setCheckAction(this, 0, (DTSetCheckFunc) &LineSetEditor2::checkSnap);
	insertItem(TR("Enable Snap"),"Snap",item,fAutoSnap);
	//item = new DisplayOptionTreeItem(tree,hitSelect,drawer);
	//item->setCheckAction(this, 0, (DTSetCheckFunc) &LineSetEditor2::checkUndefined);
	//insertItem(TR("Mark Undefined"),"Undefined",item,fAutoSnap);



	CMenu men;
	CMenu menSub;
	men.CreateMenu();

	addmen(ID_UNDO);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_COPY );
	addmen(ID_PASTE);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_SELALL);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();

	DataWindow* dw = mdoc->mpvGetView()->dwParent();
	if (dw) {
		//dw->bbDataWindow.LoadButtons("segedit.but");
		//dw->RecalcLayout();
		mdoc->mpvGetView()->mwParent()->UpdateMenu(hmenFile, hmenEdit);
		//mdoc->mpvGetView()->UpdateWindow();
	}
	return htiNode;
}
void LineSetEditor2::checkUndefined(void *v, HTREEITEM it) {
	bool val = *(bool *)v;
}

void LineSetEditor2::checkAutoClose(void *v, HTREEITEM it) {
	bool val = *(bool *)v;
	useAutoClose = val;
}

void LineSetEditor2::checkSnap(void *v, HTREEITEM it) {
	bool val = *(bool *)v;
	fAutoSnap = val;
}

void LineSetEditor2::checkSegments() {
	new CheckSegmentsForm(tree, (ILWIS::LayerDrawer *)layerDrawer, this);
}

void LineSetEditor2::getSettings() {

	IlwisSettings settings("Map Window\\Segment Editor");
	String fn = IlwWinApp()->Context()->fnUserINI().sFullName();
}

bool LineSetEditor2::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu men;
	men.CreatePopupMenu();
	addmen(ID_NORMAL);
	addmen(ID_ZOOMIN);
	addmen(ID_ZOOMOUT);
	addmen(ID_PANAREA);

	addmen(ID_CONFIGURE);
	addmen(ID_EXITEDITOR);
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	return true;

	return true;
}

zIcon LineSetEditor2::icon() const {
	return zIcon("SegIcon");
}

String LineSetEditor2::sTitle() const
{
	String s(TR("LineEditor ") + bmapptr->sName());
	return s;
}



bool LineSetEditor2::hasSelection() const { 
	return selectedSegments.size() > 0; 
}


void LineSetEditor2::updateFeature(SelectedFeature *f) {
	ILWIS::Segment *seg = CSEGMENT(f->feature);
	CoordinateSequence *seq = seg->getCoordinates();
	for(int i = 0; i < f->selectedCoords.size(); ++i) {
		int index = f->selectedCoords[i];
		Coordinate c = *(f->coords[index]);
		seq->setAt(c,index); 

	}
	seg->PutCoords(seq);
}

void LineSetEditor2::OnUpdateUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(undoItems.size() > 0);	
}

void LineSetEditor2::OnUpdateRedo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

void LineSetEditor2::OnUpdateMode(CCmdUI* pCmdUI)
{
	BOOL fCheck, fEnable=true;
	switch (pCmdUI->m_nID) {
	   case ID_SELECTMODE:
		   fCheck = (FeatureSetEditor2::msSELECT & editorState) != 0;
		   break;
	   case ID_MOVEMODE:
		   fCheck = (FeatureSetEditor2::msMOVE & editorState) != 0 || 
			   (FeatureSetEditor2::msMOVEINSERTVERTICES & editorState) != 0 ||
			   (FeatureSetEditor2::msMOVEVERTICES & editorState) != 0;
		   break;
	   case ID_INSERTMODE:
		   fCheck = (FeatureSetEditor2::msINSERT & editorState) != 0;
		   break;
	   case ID_SPLITMODE:
		   fCheck = FeatureSetEditor2::msSPLIT == editorState;
		   break;
	}
	//if (0 != mdoc->mpvGetView()->tools.size())
	//	fCheck = false;
	pCmdUI->SetRadio(fCheck);
	pCmdUI->Enable(fEnable);
}

void LineSetEditor2::setMode(FeatureSetEditor2::States state) 
{
	editorState = editorState & ~ (msINSERT | msMOVE | msSELECT | msSPLIT | msMOVEINSERTVERTICES | msMOVEVERTICES);
	editorState |= state;

	int selectedIndex = editModeItems->getState() ;
	if ( (editorState & msSPLIT) != 0) {
		curActive = curSegSplit;
		if ( selectedIndex != 3) {
			editModeItems->checkItem(3);
		}
	}
	else if ( (editorState & msMOVE) != 0) {
		curActive = curMove;
		if ( selectedIndex != 4) {
			editModeItems->checkItem(4);
		}
		pci = PointClickInfo(); // invalidate last click. next point must still be chosen
	}
	else
		FeatureSetEditor2::setMode(state);

	OnSetCursor();
}

void LineSetEditor2::OnRedo()
{
}

void LineSetEditor2::select(const CoordinateSequence *ls) {
}

bool LineSetEditor2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	editorState = editorState & ~ ( msENTER | msCTRL | msESCAPE | msDELETE);
	if (  nChar == VK_RETURN) {
		editorState |= msENTER;
	}
	else if ( nChar == VK_DELETE) {
		editorState |= msDELETE;
	} else if (  nChar == VK_ESCAPE) {
		editorState |= msESCAPE;
	} else if (  nChar == VK_CONTROL) {
		editorState |= msCTRL;
	} 
	updateState();
	return true;
}

void LineSetEditor2::OnLButtonDown(UINT nFlags, CPoint point){
	editorState = editorState & ~ ( msLMOUSEUP | msMOUSEMOVE);
	editorState |= msLMOUSEDOWN;

}

void LineSetEditor2::OnLButtonUp(UINT nFlags, CPoint point){
	editorState = editorState & ~ ( msLMOUSEDOWN | msMOUSEMOVE);
	editorState |= msLMOUSEUP;
	if ( (nFlags & MK_CONTROL))
		editorState |= msCTRL;
	else
		editorState = editorState & ~msCTRL;

	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	fillPointClickInfo(crd);
	updateState();
}

void  LineSetEditor2::OnMouseMove(UINT nFlags, CPoint point) {
	crdMouse = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	editorState = editorState & ~ msLMOUSEDOWN;
	editorState = editorState & ~  msLMOUSEUP;
	editorState |= msMOUSEMOVE;
	updateState();
}

void LineSetEditor2::fillPointClickInfo(const Coord& crd) {
	CoordBounds cbZoom = mdoc->rootDrawer->getCoordBoundsZoom();
	double delta = max(cbZoom.height(), cbZoom.width()) / 50.0;
	vector<Geometry *> geoms = bmapptr->getFeatures(crd, delta);
	pci = PointClickInfo();
	pci.crdClick = crd;
	for(int i = 0; i < geoms.size(); ++i) {
		Segment *seg = CSEGMENT(geoms.at(i));
		double d;
		long after = seg->nearSection(crd, delta,d);
		if ( after == iUNDEF)
			continue;
		pci.seg = seg;
		pci.section = after - 1;
		double d1 = rDist(crd,seg->getCoordinateN(after - 1));
		double d2 = rDist(crd,seg->getCoordinateN(after));
		if ( d1 < d2 && after < seg->getNumPoints()) {
			//if ( selectedSegments.size() == 0 || (editorState & msSELECT) || (editorState & msMOVE)) // extending existing segment; it will be selected after this operation
			//	pci.crdIndex = after -1;
			//else
			//	pci.crdIndex = seg->getNumPoints() - 1; // snapping a segment being created. It is selected so we are already busy with it.
			pci.crdIndex = after -1;
		} else {
			pci.crdIndex = after;
		}
		if ( min(d1,d2) > delta)
			pci.linePoint = true;

		vector<NewDrawer *> drws;
		mdoc->rootDrawer->getDrawerFor(seg,drws);
		if ( drws.size() > 0) {
			pci.drawer = drws[0];
		}
	}
}

void LineSetEditor2::createFeature() {
	Segment *segment = CSEGMENT(bmapptr->newFeature());
	CoordBuf buf(2);
	buf[0] = pci.crdClick;
	buf[1] = pci.crdClick;
	segment->PutCoords(buf.iSize(), buf);
	segment->PutVal(0L);

	ILWIS::DrawerParameters parms(layerDrawer->getRootDrawer(), layerDrawer);
	ILWIS::NewDrawer *segdrw = NewDrawer::getDrawer("LineFeatureDrawer", "ilwis38", &parms);
	segdrw->addDataSource(CFEATURE(segment));

	PreparationParameters fp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
	segdrw->prepare(&fp);
	layerDrawer->addDrawer(segdrw);

	pci.seg = segment;
	pci.crdIndex = 1;
	pci.drawer = segdrw;
	selectVertex();

}

int LineSetEditor2::noSnap(CoordinateSequence *seq, double snap) {
	if ( !fAutoSnap)
		return iUNDEF;

	for(int i = 0; i < seq->size() - 1; ++i) {
		Coordinate c = seq->getAt(i);
		double d = rDist(c, pci.crdClick);
		if ( d < snap )
			return i;

	}
	return iUNDEF;

}

void LineSetEditor2::insertVertex(bool endEdit) {
	vector<CoordinateSequence*> boundaries;
	pci.seg->getBoundaries(boundaries);
	CoordBounds cb = pci.drawer->getRootDrawer()->getCoordBoundsZoom();
	double snap = rSnapDistance * min(cb.width(), cb.height());
	if ( boundaries.size() == 0 )
		return;

	CoordinateSequence *seq = boundaries[0];

	vector<Coordinate> *copyv = new vector<Coordinate>();

	int k = 0;
	bool added = false;
	Coord cOld, cCurrent;
	while(k < seq->size()){
		long limit = pci.linePoint ? pci.section : pci.crdIndex;
		if ( (k != limit + 1) || added) {
			copyv->push_back(seq->getAt(k++));
			if ( pci.crdIndex == seq->size() - 1 && k == pci.crdIndex) { // insert beyond the end
				long snapPoint = 0;
				if ( !pci.crdClick.fNear(copyv->back(), snap / 10.0)) { // dont insert double points;

					if ( (snapPoint = noSnap(seq, snap)) == iUNDEF) {
						copyv->push_back(pci.crdClick);
					} else {
						copyv->push_back(copyv->at(snapPoint));
					}
					pci.crdIndex += 1;
				}
			}
		}
		else {
			copyv->push_back(pci.crdClick);
			added = true;
		}
	}
	if ( endEdit) {
		if ( useAutoClose && copyv->at(0) != copyv->back())
			copyv->at(copyv->size() - 1) = copyv->at(0);
		else {
			if ( copyv->size() > 3 && copyv->back() == copyv->at(copyv->size() - 2)) // due to the way the editor works the last point is alwats double, so remove it before commting it
				copyv->pop_back();
		}
	}
	if ( copyv->size() > 1) {
		pci.seg->PutCoords(new CoordinateArraySequence(copyv));
		PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
		pci.drawer->prepare(&pp);
	}
	selectVertex();
	if ( endEdit) {
		pci = PointClickInfo(); // end inserting op point(s)
		editorState = editorState & ~ ( msMOVEVERTICES);
	}
}

void LineSetEditor2::selectVertex(bool add) {
	SelectedSegment *ss = selectedSegments.add(pci.seg, pci.drawer, pci.linePoint ? iUNDEF : pci.crdIndex, add);
	if ( ss)
		ss->drawer->setSpecialDrawingOptions(NewDrawer::sdoSELECTED,true,ss->selectedCoords);
}

// splits a segment into two seperate segments at the clicking point. Both segment will have the same value;
void LineSetEditor2::split(){

	CoordBounds cbZoom = mdoc->rootDrawer->getCoordBoundsZoom();
	double delta = max(cbZoom.height(), cbZoom.width()) / 50.0;
	double d;
	long after = pci.seg->nearSection(pci.crdClick, delta,d);
	if ( after == iUNDEF)
		return; 	// nothing there, go back

	// clear any selection as most likely the end result of this operation will invalidate existing drawers (new ones will be created)
	selectedSegments.empty();
	bool isRing = pci.seg->isRing();
	vector<CoordinateSequence*> boundaries;
	pci.seg->getBoundaries(boundaries);
	CoordinateSequence *seq = boundaries[0];
	vector<Coordinate> *copyv1 = new vector<Coordinate>();
	vector<Coordinate> *copyv2 = new vector<Coordinate>();
	vector<Coordinate> *copyv = copyv1;
	NewDrawer *prepareDrawer = 0;
	int k = 0;
	// copy points into two vectors which will be the data for the one or two new segments
	// points will be copied until the clikcing point which will also be added to bot vectors
	// note that it is not simply splitting a segment(though that will be true in most cases). Rings do not become two segment
	// but are "opened"
	while(k < seq->size()){
		// copy to just before the first coord of the next segment in the "old" situation
		if ( k != after) {
			copyv->push_back(seq->getAt(k++));
		}
		else {
			// copy points after the clicking point
			copyv->push_back(pci.crdClick);
			copyv = copyv2;
			copyv->push_back(pci.crdClick);
			after = iUNDEF;
		}
	}

	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
	if ( isRing) {
		// if is a ring values of copyv1 are copied to copv2 vector. endpoints(both) of this new vector is clicking point
		for(int j = 0; j < copyv1->size(); j++) {
			copyv2->push_back(copyv1->at(j));
		}
		// make copyv == copyv2. this ensures that "old" segments will get the new coordinates but now with new endpoints ( the split point)
		copyv = copyv2;
		delete copyv1;
	} else {
		// in the other case a new segment is created and the points of copyv2 are assigned to it.
		ILWIS::Segment *segNew = CSEGMENT(bmapptr->newFeature());
		CoordinateArraySequence *arseq2 = new CoordinateArraySequence(copyv2);
		segNew->PutCoords(arseq2);
		segNew->PutVal(pci.seg->rValue());
		prepareDrawer = drawer;
		// make copyv == copyv1, this ensures that the "old" segment will get the same points as the first part of the splitted segment
		copyv = copyv1;
	}
	CoordinateArraySequence *arseq = new CoordinateArraySequence(copyv);
	// update the "old" segment
	pci.seg->PutCoords(arseq);
	if (prepareDrawer == 0)
		prepareDrawer  = pci.drawer;

	// as there are now new drawers a prepare most be done to generate the new drawerds
	prepareDrawer->prepare(&pp);
	vector<NewDrawer *> drws;
	((ComplexDrawer *)drawer)->getDrawerFor(pci.seg,drws);
	// the click info needs to be updated with the new drawers;
	if ( drws.size() > 0) {
		pci.drawer = drws[0];
		pci.linePoint = false;
		pci.crdIndex = 0;
	}

	return ;

}


void LineSetEditor2::updatePositions(){
	if ( selectedSegments.size() > 0 ) {
		Coord cPivot;
		for(int i =0; i < selectedSegments.size(); ++i) {
			SelectedSegment *ss = selectedSegments.at(i);
			if ( ss->seg->getGuid() == pci.seg->getGuid())
				cPivot = pci.seg->getCoordinateN(pci.crdIndex);
		}
		Coord cDelta = cPivot;
		cDelta -= crdMouse;
		if ( cDelta.x ==0 && cDelta.y == 0)
			return;


		for(int i =0; i < selectedSegments.size(); ++i) {
			SelectedSegment *ss = selectedSegments.at(i);
			CoordinateSequence *seq = ss->seg->getCoordinates();
			for( int j = 0; j <  ss->selectedCoords.size(); ++j) {
				int index = ss->selectedCoords[j];
				Coord cNew = seq->getAt(index);
				cNew -= cDelta;
				seq->setAt(cNew,index);
			}
			ss->seg->PutCoords(seq);
			PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
			ss->drawer->prepare(&pp);
		}
	}
	return ;
}

void LineSetEditor2::deleteVertices() {
	for(int i =0; i < selectedSegments.size(); ++i) {
		SelectedSegment *ss = selectedSegments.at(i);
		bool isFullyRemoved = bmapptr->removeFeature(ss->seg->getGuid(), ss->selectedCoords);
		bool removed = false;
		if ( ss->selectedCoords.size() == 0 || isFullyRemoved) {
			((ComplexDrawer *)drawer)->removeDrawer(ss->drawer->getId());
			ss->seg = 0; // has been deleted and is invalid now
			ss->drawer = 0; // has been deleted and is invalid now
			removed = true;
		}
		PreparationParameters p(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
		if ( !removed) {
			ss->drawer->prepare(&p);
		}
	}
	mdoc->rootDrawer->getDrawerContext()->doDraw();
	pci = PointClickInfo();
	selectedSegments.empty();
}

void LineSetEditor2::OnUndo()
{
	if ( undoItems.size() == 0)
		return;

	UndoItem *item = undoItems.back();
	undoItems.pop_back();
	selectedSegments.empty();
	layerDrawer->removeDrawer(item->oldDrawerId);
	bmapptr->removeFeature(item->oldFeatureId);
	pci = PointClickInfo();

	Segment *segment = CSEGMENT(bmapptr->newFeature());
	ILWIS::DrawerParameters parms(layerDrawer->getRootDrawer(), layerDrawer);
	segment->PutCoords(item->seq->clone());
	segment->PutVal(item->value);
	NewDrawer *segdrw = NewDrawer::getDrawer("LineFeatureDrawer", "ilwis38", &parms);
	segdrw->addDataSource(CFEATURE(segment));
	layerDrawer->addDrawer(segdrw);
	PreparationParameters fp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
	segdrw->prepare(&fp);
	mdoc->rootDrawer->getDrawerContext()->doDraw();
	updateUndoAdministration(item, segment, segdrw);

	delete item;

}

void LineSetEditor2::updateUndoAdministration(UndoItem *item, Segment *seg, NewDrawer *drw) {
	for(vector<UndoItem *>::iterator cur = undoItems.begin(); cur != undoItems.end(); ++cur) {
		if ( (*cur)->oldFeatureId == seg->getGuid())
			(*cur)->oldFeatureId = seg->getGuid();
		if ( (*cur)->oldDrawerId == drw->getId())
			(*cur)->oldDrawerId = drw->getId();
	}
}

void LineSetEditor2::storeUndoState(States state, bool created) {
	for(int i =0; i < selectedSegments.size(); ++i) {
		SelectedSegment *ss = selectedSegments.at(i);
		UndoItem *undoItem = new UndoItem();
		undoItem->seq = ss->seg->getCoordinates();
		undoItem->state = state;
		undoItem->value = ss->seg->rValue();
		undoItem->oldFeatureId = ss->seg->getGuid();
		undoItem->oldDrawerId = ss->drawer->getId();

		undoItems.push_back(undoItem);
		bmapptr->fChanged = true;
	}

}
// state machine for the editor
void LineSetEditor2::updateState() {
	// clikced somewhere on the map where no feature was present. state has insert so new feature created
	if ( pci.seg == 0 && hasState(msINSERT) && hasState(msLMOUSEUP)) {
		createFeature();
		storeUndoState(msINSERT, true);
		editorState |= msMOVEVERTICES;
		// clikced somewhere on the map where a feature was present and clicked on a vertex, additional vertex will be created. this
		// mode is used when extending the ends of the line
	} else if ( pci.crdIndex != iUNDEF && hasState(msINSERT) &&  hasState(msLMOUSEUP)) {
		//selectedSegments.empty();
		storeUndoState(msINSERT);
		insertVertex(hasState(msENTER));
		editorState |= msMOVEVERTICES;

		// cliked somewhere on the map where a feature was present and clicked in the middel of a line. A new vertex will be created at the click point
	} else if ( (pci.seg != 0 && pci.linePoint) &&  hasState( msINSERT ) && hasState( msLMOUSEUP)) {
		storeUndoState(msINSERT);
		insertVertex(hasState(msCTRL));

	} else if ( (pci.seg != 0 && pci.linePoint == false ) && hasState( msINSERT ) && hasState( msLMOUSEUP)) {
		storeUndoState(msINSERT);
		insertVertex(hasState(msCTRL));
		editorState |= msMOVEVERTICES;

		// start move state. pivot vertex is chosen we can start moving now
	} else if ( (pci.seg != 0 && pci.linePoint == false ) &&  hasState( msMOVE ) && hasState( msLMOUSEUP) && !hasState(msMOVEVERTICES)) {
		selectVertex(hasState(msCTRL));
		storeUndoState(msMOVE);
		editorState |= msMOVEVERTICES;
		// nothing selected (seg ==0), selectVertext will deselect all
	} else if ( (pci.seg == 0 ) && hasState( msSELECT ) && hasState( msLMOUSEUP)) {
		selectVertex( );

		// segment has been selected but no specific vertext, select whole segment
	} else if ( (pci.seg != 0 && pci.linePoint == false) && hasState( msSELECT ) && hasState( msLMOUSEUP)) {
		selectVertex(hasState(msCTRL));

	} else if ( pci.seg != 0 && hasState( msDELETE )) {
		storeUndoState(msDELETE);
		deleteVertices();
		editorState = editorState & ~ msDELETE;

	} else if ( (pci.seg != 0 && pci.linePoint) && hasState( msSELECT ) && hasState( msLMOUSEUP)) {
		selectVertex(hasState(msCTRL));

	}else if ( hasState( msMOVE ) && hasState( msENTER)) {
		pci = PointClickInfo(); // end mving op point(s)
		editorState = editorState & ~ ( msENTER | msMOVEVERTICES);

	} else if ( hasState( msINSERT ) && hasState( msENTER)) {
		fillPointClickInfo(crdMouse);
		insertVertex(true);
		//selectedSegments.empty();
		pci = PointClickInfo(); // end inserting op point(s)
		editorState = editorState & ~ ( msENTER | msMOVEVERTICES);

	}else if ( (pci.seg != 0 && pci.linePoint) && hasState( msSPLIT ) && hasState( msLMOUSEUP)) {
		split();
		selectVertex();

	}else if ( pci.seg != 0 && pci.linePoint == false && hasState( msMOVE ) && hasState( msMOUSEMOVE)) {
		updatePositions();

		// end movement of point
	} else if ( hasState( msMOVEVERTICES ) && hasState( msLMOUSEUP)  && ! hasState(msCTRL)) {
		editorState = editorState & ~msMOVEVERTICES;
		pci = PointClickInfo(); // end the move

	} else if ( pci.seg != 0 && hasState( msMOVEVERTICES ) && hasState( msMOUSEMOVE)) {
		updatePositions();

	} else if ( pci.seg == 0 && hasState( msMOVE) && hasState(msLMOUSEUP)) {
		pci = PointClickInfo();
		selectedSegments.empty();
	}
	mdoc->rootDrawer->getDrawerContext()->doDraw();
}

void LineSetEditor2::OnSelectAll()
{
	selectedSegments.clear();
	for(int i=0; i < layerDrawer->getDrawerCount(); ++i) {
		LineFeatureDrawer *fdr = dynamic_cast<LineFeatureDrawer *>(layerDrawer->getDrawer(i));
		if (!fdr)
			continue;
		ILWIS::Segment *s = CSEGMENT(fdr->getFeature());
		SelectedSegment *ss = new SelectedSegment();
		ss->drawer = fdr;
		ss->seg = s;
		selectedSegments.push_back(ss);
		fdr->setSpecialDrawingOptions(NewDrawer::sdoSELECTED,true);
	}
	mpv->Invalidate();
}

bool LineSetEditor2::fCopyOk()
{
	return selectedSegments.size() > 0;
}

bool LineSetEditor2::fPasteOk()
{
  return IsClipboardFormatAvailable(iFmtPnt) ? true : false;
}

void LineSetEditor2::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fCopyOk());
}

void LineSetEditor2::OnUpdatePaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fPasteOk());
}


void LineSetEditor2::OnCopy()
{
	if (!fCopyOk())
		return;
	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;
	EmptyClipboard();

	long iSize = 1;
	long iNr;
	CoordinateSequence *crdBuf;
	for (int i = 0; i < selectedSegments.size(); ++i) {
		ILWIS::Segment *seg = selectedSegments[i]->seg;
		crdBuf = seg->getCoordinates();
		iSize += crdBuf->size() + 1;
		delete crdBuf;
	}
	IlwisPoint* ip = new IlwisPoint[iSize];
	if (0 == ip)
		return;
	ip[0].c = Coord();
	ip[0].iRaw = iSize-1;
	long k = 1;
	Coord crd;
	const int iSIZE = 1000000;
	char* sBuf = new char[iSIZE];
	char* s = sBuf;
	String str, sVal;
	long iTotLen = 0;
	int iLen;

	for (int i = 0; i < selectedSegments.size(); ++i) {
		ILWIS::Segment *seg = selectedSegments[i]->seg;
		crdBuf = seg->getCoordinates();
		iNr = crdBuf->size();
		long iRaw = seg->iValue();
		for (int j = 0; j < iNr; ++j) {
			ip[k].c = crdBuf->getAt(j);
			ip[k].iRaw = iRaw;
			++k;
		}
		ip[k].c = Coord();
		ip[k].iRaw = iUNDEF;
		++k;

		if (iTotLen > iSIZE) 
			continue;
		for (int j = 0; j < iNr; ++j) {
			crd = crdBuf->getAt(j);
			if (0 == j)
				str = String("%g\t%g\t%S\r\n", crd.x, crd.y, seg->sValue(bmapptr->dvrs()));
			else
				str = String("%g\t%g\r\n", crd.x, crd.y);
			iLen = str.length();
			iTotLen += iLen;
			if (iTotLen > iSIZE) 
				continue;
			strcpy(s, str.sVal());
			s += iLen;
		} 
		delete crdBuf;
		iTotLen++;
		*s++ = '\r';
		*s++ = '\n';
	}

	iLen = (1+iSize) * sizeof(IlwisPoint);
	HANDLE hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	void* pv = GlobalLock(hnd);
	memcpy(pv, ip, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtPnt, hnd);

	// Ilwis Domain Format
	IlwisDomain* id = new IlwisDomain(bmapptr->dm(), bmapptr->vr());
	iLen = sizeof(IlwisDomain);
	hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	pv = GlobalLock(hnd);
	memcpy(pv, id, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtDom, hnd);
	delete id;


	*s = '\0';
	hnd = GlobalAlloc(GMEM_FIXED, strlen(sBuf)+2);
	char* pc = (char*)GlobalLock(hnd);
	strcpy(pc,sBuf);
	GlobalUnlock(hnd);
	SetClipboardData(CF_TEXT,hnd);
	delete [] ip;
	delete [] sBuf;

	CloseClipboard();
}

void LineSetEditor2::OnPaste()
{
	if (!fPasteOk()) return;
	CoordBuf crdBuf(10000);
	unsigned int iSize;

	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;

	bool fConvert = false, fValues = false, fSort = false;
	Domain dmMap, dmCb;
	ValueRange vrCb;
	if (IsClipboardFormatAvailable(iFmtDom)) {
		dmMap = bmapptr->dm();
		HANDLE hnd = GetClipboardData(iFmtDom);
		iSize = (unsigned int)GlobalSize(hnd);
		IlwisDomain id;
		if (sizeof(id) < iSize)
			iSize = sizeof(id);
		memcpy(&id, (char*)GlobalLock(hnd),iSize);
		GlobalUnlock(hnd);
		dmCb = id.dm();
		if (dmMap->pdv()) {
			if (0 == dmCb->pdv()) {
				mpv->MessageBox(TR("No numerical values in clipboard data").c_str(),TR("Segment Editor").c_str(),MB_OK|MB_ICONSTOP);
				CloseClipboard();
				return;
			}
			ValueRange vrMap = bmapptr->vr();
			vrCb = id.vr();
			fValues = true;
			if (vrMap != vrCb)
				fConvert = true;
		}
		else if (dmMap->pdc()) {
			if (0 == dmCb->pdc()) {
				mpv->MessageBox(TR("No Class values in clipboard data").c_str(),TR("Segment Editor").c_str(),MB_OK|MB_ICONSTOP);
				CloseClipboard();
				return;
			}
			fSort = true;
			if (dmMap != dmCb)
				fConvert = true;
		}
		else {
			if (dmMap != dmCb)
				fConvert = true;
			if (dmMap->pdsrt())
				fSort = true;
		}
	}

	HANDLE hnd = GetClipboardData(iFmtPnt);
	iSize = (unsigned int)GlobalSize(hnd);
	char* cp = new char[iSize];
	memcpy(cp, (char*)GlobalLock(hnd),iSize);
	IlwisPoint* ip = (IlwisPoint*) cp;
	iSize /= sizeof(IlwisPoint);
	iSize = ip[0].iRaw;
	unsigned int i;
	int j = 0;
	long iRaw = iUNDEF;
	double rVal = rUNDEF;
	for (i = 0; i < iSize; ++i) {
		if (ip[1+i].c.fUndef()) {
			if (j > 1) {
				ILWIS::Segment *seg = CSEGMENT(bmapptr->newFeature());
				seg->PutCoords(j,crdBuf);
				if (fValues)
					seg->PutVal(rVal);
				else
					seg->PutVal(iRaw);
				bmapptr->Updated();
			}  
			j = 0;
			continue;
		}  
		if (j == 0) {
			iRaw = ip[1+i].iRaw;
			if (fConvert) {
				if (fValues) {
					if (vrCb.fValid())
						rVal = vrCb->rValue(iRaw);
					else
						rVal = iRaw;
				}
				else {
					String sVal;
					if (vrCb.fValid())
						sVal = vrCb->sValueByRaw(dmCb,iRaw, 0);
					else
						sVal = dmCb->sValueByRaw(iRaw, 0);
					if (fSort) {
						if ("?" == sVal) {
							iRaw = iUNDEF;
							continue;
						}
						iRaw = dmMap->iRaw(sVal);
						if (iUNDEF == iRaw) {
							String sMsg(SEDMsgNotInDomain_SS.sVal(), sVal, dmMap->sName());
							int iRet = mpv->MessageBox(TR("Value not in domain").c_str(),TR("Segment Editor").c_str(),MB_OK|MB_ICONSTOP);
							if (IDYES == iRet)
								try {
									iRaw = dmMap->pdsrt()->iAdd(sVal);
							}
							catch (ErrorObject& err) {
								err.Show();
								iRaw = iUNDEF;
							}
							else if (IDCANCEL == iRet)
								break;
							else
								continue;
						}
					}
				}
			}
		}
		crdBuf[j++] = ip[1+i].c;
	}
	if (j > 1) {
		ILWIS::Segment *seg = CSEGMENT(bmapptr->newFeature());
		seg->PutCoords(j,crdBuf);
		if (fValues)
			seg->PutVal(rVal);
		else
			seg->PutVal(iRaw);
		bmapptr->Updated();
	}  
	CloseClipboard();
	mpv->Invalidate();
}

void LineSetEditor2::setActive(bool yesno) {
	BaseMapEditor::setActive(yesno);
	editModeItems->setActive(yesno);
	layerDrawer->setSpecialDrawingOptions(NewDrawer::sdoSELECTED | NewDrawer::sdoTOCHILDEREN,false);

	if ( bmapptr->fChanged)
		bmapptr->Store();
	mdoc->mpvGetView()->Invalidate();
}


//--------------------------------------
CheckSegmentsForm::CheckSegmentsForm(CWnd *par, LayerDrawer *sdr, LineSetEditor2 *edit) : DisplayOptionsForm(sdr,par,TR("Check Segments")){
	new PushButton(root, TR("Check Dead ends"), (NotifyProc)&CheckSegmentsForm::deadEnds);
	new PushButton(root, TR("Check Intersections"), 0);
	new PushButton(root, TR("Check Self overlap"), 0);
	new PushButton(root, TR("Check all"), 0);
	create();
}

struct Dangle{
	Segment *seg;
	NewDrawer *drw;
	vector<int> selCoords;
};

int CheckSegmentsForm::deadEnds(Event *ev) {
	//SpatialDataDrawer *abdrw = (SpatialDataDrawer *)(drw->getParentDrawer());
	//BaseMapPtr *bmp = abdrw->getBaseMap();
	//geos::operation::polygonize::Polygonizer polygonizer;
	//for (long i=0; i < bmp->iFeatures(); ++i) {
	//	const ILWIS::Segment *seg = CSEGMENT(bmp->getFeature(i));
	//	if ( !seg || !seg->fValid())
	//		continue;
	//	polygonizer.add((Geometry *)seg);
	//}
	//vector<const LineString *> *v = polygonizer.getDangles();
	//map<String, Dangle> foundSegs;
	//if ( v->size() > 0) {
	//	for(int i=0; i < v->size(); ++i) {
	//		const LineString *ls = v->at(i);
	//		for(int  j=0; j < ls->getNumPoints(); ++j) {
	//			Coord c = ls->getCoordinateN(j);
	//			vector<Geometry *> geoms = bmp->getFeatures(c);
	//			if ( geoms.size() == 0)
	//				continue;
	//			Segment *seg = CSEGMENT(geoms[0]);
	//			double d;
	//			int index = seg->nearSection(c, 0, d);
	//			map<String, Dangle>::iterator cur = foundSegs.find(seg->getGuid());
	//			if ( cur == foundSegs.end()) {
	//				vector<NewDrawer *> drawers;
	//				drw->getDrawerFor(seg, drawers);
	//				if ( drawers.size() == 0)
	//					continue;
	//				Dangle dangle;
	//				dangle.drw = drawers[0];
	//				dangle.seg = seg;
	//				foundSegs[seg->getGuid()] = dangle;
	//				foundSegs[seg->getGuid()].selCoords.push_back(index);
	//			} else {
	//				(*cur).second.selCoords.push_back(index);
	//			}

	//		}
	//	}
	//	for(map<String, Dangle>::iterator cur = foundSegs.begin(); cur != foundSegs.end(); ++cur) {
	//		int opt = (*cur).second.drw->getSpecialDrawingOption();
	//		(*cur).second.drw->setSpecialDrawingOptions(opt, true, (*cur).second.selCoords);
	//	}
	//}
	//updateMapView();
	return 1;

}

void CheckSegmentsForm::apply() {

}