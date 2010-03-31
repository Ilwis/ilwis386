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
   $Log: /ILWIS 3.0/UserInterfaceObjects/InPlaceComboBox.cpp $
 * 
 * 1     29/05/00 10:53 Willem

   InPlaceComboBox.cpp
   by Willem Nieuwenhuis, 29/5/00
   ILWIS Department ITC
  -----------------------------------------------*/

#include "Client\Headers\formelementspch.h"
#include <vector>
#include "Engine\Base\DataObjects\strng.h"
#include "Client\Editors\Utils\InPlaceComboBox.h"

BEGIN_MESSAGE_MAP(InPlaceComboBox, CComboBox)
  ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

InPlaceComboBox::InPlaceComboBox(CWnd* wndParent, int iItem, int iSubItem, vector<String>& vcStrings, CRect rcPos, int id)
	: CComboBox(),
		m_wndParent(wndParent),
		m_iItem(iItem),
		m_iSubItem(iSubItem)
{
	unsigned int iStyle = CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL | WS_GROUP | CBS_AUTOHSCROLL | CBS_HASSTRINGS;
	CComboBox::Create(iStyle, rcPos, m_wndParent, id);
	m_fESC = false;

	vector<String>::iterator iter;
	for (iter = vcStrings.begin(); iter != vcStrings.end(); ++iter)
		AddString(iter->scVal());

	ShowWindow(SW_SHOW);

	SetOwner(m_wndParent);
	SetParent(m_wndParent);

	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetFocus();
}

InPlaceComboBox::~InPlaceComboBox()
{
}

int InPlaceComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void InPlaceComboBox::OnNcDestroy()
{
	CComboBox::OnNcDestroy();

	delete this;
}

void InPlaceComboBox::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	if (!m_fESC)
		SendEndLabelNotify();

	DestroyWindow();
}

BOOL InPlaceComboBox::PreTranslateMessage(MSG* pMsg)
{
// prevent that RETURN, ESC are eaten by IsDialogMessage() check
// but allow other accelerators to function
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		  case VK_ESCAPE:
				m_fESC = true;
				DestroyWindow();
				return TRUE;
				break;
			case VK_RETURN:
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

void InPlaceComboBox::SendEndLabelNotify()
{
	int iSel = GetCurSel();
	CString str;
	GetLBText(iSel, str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.lParam = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);
}
