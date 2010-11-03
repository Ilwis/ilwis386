#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNT.H"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h"
#include "Client\Mapwindow\IlwisClipboardFormat.h"
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\ilwis.h"
#include "Client\Editors\Map\FeatureSetEditor.h"
#include "MapEditors\PointSetEditor.h"
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
#define sMen(ID) ILWSF("men",ID).scVal()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID));

ILWIS::FeatureSetEditor *createPointSetEditor(MapCompositionDoc *doc, const BaseMap& bm) {
	return new PointSetEditor(doc, (PointMap&)bm);
}

PointSetEditor::PointSetEditor(MapCompositionDoc *doc, const PointMap& bm) : 
FeatureSetEditor(doc, bm),
curEdit("EditCursor"), 
curPntEdit("EditPntCursor"),
curPntMove("EditPntMoveCursor"), 
curPntMoving("EditPntMovingCursor") 
{
	iFmtPnt = RegisterClipboardFormat("IlwisPoints");
	iFmtDom = RegisterClipboardFormat("IlwisDomain");

	CMenu men;
	CMenu menSub;
	men.CreateMenu();


	addmen(ID_UNDOALL);
	addmen(ID_FILE_SAVE);
	addmen(ID_SEGSETBOUNDS);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_CONFIGURE);
	menSub.CreateMenu();
	addSub(ID_FILE_DIGREF);
	addSub(ID_DIGACTIVE);
	men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_DIG)); 
	menSub.Detach();
	addmen(ID_EXITEDITOR);
	hmenFile = men.GetSafeHmenu();
	men.Detach();

	men.CreateMenu();
	addmen(ID_CUT  );
	addmen(ID_COPY );
	addmen(ID_PASTE);
	addmen(ID_CLEAR);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_SELALL);
	addmen(ID_EDIT);
	addmen(ID_ADDPOINT);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_SELECTMODE);
	addmen(ID_MOVEMODE);
	addmen(ID_INSERTMODE);
	men.AppendMenu(MF_SEPARATOR);
	addmen(ID_FINDUNDEFS);
	hmenEdit = men.GetSafeHmenu();
	men.Detach();

	DataWindow* dw = mdoc->mpvGetView()->dwParent();
	if (dw) {
		dw->bbDataWindow.LoadButtons("pntedit.but");
		dw->RecalcLayout();
		mdoc->mpvGetView()->mwParent()->UpdateMenu(hmenFile, hmenEdit);
	}
}

PointSetEditor::~PointSetEditor(){
}

bool PointSetEditor::OnContextMenu(CWnd* pWnd, CPoint point)
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
	return true;
}

zIcon PointSetEditor::icon() const {
	return zIcon("PntIcon");
}

String PointSetEditor::sTitle() const
{
	String s(TR("PointEditor ") + bmapptr->sName());
	return s;
}

void PointSetEditor::OnInsertMode()
{
	Mode(mode == BaseMapEditor::mINSERT ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mINSERT);
}

void PointSetEditor::OnMoveMode()
{
	Mode(mode == BaseMapEditor::mMOVE ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mMOVE);
}

void PointSetEditor::OnSelectMode()
{
	Mode(mode == BaseMapEditor::mSELECT ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mSELECT);
}

void PointSetEditor::OnUpdateMode(CCmdUI* pCmdUI)
{
	BOOL fCheck;
	switch (pCmdUI->m_nID) {
		case ID_SELECTMODE:
			fCheck = BaseMapEditor::mSELECT == mode;
			break;
		case ID_MOVEMODE:
			fCheck = BaseMapEditor::mMOVE == mode;
			break;
		case ID_INSERTMODE:
			fCheck = BaseMapEditor::mINSERT == mode;
			break;
		//case ID_FINDUNDEFS:
		//	pCmdUI->SetCheck(fFindUndefs);
			return;
	}
	//if (0 != mpv->as)
	//	fCheck = false;
	pCmdUI->SetRadio(fCheck);
}

void PointSetEditor::Mode(BaseMapEditor::Mode m) 
{
	if (mode != mSELECT)
		clear();
	mdoc->mpvGetView()->OnNoTool();
	switch (m) {
	case mSELECT:
		curActive = curEdit;
		break;
	case mMOVE:
		curActive = curPntMove;
		break;
	case mMOVING:
		curActive = curPntMoving;
		break;
	case mINSERT:
		curActive = curPntEdit;
		break;
	}
	mode = m;
	OnSetCursor();
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