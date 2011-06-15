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
// StereoscopeWindow.cpp: implementation of the StereoscopeWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Stereoscopy\StereoscopeWindow.h"
#include "Client\Editors\Stereoscopy\StereoscopePaneView.h"
#include "Engine\Stereoscopy\StereoPairEpiPolar.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Base\WinThread.h"
#include "Headers\Hs\stereoscopy.hs"
#include "Headers\constant.h"
#include "Client\Mapwindow\MapStatusBar.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\OverviewMapPaneView.h"
#include "Headers\Hs\Mapwind.hs"
#include "Headers\Htp\Ilwis.htp"
#include "Headers\Htp\Ilwismen.htp"

IMPLEMENT_DYNCREATE(StereoscopeWindow, DataWindow)

BEGIN_MESSAGE_MAP(StereoscopeWindow, DataWindow)
	ON_WM_CREATE()
	ON_WM_INITMENU()
	ON_WM_SIZING()
	ON_WM_MOVING()
	ON_MESSAGE(ILW_UPDATE, OnUpdate)
	ON_COMMAND(ID_OPENSTEREOPAIR, OnOpenStereoPair)
	ON_COMMAND(ID_EXITSTEREOSCOPE, OnClose)
	ON_COMMAND(ID_TOOLBAR_STEREOSCOPE, OnStereoscopeBar)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_STEREOSCOPE, OnUpdateStereoscopeBar)
	ON_COMMAND(ID_LEFTRIGHT_STEREOSCOPY, OnLeftRightStereoBar)
	ON_UPDATE_COMMAND_UI(ID_LEFTRIGHT_STEREOSCOPY, OnUpdateLeftRightStereoBar)
	ON_COMMAND(ID_ACTIVATE_LEFT, OnActivateLeftView)
	ON_UPDATE_COMMAND_UI(ID_ACTIVATE_LEFT, OnUpdateActiveLeftView)
	ON_COMMAND(ID_ACTIVATE_RIGHT, OnActivateRightView)
	ON_UPDATE_COMMAND_UI(ID_ACTIVATE_RIGHT, OnUpdateActiveRightView)
	ON_COMMAND(ID_UNLOCK_HSCROLL, OnUnlockHScroll)
	ON_UPDATE_COMMAND_UI(ID_UNLOCK_HSCROLL, OnUpdateUnlockHScroll)
	ON_UPDATE_COMMAND_UI(ID_POINTEDIT, OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_SEGEDIT, OnUpdateEdit)
	ON_COMMAND(ID_LAYERMANAGE, OnLayerManagement)
	ON_UPDATE_COMMAND_UI(ID_LAYERMANAGE, OnUpdateLayerManagement)
	ON_COMMAND(ID_OVERVIEW, OnOverviewWindow)
	ON_UPDATE_COMMAND_UI(ID_OVERVIEW, OnUpdateOverviewWindow)
	ON_UPDATE_COMMAND_UI(ID_CREATEPNTMAP, OnUpdateMapOperation)
	ON_UPDATE_COMMAND_UI(ID_CREATESEGMAP, OnUpdateMapOperation)
	ON_UPDATE_COMMAND_UI(ID_CREATEGRF, OnUpdateMapOperation)
	ON_UPDATE_COMMAND_UI(ID_CREATECSY, OnUpdateMapOperation)
	ON_UPDATE_COMMAND_UI(ID_OPENPIXELINFO, OnUpdateMapOperation)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StereoscopeWindow::StereoscopeWindow()
