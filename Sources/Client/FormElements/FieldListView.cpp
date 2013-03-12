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
#include "Engine\DataExchange\TableExternalFormat.h"
#include "Client\Editors\Utils\InPlaceEdit.h"
#include "Client\FormElements\FieldListView.h"
#include "Client\ilwis.h"
#include <vector>

BEGIN_MESSAGE_MAP(FLVColumnListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(FLVColumnListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchangedList)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO,    OnGetDispInfo)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT,	  OnEndLabelEdit)
	ON_NOTIFY_REFLECT(NM_CLICK,           OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK,          OnDoubleClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

FLVColumnListCtrl::FLVColumnListCtrl(FormEntry *par) : BaseZapp(par), caller(0), function(0)
{

}

void FLVColumnListCtrl::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) {
	fProcess(Event(LVN_ITEMCHANGED, GetDlgCtrlID(), (LPARAM)pNMHDR)); 
}

void FLVColumnListCtrl::SetParent(FieldListView *view) {
	parentFormEntry = view;
}

void FLVColumnListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		int iSubItem = pDispInfo->item.iSubItem;
		int item = pDispInfo->item.iItem;
		pDispInfo->item.pszText = parentFormEntry->item(item, iSubItem);
		if ( iSubItem == 0 &&  pDispInfo->item.pszText != 0) {
			FileName fn( pDispInfo->item.pszText);
			if ( fn.sExt != "") {
				pDispInfo->item.iImage = IlwWinApp()->iImage(fn.sExt);
				pDispInfo->item.mask = LVIF_TEXT | LVIF_IMAGE;
				tempString = fn.sFile;
				pDispInfo->item.pszText = tempString.sVal();
			}
		}
	}

	*pResult = 0;
}

void FLVColumnListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	LPNMLISTVIEW pNMLV = (LPNMLISTVIEW)pNMHDR;

	m_iSubItem = pNMLV->iSubItem;
	m_iItem = pNMLV->iItem;
}

void FLVColumnListCtrl::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	
	OnBeginLabelEdit(pNMHDR, pResult);
}

void FLVColumnListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{

	*pResult = 0;

	if (m_iSubItem == -1)
		return;
	if ( parentFormEntry->m_colInfo[m_iSubItem].edit == false)
		return;


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
	m_Edit = new InPlaceEdit(m_iItem, m_iSubItem, sLabelText.c_str());
	m_Edit->Create(style, rcSubItem, this, 3949);
}

void FLVColumnListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) {
	if (m_iSubItem == -1)
		return;
	if ( parentFormEntry->m_colInfo[m_iSubItem].edit == false)
		return;

	if ( m_Edit) {
		CString text;
		m_Edit->GetWindowText(text);
		String s(text);
		parentFormEntry->setItemText(m_iItem, m_iSubItem,text);
		if ( caller && function) {
			(caller->*function)(m_iItem, m_iSubItem, text);
		}
	}
}

void FLVColumnListCtrl::setItemChangedCallback(CCmdTarget *call, NotifyItemChangedProc proc){
	caller = call;
	function = proc;
}


FieldListView::FieldListView(FormEntry* feParent, const vector<FLVColumnInfo> &colInfo, long _extraStyles)
: FormEntry(feParent, 0, true), extraStyles(_extraStyles), m_clctrl(0)
{
	for(int i = 0; i < colInfo.size(); ++i) {
		m_colInfo.push_back(colInfo[i]);
	}
	int iTotalWidth = 0;
	for(vector<FLVColumnInfo>::const_iterator cur = m_colInfo.begin(); cur < m_colInfo.end(); ++cur) {
		iTotalWidth += (*cur).width;
	}
	psn->iMinWidth = psn->iWidth = iTotalWidth;
	psn->iMinHeight = psn->iHeight = 150;

//  setHelpItem(htpUiDataType);
}

FieldListView::~FieldListView()
{
	delete m_clctrl;
}

int FieldListView::iNrCols()
{
	return m_colInfo.size();
}

void FieldListView::setItemChangedCallback(CCmdTarget *call, NotifyItemChangedProc proc){
	if ( m_clctrl)
		m_clctrl->setItemChangedCallback(call, proc);
}

