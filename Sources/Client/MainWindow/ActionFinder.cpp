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
#include "Headers\stdafx.h"
#include "Client\Headers\formelementspch.h"
#include "Client\MainWindow\ActionFinder.h"
#include "Client\MainWindow\ActionViewBar.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\ACTION.H"
#include "Client\MainWindow\ACTPAIR.H"
#include "Headers\messages.h"
#include <afxole.h>
#include "Headers\constant.h"
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"

class ActionFinderListDropTarget: public COleDropTarget
{
public:
	BOOL Register(ActionFinderList* acttc)
	{
		atc = acttc;
		return COleDropTarget::Register(atc);
	}
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
	{
		return atc->OnDragEnter(pDataObject, dwKeyState, point);
	}
	void OnDragLeave(CWnd* pWnd)
	{
		atc->OnDragLeave();
	}
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
	{
		return atc->OnDragOver(pDataObject, dwKeyState, point);
	}
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
	{
		return atc->OnDrop(pDataObject, dwKeyState, point);
	}
private:
	ActionFinderList* atc;
};
//-----------------------------------------------------------
BEGIN_MESSAGE_MAP(ActionFinderEdit, CEdit)
	ON_WM_KEYUP()
END_MESSAGE_MAP()

ActionFinderEdit::ActionFinderEdit(ActionFinderList* lst) : list(lst){
}

void ActionFinderEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if ( nChar >= 32 && nChar < 256) {
		CString txt;
		GetWindowTextA(txt);
		String filter(txt);
		filter.toLower();
		list->find(filter);
		list->Invalidate();
	}
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);

	if ( nChar == 8) {
		CString txt;
		GetWindowTextA(txt);
		String filter(txt);
		filter.toLower();
		list->find(filter);
		list->Invalidate();
	}
}

//------------------------------------------------------------
ActionFinder::ActionFinder() {
}

int ActionFinder::Create(ActionTabs *tabs){
	CRect rct;
	tabs->GetClientRect(&rct);
	BOOL succes = CWnd::Create(NULL,"Finder",WS_CHILD | WS_VISIBLE | WS_BORDER,rct, tabs, 8743);
	list = new ActionFinderList();
	edit = new ActionFinderEdit(list);
	edit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER,CRect(0,0,rct.right,16),this,1934);
	list->Create(this);
	return 1;
}

void ActionFinder::size(){
	CRect rct;
	GetClientRect(rct);
	list->MoveWindow(CRect(0,18,rct.right, rct.bottom - 18));
}

//------------------------------------------------------------
BEGIN_MESSAGE_MAP(ActionFinderList, CListCtrl)
	//{{AFX_MSG_MAP(ActionFinderList)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
	ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnGetInfoTip)
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ActionFinderList::ActionFinderList()
{
	odt = new ActionFinderListDropTarget;
}

ActionFinderList::~ActionFinderList()
{
	delete odt;
}

void ActionFinderList::find(const String& filter) {
	const ActionList* actList = IlwWinApp()->acl();
	int iIndex = 0;
	SetItemCount(40);
	this->DeleteAllItems();
	for (SLIterCP<Action> iter(actList); iter.fValid(); ++iter) 
	{
		Action* act = iter();	
		String sName = act->sName();
		if ( sName == "" ) continue;
		String tmp(sName);
		if ( filter != "" && tmp.toLower().find(filter) == string::npos)
			continue;

		// check on double items (same name, same action) and skip them
		LVFINDINFO info;
		info.flags = LVFI_STRING;
		info.psz = sName.sVal();
		int iFound = FindItem(&info); 
		if (iFound > -1) {
			Action* actFound = (Action*) GetItemData(iFound);
			if (actFound->sAction() == act->sAction())
				continue;
		}

		int iImg = IlwWinApp()->iImage(act->sIcon());
		LVITEM item;
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		item.pszText = sName.sVal();

		item.iItem = iIndex;
		item.iSubItem = 0;
		item.iImage = iImg;
		item.lParam = (LPARAM) act;
		InsertItem(&item);
		//SetItemData(iIndex++, iIndex);
	}
	SetItemCount(iIndex);
}

int ActionFinderList::Create(CWnd *tabs)
{
	CRect rect;
	tabs->GetClientRect(&rect);
	CRect rct(0,18,rect.right, rect.bottom - 18);
	int iRet = CListCtrl::Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_SINGLESEL | LVS_REPORT |
		LVS_SORTASCENDING | LVS_NOCOLUMNHEADER , rct, tabs, 1945);
	//	SetExtendedStyle(LVS_EX_INFOTIP); -- switch off
	

	SetImageList(&IlwWinApp()->ilSmall, LVSIL_SMALL);
	int iImgDflt = IlwWinApp()->iImage("Exe16Ico");
	int iImgExe = iImgDflt;

	InsertColumn(0, "Results", LVCFMT_LEFT, rct.Width());
	find();
	odt->Register(this);

	return iRet;
}

void ActionFinderList::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iNr = GetSelectedCount();
	if (0 == iNr) return;
	POSITION pos = GetFirstSelectedItemPosition();
	int id = GetNextSelectedItem(pos);
	Action* act = (Action*) GetItemData(id);
	*pResult = 0;
	if (0 != act)
	{
		String sCmd = act->sAction();
		char* str = sCmd.sVal();
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
		*pResult = 1;
	}
}

