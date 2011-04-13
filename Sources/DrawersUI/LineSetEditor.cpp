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
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "FeatureSetEditor.h"
#include "DrawersUI\SetDrawerTool.h"
#include "LineSetEditor.h"
#include "Headers\constant.h"
#include "Headers\Hs\Editor.hs"
#include "Headers\Hs\Appforms.hs"
#include "Headers\Htp\Ilwis.htp"


BEGIN_MESSAGE_MAP(LineSetEditor, FeatureSetEditor)
	//{{AFX_MSG_MAP(LineSetEditor)
	// ON_COMMAND(ID_COPY, OnCopy)
	// ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
	//ON_COMMAND(ID_PASTE, OnPaste)
	// ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	//ON_COMMAND(ID_EDIT, OnEdit)
	// ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	//ON_COMMAND(ID_CLEAR, OnClear)
	// ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateClear)
	//ON_COMMAND(ID_CUT, OnCut)
	// ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCopy)
	ON_COMMAND(ID_SELECTMODE, OnSelectMode)
	ON_UPDATE_COMMAND_UI(ID_SELECTMODE, OnUpdateMode)
	ON_COMMAND(ID_MOVEMODE, OnMoveMode)
	ON_UPDATE_COMMAND_UI(ID_MOVEMODE, OnUpdateMode)
	ON_COMMAND(ID_INSERTMODE, OnInsertMode)
	ON_UPDATE_COMMAND_UI(ID_INSERTMODE, OnUpdateMode)
	ON_COMMAND(ID_SPLITMODE, OnSplitMode)
	ON_UPDATE_COMMAND_UI(ID_SPLITMODE, OnUpdateMode)
	ON_COMMAND(ID_SEGCHECKSELF, OnCheckSelf)
	ON_COMMAND(ID_SEGCHECKDEADENDS, OnCheckConnected)
	ON_COMMAND(ID_SEGCHECKINTERSECT, OnCheckIntersects)
	ON_COMMAND(ID_SEGCHECKCODECONS, OnCheckCodeConsistency)
	ON_COMMAND(ID_SEGCHECKCLOSEDSEGMENTS, OnCheckClosedSegments)
	ON_COMMAND(ID_SEGREMOVEREDUNDANT, OnRemoveRedundantNodes)
	/*ON_COMMAND(ID_SEGPOLYGONIZE, OnPolygonize)
	ON_COMMAND(ID_UNDOALL, OnUndoAllChanges)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_UNDOALL, OnUpdateFileSave)
	ON_COMMAND(ID_SEGPACK, OnSegPack)
	ON_UPDATE_COMMAND_UI(ID_SEGPACK, OnUpdateSegPack)
	ON_COMMAND(ID_SEGSETBOUNDS, OnSetBoundaries)
	ON_COMMAND(ID_SELALL, OnSelectAll)*/
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//

#define sMen(ID) ILWSF("men",ID).scVal()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 

DrawerTool *createLineSetEditor(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new LineSetEditor(zv, view, drw);
}

LineSetEditor::LineSetEditor(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
FeatureSetEditor("LineSetEditor",zv, view, drw),
curSegSplit("EditSplitCursor"),
curSegSplitting("EditSplittingCursor"),
rSnapDistance(0.01),
insertionPoint(iUNDEF)
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");
	//mergeModeItems = new SetChecks(tree,this,(DTSetCheckFunc)&LineSetEditor::setcheckMergeMode);



	//sVal = String("%06lx", (long)col);
	//GetPrivateProfileString("Segment Editor", "Normal Color", sVal.sVal(), sBuf, 79, fn.sVal());
	//sscanf(sBuf,"%lx",&col);
	//col = settings.clrValue("Normal Color", col);
	//sVal = String("%06lx", (long)colRetouch);
	//GetPrivateProfileString("Segment Editor", "Retouch Color", sVal.sVal(), sBuf, 79, fn.sVal());
	//sscanf(sBuf,"%lx",&colRetouch);
	//colRetouch = settings.clrValue("Retouch Color", colRetouch);
	//sVal = String("%06lx", (long)colDeleted);
	//GetPrivateProfileString("Segment Editor", "Deleted Color", sVal.sVal(), sBuf, 79, fn.sVal());
	//sscanf(sBuf,"%lx",&colDeleted);
	//colDeleted = settings.clrValue("Deleted Color", colDeleted);
	//colFindUndef = settings.clrValue("Find Undef Color", colFindUndef);

	//CMenu men;
	//men.CreateMenu();
	//addmen(ID_UNDOALL);
	//addmen(ID_FILE_SAVE);
	//CMenu menSub;
	//menSub.CreateMenu();
	//addSub(ID_SEGCHECKSELF);
	//addSub(ID_SEGCHECKDEADENDS);
	//addSub(ID_SEGCHECKINTERSECT);
	//menSub.AppendMenu(MF_SEPARATOR);
	//addSub(ID_SEGCHECKCODECONS);
	//menSub.AppendMenu(MF_SEPARATOR);
	//addSub(ID_SEGCHECKCLOSEDSEGMENTS);
	//men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_SEGCHECK)); 
	//menSub.Detach();
	//addmen(ID_SEGREMOVEREDUNDANT);
	//addmen(ID_SEGPACK);
	//addmen(ID_SEGPOLYGONIZE);
	//addmen(ID_SEGSETBOUNDS);
	//men.AppendMenu(MF_SEPARATOR);
	//addmen(ID_CONFIGURE);
	//menSub.CreateMenu();
	//addSub(ID_FILE_DIGREF);
	//addSub(ID_DIGACTIVE);
	//men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_DIG)); 
	//menSub.Detach();
	//addmen(ID_EXITEDITOR);
	//hmenFile = men.GetSafeHmenu();
	//men.Detach();

	//men.CreateMenu();
	//addmen(ID_CUT  );
	//addmen(ID_COPY );
	//addmen(ID_PASTE);
	//addmen(ID_CLEAR);
	//men.AppendMenu(MF_SEPARATOR);
	//addmen(ID_SELALL);
	//addmen(ID_EDIT);
	//addmen(ID_SETVAL);
	//men.AppendMenu(MF_SEPARATOR);
	//addmen(ID_SELECTMODE);
	//addmen(ID_MOVEMODE);
	//addmen(ID_INSERTMODE);
	//addmen(ID_SPLITMODE);
	//men.AppendMenu(MF_SEPARATOR);
	//addmen(ID_UNDELSEG);
	//addmen(ID_FINDUNDEFS);
	//hmenEdit = men.GetSafeHmenu();
	//men.Detach();
	////UpdateMenu();

	//DataWindow* dw = mdoc->mpvGetView()->dwParent();
	//if (dw) {
	//	dw->bbDataWindow.LoadButtons("segedit.but");
	//	dw->RecalcLayout();
	//}
	////htpTopic = htpLineSetEditor;
	//sHelpKeywords = "Segment editor";
}