: vwLeft(0), vwRight(0), docLeft(0), docRight(0)
, _fXoffsetLocked(true), _iXoffsetDelta(0), iWindowMiddle(0)
, fMasterLocked(false)
{
	fStoreToolBars = false; // overrule FrameWindow default: we dont want our toolbars to be remembered
  docLeft = new MapCompositionDoc;
  docRight = new MapCompositionDoc;
  Create(NULL, "");

	//vwLeft->ShowScrollBars(false);

	WinThread* wt = dynamic_cast<WinThread*>(AfxGetThread());
  String str = wt->sFileName();
  FileName fn(str);
	// In this "if" statement we're playing with InitialUpdate so that the window does not
	// appear yet while the user is looking at tranquilizers that calculate the output maps
	// InitialUpdate is guaranteed to be called once and only once, except when the user
	// cancels the OpenStereoPair form or the stereo pair attempted to open is invalid.
	if (fn.sExt == ".stp")
	{
		stp = StereoPair(fn);
		if (stp.fValid())
		{
			if (!stp->fCalculated())
			{
		    int iPrior = AfxGetThread()->GetThreadPriority();
		    AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
				stp->Calc();
		    AfxGetThread()->SetThreadPriority(iPrior);
			}

			// following check to be improved (why is it even necessary?)
			if (stp->fCalculated())
			{
				InitialUpdate(0,TRUE); // window will appear
				if (stp->mapLeft.fValid() && stp->mapRight.fValid()) // for fDependent()=false
					RefreshMaps(stp->mapLeft->fnObj.sFullName(), stp->mapRight->fnObj.sFullName());
			}
			else
				PostMessage(WM_CLOSE, 0, 0); // window will not appear; resources will be cleaned up
		}
		else
			PostMessage(WM_CLOSE, 0, 0); // window will not appear; resources will be cleaned up
		StereoPairUpdated();
	}
	else
	{
		StereoPairUpdated(); // in case user cancels, we wanna be in a "resetted" state
		OnOpenStereoPair();
		if (stp.fValid())
			InitialUpdate(0,TRUE); // window will appear
		else
			PostMessage(WM_CLOSE, 0, 0); // window will not appear; resources will be cleaned up
	}

	// InitialUpdate(0,TRUE) was here
	
	zIcon icon("StereoPairIcon");
	SetIcon(icon, TRUE);
  help = "ilwis\\stereopair_stereoscope_window.htm";
	sHelpKeywords = "Stereo pairs (display)";

	// The +1 is to make sure the splitter window can always be divided into two
	// equal panes (the encapsulating window must have an odd width)
	CRect rect;
	GetWindowRect(&rect);
	if (rect.Width() % 2 == 0)
		MoveWindow(rect.left, rect.top, rect.Width()+1, rect.Height());

	// Initialize the window middle so that it doesn't occur that one sizes before the middle is set
	GetWindowRect(&rect);
	iWindowMiddle = (rect.left + rect.right) / 2;
}

StereoscopeWindow::~StereoscopeWindow()
{

}

