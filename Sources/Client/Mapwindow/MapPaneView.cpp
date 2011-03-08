/***************************************************************
ILWIS integrates image, vector and thematic data in one unique 
and powerful package on the desktop. ILWIS delivers a wide 
range of feautures including import/export, digitizing, editing, 
analysis and display of data as well as production of 
quality mapsinformation about the sensor mounting platform

Exclusive rights of use by 52°North Initiative for Geospatial 
Open Source Software GmbH 2007, Germany

Copyright (C) 2007 by 52°North Initiative for Geospatial
Open Source Software GmbH

Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
tel +31-534874371

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (see gnu-gpl v2.txt); if not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA or visit the web page of the Free
Software Foundation, http://www.fsf.org.

Created on: 2007-02-8
***************************************************************/
// MapPaneView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Editors\Map\BaseMapEditor.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include <afxole.h>
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Positioner.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Headers\constant.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "winuser.h"
#include "Headers\Hs\Mapwind.hs"
#include "Client\Mapwindow\InfoLine.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Digitizer\DigiEditor.h"
#include "Client\FormElements\flddat.h"
#include "Engine\Map\Point\PNT.H"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Georef\GeoRefEditorTableView.h"
#include "Client\Editors\Georef\GeoRefEditorTableBar.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\Editors\Georef\TransformationComboBox.h"
#include "Client\Editors\Georef\TiePointEditor.h"
#include "Client\Editors\Georef\GeoRefEditor.h"
#include "Client\Editors\CoordSystem\CoordSysEditor.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Table\Rec.h"
#include "Client\TableWindow\RecordView.h"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldgrf.h"
#include "Client\FormElements\fldantxt.h"
#include "Client\FormElements\fldmsmpl.h"
#include "Client\Mapwindow\MapCompositionSrvItem.h"
#include "Client\FormElements\fldmap.h"
#include "Engine\SampleSet\SAMPLSET.H"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\Mapwindow\MapStatusBar.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\messages.h"
#include "Engine\SpatialReference\Cslatlon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MapPaneView

IMPLEMENT_DYNCREATE(MapPaneView, SimpleMapPaneView)

BEGIN_MESSAGE_MAP(MapPaneView, SimpleMapPaneView)
	// implemented in ZoomableView
	ON_COMMAND(ID_ZOOMIN, OnZoomIn)
	ON_COMMAND(ID_ZOOMOUT, OnZoomOut)
	ON_COMMAND(ID_PANAREA, OnPanArea)
	ON_COMMAND(ID_SELECTAREA, OnSelectArea)
	ON_UPDATE_COMMAND_UI(ID_ZOOMIN, OnUpdateZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOMOUT, OnUpdateZoomOut)
	ON_UPDATE_COMMAND_UI(ID_SELECTAREA, OnUpdateZoomIn)
	ON_UPDATE_COMMAND_UI(ID_PANAREA, OnUpdatePanArea)
	// own implementations
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_PIXELEDIT, OnPixelEdit)
	ON_COMMAND(ID_POINTEDIT, OnPointEdit)
	ON_COMMAND(ID_SEGEDIT, OnSegmentEdit)
	ON_COMMAND(ID_POLEDIT, OnPolygonEdit)
	ON_COMMAND(ID_NONEEDIT, OnNoneEdit)
	ON_COMMAND(ID_EDITGRF, OnGeoRefEdit)
	ON_COMMAND(ID_EDITCSY, OnCoordSysEdit)
	ON_UPDATE_COMMAND_UI(ID_EDITGRF, OnUpdateGeoRefEdit)
	ON_UPDATE_COMMAND_UI(ID_EDITCSY, OnUpdateCoordSysEdit)
	ON_COMMAND(ID_ADJUSTSIZE, OnAdjustSize)
	ON_UPDATE_COMMAND_UI(ID_ADJUSTSIZE, OnUpdateAdjustSize)
	ON_COMMAND(ID_ENTIREMAP, OnEntireMap)
	ON_UPDATE_COMMAND_UI(ID_ENTIREMAP, OnUpdateEntireMap)
	ON_COMMAND(ID_DEFAULTSCALE, OnDefaultScale)
	ON_COMMAND(ID_SCALE1, OnScaleOneToOne)
	ON_UPDATE_COMMAND_UI(ID_SCALE1, OnUpdateScaleOneToOne)
	ON_MESSAGE(ILW_UPDATE, OnUpdate)
	ON_WM_MOUSEACTIVATE()
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_CREATEPNTMAP, OnCreatePntMap)
	ON_COMMAND(ID_CREATESEGMAP, OnCreateSegMap)
	ON_COMMAND(ID_CREATERASMAP, OnCreateRasMap)
	ON_COMMAND(ID_CREATEGRF, OnCreateGeoRef)
	ON_COMMAND(ID_CREATECSY, OnCreateCoordSys)
	ON_COMMAND(ID_CREATESMS, OnCreateSampleSet)
	ON_COMMAND(ID_CREATESUBMAP, OnCreateSubMap)
	ON_MESSAGE(ILWM_VIEWSETTINGS, OnViewSettings)
	ON_COMMAND_RANGE(ID_MAPDBLCLKRECORD, ID_MAPDBLCLKACTION, OnSetDoubleClickAction)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MAPDBLCLKRECORD, ID_MAPDBLCLKACTION, OnUpdateDoubleClickAction)
	ON_COMMAND(ID_SHOWRECORDVIEW, OnShowRecordView)
	ON_UPDATE_COMMAND_UI(ID_SHOWRECORDVIEW, OnUpdateShowRecordView)
