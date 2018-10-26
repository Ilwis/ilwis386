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
/* FrameWindow
// by Wim Koolhoven
// (c) Ilwis System Development ITC
	Last change:  WK   23 Dec 98   10:40 am
*/
#define FRAMEWIN_C

#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\BaseView.h"
#include "Client\Base\Framewin.h"
#include "Client\Base\ZappTools.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Headers\Htp\Ilwis.htp"
#include <afxpriv.h>
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"
#include "Client\ilwis.h"

// Setup the indicator fields for the status bar
static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

// class FrameWindow
IMPLEMENT_DYNCREATE( FrameWindow, CFrameWnd )

BEGIN_MESSAGE_MAP( FrameWindow, CFrameWnd )
	//{{AFX_MSG_MAP( FrameWindow )
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(ID_STATUSLINE, OnStatusLine)
	ON_UPDATE_COMMAND_UI(ID_STATUSLINE, OnUpdateStatusLine)
	ON_WM_ACTIVATE()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_COMMAND(ID_HLPKEY, OnHelp)
	ON_COMMAND(ID_HELP_RELATED_TOPICS, OnRelatedTopics)
	ON_COMMAND(ID_HELP_ILWIS_OBJECTS, OnIlwisObjects)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xffff, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xffff, OnToolTipText)
END_MESSAGE_MAP()

FrameWindow::FrameWindow()
	: m_sWindowName("")
	, status(0)
	, fStoreToolBars(true)
{
  help = "ilwis\\contents.htm";
}

FrameWindow::~FrameWindow()
{
	if (m_hAccelTable) 
		DestroyAcceleratorTable(m_hAccelTable);
	m_hAccelTable = 0;
	if (status)
		delete status;
}

// Original code found in CFrameWnd::InitialUpdateFrame()
void FrameWindow::InitialUpdate(CDocument* pDoc, BOOL bMakeVisible)
{
	// if the frame does not have an active view, set to first pane
	CView* pView = NULL;
	if (GetActiveView() == NULL)
	{
		CWnd* pWnd = GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
		{
			pView = (CView*)pWnd;
			SetActiveView(pView, FALSE);
		}
	}

	if (bMakeVisible)
	{
		// send initial update to all views (and other controls) in the frame
		SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

		InitSettings();

		// finally, activate the frame
		// (send the default show command unless the main desktop window)
		int nCmdShow = SW_SHOWNORMAL;
		ActivateFrame(nCmdShow);
		if (pView != NULL) 
			SetActiveView(pView);
    SetForegroundWindow();
	}

	// update frame counts and frame title (may already have been visible)
	if (pDoc != NULL)
		pDoc->UpdateFrameCounts();
	OnUpdateFrameTitle(TRUE);
}

void FrameWindow::SetWindowName(String sName)
{
	if (!fCIStrEqual(sName, m_sWindowName))
		m_sWindowName = sName;
}

String FrameWindow::sWindowName()
{
	return m_sWindowName;
}

void FrameWindow::CheckMove()
{
	// prevent the code to find its own window
	ShowWindow(SW_HIDE);  

	CRect rect;
	GetWindowRect(&rect);
	int iMove = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER);
	int iXScreen = GetSystemMetrics(SM_CXFULLSCREEN);
	int iYScreen = GetSystemMetrics(SM_CYFULLSCREEN);

	bool fReady = false;
	while (!fReady) 
	{
		// Is there a window at the top left position of this window?
		CWnd* pw = WindowFromPoint(rect.TopLeft());
		if (pw == 0)
		{
			// is there a window at the position of the system menu
			pw = WindowFromPoint(rect.TopLeft() + CSize(iMove, iMove));
			if (pw == 0)
				break;  // not on any window
		}

		CRect rectBack;
		pw->GetWindowRect(&rectBack);
		rectBack.SetRect(rectBack.TopLeft(), rectBack.TopLeft());
		rectBack.InflateRect(iMove, iMove);
		if (rectBack.PtInRect(rect.TopLeft()) != 0) 
		{
			rect.OffsetRect(iMove, iMove);
			if (iXScreen <= rect.left || iYScreen <= rect.top)
				rect.SetRect(iMove, iMove, rect.right, rect.bottom);
			MoveWindow(&rect);
		}
		else
			fReady = true;
	}
	ShowWindow(SW_SHOW); // redisplay the window
}

