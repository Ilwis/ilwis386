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
// ActionTreeCtrl.cpp : implementation file
//

#include "Headers\stdafx.h"
#include "Client\Headers\formelementspch.h"
#include "Client\MainWindow\ActionTreeCtrl.h"
#include "Client\MainWindow\ActionViewBar.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\ACTION.H"
#include "Client\MainWindow\ACTPAIR.H"
#include "Headers\messages.h"
#include <afxole.h>
#include "Headers\constant.h"
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(ActionTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(ActionTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

class ActionTreeCtrlDropTarget: public COleDropTarget
{
public:
	BOOL Register(ActionTreeCtrl* acttc)
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
	ActionTreeCtrl* atc;
};



ActionTreeCtrl::ActionTreeCtrl()
{
	odt = new ActionTreeCtrlDropTarget;
}

ActionTreeCtrl::~ActionTreeCtrl()
{
	delete odt;
}

void ActionTreeCtrl::Create(ActionTabs *tabs, CWnd* pWnd)
{
	CRect rect;
	CTreeCtrl::Create(WS_CHILD|WS_VISIBLE|WS_BORDER|TVS_HASBUTTONS|TVS_LINESATROOT, 
		rect, tabs, 100);
	SetImageList(&IlwWinApp()->ilSmall, TVSIL_NORMAL);

	const ActionList* actList = IlwWinApp()->acl();
	int iImgDflt = IlwWinApp()->iImage("ExeIcoL");
	int iImgExe = iImgDflt;

	HTREEITEM htiTop, htiMid, htiOpt;
	String sTop, sMid, sAction, sLastTop="-", sLastMid;
	for (SLIterCP<Action> iter(actList); iter.fValid(); ++iter) 
	{
		Action* act = iter();
		sAction = act->sName();
		if ("" ==	 sAction) {
			iImgExe = IlwWinApp()->iImage(act->sIcon());
			continue;
		}
		sTop = act->sMenOpt();
		if (sLastTop != sTop) {
			if ("" == sTop)
				htiTop = TVI_ROOT;
			else {
				CString str(sTop.scVal());
				str.Remove('&');
				iImgExe = IlwWinApp()->iImage(const_cast<ActionList *>(actList)->getTopIcon(sTop));
				htiTop = InsertItem(str, iImgExe, iImgExe);
			}
			sLastTop = sTop;
			sLastMid = "-";
			htiMid = htiTop;
		}
		sMid = act->sMidOpt();
		if (sLastMid != sMid) {
			if ("" == sMid)
				htiMid = htiTop;
			else {
				CString str(sMid.scVal());
				str.Remove('&');
				String entry = act->sMenOpt() + sMid;
				iImgExe = IlwWinApp()->iImage(const_cast<ActionList *>(actList)->getMiddleIcon(entry));
				htiMid = InsertItem(str, iImgExe, iImgExe, htiTop);
			}
			sLastMid = sMid;
		}
		int iImg = IlwWinApp()->iImage(act->sIcon());
		htiOpt = InsertItem(sAction.scVal(), iImg, iImg, htiMid);
		SetItemData(htiOpt, (DWORD)act);
		iImgExe = iImgDflt;
	}

	odt->Register(this);
}


void ActionTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hti = pNMTreeView->itemNew.hItem;
	Action* act = (Action*)GetItemData(hti);	
	if (0 != act)
	{
		String sCmd = act->sAction();
		IlwisWinApp* iwa = IlwWinApp();
		iwa->SetCommandLine(sCmd);
	}
	*pResult = 0;
}

void ActionTreeCtrl::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hti = GetSelectedItem();
	Action* act = (Action*)GetItemData(hti);	
	*pResult = 0;
	if (0 != act)
	{
		String sCmd = act->sAction();
		IlwisWinApp* iwa = IlwWinApp();
		char* str = sCmd.sVal();
		iwa->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
		*pResult = 1;
	}
}

void ActionTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnMouseMove(nFlags, point);

	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	CWnd* pMessageBar = pFrameWnd->GetMessageBar();
	String sMsg;
	HTREEITEM hti = HitTest(point);
	if (hti) {
		Action* act = (Action*)GetItemData(hti);	
		if (0 != act) {
			sMsg = act->sDescription();
		}
	}
	if (pMessageBar)
		pMessageBar->SetWindowText(sMsg.sVal());
}

DROPEFFECT ActionTreeCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;
	HTREEITEM hti = HitTest(point);
	if (0 == hti)
		return DROPEFFECT_NONE;
	Action* act = (Action*)GetItemData(hti);	
	if (0 == act) 
		return DROPEFFECT_NONE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	if (0 == hDrop)
		return DROPEFFECT_NONE;
	bool fOk = false;

	char sFileName[MAX_PATH+1];
	int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
	if (0 == iFiles)
		return DROPEFFECT_NONE;
	String sMsg;
	String sAct = act->sAction();
	for (int i = 0; i < iFiles; ++i) {
		fOk = true;
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

void ActionTreeCtrl::OnDragLeave() 
{
	// TODO: Add your specialized code here and/or call the base class
}

DROPEFFECT ActionTreeCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	return OnDragEnter(pDataObject, dwKeyState, point);
}

BOOL ActionTreeCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return FALSE;
	HTREEITEM hti = HitTest(point);
	if (0 == hti)
		return FALSE;
	Action* act = (Action*)GetItemData(hti);	
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

void ActionTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);
	HTREEITEM hti = HitTest(pt);
	if (hti) {
		Action* act = (Action*)GetItemData(hti);	
		if (0 != act) {
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
					//String sHelpFile (ChmFinder::sFindChmFile("ilwis.chm"));
					//String sHelpString ("%S>sec", sHelpFile);
					//::HtmlHelp(m_hWnd,
					//				sHelpString.sVal(),
					//				HH_HELP_CONTEXT,
					//				act->iHelp());
					break;
			}
		}
	}
}

void ActionTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{ // just here to prevent that default implementation 
	// prevents a WM_CONTEXTMENU to be sent
}


void ActionTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonDblClk(nFlags, point);

	TVHITTESTINFO tvi;
	tvi.pt = point;
	HTREEITEM hti = HitTest(&tvi);
	if (0 == (tvi.flags & TVHT_ONITEMBUTTON) && 0 != hti)
	{	// user selected an item
		Action* act = (Action*)GetItemData(hti);	
		if (0 != act)
		{
			String sCmd = act->sAction();
			IlwisWinApp* iwa = IlwWinApp();
			char* str = sCmd.sVal();
			iwa->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
		}
	}	// else user clicked on '+'
}

BOOL ActionTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN) 
	{
		switch (pMsg->wParam) 
		{
			case VK_RETURN: 
			{
				HTREEITEM hti = GetSelectedItem();
				Action* act = (Action*)GetItemData(hti);	
				if (0 != act)
				{
					String sCmd = act->sAction();
					IlwisWinApp* iwa = IlwWinApp();
					char* str = sCmd.sVal();
					iwa->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
				}
				return TRUE;
			} 
		}
	}
	return CTreeCtrl::PreTranslateMessage(pMsg);
}