END_MESSAGE_MAP()

const int iMINSIZE = 50;

MapPaneView::MapPaneView()
: recBar(0), mwPar(0)
{
	dca = dcaRECORD;
	odt = new COleDropTarget;
	pib = 0;
}

MapPaneView::~MapPaneView()
{
	delete odt;
	delete recBar;
}

/////////////////////////////////////////////////////////////////////////////
// MapPaneView drawing

void MapPaneView::OnInitialUpdate() 
{
	odt->Register(this);
	//fStarting = true; // prevent too early start (moved to constructor)
	SimpleMapPaneView::OnInitialUpdate();

	if (0 == mwPar) {
		CFrameWnd* fw = GetTopLevelFrame();
		mwPar = dynamic_cast<MapWindow*>(fw);
	}

	MapCompositionDoc* mcd = GetDocument();
	createPixInfoBar();
	double rSc = mcd->rPrefScale();
	if (rSc > 0)
		PostMessage(WM_COMMAND, ID_DEFAULTSCALE, 0);
	else
		PostMessage(WM_COMMAND, ID_ENTIREMAP, 0);
}

MapWindow* MapPaneView::mwParent()
{
	if (0 == mwPar) {
		CFrameWnd* fw = GetTopLevelFrame();
		mwPar = dynamic_cast<MapWindow*>(fw);
	}
	return mwPar;
}

void MapPaneView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class

}

void MapPaneView::OnMouseMove(UINT nFlags, CPoint point) {
	SimpleMapPaneView::OnMouseMove(nFlags, point);
}

void MapPaneView::OnScaleOneToOne()
{
	_rScale = 1;
	CalcFalseOffsets();
	setScrollBars();
	SetDirty();
}

void MapPaneView::OnUpdateScaleOneToOne(CCmdUI* pCmdUI)
{
	// only allow when RasterMap or GeoRef specified
	// with vector data, an automatic georef gives weird results
}

void MapPaneView::GetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = 50;
	lpMMI->ptMinTrackSize.y = 50;
	MinMax mm = mmBounds();
	lpMMI->ptMaxTrackSize.x = scale(mm.width(), true);
	lpMMI->ptMaxTrackSize.y = scale(mm.height(), true);
}

void MapPaneView::OnDefaultScale()
{
	SetDirty();
}

void MapPaneView::OnEntireMap()
{

	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		mcd->rootDrawer->setCoordBoundsView(mcd->rootDrawer->getCoordinateSystem(), mcd->rootDrawer->getMapCoordBounds(),true);
		setScrollBars();
		fStarting = false;
		mcd->mpvGetView()->Invalidate();
	}
}

void MapPaneView::OnUpdateEntireMap(CCmdUI* pCmdUI)
{
	MapCompositionDoc* mcd = GetDocument();
	pCmdUI->Enable(mcd->rootDrawer->getDrawerCount() > 0);
}

void MapPaneView::ZoomInOn(Coord crd, double rDist)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	OnEntireMap();
	//if (crd.fUndef()) 
	//	return;
	//Coord crd1 = crd;
	//crd1.x += rDist;
	//crd1.y += rDist;
	//Coord crd2 = crd;
	//crd2.x -= rDist;
	//crd2.y -= rDist;
	//MinMax mm(rcConv(crd1), rcConv(crd2));
	//mm.MinRow() -= 1;
	//mm.MinCol() -= 1;
	//mm.MaxRow() += 1;
	//mm.MaxCol() += 1;
	//zRect rct = rctPos(mm);
	//AreaSelected(rct);	
}

void MapPaneView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch (lHint) 
	{
	case 0:
		SetDirty();
		//	Invalidate();
		break;
	case 1: {
		MinMax mm = mmBounds();
		iXsize = mm.width();
		iYsize = mm.height();
		setScrollBars();
		SetDirty();
		CFrameWnd* pFrame = GetParentFrame();
		ASSERT_VALID(pFrame);
		CRect rect;
		pFrame->GetWindowRect(&rect);
		pFrame->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),
			SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
			} break;
	case 2: 
		UpdateFrame();
		SetDirty();
		break;
	case 3: {
		MinMax mm = mmBounds();
		iXsize = mm.width();
		iYsize = mm.height();
		OnEntireMap();
			}	break;
	case 4: // maplistdrawer replaces map
		if (!fRedrawing) 
			SetDirtySilent();
		break;
	case 5: // message from SetDirty() - do nothing
		break;
	case 6: // message from SampleSetEditor - do nothing
		break;
	}
}


