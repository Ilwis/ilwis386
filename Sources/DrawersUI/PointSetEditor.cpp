#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNT.H"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Client\Mapwindow\PixelInfoBar.h"
#include "Client\Mapwindow\PixelInfoView.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "DrawersUI\LayerDrawerTool.h"
#include "FeatureSetEditor.h"
#include "PointSetEditor.h"
#include "Headers\constant.h"

BEGIN_MESSAGE_MAP(PointSetEditor, BaseMapEditor)
//	//{{AFX_MSG_MAP(PointEditor)
//	ON_COMMAND(ID_COPY, OnCopy)
//	ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
//	ON_COMMAND(ID_PASTE, OnPaste)
//	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
//	ON_COMMAND(ID_EDIT, OnEdit)
//	ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
//	ON_COMMAND(ID_CLEAR, OnClear)
//	ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateCopy)
//	ON_COMMAND(ID_CUT, OnCut)
//	ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCopy)
	ON_COMMAND(ID_SELECTMODE, OnSelectMode)
	ON_UPDATE_COMMAND_UI(ID_SELECTMODE, OnUpdateMode)
	ON_COMMAND(ID_MOVEMODE, OnMoveMode)
	ON_UPDATE_COMMAND_UI(ID_MOVEMODE, OnUpdateMode)
	ON_COMMAND(ID_INSERTMODE, OnInsertMode)
	ON_UPDATE_COMMAND_UI(ID_INSERTMODE, OnUpdateMode)
	//ON_WM_INITMENUPOPUP()
//	ON_COMMAND(ID_FINDUNDEFS, OnFindUndefs)
//	ON_UPDATE_COMMAND_UI(ID_FINDUNDEFS, OnUpdateMode)
//	ON_COMMAND(ID_CONFIGURE, OnConfigure)
//	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
//	ON_COMMAND(ID_UNDOALL, OnUndoAllChanges)
//	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
//	ON_UPDATE_COMMAND_UI(ID_UNDOALL, OnUpdateFileSave)
//	ON_COMMAND(ID_ADDPOINT, OnAddPoint)
//	ON_COMMAND(ID_SEGSETBOUNDS, OnSetBoundaries)
//	ON_COMMAND(ID_SELALL, OnSelectAll)
//	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//
//
#define sMen(ID) ILWSF("men",ID).c_str()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID));

DrawerTool *createPointSetEditor(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new PointSetEditor(zv, view, drw);
}

PointSetEditor::PointSetEditor(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
FeatureSetEditor("PointSetEditor", zv, view, drw),crdFrm(0)
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");
}

PointSetEditor::~PointSetEditor(){
}

bool PointSetEditor::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	LayerDrawerTool *sdrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if (!sdrwt)
		return false;
	ILWIS::LayerDrawer *sdrw = dynamic_cast<ILWIS::LayerDrawer *>(sdrwt->getDrawer());
	if ( !sdrw)
		return false;
	BaseMapPtr *bmp = ((SpatialDataDrawer *)sdrw->getParentDrawer())->getBaseMap();
	if ( !bmp || IOTYPE(bmp->fnObj) != IlwisObject::iotPOINTMAP)
		return false;

	if ( bmp->fReadOnly())
		return false;
	if ( drawer->inEditMode())
		active = true;
	parentTool = tool;
	return true;
}

String PointSetEditor::getMenuString() const {
	if ( drawer->inEditMode()) {
		return TR("Close Pointmap Editor");
	}
	else{
		return TR("Open Pointmap Editor");
	}
}

HTREEITEM PointSetEditor::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	htiNode = insertItem("Pointmap Editor","EditMap", item);
	FeatureSetEditor::configure(htiNode);

	CMenu men;
	CMenu menSub;
	men.CreateMenu();

	//addmen(ID_UNDOALL);
	//addmen(ID_FILE_SAVE);
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
	//addmen(ID_ADDPOINT);
	//men.AppendMenu(MF_SEPARATOR);
	addmen(ID_SELECTMODE);
	addmen(ID_MOVEMODE);
	addmen(ID_INSERTMODE);
	//men.AppendMenu(MF_SEPARATOR);
	//addmen(ID_FINDUNDEFS);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();

	DataWindow* dw = mdoc->mpvGetView()->dwParent();
	if (dw) {
		dw->bbDataWindow.LoadButtons("pntedit.but");
		dw->RecalcLayout();
		mdoc->mpvGetView()->mwParent()->UpdateMenu(hmenFile, hmenEdit);
		//mdoc->mpvGetView()->UpdateWindow();
	}
	return htiNode;
}

void PointSetEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu men;
	men.CreatePopupMenu();
	addmen(ID_NORMAL);
	addmen(ID_ZOOMIN);
	addmen(ID_ZOOMOUT);
	addmen(ID_PANAREA);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_EDIT);
	addmen(ID_CLEAR);
	//men.EnableMenuItem(ID_EDIT, fEditOk() ? MF_ENABLED : MF_GRAYED);
	men.EnableMenuItem(ID_CLEAR, fCopyOk() ? MF_ENABLED : MF_GRAYED);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_ADDPOINT);
	addmen(ID_FINDUNDEFS);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_CONFIGURE);
	addmen(ID_EXITEDITOR);
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