LineSetEditor::~LineSetEditor(){
}

String LineSetEditor::getMenuString() const {
	if ( drawer->inEditMode()) {
		return TR("Close Segmentmap Editor");
	}
	else{
		return TR("Open Segmentmap Editor");
	}
}

bool LineSetEditor::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	if (!sdrwt)
		return false;
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(sdrwt->getDrawer());
	BaseMapPtr *bmp = ((AbstractMapDrawer *)sdrw->getParentDrawer())->getBaseMap();
	if ( !bmp || IOTYPE(bmp->fnObj) != IlwisObject::iotSEGMENTMAP)
		return false;

	if ( bmp->fReadOnly())
		return false;
	if ( drawer->inEditMode())
		active = true;
	parentTool = tool;
	return true;
}

HTREEITEM LineSetEditor::configure( HTREEITEM parentItem) {
	getSettings();
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	htiNode = insertItem(TR("Segmentmap Editor"),"EditMap", item);
	FeatureSetEditor::configure(htiNode);
	DisplayOptionRadioButtonItem *ritem = new DisplayOptionRadioButtonItem(TR("Split"), tree,htiNode,drawer);
	ritem->setState(false);
	ritem->setCheckAction(this,editModeItems,0);
	insertItem(TR("Split"),"Bitmap", ritem);

	/*ritem = new DisplayOptionRadioButtonItem(TR("Merge"), tree,htiNode,drawer);
	ritem->setState(false);
	ritem->setCheckAction(this,editModeItems,0);
	HTREEITEM htMerge = insertItem(TR("Merge"),"Bitmap", ritem);*/

	//DisplayOptionButtonItem *bitem = new DisplayOptionButtonItem(TR("Snap to"), tree,htMerge,drawer);
	//bitem->setState(false);
	//bitem->setCheckAction(this,mergeModeItems,0);
	//insertItem(TR("Snap to"),"Bitmap", bitem);

	//bitem = new DisplayOptionButtonItem(TR("Connect"), tree,htMerge,drawer);
	//bitem->setState(false);
	//bitem->setCheckAction(this,mergeModeItems,0);
	//insertItem(TR("Connect"),"Bitmap", bitem);



	CMenu men;
	CMenu menSub;
	men.CreateMenu();

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

void LineSetEditor::getSettings() {

	IlwisSettings settings("Map Window\\Segment Editor");
	String fn = IlwWinApp()->Context()->fnUserINI().sFullName();
	char sBuf[80];
	String sVal;

	sVal = "yes";
	GetPrivateProfileString("Segment Editor", "Auto Snap", sVal.sVal(), sBuf, 79, fn.sVal());
	fAutoSnap = strcmp(sBuf, "no") ? true : false;
	fAutoSnap = settings.fValue("Auto Snap", fAutoSnap);
	sVal = "yes";
	GetPrivateProfileString("Segment Editor", "Show Nodes", sVal.sVal(), sBuf, 79, fn.sVal());
	fShowNodes = strcmp(sBuf, "no") ? true : false;
	fShowNodes = settings.fValue("Show Nodes", fShowNodes);
}

bool LineSetEditor::OnContextMenu(CWnd* pWnd, CPoint point)
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

zIcon LineSetEditor::icon() const {
	return zIcon("SegIcon");
}

String LineSetEditor::sTitle() const
{
	String s(TR("LineEditor ") + bmapptr->sName());
	return s;
}


void LineSetEditor::OnCopy()
{
	if (!fCopyOk())
		return;
	CWaitCursor curWait;
	if (!mdoc->mpvGetView()->OpenClipboard())
		return;
	EmptyClipboard();

	const int iSIZE = 1000000;
	char* sBuf = new char[iSIZE];
	char* s = sBuf;
	String str, sVal;
	long iSize = 0;
	for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
		SelectedFeature *sf = (*cur).second;
		iSize += sf->selectedCoords.size();
	}

	IlwisPoint* ip = new IlwisPoint[iSize];
	int index = 0;
	for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
		SelectedFeature *sf = (*cur).second;
		int crdIndex = sf->selectedCoords[index];
		Coord crd = *(sf->coords[crdIndex]);
		ip[index].c = crd;
		ip[index++].iRaw = sf->feature->iValue();
	}

	long iLen = (iSize) * sizeof(IlwisPoint);
	HANDLE hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	void* pv = GlobalLock(hnd);
	memcpy(pv, ip, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtPnt, hnd);

	iLen = 0;
	for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
		SelectedFeature *sf = (*cur).second;
		sVal = sf->feature->sValue(bmapptr->dvrs());
		int crdIndex = sf->selectedCoords[index];
		Coord crd = *(sf->coords[crdIndex]);
		str = String("%.3f\t%.3f\t%S\r\n", crd.x, crd.y, sVal);
		iLen += str.length();
		if (iLen > iSIZE) 
			continue;
		strcpy(s, str.sVal());
		s += str.size();
	}

	// Ilwis Domain Format
	IlwisDomain* id = new IlwisDomain(bmapptr->dm(), bmapptr->vr());
	iLen = sizeof(IlwisDomain);
	hnd = GlobalAlloc(GMEM_MOVEABLE,iLen);
	pv = GlobalLock(hnd);
	memcpy(pv, id, iLen);
	GlobalUnlock(hnd);
	SetClipboardData(iFmtDom, hnd);
	delete id;

	// Text Format
	*s = '\0';
	hnd = GlobalAlloc(GMEM_FIXED, strlen(sBuf)+2);
	char* pc = (char*)GlobalLock(hnd);
	strcpy(pc,sBuf);
	GlobalUnlock(hnd);
	SetClipboardData(CF_TEXT,hnd);

	delete ip;
	delete sBuf;

	CloseClipboard();
}




