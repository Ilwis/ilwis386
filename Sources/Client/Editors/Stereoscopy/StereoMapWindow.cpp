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
// StereoMapWindow.cpp: implementation of the StereoMapWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Stereoscopy\StereoMapWindow.h"
#include "Client\Editors\Stereoscopy\PreStereoMateView.h"
#include "Client\Editors\Stereoscopy\MakeEpiPolarFormView.h"
#include "Client\Editors\Stereoscopy\MakeEpipolarDocument.h"
#include "Engine\Stereoscopy\StereoPairEpiPolar.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Headers\Hs\stereoscopy.hs"
#include "Headers\constant.h"
#include "Client\Mapwindow\MapStatusBar.h"
#include "Client\Base\WinThread.h"
#include "Engine\Applications\objvirt.h"
#include "Client\ilwis.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\Htp\Ilwis.htp"

IMPLEMENT_DYNCREATE(StereoMapWindow, DataWindow)

BEGIN_MESSAGE_MAP(StereoMapWindow, DataWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEWSTEREOPAIR, OnNewStereoPair)
	ON_COMMAND(ID_OPENSTEREOPAIR, OnOpenStereoPair)
	ON_COMMAND(ID_EXITSTEREOPAIR, OnClose)
	ON_COMMAND(ID_STEREO_CUSTOMIZE, OnConfigure)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND(ID_EXITSHOWSTEREOPAIR, OnExitShowStereoPair)
	ON_UPDATE_COMMAND_UI(ID_EXITSHOWSTEREOPAIR, OnUpdateExitShowStereopair)
	ON_COMMAND(ID_QUITSTEREOPAIR, OnQuit)
	ON_COMMAND(ID_TOOLBAR_STEREOSCOPY, OnStereoscopyBar)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_STEREOSCOPY, OnUpdateStereoscopyBar)
	ON_COMMAND(ID_ACTIVATE_LEFT, OnActivateLeftView)
	ON_UPDATE_COMMAND_UI(ID_ACTIVATE_LEFT, OnUpdateActiveLeftView)
	ON_COMMAND(ID_ACTIVATE_RIGHT, OnActivateRightView)
	ON_UPDATE_COMMAND_UI(ID_ACTIVATE_RIGHT, OnUpdateActiveRightView)
	ON_COMMAND(ID_TOOLBAR_INFOWINDOW, OnInfoWindowBar)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_INFOWINDOW, OnUpdateInfoWindowBar)
END_MESSAGE_MAP()


// vwLeft and vwRight can be used for functions 
// to replace the viewed raster maps. throigh the view the document can 
// be accessed and if needed reloaded

StereoMapWindow::StereoMapWindow()
: vwLeft(0), vwRight(0), docLeft(0), docRight(0), mkDocLeft(0), mkDocRight(0), frmLeft(0), frmRight(0), iTimer(0), fInitiallyUpdated(0)
{
  Create(NULL, "");
  docLeft = new MapCompositionDoc;
  docRight = new MapCompositionDoc;
  docLeft->AddView(vwLeft);
  docRight->AddView(vwRight);
	mkDocLeft = new MakeEpipolarDocument;
	mkDocRight = new MakeEpipolarDocument;
	mkDocLeft->AddView(frmLeft);
	mkDocRight->AddView(frmRight);
	// Assign 2nd document to a view with the following 4 lines:
	mkDocLeft->SetView(vwLeft);
	mkDocRight->SetView(vwRight);
	vwLeft->SetEpipolarDocument(mkDocLeft);
	vwRight->SetEpipolarDocument(mkDocRight);

  WinThread* wt = dynamic_cast<WinThread*>(AfxGetThread());
  String str = wt->sFileName();
  FileName fn(str);

	if (fn.sExt == ".stp")
		stp = StereoPair(fn, false);
	else
	{
		StereoPairUpdated(); // in case user cancels, we wanna be in a "resetted" state
		OnNewStereoPair();
	}

	if (stp.fValid())
	{
		InitialUpdate(0,TRUE);
		fInitiallyUpdated = true;
		StereoPairEpiPolar* spepi = dynamic_cast<StereoPairEpiPolar*>(stp->pStereoPairVirtual());
		if (spepi) // check on cast success!!
			RefreshMaps(spepi->sLeftInputMapName(), spepi->sRightInputMapName());
	}
	else
		PostMessage(WM_CLOSE, 0, 0); // window will not appear; resources will be cleaned up

	if (fn.sExt == ".stp")
		StereoPairUpdated();

	zIcon icon("StereoPairIcon");
	SetIcon(icon, TRUE);
  help = "ilwis\\epipolar_stereopair_creation_window.htm";
	sHelpKeywords = "Stereo pairs (create)";

	iTimer = SetTimer(1, 1000, 0); // timer id = 1, trigger every 1 sec, kill while m_hWnd still valid
}

