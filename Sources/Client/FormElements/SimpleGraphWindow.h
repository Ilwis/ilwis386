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
#if !defined(AFX_SimpleGraphWindow_H__D240A3C2_26F1_4917_A852_0592102BC675__INCLUDED_)
#define AFX_SimpleGraphWindow_H__D240A3C2_26F1_4917_A852_0592102BC675__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimpleGraphWindow.h : header file
//

#include "Engine\Base\Algorithm\SimpleFunction.h"

#include <afxwin.h> // CWnd

/////////////////////////////////////////////////////////////////////////////
// SimpleGraphWindow window

class SimpleGraphWindow : public CWnd
{
// Construction
public:
	SimpleGraphWindow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SimpleGraphWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SimpleGraphWindow();
	void SetFunction(SimpleFunction * funPtr);
	void SetAxisFont(CFont * fnt);
	void SetBorderThickness(int l, int t, int r, int b);
	void Replot();

	// Generated message map functions

protected:
	//{{AFX_MSG(SimpleGraphWindow)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	bool m_fDragging;
	SimpleFunction * m_pFunc;

private:
	void DrawFunction(const SimpleFunction * pFunc);
	void DrawFunction(CDC* pDC, const SimpleFunction * pFunc);
	void DrawAxes(CDC* pDC);
	const double rXToCFactor() const;
	const double rYToRFactor() const;
	const int iXToScreen(const double rX) const;
	const int iYToScreen(const double rY) const;
	const double rScreenToX(int iScreenX) const;
	const double rScreenToY(int iScreenY) const;
	CRect GetFunctionPlotRect() const;
	static UINT PaintInThread(LPVOID pParam);
	void SetDirty(bool fRedraw = false);
	void StartDrag(CPoint point);
	void Drag(CPoint point);
	void EndDrag(CPoint point);
	void CloseThreads();
	void ProcessMessages();

	DoubleRect m_XYFunctionDomain; // function bounds
	CPoint m_pointStartDragPosition; // in plain screen coordinates
	bool m_fBusyInPaintThread;
	bool m_fDirty;
	bool m_fRedraw;
	bool m_fAbortPaintThread;
 	CBitmap* m_bmMemory;
	CBitmap* m_bmOldBitmap;
	CDC* m_dcMemory;

	int m_iLeftBorderThickness;
	int m_iTopBorderThickness;
	int m_iRightBorderThickness;
	int m_iBottomBorderThickness;
	CFont * m_fnt;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SimpleGraphWindow_H__D240A3C2_26F1_4917_A852_0592102BC675__INCLUDED_)
