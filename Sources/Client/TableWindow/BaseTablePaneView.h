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
#if !defined(AFX_BASETABLEPANEVIEW_H__F278ED13_1FD4_11D3_B778_00A0C9D5342F__INCLUDED_)
#define AFX_BASETABLEPANEVIEW_H__F278ED13_1FD4_11D3_B778_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseTablePaneView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// BaseTablePaneView view

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class FrameWindow;

class BaseTblField;

class IMPEXP BaseTablePaneView : public CView
{
protected:
	BaseTablePaneView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(BaseTablePaneView)

public:
	FrameWindow* fwParent();
  void ColRow(zPoint, int& iCol, int& iRow);
  long iFirstRec() const;
  long iFirstCol() const;
  int iHeight() const { return iHght; }
  int iButWidth() const { return iButtonWidth; }
  void SetButWidth(int iButWidth) { iButtonWidth = iButWidth; }
  int iChrWidth() const { return iCharWidth; }

  long iFirstVisibleRow() const;
  long iLastVisibleRow() const;
  int iFirstVisibleColumn() const;
  int iLastVisibleColumn() const;
  int iSelectedColumn() const;
  int iSelectedRow() const;
	enum UpdateHint { uhALL = 0, uhNOBUTTONS = 1 /* update columns only */ };
protected:
  void deleteField();
public:
  zPoint pntField(int iCol, long iRec) const;
  zRect rectField(int iCol, long iRec) const;
  zRect rectBlock(const MinMax&) const;
  zRect rectSelect() const;
  void SetSelect(const MinMax& mm) { mmSelect = mm; }
	MinMax mmSel() const { return mmSelect; }
	int iColumnPix(int iCol) const { return iColPix[iCol]; }
	bool fValidSelection() const;
	void SetScrollBars();
	virtual bool fAllowMoveCol() const;
protected:
  int iCharWidth;
  int iButtonWidth;  // nr of chars in the row button, to be set by derived class
  int* iColPix;   // derived class should create and manage iColPix
  int* iColWidth; // derived class should create and manage iColWidth
  BaseTblField* tField;
	bool fShowHeading;
public:
  int vertLineMove(int);
  int vertPageMove(int);
  int vertTop();
  int vertBottom();
  int vertThumbPos(int nPos);
  int vertPixMove(long iDiff);
  int horzLineMove(int);
  int horzPageMove(int);
  int horzLeft();
  int horzRight();
  int horzThumbPos(int nPos);
  int horzPixMove(long iDiff);

private:
  void drawColMoving();
  void drawColButton(int iCol);
  void PushULButton();
  void ReleaseULButton();
  void PushColButton();
  void ReleaseColButton();
  void PushRowButton();
  void ReleaseRowButton();
  void MoveMouse(short xInc, short yInc) ;
protected:  
  MinMax mmSelect;
	CFont *m_PrintFont, *m_PrintFontBold;
	bool fHeaderOnAllPages, fLeftMostColOnAllPages;
	int iMaxPages;
	int iCalcHorzPages(int iLeftColWidth, bool fLeftMostColOnAllPages, int iCharW, int iPageWidth);
  void CalcColsForColumnPages(int iColPage, int& iFirstCol, int& iLastCol, int iLeftColWidth, 
											bool fLeftMostColOnAllPages, int iCharW, int iPageWidth);
	int iFmtTbl; // special clip format for tables
private:
  long iRec1, iCol1;
	int iHght;
  bool fColMove, fColMoving;
  bool fULButtonDown, fColButtonDown, fRowButtonDown;
  bool fFieldDown, fSelecting;    
  int iColPixMoving, iColMoving;
  long iRowMoving;
  zCursor curArrow, curSplitCol, curColumn;
  zPoint pSelectStart;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BaseTablePaneView)
	public:
	virtual void InitColPix(CDC* cdc);
  virtual int iCols() const;
  virtual long iRows() const;
  virtual String sULButton() const; // upper left button
  virtual String sColButton(int iCol) const;
  virtual String sRowButton(long iRow) const;
	virtual void OnInitialUpdate();
	protected:
  virtual void OnULButtonPressed();  // upper left button
  virtual void OnColButtonPressed(int iCol);
  virtual void OnRowButtonPressed(long iRow);
  virtual void OnFieldPressed(int iCol, long iRow, bool fLeft=true);
  virtual String sField(int iCol, long iRow) const;
  virtual String sDescrULButton() const; // upper left button
  virtual String sDescrColButton(int iCol) const;
  virtual String sDescrRowButton(long iRow) const;
  virtual String sDescrField(int iCol, long iRow) const;
	virtual String sPrintTitle() const;
  virtual void MoveCol(int iFrom, int iTo);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~BaseTablePaneView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual bool fAllowCopy() const;
	virtual bool fAllowClear() const;
	virtual bool fAllowPaste() const;
  bool fHasFocus;
	// Generated message map functions
protected:
	//{{AFX_MSG(BaseTablePaneView)
  afx_msg void OnSelectAll();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnGotoField(WPARAM wParam, LPARAM lParam);
  afx_msg void OnSetFocus(CWnd* pNewWnd);	
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASETABLEPANEVIEW_H__F278ED13_1FD4_11D3_B778_00A0C9D5342F__INCLUDED_)
