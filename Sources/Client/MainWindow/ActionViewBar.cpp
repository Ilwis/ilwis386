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
// ActionViewBar.cpp: implementation of the ActionViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Client\MainWindow\ActionViewBar.h"
#include "Client\MainWindow\ActionTreeCtrl.h"
#include "Client\MainWindow\ActionFinder.h"
#include "Engine\Base\File\Directory.h"
#include "Client\MainWindow\NavigatorTreeCtrl.h"
#include "Client\ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\messages.h"
#include "Headers\Hs\Mainwind.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(ActionTabs, CTabCtrl)
	ON_NOTIFY_REFLECT( NM_CLICK, OnTabPressed)
END_MESSAGE_MAP() 

ActionTabs::~ActionTabs()
{
	delete atc;
	delete nav;
	delete afnd;
}

int ActionTabs::Create(ActionViewBar *av)
{
	iCurrIndex = iUNDEF;
	CRect rctClient(10,30, 250, 300);
	//av->GetClientRect(&rctClient);

	//rctClient.DeflateRect(50, 50);

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TCS_MULTILINE;
	int ir = CTabCtrl::Create(dwStyle, rctClient, av, 1032);

	CFont * fnt = IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium);
	SetFont(fnt);

	atc = new ActionTreeCtrl;
	atc->Create(this, av);
	nav = new NavigatorTreeCtrl(this);
	afnd = new ActionFinder();
	afnd->Create(this);

//	SetImageList(&(IlwWinApp()->ilSmall));
	InsertItem(0, TR("Operation-Tree").c_str());
	InsertItem(1, TR("Navigator").c_str());
	InsertItem(2, TR("Finder").c_str());
	atc->ShowWindow(SW_SHOW);
	nav->ShowWindow(SW_HIDE);
	afnd->ShowWindow(SW_HIDE);

	return 1;
}

void ActionTabs::OnSize( UINT nType, int cx, int cy )
{
	MoveWindow(0,0,cx, cy);
	CRect rct, rct2;
	GetItemRect(0, &rct2);
	int iHeight = rct2.Height() * GetRowCount();
	GetClientRect(&rct);
	rct.left += 5;
	rct.top += iHeight + 5;
	rct.right -= 5;
	rct.bottom -= 5;
	if (atc->GetSafeHwnd() != NULL)
  {
		atc->MoveWindow(&rct);
	}
	//if (alc->GetSafeHwnd() != NULL)
	//{
	//	alc->MoveWindow(&rct);
	//	LVCOLUMN col;
	//	col.mask = LVCF_WIDTH;
	//	col.cx = rct.Width() - 20;
	//	alc->SetColumn(0, &col);
	//}
	if (nav->GetSafeHwnd() != NULL)
	{
		nav->MoveWindow(&rct);
	}
	if (afnd->GetSafeHwnd() != NULL)
	{
		afnd->MoveWindow(&rct);
		afnd->size();
	}
}

void ActionTabs::OnTabPressed( NMHDR * pNotifyStruct, LRESULT* result )
{
	int iIndex = GetCurSel();
	if ( iIndex == 2 && iCurrIndex != iIndex )
			nav->Refresh();
	
	atc->ShowWindow(SW_HIDE);
	nav->ShowWindow(SW_HIDE);
	afnd->ShowWindow(SW_HIDE);

	switch( iIndex)
	{
		case 0:
			atc->ShowWindow(SW_SHOW); break;
		case 1:
			nav->ShowWindow(SW_SHOW); break;
		case 2:
			afnd->ShowWindow(SW_SHOW); break;
	}
	iCurrIndex = iIndex;
}

void ActionTabs::AddToHistory(const String& sFn)
{
	nav->AddToHistory(Directory(sFn));
}

void ActionTabs::SaveSettings(IlwisSettings& settings)
{
	int iIndex = GetCurSel();
	settings.SetValue("ActiveTab", iIndex);	
}

void ActionTabs::LoadSettings(IlwisSettings& settings)
{
	int iIndex = settings.iValue("ActiveTab",0);	
	SetCurSel(iIndex);
	OnTabPressed(NULL, NULL);
}

void ActionTabs::Refresh()
{
	nav->Refresh();
}
//----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( ActionViewBar, CSizingControlBar )
	//{{AFX_MSG_MAP( ActionViewBar )
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


ActionViewBar::ActionViewBar()
: tabs(0)
{
}

ActionViewBar::~ActionViewBar()
{
	delete tabs;
}

void ActionViewBar::OnUpdateCmdUI(CFrameWnd* pParent, BOOL)
{
	tabs->nav->OnUpdateCmdUI();
}


BOOL ActionViewBar::Create(CWnd* pParent)
{
	if (!CSizingControlBar::Create("", pParent, CSize(200,500), TRUE, 125))
		return FALSE;
  m_dwSCBStyle |= SCBS_SHOWEDGES;

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	SetWindowText(TR("Operations").c_str());

	tabs = new ActionTabs;
	tabs->Create(this);
	return TRUE;
}

void ActionViewBar::OnSize(UINT nType, int cx, int cy)
{
	if (0 == tabs)
		return;
	tabs->OnSize(nType,cx,cy);
}

void ActionViewBar::AddToHistory(const String& sFn)
{
	tabs->AddToHistory(sFn);
}

void ActionViewBar::SaveSettings(IlwisSettings& settings)
{
	tabs->SaveSettings(settings);
}

void ActionViewBar::LoadSettings(IlwisSettings& settings)
{
	tabs->LoadSettings(settings);
}

void ActionViewBar::Refresh()
{
	tabs->Refresh();	
}