#define sMen(ID) ILWSF("men",ID).scVal()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int StereoscopeWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	MapStatusBar* msb = new MapStatusBar;
	status = msb;
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	// Here the vwLeft and vwRight already got their final values
  docLeft->AddView(vwLeft);
  docRight->AddView(vwRight);
	msb->Create(this);
	EnableDocking(CBRS_ALIGN_ANY);

  CMenu men;
	CMenu menPopup;
	CMenu menSub;
  men.CreateMenu();
  menPopup.CreateMenu();
	add(ID_OPENSTEREOPAIR);
  addBreak;
		menSub.CreateMenu();
	  addSub(ID_CREATEPNTMAP);
	  addSub(ID_CREATESEGMAP);
		addSub(ID_CREATEGRF);
	  addSub(ID_CREATECSY);
	  addSubMenu(ID_FILE_CREATE);
	addBreak;
	menPropLayer.CreateMenu();
  menPopup.AppendMenu(MF_POPUP, (UINT)menPropLayer.GetSafeHmenu(), sMen(ID_PROPLAYER)); 	
  add(ID_OPENPIXELINFO);
  addBreak;
  add(ID_EXITSTEREOSCOPE);
  addMenu(ID_MEN_FILE);

	menPopup.CreateMenu();
	menEditLayer.CreateMenu();
  menPopup.AppendMenu(MF_POPUP, (UINT)menEditLayer.GetSafeHmenu(), sMen(ID_EDITLAYER)); 	
    menSub.CreateMenu();
    addSub(ID_POINTEDIT);
    addSub(ID_SEGEDIT);
		addSubMenu(ID_EDITOR);
	addBreak;

	//add(ID_EDITGRF);
  add(ID_EDITCSY);
	hMenEdit = menPopup.GetSafeHmenu();
  addMenu(ID_MEN_EDIT);
	
	menPopup.CreateMenu();
	add(ID_ADDLAYER);
	add(ID_ADD_GRID);
	add(ID_ADD_GRATICULE);
	add(ID_REMOVELAYER);
	menDataLayer.CreateMenu();
  menPopup.AppendMenu(MF_POPUP, (UINT)menDataLayer.GetSafeHmenu(), sMen(ID_LAYEROPTIONS)); 	

	addMenu(ID_MEN_LAYERS);
	
	menPopup.CreateMenu();
  add(ID_ENTIREMAP);
	add(ID_NORMAL);
  add(ID_ZOOMIN);
  add(ID_ZOOMOUT);
	add(ID_PANAREA);
  add(ID_SCALE1);
  addBreak;
  add(ID_REDRAW);
  add(ID_MEASUREDIST);
  addBreak;

  add(ID_LAYERMANAGE);
  add(ID_OVERVIEW);
  add(ID_BUTTONBAR);
	add(ID_LEFTRIGHT_STEREOSCOPY);
  add(ID_STATUSLINE);
  addMenu(ID_MEN_OPTIONS);

  menPopup.CreateMenu();
  add(ID_HLPKEY);
	add(ID_HELP_RELATED_TOPICS);
	addBreak
  add(ID_HLPCONTENTS);
  add(ID_HLPINDEX);
  add(ID_HLPSEARCH);
	addBreak
	add(ID_ABOUT)
  addMenu(ID_MEN_HELP);

  SetMenu(&men);
  menPopup.Detach();
  men.Detach();

	SetAcceleratorTable();
	bbStereoscope.Create(this, "stereoscope.but", SStcUiStereoscope, 100); // contains steropair specific buttons
	bbStereoscope.EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	bbDataWindow.Create(this, "stereoscopeGen.but", SStcUiStereoToolBar, 101); // contains general buttons (zoom etc)
	bbDataWindow.EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);

	DockControlBar(&bbDataWindow, AFX_IDW_DOCKBAR_TOP);
	RecalcLayout();
	CRect rect;
	bbDataWindow.GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	DockControlBar(&bbStereoscope, AFX_IDW_DOCKBAR_TOP, rect);
	//barScale.Create(this);
	RecalcLayout();
	bbStereoscope.GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	//DockControlBar(&barScale,AFX_IDW_DOCKBAR_TOP,rect);

	ltbLeft.Create(this, 124, CSize(150,200));
  ltbLeft.SetWindowText(SMWTitleLayerManagement.scVal());
	ltbLeft.EnableDocking(CBRS_ALIGN_LEFT);
  ltbLeft.view = new LayerTreeView;
	ltbLeft.view->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
			CRect(0,0,0,0), &ltbLeft, 100, 0);
	docLeft->AddView(ltbLeft.view);
	ltbRight.Create(this, 124, CSize(150,200));
  ltbRight.SetWindowText(SMWTitleLayerManagement.scVal());
	ltbRight.EnableDocking(CBRS_ALIGN_RIGHT);
  ltbRight.view = new LayerTreeView;
	ltbRight.view->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
			CRect(0,0,0,0), &ltbRight, 100, 0);
	docRight->AddView(ltbRight.view);
	
  // half of the requested height will be assigned
	gbOverviewLeft.Create(this, 125, CSize(150,300));
  gbOverviewLeft.m_szFloat = CSize(150,150);
	gbOverviewLeft.EnableDocking(CBRS_ALIGN_LEFT);
  gbOverviewLeft.view = new OverviewMapPaneView;
  gbOverviewLeft.view->Create(NULL,NULL,AFX_WS_DEFAULT_VIEW,CRect(0,0,0,0),&gbOverviewLeft,100.0);
  gbOverviewLeft.SetWindowText(SMWTitleOverviewWindow.scVal());
	docLeft->AddView(gbOverviewLeft.view);
	gbOverviewRight.Create(this, 125, CSize(150,300));
  gbOverviewRight.m_szFloat = CSize(150,150);
	gbOverviewRight.EnableDocking(CBRS_ALIGN_RIGHT);
  gbOverviewRight.view = new OverviewMapPaneView;
  gbOverviewRight.view->Create(NULL,NULL,AFX_WS_DEFAULT_VIEW,CRect(0,0,0,0),&gbOverviewRight,100.0);
  gbOverviewRight.SetWindowText(SMWTitleOverviewWindow.scVal());
	docRight->AddView(gbOverviewRight.view);

	DockControlBar(&ltbLeft,AFX_IDW_DOCKBAR_LEFT);
  RecalcLayout();
  ltbLeft.GetWindowRect(&rect);
	rect.OffsetRect(0,1);
	DockControlBar(&gbOverviewLeft,AFX_IDW_DOCKBAR_LEFT,rect);
	ShowControlBar(&ltbLeft,FALSE,FALSE);
	ShowControlBar(&gbOverviewLeft,FALSE,FALSE);
	// RecalcLayout();
	DockControlBar(&ltbRight,AFX_IDW_DOCKBAR_RIGHT); 
	RecalcLayout();
  ltbRight.GetWindowRect(&rect);
	rect.OffsetRect(0,1);
	DockControlBar(&gbOverviewRight,AFX_IDW_DOCKBAR_RIGHT,rect);
	ShowControlBar(&ltbRight,FALSE,FALSE);
	ShowControlBar(&gbOverviewRight,FALSE,FALSE);

	RecalcLayout();

	SetWindowName("Stereoscope Window");

	return 0;
}