bool LineSetEditor::fPasteOk()
{
	return IsClipboardFormatAvailable(iFmtPnt) ? true : false;
}

bool LineSetEditor::insertVertex(UINT nFlags, CPoint point) {
	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	vector<NewDrawer *> drawers;
		// find the lowest drawer that belongs to this feature
	setdrawer->getRootDrawer()->getDrawerFor(segment, drawers);


	vector<CoordinateSequence *> bound;
	segment->getBoundaries(bound);
	CoordBuf buf(bound[0]);
	delete bound[0];
	if ( insertionPoint == iUNDEF)
		insertionPoint = buf.size() - 1;
	buf[insertionPoint++] = crd;
	buf.add(crd);
	/*for(int i=0; i < buf.size(); ++i) {
		TRACE(String("%d %f %f\n",i, buf[i].x, buf[i].y).scVal());
	}*/
	segment->PutCoords(buf.iSize(), buf);
	PreparationParameters fp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER, bmapptr->cs());
	if ( drawers.size() > 0) {
		drawers[0]->prepare(&fp);
	}
	//select(nFlags, point);

	SFMIter cur = selectedFeatures.find(currentGuid);
	if ( cur != selectedFeatures.end()) {
		delete (*cur).second;
		selectedFeatures.erase(currentGuid);
		vector<NewDrawer *> drws;
		drws.push_back(drawers[0]);
		addToSelectedFeatures(segment,crd,drws, buf.iSize() - 1);

	}

	drawer->getRootDrawer()->getDrawerContext()->doDraw();

	return true;
}

long LineSetEditor::findInsertSegment(const Coord& crd, Segment** seg) {
	CoordBounds cbZoom = mdoc->rootDrawer->getCoordBoundsZoom();
	double delta = max(cbZoom.height(), cbZoom.width()) / 50.0;
	vector<Geometry *> geoms = bmapptr->getFeatures(crd, delta);
	for(int i = 0; i < geoms.size(); ++i) {
		*seg = CSEGMENT(geoms.at(i));
		double d;
		long after = (*seg)->nearSection(crd, delta,d);
		if ( after == iUNDEF)
			continue;


		return after;
	}
	*seg = 0;
	return iUNDEF;
}

Coord LineSetEditor::insertPoint(const Coord& crd, Segment* seg, long after) {
	vector<CoordinateSequence*> boundaries;
	seg->getBoundaries(boundaries);
	CoordBounds cb = drawer->getRootDrawer()->getCoordBoundsZoom();
	double snap = rSnapDistance * min(cb.width(), cb.height());
	CoordinateSequence *seq = boundaries[0];
	for(int i =0; i < seq->size(); ++i) {
		Coord c = seq->getAt(i);
		if ( rDist(c,crd) < snap) {
			return c;
		}
	}
	vector<Coordinate> *copyv = new vector<Coordinate>();
	int k = 0;
	while(k < seq->size()){
		if ( k != after) {
			copyv->push_back(seq->getAt(k++));
		}
		else {
			copyv->push_back(crd);
			after = iUNDEF;
		}
	}
	vector<NewDrawer *> drawers;
	drawer->getRootDrawer()->getDrawerFor(seg, drawers);
	seg->PutCoords(new CoordinateArraySequence(copyv));
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
	drawers[0]->prepare(&pp);
	mdoc->mpvGetView()->Invalidate();
	return Coord();
}