#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
void MapPaneView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (tools.size() == 0)
		return;
	if (edit && edit->OnContextMenu(pWnd, point))
		return;
	CMenu men, menSub;
	men.CreatePopupMenu();
	add(ID_NORMAL);
	add(ID_ZOOMIN);
	add(ID_ZOOMOUT);
	add(ID_PANAREA);
	add(ID_ENTIREMAP);
	men.AppendMenu(MF_SEPARATOR);
	menSub.CreateMenu();
	GetDocument()->menLayers(menSub, ID_LAYFIRST);
	men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_LAYEROPTIONS)); 
	menSub.Detach();    

	// grey out options if not available without coding anything anew
	CCmdUI cmdUIstate;
	cmdUIstate.m_pMenu = &men;
	cmdUIstate.m_pSubMenu = 0;
	cmdUIstate.m_nIndexMax = 4;
	// loop from ZoomIn till PanArea
	for (cmdUIstate.m_nIndex = 1; cmdUIstate.m_nIndex < cmdUIstate.m_nIndexMax;
		cmdUIstate.m_nIndex++)
	{
		cmdUIstate.m_nID = men.GetMenuItemID(cmdUIstate.m_nIndex);
		cmdUIstate.DoUpdate(this, 0);
	}

	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void MapPaneView::ShowRecord(const Ilwis::Record& rec)
{
	if (rec.fValid()) {
		CFrameWnd* fw = GetParentFrame();
		if (0 == recBar) {
			recBar = new RecordBar();
			recBar->Create(fw, ID_RECORDBAR);
			recBar->SetWindowText(SMWTitleAttributeRecord.scVal());
			fw->FloatControlBar(recBar,CPoint(100,100));
		}
		recBar->view->SetRecord(rec,0);
		fw->ShowControlBar(recBar,TRUE,FALSE);
		return;
	}
}

void MapPaneView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//if (edit && edit->OnLButtonDblClk(nFlags, point)) 
	//	return;

	//Coord c = crdPnt(point);
	//MapCompositionDoc* mcd = GetDocument();
	//for (int i = 0; i < mcd->rootDrawer->getDrawerCount(); ++i) 
	//{
	//	NewDrawer* dr = rootDrawer->getDrawer(i);
	//	Coord crd = c;
	//	if (dr->isSelectable() && dr->isActive()) 
	//	{
	//	AbstractMapDrawer* bmd = dynamic_cast<AbstractMapDrawer*>(dr);
	//	if ( !bmd)
	//		continue;
	//	BaseMapPtr *bmpPtr = bmd->getBaseMap();
	//	if (bmpPtr->cs() != mcd->georef->cs())
	//	{
	//		crd = bmd->basemap()->cs()->cConv(mcd->rootDrawer->getCoordinateSystem(), c);
	//	}
	//	switch (dca) 
	//	{
	//		case dcaRECORD:
	//		{
	//			Ilwis::Record rec = dr->rec(crd);
	//			if (rec.fValid()) {
	//				ShowRecord(rec);
	//				return;
	//			}
	//		} break;
	//		case dcaRPR:
	//		{
	//			FormBaseDialog* frm = dr->wEditRpr(this, crd);
	//			if (0 == frm)
	//				continue;
	//			else {
	//				if (frm->fOkClicked())
	//					SetDirty();
	//				delete frm;
	//				SetFocus();
	//				return;
	//			}
	//		}	break;
	//		case dcaACTION:
	//			dr->ExecuteAction(crd);
	//			return;
	//	}
	//}
	//}
	::MessageBox(0,"To Be implemented again", "!!!", MB_OK);
	CView::OnLButtonDblClk(nFlags, point);
}

void MapPaneView::EditNamedLayer(const FileName& fn)
{
	if (fn.sFile[0] == '#') {
		MessageBox(SMWErrCantEditBackup.sVal(), SMWErrError.sVal());
		return;
	}
	delete edit;
	edit = 0;
	//if (".sms" == fn.sExt) {
	//	SampleSet smp(fn);
	//	if (smp->fInitStat())
	//		edit = new SampleSetEditor(this, smp);
	//}
	//else if (".mpr" == fn.sExt) {
	//	Map mp(fn);
	//	edit = new PixelEditor(this, mp);
	//}
	//else if (".mpp" == fn.sExt) {
	//	PointMap mp(fn);
	//	edit = new PointEditor(this, mp);
	//}
	//else if (".mps" == fn.sExt) {
	//	SegmentMap mp(fn);
	//	edit = new SegmentEditor(this, mp);
	//}
	//else if (".mpa" == fn.sExt) {
	//	PolygonMap mp(fn);
	//	edit = new PolygonEditor(this, mp);
	//}
	//if (edit && !edit->fOk) {
	//	delete edit;
	//	edit = 0;
	//}
	//if (edit)	{
	//	SetDirty();
	//}
	//UpdateFrame();
	//MapWindow* mw = mwParent();
	//if (mw)
	//	mw->SetAcceleratorTable();
}

