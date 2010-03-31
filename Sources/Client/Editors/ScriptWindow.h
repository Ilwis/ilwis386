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
#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#if !defined(AFX_TABWND_H__E22ADFB8_5063_11D1_8E9B_4854E827046F__INCLUDED_)
#include "Client\Editors\Utils\Tabwnd.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP ScriptWindow : public TextWindow
{
protected: // create from serialization only
	ScriptWindow();
	DECLARE_DYNCREATE(ScriptWindow)
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScriptWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ScriptWindow();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
//	CToolBar    m_wndToolBar;
	virtual void LoadState(IlwisSettings& settings);
	virtual void SaveState(IlwisSettings& settings);

	CTabWnd tabWnd;

// Generated message map functions
protected:					
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext); 
	//{{AFX_MSG(ScriptWindow)
	afx_msg void OnButtonBar();
	afx_msg void OnUpdateBar(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // SCRIPTWINDOW_H
