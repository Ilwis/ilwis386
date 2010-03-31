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
   $Log: /ILWIS 3.0/FormElements/ListCtrlList.h $
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

   ListCtrlList.h
   by Willem Nieuwenhuis, 26/6/00
   ILWIS Department ITC
  -----------------------------------------------*/

#ifndef ILW_LISTCTRLLIST_H
#define ILW_LISTCTRLLIST_H

class InPlaceEdit;
class FieldLister;

class _export ListCtrlList: public CListCtrl, public BaseZapp
{
public:
	ListCtrlList();
	virtual ~ListCtrlList();

	void SetParentField(FieldLister*);

private:
	FieldLister *m_fclParent;

	int m_iItem;
	int m_iSubItem;
	int m_iNextFree;
	CString m_sPool[3];
	LPTSTR m_psPool[3];
	LPTSTR AddPool(CString* ps);

	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnChange(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP();
};

#endif