void MapPaneView::UpdateFrame()
{
	CFrameWnd* wnd = fwParent();
	if (0 == wnd) // happens with inplace activation
		return;
	if (edit) {
		wnd->SetIcon(edit->icon(), TRUE);
		String sTitle = edit->sTitle();
		wnd->SetWindowText(sTitle.sVal());
	}
	else {
		wnd->SetIcon(GetDocument()->icon(), TRUE);
		wnd->OnUpdateFrameTitle(TRUE);
	}
}


void MapPaneView::OnPixelEdit()
{
	class PixMapForm: public FormWithDest
	{
	public:
		PixMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMWTitleEditRasMap)
		{
			new FieldBlank(root);
			FieldDataTypeLarge* fdtl = 
				new FieldDataTypeLarge(root, sName, ".MPR.SMS");
			fdtl->SetOnlyEditable();  
			//    new FieldMap(root, SDUiMapName, sName);
			SetMenHelpTopic(htpDspEditPixMap);
			create();
		}
	};

	String sRasName;
	PixMapForm frm(this, &sRasName);
	if (frm.fOkClicked()) {
		FileName fn(sRasName);
		EditNamedLayer(fn);
	}
}

void MapPaneView::OnNoneEdit()
{
	SetFocus(); // be sure that edit fields get a WM_KILLFOCUS
	delete edit;
	edit = 0;
	MapWindow* mw = mwParent();
	if (0 != mw) {
		mw->UpdateMenu();
		mw->SetAcceleratorTable();
		mw->bbDataWindow.LoadButtons("map.but");
		mw->RecalcLayout();
		mw->ShowControlBar(&mw->barScale,TRUE,FALSE);
		mw->RecalcLayout();
	}
	UpdateFrame();
	GetParentFrame()->RecalcLayout();
	GetDocument()->UpdateAllViews(0);
}

void MapPaneView::OnPointEdit()
{
	class PntMapForm: public FormWithDest
	{
	public:
		PntMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMWTitleEditPntMap)
		{
			new FieldBlank(root);
			FieldDataTypeLarge* fdtl = 
				new FieldDataTypeLarge(root, sName, ".MPP");
			fdtl->SetOnlyEditable();  
			//    new FieldPointMap(root, SDUiPntMap, sName);
			SetMenHelpTopic(htpDspEditPntMap);
			create();
		}
	};
	String sPntName;
	/*
	if (sPntName.length() == 0) {
	for(DLIterP<Drawer> iter(dl);
	iter.fValid();
	++iter) {
	String s = iter()->sName();
	if (s.sLeft(3) == "pnt" && s.length() > 4) {
	sPntName = &s[4];
	break;
	}
	}
	}
	*/
	PntMapForm frm(this, &sPntName);
	if (frm.fOkClicked()) {
		FileName fn(sPntName,".mpp",true);
		EditNamedLayer(fn);
	}
}

void MapPaneView::OnSegmentEdit()
{
	class SegMapForm: public FormWithDest
	{
	public:
		SegMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMWTitleEditSegMap)
		{
			new FieldBlank(root);
			FieldDataTypeLarge* fdtl = 
				new FieldDataTypeLarge(root, sName, ".MPS");
			fdtl->SetOnlyEditable();  
			//    new FieldSegmentMap(root, SDUiSegMap, sName);
			SetMenHelpTopic(htpDspEditSegMap);
			create();
		}
	};
	String sSegName;
	SegMapForm frm(this, &sSegName);
	if (frm.fOkClicked()) {
		FileName fn(sSegName,".mps",true);
		EditNamedLayer(fn);
	}
}

void MapPaneView::OnPolygonEdit()
{
	class PolMapForm: public FormWithDest
	{
	public:
		PolMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMWTitleEditPolMap)
		{
			new FieldBlank(root);
			FieldDataTypeLarge* fdtl = 
				new FieldDataTypeLarge(root, sName, ".MPA");
			fdtl->SetOnlyEditable();  
			//    new FieldPolygonMap(root, SDUiPolMap, sName);
			SetMenHelpTopic(htpDspEditPolMap);
			create();
		}
	};

	String sPolName;
	PolMapForm frm(this, &sPolName);
	if (frm.fOkClicked()) {
		FileName fn(sPolName,".mpa",true);
		EditNamedLayer(fn);
	}
}