zIcon PointSetEditor::icon() const {
	return zIcon("PntIcon");
}

String PointSetEditor::sTitle() const
{
	String s(TR("PointEditor ") + bmapptr->sName());
	return s;
}

//void PointSetEditor::OnInsertMode()
//{
//	Mode(mode == BaseMapEditor::mINSERT ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mINSERT);
//}
//
//void PointSetEditor::OnMoveMode()
//{
//	Mode(mode == BaseMapEditor::mMOVE ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mMOVE);
//}
//
//void PointSetEditor::OnSelectMode()
//{
//	Mode(mode == BaseMapEditor::mSELECT ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mSELECT);
//}



void PointSetEditor::OnUpdateMode(CCmdUI* pCmdUI)
{
	BOOL fCheck;

	pCmdUI->SetRadio(FALSE);
	switch (pCmdUI->m_nID) {
		case ID_SELECTMODE:
			fCheck = (BaseMapEditor::mSELECT & mode) != 0;
			break;
		case ID_MOVEMODE:
			fCheck = (BaseMapEditor::mMOVE & mode) != 0;
			break;
		case ID_INSERTMODE:
			fCheck = (BaseMapEditor::mINSERT & mode) != 0;
			break;
		return;
	}
	pCmdUI->SetRadio(fCheck);
}

void PointSetEditor::OnCopy()
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




bool PointSetEditor::fPasteOk()
{
  return IsClipboardFormatAvailable(iFmtPnt) ? true : false;
}

bool PointSetEditor::insertFeature(UINT nFlags, CPoint point) {
	clear();
	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	ILWIS::Point * p = CPOINT(bmapptr->newFeature());
	p->setCoord(crd);
	p->PutVal(0L);
	mdoc->pixInfoDoc->setEditFeature(CFEATURE(p));

	CoordWithCoordSystem cwcs(crd, bmapptr->cs());
	if ( LayerDrawer) {
		ILWIS::DrawerParameters parms(LayerDrawer->getRootDrawer(), LayerDrawer);
		ILWIS::NewDrawer *drawer = NewDrawer::getDrawer("PointFeatureDrawer", "ilwis38", &parms);
		drawer->addDataSource(CFEATURE(p));
		PreparationParameters fp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER, 0);
		drawer->prepare(&fp);
		drawer->setSpecialDrawingOptions(NewDrawer::sdoSELECTED,true);
		LayerDrawer->addDrawer(drawer);
	}
	select(nFlags, point);
	IlwWinApp()->SendUpdateCoordMessages(cmINSERT, &cwcs);
	mdoc->mpvGetView()->Invalidate();

	return true;
}

void PointSetEditor::updateFeature(SelectedFeature *f) {
	for(int i = 0; i < f->selectedCoords.size(); ++i) {
		int index = f->selectedCoords[i];
		ILWIS::Point *p = CPOINT(f->feature);
		Coord c = *(f->coords[index]);
		p->setCoord(c);
	}
}

//void PointSetEditor::removeSelectedFeatures() {
//	for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
//		SelectedFeature *sfeature = (*cur).second;
//		bmapptr->removeFeature(sfeature->feature->getGuid(), sfeature->selectedCoords);
//		((ComplexDrawer *)drawer)->removeDrawer(sfeature->drawers[0]->getId());
//
//	}
//	selectedFeatures.clear();
//	currentCoordIndex = iUNDEF;
//	currentGuid = sUNDEF;
//}

void PointSetEditor::OnLButtonDblClk(UINT nFlags, CPoint point){
	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	vector<Geometry *> geometries = bmapptr->getFeatures(crd);
	for(int i = 0; i < geometries.size(); ++i) {
		Feature *f = CFEATURE(geometries[i]);
		if ( f) {
			for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
				Feature *fselected = (*cur).second->feature;
				if ( fselected->getGuid() == f->getGuid()) {
					if (!crdFrm) {
						ComplexDrawer *cdrw = (ComplexDrawer *)getDrawer();
						crdFrm = new CoordForm(tree, cdrw,fselected);
					}
					else {
						crdFrm->setFeature(fselected);
						crdFrm->ShowWindow(SW_SHOW);
					}
				}
			}
		}
	}
}
//-------------------------------------
CoordForm::CoordForm(CWnd *wPar, ComplexDrawer *dr, Feature *feature) : 
DisplayOptionsForm(dr,wPar,TR("Coordinate(s)"),fbsApplyButton | fbsBUTTONSUNDER | fbsOKHASCLOSETEXT | fbsSHOWALWAYS|fbsHIDEONCLOSE){
	pnt = CPOINT(feature);
	crd = *pnt->getCoordinate();
	fc = new FieldCoord(root,TR("Coordinate"),&crd);
	create();
}

void CoordForm::apply() {
	fc->StoreData();
	pnt->setCoord(crd);
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER, ((LayerDrawer *)drw)->getCoordSystem());
	drw->prepare(&pp);

	this->ShowWindow(SW_HIDE);

	updateMapView();
}

void CoordForm::setFeature(Feature *f){
	pnt = CPOINT(f);
	crd = *pnt->getCoordinate();
	fc->SetVal(crd);


}
