#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Domain\dmsort.h"
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
#include "drawers\pointdrawer.h"
#include "drawers\PointFeatureDrawer.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "DrawersUI\LayerDrawerTool.h"
#include "FeatureSetEditor.h"
#include "PointSetEditor.h"
#include "Headers\constant.h"

BEGIN_MESSAGE_MAP(PointSetEditor, BaseMapEditor)
//	//{{AFX_MSG_MAP(PointEditor)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	//ON_COMMAND(ID_EDIT, OnEdit)
	//ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	//ON_COMMAND(ID_CLEAR, OnClear)
	//ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateCopy)
	//ON_COMMAND(ID_CUT, OnCut)
	//ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCopy)
	//ON_COMMAND(ID_SELECTMODE, OnSelectMode)
	//ON_UPDATE_COMMAND_UI(ID_SELECTMODE, OnUpdateMode)
	//ON_COMMAND(ID_MOVEMODE, OnMoveMode)
	//ON_UPDATE_COMMAND_UI(ID_MOVEMODE, OnUpdateMode)
	//ON_COMMAND(ID_INSERTMODE, OnInsertMode)
	//ON_UPDATE_COMMAND_UI(ID_INSERTMODE, OnUpdateMode)
	//ON_WM_INITMENUPOPUP()
//	ON_COMMAND(ID_FINDUNDEFS, OnFindUndefs)
//	ON_UPDATE_COMMAND_UI(ID_FINDUNDEFS, OnUpdateMode)
//	ON_COMMAND(ID_CONFIGURE, OnConfigure)
//	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
//	ON_COMMAND(ID_UNDOALL, OnUndoAllChanges)
//	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
//	ON_UPDATE_COMMAND_UI(ID_UNDOALL, OnUpdateFileSave)
//	ON_COMMAND(ID_ADDPOINT, OnAddPoint)
	//ON_COMMAND(ID_SEGSETBOUNDS, OnSetBoundaries)
	ON_COMMAND(ID_SELALL, OnSelectAll)
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


	//men.CreateMenu();
	//addmen(ID_CUT  );
	addmen(ID_COPY );
	addmen(ID_PASTE);
	addmen(ID_CLEAR);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_SELALL);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();

	DataWindow* dw = mdoc->mpvGetView()->dwParent();
	if (dw) {
		//dw->bbDataWindow.LoadButtons("pntedit.but");
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
	//addmen(ID_ADDPOINT);
	//addmen(ID_FINDUNDEFS);
	//men.AppendMenu(MF_SEPARATOR);
	//addmen(ID_CONFIGURE);
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

void PointSetEditor::setActive(bool yesno) {
	BaseMapEditor::setActive(yesno);
	if (!yesno) {
		BaseMapPtr *bmp = ((SpatialDataDrawer *)drawer->getParentDrawer())->getBaseMap();
		if ( bmp) {
			bmp->Store();
			mdoc->mpvGetView()->Invalidate();
		}
	}
}

void PointSetEditor::OnSelectAll()
{
	selectedFeatures.clear();
	for(int i=0; i < LayerDrawer->getDrawerCount(); ++i) {
		PointFeatureDrawer *fdr = dynamic_cast<PointFeatureDrawer *>(LayerDrawer->getDrawer(i));
		if (!fdr)
			continue;
		ILWIS::Point *pnt = CPOINT(fdr->getFeature());
		SelectedFeature *sf = new SelectedFeature();
		sf->drawers.push_back(fdr);
		sf->feature = pnt;
		selectedFeatures[pnt->getGuid()] = sf;
		fdr->setSpecialDrawingOptions(NewDrawer::sdoSELECTED,true);
	}
	mpv->Invalidate();
}

bool PointSetEditor::fPasteOk()
{
  return IsClipboardFormatAvailable(iFmtPnt) ? true : false;
}

void PointSetEditor::OnUpdatePaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fPasteOk());
}

void PointSetEditor::OnPaste()
{
	if (!fPasteOk()) return;
	unsigned int iSize;

	CWaitCursor curWait;
	if (!mpv->OpenClipboard())
		return;

	bool fConvert = false, fValues = false, fSort = false;
	Domain dmMap, dmCb;
	ValueRange vrCb;
	if (IsClipboardFormatAvailable(iFmtDom)) 
	{
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
				mpv->MessageBox(TR("No numerical values in clipboard data").c_str(),TR("Point Editor").c_str(),MB_OK|MB_ICONSTOP);
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
				mpv->MessageBox(TR("No Class values in clipboard data").c_str(),TR("Point Editor").c_str(),MB_OK|MB_ICONSTOP);
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


	for (unsigned int j = 0; j < iSize; ++j) {
		Coord crd = Coord(ip[1+j].x, ip[1+j].y, 0); // Paste sets z-coord to 0, just like when manually digitizing new points.
		bmapptr->fChanged = true;
		if (!crd.fUndef()) {
			ILWIS::Point *pnt = CPOINT(bmapptr->newFeature());
			pnt->setCoord(crd);
			long iRaw = ip[1+j].iRaw;
			if (fConvert) {
				if (fValues) {
					double rVal;
					if (vrCb.fValid())
						rVal = vrCb->rValue(iRaw);
					else
						rVal = iRaw;
					pnt->PutVal(rVal);
				}
				else {
					String sVal;
					if (vrCb.fValid())
						sVal = vrCb->sValueByRaw(dmCb,iRaw, 0);
					else
						sVal = dmCb->sValueByRaw(iRaw, 0);
					if (fSort) {
						if ("?" == sVal) {
							pnt->PutVal(iUNDEF);
							continue;
						}
						iRaw = dmMap->iRaw(sVal);
						if (iUNDEF == iRaw) {
							String sMsg(TR("Value %S not in domain %S").c_str(), sVal, dmMap->sName());
							int iRet = mpv->MessageBox(sMsg.sVal(),TR("Point Editor").c_str(),MB_YESNOCANCEL|MB_ICONASTERISK);
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
						pnt->PutVal(iRaw);
					}
					else
						if (dmMap->fValid(sVal))
							pnt->PutVal(bmapptr->dvrs(), sVal);
				}
			}
			else
				pnt->PutVal(iRaw);
		}
		mdoc->mpvGetView()->Invalidate();
	}
	CloseClipboard();
}

void PointSetEditor::OnCopy()
{
	if (!fCopyOk())
		return;
	CWaitCursor curWait;
	if (!mdoc->mpvGetView()->OpenClipboard())
		return;
	EmptyClipboard();

	const int iSIZE = 10000000;
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
	int iLen = 0;
	for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
		SelectedFeature *sf = (*cur).second;
		ILWIS::Point *pnt = CPOINT(sf->feature);
		Coord crd = *(pnt->getCoordinate());
		ip[index].x = crd.x;
		ip[index].y = crd.y;
		ip[index++].iRaw = pnt->iValue();
		String sVal = pnt->sValue(bmapptr->dvrs());

		str = String("%.3f\t%.3f\t%S\r\n", crd.x, crd.y, sVal);
		iLen += str.length();
		if (iLen > iSIZE) 
			continue;
		strcpy(s, str.sVal());
		s += str.size();
	}

	iLen = (iSize) * sizeof(IlwisPoint);
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

	// Text Format
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


bool PointSetEditor::fCopyOk()
{
	return selectedFeatures.size() > 0;
}


void PointSetEditor::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fCopyOk());
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
