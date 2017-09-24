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
   $Log: /ILWIS 3.0/FormElements/FieldColumnLister.cpp $
 * 
 * 11    11/21/02 8:15a Martin
 * extended the width for the column Undef Value to fit the header
 * 
 * 10    1/14/02 10:34a Martin
 * spelling error
 * 
 * 9     11/19/01 11:22a Martin
 * changed some of the texts of the columnsheaders
 * 
 * 8     19/01/01 9:53 Willem
 * Increased the default width of the columnlist control to fill out the
 * space on the table wizard better
 * 
 * 7     21/11/00 12:09 Willem
 * Added function ToggleAsKey(int)
 * 
 * 6     13/10/00 15:02 Willem
 * - The font of the ColumnLister is now set to form font
 * - Added extra columns: 'New Domain' (yesno type) and 'Width' (integer)
 * - Field will now call a callback function when a value is changed
 * 
 * 5     30/06/00 15:46 Willem
 * Added function to dynamically size the listctrl
 * 
 * 4     22/06/00 15:46 Willem
 * The positioner is now correctly initialized in the constructor instead
 * of in the create() function
 * 
 * 3     21/06/00 18:15 Willem
 * Improved version of the import table wizard
 * - better column selection handling
 * - scan is done each time the format selection is changed
 * 
 * 2     24/05/00 9:09 Willem
 * The Domain column is now twice as wide
 * 
 * 1     19/05/00 14:42 Willem
 * Add components for columnlist in import ascii table wizard

   FieldColumnLister.cpp
   by Willem Nieuwenhuis, 17/5/00
   ILWIS Department ITC
  -----------------------------------------------*/

#include "Headers\stdafx.h"
#include "Client\Headers\formelementspch.h"
#include "Engine\DataExchange\TableExternalFormat.h"
#include "Client\FormElements\FieldColumnLister.h"
#include "Client\ilwis.h"
#include <vector>

FieldColumnLister::FieldColumnLister(FormEntry* feParent, vector<ClmInfo> &colInfo)
	: FormEntry(feParent, 0, true), m_colInfo(colInfo)
{
	psn->iMinWidth = psn->iWidth = 450;
	psn->iMinHeight = psn->iHeight = 150;

//  setHelpItem(htpUiDataType);
}

FieldColumnLister::~FieldColumnLister()
{
}

ClmInfo& FieldColumnLister::ciColumn(int iCol)
{
	return m_colInfo[iCol];
}

int FieldColumnLister::iNrCols()
{
	return m_colInfo.size();
}

void FieldColumnLister::create()
{
	zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
	zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
	CRect rect(pntFld, dimFld);

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | LVS_REPORT;
	dwStyle |= LVS_OWNERDATA | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS;
	dwStyle &= ~(LVS_SORTASCENDING | LVS_SORTDESCENDING);
	m_clctrl.Create(dwStyle, rect, frm()->wnd(), Id());
	m_clctrl.SetImageList(&IlwWinApp()->ilSmall, LVSIL_SMALL);
	m_clctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_clctrl.SetParentField(this);
	m_clctrl.SetFont(_frm->fnt);

	BuildColumns();

	Fill();

  CreateChildren();
}

void FieldColumnLister::BuildColumns()
{
	int iCheckWidth = ::GetSystemMetrics(SM_CXMENUCHECK);

	int iTotalWidth = 0;
	int iCurCol = 0;
	CSize isHeader;
	CString sColName, sDummy;

	sColName = "Column Name";

	sDummy = CString('x', 15);
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	iTotalWidth += iCheckWidth + isHeader.cx;

	sColName = "Width";
	sDummy = CString('x', 10);
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);

	sColName = "New Domain";
	sDummy = sColName;
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);

	sColName = "Domain Type";
	sDummy = sColName;
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	iTotalWidth += iCheckWidth + isHeader.cx;

	sColName = "Domain Name";
	sDummy = CString('x', 20);
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	iTotalWidth += iCheckWidth + isHeader.cx;

	sColName = "Extend";
	sDummy = CString('x', 10);
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	iTotalWidth += iCheckWidth + isHeader.cx;

	sColName = "Min";
	sDummy = CString('x', 10);
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	iTotalWidth += iCheckWidth + isHeader.cx;

	sColName = "Max";
	sDummy = CString('x', 10);
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	iTotalWidth += iCheckWidth + isHeader.cx;

	sColName = "Precision";
	sDummy = CString('x', 10);
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	iTotalWidth += iCheckWidth + isHeader.cx;

	sColName = "Undef Value";
	sDummy = CString('x', 10);
	isHeader = m_clctrl.GetStringWidth(sDummy);
	m_clctrl.InsertColumn(iCurCol++, sColName, LVCFMT_LEFT, iCheckWidth + isHeader.cx + 10);
	iTotalWidth += iCheckWidth + isHeader.cx;
}

int FieldColumnLister::iRowCount()
{
	if (m_clctrl.GetSafeHwnd() != 0)
		return m_clctrl.GetItemCount();
	else
		return 0;
}

void FieldColumnLister::SetRowCount(int iNrItems)
{
	if (m_clctrl.GetSafeHwnd() != 0)
		m_clctrl.SetItemCountEx(iNrItems, LVSICF_NOSCROLL);

	if (iNrItems > m_colInfo.size())
		m_colInfo.resize(iNrItems);
}

void FieldColumnLister::SetColWidth(int iCol, int iWidth)
{
	m_clctrl.SetColumnWidth(iCol, iWidth);
}

void FieldColumnLister::Fill()
{
	SetRowCount(m_colInfo.size());
}

void FieldColumnLister::ToggleAsKey(int iItem)
{
	m_clctrl.ToggleAsKey(iItem);
}

void FieldColumnLister::ToggleSelectedAsKey()
{
	m_clctrl.ToggleSelectedAsKey();
}

void FieldColumnLister::CallChangeCallback()
{
	if (_npChanged)
		(_cb->*_npChanged)(0);
}

void FieldColumnLister::SelectColumn(int iCol)
{
	m_clctrl.SelectColumn(iCol);
}
