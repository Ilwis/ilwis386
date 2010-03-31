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
//created Martin Schouwenburg 18-3-99

#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#if !defined(AFX_ACCELERATORMANAGER_H__4563CF77_639B_11D4_B8E4_00A0C9D5342F__INCLUDED_)
#include "Client\MainWindow\AcceleratorManager.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class EditView : public CEditView
{
	public:
		EditView();
		~EditView();
		void OnInitialUpdate();

		BOOL OnPreparePrinting(CPrintInfo* pInfo);
		void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
		void OnPrint(CDC* pDC, CPrintInfo* pInfo);
		BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) ;
		void OnDelete();
		void OnUpdateDelete(CCmdUI *cmd);
		void OnUpdatePrint(CCmdUI *cmd)		;
		DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) ;
		void CalcPageCount(CDC* pDC, CPrintInfo* pInfo);		

  BOOL	m_bPrintSelection;
  int   m_nNextSelCharToBePrint;
  int   m_nFirstNonSelChar;
		COleDropTarget* odt;

	DECLARE_MESSAGE_MAP()
};

class IMPEXP TextView : public BaseView2, public AcceleratorManager
{
protected: // create from serialization only
	TextView();
	DECLARE_DYNCREATE(TextView)

// Attributes
public:
	TextDocument* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TextView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~TextView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void OnInitialUpdate();
	virtual const string sText() const;
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	void OnFileSave();
	String sGetDescription();

protected:

	//void OnFileSaveAs();
	void OnEditPaste();
	void OnEditCopy();
	void OnEditCut();
	void OnEditFind();
	void OnEditReplace();
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	void OnUpdatePaste(CCmdUI *cmd);
//	void OnUpdatePrint(CCmdUI *cmd)	;
	void OnUpdateDelete(CCmdUI *cmd)	;
//  BOOL OnPreparePrinting( CPrintInfo* pInfo );
//	void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
	void OnPrint();
//	void OnUpdateSave(CCmdUI *cmd);
	void OnUpdateFindReplace(CCmdUI *cmd);
	void OnSetFocus( CWnd* pNewWnd );
	void OnKillFocus( CWnd* pNewWnd );
	void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint );
	void OnDelete();
	void OnUpdatePrint(CCmdUI *cmd);
	void OnUpdatePrintDirect(CCmdUI *cmd)	;
	BOOL PreTranslateMessage(MSG* pMsg);

	virtual void LoadState(IlwisSettings& settings);
	virtual void SaveState(IlwisSettings& settings);

	LRESULT OnDescriptionChanged(WPARAM wP, LPARAM lP);
	LRESULT OnViewSettings(WPARAM wP, LPARAM lP);
	void OnSize( UINT nType, int cx, int cy );
	
	ButtonBar	m_bbTools;
	EditView *edv;

	DECLARE_MESSAGE_MAP()
};


#endif TEXTVIEW_H
