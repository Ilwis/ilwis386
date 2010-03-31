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
#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Base\Framewin.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Editors\Editor.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MiniMapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\SpatialReference\GR3D.H"
#include "Client\Editors\Utils\GeneralBar.h"
#include "Client\Editors\Georef\GeoRef3DDirectionPane.h"
#include "Client\Editors\Utils\bevel.h"
#include "Client\Editors\Georef\GeoRef3DEditor.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\Drawers\Grid3DDrawer.h"
#include "Headers\constant.h"
#include "Headers\Hs\GEOREF3D.hs"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Htp\Ilwis.htp"

const int ID_EDIT3DBAR=8564;
const int iDEFSIZE = 280;


BEGIN_MESSAGE_MAP(GeoRef3DEditorBar, CSizingControlBar)
	ON_BN_CLICKED(ID_EXTENDBOUNDARIES, OnExtend)
	ON_BN_CLICKED(ID_SETFROMVIEWPOINT, OnSetViewPoint)	
	ON_WM_SIZE()
END_MESSAGE_MAP()

GeoRef3DEditorBar::GeoRef3DEditorBar() :
	form(NULL),
  mcdMiniMap(NULL),
	mpvMiniMap(NULL)
{
}

GeoRef3DEditorBar::~GeoRef3DEditorBar()
{
//	delete mpvMiniMap;
}

BOOL GeoRef3DEditorBar::Create(MapPaneView *pane, GeoRef3D *grf)
{
	mpMapPane = pane;
  gr3d = grf;

	CFrameWnd* pParent = pane->GetParentFrame();
	ASSERT(pParent);	

	BOOL iRet = CSizingControlBar::Create(S3DTitleGeoRef3DEditor.scVal(), pParent, CSize(iDEFSIZE + 5 ,675), 
		                                    TRUE, ID_EDIT3DBAR, WS_CHILD | WS_VISIBLE | CBRS_TOP | WS_VSCROLL );

	double rFac = (double)grf->mapDTM->rcSize().Row / grf->mapDTM->rcSize().Col;
	double iYSize = rFac > 1.0 ? iDEFSIZE : iDEFSIZE * rFac;	// keeping the size down
	double iXSize = rFac > 1.0 ? iDEFSIZE / rFac: iDEFSIZE - 20;

	CRect rct;	

	CClientDC cdc(this);
	CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
	CFont* fntOld = cdc.SelectObject(fnt);
	CSize sz  = cdc.GetTextExtent(CString(S3DTitleExtendBoundaries.scVal()));
	cdc.SelectObject(fntOld);	
	
	butExtend.Create(S3DTitleExtendBoundaries.scVal(), WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON, 
		               CRect(5, 10, sz.cx + 30, sz.cy + 20), this, ID_EXTENDBOUNDARIES);

	CSize sz2  = cdc.GetTextExtent(CString(S3DTitleViewPoint.scVal()));
	butViewPoint.Create(S3DTitleViewPoint.scVal(), WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON, 
		               CRect(sz.cx + 30, 10, sz.cx  + sz2.cx + 60, sz.cy + 20), this, ID_SETFROMVIEWPOINT);	

	butExtend.SetFont(fnt);
	butViewPoint.SetFont(fnt);

	mcdMiniMap = new MapCompositionDoc;
	mcdMiniMap->OnOpenDocument(grf->mapDTM->fnObj.sFullPath().scVal(), IlwisDocument::otNOASK);

	mpvMiniMap = new GeoRef3DDirectionPane(grf, this);

	CRect rctLoc(5, sz.cy + 25, iXSize + 5, sz.cy + iYSize + 30 );
	mpvMiniMap->SetLocation(rctLoc); 		
	mpvMiniMap->CView::Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rctLoc, this, 100, 0);

	mcdMiniMap->AddView(mpvMiniMap);
	mpvMiniMap->OnInitialUpdate();
	mpvMiniMap->GetClientRect(&rct);
	stSeperator.Create(this, CRect(5, rct.bottom + sz.cy + 35, iDEFSIZE - 20, rct.bottom + sz.cy + 37), false );
	//GetClientRect(&rct);
	rctLoc = CRect(0, rct.bottom + sz.cy + 38, iDEFSIZE , rct.bottom + sz.cy + 380);	
  form = new GeoRef3DForm(pane, grf, this);				
	form->Create(NULL, "name", WS_CHILD | WS_VISIBLE, rctLoc, this, 278);
	form->CreateForm();


	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	SetWindowText(S3DTitleGeoRef3DEditor.scVal());
	return iRet;
}

