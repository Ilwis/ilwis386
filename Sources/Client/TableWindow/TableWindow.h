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
#if !defined(AFX_TABLEWINDOW_H__7DD2579C_1E42_11D3_B776_00A0C9D5342F__INCLUDED_)
#define AFX_TABLEWINDOW_H__7DD2579C_1E42_11D3_B776_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TableWindow.h : header file
//

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class SimpleCalcResultForm;
class TableView;
class TablePaneView;
class TableSummaryPaneView;


  class TableSplitterWindow : public CSplitterWnd  
  {
  public:
    TableSplitterWindow() { iIdealStatPaneHeight = 0; }
    ~TableSplitterWindow() {}
    int iIdealStatPaneHeight;
	  //{{AFX_VIRTUAL(TableSplitterWindow)
	  //}}AFX_VIRTUAL
	  //{{AFX_MSG(TableSplitterWindow)
      afx_msg void OnSize(UINT nType, int cx, int cy);
			afx_msg void OnPaint();
	  //}}AFX_MSG
	  DECLARE_DYNCREATE(TableSplitterWindow)
	  DECLARE_MESSAGE_MAP()
  };

/////////////////////////////////////////////////////////////////////////////
// TableWindow window


class IMPEXP TableWindow : public DataWindow
{
// Construction
public:
	TableWindow();
	virtual ~TableWindow();
  bool fExecute(const String& sCmd);
  TableView* tvw() const;
  virtual void InitSettings();
  virtual void SaveSettings();
  void CreateRecordBar();
	SimpleCalcResultForm *frmSimpleCalcResults();
	// Generated message map functions
	RecordBar* recBar;
protected:
	ButtonBar bBar;
	void GotoRecord(long iRec);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//{{AFX_MSG(TableWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LONG OnExecute(UINT, LONG);
	afx_msg void OnTblStats();
	afx_msg void OnAggregate();
	afx_msg void OnJoin();
	afx_msg void OnLeastSquares();
	afx_msg void OnCumulative();
	afx_msg void OnSemiVariogram();
	afx_msg void OnSlicing();
	afx_msg void OnFileProp();
	afx_msg void OnTimeColumn();
afx_msg LONG OnCommandUI(UINT, LONG lParam);
  afx_msg void OnAddRecord();
	afx_msg void OnUpdateAddRecord(CCmdUI* pCmdUI);
  afx_msg void OnAdditionalInfo();
	afx_msg void OnUpdateAdditionalInfo(CCmdUI* pCmdUI);
	afx_msg void OnCommandLine();
	afx_msg void OnUpdateCommandLine(CCmdUI* pCmdUI);
	afx_msg void OnNewGraph();
	afx_msg void OnNewRoseDiagram();
	afx_msg void OnStatPane();
	afx_msg void OnColMan();
	afx_msg void OnUpdateStatPane(CCmdUI* pCmdUI);
	afx_msg void OnShowRecordView();
	afx_msg void OnUpdateShowRecordView(CCmdUI* pCmdUI);
	afx_msg void OnButtonBar();
	afx_msg void OnUpdateButtonBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecFirst(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecLast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecPrev(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecNext(CCmdUI* pCmdUI);
	afx_msg void OnGotoRecord();
	afx_msg void OnGotoRecordFirst();
	afx_msg void OnGotoRecordLast();
	afx_msg void OnGotoRecordPrev();
	afx_msg void OnGotoRecordNext();
	afx_msg void OnConfusionMatrix();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
protected:
  CommandBar commBar;
  TableCommandHandler commHandler;
	TableSplitterWindow wndSplitter;
	SimpleCalcResultForm *frmSimpleCalc;	
  int iPrevStatHeight;
  TablePaneView* tpv;
private:  
	DECLARE_DYNCREATE(TableWindow)
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLEWINDOW_H__7DD2579C_1E42_11D3_B776_00A0C9D5342F__INCLUDED_)