void FrameWindow::LoadState(IlwisSettings& ilwSettings)
{
	if (fStoreToolBars)
	{
		String sBarKey = String("%S\\ToolBars\\Bars", ilwSettings.sIlwisSubKey());
		LoadBarState(sBarKey.c_str());
	}

	WINDOWPLACEMENT wpl, wplDefault;
	GetWindowPlacement(&wpl);
	wplDefault = wpl;
	wpl = ilwSettings.GetWindowPlacement("WindowPosition", wplDefault);

	SetWindowPlacement(&wpl);
	if (wpl.showCmd != SW_SHOWMAXIMIZED && wpl.showCmd != SW_SHOWMINIMIZED)
		CheckMove();
}

void FrameWindow::SaveState(IlwisSettings& ilwSettings)
{
	// remove addinfo and record bar first
	POSITION pos = m_listControlBars.GetHeadPosition();
	while (pos != NULL) {
		CControlBar* pBar =
			(CControlBar*)m_listControlBars.GetNext(pos);
		int iID = pBar->GetDlgCtrlID();
		if (ID_ADDINFOBAR == iID || ID_RECORDBAR == iID) {
			delete pBar;
      pos = m_listControlBars.GetHeadPosition();
		}
	}

	if (fStoreToolBars)
	{
		String sBarKey = String("%S\\ToolBars\\Bars", ilwSettings.sIlwisSubKey());
		SaveBarState(sBarKey.c_str());
	}

	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);

 	ilwSettings.SetWindowPlacement("WindowPosition", wpl);
}

void FrameWindow::InitSettings()
{
	SendMessageToDescendants(ILWM_VIEWSETTINGS, LOADSETTINGS, 0, TRUE, TRUE);

	IlwisSettings settings(sWindowName());
	if (sWindowName() != "")
		LoadState(settings);
}

void FrameWindow::SaveSettings()
{
	SendMessageToDescendants(ILWM_VIEWSETTINGS, SAVESETTINGS, 0, TRUE, TRUE);
  
	if (sWindowName() != "")
		SaveState(IlwisSettings(sWindowName()));
}

void FrameWindow::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
}

void FrameWindow::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);
}

int FrameWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd ::OnCreate(lpCreateStruct) == -1)  // this call creates the view
		return -1;
	
	if (0 == status) {
		status = new CStatusBar;
		status->Create(this);
		status->SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	}

	return 0;
}

void FrameWindow::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// largely copied from CFrameWnd::OnUpdateFrameTitle() 
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave it alone!

	if (m_pNotifyHook != NULL) {
		CFrameWnd::OnUpdateFrameTitle(bAddToTitle);
		return;
	}

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle && pDocument != NULL)
		UpdateFrameTitleForDocument(pDocument->GetTitle());
	else
		UpdateFrameTitleForDocument(NULL);
}

void FrameWindow::UpdateFrameTitleForDocument(LPCTSTR lpszDocName)
{
	// largely copied from CFrameWnd::UpdateFrameTitleForDocument()
	// copy first part of title loaded at time of frame creation
	TCHAR szText[256+_MAX_PATH];

	if (GetStyle() & FWS_PREFIXTITLE)
	{
		szText[0] = '\0';   // start with nothing

		// get name of currently active view
		if (lpszDocName != NULL)
		{
			std::string s1(lpszDocName);
			if ( s1.size() > 75) {
				s1 = s1.substr(0,75);
				s1 += "...";
			}
			lstrcpy(szText, s1.c_str());
			lstrcat(szText, _T(" - "));
		}
		std::string s(m_strTitle);
		if ( s.size() > 75) {
			s = s.substr(0,75);
			s += "...";
		}
		lstrcat(szText, s.c_str());
	}
	else
	{
		// get name of currently active view
		std::string s (m_strTitle);
		if (s.size() > 75)
			s = s.substr(0, 75) + "...";
		lstrcpy(szText, s.c_str());
		if (lpszDocName != NULL)
		{
			lstrcat(szText, _T(" - "));
			std::string s1(lpszDocName);
			if (s1.size() > 75)
				s1 = s1.substr(0, 75) + "...";
			lstrcat(szText, s1.c_str());
		}
	}

	// set title if changed, but don't remove completely
	// Note: will be excessive for MDI Frame with maximized child
	AfxSetWindowText(m_hWnd, szText);
}

/*
The state storing code must be placed in CMainFrame::DestroyWindow() (via OnClose()), 
not in OnDestroy(), because at the time WM_DESTROY message is received, the floating 
bars are already destroyed.
*/

void FrameWindow::OnClose() 
{
	SaveSettings();
	if ( GetSafeHwnd() != NULL)
		CFrameWnd::OnClose();
}

void FrameWindow::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	// only post WM_QUIT when in seperate thread
	if (AfxGetThread() != AfxGetApp())
		PostQuitMessage(0);
}