// inserts 2 points for a segment. The second one will/can be moved byt the mouse though they begin at the same location
bool LineSetEditor::insertFeature(UINT nFlags, CPoint point) {
	clear();
	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	insertionPoint = findInsertSegment(crd, &segment);
	if ( segment == 0) {
		segment = CSEGMENT(bmapptr->newFeature());
	} else {
		crd = insertPoint(crd, segment, insertionPoint);
	}
	if ( crd.fUndef())
		return true;

	if ( !( insertionPoint == segment->iBegin() || insertionPoint == segment->iEnd())) {
		CoordBuf buf(2);
		buf[0] = crd;
		buf[1] = crd;
		segment->PutCoords(buf.iSize(), buf);
		segment->PutVal(0L);
	} else {
		CoordinateSequence *seq = segment->getCoordinates();
		seq->add(crd);
		segment->PutCoords(seq);
		insertionPoint++;
	}
	mdoc->pixInfoDoc->setEditFeature(CFEATURE(segment));

	CoordWithCoordSystem cwcs(crd, bmapptr->cs());
	if ( setdrawer) {
		ILWIS::DrawerParameters parms(setdrawer->getRootDrawer(), setdrawer);
		ILWIS::NewDrawer *sdrw = NewDrawer::getDrawer("LineFeatureDrawer", "ilwis38", &parms);
		sdrw->addDataSource(CFEATURE(segment));
		PreparationParameters fp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
		sdrw->prepare(&fp);
		sdrw->setSpecialDrawingOptions(NewDrawer::sdoSELECTED,true);
		if ( segment->iValue() == 0)
			setdrawer->addDrawer(sdrw);

		vector<NewDrawer *> drawers;
		drawers.push_back(sdrw);
		currentGuid = segment->getGuid();
		addToSelectedFeatures(segment,crd,drawers,segment->iValue() != 0 ? insertionPoint : 1);
	}


	mode |= mMOVING;
	IlwWinApp()->SendUpdateCoordMessages(cmINSERT, &cwcs);
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
	return true;
}

void LineSetEditor::updateFeature(SelectedFeature *f) {
	ILWIS::Segment *seg = CSEGMENT(f->feature);
	CoordinateSequence *seq = seg->getCoordinates();
	for(int i = 0; i < f->selectedCoords.size(); ++i) {
		int index = f->selectedCoords[i];
		Coordinate c = *(f->coords[index]);
		seq->setAt(c,index); 

	}
	seg->PutCoords(seq);
}

void LineSetEditor::OnUpdateMode(CCmdUI* pCmdUI)
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

void LineSetEditor::setMode(BaseMapEditor::Mode m) 
{
	mode = m;
	int selectedIndex = editModeItems->getState();
	switch (mode) {
	case BaseMapEditor::mSPLIT:
		curActive = curSegSplit;
		if ( selectedIndex != 3) {
			editModeItems->checkItem(3);
		}
		break;
	case BaseMapEditor::mMERGE:
		curActive = curEdit;
		if ( selectedIndex != 4) {
			editModeItems->checkItem(4);
		}
		break;
	default:
		FeatureSetEditor::setMode(m);
	}
	OnSetCursor();
}

class AskStartSegForm: public FormWithDest
{
public:
  AskStartSegForm(CWnd* wnd, const String& sRemark,
                  String* sMsk, long iMax, long* iStartSeg, long htp)
  : FormWithDest(wnd, SEDMsgCheckSegments),
    iStart(iStartSeg), sMask(sMsk)
  {
    StaticText* st = new StaticText(root, sRemark);
    st->SetIndependentPos();
    fMask = sMask->length() > 0;
    CheckBox* cbMask = new CheckBox(root, SAFUiMask, &fMask);
    new FieldString(cbMask, "", sMask);
    iOption = 0;
    if (*iStartSeg < 0)
      *iStartSeg = 0;
//    else if (*iStartSeg > 1)
//      iOption = 1;
    RadioGroup* rg = new RadioGroup(root, "", &iOption);
    rg->Align(cbMask, AL_UNDER);
    new RadioButton(rg, SEDUiStartBegin);
    RadioButton* rb = new RadioButton(rg, SEDUiStartSegNum);
    RangeInt ri(1, iMax);
    new FieldInt(rb, "", iStart, ri, true);
    SetMenHelpTopic(htp);
    create();
  }
private:
  int exec() {
    FormWithDest::exec();
    if (!fMask)
      *sMask = "";
    if (0 == iOption)
      *iStart = 0;
    else
      *iStart -= 1;
    return 0;
  }
  long* iStart;
  String* sMask;
  bool fMask;
  int iOption;
};

