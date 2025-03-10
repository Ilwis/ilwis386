/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
// StereoscopeSplitterWindow.h: interface for the StereoscopeSplitterWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOSCOPESPLITTERWINDOW_H__E4AA5030_1E41_4DD6_A5C8_9E630380E8D6__INCLUDED_)
#define AFX_STEREOSCOPESPLITTERWINDOW_H__E4AA5030_1E41_4DD6_A5C8_9E630380E8D6__INCLUDED_

#include <afxext.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class StereoscopeSplitterWindow : public CSplitterWnd  
{
public:
	StereoscopeSplitterWindow();
	virtual ~StereoscopeSplitterWindow();
	virtual BOOL CreateStatic(CWnd* pParentWnd, int nRows, int nCols, DWORD dwStyle = WS_CHILD | WS_VISIBLE, UINT nID = AFX_IDW_PANE_FIRST);
  afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_DYNCREATE(StereoscopeSplitterWindow)
	DECLARE_MESSAGE_MAP()
private:
	bool fSplitterCreated;
};

#endif // !defined(AFX_STEREOSCOPESPLITTERWINDOW_H__E4AA5030_1E41_4DD6_A5C8_9E630380E8D6__INCLUDED_)