BOOL FrameWindow::ActivateMenuItem(WORD wID)
{
	HiliteMenuItem(GetMenu(), wID, MF_BYCOMMAND | MF_HILITE);
	SendMessage(WM_COMMAND, wID, (LPARAM)m_hWnd);
	HiliteMenuItem(GetMenu(), wID, MF_BYCOMMAND | MF_UNHILITE);
	return TRUE;
}

void FrameWindow::OnStatusLine()
{
	if (status->IsWindowVisible())
		ShowControlBar(status,FALSE,FALSE);
	else
		ShowControlBar(status,TRUE,FALSE);
}

void FrameWindow::OnUpdateStatusLine(CCmdUI* pCmdUI)
{
	bool fCheck = status->IsWindowVisible() != 0;
	pCmdUI->SetCheck(fCheck);
}

LRESULT FrameWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	try {	
		return CFrameWnd::WindowProc(message, wParam, lParam);
	}
	catch (ErrorObject& err) {
		err.Show();
	}
	catch(CException* err)
	{
    MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
		err->Delete();
	}
	return DefWindowProc(message, wParam, lParam);
}

void FrameWindow::GetMessageString(UINT nID, CString& rMessage) const
{
	string s;
	if (IlwisWinApp::fHelpNumbers())
		s = String("%i: %S", nID, ILWSF("dsc", nID));
	else
		s = ILWSF("dsc", nID);
	rMessage = s.c_str();
}


void FrameWindow::GetToolText(UINT uID, CString& sMess) const
{
	sMess = ILWSF("men", uID).c_str();
	sMess.Replace(8,0);		// backspace
	sMess.Replace('.',0); // ... (elipses)
}


#define _countof(array) (sizeof(array)/sizeof(array[0]))

BOOL FrameWindow::OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	UINT nID = pNMHDR->idFrom;
/*
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
*/
		CString sLoc;
		GetToolText(nID, sLoc);

#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
			lstrcpyn(pTTTA->szText, sLoc, _countof(pTTTA->szText));
		else
			_mbstowcsz(pTTTW->szText, sLoc, _countof(pTTTW->szText));
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
			_wcstombsz(pTTTA->szText, sLoc, _countof(pTTTA->szText));
		else
			lstrcpyn(pTTTW->szText, sLoc, _countof(pTTTW->szText));
#endif

		if (pNMHDR->code == TTN_NEEDTEXTA)
			pTTTA->hinst = NULL;
		else
			pTTTW->hinst = NULL;
		return TRUE;
/*
	}
	return FALSE;
*/
}

void FrameWindow::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CFrameWnd ::OnActivate(nState, pWndOther, bMinimized);
	
	if (pWndOther && pWndOther->GetTopLevelOwner() == this)
		return;
	switch (nState) {
		case WA_INACTIVE:
			//TRACE("Inactivate window\n");
			OnShowFloatingControlBars(false);
			break;
		case WA_CLICKACTIVE:
			//TRACE("Click");
		case WA_ACTIVE:
			//TRACE("Activate window\n");
			OnShowFloatingControlBars(true);
			break;
	}	
}

// largely copied from COleServerDoc::OnShowControlBars()
void FrameWindow::OnShowFloatingControlBars(bool fShow)
{
	// show/hide all floating control bars
	POSITION pos = m_listControlBars.GetHeadPosition();
	while (pos != NULL)
	{
		// show/hide the next control bar
		CControlBar* pBar =
			(CControlBar*)m_listControlBars.GetNext(pos);
		ASSERT_VALID(pBar);
		CDockBar* db = dynamic_cast<CDockBar*>(pBar);
		if (db)
			continue;
		if (fShow)
		{
			if (pBar->m_nStateFlags & CControlBar::tempHide)
			{
				pBar->m_nStateFlags &= ~CControlBar::tempHide;
				ShowControlBar(pBar, TRUE, TRUE);
			}
		}
		else
		{
			if (pBar->IsVisible() && pBar->IsFloating())
			{
				pBar->m_nStateFlags |= CControlBar::tempHide;
				ShowControlBar(pBar, FALSE, TRUE);
			}
		}
	}
}

void FrameWindow::resize(CSize& sz) // change size (increment)
{
  zRect rExt;
  GetWindowRect(rExt);
  rExt.right() += sz.cx;
  rExt.bottom() += sz.cy;

	// if the window does not fit nicely in the available work area move it
	zRect rectWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);

	int iRight = rectWorkArea.right() - rExt.right();
	if (iRight < 10) {
		int iWidth = rExt.width();
		rExt.left() += iRight - 10;
		if (rExt.left() < rectWorkArea.left())
			rExt.left() = rectWorkArea.left();
		rExt.right() = rExt.left() + iWidth;
	}
	int iBottom = rectWorkArea.bottom() - rExt.bottom();
	if (iBottom < 10) {
		int iHeight = rExt.height();
		rExt.top() += iBottom	- 10;
		if (rExt.top() < rectWorkArea.top())
			rExt.top() = rectWorkArea.top();
		rExt.bottom() = rExt.top() + iHeight;
	}

  move(rExt);
};