void LineSetEditor::OnCheckSelf()
{
  long iStart = 0;
  AskStartSegForm frm(mdoc->mpvGetView(), SEDMsgSelfCheck, &sMask, bmapptr->iFeatures()-1, &iStart,
                      htpSegEditCheckSelf);
  if (frm.fOkClicked()) {
    Tranquilizer trq(SEDMsgCheckSegments);
		if (fCheckSelf(trq, iStart))
	    mdoc->mpvGetView()->MessageBox(SEDMsgOkSelfCheck.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void LineSetEditor::OnCheckConnected()
{
  long iStart = 0;
  AskStartSegForm frm(mdoc->mpvGetView(), SEDMsgConnectCheck, &sMask, bmapptr->iFeatures()-1, &iStart,
                      htpSegEditCheckConnected);
  if (frm.fOkClicked()) {
  //  YieldActive ya;
    Tranquilizer trq(SEDMsgCheckSegments);
    if (fCheckConnected(trq, iStart))
	    mdoc->mpvGetView()->MessageBox(SEDMsgOkCheckConnected.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void LineSetEditor::OnCheckIntersects()
{
  long iStart = 0;
  AskStartSegForm frm(mdoc->mpvGetView(), SEDMsgIntersectCheck, &sMask, bmapptr->iFeatures()-1, &iStart,
                      htpSegEditCheckIntersects);
  if (frm.fOkClicked()) {
  //  YieldActive ya;
    Tranquilizer trq(SEDMsgCheckSegments);
    if (fCheckIntersects(trq, iStart))
	    mdoc->mpvGetView()->MessageBox(SEDMsgOkIntersects.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void LineSetEditor::OnCheckCodeConsistency()
{
  long iStart = 0;
  AskStartSegForm frm(mdoc->mpvGetView(), SEDMsgCheckCodeConsistency, &sMask, bmapptr->iFeatures()-1, &iStart,
                      htpSegEditCheckCodeConsistency);
  if (frm.fOkClicked()) {
    Tranquilizer trq(SEDMsgCheckSegments);
    if (fCheckCodeConsistency(trq, iStart))
	    mdoc->mpvGetView()->MessageBox(SEDMsgOkCodeConsistency.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void LineSetEditor::OnCheckClosedSegments()
{
  long iStart = 0;
  AskStartSegForm frm(mdoc->mpvGetView(), SEDMsgCheckClosedSegments, &sMask, bmapptr->iFeatures()-1, &iStart,
                      htpSegEditCheckClosedSegments);
  if (frm.fOkClicked()) {
    Tranquilizer trq(SEDMsgCheckSegments);
    if (fCheckClosedSegments(trq, iStart))
	    mdoc->mpvGetView()->MessageBox(SEDMsgOkClosedSegments.scVal(),SEDErrSegEditor.sVal(),MB_OK|MB_ICONINFORMATION);
  }
}

void LineSetEditor::OnRemoveRedundantNodes()
{
  int iRet = mdoc->mpvGetView()->MessageBox(SEDMsgRemoveRedundantNodes.sVal(),
        SEDMsgSegEditor.sVal(), MB_ICONQUESTION|MB_OKCANCEL);
  if (IDOK == iRet) {
   /* for (SLIter<ILWIS::Segment *> iter(&segList); iter.fValid(); iter.first())
      iter.remove();*/
		CWaitCursor curWait;
    Tranquilizer trq(SEDMsgSegEditor);
    ((SegmentMapPtr *)bmapptr)->RemoveRedundantNodes(trq);
    mdoc->mpvGetView()->Invalidate();
  }
}

bool LineSetEditor::fCheckClosedSegments(Tranquilizer& trq, long iStartSeg)
{
	SetActNode(Coord());
	trq.SetText(SEDMsgCheckClosedSegments);
	Mask mask(bmapptr->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, bmapptr->iFeatures()))
		return false;
	for (int j = iStartSeg; j < bmapptr->iFeatures(); ++j) {
		ILWIS::Segment *seg = (ILWIS::Segment *)bmapptr->getFeature(j);
		if ( !(seg && seg->fValid()))
			continue;
		if (trq.fUpdate(j, bmapptr->iFeatures()))
			return false;
		if (fMask && !seg->fInMask(bmapptr->dvrs(), mask))
			continue;
		Coord crdBegin = seg->crdBegin();
		Coord crdEnd = seg->crdEnd();
		if (crdBegin != crdEnd) {
			trq.Stop();
			SetActNode(crdEnd);
			MessageBeep(MB_ICONEXCLAMATION);
			String sMsg(SEDMsgSegNotClosed_S.scVal(), String("%S (nr %i)", seg->sValue(bmapptr->dvrs()), j));
			String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
			int iRet = mdoc->mpvGetView()->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
			if (IDYES == iRet)
				ZoomInOnError();
			return false;
		}
	}
	SetActNode(Coord());
	trq.Stop();
	return true;
}

bool LineSetEditor::fCheckCodeConsistency(Tranquilizer& trq, long iStartSeg)
{
	SetActNode(Coord());
	trq.SetText(SEDMsgCheckCodeConsistency);
	Mask mask(bmapptr->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, bmapptr->iFeatures()))
		return false;
	for (int j = iStartSeg; j < bmapptr->iFeatures(); ++j) {
		ILWIS::Segment *seg = (ILWIS::Segment *)bmapptr->getFeature(j);
		if ( !(seg && seg->fValid()))
			continue;
		if (trq.fUpdate(j, bmapptr->iFeatures()))
			return false;
		if (fMask && !seg->fInMask(bmapptr->dvrs(), mask))
			continue;
		Coord crdBegin = seg->crdBegin();
		Coord crdEnd = seg->crdEnd();
		String sVal = seg->sValue(bmapptr->dvrs());
		ILWIS::Segment *s;
		for (int i = 0; i < bmapptr->iFeatures(); ++i) {
			s = (ILWIS::Segment *)bmapptr->getFeature(i);
			if ( !(s && s->fValid()))
				continue;
			if (trq.fAborted())
				return false;
			if (i == j)
				continue;
			if (fMask && !s->fInMask(bmapptr->dvrs(), mask))
				continue;
			bool fBegin = s->crdBegin() == crdBegin || s->crdEnd() == crdBegin;
			bool fEnd = s->crdBegin() == crdEnd || s->crdEnd() == crdEnd;
			if (fBegin || fEnd) {
				if (s->sValue(bmapptr->dvrs()) != sVal) {
					trq.Stop();
					if (fBegin)
						SetActNode(crdBegin);
					else
						SetActNode(crdEnd);
					MessageBeep(MB_ICONEXCLAMATION);
					String sMsg(SEDMsgDifferentCodes_SS.sVal(), sVal, s->sValue(bmapptr->dvrs()));
					String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
					int iRet = mdoc->mpvGetView()->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
					if (IDYES == iRet)
						ZoomInOnError();
					setMode(BaseMapEditor::mSELECT);
					if (fBegin)
						SetActNode(crdBegin);
					else
						SetActNode(crdEnd);
					return false;
				}
			}
		}
	}
	SetActNode(Coord());
	trq.Stop();
	return true;
}

bool LineSetEditor::fCheckIntersects(Tranquilizer& trq, long iStartSeg) // returns true if ok
{
	SetActNode(Coord());
	trq.SetText(SEDMsgIntersectCheck);
	Mask mask(bmapptr->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, bmapptr->iFeatures()))
		return false;

	bool fCheckNeeded = true;
	while (fCheckNeeded) {
		fCheckNeeded = false;
		int iSeg = bmapptr->iFeatures();
		//  ArrayLarge<CoordBounds> acb(iSeg,1);
		ArrayLarge<CoordBounds> acb(iSeg+1); // zero based
		ILWIS::Segment *s;
		for (int j = 0; j < bmapptr->iFeatures(); ++j) {
			s = (ILWIS::Segment *)bmapptr->getFeature(j);
			if ( !( s && s->fValid()))
				continue;
			if (fMask && !s->fInMask(bmapptr->dvrs(), mask))
				continue;
			int i = j;
			acb[i] = s->cbBounds();
		}

		for (int j = iStartSeg; j < bmapptr->iFeatures(); ++j) {
			ILWIS::Segment *seg = (ILWIS::Segment *)bmapptr->getFeature(j);
			if ( !(seg && seg->fValid()))
				continue;
			if (trq.fUpdate(j, bmapptr->iFeatures()))
				return false;
			if (fMask && !seg->fInMask(bmapptr->dvrs(), mask))
				continue;
			int iCurr = j;
			CoordBounds cbCurr = acb[iCurr];
			for (int i = 0; i <= iSeg; ++i) {
				if (trq.fAborted())
					return false;
				if (i == iCurr)
					continue;
				if (!cbCurr.fContains(acb[i]))
					continue;
				bool fOverlay;
				long iAft, iAft2;
				Coord crdAt;
				ILWIS::Segment *s2 = (ILWIS::Segment *)bmapptr->getFeature(i);
				if (seg->fIntersects(fOverlay, iAft, crdAt, s2, iAft2, trq)) {
					trq.Stop();
					SetActNode(crdAt);
					MessageBeep(MB_ICONEXCLAMATION);
					String sVal = String("%S (nr %i)", seg->sValue(bmapptr->dvrs()), i);
					if (fOverlay) {
						String sMsg(SEDMsgSegOverlayingSeg_S.sVal(), sVal);
						String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
						int iRet = mdoc->mpvGetView()->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
						if (IDYES == iRet)
							ZoomInOnError();
						setMode(BaseMapEditor::mMOVE);
						SetActNode(crdAt);
						return false;
					}
					else {
						int iRet = mdoc->mpvGetView()->MessageBox(SEDMsgZoomInOnError.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
						if (IDYES == iRet)
							ZoomInOnError();
						String sMsg(SEDMsgSegIntersectWithoutNodeSplitSeg_S.sVal(), sVal);
						iRet = mdoc->mpvGetView()->MessageBox(sMsg.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
						if (IDYES == iRet) {
							seg->segSplit(iAft, crdAt);
							s2->segSplit(iAft2, crdAt);
							SetActNode(Coord());
							trq.Start();
							fCheckNeeded = true;
						}
						else {
							setMode(BaseMapEditor::mMOVE);
							SetActNode(crdAt);
							return false;
						}
					}
				}
			}
		}
	}
	if (trq.fUpdate(bmapptr->iFeatures(), bmapptr->iFeatures()))
		return false;
	SetActNode(Coord());
	trq.Stop();
	return true;
}

bool LineSetEditor::fCheckConnected(Tranquilizer& trq, long iStartSeg) // returns true if ok
{
	SetActNode(Coord());
	trq.SetText(SEDMsgConnectCheck);
	Mask mask(bmapptr->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, bmapptr->iFeatures()))
		return false;

	int iSeg = bmapptr->iFeatures();
	//  ArrayLarge<Coord> crdF(iSeg,1), crdL(iSeg,1);
	ArrayLarge<Coord> crdF(iSeg+1), crdL(iSeg+1); // zero based
	ILWIS::Segment *s;
	for (int j = 0; j < bmapptr->iFeatures(); ++j) {
		s = (ILWIS::Segment *)bmapptr->getFeature(j);
		if ( !(s && s->fValid()))
			continue;
		if (fMask && !s->fInMask(bmapptr->dvrs(), mask))
			continue;
		int i = j;
		crdF[i] = s->crdBegin();
		crdL[i] = s->crdEnd();
	}

	for (int j = iStartSeg; j < bmapptr->iFeatures(); ++j) {
		ILWIS::Segment *seg = (ILWIS::Segment *)bmapptr->getFeature(j);
		if ( !(seg && seg->fValid()))
			continue;
		if (trq.fUpdate(j, bmapptr->iFeatures()))
			return false;
		if (fMask && !seg->fInMask(bmapptr->dvrs(), mask))
			continue;
		bool fBegin = false;
		bool fEnd = false;
		Coord crdBegin = seg->crdBegin();
		Coord crdEnd = seg->crdEnd();
		double rAzimBegin = seg->rAzim(false);
		double rAzimEnd = seg->rAzim(true);
		if (crdBegin == crdEnd) {
			fBegin = fEnd = true;
			if (abs(rAzimBegin - rAzimEnd) < 1e-6) {
				trq.Stop();
				SetActNode(crdBegin);
				MessageBeep(MB_ICONEXCLAMATION);
				String sVal = String("%S (nr %i)", seg->sValue(bmapptr->dvrs()), j);
				String sMsg(SEDMsgSegSelfOverlay_S.sVal(), sVal);
				String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
				int iRet = mdoc->mpvGetView()->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
				if (IDYES == iRet)
					ZoomInOnError();
				setMode(BaseMapEditor::mMOVE);
				SetActNode(crdBegin);
				return false;
			}
		}
		for (int i = 0; i < iSeg; ++i) {
			if (trq.fAborted())
				return false;
			if (i == j)
				continue;
			bool fBeginError = false;
			bool fEndError = false;
			if (crdF[i] == crdBegin) {
				fBegin = true;
				s = (ILWIS::Segment *)bmapptr->getFeature(i);
				if (abs(s->rAzim(false) - rAzimBegin) < 1e-6) {
					fBeginError = true;
					SetActNode(crdBegin);
				}
			}
			if (crdL[i] == crdBegin) {
				fBegin = true;
				s = (ILWIS::Segment *)bmapptr->getFeature(i);
				if (abs(s->rAzim(true) - rAzimBegin) < 1e-6) {
					fBeginError = true;
					SetActNode(crdBegin);
				}
			}
			if (!fBeginError && !fEndError && crdBegin == crdEnd)
				continue;
			if (crdF[i] == crdEnd) {
				fEnd = true;
				s = (ILWIS::Segment *)bmapptr->getFeature(i);
				if (abs(s->rAzim(false) - rAzimEnd) < 1e-6) {
					fEndError = true;
					SetActNode(crdEnd);
				}
			}
			if (crdL[i] == crdEnd) {
				fEnd = true;
				s = (ILWIS::Segment *)bmapptr->getFeature(i);
				if (abs(s->rAzim(true) - rAzimEnd) < 1e-6) {
					fEndError = true;
					SetActNode(crdEnd);
				}
			}
			if (fBeginError || fEndError) {
				trq.Stop();
				MessageBeep(MB_ICONEXCLAMATION);
				String sVal = String("%S (nr %i)", seg->sValue(bmapptr->dvrs()), i);
				String sMsg(SEDMsgSegOverlayAtNode_S.sVal(), sVal);
				String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
				int iRet = mdoc->mpvGetView()->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
				if (IDYES == iRet)
					ZoomInOnError();
				setMode(BaseMapEditor::mMOVE);
				if (fBeginError)
					SetActNode(crdBegin);
				else
					SetActNode(crdEnd);
				return false;
			}
		}
		if (!fBegin || !fEnd) {
			if (!fBegin)
				SetActNode(crdBegin);
			else 
				SetActNode(crdEnd);
			trq.Stop();
			MessageBeep(MB_ICONEXCLAMATION);
			String sVal = String("%S (nr %i)", seg->sValue(bmapptr->dvrs()), j);
			String sMsg(SEDMsgSegDeadEnd_S.sVal(), sVal);
			String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
			int iRet = mdoc->mpvGetView()->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
			if (IDYES == iRet)
				ZoomInOnError();
			setMode(BaseMapEditor::mMOVE);
			if (!fBegin)
				SetActNode(crdBegin);
			else
				SetActNode(crdEnd);
			return false;
		}
	}
	if (trq.fUpdate(bmapptr->iFeatures(), bmapptr->iFeatures()))
		return false;
	SetActNode(Coord());
	trq.Stop();
	return true;
}

bool LineSetEditor::fCheckSelf(Tranquilizer& trq, long iStartSeg) // returns true if ok
{
	SetActNode(Coord());
	trq.SetText(SEDMsgSelfCheck);
	Mask mask(bmapptr->dm(), sMask);
	bool fMask = sMask.length() > 0;
	if (trq.fUpdate(iStartSeg, bmapptr->iFeatures()))
		return false;
	for (int j = iStartSeg; j < bmapptr->iFeatures(); ++j) {
		ILWIS::Segment *seg = (ILWIS::Segment *)bmapptr->getFeature(j);
		if ( !(seg && seg->fValid()))
			continue;
		if (trq.fUpdate(j-1, bmapptr->iFeatures()))
			return false;
		if (fMask && !seg->fInMask(bmapptr->dvrs(), mask))
			continue;
		long iFirst, iSecond;
		Coord crdAt;
		while (!seg->fSelfCheck(iFirst, iSecond, crdAt)) {
			trq.Stop();
			SetActNode(crdAt);
			MessageBeep(MB_ICONEXCLAMATION);
			String sVal = String("%S (nr %i)", seg->sValue(bmapptr->dvrs()), j);
			if (-1 == iFirst) { // self overlay
				String sMsg(SEDMsgSegSelfOverlay_S.sVal(), sVal);
				String str("%S\n%S", sMsg, SEDMsgZoomInOnError);
				int iRet = mdoc->mpvGetView()->MessageBox(str.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
				if (IDYES == iRet)
					ZoomInOnError();
				setMode(BaseMapEditor::mMOVE);
				SetActNode(crdAt);
				return false;
			}
			else { // self cross
				int iRet = mdoc->mpvGetView()->MessageBox(SEDMsgZoomInOnError.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNO);
				if (IDYES == iRet)
					ZoomInOnError();
				String sMsg(SEDMsgSegSelfCrossRemFalsePol_S.sVal(), sVal);
				iRet = mdoc->mpvGetView()->MessageBox(sMsg.sVal(), SEDMsgCheckSegments.sVal(), MB_ICONEXCLAMATION|MB_YESNOCANCEL);
				if (IDYES == iRet) {
					seg->segSplit(iSecond, crdAt);
					ILWIS::Segment *s = CSEGMENT(bmapptr->newFeature());
					seg->segSplit(iFirst, crdAt,&s);
					if (s->rLength() > seg->rLength()) {
						ILWIS::Segment *sTmp = s;
						s = seg;
						seg = sTmp;
					}
					//SetDirty(s);
					s->Delete();
					trq.Start();
					continue;
				}
				else if (IDNO == iRet) {
					if (0 == AskSplit(seg, iSecond, crdAt)) {
						seg->segSplit(iFirst, crdAt);
						trq.Start();
						continue;
					}
				}
				setMode(BaseMapEditor::mMOVE);
				SetActNode(crdAt);
				return false;
			}
		}
	}
	if (trq.fUpdate(bmapptr->iFeatures(), bmapptr->iFeatures()))
		return false;
	SetActNode(Coord());
	trq.Stop();
	return true;
}

void LineSetEditor::SetActNode(Coord crd) {

}

void LineSetEditor::ZoomInOnError()
{
	CoordBounds cb = bmapptr->cb();
	double rDist = (cb.width() + cb.height())/20;
	Coord crdActNode;
	mdoc->mpvGetView()->ZoomInOn(crdActNode, rDist);
}

int LineSetEditor::AskSplit(ILWIS::Segment *seg, long iAfter, Coord crdAt)
{
  int iRet = 1;
  SetActNode(crdAt);

  String sVal = seg->sValue(bmapptr->dvrs());
  String sMsg(SEDMsgSplitSeg_S.sVal(), sVal);
  int iMsg = mdoc->mpvGetView()->MessageBox(sMsg.sVal(), SEDMsgSegEditor.sVal(), MB_ICONQUESTION|MB_YESNO);
  if (IDYES == iMsg) {
	  ILWIS::Segment *s = CSEGMENT(bmapptr->newFeature());
    seg->segSplit(iAfter, crdAt, &s);
    iRet = 0;
  }

  SetActNode(Coord());
  return iRet;
}

void LineSetEditor::OnLButtonDown(UINT nFlags, CPoint point){

	FeatureSetEditor::OnLButtonDown(nFlags, point);
	if ( mode & mSPLIT) {
		Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
		CoordBounds cbZoom = mdoc->rootDrawer->getCoordBoundsZoom();
		double delta = max(cbZoom.height(), cbZoom.width()) / 50.0;
		vector<Geometry *> geoms = bmapptr->getFeatures(crd, delta);
		for(int i = 0; i < geoms.size(); ++i) {
			Segment *seg = CSEGMENT(geoms.at(i));
			double d;
			long after = seg->nearSection(crd, delta,d);
			if ( after == iUNDEF)
				continue;
			bool isRing = seg->isRing();
			vector<CoordinateSequence*> boundaries;
			seg->getBoundaries(boundaries);
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
					copyv->push_back(crd);
					copyv = copyv2;
					copyv->push_back(crd);
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
				segNew->PutVal(seg->rValue());
				prepareDrawer = drawer;
				copyv = copyv1;
			}
			vector<NewDrawer *> drawers;
			drawer->getRootDrawer()->getDrawerFor(seg, drawers);
			if ( drawers.size() > 0) {		
				CoordinateArraySequence *arseq = new CoordinateArraySequence(copyv);
				seg->PutCoords(arseq);
				if (prepareDrawer == 0)
					prepareDrawer  = drawers[0];

			}
			prepareDrawer->prepare(&pp);
			select(nFlags, point);
			mdoc->mpvGetView()->Invalidate();
			break;
		}
	}

	return ;

}

void LineSetEditor::OnLButtonUp(UINT nFlags, CPoint point){

	FeatureSetEditor::OnLButtonUp(nFlags,point);

	// segment moving mode only ends when it is not in insert mode. else for every new leg a new feature 
	// would be added.
	if ((mode & mINSERT) == 0){
		mode = mode & (~mMOVING);
	} else if ( (mode & mINSERT) == 0) {
	}
			
	return ;
}
//
//void LineSetEditor::setcheckMergeMode(void *value) {
//	if ( value == 0)
//		return;
//	HTREEITEM hItem = *((HTREEITEM *)value);
//
//	DisplayOptionButtonItem *item = dynamic_cast<DisplayOptionButtonItem * >((LayerTreeItem *)(tree->GetTreeCtrl().GetItemData(hItem)));
//	int choice = item->getChecks()->getState();
//	if ( choice == 0) {
//		//OnSelectMode();
//	} else if ( choice == 1) {
//		//OnInsertMode();
//	} 
//	//mdoc->mpvGetView()->iActiveTool = getId();
//	//setActive(true);
//}

