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
   $Log: /ILWIS 3.0/FormElements/ColumnListCtrl.cpp $
 * 
 * 24    2/24/03 4:48p Martin
 * two additional bugs solved. 1 A new class domain always set filename to
 * empty, while OK in it self, the empty filename prevented that the
 * domain could be switchec again. 2 if domain string was reselected it
 * did not reset the creation flag which resulted in errors. (see bug
 * 5689)
 * 
 * 23    11/22/02 10:58a Martin
 * new domain entry will revert to "no" when a domain string is selected
 * 
 * 22    1/14/02 10:27a Martin
 * someone had changed the names of the columns. There was no match
 * anymore in this control so certain changes did not work
 * 
 * 21    9/13/01 9:46 Willem
 * In the column details edit page:
 * The domain name cannot be editted anymore when a domain type string is
 * selected
 * 
 * 20    9/06/01 16:08 Willem
 * - The domain string cannot be created; therefore the option "new
 * domain" is disabled now for domain string
 * 
 * 19    5/22/01 12:38 Willem
 * If the domain type string or value is selected the filename is now set
 * to the default string.dom (for string) or value.dom (for value)
 * 
 * 18    13-03-01 14:33 Hendrikse
 * CString cannot be created from String() constructor.
 * 
 * 17    12/03/01 17:23 Willem
 * The control does not display "none" for domaintype anymore; instead
 * empty string is displayed (DomainNone for Column is not allowed)
 * 
 * 16    13/02/01 10:56 Willem
 * The extension of the selected domain is retained if it is different
 * from ".dom"
 * 
 * 15    13/02/01 10:44 Willem
 * Added extra initialization when changing domain type to DomainValue
 * from domain class/id
 * 
 * 14    19/01/01 11:39 Willem
 * - For a new column domain now system domain names are disallowed
 * - Build in a guard to only show errors once
 * 
 * 13    18/01/01 17:06 Willem
 * Visual feedback for key column in "edit column details" is now correct:
 * the '*' is now set properly before the key column
 * 
 * 12    6-12-00 12:36 Koolhoven
 * in OnGetDispInfo() use the reference of ciColumn() and not a copy,
 * prevents crashes
 * 
 * 11    21/11/00 12:09 Willem
 * Added function ToggleAsKey(int)
 * 
 * 10    24/10/00 17:59 Willem
 * Added error when trying to select a column as the table domain (Key)
 * that has duplicate elements
 * 
 * 9     13/10/00 14:59 Willem
 * - Rearranged the columns: Split the Extend column in two: the 'extend'
 * column is now only used to indicate possible extension of the domain;
 * the 'New Domain' column indicates whether to create a new domain or
 * not.
 * - In place editting can now also be activated by double clicking
 * - Added width column for import of fixed format
 * 
 * 8     27/07/00 15:47 Willem
 * The domain name column is now editable when Extend:new is selected
 * 
 * 7     26/06/00 15:30 Willem
 * Improved interface behaviour:
 * - Fields are only editable when appropriate
 * - added breaks in switch statement
 * 
 * 6     22/06/00 16:18 Willem
 * Extend column is not relevant for Value or Bool: it is empty and cannot
 * be editted.
 * 
 * 5     21/06/00 18:15 Willem
 * Improved version of the import table wizard
 * - better column selection handling
 * - scan is done each time the format selection is changed
 * 
 * 4     29/05/00 10:58 Willem
 * Added in place editor for the extend column (InPlaceComboBox)
 * 
 * 3     26/05/00 10:07 Willem
 * ColumListCtrl now also supports InPlaceNameEdit for domain column
 * 
 * 2     22/05/00 10:43 Willem
 * Replaced call to DomainAsType (obsolete) with call to sDomainType
 * 
 * 1     19/05/00 14:42 Willem
 * Add components for columnlist in import ascii table wizard

   ColumnListCtrl.cpp
   by Willem Nieuwenhuis, 17/5/00
   ILWIS Department ITC
  -----------------------------------------------*/