void ActionFinderList::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDblclk(pNMHDR, pResult);
}

void ActionFinderList::OnGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVGETINFOTIP git = (LPNMLVGETINFOTIP) pNMHDR;
	Action* act = (Action*) GetItemData(git->iItem);
	String s = act->sDescription();
	strcpy(git->pszText, s.scVal());
}


DROPEFFECT ActionFinderList::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;
	int iIndex = HitTest(point);
	if (-1 == iIndex)
		return DROPEFFECT_NONE;
	Action* act = (Action*)GetItemData(iIndex);	
	if (0 == act) 
		return DROPEFFECT_NONE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	if (0 == hDrop)
		return DROPEFFECT_NONE;
	bool fOk = true;

	char sFileName[MAX_PATH+1];
	int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
	if (0 == iFiles)
		return DROPEFFECT_NONE;
	String sMsg;
	String sAct = act->sAction();
	for (int i = 0; i < iFiles; ++i) {
		DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
		FileName fn(sFileName);
		ActionPair* ap = IlwWinApp()->apl()->ap(sAct, fn);
		if (0 == ap) {
			fOk = false;
			break;
		}
		if (i == 0)
			sMsg = ap->sDescription(fn);
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);

	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	CWnd* pMessageBar = pFrameWnd->GetMessageBar();
	if (pMessageBar)
		pMessageBar->SetWindowText(sMsg.sVal());

	if (fOk)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

void ActionFinderList::OnDragLeave() 
{
	// TODO: Add your specialized code here and/or call the base class
}

DROPEFFECT ActionFinderList::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	return OnDragEnter(pDataObject, dwKeyState, point);
}

BOOL ActionFinderList::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return FALSE;
	int iIndex = HitTest(point);
	if (-1 == iIndex)
		return FALSE;
	Action* act = (Action*)GetItemData(iIndex);	
	if (0 == act) 
		return FALSE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	int iFiles = 0;
	FileName* afn = 0;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		afn = new FileName[iFiles];
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
			afn[i] = FileName(sFileName);
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);
	// if send by SendMessage() prevent deadlock
	ReplyMessage(0);

	FileName fn = afn[0];
	String sAct = act->sAction();
	ActionPair* ap = IlwWinApp()->apl()->ap(sAct, fn);
	if (0 == ap) 
		return FALSE;
	String sCmd = ap->sExec(fn);
	for (int i = 1; i < iFiles; ++i) {
		FileName fn = afn[i];
		sCmd &= " ";
		sCmd &= fn.sFullNameQuoted();
	}
	IlwisWinApp* iwa = IlwWinApp();
	char* str = sCmd.sVal();
	iwa->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
	return TRUE;
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void ActionFinderList::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);
	int iIndex = HitTest(pt);
	if (-1 == iIndex)
		return;
	Action* act = (Action*)GetItemData(iIndex);	
	if (0 == act) 
		return;
	CMenu men;
	men.CreatePopupMenu();
	pmadd(ID_ACT_RUN);
	if (act->iHelp() > 0)
		pmadd(ID_ACT_HELP);
	switch (men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, this))
	{
	case ID_ACT_RUN:
		{
			String sCmd = act->sAction();
			IlwisWinApp* iwa = IlwWinApp();
			char* str = sCmd.sVal();
			iwa->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
		} break;
	case ID_ACT_HELP:
		String 	sHelpFile (ChmFinder::sFindChmFile("ilwis.chm"));
		String sHelpString ("%S>sec", sHelpFile);
		::HtmlHelp(m_hWnd,
			sHelpString.sVal(),
			HH_HELP_CONTEXT,
			act->iHelp());
		break;
	}
}

void ActionFinderList::OnRButtonDown(UINT nFlags, CPoint point)
{ // just here to prevent that default implementation 
	// prevents a WM_CONTEXTMENU to be sent
}

void ActionFinderList::OnMouseMove(UINT nFlags, CPoint point) 
{
	CListCtrl::OnMouseMove(nFlags, point);

	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	CWnd* pMessageBar = pFrameWnd->GetMessageBar();
	String sMsg;
	int iIndex = HitTest(point);
	if (-1 != iIndex) {
		Action* act = (Action*)GetItemData(iIndex);	
		if (0 != act) {
			sMsg = act->sDescription();
		}
	}
	if (pMessageBar)
		pMessageBar->SetWindowText(sMsg.sVal());
}

void ActionFinderList::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iNr = GetSelectedCount();
	if (0 == iNr) return;
	POSITION pos = GetFirstSelectedItemPosition();
	int id = GetNextSelectedItem(pos);
	Action* act = (Action*) GetItemData(id);
	if (0 != act)
	{
		String sCmd = act->sAction();
		IlwisWinApp* iwa = IlwWinApp();
		iwa->SetCommandLine(sCmd);
	}
}

BOOL ActionFinderList::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam) 
		{
		case VK_RETURN:
			int iNr = GetSelectedCount();
			if (0 == iNr) 
				break;
			POSITION pos = GetFirstSelectedItemPosition();
			int id = GetNextSelectedItem(pos);
			Action* act = (Action*) GetItemData(id);
			if (0 != act) {
				String sCmd = act->sAction();
				char* str = sCmd.sVal();
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
			}
			return TRUE;
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}