void MapPaneView::OnGeoRefEdit()
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//GeoRefCTP* gc = georef->pgCTP();
	//if (gc ) {
	//	if (edit)
	//		delete edit;
	//	edit = 0;
	//	if (gc)
	//		edit = new GeoRefEditor(this,georef);
	//	if (edit)	{
	//		Invalidate();
	//	}
	//	UpdateFrame();
	//	MapWindow* mw = mwParent();
	//	if (0 != mw) 
	//		mw->SetAcceleratorTable();
	//	return;  
	//}
	//String sFileName = georef->fnObj.sFullNameQuoted();
	//if ("" == sFileName)
	//	return;
	//IlwWinApp()->OpenDocumentFile(sFileName.sVal());
}

void MapPaneView::OnCoordSysEdit()
{
	CoordSystem cs = GetDocument()->rootDrawer->getCoordinateSystem();
	CoordSystemCTP* cstp = cs->pcsCTP();
	if (cstp) {
		if (edit)
			delete edit;
		edit = 0;
		//edit = new CoordSystemEditor(this,cs);
		throw ErrorObject(String("TO be done %s %s", __FILE__, __LINE__));
		if (edit)	{
			Invalidate();
		}
		UpdateFrame();
		MapWindow* mw = mwParent();
		if (0 != mw) 
			mw->SetAcceleratorTable();
		return;  
	}
	String sFileName = cs->fnObj.sFullNameQuoted();
	if ("" == sFileName)
		return;
	IlwWinApp()->OpenDocumentFile(sFileName.sVal());
}

void MapPaneView::OnUpdateGeoRefEdit(CCmdUI* pCmdUI)
{
	//throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//GeoRef georef = GetDocument()->georef;
	//bool fEdit = georef.fValid() && !georef->fReadOnly() && !georef->fDependent();
	//fEdit = fEdit && georef->fnObj.sFullName().length() > 0; // internal georefs are not editable
	//pCmdUI->Enable(fEdit);
}

void MapPaneView::OnUpdateCoordSysEdit(CCmdUI* pCmdUI)
{
	bool fEdit = false;
	CoordSystem cs = GetDocument()->rootDrawer->getCoordinateSystem();
	fEdit = cs.fValid() && !cs->fReadOnly();
	fEdit = fEdit && cs->fnObj.sFullName().length() > 0; // internal coordsystems are not editable
	pCmdUI->Enable(fEdit);
}

LRESULT MapPaneView::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	if (pib){
		MapCompositionDoc* mcd = GetDocument();
		//mcd->pixInfoDoc->SendMessage(ILW_UPDATE, wParam, lParam);
	}
	if (edit)
		return edit->OnUpdate(wParam,lParam);
	return 0;
}

void MapPaneView::OnAdjustSize()
{
	fAdjustSize = !fAdjustSize;
}

void MapPaneView::OnUpdateAdjustSize(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(fAdjustSize);
	pCmdUI->Enable(!GetParentFrame()->IsZoomed());
}

void MapPaneView::UseScale(double rScale)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//double rOldScale = rScaleShow();

	//MapCompositionDoc* mcd = GetDocument();
	//double rPixSize = mcd->georef->rPixSize();
	//if (rUNDEF == rPixSize)
	//	return;
	//CoordSystemLatLon* csll = mcd->georef->cs()->pcsLatLon();
	//if (0 != csll)
	//	rPixSize *= M_PI / 180 * csll->ell.a;

	//CClientDC dc(this);
	//int iPixPerInchY = dc.GetDeviceCaps(LOGPIXELSY);
	//double rPixPermm = iPixPerInchY / 25.4;         // (pixel per mm)
	//rPixSize *= rPixPermm;
	//rPixSize *= 1000; // 1000 mm per m

	//iXpos -= scale(dim.width()) / 2;
	//iYpos -= scale(dim.height()) / 2;

	//rScale /= rPixSize;
	//if (rScale < 1)
	//	_rScale = 1/rScale;
	//else
	//	_rScale = -rScale;

	//bool fTooSmall = scale(dim.width()) < 2 || scale(dim.height()) < 2;
	//if (fTooSmall) {
	//	if (rOldScale > rScale) {
	//		MessageBeep(MB_ICONASTERISK);
	//		UseScale(rOldScale);
	//		return;
	//	}
	//}

	//iXpos += scale(dim.width()) / 2;
	//iYpos += scale(dim.height()) / 2;
	//CalcFalseOffsets();
	//CalcMax();
	//setScrollBars();
	//SetDirty();
}

void MapPaneView::OnSetDoubleClickAction(UINT nID)
{
	switch (nID)
	{
	case ID_MAPDBLCLKRECORD:
		dca = dcaRECORD;
		break;
	case ID_MAPDBLCLKRPR:
		dca = dcaRPR;
		break;
	case ID_MAPDBLCLKACTION:
		dca = dcaACTION;
		break;
	}
}

