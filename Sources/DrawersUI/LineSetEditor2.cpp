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
//-----------------------------------------------------
SelectedSegments::~SelectedSegments(){
	empty();
}

void SelectedSegments::empty() {
	for(int i=0; i <  size(); ++i) {
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
	if ( index == iUNDEF && !add) {
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
	ON_COMMAND(ID_SELECTMODE, OnSelectMode)
	ON_UPDATE_COMMAND_UI(ID_SELECTMODE, OnUpdateMode)
	ON_COMMAND(ID_MOVEMODE, OnMoveMode)
	ON_UPDATE_COMMAND_UI(ID_MOVEMODE, OnUpdateMode)
	ON_COMMAND(ID_INSERTMODE, OnInsertMode)
	ON_UPDATE_COMMAND_UI(ID_INSERTMODE, OnUpdateMode)
	ON_COMMAND(ID_SPLITMODE, OnSplitMode)
	ON_UPDATE_COMMAND_UI(ID_SPLITMODE, OnUpdateMode)
END_MESSAGE_MAP()

//
//

#define sMen(ID) ILWSF("men",ID).scVal()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 

DrawerTool *createLineSetEditor2(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new LineSetEditor2(zv, view, drw);
}

LineSetEditor2::LineSetEditor2(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
FeatureSetEditor2("LineSetEditor2",zv, view, drw),
curSegSplit("EditSplitCursor"),
curSegSplitting("EditSplittingCursor"),
rSnapDistance(0.01),
insertionPoint(iUNDEF)
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");
	editorState = msNONE;

}

LineSetEditor2::~LineSetEditor2(){
	while( !undoItems.empty()) {
		delete undoItems.top();
		undoItems.pop();
	}
}

String LineSetEditor2::getMenuString() const {
	if ( drawer->inEditMode()) {
		return TR("Close Segmentmap Editor");
	}
	else{
		return TR("Open Segmentmap Editor");
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


	CMenu men;
	CMenu menSub;
	men.CreateMenu();

	addmen(ID_UNDO);
	//addmen(ID_REDO);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_SELECTMODE);
	addmen(ID_MOVEMODE);
	addmen(ID_INSERTMODE);
	addmen(ID_SPLITMODE);
	//men.AppendMenu(MF_SEPARATOR);
	//addmen(ID_FINDUNDEFS);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();

	DataWindow* dw = mdoc->mpvGetView()->dwParent();
	if (dw) {
		dw->bbDataWindow.LoadButtons("segedit.but");
		dw->RecalcLayout();
		mdoc->mpvGetView()->mwParent()->UpdateMenu(hmenFile, hmenEdit);
		//mdoc->mpvGetView()->UpdateWindow();
	}
	return htiNode;
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
	men.AppendMenu(MF_SEPARATOR);
	switch (mode) {
		case BaseMapEditor::mSELECT: {
			addmen(ID_EDIT);
			addmen(ID_EDIT_COPY);
			BOOL fEdit = selectedFeatures.size() > 0;
			men.EnableMenuItem(ID_EDIT, fEdit ? MF_ENABLED : MF_GRAYED);
			addmen(ID_CLEAR);
			men.EnableMenuItem(ID_CLEAR, fEdit ? MF_ENABLED : MF_GRAYED);
			men.AppendMenu(MF_SEPARATOR);
									 }

									 break;
		case BaseMapEditor::mINSERT:
			addmen(ID_SETVAL);
			addmen(ID_EDIT_PASTE);
			men.AppendMenu(MF_SEPARATOR);
			break;
	}
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
		   fCheck = (BaseMapEditor::mSELECT & mode) != 0;
		   break;
	   case ID_MOVEMODE:
		   fCheck = (BaseMapEditor::mMOVE & mode) != 0 || (BaseMapEditor::mMOVING & mode) != 0;
		   break;
	   case ID_INSERTMODE:
		   fCheck = (BaseMapEditor::mINSERT & mode) != 0;
		   fEnable = (BaseMapEditor::mMOVING & mode) == 0;
		   break;
	   case ID_SPLITMODE:
		   fCheck = BaseMapEditor::mSPLIT == mode;
		   fEnable = (BaseMapEditor::mMOVING & mode) == 0;
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
		curActive = curEdit;
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
		double d1 = rDist(crd,seg->getCoordinateN(after - 1));
		double d2 = rDist(crd,seg->getCoordinateN(after));
		if ( d1 < d2) {
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
	CoordinateSequence *seq = boundaries[0];
	//for(int i =0; i < seq->size(); ++i) {
	//	Coord c = seq->getAt(i);
	//	if ( rDist(c,pci.crdClick) < snap) {
	//		break;
	//	}
	//}
	vector<Coordinate> *copyv = new vector<Coordinate>();
	int k = 0;
	bool added = false;
	while(k < seq->size()){
		if ( (k != pci.crdIndex + 1) || added) {
			copyv->push_back(seq->getAt(k++));
			if ( pci.crdIndex == seq->size() - 1 && k == pci.crdIndex) { // insert beyond the end
				long snapPoint = 0;
				if ( (snapPoint = noSnap(seq, snap)) == iUNDEF) {
					copyv->push_back(pci.crdClick);
				} else {
					copyv->push_back(copyv->at(0));
				}
				pci.crdIndex += 1;
			}
		}
		else {
			copyv->push_back(pci.crdClick);
			added = true;
		}
	}
	pci.seg->PutCoords(new CoordinateArraySequence(copyv));
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
	pci.drawer->prepare(&pp);
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

void LineSetEditor2::split(){

	CoordBounds cbZoom = mdoc->rootDrawer->getCoordBoundsZoom();
	double delta = max(cbZoom.height(), cbZoom.width()) / 50.0;
	double d;
	long after = pci.seg->nearSection(pci.crdClick, delta,d);
	if ( after == iUNDEF)
		return;
	bool isRing = pci.seg->isRing();
	vector<CoordinateSequence*> boundaries;
	pci.seg->getBoundaries(boundaries);
	CoordinateSequence *seq = boundaries[0];
	vector<Coordinate> *copyv1 = new vector<Coordinate>();
	vector<Coordinate> *copyv2 = new vector<Coordinate>();
	vector<Coordinate> *copyv = copyv1;
	NewDrawer *prepareDrawer = 0;
	int k = 0;
	while(k < seq->size()){
		if ( k != after) {
			copyv->push_back(seq->getAt(k++));
		}
		else {
			copyv->push_back(pci.crdClick);
			copyv = copyv2;
			copyv->push_back(pci.crdClick);
			after = iUNDEF;
		}
	}
	//addToSelectedFeatures(seg, crd,,);
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
	if ( isRing) {
		for(int j = 0; j < copyv1->size(); j++) {
			copyv2->push_back(copyv1->at(j));
		}
		copyv = copyv2;
		delete copyv1;
	} else {
		ILWIS::Segment *segNew = CSEGMENT(bmapptr->newFeature());
		CoordinateArraySequence *arseq2 = new CoordinateArraySequence(copyv2);
		segNew->PutCoords(arseq2);
		segNew->PutVal(pci.seg->rValue());
		prepareDrawer = drawer;
		copyv = copyv1;
	}
	CoordinateArraySequence *arseq = new CoordinateArraySequence(copyv);
	pci.seg->PutCoords(arseq);
	if (prepareDrawer == 0)
		prepareDrawer  = pci.drawer;

	prepareDrawer->prepare(&pp);

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
		bmapptr->removeFeature(ss->seg->getGuid(), ss->selectedCoords);
		bool removed = false;
		if ( ss->selectedCoords.size() == 0 || ss->selectedCoords.size() == ss->seg->getNumPoints()) {
			((ComplexDrawer *)drawer)->removeDrawer(ss->drawer->getId());
			removed = true;
		}
		if ( !removed) {
			PreparationParameters p(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
			ss->drawer->prepare(&p);
		}
	}
	pci = PointClickInfo();
	selectedSegments.empty();
}

void LineSetEditor2::OnUndo()
{
	if ( undoItems.size() == 0)
		return;

	UndoItem *item = undoItems.top();
	undoItems.pop();
	NewDrawer *segdrw = 0;

	if ( item->state == msMOVE) {
		item->seg->PutCoords(item->seq->clone());
		item->seg->PutVal(item->value);
		segdrw = item->drawer;
	} else if ( item->state == msDELETE) {
		Segment *segment;
		if ( item->seg == 0) { // recreate feature
			segment = CSEGMENT(bmapptr->newFeature());
			ILWIS::DrawerParameters parms(layerDrawer->getRootDrawer(), layerDrawer);
			segdrw = NewDrawer::getDrawer("LineFeatureDrawer", "ilwis38", &parms);
			segdrw->addDataSource(CFEATURE(segment));
		} else {
			segdrw = item->drawer;
			segment = item->seg;
		}
		segment->PutCoords(item->seq->clone());
		segment->PutVal(item->value);

	} else if ( item->state == msINSERT) {
		if ( item->created) {
			bmapptr->removeFeature(item->seg->getGuid());
			((ComplexDrawer *)drawer)->removeDrawer(item->drawer->getId());
		} else {
			item->seg->PutCoords(item->seq->clone());
			item->seg->PutVal(item->value);
			segdrw = item->drawer;
		}

	}
	delete item;

	PreparationParameters fp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
	if ( segdrw)
		segdrw->prepare(&fp);
	mdoc->rootDrawer->getDrawerContext()->doDraw();

}

void LineSetEditor2::storeUndoState(States state, bool created) {
	if ( state == msMOVE) {
		for(int i =0; i < selectedSegments.size(); ++i) {
			SelectedSegment *ss = selectedSegments.at(i);
			UndoItem *undoItem = new UndoItem();
			undoItem->seg = ss->seg;
			undoItem->seq = ss->seg->getCoordinates();
			undoItem->value = ss->seg->rValue();
			undoItem->drawer = ss->drawer;
			undoItem->state = state;
			undoItems.push(undoItem);

		}
	} else if ( state == msDELETE) {
		for(int i =0; i < selectedSegments.size(); ++i) {
			SelectedSegment *ss = selectedSegments.at(i);
			UndoItem *undoItem = new UndoItem();
			if ( ss->selectedCoords.size() == 0 || ss->selectedCoords.size() == ss->seg->getNumPoints()) {
				undoItem->seg = 0; // segment has will be removed so any reference to it is pointless
				undoItem->drawer = 0; // same for drawer
			} else {
				undoItem->seg = ss->seg;
				undoItem->drawer = ss->drawer;
			}
			undoItem->seq = ss->seg->getCoordinates();
			undoItem->state = state;
			undoItem->value = ss->seg->rValue();
			
			undoItems.push(undoItem);
		}
	} else if ( state == msINSERT) {
			UndoItem *undoItem = new UndoItem();
			undoItem->seg = pci.seg;
			undoItem->seq = pci.seg->getCoordinates();
			undoItem->value = pci.seg->rValue();
			undoItem->drawer = pci.drawer;
			undoItem->state = state;
			undoItem->created = created;
			undoItems.push(undoItem);
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
		storeUndoState(msINSERT);
		insertVertex(hasState(msCTRL));
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

	} else if ( (pci.seg != 0 && pci.linePoint == false) && hasState( msDELETE )) {
		storeUndoState(msDELETE);
		deleteVertices();
		editorState = editorState & ~ msDELETE;

	} else if ( (pci.seg != 0 && pci.linePoint) && hasState( msSELECT ) && hasState( msLMOUSEUP)) {
		selectVertex(hasState(msCTRL));

	}else if ( hasState( msMOVE ) && hasState( msENTER)) {
		pci = PointClickInfo(); // end mving op point(s)
		editorState = editorState & ~ ( msENTER | msMOVEVERTICES);

	} else if ( hasState( msINSERT ) && hasState( msENTER)) {
		pci = PointClickInfo(); // end inserting op point(s)
		editorState = editorState & ~ ( msENTER | msMOVEVERTICES);

	}else if ( (pci.seg != 0 && pci.linePoint) && hasState( msSPLIT ) && hasState( msLMOUSEUP)) {
		split();

	}else if ( pci.seg != 0 && pci.linePoint == false && hasState( msMOVE ) && hasState( msMOUSEMOVE)) {
		updatePositions();

	} else if ( hasState( msMOVEVERTICES ) && hasState( msLMOUSEUP)) {
		editorState = editorState & ~msMOVEVERTICES;
		pci = PointClickInfo(); // end the move

	} else if ( pci.seg != 0 && hasState( msMOVEVERTICES ) && hasState( msMOUSEMOVE)) {
		updatePositions();

	}
	mdoc->rootDrawer->getDrawerContext()->doDraw();
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