StereoMapWindow::~StereoMapWindow()
{
	if (fnt.GetSafeHandle()) // if font was created, then delete it
		fnt.DeleteObject();
}

#define sMen(ID) ILWSF("men",ID).scVal()

#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);
#define addMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID)); menPopup.Detach();
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) menPopup.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

int StereoMapWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	MapStatusBar* msb = new MapStatusBar;
	status = msb;
	if (DataWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	msb->Create(this);
	EnableDocking(CBRS_ALIGN_ANY);

	CMenu men;
	CMenu menPopup;
	CMenu menSub;
	men.CreateMenu();
	menPopup.CreateMenu();
	add(ID_NEWSTEREOPAIR);
	add(ID_OPENSTEREOPAIR);
	addBreak;
	add(ID_STEREO_CUSTOMIZE);
	addBreak;
	add(ID_QUITSTEREOPAIR);
	add(ID_EXITSTEREOPAIR);
	add(ID_EXITSHOWSTEREOPAIR);
	addMenu(ID_MEN_FILE);

	menPopup.CreateMenu();
	add(ID_SELECTFIDUCIALS);
	add(ID_SELECTPPOINT);
	add(ID_SELECTTPPOINT);
	add(ID_SELECTSCALINGPTS);
	addMenu(ID_MEN_EDIT);

	menPopup.CreateMenu();
	add(ID_ADD_GRID);
	add(ID_ADD_GRATICULE);
	addMenu(ID_MEN_LAYERS);

  menPopup.CreateMenu();
	add(ID_ENTIREMAP);
	add(ID_NORMAL);
	add(ID_ZOOMIN);
	add(ID_ZOOMOUT);
	add(ID_SCALE1);
	add(ID_PANAREA);
  addBreak;
  add(ID_REDRAW);
	addBreak;
  add(ID_TOOLBAR_STEREOSCOPY);
	add(ID_TOOLBAR_INFOWINDOW);
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

	bbStereoscopy.Create(this, "stereoscopy.but", SStcUiStereoToolBar, 100); // to be substituted by string
	DockControlBar(&bbStereoscopy, AFX_IDW_DOCKBAR_TOP);

	// Still implement the box (a toolbar with CEdit) for user info / errors

	// Init the font for the CEdit
	
	if (!::GetSystemMetrics(SM_DBCSENABLED))
	{
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(logFont));
		// Since design guide says toolbars are fixed height so is the font.
		logFont.lfHeight = -12;
		logFont.lfWeight = FW_BOLD;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		lstrcpy(logFont.lfFaceName, "MS Sans Serif");
		fnt.CreateFontIndirect(&logFont);
	}
	// Code for the CToolBar bbTxt
	CWindowDC dc(CWnd::GetDesktopWindow());
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
  int iBBTxtHeight = tm.tmHeight * 8;
	int iBBTxtWidth = tm.tmAveCharWidth * 52; // reserve reasonable width for text

	CRect rect(0,0,iBBTxtWidth,iBBTxtHeight);

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY;
	CRect rectBB;
	rectBB.SetRect(2,1,2,1);
	bbTxt.CreateEx(this, dwCtrlStyle, dwStyle, rectBB, 201);
	bbTxt.SetWindowText(SStcUiInfoWindowTitle.scVal());
	UINT ai[2];
	ai[0] = ID_EDITGRFTRANSF;
	ai[1] = ID_SEPARATOR;
	bbTxt.SetButtons(ai,2);
	// iBBTxtHeight = tm.tmHeight; // reset height??
	bbTxt.GetItemRect(0, &rect);
	bbTxt.SetButtonInfo(1, 201,	TBBS_SEPARATOR, iBBTxtWidth - rect.Width());
	rect.top = 2;
	rect.right = rect.left + iBBTxtWidth;
	rect.bottom = rect.top + iBBTxtHeight;
	// Code for the CEdit edTxt on the CToolBar bbTxt
	edTxt.Create(WS_VISIBLE|WS_CHILD|ES_READONLY,rect,&bbTxt,201);
	edTxt.SetFont(&fnt);

	bbTxt.EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	bbTxt.SetBarStyle(bbTxt.GetBarStyle()|CBRS_GRIPPER|CBRS_BORDER_3D);
	RecalcLayout(); // to get latest dimension after appending bbStereoscopy
	bbStereoscopy.GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	DockControlBar(&bbTxt,AFX_IDW_DOCKBAR_TOP,rect);

	RecalcLayout(); // recalc the layout due to toolbar bbTxt append

  return 0;
}


