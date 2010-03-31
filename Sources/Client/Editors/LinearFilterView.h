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
#if !defined(AFX_LINEARFILTERVIEW_H__F370FF85_B203_11D3_B82F_00A0C9D5342F__INCLUDED_)
#define AFX_LINEARFILTERVIEW_H__F370FF85_B203_11D3_B82F_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinearFilterView.h : header file
//

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// LinearFilterView view

class IMPEXP LinearFilterView: public BaseTablePaneView
{
public:
	LinearFilterView();           // protected constructor used by dynamic creation
	virtual ~LinearFilterView();
	virtual void InitColPix(CDC* cdc);
  virtual int iCols() const;
  virtual long iRows() const;
  virtual String sColButton(int iCol) const;
  virtual String sRowButton(long iRow) const;
	virtual void OnInitialUpdate();
	LinearFilterDoc* GetDocument();
	const LinearFilterDoc* GetDocument() const;
	void OnEditPaste();
	void OnEdit();
	void OnUpdateEdit(CCmdUI* pCmdUI);
	void OnContextMenu(CWnd*,CPoint);
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  virtual String sField(int iCol, long iRow) const;
  virtual void OnFieldPressed(int iCol, long iRow, bool fLeft=true);
	virtual bool fAllowPaste() const;
	//{{AFX_MSG(LinearFilterView)
	//}}AFX_MSG
	DECLARE_DYNCREATE(LinearFilterView)
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINEARFILTERVIEW_H__F370FF85_B203_11D3_B82F_00A0C9D5342F__INCLUDED_)