BOOL StereoscopeWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
 	CCreateContext contextT;
  pContext = &contextT;
	wndSplitter.CreateStatic(this, 1, 2, WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL);
	wndSplitter.CreateView(0, 0, RUNTIME_CLASS(StereoscopePaneView), CSize(400, 500), pContext);
	wndSplitter.CreateView(0, 1, RUNTIME_CLASS(StereoscopePaneView), CSize(400, 500), pContext);
  vwLeft = (StereoscopePaneView*)wndSplitter.GetPane(0,0);
  vwRight = (StereoscopePaneView*)wndSplitter.GetPane(0,1);
	vwLeft->SetLeft(true);
	vwRight->SetLeft(false);
	vwLeft->SetSiblingPane(vwRight);
	vwRight->SetSiblingPane(vwLeft);

	SetActiveView(vwLeft);

	return TRUE;
}

class OpenStereoObjectForm: public FormWithDest
{
public:
  OpenStereoObjectForm(CWnd* wPar, String* sStereoPairName)
    : FormWithDest(wPar, SStcTitleOpenMapPair)    
  {
		new FieldDataType(root, SStcUiStereoPair, sStereoPairName, ".stp", true);
    SetMenHelpTopic("ilwismen\\open_stereopair.htm");
    create();    
  }    
};

void StereoscopeWindow::OnOpenStereoPair()
{
	String sStereoPairName;
	OpenStereoObjectForm frm(this, &sStereoPairName);
	if (frm.fOkClicked())
	{
		stp = StereoPair(FileName(sStereoPairName, ".stp"));
		if (stp.fValid())
		{
			if (!stp->fCalculated())
			{
		    int iPrior = AfxGetThread()->GetThreadPriority();
		    AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
				stp->Calc();
		    AfxGetThread()->SetThreadPriority(iPrior);
			}
			if (stp->fCalculated())
				if (stp->mapLeft.fValid() && stp->mapRight.fValid()) // for fDependent()=false
					RefreshMaps(stp->mapLeft->fnObj.sFullName(), stp->mapRight->fnObj.sFullName());
		}
		StereoPairUpdated();
	}
}