void GeoRef3DEditorBar::SetValue(int items, ValueToUpdate v, double rValue1, double rValue2)
{
	Coord crd(rValue1, rValue2);
	if ( items & ituFORM)	
	{
		switch(v)
		{
			case	vtuVIEWPOINT:
				form->fcLoc->SetVal(crd);
				gr3d->SetViewPoint( crd);
				break;
			case vtuFOCALPOINT:
				form->fcView->SetVal(crd);
				gr3d->SetFocalPoint(crd);
				break;
		}			
	}	
	if ( items & ituMINIPANE )
	{
		switch ( v)
		{
			case	vtuVIEWPOINT:
				mpvMiniMap->SetViewPoint(crd); 				
				gr3d->SetViewPoint( crd);
				break;
			case vtuFOCALPOINT:
				mpvMiniMap->SetFocalPoint(crd); 			
				gr3d->SetFocalPoint(crd);
				break;
		}			
	}		
}

void GeoRef3DEditorBar::OnExtend()
{
	MapCompositionDoc *doc = mpvMiniMap->GetDocument();
	doc->OnExtPerc();
	mpvMiniMap->OnEntireMap();
	mpvMiniMap->Invalidate();
}

MapPaneView *GeoRef3DEditorBar::MainPane()
{
	return mpMapPane;
}

void GeoRef3DEditorBar::RedrawMiniMap()
{
	mpvMiniMap->Invalidate();
}

void GeoRef3DEditorBar::OnSetViewPoint()
{
	mpvMiniMap->OnSetFromViewPoint();
}

void GeoRef3DEditorBar::OnSize( UINT nType, int cx, int cy )
{

	if ( form && form->GetSafeHwnd() )
	{
		CRect rct;
		form->GetClientRect(&rct);
		rct.bottom = cy;
		rct.right = cx;
		form->SetWindowPos(NULL , 0,0, cx, cy - iDEFSIZE + 10, SWP_NOMOVE );
	}		

}

//--------------------------------------------------------
#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 

BEGIN_MESSAGE_MAP(GeoRef3DEditor, Editor)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
	ON_COMMAND(ID_SETFROMVIEWPOINT, OnSetViewPoint)
	ON_COMMAND(ID_SETTOVIEWPOINT, OnSetFocalPoint)
	ON_COMMAND(ID_GRF3DBAR, OnGrd3DBar)
	ON_UPDATE_COMMAND_UI(ID_GRF3DBAR, OnUpdateGrd3DBar)
//	ON_WM_LBUTTONUP()
//	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

