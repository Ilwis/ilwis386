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
// NavigatorCtrl.cpp : implementation file
//

#include "Headers\toolspch.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\File\Directory.h"
#include "Client\MainWindow\NavigatorCtrl.h"
#include "Client\MainWindow\NavigatorTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(NavigatorCtrl, CComboBoxEx)
	//{{AFX_MSG_MAP(NavigatorCtrl)
	ON_NOTIFY_REFLECT(CBEN_GETDISPINFO, OnGetdispinfo)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NavigatorCtrl

NavigatorCtrl::NavigatorCtrl()
: ntc(0)
{
}

NavigatorCtrl::~NavigatorCtrl()
{
	delete ntc;
}

BOOL NavigatorCtrl::Create(CWnd* wParent)
{
	CRect rect(0,0,150,150);
	DWORD dwStyle = WS_CHILD | 
									WS_VISIBLE |
							    //CBS_DROPDOWN; 
							    CBS_DROPDOWNLIST; 
	if (!CComboBoxEx::Create(dwStyle, rect, wParent, 100))
		return FALSE;

	SetExtendedStyle(0, CBES_EX_PATHWORDBREAKPROC);
	SetImageList(&IlwWinApp()->ilSmall);

	COMBOBOXEXITEM it;
	it.mask = CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;  
	it.iItem = 0;
	it.pszText = LPSTR_TEXTCALLBACK;
	it.iImage = IlwWinApp()->iImage("directory");
	it.iSelectedImage = it.iImage;
	it.iOverlay = 0;
	it.iIndent = 0;		
	it.lParam = 0;
	InsertItem(&it);

	SetCurSel(0);

	return TRUE;
}


void NavigatorCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMCOMBOBOXEX* nmc = (NMCOMBOBOXEX*) pNMHDR;

	if (nmc->ceItem.mask | CBEIF_TEXT) {
		nmc->ceItem.pszText = sCurDir.sVal();
	}
	
	*pResult = 0;
}

void NavigatorCtrl::SetCurDir(const String& str)
{ 
	if (sCurDir != str) {
		sCurDir = str;
		SetCurSel(0);
	}
}

void NavigatorCtrl::OpenTreeCtrl()
{
	TRACE("Open\n");
	if (0 == ntc) {
		ntc = new NavigatorTreeCtrl(this);
	}
	else {
		int iTime = GetTickCount();
		if (abs(ntc->iHideTime - iTime) < 100) {
			SetFocus();
			return;
		}
	}

	CRect rect;
	GetWindowRect(rect); 
	rect.top = rect.bottom + 1;
	rect.bottom += 250;
	//rect.right += 30;
	ntc->SetWindowPos(&wndTop,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW);
}

void NavigatorCtrl::HideTreeCtrl()
{
	TRACE("Hide\n");
	int iTime = GetTickCount();
	if (abs(ntc->iHideTime - iTime) < 100) 
		return;
	ntc->ShowWindow(SW_HIDE);
}


void NavigatorCtrl::OnDropdown() 
{
	TRACE("OnDropDown\n");
	PostMessage(CB_SHOWDROPDOWN, FALSE, 0);
}

void NavigatorCtrl::OnCloseup() 
{
	TRACE("OnCloseUp\n");
	if (ntc && ntc->IsWindowVisible())
		HideTreeCtrl();
	else 
		OpenTreeCtrl();
}

void NavigatorCtrl::AddToHistory(const String& fn)
{
	String sDir = fn;
	if ( fn.find("!REGKEY:") != -1)
		return;
	if ( sDir[fn.size() - 1] != '\\')
		sDir += '\\';
  if ( find(lsHistory.begin(), lsHistory.end(), sDir) == lsHistory.end())
	{
		lsHistory.push_front(sDir);
//		if ( ntc)
//			ntc->AddToHistoryNode(sDir);
			
	}
}

const list<String>& NavigatorCtrl::GetHistory()
{
	return lsHistory;
}
