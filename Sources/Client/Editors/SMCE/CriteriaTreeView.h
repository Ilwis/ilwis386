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
// CriteriaTreeView.h: interface for the CriteriaTreeView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITERIATREEVIEW_H__8BB22B86_AFFC_4A5E_9E0A_E1FF5995880D__INCLUDED_)
#define AFX_CRITERIATREEVIEW_H__8BB22B86_AFFC_4A5E_9E0A_E1FF5995880D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client\Editors\SMCE\TreeListView.h"
#include <afxcmn.h>

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class CriteriaTreeDoc;
class CriteriaTreeItem;

class IMPEXP CriteriaTreeView : public TreeListView  
{
	DECLARE_DYNCREATE(CriteriaTreeView)
	DECLARE_MESSAGE_MAP()

public:
	CriteriaTreeView();
	virtual ~CriteriaTreeView();

protected:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

private:
	CriteriaTreeDoc* GetDocument();
	void RecursiveReadDoc(HTREEITEM hInsertAt, CriteriaTreeItem* ctiReadFrom);
	void RecursiveReadChildren(HTREEITEM hInsertAt, CriteriaTreeItem* ctiReadFrom);
	void OnShow();
	void OnProperties();
	void OnHistogram();
	void OnAggregateValues();
	void OnAggregateMap();
	void OnCancelAggregateMap();
	void OnSlice();
	void OnShowSliced();
	void OnHistogramSliced();
	void OnAggregateValuesSliced();
	void OnGenerateContourMaps();
	void OnShowContourMaps();
	void OnShowStandardized();
	void OnUpdateShow(CCmdUI*);
	void OnUpdateProperties(CCmdUI*);
	void OnUpdateHistogram(CCmdUI*);
	void OnUpdateAggregateValues(CCmdUI*);
	void OnUpdateAggregateMap(CCmdUI*);
	void OnUpdateCancelAggregateMap(CCmdUI*);
	void OnUpdateSlice(CCmdUI*);
	void OnUpdateShowSliced(CCmdUI*);
	void OnUpdateHistogramSliced(CCmdUI*);
	void OnUpdateAggregateValuesSliced(CCmdUI*);
	void OnUpdateGenerateContourMaps(CCmdUI*);
	void OnUpdateShowContourMaps(CCmdUI*);
	void OnUpdateShowStandardized(CCmdUI*);
	void OnEdit(); // to Edit a treelistitem - only here we know the active column
	void OnUpdateEdit(CCmdUI*);
	void OnDelete(); // to Delete a treelistitem - only here we know the active column
	void OnUpdateDelete(CCmdUI*);
	bool fDeleteAllowed();
	void OnInsert();
	void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	void OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	void OnGenerateSelectedMap();
	void OnUpdateGenerateSelectedMap(CCmdUI*);
	UINT nEditTimer;

	CriteriaTreeItem* ctiSelected;
	CriteriaTreeItem* ctiDragSource;
	CImageList m_cImageList;
	BOOL m_CTLInitialized;
	bool fLMouseButtonDown;
	zCursor m_curDragProhibited, m_curNormal;
	bool m_fDragImageOnScreen;
};

#endif // !defined(AFX_CRITERIATREEVIEW_H__8BB22B86_AFFC_4A5E_9E0A_E1FF5995880D__INCLUDED_)
