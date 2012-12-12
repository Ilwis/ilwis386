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
#if !defined(AFX_TABLEPANEVIEW_H__7DD2579E_1E42_11D3_B776_00A0C9D5342F__INCLUDED_)
#define AFX_TABLEPANEVIEW_H__7DD2579E_1E42_11D3_B776_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TablePaneView.h : header file
//

#if !defined(AFX_RECORDBAR_H__4BF33AB6_4E29_11D3_B7BD_00A0C9D5342F__INCLUDED_)
#include "Client\TableWindow\RecordBar.h"
#endif
#include "Client\Base\ButtonBar.h"

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class TableWindow;

/////////////////////////////////////////////////////////////////////////////
// TablePaneView view

class IMPEXP TablePaneView : public BaseTablePaneView
{
	friend class TableWindow; // for access to recBar
protected:
	TablePaneView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(TablePaneView)
public:
	TableDoc* GetDocument();
	const TableDoc* GetDocument() const;
//	TableWindow* twParent();
	const TableView* tvw() const { return GetDocument()->tvw; }
	TableView* tvw() { return GetDocument()->tvw; }
  TableWindow* twParent();
	int iCurrRec() const;
	virtual bool fAllowMoveCol() const;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TablePaneView)
	public:
	virtual void InitColPix(CDC* cdc);
  virtual long iCols() const;
  virtual long iRows() const;
  virtual String sULButton() const; // upper left button
  virtual String sColButton(int iCol) const;
  virtual String sRowButton(long iRow) const;
	virtual void OnInitialUpdate();
	virtual String sPrintTitle() const;
	protected:
  virtual void OnULButtonPressed();  // upper left button
  virtual void OnRowButtonPressed(long iRow);
  virtual void OnColButtonPressed(int iCol);
  virtual void OnFieldPressed(int iCol, long iRow, bool fLeft=true);
  virtual String sField(int iCol, long iRow) const;
  virtual String sDescrULButton() const; // upper left button
  virtual String sDescrColButton(int iCol) const;
  virtual String sDescrRowButton(long iRow) const;
  virtual String sDescrField(int iCol, long iRow) const;
  virtual void MoveCol(int iFrom, int iTo);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void updateSelection();
	void selectFeatures(const RowSelectInfo& inf);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~TablePaneView();
	void GotoRecord(long iRec);
	virtual bool fAllowPaste() const;
	void CreateColPix(TableDoc* td)	;
	void setSelectionOtherViews(const vector<long>& raws, const Table& tbl);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(TablePaneView)
	afx_msg void OnEdit();
	afx_msg void OnUpdateEdit(CCmdUI* pCmdUI);
	afx_msg void OnEditField();
	afx_msg void OnEditClear();
	afx_msg void OnEditPaste();
	afx_msg void OnGotoColumn();
	afx_msg void OnContextMenu(CWnd*,CPoint);
	afx_msg void OnSortOn();
	afx_msg void OnSortOnAscending();
	afx_msg void OnSortOnDescending();
	afx_msg void OnUpdateAllColumns();
	afx_msg void OnProp();
	afx_msg void OnSelectFeaturesByColumn();
	afx_msg void OnUpdateProp(CCmdUI* pCmdUI);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLEPANEVIEW_H__7DD2579E_1E42_11D3_B776_00A0C9D5342F__INCLUDED_)
