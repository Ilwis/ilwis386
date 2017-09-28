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
#include <winuser.h>
#include "Client\ilwis.h"
#include "Engine\Base\System\Engine.h"
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
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Headers\constant.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Editors\Map\BaseMapEditor.h"
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
#include "Client\TableWindow\HistogramDoc.h"
#include "Client\TableWindow\HistogramGraphView.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Georef\GeoRefEditorTableView.h"
#include "Client\Editors\Georef\GeoRefEditorTableBar.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\Editors\Georef\TransformationComboBox.h"
#include "Client\Editors\Georef\TiePointEditor.h"
#include "Client\Editors\Georef\GeoRefEditor.h"
#include "Client\Editors\CoordSystem\CoordSysEditor.h"
#include "Client\FormElements\syscolor.h"
#include "Client\Mapwindow\PixelInfoBar.h"
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
	//ON_COMMAND(ID_SELECTAREA, OnSelectArea)
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
	ON_COMMAND(ID_OPENPIXELINFO, OnShowPixelInfo)
	ON_UPDATE_COMMAND_UI(ID_OPENPIXELINFO, OnUpdatePixelInfo)
	ON_COMMAND(ID_DEFAULTSCALE, OnDefaultScale)
	ON_COMMAND(ID_SCALE1, OnScaleOneToOne)
	ON_UPDATE_COMMAND_UI(ID_SCALE1, OnUpdateScaleOneToOne)
	ON_MESSAGE(ILW_UPDATE, OnUpdate)
	ON_WM_MOUSEACTIVATE()
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_METRIC_COORD, OnMetricCoord)
	ON_UPDATE_COMMAND_UI(ID_METRIC_COORD, OnUpdateMetricCoord)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_CREATEPNTMAP, OnCreatePntMap)
	ON_COMMAND(ID_CREATESEGMAP, OnCreateSegMap)
	ON_COMMAND(ID_CREATERASMAP, OnCreateRasMap)
	ON_COMMAND(ID_CREATEGRF, OnCreateGeoRef)
	ON_COMMAND(ID_CREATECSY, OnCreateCoordSys)
	ON_COMMAND(ID_CREATESMS, OnCreateSampleSet)
	ON_COMMAND(ID_CREATESUBMAP, OnCreateSubMap)
	ON_COMMAND(ID_COPY_COORD, OnCopyCoord)
	ON_MESSAGE(ILWM_VIEWSETTINGS, OnViewSettings)
	ON_COMMAND_RANGE(ID_MAPDBLCLKRECORD, ID_MAPDBLCLKACTION, OnSetDoubleClickAction)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MAPDBLCLKRECORD, ID_MAPDBLCLKACTION, OnUpdateDoubleClickAction)
	ON_COMMAND(ID_SHOWRECORDVIEW, OnShowRecordView)
	ON_UPDATE_COMMAND_UI(ID_SHOWRECORDVIEW, OnUpdateShowRecordView)
	ON_COMMAND(ID_SAVE_SELECTION, OnSaveSelection)
	ON_UPDATE_COMMAND_UI(ID_SAVE_SELECTION, OnUpdateSaveSelection)
	ON_MESSAGE(ILWM_OPENMAP,OnOpenMap)
	ON_WM_KEYDOWN()
	ON_MESSAGE(ILWM_UPDATE_ANIM, OnSendUpdateAnimMessages)
	ON_WM_DESTROY()
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

void MapPaneView::OnDestroy()
{
	AfxGetApp()->PostThreadMessage(ILW_REMOVEDATAWINDOW, (WPARAM)m_hWnd, 0);
}

/////////////////////////////////////////////////////////////////////////////
// MapPaneView drawing