void MapPaneView::OnUpdateDoubleClickAction(CCmdUI* pCmdUI)
{
	bool fYes = false;
	switch (pCmdUI->m_nID) 
	{
	case ID_MAPDBLCLKRECORD:
		fYes = dca == dcaRECORD;
		break;
	case ID_MAPDBLCLKRPR:
		fYes = dca == dcaRPR;
		break;
	case ID_MAPDBLCLKACTION:
		fYes = dca == dcaACTION;
		break;
	}
	pCmdUI->SetRadio(fYes);
	if (edit)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void MapPaneView::OnSetDoublClickAction()
{
	class SetDblClkActionForm: public FormWithDest
	{
	public:
		SetDblClkActionForm(CWnd* parent, int* iDblClkAct)
			: FormWithDest(parent, SMWTitleDblClkAction)
		{
			new FieldBlank(root);
			RadioGroup* rg = new RadioGroup(root, SMWUiDblClkTo, iDblClkAct);
			new RadioButton(rg, SMWUiEditAttribute);
			new RadioButton(rg, SMWUiEditRepres);
			new RadioButton(rg, SMWUiExecuteAction);
			SetMenHelpTopic(htpDspDblClkAction);
			create();
		}
	};
	int iDblClkAct = (int)dca;
	SetDblClkActionForm frm(this, &iDblClkAct);
	if (frm.fOkClicked())
		dca = (DoubleClickAction)iDblClkAct;
}

// for the moment no OLE Paste yet, only simple Clipboard Paste
void MapPaneView::OnPaste()
{
	vector<FileName> arFiles;
	Catalog::GetFromClipboard(arFiles);
	if (arFiles.size() == 0) 
		return;
	AddFiles(arFiles);
}

void MapPaneView::OnUpdatePaste(CCmdUI* pCmdUI)
{
	int iFmtCopy = RegisterClipboardFormat("IlwisCopy");
	bool fPaste = IsClipboardFormatAvailable(iFmtCopy) 
		? true : false;
	pCmdUI->Enable(fPaste);
}

void MapPaneView::OnCopy()
{
	MapCompositionDoc* mcd = GetDocument();
	MapCompositionSrvItem* mcsi = mcd->GetEmbeddedItem();
	//	mcsi->CopyToClipboard(FALSE);
	COleDataSource* pDataSource = new COleDataSource;
	try {
		mcsi->AddOtherClipboardData(pDataSource);
	}
	catch (...)
	{
		delete pDataSource;
	}
	pDataSource->SetClipboard();
}

DROPEFFECT MapPaneView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	MapCompositionDoc* mcd = GetDocument();
	bool fOk = false;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
			FileName fn(sFileName);
			if (mcd->fAppendable(fn))	{
				fOk = true;
				break;
			}
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);
	if (fOk)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

void MapPaneView::OnDragLeave() 
{
	// TODO: Add your specialized code here and/or call the base class
}

DROPEFFECT MapPaneView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	return OnDragEnter(pDataObject, dwKeyState, point);
	//return CView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL MapPaneView::AddFiles(vector<FileName>& afn)
{
	bool fOk = false;
	MapCompositionDoc* mcd = GetDocument();
	for(vector<FileName>::iterator cur = afn.begin(); cur != afn.end(); ++cur)
	{
		FileName fn = *cur;
		if (!fn.fExist()) {
			FileName fnSys = fn;
			String sStdDir = IlwWinApp()->Context()->sStdDir();
			fnSys.Dir(sStdDir);
			if (fnSys.fExist())
				fn = fnSys;
		}
		mcd->drAppend(fn);
		// configure new drawer (option: no show?)
		if (fOk) {
			mcd->UpdateAllViews(0,2);
			mcd->SetModifiedFlag();
			mcd->UpdateAllItems(0);
		}
		return fOk;
	}
	return FALSE;
}

struct FilesInThreadStruct
{
	HWND handle;
	vector<FileName> afn;
};

BOOL MapPaneView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return FALSE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	int iFiles = 0;
	vector<FileName> afn;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
			FileName fn(sFileName);
			afn.push_back(fn);
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);
	// if send by SendMessage() prevent deadlock
	BOOL fCalledFromOtherThread = ReplyMessage(0);
	return AddFiles(afn);
	// code below is wrong , mpv may not be transferred to other thread. Handles may not be correct there
	//if (fCalledFromOtherThread)
	//	return AddFiles(afn);
	//else
	//{
	//	FilesInThreadStruct * pfits = new FilesInThreadStruct; // deleted in thread
	//	pfits->afn = afn;
	//	pfits->handle = this->m_hWnd;
	//	/AddFilesInThread((void *)pfits);
	//	AfxBeginThread(AddFilesInThread, pfits);
	//	return TRUE;
	//}
}

