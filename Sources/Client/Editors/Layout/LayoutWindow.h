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
#if !defined(AFX_LAYOUTWINDOW_H__26187C9E_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
#define AFX_LAYOUTWINDOW_H__26187C9E_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayoutWindow.h : header file
//
#if !defined(AFX_BUTTONBAR_H__054E1556_3E6F_11D3_9CE3_00A0C9D5320B__INCLUDED_)
#include "Client\Base\ButtonBar.h"
#endif

#if !defined(AFX_LAYOUTLISTBAR_H__78B985ED_FFC4_11D3_B87F_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Layout\LayoutListBar.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP LayoutWindow: public DataWindow
{
public:
	LayoutWindow();           // protected constructor used by dynamic creation
	virtual ~LayoutWindow();
protected:
	//{{AFX_VIRTUAL(LayoutWindow)
	//}}AFX_VIRTUAL
	//{{AFX_MSG(LayoutWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnStandardBar();
	afx_msg void OnLayoutBar();
	afx_msg void OnUpdateStandardBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayoutBar(CCmdUI* pCmdUI);
	afx_msg void OnLayerManagement();
	afx_msg void OnUpdateLayerManagement(CCmdUI* pCmdUI);
	//}}AFX_MSG
protected:
	ButtonBar bbStandard, bbLayout;
	LayoutListBar llb;
	DECLARE_DYNCREATE(LayoutWindow)
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYOUTWINDOW_H__26187C9E_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