void MapPaneView::OnInitialUpdate() 
{
	odt->Register(this);
	SimpleMapPaneView::OnInitialUpdate();

	if (0 == mwPar) {
		CFrameWnd* fw = GetTopLevelFrame();
		mwPar = dynamic_cast<MapWindow*>(fw);
	}

	createPixInfoBar();
	PostMessage(WM_COMMAND, ID_DEFAULTSCALE, 0);

	AfxGetApp()->PostThreadMessage(ILW_ADDDATAWINDOW, (WPARAM)m_hWnd, 0);
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
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		if (mcd->rootDrawer->getGeoReference().fValid()) { // rootDrawer is set to a georeference
			CoordBounds cbMap = mcd->rootDrawer->getMapCoordBounds();
			double rFactor = min((double)dim.width() / cbMap.width(), (double)dim.height() / cbMap.height()); // set cbView to cbMap * rFactor to get 1:1 pixels
			if (rFactor > 1.0) { // grow cbView (temporarily zooms out to an impossible zoomlevel; the user couldn't zoom out to this with the zoom-buttons)
				cbMap *= rFactor;
				mcd->rootDrawer->setCoordBoundsView(cbMap, true);
			} else { // shrink: we can't shrink cbView any more than cbMap; alter cbZoom, without panning
				CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom(); // recalculate the factor based on cbZoom
				rFactor = min((double)dim.width() / cbZoom.width(), (double)dim.height() / cbZoom.height());
				cbZoom *= rFactor;
				RecenterZoomHorz(cbZoom, cbMap);
				RecenterZoomVert(cbZoom, cbMap);
				mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
			}
			setScrollBars();
			OnDraw(0);
		} else {
			GeoRef gr;
			for(int i = mcd->rootDrawer->getDrawerCount() - 1; i >= 0; --i) {
				SpatialDataDrawer *dataDrw = dynamic_cast<SpatialDataDrawer *>(mcd->rootDrawer->getDrawer(i));
				if ( !dataDrw)
					continue;
				IlwisObjectPtr *ptr = dataDrw->getObject();
				if ( IOTYPE(ptr->fnObj) == IlwisObject::iotRASMAP) {
					MapPtr * mp = (MapPtr*)ptr;
					gr = mp->gr();
					break;
				} else if ( IOTYPE(ptr->fnObj) == IlwisObject::iotMAPLIST) {
					MapListPtr * mplp = (MapListPtr*)ptr;
					gr = mplp->gr();
					break;
				}
			}
			if (gr.fValid()) {
				CoordBounds cb = gr->cb();
				if (gr->cs().fValid() && mcd->rootDrawer->fConvNeeded(gr->cs()))
					cb = mcd->rootDrawer->getCoordinateSystem()->cbConv(gr->cs(), cb);
				RowCol rcSize = gr->rcSize();
				double rPixSizeX = 0;
				double rPixSizeY = 0;
				if (rcSize.Col != 0)
					rPixSizeX = cb.width() / (double)rcSize.Col;
				else
					rPixSizeX = gr->rPixSize();
				if (rcSize.Row != 0)
					rPixSizeY = cb.height() / (double)rcSize.Row;
				else
					rPixSizeY = gr->rPixSize();

				double rNewMapWidth = dim.width() * rPixSizeX;
				double rNewMapHeight = dim.height() * rPixSizeY;

				CoordBounds cbMap = mcd->rootDrawer->getMapCoordBounds();
				double rFactor = min(rNewMapWidth / cbMap.width(), rNewMapHeight / cbMap.height()); // set cbView to cbMap * rFactor to get 1:1 pixels
				if (rFactor > 1.0) { // grow cbView (temporarily zooms out to an impossible zoomlevel; the user couldn't zoom out to this with the zoom-buttons)
					cbMap *= rFactor;
					mcd->rootDrawer->setCoordBoundsView(cbMap, true);
				} else if (rFactor > 0) { // shrink: we can't shrink cbView any more than cbMap; alter cbZoom, without panning
					CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom(); // recalculate the factor based on cbZoom
					rFactor = min(rNewMapWidth / cbZoom.width(), rNewMapHeight / cbZoom.height());
					cbZoom *= rFactor;
					RecenterZoomHorz(cbZoom, cbMap);
					RecenterZoomVert(cbZoom, cbMap);
					mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
				}

				setScrollBars();
				OnDraw(0);
			}
		}
	}
}

void MapPaneView::OnUpdateScaleOneToOne(CCmdUI* pCmdUI)
{
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		GeoRef gr = mcd->rootDrawer->getGeoReference();
		if (!gr.fValid()) {
			for(int i = mcd->rootDrawer->getDrawerCount() - 1; i >= 0; --i) {
				SpatialDataDrawer *dataDrw = dynamic_cast<SpatialDataDrawer *>(mcd->rootDrawer->getDrawer(i));
				if ( !dataDrw)
					continue;
				IlwisObjectPtr *ptr = dataDrw->getObject();
				if ( IOTYPE(ptr->fnObj) == IlwisObject::iotRASMAP) {
					MapPtr * mp = (MapPtr*)ptr;
					gr = mp->gr();
					break;
				} else if ( IOTYPE(ptr->fnObj) == IlwisObject::iotMAPLIST) {
					MapListPtr * mplp = (MapListPtr*)ptr;
					gr = mplp->gr();
					break;
				}
			}
		}
		pCmdUI->Enable(gr.fValid());
	} else
		pCmdUI->Enable(FALSE);
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

void MapPaneView::OnShowPixelInfo()
{
	if (pib) {
		if (pib->IsWindowVisible())
			getFrameWindow()->ShowControlBar(pib,FALSE,FALSE);
		else
			getFrameWindow()->ShowControlBar(pib,TRUE,FALSE);
	}
}

void MapPaneView::OnEntireMap()
{
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		mcd->rootDrawer->setCoordBoundsView(mcd->rootDrawer->getMapCoordBounds(),true);
		setScrollBars();
		fStarting = false;
		mcd->mpvGetView()->Invalidate();
	}
	if (iActiveTool == ID_ZOOMOUT)
		noTool(ID_ZOOMOUT);
}

void MapPaneView::OnUpdateEntireMap(CCmdUI* pCmdUI)
{
	MapCompositionDoc* mcd = GetDocument();
	pCmdUI->Enable(mcd->rootDrawer->getDrawerCount() > 0);
}

void MapPaneView::OnUpdatePixelInfo(CCmdUI* pCmdUI)
{
	bool fCheck = (pib != 0) && (pib->IsWindowVisible() != 0);
	pCmdUI->SetCheck(fCheck);
}