GeoRef3DEditor::GeoRef3DEditor(MapPaneView *pane) :
    Editor(pane),
		fSetFocalPoint(false) 	
{
  grf = dynamic_cast<GeoRef3D*> (pane->GetDocument()->georef.ptr());
  if (!grf) 
	{
    fOk=false;
    return;			
  }		
	CFrameWnd *wnd = pane->GetDocument()->wndGetActiveView()->GetTopLevelFrame();
	bar3DEdit.Create(pane, grf);


	wnd->DockControlBar(&bar3DEdit,AFX_IDW_DOCKBAR_RIGHT);
	wnd->ShowControlBar(&bar3DEdit,TRUE,FALSE);
  //((BaseWindow*)(mappane->parent()))->SetTitle(pane->georef);
  String s(S3DTitleGeoRef3DEditor_s.scVal(), grf->sName());
  pane->GetParentFrame()->SetWindowText(s.scVal());


	CMenu men;
	men.CreateMenu();
  add(ID_CONFIGURE);	
	men.AppendMenu(MF_SEPARATOR);	
	add(ID_EXITEDITOR);
	hmenFile = men.GetSafeHmenu();
	men.Detach();
	men.CreateMenu();
	add(ID_SETTOVIEWPOINT);		
	hmenEdit = men.GetSafeHmenu();
	men.Detach();

	if (wnd) {
		CMenu* men = wnd->GetMenu();
		int iNr = men->GetMenuItemCount();
		CMenu* menOptions = men->GetSubMenu(iNr-2);
		menOptions->AppendMenu(MF_STRING, ID_GRF3DBAR, sMen(ID_GRF3DBAR)); 
	}	

	DataWindow* dw = mpv->dwParent();
	ButtonBar& bb = dw->bbDataWindow;
	bb.LoadButtons("gr3dedit.but");	
	UpdateMenu();
	//CRect rct;
	//bar3DEdit.GetWindowRect(&rct);
	CRect rctWindow;
	wnd->GetWindowRect(&rctWindow);
	//rctWindow.bottom = rctWindow.top + 730;
	int iW = GetSystemMetrics(SM_CXSCREEN);
	int iH = GetSystemMetrics(SM_CYSCREEN);
	rctWindow.left  = 100;
	rctWindow.right = 100 + iW * 0.85;
	rctWindow.top = 75;
	rctWindow.bottom = 75 + iH * 0.85;

	wnd->MoveWindow(rctWindow);
	pane->OnEntireMap();

  htpTopic = htpGeoRef3DEditor;
	sHelpKeywords = "Georeference 3D editor";		
}

GeoRef3DEditor::~GeoRef3DEditor()
{
	// Clean up the "3D Bar" menu item
	CFrameWnd *wnd = pane()->GetDocument()->wndGetActiveView()->GetTopLevelFrame();
	if (wnd)
	{
		CMenu* men = wnd->GetMenu();
		if (men)
			men->DeleteMenu(ID_GRF3DBAR, MF_BYCOMMAND); 
	}	

  grf->Updated();
}

int GeoRef3DEditor::Update(Event *ev)
{
	return 0;
}

int GeoRef3DEditor::draw(CDC *dc, zRect, Positioner* psn, volatile bool *)
{ 
  return 0;
}



void GeoRef3DEditor::Copy()
{}

void GeoRef3DEditor::Clear()
{}

void GeoRef3DEditor::Paste()
{}

String GeoRef3DEditor::sTitle() const
{
	return grf->sTypeName();
}

void GeoRef3DEditor::OnConfigure()
{
	GeoRef3DConfigForm frm(&bar3DEdit);
	if ( frm.fOkClicked() )
		bar3DEdit.RedrawMiniMap();
		
}

void GeoRef3DEditor::OnSetViewPoint()
{
	fSetFocalPoint = false;
	bar3DEdit.OnSetViewPoint();
}

void GeoRef3DEditor::OnSetFocalPoint()
{
	fSetFocalPoint = true;
}

bool GeoRef3DEditor::OnLButtonUp(UINT nFlags, CPoint point) 
{
	bool fret = Editor::OnLButtonUp(nFlags, point);
	if ( fSetFocalPoint)
	{
		Coord crdFoc = bar3DEdit.MainPane()->crdPnt(point);
		bar3DEdit.SetValue(GeoRef3DEditorBar::ituMINIPANE, GeoRef3DEditorBar::vtuFOCALPOINT, crdFoc.x, crdFoc.y);				
		bar3DEdit.SetValue(GeoRef3DEditorBar::ituFORM, GeoRef3DEditorBar::vtuFOCALPOINT, crdFoc.x, crdFoc.y);	
		CDocument *doc2 = bar3DEdit.MainPane()->GetDocument();
		doc2->UpdateAllViews(0);
		bar3DEdit.RedrawMiniMap();		
		fSetFocalPoint = false;
		//SetCursor(LoadCursor(IlwWinApp()->m_hInstance, IDC_ARROW));		
	}

	return fret;
}

void GeoRef3DEditor::OnGrd3DBar()
{
	CFrameWnd* fw = mpv->GetTopLevelFrame();
	if (0 == fw)
		return;
	if (bar3DEdit.IsWindowVisible())
		fw->ShowControlBar(&bar3DEdit,FALSE,FALSE);
	else
		fw->ShowControlBar(&bar3DEdit,TRUE,FALSE);	
}

