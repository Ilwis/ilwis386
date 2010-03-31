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
/*-----------------------------------------------
   $Log: /ILWIS 3.0/UserInterfaceObjects/InPlaceNameEdit.cpp $
 * 
 * 3     26/05/00 10:26 Willem
 * ObjectLister is now passed as parameter
 * 
 * 2     26/05/00 10:18 Willem
 * InPlaceNameEdit now behaves as expected:
 * - ESC key removes inplace edit
 * - Object tree control is now properly set to previously selected file
 * 
 * 1     24/05/00 9:06 Willem
 * First version of the NameEdit as inplace control

   InPlaceNameEdit.cpp
   by Willem Nieuwenhuis, 22/5/00
   ILWIS Department ITC
  -----------------------------------------------*/

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\ObjectTreeCtrl.h"
#include "Client\Editors\Utils\InPlaceNameEdit.h"

BEGIN_MESSAGE_MAP(InPlaceNameEdit, NameEdit)
  ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

InPlaceNameEdit::InPlaceNameEdit(CWnd* wndParent, FormBase* fb, int iItem, int iSubItem, ObjectLister* ol, CRect rcPos, int id)
	: NameEdit(0, fb, rcPos.TopLeft(), id, String(), true, ol, true, rcPos.Width()),
		m_wndParent(wndParent),
		m_iItem(iItem),
		m_iSubItem(iSubItem)
{
	SetOwner(m_wndParent);
	SetParent(m_wndParent);
	m_fESC = false;

	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetFocus();
}

InPlaceNameEdit::~InPlaceNameEdit()
{
}

int InPlaceNameEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (NameEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void InPlaceNameEdit::OnNcDestroy()
{
	NameEdit::OnNcDestroy();

	delete this;
}

void InPlaceNameEdit::OnKillFocus(CWnd* pNewWnd)
{
	NameEdit::OnKillFocus(pNewWnd);

	ObjectTreeCtrl* potc = dynamic_cast<ObjectTreeCtrl*>(pNewWnd);
	if (!potc)
	{
		SendEndLabelNotify();

		DestroyWindow();
	}
}

BOOL InPlaceNameEdit::PreTranslateMessage(MSG* pMsg)
{
// prevent that RETURN, ESC are eaten by IsDialogMessage() check
// but allow other accelerators to function
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
		  case VK_ESCAPE: 
				DestroyWindow();
				return TRUE;
				break;
			case VK_RETURN:
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}
	return NameEdit::PreTranslateMessage(pMsg);
}

void InPlaceNameEdit::SendEndLabelNotify()
{
	String str = sName();

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.lParam = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = LPTSTR(str.scVal());
	dispinfo.item.cchTextMax = str.length();

	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);
}