void FieldListView::create()
{
	zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
	zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
	CRect rect(pntFld, dimFld);

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | LVS_REPORT;
	dwStyle |= LVS_EDITLABELS | LVS_OWNERDATA | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS;
	dwStyle &= ~(LVS_SORTASCENDING | LVS_SORTDESCENDING);
	m_clctrl = new FLVColumnListCtrl(this);
	m_clctrl->Create(dwStyle, rect, frm()->wnd(), Id());
	m_clctrl->SetImageList(&IlwWinApp()->ilSmall, LVSIL_SMALL);
	m_clctrl->SetExtendedStyle(LVS_EX_FULLROWSELECT | extraStyles);
	m_clctrl->SetParent(this);
	m_clctrl->SetFont(_frm->fnt);
	m_clctrl->ShowWindow(SW_HIDE);

	BuildColumns();

	Fill();

    CreateChildren();

}

void FieldListView::show(int v) {
	if ( m_clctrl && m_clctrl->GetSafeHwnd() != NULL)
		m_clctrl->ShowWindow(v)  ;
}

void FieldListView::clear() {
	data.clear();
}

void FieldListView::setData(int row, const vector<String>& rowdata) {
	if ( row == -1) {
		data.clear();
		return ;
	}
	if ( row >= data.size())
		throw ErrorObject(TR("Illegal row number when changing data"));
	data[row].clear();
	for(vector<String>::const_iterator cur = rowdata.begin(); cur != rowdata.end(); ++cur) {
		data[row].push_back(*cur);	
	}
}

void FieldListView::AddData(const vector<String>& rowdata) {
	SetRowCount(iRowCount() + 1);
	data.push_back(vector<String>());
	setData(data.size() - 1, rowdata);
}

char *FieldListView::item(int row, int col) {
	if ( iRowCount() < data.size())
		SetRowCount(data.size());
	if ( row < data.size())
		return data[row][col].sVal();
	return 0;
}

void FieldListView::setItemText(int row, int col, const String& txt) {
	if ( row < data.size())
		data[row][col] = txt;
}

void FieldListView::BuildColumns()
{
	//int iCheckWidth = ::GetSystemMetrics(SM_CXMENUCHECK);

	int iTotalWidth = 0;
	int iCurCol = 0;
	CString sColName, sDummy;
	sDummy = CString('x', 15);
	CSize sz = m_clctrl->GetStringWidth(sDummy);

	for(vector<FLVColumnInfo>::const_iterator cur = m_colInfo.begin(); cur < m_colInfo.end(); ++cur) {
		m_clctrl->InsertColumn(iCurCol++, (*cur).columnName.c_str(), LVCFMT_LEFT, (*cur).width);
		iTotalWidth += (*cur).width;
	}
}

int FieldListView::iRowCount()
{
	if (m_clctrl->GetSafeHwnd() != 0)
		return m_clctrl->GetItemCount();
	else
		return 0;
}

void FieldListView::SetRowCount(int iNrItems)
{
	if (m_clctrl->GetSafeHwnd() != 0)
		m_clctrl->SetItemCountEx(iNrItems, LVSICF_NOSCROLL);

}

void FieldListView::SetColWidth(int iCol, int iWidth)
{
	m_clctrl->SetColumnWidth(iCol, iWidth);
}

void FieldListView::Fill()
{
	SetRowCount(1);
}

void FieldListView::CallChangeCallback()
{
	if (_npChanged)
		(_cb->*_npChanged)(0);
}

void FieldListView::update() {
	m_clctrl->RedrawItems(0, m_clctrl->GetItemCount());
	m_clctrl->UpdateWindow();
}

void FieldListView::setSelectedRows(vector<int>& rowNumbers) {
	for(int i=0; i < rowNumbers.size(); ++i) {
		m_clctrl->SetItemState(rowNumbers[i], LVIS_SELECTED, LVIS_SELECTED);
	}
}

void FieldListView::getSelectedRowNumbers(vector<int>& rowNumbers) const{
	POSITION pos = m_clctrl->GetFirstSelectedItemPosition();
	if (pos == NULL)
	   return;
	else
	{
	   while (pos)
	   {
		  int nItem = m_clctrl->GetNextSelectedItem(pos);
		  rowNumbers.push_back(nItem);	
	   }
}

}