UINT MapPaneView::AddFilesInThread(LPVOID pParam)
{
	FilesInThreadStruct * pfits = (FilesInThreadStruct*)pParam;
	//MapPaneView *mpv = (MapPaneView *)CWnd::FromHandle(pfits->handle);
	if (pfits == NULL)
		return 1;

	IlwWinApp()->Context()->InitThreadLocalVars(); // this is a new thread .. and localvars are touched in AddFiles

	//mpv->AddFiles(pfits->afn);
	delete pfits;

	IlwWinApp()->Context()->RemoveThreadLocalVars();

	return 0;
}

/*
void MapPaneView::ShowLoadingText()
{
return; // do not show anything
CClientDC dc(this);
CFont fnt;
fnt.CreatePointFont(160,"Arial",&dc);
CFont* fntOld = dc.SelectObject(&fnt);
dc.SetBkMode(OPAQUE);
dc.SetBkColor(RGB(255,255,255));
dc.SetTextColor(RGB(0,0,0));
dc.SetTextAlign(TA_CENTER|TA_BASELINE);
CRect rect;
GetClientRect(&rect);
CPoint pt = rect.CenterPoint();
String s = SMWRemLoading;
CString str = s.scVal();
dc.TextOut(pt.x,pt.y,str);
dc.SelectObject(fntOld);
}
*/

void MapPaneView::OnCreatePntMap()
{
	String sMap, sDom;
	MapCompositionDoc* mcd = GetDocument();
	CoordSystem csy = mcd->rootDrawer->getCoordinateSystem();
	String sCsys = csy->sName();
	CoordBounds cb = csy->cb;
	bool fOk;
	{
		FormCreatePnt frm(this, &sMap, sCsys, cb, sDom);
		fOk = frm.fOkClicked();
	}  
	if (fOk) {
		FileName fn(sMap,".mpp",true);
		EditNamedLayer(fn);
	}
}

void MapPaneView::OnCreateSegMap()
{
	String sMap, sDom;
	MapCompositionDoc* mcd = GetDocument();
	String sCsys = mcd->rootDrawer->getCoordinateSystem()->sName();
	CoordBounds cb = mcd->rootDrawer->getCoordinateSystem()->cb;
	bool fOk;
	{
		FormCreateSeg frm(this, &sMap, sCsys, cb, sDom);
		fOk = frm.fOkClicked();
	}  
	if (fOk) {
		FileName fn(sMap,".mps",true);
		EditNamedLayer(fn);
	}
}

void MapPaneView::OnCreateRasMap()
{
	String sMap, sDom;
	FormCreateMap frm(this, &sMap, "", sDom);
	bool fOk = frm.fOkClicked();
	if (fOk) {
		FileName fn(sMap,".mpr",true);
		EditNamedLayer(fn);
	}
}

void MapPaneView::OnCreateGeoRef()
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//String sGrf;
	//bool fOk;
	//MapCompositionDoc* mcd = GetDocument();
	//Map mp = mcd->mp;
	//GeoRef georef = mcd->georef;
	//if (!mp.fValid())
	//	return;
	//{
	//	FormCreateGeoRefRC frm(this, &sGrf, mp, georef->cs(), georef->cb(), true);
	//	fOk = frm.fOkClicked();
	//}  
	//if (fOk) {
	//	FileName fn(sGrf);
	//	fn.sExt = ".grf";
	//	GeoRef grf(fn);

	//	mp->SetGeoRef(grf);
		//mcd->SetGeoRef(grf);

		//for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
		//{
		//	Drawer* drw = *iter;
		//    MapDrawer* md = dynamic_cast<MapDrawer*>(drw);
		//    if (md) {
		//      MapList ml;
		//      MapListColorCompDrawer* mlcd = dynamic_cast<MapListColorCompDrawer*>(drw);
		//      MapListDrawer* mld = dynamic_cast<MapListDrawer*>(drw);
		//      if (mlcd)
		//        ml = mlcd->ml();
		//      else if (mld)
		//        ml = mld->ml();
		//      if (ml.fValid())
		//        ml->SetGeoRef(grf);
		//    }
		//  }
		//OnGeoRefEdit();
	//}  
}