void GeoRef3DEditor::OnUpdateGrd3DBar(CCmdUI* pCmdUI)
{
	if (!IsWindow(bar3DEdit.m_hWnd))
		return;
	bool fCheck = bar3DEdit.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);	
}

bool GeoRef3DEditor::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CMenu men;
	men.CreatePopupMenu();
	addmen(ID_NORMAL);
	addmen(ID_ZOOMIN);
	addmen(ID_ZOOMOUT);
	addmen(ID_PANAREA);
  men.AppendMenu(MF_SEPARATOR);
  addmen(ID_CONFIGURE);
	addmen(ID_SETTOVIEWPOINT);
  men.AppendMenu(MF_SEPARATOR);	
  addmen(ID_EXITEDITOR);
 men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);

 return true;
}	


//------------------------------------------------------------------------------
GeoRef3DForm::GeoRef3DForm(MapPaneView *pn, GeoRef3D* grf, GeoRef3DEditorBar *_bar) 
:   FormBaseView(),
    gr3d(grf),
    pane(pn),
		bar(_bar)
{
}

void GeoRef3DForm::CreateForm()
{
		fbs |= fbsNOCANCELBUTTON | fbsNOOKBUTTON | fbsNOBEVEL;
    frViewAngle = new FieldReal(root, S3DUiViewingAngle, &gr3d->rAngle, ValueRangeReal(0,180,0.01));
    frViewAngle->SetCallBack((NotifyProc)&GeoRef3DForm::CallBack);
    frHorRot = new FieldReal(root, S3DUiHorRot, &gr3d->rPhi, ValueRange(-360, 360, 0.01));
    frHorRot->SetCallBack((NotifyProc)&GeoRef3DForm::RotCallBack);
    frVerRot = new FieldReal(root, S3DUiVerRot, &gr3d->rTheta, ValueRange(-360, 360, 0.01));
    frVerRot->SetCallBack((NotifyProc)&GeoRef3DForm::RotCallBack);		
    frDistance = new FieldReal(root, S3DUiDistance, &gr3d->rDistance, ValueRange(0, 1e6, 0.01));
    frDistance->SetCallBack((NotifyProc)&GeoRef3DForm::RotCallBack);		
    frScaleHeight = new FieldReal(root, S3DUiScaleHeight, &gr3d->rScaleH, ValueRange(0, 10, 0.1));
    frScaleHeight->SetCallBack((NotifyProc)&GeoRef3DForm::CallBack);
    fcView = new FieldCoord(root, S3DUiViewCoordsX, &gr3d->cView);
    fcView->SetCallBack((NotifyProc)&GeoRef3DForm::LocCallBack);	
		fcView->SetWidth(35);
		fcLoc = new FieldCoord(root, S3DUiLocCoordsX, &gr3d->cLoc);
		fcLoc->SetWidth(35);
		fcLoc->SetCallBack((NotifyProc)&GeoRef3DForm::LocCallBack);				
    frLocHeight=new FieldReal(root, S3DUiLocHeight, &gr3d->rLocH, ValueRange(0, 1e6, 0.01));
    frLocHeight->SetCallBack((NotifyProc)&GeoRef3DForm::LocCallBack);
		new FieldSeparatorLine(root, iDEFSIZE - 30, FormEntry::bsLOWERED);
		new PushButton(root, S3DUiDisplayOptions, (NotifyProc)&GeoRef3DForm::DispOptCallBack);

    fInsideCallback = false;
		create();

 //   SetMenHelpTopic(htpGrf3DEditForm);
}

GeoRef3DForm::~GeoRef3DForm()
{
}

int GeoRef3DForm::Grid(Event *ev)
{
		MapCompositionDoc *doc =  pane->GetDocument();
    for(list<Drawer*>::iterator cur = doc->dl.begin(); cur != doc->dl.end(); ++cur) 
      if (dynamic_cast<Grid3DDrawer*>(*cur) )
        (*cur)->Configure();
    return 0;
}