#include "Headers\stdafx.h"
#include "Client\Headers\formelementspch.h"
#include "Engine\DataExchange\TableExternalFormat.h"
#include "Client\FormElements\ColumnListCtrl.h"
#include "Client\FormElements\FieldColumnLister.h"
#include "Client\Editors\Utils\InPlaceEdit.h"
#include "Client\Editors\Utils\InPlaceNameEdit.h"
#include "Client\Editors\Utils\InPlaceComboBox.h"
#include "Client\FormElements\objlist.h"
#include "Client\ilwis.h"
#include "Headers\Hs\Userint.hs"
#include "Headers\Hs\TabelWizard.hs"

BEGIN_MESSAGE_MAP(ColumnListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(ColumnListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO,    OnGetDispInfo)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT,	  OnEndLabelEdit)
	ON_NOTIFY_REFLECT(NM_CLICK,           OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK,          OnDoubleClick)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ColumnListCtrl::ColumnListCtrl()
{
    m_psPool[0] =
	m_psPool[1] =
	m_psPool[2] = NULL;
	m_iNextFree = 0;
}

ColumnListCtrl::~ColumnListCtrl()
{
}

void ColumnListCtrl::SetParentField(FieldColumnLister* fcl)
{
	m_fclParent = fcl;
}

int ColumnListCtrl::iColumnFromName(const String& sCol)
{
	CHeaderCtrl *hdr = GetHeaderCtrl();
	bool fFound = false;

	int i = 0;
	while (i < hdr->GetItemCount() && !fFound)
	{
		HD_ITEM hdi;
		TCHAR  lpBuffer[32];

		hdi.mask = HDI_TEXT;
		hdi.pszText = lpBuffer;
		hdi.cchTextMax = 32;
		hdr->GetItem(i, &hdi);

		String s(lpBuffer);
		fFound = fCIStrEqual(sCol, s);
		++i;
	}
	if (fFound)
		return i - 1;
	else
		return -1;
}

// Private functions
/*	OnGetDispInfo()
	This callback function is called by MFC whenever an item in the ListView
	needs to be displayed.
*/
void ColumnListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		int iSubItem = pDispInfo->item.iSubItem;

		CString sField;
		ClmInfo& ci = m_fclParent->ciColumn(pDispInfo->item.iItem);
		switch (iSubItem)
		{
			case 0: // column name
				if (ci.fKey)
					sField = String("* %S", ci.sColumnName).c_str();
				else
					sField = ci.sColumnName.c_str();
				break;
			case 1:  // field width
				{
					int iVal = ci.iColumnWidth == iUNDEF ? 0 : ci.iColumnWidth;
					sField = String("%li", iVal).c_str();
					break;
				}
			case 2:  // new domain toggle
				if (ci.fCreate && ci.dtDomainType != dmtSTRING)
					sField = TR("Yes").c_str();
				else
					sField = TR("No").c_str();
				break;
			case 3:  // domain type
				sField = Domain::sDomainType(ci.dtDomainType).c_str();
				// Check for DomainNone, because this is not allowed for columns
				if (fCIStrEqual(sField, "none"))
					sField = "";
				break;
			case 4:  // domain
				if (ci.fnDomain.fValid())
					sField = ci.fnDomain.sRelative(false).c_str();
				else
					sField = "";
				break;
			case 5:  // Extend
				if (ci.dtDomainType != dmtCLASS && ci.dtDomainType != dmtID)
					sField = "";
				else
					sField = ci.fCreate ? TR("Yes").c_str() : TR("No").c_str();
				break;
			case 6: 
				if (ci.dtDomainType != dmtVALUE)
					sField = "";
				else
					sField = String("%g", ci.dvs.vr()->rrMinMax().rLo()).c_str();
				break;
			case 7:
				if (ci.dtDomainType != dmtVALUE)
					sField = "";
				else
					sField = String("%g", ci.dvs.vr()->rrMinMax().rHi()).c_str();
				break;
			case 8:
				if (ci.dtDomainType != dmtVALUE)
					sField = "";
				else
					sField = String("%g", ci.dvs.vr()->rStep()).c_str();
				break;
			case 9: 
				sField = ci.sUndefValue.c_str(); 
				break;
			default:
				sField = "";
		}
		if (pDispInfo->item.mask & LVIF_IMAGE)
			pDispInfo->item.iImage = IlwWinApp()->iImage("column");

		LPTSTR psBuffer = AddPool(&sField);
		pDispInfo->item.pszText = psBuffer;
	}

	*pResult = 0;
}

