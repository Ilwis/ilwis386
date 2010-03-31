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
// StereoscopeSplitterWindow.cpp: implementation of the StereoscopeSplitterWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Editors\Stereoscopy\StereoscopeSplitterWindow.h"

IMPLEMENT_DYNCREATE(StereoscopeSplitterWindow, CSplitterWnd)

BEGIN_MESSAGE_MAP(StereoscopeSplitterWindow, CSplitterWnd)
	//{{AFX_MSG_MAP(StereoscopeSplitterWindow)
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StereoscopeSplitterWindow::StereoscopeSplitterWindow()
: fSplitterCreated(false)
{

}

StereoscopeSplitterWindow::~StereoscopeSplitterWindow()
{

}

//////////////////////////////////////////////////////////////////////
// OnSize: keep vertical scrollbar in middle of available size
//////////////////////////////////////////////////////////////////////
void StereoscopeSplitterWindow::OnSize(UINT nType, int cx, int cy)
{
   CSplitterWnd::OnSize(nType, cx, cy);

	 if (fSplitterCreated)
	 {
		 CRect rect;
		 GetWindowRect(&rect);
		 // First take care of width
		 int iWidth = max(0, rect.Width() -22 -2*m_cxBorder); // Til we find a better calculation
		 iWidth /= 2;
		 SetColumnInfo(0, iWidth, 10);
		 SetColumnInfo(1, iWidth, 10);
		 // Then redraw panes
		 RecalcLayout();
	 }
}

BOOL StereoscopeSplitterWindow::CreateStatic(CWnd* pParentWnd, int nRows, int nCols, DWORD dwStyle, UINT nID)
{
	BOOL fRetVal = CSplitterWnd::CreateStatic(pParentWnd, nRows, nCols, dwStyle, nID);
	fSplitterCreated = (TRUE == fRetVal); // translate BOOL to bool
	return fRetVal;
}

void StereoscopeSplitterWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Nothing
}

void StereoscopeSplitterWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);
}

BOOL StereoscopeSplitterWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// Skip CSplitterWnd::OnSetCursor
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