// instead of CSplitterWnd perhaps a special class has to be defined
// see also TableWindow, this special class could make sure of special sizing
BOOL StereoMapWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
 	CCreateContext contextT;
  pContext = &contextT;
	wndSplitter.CreateStatic(this, 2, 2, WS_CHILD|WS_VISIBLE);
	wndSplitter.CreateView(0, 0, RUNTIME_CLASS(PreStereoMateView), CSize(400, 500), pContext);
	wndSplitter.CreateView(0, 1, RUNTIME_CLASS(PreStereoMateView), CSize(400, 500), pContext);
  wndSplitter.CreateView(1, 0, RUNTIME_CLASS(MakeEpipolarFormView), CSize(400, 500), pContext);
  wndSplitter.CreateView(1, 1, RUNTIME_CLASS(MakeEpipolarFormView), CSize(400, 500), pContext);
  vwLeft = (PreStereoMateView*)wndSplitter.GetPane(0,0);
  vwRight = (PreStereoMateView*)wndSplitter.GetPane(0,1);
	frmLeft = (MakeEpipolarFormView*)wndSplitter.GetPane(1,0);
	frmRight = (MakeEpipolarFormView*)wndSplitter.GetPane(1,1);

	// Make sure from mapview you can fully reach the corresponding formview and vice versa
	// frmLeft->SetStereoMateView(vwLeft, frmRight, true); // 2nd parameter points to the other form
	// frmRight->SetStereoMateView(vwRight, frmLeft, false); // 3rd parameter indicates left or right form
	SetActiveView(vwLeft);
	return TRUE;
}

