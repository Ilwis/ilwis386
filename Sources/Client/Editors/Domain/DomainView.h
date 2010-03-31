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
#if !defined(AFX_DOMAINSORTVIEW_H__4DDF7EA6_B5DE_11D2_B6FD_00A0C9D5342F__INCLUDED_)
#define AFX_DOMAINSORTVIEW_H__4DDF7EA6_B5DE_11D2_B6FD_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DomainView.h : header file
//

class DomainSortDoc;
class DomainGroupDoc;

#include "Client\Base\BaseView2.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\Editors\Utils\InPlaceEdit.h"
#include "Client\Editors\Domain\DomainSortListCtrl.h"

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
//! DomainSortView view
class IMPEXP DomainSortView : public BaseView2
{
public:
	virtual void BuildColumns();
	virtual void BuildMenu();
	virtual void AddToolBars();
	DomainSortDoc* GetDocument();
	void Clear();
	void GetMinViewSize(CSize& size, const CSize& margin);

	virtual void OnInitialUpdate();
	void DoLButtonDblClk(UINT nFlags, CPoint point);
	void MoveMouse(short xInc, short yInc); 


protected:
	DomainSortView();           // protected constructor used by dynamic creation
	virtual ~DomainSortView();
	BOOL PreCreateWindow(CREATESTRUCT& cs);

	BOOL PreTranslateMessage(MSG* pMsg);

	CListCtrl& GetListCtrl();
	CHeaderCtrl& GetHeaderCtrl();

	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	void OnDeleteNoAsk();
	void DropItemOnList();
	virtual void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
	virtual void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
	virtual void PrintColumns(CDC *pDC, CPrintInfo *pInfo);
	virtual void PrintItem(CDC *pDC, CPrintInfo *pInfo, int iIndex);
	virtual void PrintNewLine(CDC *pDC, CPrintInfo *pInfo);
	String sMakeShortString(CDC* pDC, const String& sVal, int iColumnLen);
	virtual void SortDomain();
	void SetSelectedItem(long iItem, bool fSelected);

	virtual void UpdateHeaderImages();
	void CalcSetDefaultSize(int iTotWidth);

	virtual void SaveState(IlwisSettings& settings);
	virtual void LoadState(IlwisSettings& settings);

	bool fWarnForSortManualChangeOK();

	ButtonBar	m_bbTools;

	int m_iNextFree;
	CString m_sPool[3];
	LPTSTR m_psPool[3];
	LPTSTR AddPool(CString *);

// Print stuff
	vector<int> m_aiColOffsets;
	int m_cxChar, m_cyChar;
	CTime m_timeSys;

// Drag/drop stuff
	CImageList*		m_pDragImage;
	bool			m_fDragging, m_fAbortDrag;
	int				m_iDragIndex;
	CPoint			m_ptDropPoint;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	afx_msg void OnEdit();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnMerge();
	afx_msg void OnChangePrefix();
	afx_msg void OnAlphaSort();
	afx_msg void OnManualSort();
	afx_msg void OnAlphaNumSort();
	afx_msg void OnCodeAlphaSort();
	afx_msg void OnCodeAlphaNumSort();
//	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnSelectAll();
	afx_msg void OnUnSelectAll();
	afx_msg void OnDescriptionBar();
	afx_msg void OnButtonBar();
	afx_msg void OnEscape();
	afx_msg void OnUpdateEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMerge(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChangePrefix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSortType(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDescriptionBar(CCmdUI* pCmdUI);
	afx_msg void OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu( CWnd* pWnd, CPoint pos );
	afx_msg void OnSetFocus( CWnd* ); 

	DECLARE_DYNCREATE(DomainSortView)
	DECLARE_MESSAGE_MAP()

private:
	void CalcPageCount(CDC* pDC, CPrintInfo* pInfo);
	CFont *m_PrintFont, *m_PrintFontBold;
	POSITION m_posLast;  // remember last position for printing selected items.
	int m_iPageHeight;
	int m_iItemsPerPage;
	int m_iPrintDPI;
	int m_iScreenDPI;
	int m_iItem, m_iSubItem;
	String sLabelText;  // placeholder for inplace edit text

	IlwisHeaderCtrl     m_HeaderCtrl;
	DomainSortListCtrl  m_ListCtrl;
	InPlaceEdit         *m_Edit;
};

/////////////////////////////////////////////////////////////////////////////
//! DomainGroupView view
class IMPEXP DomainGroupView : public DomainSortView
{
public:
	virtual void BuildColumns();
	virtual void BuildMenu();
	virtual void AddToolBars();
	DomainGroupDoc* GetDocument();

	void DoLButtonDblClk(UINT nFlags, CPoint point);

protected:
	DomainGroupView();           // protected constructor used by dynamic creation
	virtual ~DomainGroupView();

	virtual void PrintItem(CDC *pDC, CPrintInfo *pInfo, int iIndex);
	virtual void SaveState(IlwisSettings& settings);
	virtual void LoadState(IlwisSettings& settings);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	afx_msg void OnEdit();
	afx_msg void OnAdd();
	afx_msg void OnDelete();

	DECLARE_DYNCREATE(DomainGroupView)
	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOMAINSORTVIEW_H__4DDF7EA6_B5DE_11D2_B6FD_00A0C9D5342F__INCLUDED_)
