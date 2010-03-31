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
// ObjectTreeCtrl.h: interface for the ObjectTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTTREECTRL_H__57D553F2_8306_11D3_B800_00A0C9D5342F__INCLUDED_)
#define AFX_OBJECTTREECTRL_H__57D553F2_8306_11D3_B800_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//! Help class for NameEdit
/*! This class is the tree control shown instead of the 
  list box when a NameEdit is droppeddown
*/
class ObjectTreeCtrl: public CTreeCtrl  
{
public:
	ObjectTreeCtrl(NameEdit*);
	virtual ~ObjectTreeCtrl();
	void Reset();
	void SelectAttribColumn(const FileName& fnMapWithCol);
	long iHideTime;
	int iCompareByIndex(long iNdx1, long iNdx2);
private:
	void Fill();
	void OnKillFocus(CWnd* pNewWnd);
	void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	void ExpandDir(HTREEITEM hti);
	void ExpandPath(HTREEITEM hti, const String& sPath);
	void ExpandMapList(HTREEITEM hti);
	void ExpandObjectCollection(HTREEITEM hti);
	void ExpandAttribTable(HTREEITEM hti, const FileName& fnMap);
	void FillWithColumns(const FileName& fnMap, vector<String>& arColumns);
	void AddSystemObjects(HTREEITEM);
	void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnContextMenu(CWnd* pWnd, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	BOOL PreTranslateMessage(MSG* pMsg);
	void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	Array<FileName> afn;
	NameEdit* ne;
	bool fFilled;
	int iImgOvlSystem;
	DECLARE_MESSAGE_MAP();
};

#endif // !defined(AFX_OBJECTTREECTRL_H__57D553F2_8306_11D3_B800_00A0C9D5342F__INCLUDED_)