// Extra function to temporarily buffer strings for ListView's
// space is needed for at most three strings (2 extra LVN_GETDISPINFO
// notifications) See also MFC with Visual C++, Blaszczak, page 383.
LPTSTR ColumnListCtrl::AddPool(CString* ps)
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

void ColumnListCtrl::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	
	OnBeginLabelEdit(pNMHDR, pResult);
}

void ColumnListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	LPNMLISTVIEW pNMLV = (LPNMLISTVIEW)pNMHDR;

	m_iSubItem = pNMLV->iSubItem;
	m_iItem = pNMLV->iItem;
}

void ColumnListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	switch ( nChar)
	{
		case VK_LEFT:
			m_iSubItem = m_iSubItem - nRepCnt;
			if (m_iSubItem < 0)
				m_iSubItem = 0;
			break;
		case VK_RIGHT:
			m_iSubItem = m_iSubItem + nRepCnt;
			if (m_iSubItem >= m_fclParent->iNrCols())
				m_iSubItem = m_fclParent->iNrCols() - 1;
			break;
	}
}

void ColumnListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
	switch ( nChar)
	{
		case VK_INSERT:
	   	case VK_F2:
			int iSelected = GetSelectionMark();
			if (iSelected >= 0) {
				m_iItem = iSelected;
				LRESULT pResult;
				OnBeginLabelEdit(0, &pResult);
			}
			break;
	}		
}

void ColumnListCtrl::ToggleAsKey(int iItem)
{
	m_iItem = iItem;

	ToggleSelectedAsKey();
}

void ColumnListCtrl::ToggleSelectedAsKey()
{
	for (int i = 0; i < m_fclParent->iNrCols(); ++i)
	{
		ClmInfo& ci = m_fclParent->ciColumn(i);
		if (m_iItem == i && !ci.fKeyAllowed)
		{
			AfxMessageBox(TR("This column has duplicate values and therefore cannot be used for the table domain").c_str());
			return;
		}
	}

	for (int i = 0; i < m_fclParent->iNrCols(); ++i)
	{
		ClmInfo& ci = m_fclParent->ciColumn(i);
		if (m_iItem == i)
			ci.fKey = true;
		else
			ci.fKey = false;
		Update(i);
	}
}