int GeoRef3DForm::GridDistCallBack(Event *ev)
{
	// This callback is not used anymore ???
	root->StoreData();
	/*
	MapCompositionDoc *doc =  pane->GetDocument();			
	for(list<Drawer*>::iterator cur = doc->dl.begin(); cur != doc->dl.end(); ++cur)
	{
		Grid3DDrawer *dr = dynamic_cast<Grid3DDrawer*>(*cur);
		if (dr )
		{
			dr->SetDisplayOptions(_fGrid, _rDist, _fDrape, _iFillSize);
		}			
	}
	*/
	pane->PostMessage(WM_COMMAND, ID_REDRAW, 0);
	return 0;
}

int GeoRef3DForm::DispOptCallBack(Event *ev)
{
	MapCompositionDoc *doc =  pane->GetDocument();				
	for(list<Drawer*>::iterator cur = doc->dl.begin(); cur != doc->dl.end(); ++cur)
	{
		Grid3DDrawer *dr = dynamic_cast<Grid3DDrawer*>(*cur);
		if (dr )
		{
			if ( dr->Configure() )
				pane->PostMessage(WM_COMMAND, ID_REDRAW, 0);						
		}
	}
	return 1;
}

int GeoRef3DForm::CallBack(Event*)
{
  root->StoreData();
  gr3d->CalcTrig();
  return 0;
}

int GeoRef3DForm::RotCallBack(Event*)
{
  if (fInsideCallback)
    return 0;
  fInsideCallback = true;  
  fcView->StoreData();
  frHorRot->StoreData();
  frVerRot->StoreData();
  frDistance->StoreData();
  gr3d->ComputeLoc();
  fcLoc->SetVal(gr3d->cLoc);
  frLocHeight->SetVal(gr3d->rLocH);
  fInsideCallback = false;  
  return 0;
}

int GeoRef3DForm::LocCallBack(Event*)
{
  if (fInsideCallback)
    return 0;
  fInsideCallback = true;  
  fcLoc->StoreData();
  frLocHeight->StoreData();
  gr3d->ComputeRot();
  frHorRot->SetVal(gr3d->rPhi);
  frVerRot->SetVal(gr3d->rTheta);
  frDistance->SetVal(gr3d->rDistance);
  fInsideCallback = false;  
  return 0;
}

int GeoRef3DForm::Redraw(Event *ev)
{
  pane->Invalidate();
  return 0;
}

int GeoRef3DForm::exec()
{ 
  root->StoreData();
  gr3d->fChanged = true;
  pane->GetDocument()->wndGetActiveView()->PostMessage(WM_COMMAND, ID_NONEEDIT, 0);
  return 0;
}

BOOL GeoRef3DForm::PreTranslateMessage(MSG* pMsg)
{
  if (pMsg->message == WM_KEYDOWN )
	{
		if (pMsg->wParam == VK_RETURN)
		{
			root->StoreData();

			Coord crdLoc = gr3d->crdViewPoint();
			Coord crdFoc = gr3d->crdFocalPoint();

			bar->SetValue(GeoRef3DEditorBar::ituMINIPANE, GeoRef3DEditorBar::vtuVIEWPOINT, crdLoc.x, crdLoc.y);
			bar->SetValue(GeoRef3DEditorBar::ituMINIPANE, GeoRef3DEditorBar::vtuFOCALPOINT, crdFoc.x, crdFoc.y);				

			pane->PostMessage(WM_COMMAND, ID_REDRAW, 0);
		}
	}
  return FormBaseView::PreTranslateMessage(pMsg);
}

//--------------------------------------------------------------------
GeoRef3DConfigForm::GeoRef3DConfigForm(CWnd *par) :
	FormWithDest(par, S3DTitleConfigure)
{
	IlwisSettings settings("GeoRef3DEditor");
	clrViewAxis = settings.clrValue("ViewAxis", RGB(255,255,255));
	clrAngle = settings.clrValue("ViewAngle", RGB(0,255,0));	
	
	new FieldColor(root, S3DUiViewAxisColor, &clrViewAxis);
	new FieldColor(root, S3DUiViewAngleColor, &clrAngle);	

	create();
}

int GeoRef3DConfigForm::exec()
{
	FormWithDest::exec();
	IlwisSettings settings("GeoRef3DEditor");
	settings.SetValue("ViewAxis", clrViewAxis);
	settings.SetValue("ViewAngle", clrAngle);	

	return 1;
}
