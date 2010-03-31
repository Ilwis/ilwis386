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
// NavigatorBar.cpp : implementation file
//
#include "Headers\toolspch.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\NavigatorBar.h"
#include "Client\MainWindow\NavigatorCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NavigatorBar

BEGIN_MESSAGE_MAP(NavigatorBar, BaseBar)
	//{{AFX_MSG_MAP(NavigatorBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

NavigatorBar::NavigatorBar()
: nc(0)
{
}

NavigatorBar::~NavigatorBar()
{
	delete nc;
}

BOOL NavigatorBar::Create(CFrameWnd* pParent) 
{
	if (!BaseBar::Create(pParent, 126))
		return FALSE;
	nc = new NavigatorCtrl;
	nc->Create(this);
	iHeight = nc->GetItemHeight(-1) + 10;

	return TRUE;
}

void NavigatorBar::OnSize(UINT nType, int cx, int cy) 
{
	BaseBar::OnSize(nType, cx, cy);
	if (0 == nc)
		return;
	nc->MoveWindow(0,0,cx,100);
}

void NavigatorBar::OnUpdateCmdUI(CFrameWnd* pParent, BOOL)
{
	String sDir = IlwWinApp()->sGetCurDir();
	nc->SetCurDir(sDir);
}

BOOL NavigatorBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1 && lParam == 0) {
		CString str;
		nc->GetEditCtrl()->GetWindowText(str);
		IlwWinApp()->OpenDirectory(str);
		nc->SetCurDir("");
		String sDir = IlwWinApp()->sGetCurDir();
		nc->SetCurDir(sDir);
		return TRUE;
	}
	return BaseBar::OnCommand(wParam, lParam);
}

void NavigatorBar::AddToHistory(const String& sDir)
{
	if ( nc)
		nc->AddToHistory(sDir);
}