void StereoscopeWindow::OnActivateLeftView()
{
	SetActiveView(vwLeft);
}

void StereoscopeWindow::OnUpdateActiveLeftView(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetActiveView() == vwLeft);
}

void StereoscopeWindow::OnActivateRightView()
{
	SetActiveView(vwRight);
}

void StereoscopeWindow::OnUpdateActiveRightView(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetActiveView() == vwRight);
}

void StereoscopeWindow::RefreshMaps(const String& sLeftMap, const String& sRightMap)
{
  docLeft->OnOpenDocument(sLeftMap.scVal(), IlwisDocument::otNOASK);
  docRight->OnOpenDocument(sRightMap.scVal(), IlwisDocument::otNOASK);
	docLeft->UpdateAllViews(0,3); // entiremap to left views
	docRight->UpdateAllViews(0,3); // entiremap to right views
}

void StereoscopeWindow::StereoPairUpdated()
{
	String sTitle = SStcTitleStereoPairScopeView;
	if (stp.fValid())
		//sTitle = String ("%S - ", stp->sName()) + sTitle;
		sTitle = stp->sDescription + sTitle;
	SetWindowText(sTitle.sVal());
}

void StereoscopeWindow::OnClose()
{
  docLeft->RemoveView(vwLeft);
  docRight->RemoveView(vwRight);
	DataWindow::OnClose();
}

void StereoscopeWindow::OnStereoscopeBar()
{
	if (bbDataWindow.IsWindowVisible())
		ShowControlBar(&bbDataWindow,FALSE,FALSE);
	else
		ShowControlBar(&bbDataWindow,TRUE,FALSE);
}

void StereoscopeWindow::OnUpdateStereoscopeBar(CCmdUI* pCmdUI)
{
	bool fCheck = bbDataWindow.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void StereoscopeWindow::OnLeftRightStereoBar()
{
	if (bbStereoscope.IsWindowVisible())
		ShowControlBar(&bbStereoscope,FALSE,FALSE);
	else
		ShowControlBar(&bbStereoscope,TRUE,FALSE);
}

void StereoscopeWindow::OnUpdateLeftRightStereoBar(CCmdUI* pCmdUI)
{
	bool fCheck = bbStereoscope.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void StereoscopeWindow::OnUnlockHScroll()
{
	_fXoffsetLocked = !_fXoffsetLocked;
}

void StereoscopeWindow::OnUpdateUnlockHScroll(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!_fXoffsetLocked); // pressed when unlocked
}

void StereoscopeWindow::OnUpdateEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(stp.fValid() && stp->fCalculated() && vwLeft->editGet() == 0 && vwRight->editGet() == 0);
}

void StereoscopeWindow::OnUpdateMapOperation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(stp.fValid() && stp->fCalculated());
}

bool StereoscopeWindow::fXoffsetLocked()
{
	return _fXoffsetLocked;
}

int StereoscopeWindow::iXoffsetDelta()
{
	return _iXoffsetDelta;
}

void StereoscopeWindow::SetXoffsetDelta(int i)
{
	if (!_fXoffsetLocked) // if it is locked, don't set anything
		_iXoffsetDelta = i;
}

void StereoscopeWindow::OnLayerManagement()
{
	// Let the user respond to the following (Layer/Overview+Updates)
	// The alternative is to do this only for the the active CDocument
	if (ltbLeft.IsWindowVisible() && ltbRight.IsWindowVisible())
	{
		ShowControlBar(&ltbLeft,FALSE,FALSE);
		ShowControlBar(&ltbRight,FALSE,FALSE);
	}
	else
	{
		ShowControlBar(&ltbLeft,TRUE,FALSE);
		ShowControlBar(&ltbRight,TRUE,FALSE);
	}
}