void MapPaneView::ZoomInOn(Coord crd, double rDist)
{
	//throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	OnEntireMap();
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


#define sMen(ID) ILWSF("men",ID).c_str()
#define addMenu(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
void MapPaneView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (edit && edit->OnContextMenu(pWnd, point))
		return;
	CMenu men;
	men.CreatePopupMenu();
	addMenu(ID_NORMAL);
	addMenu(ID_ZOOMIN);
	addMenu(ID_ZOOMOUT);
	addMenu(ID_PANAREA);
	addMenu(ID_ENTIREMAP);
	addMenu(ID_COPY_COORD);

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

	//men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
	switch (men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, point.x, point.y, this)){
		case ID_COPY_COORD:
			{
					MapCompositionDoc* mcd = GetDocument();
					POINT p = point;
					ScreenToClient(&p);
					Coord cRowCol (mcd->rootDrawer->screenToOpenGL(RowCol(p.y,p.x)));
					if ( cRowCol.fUndef())
						return ;
					Coord c (mcd->rootDrawer->glToWorld(cRowCol));
					String s("%f,%f", c.x, c.y);
					zClipboard clip(this);
					clip.add((char *)s.c_str());
			/*		OpenClipboard();
					EmptyClipboard();	
					HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE,s.size()+1);
					if (!hg){
						CloseClipboard();
						return;
					}
					memcpy(GlobalLock(hg),s.c_str(),s.size()+1);
					GlobalUnlock(hg);
					SetClipboardData(CF_TEXT,hg);
					CloseClipboard();
					GlobalFree(hg);*/
			}
			break;
		case ID_NORMAL:
			OnNoTool();
			break;
		case ID_ZOOMIN:
			OnZoomIn();
			break;
		case ID_ZOOMOUT:
			OnZoomOut();
			break;
		case ID_PANAREA:
			OnPanArea();
			break;
		case ID_ENTIREMAP:
			OnEntireMap();
			break;
	}
}

#undef addMenu

void MapPaneView::ShowRecord(const Ilwis::Record& rec)
{
	if (rec.fValid()) {
		CFrameWnd* fw = GetParentFrame();
		if (0 == recBar) {
			recBar = new RecordBar();
			recBar->Create(fw, ID_RECORDBAR);
			recBar->SetWindowText(TR("Attributes").c_str());
			fw->FloatControlBar(recBar,CPoint(100,100));
		}
		recBar->view->SetRecord(rec,0);
		fw->ShowControlBar(recBar,TRUE,FALSE);
		return;
	}
}

void MapPaneView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDblClk(nFlags, point);
}

