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
/* CatalogFrameWindow
// by Wim Koolhoven
// (c) Ilwis System Development ITC
Last change:  WK   23 Dec 98   10:40 am
*/
#define FRAMEWIN_C

#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include <shlwapi.h>
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\BaseView.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\Base\Framewin.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\Catalog\CatalogFrameWindow.h"
#include "Client\Base\ZappTools.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Headers\Htp\Ilwis.htp"
#include <afxpriv.h>
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"


// class CatalogFrameWindow
IMPLEMENT_DYNCREATE( CatalogFrameWindow, CMDIChildWnd )

BEGIN_MESSAGE_MAP( CatalogFrameWindow, CMDIChildWnd )
	ON_WM_CREATE()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_COMMAND(ID_HLPKEY, OnHelp)
	ON_COMMAND(ID_HELP_RELATED_TOPICS, OnRelatedTopics)
	ON_WM_MDIACTIVATE()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xffff, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xffff, OnToolTipText)
END_MESSAGE_MAP()

CatalogFrameWindow::CatalogFrameWindow()
: m_sWindowName("")
{
	help =  "ilwis\\main_window.htm";
	sHelpKeywords = "Main Window";
}

CatalogFrameWindow::~CatalogFrameWindow()
{
}

int CatalogFrameWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (0 != CMDIChildWnd::OnCreate(lpCreateStruct))
		return -1;

	// if maximized at the moment rstore to prevent that the user does not realize that multiple catalogs are open
	CMDIFrameWnd* mfw = GetMDIFrame();
	if (mfw) 
	{
		BOOL fMaximized;
		CMDIChildWnd* mcw = mfw->MDIGetActive(&fMaximized);
		if (0 != mcw && fMaximized)
			mcw->MDIRestore();
	}
	return 0;
}

void CatalogFrameWindow::SetWindowName(String sName)
{
	if (!fCIStrEqual(sName, m_sWindowName))
		m_sWindowName = sName;
}

String CatalogFrameWindow::sWindowName()
{
	return m_sWindowName;
}

void CatalogFrameWindow::LoadState(IlwisSettings& ilwSettings)
{
}

void CatalogFrameWindow::SaveState(IlwisSettings& ilwSettings)
{
	String sBarKey = String("%S\\ToolBars\\Bars", ilwSettings.sIlwisSubKey());
	SaveBarState(sBarKey.c_str());

	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);

	ilwSettings.SetWindowPlacement("WindowPosition", wpl);
}

void CatalogFrameWindow::InitSettings()
{
	SendMessageToDescendants(ILWM_VIEWSETTINGS, LOADSETTINGS, 0, TRUE, TRUE);

	IlwisSettings settings(sWindowName());
	if (sWindowName() != "")
		LoadState(settings);
}

void CatalogFrameWindow::SaveSettings(IlwisSettings& settings, const WINDOWPLACEMENT& place, int iNr)
{
	Catalog *cat = (Catalog *)GetActiveView();
	CatalogDocument *doc = cat->GetDocument();
	String sKey = String("%S\\%S", settings.sIlwisSubKey(), doc->sName());
	cat->SaveCatalogView(sKey, iNr);
}

LRESULT CatalogFrameWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	try {	
		return CMDIChildWnd::WindowProc(message, wParam, lParam);
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

void CatalogFrameWindow::GetMessageString(UINT nID, CString& rMessage) const
{	
	string s;
	if (IlwisWinApp::fHelpNumbers())
		s = String("%i: %S", nID, ILWSF("dsc", nID));
	else
		s = ILWSF("dsc", nID);
	rMessage = s.c_str();
}


void CatalogFrameWindow::GetToolText(UINT uID, CString& sMess) const
{
	sMess = ILWSF("men", uID).c_str();
	sMess.Replace(8,0);
	if ("cd .." == sMess)
		return;
	sMess.Replace('.',0); // ... (elipses)
}


#define _countof(array) (sizeof(array)/sizeof(array[0]))

BOOL CatalogFrameWindow::OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
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

void CatalogFrameWindow::resize(CSize& sz) // change size (increment)
{
	zRect rExt;
	GetWindowRect(rExt);
	rExt.right() += sz.cx;
	rExt.bottom() += sz.cy;
	move(rExt);
};


#define sMen(ID) ILWSF("men",ID).c_str()

void CatalogFrameWindow::OnHelp() 
{
	IlwWinApp()->showHelp(help);
}

LRESULT CatalogFrameWindow::OnCommandHelp(WPARAM, LPARAM lParam)
{
	OnHelp();
	return TRUE;
}

void CatalogFrameWindow::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	if (!bActivate) 
		return;

	CatalogDocument* doc = dynamic_cast<CatalogDocument*>(GetActiveDocument());
	if (doc)
	{
		IlwWinApp()->SetCatalogDocument(doc);
		String sPath(doc->sGetPathName());
		// happens when initializing, !REGKEY is used to be able to start a catdoc with entries of the 
		// registry, CDocument at this moment still has this "path"
		if (sPath.find("!REGKEY") != -1) // 
			sPath = sPath.sHead("!");
		FileName fnPath(sPath);
		fnPath = FileName(sPath);
		if (fnPath.sExt != "")
			sPath = fnPath.sPath();
		SetCurrentDirectory(sPath.c_str());
		IlwWinApp()->SetCurDir(sPath.c_str());
		Catalog* cat = dynamic_cast<Catalog*>(doc->wndGetActiveView());
		if (cat)
			cat->SetFilterPane();
		SetIcon(doc->icon(), TRUE);
	}
}


void CatalogFrameWindow::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CMDIChildWnd::OnUpdateFrameTitle(bAddToTitle);
	char outPath[30], inPath[MAX_PATH];
	CatalogDocument* doc = dynamic_cast<CatalogDocument*>(GetActiveDocument());
	if ( doc )
	{
		CString str = doc->GetPathName();
		strcpy(inPath, str);
		::PathCompactPathEx(outPath, inPath, 30, 0);
		SetWindowText(outPath);
		SetIcon(doc->icon(), TRUE);
	}
}

void CatalogFrameWindow::OnRelatedTopics()
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