void StereoscopeWindow::OnUpdateLayerManagement(CCmdUI* pCmdUI)
{
	bool fCheck = ltbLeft.IsWindowVisible() && ltbRight.IsWindowVisible();
	pCmdUI->SetCheck(fCheck);
}


void StereoscopeWindow::OnOverviewWindow()
{
	if (gbOverviewLeft.IsWindowVisible() && gbOverviewRight.IsWindowVisible())
	{
		ShowControlBar(&gbOverviewLeft,FALSE,FALSE);
		ShowControlBar(&gbOverviewRight,FALSE,FALSE);
	}
	else
	{
		ShowControlBar(&gbOverviewLeft,TRUE,FALSE);
		ShowControlBar(&gbOverviewRight,TRUE,FALSE);
	}
}

void StereoscopeWindow::OnUpdateOverviewWindow(CCmdUI* pCmdUI)
{
	bool fCheck = gbOverviewLeft.IsWindowVisible() && gbOverviewRight.IsWindowVisible();
	pCmdUI->SetCheck(fCheck);
}

	/*
void StereoscopeWindow::OnScaleControl()
{
	if (barScale.IsWindowVisible())
		ShowControlBar(&barScale,FALSE,FALSE);
	else
		ShowControlBar(&barScale,TRUE,FALSE);
}

void StereoscopeWindow::OnUpdateScaleControl(CCmdUI* pCmdUI)
{
	bool fCheck = barScale.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}
*/
void StereoscopeWindow::OnInitMenu(CMenu* pMenu) 
{
	DataWindow::OnInitMenu(pMenu);
	
  MapPaneView* mpv = dynamic_cast<MapPaneView*>(GetActiveView());
	if (0 == mpv) 
		return;
  MapCompositionDoc* mcd = mpv->GetDocument();  
	if (0 == mcd) 
		return;
	mcd->menLayers(menPropLayer, ID_PROPLAYER);
  mcd->menLayers(menEditLayer, ID_EDITLAYER);
	mcd->menLayers(menDataLayer, ID_LAYFIRST);
  DrawMenuBar();
}

void StereoscopeWindow::OnMoving(UINT nSide, LPRECT lpRect)
{
	iWindowMiddle = (lpRect->left + lpRect->right) / 2;
}

void StereoscopeWindow::OnSizing(UINT nSide, LPRECT lpRect)
{
	if (nSide == WMSZ_BOTTOMLEFT || nSide == WMSZ_LEFT || nSide == WMSZ_TOPLEFT)
		lpRect->right = lpRect->left + (iWindowMiddle - lpRect->left)*2 + 1;
	else if (nSide == WMSZ_BOTTOMRIGHT || nSide == WMSZ_RIGHT || nSide == WMSZ_TOPRIGHT)
		lpRect->left = lpRect->right - (lpRect->right - iWindowMiddle)*2 - 1;
	// The -1 and +1 are to make sure the splitter window can always be divided into two
	// equal panes (the encapsulating window must have an odd width)
	DataWindow::OnSizing(nSide, lpRect);
}

bool StereoscopeWindow::fRequestMasterLock()
{
	bool fIAmSender = false;
	if (!fMasterLocked)
	{
		fIAmSender = true;
		fMasterLocked = true;
	}
	return fIAmSender;
}

void StereoscopeWindow::ReleaseMasterLock()
{
	fMasterLocked = false;
}

LRESULT StereoscopeWindow::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	LRESULT lReturn = 0;
	if (vwLeft)
		lReturn = vwLeft->SendMessage(ILW_UPDATE, wParam, lParam);
	if (vwRight)
		lReturn = vwRight->SendMessage(ILW_UPDATE, wParam, lParam);
	return lReturn;
}