void MapPaneView::EditNamedLayer(const FileName& fn)
{
	if (fn.sFile[0] == '#') {
		MessageBox(TR("Can't edit backup file").c_str(), TR("Error").c_str());
		return;
	}
	delete edit;
	edit = 0;

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
			: FormWithDest(parent, TR("Edit Raster Map"))
		{
			new FieldBlank(root);
			FieldDataTypeLarge* fdtl = 
				new FieldDataTypeLarge(root, sName, ".MPR.SMS");
			fdtl->SetOnlyEditable();  
			//    new FieldMap(root, SDUiMapName, sName);
			SetMenHelpTopic("ilwismen\\edit_raster_map.htm");
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
		//mw->ShowControlBar(&mw->barScale,TRUE,FALSE);
		//mw->RecalcLayout();
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
			: FormWithDest(parent, TR("Edit Point Map"))
		{
			new FieldBlank(root);
			FieldDataTypeLarge* fdtl = 
				new FieldDataTypeLarge(root, sName, ".MPP");
			fdtl->SetOnlyEditable();  
			//    new FieldPointMap(root, SDUiPntMap, sName);
			SetMenHelpTopic("ilwismen\\edit_point_map.htm");
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
			: FormWithDest(parent, TR("Edit Segment Map"))
		{
			new FieldBlank(root);
			FieldDataTypeLarge* fdtl = 
				new FieldDataTypeLarge(root, sName, ".MPS");
			fdtl->SetOnlyEditable();  
			//    new FieldSegmentMap(root, SDUiSegMap, sName);
			SetMenHelpTopic("ilwismen\\edit_segment_map.htm");
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
			: FormWithDest(parent, TR("Edit Polygon Map"))
		{
			new FieldBlank(root);
			FieldDataTypeLarge* fdtl = 
				new FieldDataTypeLarge(root, sName, ".MPA");
			fdtl->SetOnlyEditable();  
			//    new FieldPolygonMap(root, SDUiPolMap, sName);
			SetMenHelpTopic("ilwismen\\edit_polygon_map.htm");
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
	SpatialDataDrawer *absndrw = dynamic_cast<SpatialDataDrawer *>(GetDocument()->rootDrawer->getDrawer(0));
	BaseMapPtr *bmp = absndrw->getBaseMap();
	if ( !IOTYPE((*bmp).fnObj) == IlwisObject::iotRASMAP)
		return;
	MapPtr *mptr = (MapPtr *)bmp;
	GeoRef georef = (*mptr).gr();
	GeoRefCTP* gc = georef->pgCTP();
	if (gc ) {
		if (edit)
			delete edit;
		edit = 0;
		//throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
		if (gc)
			edit = new GeoRefEditor(this,georef);
		if (edit)	{
			Invalidate();
		}
		UpdateFrame();
		MapWindow* mw = mwParent();
		if (0 != mw) 
			mw->SetAcceleratorTable();
		return;  
	}
	String sFileName = georef->fnObj.sFullNameQuoted();
	if ("" == sFileName)
		return;
	IlwWinApp()->OpenDocumentFile(sFileName.sVal());
}

void MapPaneView::OnCoordSysEdit()
{
	CoordSystem cs = GetDocument()->rootDrawer->getCoordinateSystem();
	CoordSystemCTP* cstp = cs->pcsCTP();
	if (cstp) {
		if (edit)
			delete edit;
		edit = 0;
		edit = new CoordSystemEditor(this,cs);
		//throw ErrorObject(String("TO be done %s %s", __FILE__, __LINE__));
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
			: FormWithDest(parent, TR("Double Click Action"))
		{
			new FieldBlank(root);
			RadioGroup* rg = new RadioGroup(root, TR("Double Click to"), iDblClkAct);
			new RadioButton(rg, TR("&Edit Attribute"));
			new RadioButton(rg, TR("Edit &Representation"));
			new RadioButton(rg, TR("Execute &Action"));
			SetMenHelpTopic("ilwismen\\double_click_action.htm");
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
	AddFiles(arFiles, false);
}

void MapPaneView::OnUpdateMetricCoord(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(useMetricCoords);
}

void MapPaneView::OnMetricCoord() {
	useMetricCoords = !useMetricCoords;
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
	class ClipboardCopyForm: public FormWithDest
	{
	public:
		ClipboardCopyForm(CWnd* wParent, int* iDPI, int* _iXSize, int* _iYSize)
		: FormWithDest(wParent, TR("Copy image to clipboard"))
		, iXSize(_iXSize)
		, iYSize(_iYSize)
		, fLock(true)
		, fInCallBack(false)
		{
			if (*iXSize > 0 && *iYSize > 0)
				rRatio = (*iYSize) / (double)(*iXSize);
			else
				rRatio = 1;
			ValueRange vri(1, 9999999, 1);
			cbLock = new CheckBox(root, TR("Lock aspect ratio"), &fLock);
			cbLock->SetCallBack((NotifyProc)&ClipboardCopyForm::LockChanged);
			cbLock->SetIndependentPos();
			fiXSize = new FieldInt(root, TR("X-pixels"), iXSize, vri);
			fiXSize->SetCallBack((NotifyProc)&ClipboardCopyForm::XSizeChanged);
			fiYSize = new FieldInt(root, TR("Y-pixels"), iYSize, vri);
			fiYSize->Align(fiXSize, AL_AFTER);
			fiYSize->SetCallBack((NotifyProc)&ClipboardCopyForm::YSizeChanged);
			ValueRange vrdpi(1, 153600, 1);
			FieldInt * fiDPI = new FieldInt(root, TR("DPI"), iDPI, vrdpi);
			fiDPI->Align(fiXSize, AL_UNDER);
			//fiDPI->SetIndependentPos();
			create();
		}
	private:
		int XSizeChanged(Event *) {
			if (fInCallBack)
				return 0;
			fInCallBack = true;
			if (fLock) {
				fiXSize->StoreData();
				*iYSize = round(rRatio * *iXSize);
				fiYSize->SetVal(*iYSize);
			}
			fInCallBack = false;
			return 0;
		}
		int YSizeChanged(Event *) {
			if (fInCallBack)
				return 0;
			fInCallBack = true;
			if (fLock) {
				fiYSize->StoreData();
				*iXSize = round(*iYSize / rRatio);
				fiXSize->SetVal(*iXSize);
			}
			fInCallBack = false;
			return 0;
		}
		int LockChanged(Event *) {
			cbLock->StoreData();
			return 0;
		}
		double rRatio;
		int * iXSize;
		int * iYSize;
		bool fLock;
		FieldInt * fiXSize;
		FieldInt * fiYSize;
		CheckBox * cbLock;
		bool fInCallBack;
	};

	// Get the Views size (client area!)
	CRect	mRect;			// Position of this window's client area
	GetClientRect(&mRect);
	ASSERT(mRect.left == 0);
	ASSERT(mRect.top == 0);

	int nReduceResCount = 0;
	int iDPI = 300;
	double fac = iDPI/72.;		// 72 DPI (screen) --> <user selected> DPI (clipboard)
	int nXRes = int(mRect.right * fac); // initial values, proposed to the user
	int nYRes = int(mRect.bottom * fac);
	CoordBounds cbZoom = GetDocument()->rootDrawer->getCoordBoundsZoom();
	CoordBounds cbMap = GetDocument()->rootDrawer->getMapCoordBounds();
	CoordBounds cbClip; // default: cbUNDEF; if required, cbClip will be cbMap intersected with cbZoom (thus cbZoom without the gray "outside" areas)
	if (!GetDocument()->rootDrawer->is3D() && (cbZoom.MinX() < cbMap.MinX() || cbZoom.MaxX() > cbMap.MaxX() || cbZoom.MinY() < cbMap.MinY() || cbZoom.MaxY() > cbMap.MaxY())) {
		// use cbMap and cbZoom to compute cbClip
		cbClip = cbMap;
		// compute intersection with cbZoom
		if (cbClip.MinX() < cbZoom.MinX())
			cbClip.MinX() = cbZoom.MinX();
		if (cbClip.MaxX() > cbZoom.MaxX())
			cbClip.MaxX() = cbZoom.MaxX();
		if (cbClip.MinY() < cbZoom.MinY())
			cbClip.MinY() = cbZoom.MinY();
		if (cbClip.MaxY() > cbZoom.MaxY())
			cbClip.MaxY() = cbZoom.MaxY();
		if (cbClip.width() != 0 && cbClip.height() != 0) {
			if (cbClip.width() > cbClip.height()) // horizontal dimension more accurate
				nYRes = round(nXRes * cbClip.height() / cbClip.width());
			else // vertical dimension more accurate
				nXRes = round(nYRes * cbClip.width() / cbClip.height());
		} else
			cbClip = CoordBounds();
	}
	ClipboardCopyForm frm (0, &iDPI, &nXRes, &nYRes);
	if (frm.fOkClicked()) {
		BeginWaitCursor();
		SetRedraw(FALSE);
		fac = iDPI/72.;
		while (!EditCopy(nXRes, nYRes, fac, cbClip))
		{
			// retry again with reduced resolution
			++nReduceResCount;
			if (nReduceResCount >= 10)
				break;
			nXRes /= 2;
			nYRes /= 2;
		}
		SetRedraw(TRUE);
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_NOERASE);
		EndWaitCursor();
	}
}

BOOL MapPaneView::EditCopy(int nXRes, int nYRes, double fac, CoordBounds & cbClip)
{
	HDC		hMemDC, hTmpDC;
	BITMAPINFO	bitmapInfo;
	HBITMAP		hDib;
	LPVOID		pBitmapBits;
	BOOL		bSuccess = FALSE;

//	ScaleFont(fac);

	//nXRes = (nXRes + (sizeof(DWORD)-1)) & ~(sizeof(DWORD)-1);	// aligning width to 4 bytes (sizeof(DWORD)) avoids 
	nXRes = nXRes & ~(sizeof(DWORD)-1);							// aligning width to 4 bytes (sizeof(DWORD)) avoids 
																// pixel garbage at the upper line

	// Initialize the bitmap header information...
	memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
	bitmapInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth		= nXRes;
	bitmapInfo.bmiHeader.biHeight		= nYRes;
	bitmapInfo.bmiHeader.biPlanes		= 1;
	bitmapInfo.bmiHeader.biBitCount		= 24;
	bitmapInfo.bmiHeader.biCompression	= BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage	= bitmapInfo.bmiHeader.biWidth * bitmapInfo.bmiHeader.biHeight * 3;
	bitmapInfo.bmiHeader.biXPelsPerMeter = int(72.*fac/.0254);	// 72*fac DPI
	bitmapInfo.bmiHeader.biYPelsPerMeter = int(72.*fac/.0254);	// 72*fac DPI

	// create DIB
	hTmpDC = ::GetDC(m_hWnd);
	hDib = CreateDIBSection(hTmpDC, &bitmapInfo, DIB_RGB_COLORS, &pBitmapBits, NULL, (DWORD)0);

	// create memory device context
	CDC *pDC = new CWindowDC(this);
	//pDC->m_bPrinting = TRUE;	// this does the trick in OnDraw: it prevents changing rendering context and swapping buffers
	if ((hMemDC = CreateCompatibleDC(pDC == NULL ? NULL : pDC->GetSafeHdc())) == NULL)
	{
		DeleteObject(hDib);
		return FALSE;
	}
	HGDIOBJ hOldDib = SelectObject(hMemDC, hDib);

	// Store current context and viewport
	DrawerContext *context = GetDocument()->rootDrawer->getDrawerContext();
	RowCol viewportOld = GetDocument()->rootDrawer->getViewPort();

	// Make hMemDC the current OpenGL rendering context.
	DrawerContext * contextMem = new ILWIS::DrawerContext();
	int contextMode = DrawerContext::mDRAWTOBITMAP;
	if (fSoftwareRendering)
		contextMode |= DrawerContext::mSOFTWARERENDERER;
	contextMem->initOpenGL(hMemDC, 0, contextMode);
	CoordBounds cbZoom = GetDocument()->rootDrawer->getCoordBoundsZoom();
	CoordBounds cbMap = GetDocument()->rootDrawer->getMapCoordBounds();
	CoordBounds cbView = GetDocument()->rootDrawer->getCoordBoundsView();
	GetDocument()->rootDrawer->setDrawerContext(contextMem);
	GetDocument()->rootDrawer->setViewPort(RowCol(nYRes,nXRes), false); // false: autozoom to the given pixelsize
	if (cbClip.fValid()) {
		GetDocument()->rootDrawer->setCoordBoundsView(cbMap, true); // set new aspect ratio
		GetDocument()->rootDrawer->setCoordBoundsZoom(cbClip); // set the cbZoom to cbClip, for correct clipping
	}

	PreparationParameters ppEDITCOPY (ILWIS::NewDrawer::ptOFFSCREENSTART);
	vector<NewDrawer*> drawerList;
	vector<NewDrawer *> allDrawers;
	GetDocument()->rootDrawer->getDrawers(allDrawers);
	int count = allDrawers.size();

	count = allDrawers.size();
	for (int i = 0; i < count; ++i)	{
		ILWIS::NewDrawer *drw = allDrawers[i];
		if ( !drw)
			continue;
		ILWIS::SpatialDataDrawer* spatialDataDrawer = dynamic_cast<ILWIS::SpatialDataDrawer*>(drw);
		if (spatialDataDrawer != 0)	{
			int count2 = spatialDataDrawer->getDrawerCount();
			for (int j = 0; j < count2; ++j) {
				ILWIS::ComplexDrawer* drawer = dynamic_cast<ILWIS::ComplexDrawer*>(spatialDataDrawer->getDrawer(j));
				if (drawer != 0)	{
					drawerList.push_back(drawer); // remember for 2nd loop later on so that we dont need to repeat the dynamic_cast
					drawer->prepare(&ppEDITCOPY);
				}
			}
		} else {
			if ( drw->getId() != "CanvasBackgroundDrawer") {
				drawerList.push_back(drw);
				drw->prepare(&ppEDITCOPY);
			}
		} 
	}

	contextMem->TakeContext();
	GetDocument()->rootDrawer->draw(cbClip.fValid() ? cbClip : cbZoom);
	glFinish();	// Finish all OpenGL commands
	contextMem->ReleaseContext();

	// Restore original context and viewport
	GetDocument()->rootDrawer->setDrawerContext(context);
	GetDocument()->rootDrawer->setViewPort(viewportOld, false); // false: just re-assign the original values
	if (cbClip.fValid()) {
		if (cbMap.width() < cbView.width() && cbMap.height() < cbView.height()) { // we need to oversize cbView; there was "gray" on all 4 sides
			double rFactor = min(cbView.width() / cbMap.width(), cbView.height() / cbMap.height()); // the minimum factor required to "fill" the cbView
			cbMap *= rFactor;
		}
		GetDocument()->rootDrawer->setCoordBoundsView(cbMap, true); // restore aspect ratio and view size
		GetDocument()->rootDrawer->setCoordBoundsZoom(cbZoom); // restore zoom position
	}

	PreparationParameters ppEDITCOPYDONE (ILWIS::NewDrawer::ptOFFSCREENEND);
	for (vector<NewDrawer*>::iterator it = drawerList.begin(); it != drawerList.end(); ++it)
		(*it)->prepare(&ppEDITCOPYDONE);

	// contextMem is no longer needed
	delete contextMem;

	if (OpenClipboard())
	{
		HGLOBAL hClipboardCopy = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(BITMAPINFOHEADER) + bitmapInfo.bmiHeader.biSizeImage);
		if (hClipboardCopy != NULL)
		{
			LPVOID lpClipboardCopy, lpBitmapBitsOffset;
			lpClipboardCopy = ::GlobalLock((HGLOBAL) hClipboardCopy);
			lpBitmapBitsOffset = (LPVOID)((BYTE*)lpClipboardCopy + sizeof(BITMAPINFOHEADER));

			memcpy(lpClipboardCopy, &bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
			memcpy(lpBitmapBitsOffset, pBitmapBits, bitmapInfo.bmiHeader.biSizeImage);
			::GlobalUnlock(hClipboardCopy);

			EmptyClipboard();

			if (SetClipboardData(CF_DIB, hClipboardCopy) != NULL)
			{
				bSuccess = TRUE;
				//if (nReduceResCount > 0)
				//	pApp->SetStatusBarInfo(POLICY_RESOLUTION_REDUCED, (int)round(fac*72.));
			}
			else
			{
				GlobalFree(hClipboardCopy);
			}
			CloseClipboard();
		}
	}
	if (hOldDib != NULL)
		SelectObject(hMemDC, hOldDib);

	// Delete our DIB and device context
	DeleteObject(hDib);
	DeleteDC(hMemDC);

	return bSuccess;
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

BOOL MapPaneView::AddFiles(vector<FileName>& afn, bool asAnimation)
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
		mcd->drAppend(fn, asAnimation ? IlwisDocument::otANIMATION : IlwisDocument::otUNKNOWN);
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
	int pressed = GetKeyState(VK_CONTROL) & 0x8000;
	bool asAnimation = pressed != 0;
	GlobalFree(hnd);
	// if send by SendMessage() prevent deadlock
	BOOL fCalledFromOtherThread = ReplyMessage(0);
	return AddFiles(afn, asAnimation);
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
String s = TR("Loading ...");
CString str = s.c_str();
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
	String sGrf;
	bool fOk;
	MapCompositionDoc* mcd = GetDocument();
	Map mp;
	MapList mpl;
	for (int i = 0; i < mcd->rootDrawer->getDrawerCount(); ++i) {
		NewDrawer* drw = mcd->rootDrawer->getDrawer(i);
		SpatialDataDrawer* bmd = dynamic_cast<SpatialDataDrawer*>(drw);
		BaseMapPtr* bmp = bmd->getBaseMap();
		IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(bmp->fnObj);
		if ( otype == IlwisObject::iotRASMAP) {
			MapPtr * mpptr = dynamic_cast<MapPtr*>(bmp);
			if (mpptr) {
				mp.SetPointer(mpptr);
				IlwisObjectPtr * ioptr = bmd->getObject();
				MapListPtr * mplptr = dynamic_cast<MapListPtr*>(ioptr);
				if (mplptr)
					mpl.SetPointer(mplptr);
				break;
			}
		}
	}
	if (!mp.fValid())
		return;
	GeoRef georef = mp->gr();
	{
		FormCreateGeoRefRC frm(this, &sGrf, mp, georef->cs(), georef->cb(), true);
		fOk = frm.fOkClicked();
	}  
	if (fOk) {
		FileName fn(sGrf);
		fn.sExt = ".grf";
		GeoRef grf(fn);

		mp->SetGeoRef(grf);
		mcd->rootDrawer->setGeoreference(grf, true);
		if (mpl.fValid())
			mpl->SetGeoRef(grf);
		OnGeoRefEdit();
	}
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
			SpatialDataDrawer* bmd = dynamic_cast<SpatialDataDrawer*>(drw);
			if (bmd) {
				BaseMapPtr* bmp = bmd->getBaseMap();
				String s(TR("Replace Coordinate System of %S (now %S) with %S?").c_str(), bmp->sName(true), bmp->cs()->sName(), csy->sName());
				int iRes = MessageBox(s.c_str(), TR("Replace Coordinate System").c_str(), MB_ICONQUESTION|MB_YESNO);
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

LRESULT MapPaneView::OnOpenMap(WPARAM wP, LPARAM lp) {
	String *s = (String *)(void *)wP;
	String filename(*s);
	delete s;
	if ( filename.size() > 0) {
		FileName fn(filename);
		getEngine()->getContext()->SetThreadLocalVar(IlwisAppContext::tlvMAPWINDOWAPP, 0);
		if ( IOTYPEBASEMAP(fn)) {
			GetDocument()->drAppend(fn);
		}
		if ( IOTYPE(fn) == IlwisObject::iotMAPLIST || IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION) {
			if ( ::MessageBox(m_hWnd,TR("Open as Animation").c_str(), TR("Open").c_str(),MB_YESNO) == MB_OK) {
				GetDocument()->drAppend(fn, IlwisDocument::otANIMATION);
			} else {
				GetDocument()->drAppend(fn);
			}
		}
	}
	return TRUE;
}

void MapPaneView::OnUpdateSaveSelection(CCmdUI* pCmdUI) {
}


void MapPaneView::OnSaveSelection() {
	class SaveSelectionForm : public FormWithDest {
	public:
		SaveSelectionForm(CWnd *parent, RootDrawer *rootdrw, vector<Feature*>& feat, String& selectedF, String& out ) :
			FormWithDest(parent, TR("Save Layer selection")), 
				features(feat),
				outname(out),
				selectedFile(selectedF)
			{
			vector<String> names;
			selected=0;
			for(int i = 0; i < rootdrw->getDrawerCount(); ++i) {
				SpatialDataDrawer *dataDrw = dynamic_cast<SpatialDataDrawer *>(rootdrw->getDrawer(i));
				if ( !dataDrw)
					continue;
				IlwisObjectPtr *ptr = dataDrw->getObject();
				if ( IOTYPEFEATUREMAP(ptr->fnObj)) {
					if ( names.size() == 0) {
						String input = ptr->fnObj.sFile + ptr->fnObj.sExt;
						FileName fn = FileName::fnUnique(FileName(input));
						outname = fn.sFile + fn.sExt;
					}
					names.push_back(ptr->fnObj.sFile + ptr->fnObj.sExt);
					drawers.push_back(dataDrw);
				}
			}
			new FieldOneSelectString(root,TR("Layers"),&selected, names);
			FieldString *fs = new FieldString(root, TR("Output map"),&outname);
			fs->SetWidth(80);
			create();
		}

		int exec() {
			FormWithDest::exec();
			if ( selected >= 0) {
				features = drawers[selected]->getSelectedFeatures();
				IlwisObjectPtr *ptr = drawers[selected]->getObject();
				selectedFile = ptr->fnObj.sPhysicalPath();
			}
			return 1;
		}

	private:
		vector<SpatialDataDrawer *> drawers;
		long selected;
		String &outname;
		String &selectedFile;
		vector<Feature *> &features;

	};

	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		vector<Feature *> features;
		String outname;
		String selectedFile;
		SaveSelectionForm frm(this,mcd->rootDrawer, features, selectedFile, outname);
		if ( frm.fOkClicked()) {
			if ( features.size() > 0) {
				SaveFeatures(features, selectedFile, outname);
			}
		}
	}
}

void MapPaneView::SaveFeatures(const vector<Feature *>& features, const String& inFile, const String& outFile) const{
	FileName fnIn(inFile);
	FileName fnOut(outFile);
	CoordBounds bnds;
	for(vector<Feature *>::const_iterator iter = features.begin(); iter != features.end(); ++iter) {
		bnds += (*iter)->cbBounds();
	}
	bnds *= 1.1;
	if ( IOTYPE(fnIn) == IlwisObject::iotPOINTMAP) {
		PointMap pmIn(fnIn);
		PointMap pmFnOut(fnOut,pmIn->cs(), bnds, pmIn->dvrs());
		for(vector<Feature *>::const_iterator iter = features.begin(); iter != features.end(); ++iter) {
			ILWIS::Point *point = CPOINT(*iter);
			Feature *f = pmFnOut->newFeature(point);
			f->PutVal(point->rValue());
		}
		pmFnOut->SetAttributeTable(pmIn->tblAtt());
		pmFnOut->Store();
	}
	if ( IOTYPE(fnIn) == IlwisObject::iotSEGMENTMAP) {
		SegmentMap smIn(fnIn);
		SegmentMap smFnOut(fnOut,smIn->cs(), bnds, smIn->dvrs());
		for(vector<Feature *>::const_iterator iter = features.begin(); iter != features.end(); ++iter) {
			ILWIS::Segment *seg = CSEGMENT(*iter);
			Feature *f = smFnOut->newFeature(seg);
			f->PutVal(seg->rValue());
		}
		smFnOut->SetAttributeTable(smIn->tblAtt());
		smFnOut->Store();
	}
	if ( IOTYPE(fnIn) == IlwisObject::iotPOLYGONMAP) {
		PolygonMap pmIn(fnIn);
		PolygonMap pmFnOut(fnOut,pmIn->cs(), bnds, pmIn->dvrs());
		for(vector<Feature *>::const_iterator iter = features.begin(); iter != features.end(); ++iter) {
			ILWIS::Polygon *pol = CPOLYGON(*iter);
			Feature *f = pmFnOut->newFeature(pol);
			f->PutVal(pol->rValue());
		}
		pmFnOut->SetAttributeTable(pmIn->tblAtt());
		pmFnOut->Store();
	}
}

void MapPaneView::OnCreateSubMap()
{
	MapCompositionDoc* mcd = GetDocument();
	for (int i = 0; i < mcd->rootDrawer->getDrawerCount(); ++i) 
	{
		NewDrawer* drw = mcd->rootDrawer->getDrawer(i);
		SpatialDataDrawer *mpdr = dynamic_cast<SpatialDataDrawer *>(drw);
		BaseMapPtr *mptr = mpdr->getBaseMap();
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(mptr->fnObj);
		CoordBounds cb  = mcd->rootDrawer->getCoordBoundsZoom();
		if ( mpdr && mpdr->isActive() &&  type == IlwisObject::iotRASMAP)
		{
			CoordBounds cbRas = mptr->cs()->cbConv(mcd->rootDrawer->getCoordinateSystem(), cb);
			RowCol rcTop = ((MapPtr *)mptr)->gr()->rcConv(cbRas.cMin);
			RowCol rcBottom = ((MapPtr *)mptr)->gr()->rcConv(cbRas.cMax);
			RowCol rc1(min(rcTop.Row, rcBottom.Row), min(rcTop.Col, rcBottom.Col));
			RowCol rc2(max(rcTop.Row, rcBottom.Row), max(rcTop.Col, rcBottom.Col));


			String sCommand("subras %S %d %d %d %d", mptr->fnObj.sRelativeQuoted(),rc1.Row, rc1.Col, rc2.Row, rc2.Col);

			//FormMapSubMap *frm = new FormMapSubMap(this, sCommand.c_str());
			IlwWinApp()->ExecuteUI(sCommand, this);
		}
		if ( type == IlwisObject::iotPOINTMAP || type == IlwisObject::iotSEGMENTMAP)
		{
			MapCompositionDoc *mcd = GetDocument();
			CoordBounds cbF = mptr->cs()->cbConv(mcd->rootDrawer->getCoordinateSystem(), cb);
			Coord cMin = cbF.cMin, cMax = cbF.cMax;
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

LRESULT MapPaneView::OnSendUpdateAnimMessages(WPARAM p1, LPARAM p2) {
	if ( !p1)
		return 1;

	FileName fn = *(FileName *)(void *)(p1);
	MapCompositionDoc *doc = GetDocument();
	Map mp;
	Map mpCurrent;
	if ( IOTYPE(fn) == IlwisObject::iotMAPLIST) {
		MapList mpl(fn);
		if (!mpl.fValid())
			return 1;
		mp = mpl[p2];
		mpCurrent = mpl[p2 == 0 ? mpl->iSize() - 1 : p2-1];
	} else if (IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc(fn);
		if (!oc.fValid())
			return 1;
		FileName fnp2 = oc->fnObject(p2);
		FileName fnp2curr = oc->fnObject(p2 == 0 ? oc->iNrObjects() - 1 : p2-1);
		if (IOTYPE(fnp2) == IlwisObject::iotRASMAP && IOTYPE(fnp2curr) == IlwisObject::iotRASMAP) {
			mp = Map(fnp2);
			mpCurrent = Map(fnp2curr);
		} else
			return 0;
	}
	HistogramGraphView *hview = doc->getHistoView(mpCurrent->fnObj);
	if ( !hview)
		return 1;

	HistogramGraphDoc* hgd = (HistogramGraphDoc*)hview->GetDocument();
	if (!hgd)
		return 1;
	//for(int i=0; i < doc->rootDrawer->getDrawerCount(); ++i) {
	//	NewDrawer* dr = rootDrawer->getDrawer(i);
	//	SpatialDataDrawer* md = dynamic_cast<SpatialDataDrawer*>(dr);
	//	if (!md)
	//		continue;

	//	if (md->getType() == "AnimationDrawer") {

	//	}
	//}

	TableHistogramInfo thi(mp);
	hgd->OnOpenDocument(thi.tbl());
	doc->replaceHistFile(mpCurrent->fnObj, mp->fnObj);
	hview->OnInitialUpdate();
	hview->Invalidate();

	return 1;
}

void MapPaneView::OnCopyCoord() {

}