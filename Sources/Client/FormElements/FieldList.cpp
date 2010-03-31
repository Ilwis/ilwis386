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
   $Log: /ILWIS 3.0/FormElements/FieldList.cpp $
 * 
 * 4     30/06/00 15:47 Willem
 * The listctrl can now handle dynamic resizing
 * Added callback mechanism
 * 
 * 3     28/06/00 12:50 Willem
 * Completed the components
 * 
 * 2     26/06/00 15:51 Willem
 * New component to edit a list
 * 
 * 1     26/06/00 15:40 Willem

   FieldList.cpp
   by Willem Nieuwenhuis, 26/6/00
   ILWIS Department ITC
  -----------------------------------------------*/

#include "Headers\stdafx.h"
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldList.h"
#include <vector>

FieldLister::FieldLister(FormEntry* feParent, vector<String> &vsList)
	: FormEntry(feParent, 0, true), m_vsList(vsList)
{
	psn->iMinWidth = psn->iWidth = 100;
	psn->iMinHeight = psn->iHeight = 150;
}

FieldLister::~FieldLister()
{
}

void FieldLister::create()
{
	zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
	zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
	CRect rect(pntFld, dimFld);

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | LVS_REPORT;
	dwStyle |= LVS_EDITLABELS | LVS_OWNERDATA | LVS_SHOWSELALWAYS;
	dwStyle &= ~(LVS_SORTASCENDING | LVS_SORTDESCENDING);
	m_clctrl.Create(dwStyle, rect, frm()->wnd(), Id());
	m_clctrl.SetParentField(this);

	BuildColumns();

	Fill();

	CreateChildren();

	m_clctrl.ShowWindow(SW_HIDE);
}

String& FieldLister::sListItem(int iItem)
{
	return m_vsList[iItem];
}

void FieldLister::SetListItem(int iItem, const String& sField)
{
	if (iItem >= m_vsList.size())
		m_vsList.resize(iItem + 1);

	m_vsList[iItem] = sField;
}

int FieldLister::iNrItems()
{
	return m_vsList.size();
}

int FieldLister::iRowCount()
{
	if (m_clctrl.GetSafeHwnd() != 0)
		return m_clctrl.GetItemCount();
	else
		return 0;
}

void FieldLister::SetRowCount(int iNrItems)
{
	if (m_clctrl.GetSafeHwnd() != 0)
		m_clctrl.SetItemCountEx(iNrItems, LVSICF_NOSCROLL);

	if (iNrItems > m_vsList.size())
		m_vsList.resize(iNrItems);
}

void FieldLister::BuildColumns()
{
	int iCheckWidth = ::GetSystemMetrics(SM_CXMENUCHECK);

	int iTotalWidth = 0;
	int iCurCol = 0;
	CSize isHeader;
	CString sColName, sDummy;

	sColName = "Column width";

	sDummy = CString('x', 15);
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
}

void FieldLister::Fill()

{
	SetRowCount(m_vsList.size());
}

void FieldLister::CallChangeCallback()
{
	if (_npChanged)
		(_cb->*_npChanged)(0);
}

void FieldLister::show(int sw)
{
	if (m_clctrl.GetSafeHwnd() != NULL) {
			m_clctrl.ShowWindow(sw);
	}
}