void MapPaneView::OnCreateCoordSys()
{
	String sCsy;
	FormCreateCoordSystemMW frm(this, &sCsy);
	if (frm.fOkClicked()) {
		CoordSystem csy(sCsy);
		MapCompositionDoc* mcd = GetDocument();
		csy->cb = csy->cbConv(mcd->rootDrawer->getCoordinateSystem(), mcd->rootDrawer->getCoordinateSystem()->cb);
		csy->Store();
		mcd->rootDrawer->setCoordinateSystem(csy, true);
		CoordSystemTiePoints* cstp = csy->pcsTiePoints();

			for (int i = 0; i < mcd->rootDrawer->getDrawerCount(); ++i) 
			{
			NewDrawer* drw = mcd->rootDrawer->getDrawer(i);
			AbstractMapDrawer* bmd = dynamic_cast<AbstractMapDrawer*>(drw);
			if (bmd) {
				BaseMapPtr* bmp = bmd->getBaseMap();
				String s(SMWMsgRplCsy_SSS.scVal(), bmp->sName(true), bmp->cs()->sName(), csy->sName());
				int iRes = MessageBox(s.scVal(), SMWMsgRplCsy.scVal(), MB_ICONQUESTION|MB_YESNO);
				if (IDYES == iRes)
					bmp->SetCoordSystem(csy);
			}
		}
		OnCoordSysEdit();
	}
}

void MapPaneView::OnCreateSampleSet()
{
	String sSms, sMapList;
	bool fOk;

	MapCompositionDoc* mcd = GetDocument();
		for (int i = 0; i < mcd->rootDrawer->getDrawerCount(); ++i) 
	{
		NewDrawer* drw = mcd->rootDrawer->getDrawer(i);
		//MapListColorCompDrawer* mlccd = 0 ;;dynamic_cast<MapListColorCompDrawer*>(drw);
		//if (mlccd) {
		//	sMapList = mlccd->sName();
		//	break;
		//}
	}
	{
		FormCreateSampleSet frm(this, &sSms, "", sMapList);
		fOk = frm.fOkClicked();
	}
	if (fOk) {  
		FileName fn(sSms, String(".sms"));
		EditNamedLayer(fn);
	}
}

LRESULT MapPaneView::OnViewSettings(WPARAM wP, LPARAM lP)
{
	/*if (wP == SAVESETTINGS) {
		if (0 != edit)
			edit->PreSaveState();
	}*/
	return TRUE;
}


void MapPaneView::OnCreateSubMap()
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));

	MapCompositionDoc* mcd = GetDocument();
	for (int i = 0; i < mcd->rootDrawer->getDrawerCount(); ++i) 
	{
		NewDrawer* drw = mcd->rootDrawer->getDrawer(i);
		AbstractMapDrawer *mpdr = dynamic_cast<AbstractMapDrawer *>(drw);
		BaseMapPtr *mptr = mpdr->getBaseMap();
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(mptr->fnObj);
		MinMax mm = mmVisibleMapArea();
		if ( mpdr && mpdr->isActive() &&  type == IlwisObject::iotRASMAP)
		{
			String sCommand("subras %S %d %d %d %d", mptr->fnObj.sRelativeQuoted(),mm.rcMin.Row, mm.rcMin.Col, mm.rcMax.Row, mm.rcMax.Col);

			//FormMapSubMap *frm = new FormMapSubMap(this, sCommand.scVal());
			IlwWinApp()->ExecuteUI(sCommand, this);
		}
		if ( type == IlwisObject::iotPOINTMAP || type == IlwisObject::iotSEGMENTMAP)
		{
			MapCompositionDoc *mcd = GetDocument();
			//GeoRef grf = mcd->georef;
			//Coord cMin = grf->cConv(mm.rcMin);
			//Coord cMax = grf->cConv(mm.rcMax);
			Coord cMin, cMax;
			String sCommand = mptr->fnObj.sRelativeQuoted() + " " + String("%f", cMin.x) + " " + String("%f", cMin.y) + " " +
				String("%f", cMax.x) + " " + String("%f", cMax.y);

			if ( type == IlwisObject::iotSEGMENTMAP)
				sCommand = "subseg " + sCommand;
			else if ( type == IlwisObject::iotPOINTMAP)
				sCommand = "subpnt " + sCommand;	

			IlwWinApp()->ExecuteUI(sCommand, this);
		}			
	}		

}

void MapPaneView::SetDirty()
{
	if (fStarting)
		return;
	MapCompositionDoc* mcd = GetDocument();
	SimpleMapPaneView::SetDirty();
	mcd->UpdateAllViews(this,5);
}

void MapPaneView::OnShowRecordView()
{
	CFrameWnd* fw = GetTopLevelFrame();
	if (recBar->IsWindowVisible())
		fw->ShowControlBar(recBar,FALSE,FALSE);
	else
		fw->ShowControlBar(recBar,TRUE,FALSE);
}

void MapPaneView::OnUpdateShowRecordView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(0 != recBar && 0 != recBar->GetSafeHwnd());
	pCmdUI->SetCheck(0 != recBar && 0 != recBar->GetSafeHwnd() && recBar->IsVisible());
}

void MapPaneView::OnTimer(UINT timerID) {
	MapCompositionDoc *doc = GetDocument();
	doc->rootDrawer->timedEvent(timerID);
	doc->ltvGetView()->getRootTool()->timedEvent(timerID);
}