class OpenMapPairForm: public FormWithDest
{
public:
  OpenMapPairForm(CWnd* wPar, String* sLeftMap, String* sRightMap, String* sStereoPairName, String* sDescr)
    : FormWithDest(wPar, SStcTitleCreateEpiStereoPair), m_psStereoName(sStereoPairName), 
		  m_psLeftMap(sLeftMap), m_psRightMap(sRightMap)
  {
    fmLeft  = new FieldMap(root, SStcUiLeftMap, m_psLeftMap);
    fmRight = new FieldMap(root, SStcUiRightMap, m_psRightMap);
		fdtc = new FieldDataTypeCreate(root, SStcUiOutputStereoPair, m_psStereoName, ".stp", true);
		fmLeft->SetCallBack((NotifyProc)&OpenMapPairForm::CallBackName);
		fmRight->SetCallBack((NotifyProc)&OpenMapPairForm::CallBackName);
		fdtc->SetCallBack((NotifyProc)&OpenMapPairForm::CallBackName);

		FieldString * fs = new FieldString(root, SStcUiDescription, sDescr);
		fs->SetWidth(90);
		stRemark = new StaticText(root, SStcErrStereopairExists); // trick to get enough space for the static
		stRemark->SetVal(String());  // reset string value
    SetMenHelpTopic("ilwismen\\create_a_stereopair.htm");
		create();    
  } 
	int CallBackName(Event*)
	{
		fdtc->StoreData();
		FileName fn(*m_psStereoName, ".stp");
		bool fOk = false;
		if (fn.fValid())
		{
			if (File::fExist(fn))   
				stRemark->SetVal(SStcErrStereopairExists);
			else
			{
				stRemark->SetVal(String());
				fmLeft->StoreData();
				fmRight->StoreData();
				fOk = m_psLeftMap->length() > 0 && m_psRightMap->length() > 0;
			}
		}
		if (fOk)
			EnableOK();
		else    
			DisableOK();
		return 0;
	}
private:
	FieldDataTypeCreate *fdtc;
	FieldMap            *fmLeft, *fmRight;
	String              *m_psStereoName, *m_psLeftMap, *m_psRightMap;
  String               sNewName;
	StaticText          *stRemark;
};

void StereoMapWindow::OnNewStereoPair()
{
  String sLeftMap, sRightMap;
	String sStereoPairName;
	String sDescr = "";
	OpenMapPairForm frm(this, &sLeftMap, &sRightMap, &sStereoPairName, &sDescr);
  if (frm.fOkClicked())
  {
		if (stp.fValid())
			stp->Updated(); // default: save old one (is this what user expects?)
		FileName fn (sStereoPairName);
		IlwWinApp()->SetCurDir(fn.sPath());
		stp = StereoPair(FileName(sStereoPairName, ".stp"), sLeftMap, sRightMap);
		if (sDescr.length() == 0)
		{
			sDescr = String(SStcTextStereopairFrom_S_S_S.scVal(), FileName(sStereoPairName).sFile,
							FileName(sLeftMap).sFile, FileName(sRightMap).sFile);
		}
		stp->sDescription = sDescr;
		StereoPairUpdated();
		if (fInitiallyUpdated)
			RefreshMaps(sLeftMap, sRightMap);
  }
}

class OpenStereoObjectForm: public FormWithDest
{
public:
  OpenStereoObjectForm(CWnd* wPar, String* sStereoPairName)
    : FormWithDest(wPar, SStcTitleOpenMapPair)    
  {
		new FieldDataType(root, SStcUiStereoPair, sStereoPairName, ".stp", true);
    create();    
  }    
};

void StereoMapWindow::OnOpenStereoPair()
{
	String sStereoPairName;
	OpenStereoObjectForm frm(this, &sStereoPairName);
	if (frm.fOkClicked())
	{
		if (stp.fValid())
			stp->Updated(); // default: save old one (is this what user expects?)
		FileName fn (sStereoPairName);
		IlwWinApp()->SetCurDir(fn.sPath());
		stp = StereoPair(FileName(sStereoPairName, ".stp"), false);
		if (stp.fValid())
		{
			StereoPairEpiPolar* spepi = dynamic_cast<StereoPairEpiPolar*>(stp->pStereoPairVirtual());
			if (spepi) // check on cast success!!
				RefreshMaps(spepi->sLeftInputMapName(), spepi->sRightInputMapName());
		}
		StereoPairUpdated();
	}
}