/*
	OnBeginLabelEdit() 
	OnEndLabelEdit() 
	These functions contain the functionality
	for the in place editting of listview items. (in any column)
	The OnClick handler set the appropriate m_iItem and m_iSubItem members
*/
void ColumnListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	if (m_iSubItem == -1)
		return;

	m_iErrCnt = 0;
	*pResult = 1; // No processing needed after return

	CRect rcSubItem;
	GetSubItemRect(m_iItem, m_iSubItem, LVIR_LABEL, rcSubItem);
	int margin = 2;
	if (m_iSubItem > 0)
		rcSubItem.left += 2; // columns other than column 0 are indented 2 pixels extra!
	rcSubItem.top -= margin;
	rcSubItem.bottom += margin;

	ClmInfo& ci = m_fclParent->ciColumn(m_iItem);

	if (m_iSubItem == iColumnFromName("Domain type"))  // The domain type column
	{
		vector<String> vcDomainTypes;
		vcDomainTypes.push_back(Domain::sDomainType(dmtCLASS));
		vcDomainTypes.push_back(Domain::sDomainType(dmtID));
		vcDomainTypes.push_back(Domain::sDomainType(dmtVALUE));
		vcDomainTypes.push_back(Domain::sDomainType(dmtBOOL));
		vcDomainTypes.push_back(Domain::sDomainType(dmtSTRING));
		rcSubItem.InflateRect(0, 0, 0, 150);
		InPlaceComboBox *icb = new InPlaceComboBox(this, m_iItem, m_iSubItem, vcDomainTypes, rcSubItem, m_fclParent->Id());
		icb->SelectString(-1, (LPCTSTR)GetItemText(m_iItem, m_iSubItem));
	}
	else if (m_iSubItem == iColumnFromName("Domain Name") && !ci.fCreate)  // The domain column
	{
		if (ci.dtDomainType != dmtSTRING)
		{
			// InPlaceNameEdit takes care of Create and will also takes care of removing DomainLister
			DomainLister *ol = new DomainLister(dmCLASS | dmIDENT | dmVALUE | dmGROUP | dmBOOL);
			m_ne = new InPlaceNameEdit(this, m_fclParent->frm(), m_iItem, m_iSubItem, ol, rcSubItem, m_fclParent->Id());

			m_ne->SetVal(ci.fnDomain);
			m_ne->SetFocus();
		}
	}
	else if (m_iSubItem == iColumnFromName("Extend"))  // the extend column
	{
		if (ci.dtDomainType == dmtCLASS || ci.dtDomainType == dmtID)
		{
			vector<String> vcYesNo;
			vcYesNo.push_back(TR("Yes"));
			vcYesNo.push_back(TR("No"));
			rcSubItem.InflateRect(0, 0, 0, 150);
			InPlaceComboBox *icb = new InPlaceComboBox(this, m_iItem, m_iSubItem, vcYesNo, rcSubItem, m_fclParent->Id());
			icb->SelectString(-1, (LPCTSTR)GetItemText(m_iItem, m_iSubItem));
		}
	}
	else if (m_iSubItem == iColumnFromName("New Domain"))
	{
		if (ci.dtDomainType != dmtSTRING)  // Cannot create a new string domain
		{
			vector<String> vcYesNo;
			vcYesNo.push_back(TR("Yes"));
			vcYesNo.push_back(TR("No"));
			rcSubItem.InflateRect(0, 0, 0, 150);
			InPlaceComboBox *icb = new InPlaceComboBox(this, m_iItem, m_iSubItem, vcYesNo, rcSubItem, m_fclParent->Id());
			icb->SelectString(-1, (LPCTSTR)GetItemText(m_iItem, m_iSubItem));
		}
	}
	else
	{
		bool fEditAllowed = false;
		if (ci.dtDomainType == dmtVALUE)
		{
			switch (m_iSubItem)
			{
				case 6:  // iColumnFromName("Min"):
				case 7:  // iColumnFromName("Max"):
				case 8:  // iColumnFromName("Step"):
					fEditAllowed = true;
					break;
			}
		}
		if (m_iSubItem == iColumnFromName("Undef")  ||
			m_iSubItem == iColumnFromName("Domain Name") ||  // in case of Create = yes
			m_iSubItem == iColumnFromName("Width") ||
			m_iSubItem == iColumnFromName("Column name"))
			fEditAllowed = true;

		if (fEditAllowed)
		{
			String sLabelText = String(GetItemText(m_iItem, m_iSubItem));

			// Create the InPlaceEdit; there is no need to delete it afterwards, it will destroy itself
			DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL;
			m_Edit = new InPlaceEdit(m_iItem, m_iSubItem, sLabelText.c_str());
			m_Edit->Create(style, rcSubItem, this, m_fclParent->Id());
		}
	}

}

void ColumnListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.pszText != 0)
	{
		String sField(pDispInfo->item.pszText);
		sField.sTrimSpaces();

		ClmInfo& ci = m_fclParent->ciColumn(m_iItem);
		double rMin;
		double rMax;
		double rStep;
		int    iWidth;
		switch (m_iSubItem)
		{
			case 0:  // Column Name
				ci.sColumnName = sField;
				break;
			case 1:  // width (only for fixed format)
				iWidth = sField.iVal();
				if (iWidth != iUNDEF && iWidth > 0)
					ci.iColumnWidth = iWidth;
				break;
			case 2:  // new domain
				ci.fCreate = fCIStrEqual(sField, TR("Yes"));
				if (ci.fCreate)
				{
					FileName fnDom = IlwisObjectPtr::fnCheckPath(ci.fnDomain);
					if (ObjectInfo::fSystemObject(fnDom))
						ci.fnDomain = FileName(); // do not allow system domain to be choosen as new domain
				}
				break;
			case 3:  // DomainType
				ci.dtDomainType = Domain::dmt(sField);
//				if (ci.fnDomain.fValid())
					if (Domain::dmt(ci.fnDomain) != ci.dtDomainType)
					{
						if (ci.dtDomainType == dmtSTRING)
						{
							ci.fnDomain = IlwisObjectPtr::fnCheckPath("string.dom");
							ci.fCreate = false;
						}							
						else if (ci.dtDomainType == dmtVALUE)
							ci.fnDomain = IlwisObjectPtr::fnCheckPath("value.dom");
						else if (!((ci.dtDomainType == dmtCLASS || ci.dtDomainType == dmtID) && ci.fCreate)) // new class/id domain
							ci.fnDomain = FileName();
					}
				if (ci.dtDomainType == dmtVALUE && !ci.dvs.vr().fValid())
					ci.dvs = DomainValueRangeStruct(ValueRange(-1.0e9, 1.0e9, 0.1));
				break;
			case 4:  // Domain
				ci.fnDomain = FileName(sField, ".dom", false);
				if (ci.fnDomain.fValid())
				{
					if (ci.fCreate && File::fExist(ci.fnDomain))
					{
						ci.fnDomain = FileName();
						if (m_iErrCnt == 0)  // display the message only once
							AfxMessageBox(TR("Domain already exists, choose another").c_str());
						m_iErrCnt++;
					}
					else if (!ci.fCreate)
					{
						ci.dtDomainType = Domain::dmt(ci.fnDomain);
						if (ci.fnDomain.fValid())
							if (Domain::dmt(ci.fnDomain) != ci.dtDomainType && 
								!((ci.dtDomainType == dmtCLASS || ci.dtDomainType == dmtID) && ci.fCreate)) // new class/id domain
								ci.fnDomain = FileName();
						if (ci.dtDomainType == dmtVALUE && !ci.dvs.vr().fValid())
							ci.dvs = DomainValueRangeStruct(ValueRange(-1.0e9, 1.0e9, 0.1));
					}
				}
				else
					ci.fnDomain = FileName();
				break;
			case 5:  // Extend
				ci.fExtend = fCIStrEqual(sField, TR("Yes"));
				break;
			case 6:  // min value
				rMin = sField.rVal();
				if (rMin == rUNDEF)
					break;
				rMax = ci.dvs.vr()->rrMinMax().rHi();
				if (rMin < rMax)
				{
					ValueRange vr(rMin, rMax, ci.dvs.rStep());
					ci.dvs.SetValueRange(vr);
				}
				break;
			case 7:  // max value
				rMax = sField.rVal();
				if (rMax == rUNDEF)
					break;
				rMin = ci.dvs.vr()->rrMinMax().rLo();
				if (rMin < rMax)
				{
					ValueRange vr(rMin, rMax, ci.dvs.rStep());
					ci.dvs.SetValueRange(vr);
				}
				break;
			case 8:  // step size
				rStep = sField.rVal();
				if (rStep == rUNDEF)
					break;
				rMin = ci.dvs.vr()->rrMinMax().rLo();
				rMax = ci.dvs.vr()->rrMinMax().rHi();
				{
					ValueRange vr(rMin, rMax, rStep);
					ci.dvs.SetValueRange(vr);
				}
				break;
			case 9: 
				ci.sUndefValue = sField;
				break;
		}
		m_fclParent->CallChangeCallback();
		Update(m_iItem);
	}

	*pResult = 0;
}

void ColumnListCtrl::SelectColumn(int iCol)
{
	SetItemState(m_iItem, ~LVIS_SELECTED, LVIS_SELECTED);
	SetItemState(iCol, LVIS_SELECTED, LVIS_SELECTED);
	SetSelectionMark(iCol);
	EnsureVisible(iCol, FALSE);
}

