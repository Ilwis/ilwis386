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
   $Log: /ILWIS 3.0/FormElements/ColumnListCtrl.h $
 * 
 * 7     19/01/01 11:39 Willem
 * - For a new column domain now system domain names are disallowed
 * - Build in a guard to only show errors once
 * 
 * 6     21/11/00 12:09 Willem
 * Added function ToggleAsKey(int)
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

   ColumnListCtrl.h
   by Willem Nieuwenhuis, 17/5/00
   ILWIS Department ITC
  -----------------------------------------------*/

#ifndef ILW_COLUMNLISTCTRL_H
#define ILW_COLUMNLISTCTRL_H

class FieldColumnLister;
class InPlaceEdit;
class NameEdit;

class _export ColumnListCtrl: public CListCtrl
{
public:
	ColumnListCtrl();
	virtual ~ColumnListCtrl();

	int iColumnFromName(const String& sCol);
	void SetParentField(FieldColumnLister*);
	void ToggleAsKey(int iItem);
	void ToggleSelectedAsKey();

private:
	NameEdit* m_ne;
	FieldColumnLister *m_fclParent;
	InPlaceEdit *m_Edit;

	bool m_iErrCnt;
	int  m_iItem;
	int  m_iSubItem;
	int  m_iNextFree;
	CString m_sPool[3];
	LPTSTR m_psPool[3];
	LPTSTR AddPool(CString* ps);

	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP();
};

#endif