void StereoMapWindow::OnClose()
{
	if (iTimer) // in case init failed somehow
	{
		KillTimer(iTimer);
		iTimer = 0;
	}

  RemoveViews();
	if (stp.fValid())
		stp->Updated();
	DataWindow::OnClose();
}

void StereoMapWindow::OnExitShowStereoPair()
{
	if (iTimer) // in case init failed somehow
	{
		KillTimer(iTimer);
		iTimer = 0;
	}

	if (stp.fValid())
	{
		RemoveViews();
		stp->Updated();
		String sExec("show %S", stp->sNameQuoted(true));
		IlwWinApp()->Execute(sExec);
		DataWindow::OnClose(); // intentional: don't close if stp is invalid:
													 // the user will expect to see a "show" happening
	}
}

void StereoMapWindow::OnQuit()
{
	if (iTimer) // in case init failed somehow
	{
		KillTimer(iTimer);
		iTimer = 0;
	}
	RemoveViews();
	DataWindow::OnClose();
}

void StereoMapWindow::StereoPairUpdated()
{
	String sTitle = SStcTitleStereoPairCreation; // Start composing title
	// Reset: previous value may be invalid
	mkDocLeft->SetGrEpiMember(0);
	mkDocRight->SetGrEpiMember(0);
		// pass the new left/right georef pointers to the left/right MakeEpipolarDocuments
	if (stp.fValid())
	{
		sTitle = String ("%S - ", stp->sName()) + sTitle;
		StereoPairEpiPolar* spepi = dynamic_cast<StereoPairEpiPolar*>(stp->pStereoPairVirtual());
		if (spepi) // check on cast success!!
		{
			mkDocLeft->SetGrEpiMember(spepi->pGetEpiLeft());
			mkDocRight->SetGrEpiMember(spepi->pGetEpiRight());
			mkDocLeft->SetState(MakeEpipolarDocument::ifsFIDUCIALS);
			mkDocRight->SetState(MakeEpipolarDocument::ifsFIDUCIALS);
			mkDocLeft->UpdateAllEpipolarViews(0, 10000); // reflect the .stp just opened in the views
			mkDocRight->UpdateAllEpipolarViews(0, 10000); // reflect the .stp just opened in the views
		}
	}

	SetWindowText(sTitle.sVal());
}

void StereoMapWindow::RefreshMaps(const String& sLeftMap, const String& sRightMap)
{
	frmLeft->SetInputImageName(sLeftMap);
	frmRight->SetInputImageName(sRightMap);
  docLeft->OnOpenDocument(sLeftMap.scVal(), IlwisDocument::otNOASK);
  docRight->OnOpenDocument(sRightMap.scVal(), IlwisDocument::otNOASK);
	docLeft->UpdateAllViews(0,3); // entiremap to left views
	docRight->UpdateAllViews(0,3); // entiremap to right views
}

void StereoMapWindow::RemoveViews()
{
	// cleanup views attached, in reverse order
	vwLeft->SetEpipolarDocument(0);
	vwRight->SetEpipolarDocument(0);
	mkDocLeft->SetView(0);
	mkDocRight->SetView(0);
	mkDocLeft->RemoveView(frmLeft); // mkDocLeft is actually deleted in RemoveView
	mkDocRight->RemoveView(frmRight);
  docLeft->RemoveView(vwLeft); // docLeft is actually deleted in RemoveView
  docRight->RemoveView(vwRight);
}

void StereoMapWindow::OnTimer(UINT nIDEvent)
{
	if (1 == nIDEvent) // is it for us? our id == 1
	{
		String sErr = "";
		if (stp.fValid())
		{
			StereoPairEpiPolar* spepi = dynamic_cast<StereoPairEpiPolar*>(stp->pStereoPairVirtual());
			if (spepi) // check on cast success!!
				sErr = spepi->sErrGeorefs();
		}

		edTxt.SetWindowText(sErr.scVal());
	}

	// Still call base class
	DataWindow::OnTimer(nIDEvent);
}

