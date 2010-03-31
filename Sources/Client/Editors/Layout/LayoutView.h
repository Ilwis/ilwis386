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
#if !defined(AFX_LAYOUTVIEW_H__26187C9F_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
#define AFX_LAYOUTVIEW_H__26187C9F_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayoutView.h : header file
//

#if !defined(AFX_ZOOMABLEVIEW_H__26187CDC_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\ZoomableView.h"
#endif

#if !defined(AFX_LAYOUTRECTTRACKER_H__D63A9FF4_F8B8_11D3_B875_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Layout\LayoutRectTracker.h"
#endif

class LayoutDoc;
class LayoutItem;
class COleDropTarget;

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP LayoutView : public ZoomableView
{
public:
	LayoutView();           // protected constructor used by dynamic creation
	virtual ~LayoutView();
	LayoutDoc* GetDocument();
  virtual MinMax mmBounds(); // in 0.1 mm units
	MinMax mmPaperBounds();
protected:
	virtual void SetDirty();
	//{{AFX_VIRTUAL(LayoutView)
	public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo *info);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(LayoutView)
	afx_msg void OnEntirePage();
	afx_msg void OnPageSetup();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEditClear();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnPrintHelp(WPARAM, LPARAM);
  LRESULT OnViewSettings(WPARAM wP, LPARAM lP);
	//}}AFX_MSG

	void DrawItem(CDC*, LayoutItem*);	 
	void DrawPageBorder(CDC*);
	LayoutItem* liHitTest(CPoint point);
	void SetActiveItem(LayoutItem*);
	void ReportPaperSizeOnStatusBar();
	void OnMakeBitmap();
	void Print(CDC* cdc, double rFactX, double rFactY);

	bool fStarting;

	LayoutItem* liActive;
	LayoutRectTracker tracker;
	COleDropTarget* odt;
	int iExtraOffsetX, iExtraOffsetY;
	HelpTopic htp;
	
	DECLARE_DYNCREATE(LayoutView)
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYOUTVIEW_H__26187C9F_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
