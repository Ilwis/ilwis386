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
#if !defined(AFX_TABLEDOC_H__7DD2579D_1E42_11D3_B776_00A0C9D5342F__INCLUDED_)
#define AFX_TABLEDOC_H__7DD2579D_1E42_11D3_B776_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TableDoc.h : header file
//

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class ColumnView;
class TableView;
class TableGraphView;

/////////////////////////////////////////////////////////////////////////////
// TableDoc document

class IMPEXP TableDoc : public IlwisDocument
{
	friend class TableCommandHandler;
	friend class TablePaneView;	
	friend class TableGraphView;
	friend class RoseDiagramView;
public:
	TableDoc();           
	DECLARE_DYNCREATE(TableDoc)
	const Table& table() const;
	const Domain& dm() const;
	int iCols() const;
	long iRows() const;
  ColumnView& cv(int i);
  const ColumnView& cv(int i) const;
  ColumnView& cvKey();
  const ColumnView& cvKey() const;
	const TableView* view() const { return tvw; }
	long iRow(const String sRec) const;
  String sRow(long iRow) const;
  String sValue(short iCol, long iRow, short iWidth=-1, short iDec=-1) const;
  void MakeUsable(short iCol) const;
  void CheckNrRecs(); 
	void MoveCol(int iFrom, int iTo);
	void SetColPos(int iCol, int iPos);
	bool fDelColumn(int iCol);

	bool fCmdCalc(const String sCmd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TableDoc)
	public:
	virtual	IlwisObject	obj() const;
	virtual zIcon	icon() const;
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPath, ParmList& pm) ;
protected:
	virtual BOOL OnNewDocument();
	BOOL SaveDocumentAs(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL OnOpenDocument(const Table& table);
	virtual ~TableDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	Table tbl;
	TableView* tvw;
public:
	afx_msg void OnAddColumn();
protected:
  void OnNewGraph();
  void OnNewRoseDiagram();
	BOOL OnSaveDocument(LPCTSTR lpszPathName);
	void OnUpdateSaveAs(CCmdUI* pCmdUI) 	;
	void OnFileSaveAs()	;

  afx_msg void OnSort();
	afx_msg void OnUpdateAllColumns();
  afx_msg void OnUpdateAddColumn(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLEDOC_H__7DD2579D_1E42_11D3_B776_00A0C9D5342F__INCLUDED_)