void StereoMapWindow::OnStereoscopyBar()
{
	if (bbStereoscopy.IsWindowVisible())
		ShowControlBar(&bbStereoscopy,FALSE,FALSE);
	else
		ShowControlBar(&bbStereoscopy,TRUE,FALSE);
}

void StereoMapWindow::OnUpdateStereoscopyBar(CCmdUI* pCmdUI)
{
	bool fCheck = bbStereoscopy.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

void StereoMapWindow::OnUpdateExitShowStereopair(CCmdUI* pCmdUI)
{
	String sErr = "";
	if (stp.fValid())
	{
		StereoPairEpiPolar* spepi = dynamic_cast<StereoPairEpiPolar*>(stp->pStereoPairVirtual());
		if (spepi) // check on cast success!!
			sErr = spepi->sErrGeorefs();
		pCmdUI->Enable(sErr.length() == 0);
	}
	else
		pCmdUI->Enable(false); // can't do calc/show: there's no stp loaded
}

void StereoMapWindow::OnActivateLeftView()
{
	SetActiveView(vwLeft);
}

void StereoMapWindow::OnUpdateActiveLeftView(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetActiveView() == vwLeft);
}

void StereoMapWindow::OnActivateRightView()
{
	SetActiveView(vwRight);
}

void StereoMapWindow::OnUpdateActiveRightView(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(GetActiveView() == vwRight);
}

void StereoMapWindow::OnInfoWindowBar()
{
		if (bbTxt.IsWindowVisible())
		ShowControlBar(&bbTxt,FALSE,FALSE);
	else
		ShowControlBar(&bbTxt,TRUE,FALSE);
}

void StereoMapWindow::OnUpdateInfoWindowBar(CCmdUI* pCmdUI)
{
	bool fCheck = bbTxt.IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

class StereoMakerConfigForm: public FormWithDest
{
public:
  StereoMakerConfigForm(CWnd* wPar, PreStereoMateView* vw, Color* colActive)
  : FormWithDest(wPar, SStcTitleCustomize)
  {
    new FieldColor(root, SStcUiColFidMarks, &vw->colFidMarks);
    new FieldColor(root, SStcUiColScalePnt, &vw->colScalePnts);
    new FieldColor(root, SStcUiColPrincPnt, &vw->colPrincPnts);
    new FieldColor(root, SStcUiColUPPnt, &vw->colUserPrincPnts);
		new FieldColor(root, SStcUiColActive, colActive);
    create();
  }
};

void StereoMapWindow::OnConfigure()
{
	Color colActiveColor (frmLeft->GetActiveColor());
  StereoMakerConfigForm frm(this, vwLeft, &colActiveColor);
  if (frm.fOkClicked())
	{
		// store in registry
  	IlwisSettings settings("Stereo Maker");
		settings.SetValue("FM", vwLeft->colFidMarks);
		settings.SetValue("PP", vwLeft->colPrincPnts);
		settings.SetValue("SP", vwLeft->colScalePnts);
		settings.SetValue("UPP", vwLeft->colUserPrincPnts);
		settings.SetValue("Background Active", colActiveColor);

		// commit the changes in the views
		CopyLeftColorsToRightView();
		frmLeft->SetActiveColor(colActiveColor);
		frmRight->SetActiveColor(colActiveColor);

		// update the views
		mkDocLeft->UpdateAllEpipolarViews(0, 10000);
		mkDocRight->UpdateAllEpipolarViews(0, 10000);
  }  
}

void StereoMapWindow::CopyLeftColorsToRightView()
{
	vwRight->colFidMarks = vwLeft->colFidMarks;
	vwRight->colPrincPnts = vwLeft->colPrincPnts;
	vwRight->colScalePnts = vwLeft->colScalePnts;
	vwRight->colUserPrincPnts = vwLeft->colUserPrincPnts;
}
