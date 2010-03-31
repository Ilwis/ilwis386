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
// TreeListView.h: interface for the TreeListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREELISTVIEW_H__84F14B26_F61A_41DB_BD19_CC098F2B6ADC__INCLUDED_)
#define AFX_TREELISTVIEW_H__84F14B26_F61A_41DB_BD19_CC098F2B6ADC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client\Editors\SMCE\TreeListHeaderCtrl.h"
#include <afxwin.h>
#include <vector>
using namespace std;
#include <afxtempl.h>

#define ID_TREE_LIST_CTRL   373
#define ID_TREE_LIST_HEADER 337

class TreeListView;

class TreeListItem
{
public:
	TreeListItem();
	CString GetItemText(int iCol = 0);
	void SetItemData(DWORD m_iData, HTREEITEM m_hItem); // the HTREEITEM will be passed through for a more efficient UpdateView
	DWORD GetItemData();
	void CustomDraw(TreeListView* pParent, NMHDR* pNMHDR, LRESULT* pResult);

private:
	void ItemCustomDraw(CDC* pDC, CRect rect, int iCol, int nFormat);
	DWORD m_Data;
};

// We make our own TreeListCtrl only for customizing the OnPaint
// This is necessary to offset the CTreeCtrl for the scrollbar position (otherwise it doesn't listen)
class TreeListCtrl : public CTreeCtrl
{
	DECLARE_DYNCREATE(TreeListCtrl)
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnPaint();
};

// We derive TreeListView from CView. If we derive from CTreeView, there's no way
// to correctly draw a header without creating another encapsulating view.
class TreeListView : public CView
{
	DECLARE_DYNCREATE(TreeListView)

public:
	TreeListView();

	TreeListCtrl & GetTreeCtrl();

	HTREEITEM InsertItem( LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST );
	HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter );
	HTREEITEM CopyItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);
	HTREEITEM MoveItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);
	BOOL DeleteItem(HTREEITEM hItem);
	BOOL DeleteChildren(HTREEITEM hItem);
	BOOL DeleteAllItems();

	// BOOL SetItemText(HTREEITEM hItem, int nCol ,LPCTSTR lpszItem);
	CString GetItemText(HTREEITEM hItem, int nSubItem = 0);
	// CString GetItemText(int nItem, int nSubItem);
	BOOL SetItemData(HTREEITEM hItem, DWORD dwData);
	DWORD GetItemData(HTREEITEM hItem) const;
	BOOL SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage);
	BOOL Expand(HTREEITEM hItem, UINT nCode);
	BOOL SelectItem(HTREEITEM hItem);
	HTREEITEM GetSelectedItem();
	HTREEITEM GetRootItem();
	HTREEITEM HitTest(CPoint pt, UINT* pFlags);

	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);

	CImageList* SetImageList(CImageList * pImageList, int nImageListType);
	BOOL DeleteColumn(int nPos);

	int iGetColumnWidth(int nCol);
	int iGetColumnAlign(int nCol);
	int iColumnsWidth() {return m_nColumnsWidth;};
	int iColumns() {return m_nColumns;};
	int iOffset() {return m_nOffset;}; // for the TreeListCtrl to estimate where to draw itself
	int iActiveColumn() {return m_iActiveColumn;};

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	    
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);		
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnDraw(CDC* pDC);  // overridden .. compulrosy in CView
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual ~TreeListView();

private:
	void Initialize();
	void ResetScrollBar();
	BOOL VerticalScrollVisible();
	BOOL HorizontalScrollVisible();
	int StretchWidth(int m_nWidth, int m_nMeasure);
	void RecalcColumnsWidth();
	void ResetVertScrollBar();
	void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	void OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	void OnHScroll1(NMHDR* pNMHDR, LRESULT* pResult);
	void BackupTreeListItemPtrs(HTREEITEM hParent);
	BOOL DeleteOneItem(HTREEITEM hItem);
	BOOL m_TLInitialized;	

	TreeListCtrl tc;
	TreeListHeaderCtrl m_wndHeader;
	int m_nOffset;
	CArray<TreeListItem*,TreeListItem*> TreeListItemPtrs;
	CScrollBar m_horScrollBar;
	int m_nColumnsWidth;
	int m_nColumns;
	int m_iActiveColumn;

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_TREELISTVIEW_H__84F14B26_F61A_41DB_BD19_CC098F2B6ADC__INCLUDED_)