void FrameWindow::SetAcceleratorTable()
{
	if (m_hAccelTable) 
		DestroyAcceleratorTable(m_hAccelTable);
	m_hAccelTable = 0;
	CMenu* men = GetMenu();
	vector<ACCEL> va;
	// add accelerators mentioned in menu
	AddAccel(men, va);

	// add standard accelerators
	ACCEL acc;
	acc.cmd = ID_EDIT_COPY; // Ctrl-Ins
	acc.key = VK_INSERT;
	acc.fVirt = FVIRTKEY | FCONTROL;
	va.push_back(acc);
	acc.cmd = ID_EDIT_PASTE;	// Shft-Ins
	acc.key = VK_INSERT;
	acc.fVirt = FVIRTKEY | FSHIFT;
	va.push_back(acc);
	acc.cmd = ID_EDIT_CUT;	// Shft-Del
	acc.key = VK_DELETE;
	acc.fVirt = FVIRTKEY | FSHIFT;
	va.push_back(acc);

	acc.cmd = ID_HELP;	// F1
	acc.key = VK_F1;
	acc.fVirt = FVIRTKEY;
	va.push_back(acc);
	acc.cmd = ID_REDRAW;	// F5
	acc.key = VK_F5;
	acc.fVirt = FVIRTKEY;
	va.push_back(acc);
	acc.cmd = ID_HLPCONTENTS;	// F11
	acc.key = VK_F11;
	acc.fVirt = FVIRTKEY;
	va.push_back(acc);
	acc.key = VK_F1;
	acc.fVirt = FVIRTKEY | FSHIFT;
	va.push_back(acc);

	m_hAccelTable = CreateAcceleratorTable(&*va.begin(), va.size());
}

BOOL FrameWindow::PreTranslateMessage(MSG* pMsg)
{
	CString str;
  if (pMsg->message == WM_KEYDOWN)
	{
    if (pMsg->wParam != VK_F1 && pMsg->wParam != VK_F11
		    && !fHandleAccelerator)
			  return FALSE;
	}
  return CFrameWnd::PreTranslateMessage(pMsg);
}

void FrameWindow::OnHelp() 
{
	IlwWinApp()->showHelp(help);
}

LRESULT FrameWindow::OnCommandHelp(WPARAM, LPARAM lParam)
{
	OnHelp();
	return TRUE;
}

void FrameWindow::OnRelatedTopics()
{
	//HH_AKLINK link;
	//link.cbStruct =     sizeof(HH_AKLINK);
	//link.fReserved =    FALSE;
	//link.pszKeywords =  sHelpKeywords.c_str(); 
	//link.pszUrl =       NULL; 
	//link.pszMsgText =   NULL; 
	//link.pszMsgTitle =  NULL; 
	//link.pszWindow =    NULL;
	//link.fIndexOnFail = TRUE;

	//String sHelpFile (ChmFinder::sFindChmFile("ilwis.chm"));

	//::HtmlHelp(::GetDesktopWindow(), sHelpFile.sVal(), HH_KEYWORD_LOOKUP, (DWORD)&link);
}

void FrameWindow::OnIlwisObjects()
{
	IlwWinApp()->showHelp(help);
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) menPopup.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak menPopup.AppendMenu(MF_SEPARATOR);

void FrameWindow::AddHelpMenu()
{
	CMenu* men = GetMenu();
	CMenu menPopup;
  menPopup.CreateMenu();
  add(ID_HLPKEY);
	add(ID_HELP_RELATED_TOPICS);
	addBreak
  add(ID_HLPCONTENTS);
  add(ID_HLPINDEX);
  add(ID_HLPSEARCH);
	addBreak
	add(ID_ABOUT)
  men->AppendMenu(MF_POPUP, (UINT)menPopup.GetSafeHmenu(), sMen(ID_MEN_HELP));
  menPopup.Detach();
}

BOOL FrameWindow::OnHelpInfo(HELPINFO* /*pHelpInfo*/)
{
	// default implementation sends a second ID_HELP
	// command, which will not be routed!
	// so do nothing
	return TRUE;
}

int FrameWindow::iNewBarID()
{
	int iID = ID_NEWBARS;
	POSITION pos = m_listControlBars.GetHeadPosition();
	while (pos != NULL) {
		CControlBar* pBar =
			(CControlBar*)m_listControlBars.GetNext(pos);
		iID = max(iID, pBar->GetDlgCtrlID());
	}
	return iID+1;
}
