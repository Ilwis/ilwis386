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
   $Log: /ILWIS 3.0/FormElements/ListCtrlList.cpp $
 * 
 * 4     30/06/00 15:48 Willem
 * The ListCtrl is now ILWIS type callback aware. Reacts on inplace edit
 * and regular change
 * 
 * 3     28/06/00 12:50 Willem
 * Completed the components
 * 
 * 2     26/06/00 15:51 Willem
 * New component to edit a list
 * 
 * 1     26/06/00 15:40 Willem

   ListCtrlList.cpp
   by Willem Nieuwenhuis, 26/6/00
   ILWIS Department ITC
  -----------------------------------------------*/

#include "Headers\stdafx.h"
#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Utils\InPlaceEdit.h"
#include "Client\FormElements\FieldList.h"
#include "Headers\Hs\Userint.hs"

BEGIN_MESSAGE_MAP(ListCtrlList, CListCtrl)
	//{{AFX_MSG_MAP(ListCtrlList)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO,    OnGetDispInfo)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT,	  OnEndLabelEdit)
	ON_NOTIFY_REFLECT(NM_CLICK,           OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK,          OnDoubleClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ListCtrlList::ListCtrlList()
{
    m_psPool[0] =
	m_psPool[1] =
	m_psPool[2] = NULL;
	m_iNextFree = 0;
}

ListCtrlList::~ListCtrlList()
{
}

void ListCtrlList::SetParentField(FieldLister* fcl)
{
	m_fclParent = fcl;
}

// Private functions
/*	OnGetDispInfo()
	This callback function is called by MFC whenever an item in the ListView
	needs to be displayed.
*/
void ListCtrlList::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if ( pDispInfo->item.iSubItem != 0) {
		int kk =0;
		++kk;
	}

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		int iSubItem = pDispInfo->item.iSubItem;

		CString sField;
		if (m_fclParent->iNrItems() > pDispInfo->item.iItem)
			sField = m_fclParent->sListItem(pDispInfo->item.iItem, iSubItem).scVal();
		else
			sField = "";

		LPTSTR psBuffer = AddPool(&sField);
		pDispInfo->item.pszText = psBuffer;
	}

	*pResult = 0;
}

// Extra function to temporarily buffer strings for ListView's
// space is needed for at most three strings (2 extra LVN_GETDISPINFO
// notifications) See also MFC with Visual C++, Blaszczak, page 383.
LPTSTR ListCtrlList::AddPool(CString* ps)
{
	LPTSTR psRetVal;
	int iOldest = m_iNextFree;

	m_sPool[m_iNextFree] = *ps;
	psRetVal = m_sPool[m_iNextFree].LockBuffer();
	m_psPool[m_iNextFree++] = psRetVal;
	m_sPool[iOldest].ReleaseBuffer();

	if (m_iNextFree == 3)
		m_iNextFree = 0;
	return psRetVal;
}

void ListCtrlList::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

}

void ListCtrlList::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	LPNMLISTVIEW pNMLV = (LPNMLISTVIEW)pNMHDR;

	m_iSubItem = pNMLV->iSubItem;
	m_iItem = pNMLV->iItem;
}

/*
	OnBeginLabelEdit() 
	OnEndLabelEdit() 
	These functions contain the functionality
	for the in place editting of listview items. (in any column)
	The OnClick handler set the appropriate m_iItem and m_iSubItem members
*/
void ListCtrlList::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	if (m_iSubItem == -1)
		return;

	*pResult = 1; // No processing needed after return

	CRect rcSubItem;
	GetSubItemRect(m_iItem, m_iSubItem, LVIR_LABEL, rcSubItem);
	int margin = 2;
	if (m_iSubItem > 0)
		rcSubItem.left += 2; // columns other than column 0 are indented 2 pixels extra!
	rcSubItem.top -= margin;
	rcSubItem.bottom += margin;

	String sLabelText = String(GetItemText(m_iItem, m_iSubItem));

	// Create the InPlaceEdit; there is no need to delete it afterwards, it will destroy itself
	DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL;
	InPlaceEdit *m_Edit = new InPlaceEdit(m_iItem, m_iSubItem, sLabelText.scVal());
	m_Edit->Create(style, rcSubItem, this, m_fclParent->Id());
}

void ListCtrlList::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.pszText != 0)
	{
		String sField(pDispInfo->item.pszText);
		sField.sTrimSpaces();

		m_fclParent->SetListItem(m_iItem, sField);
		m_fclParent->CallChangeCallback();

		Update(m_iItem);
	}

	*pResult = 0;
}

afx_msg void ListCtrlList::OnChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_fclParent->CallChangeCallback();
}

void ListCtrlList::getSelectedIndex(vector<int>& indexes) {
	UINT i, uSelectedCount = GetSelectedCount();
	int  nItem = -1;

	if (uSelectedCount > 0)
	{
		for (i=0;i < uSelectedCount;i++)
		{
			nItem = GetNextItem(nItem, LVNI_SELECTED);
			if ( nItem != -1) {
				indexes.push_back(nItem);	
			}
		}
	}
}