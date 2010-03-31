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
// SMCEWindow.h: interface for the SMCEWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMCEWINDOW_H__9A0D848B_F83C_4640_BD5B_DA40A505C2B0__INCLUDED_)
#define AFX_SMCEWINDOW_H__9A0D848B_F83C_4640_BD5B_DA40A505C2B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #if !defined(AFX_BUTTONBAR_H__054E1556_3E6F_11D3_9CE3_00A0C9D5320B__INCLUDED_)
// #include "Client\Base\ButtonBar.h"
// #endif

#if !defined(DataWindow_H)
#include "Client\Base\datawind.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP SMCEWindow : public DataWindow  
{
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(SMCEWindow)

public:
	virtual ~SMCEWindow();

protected: // create from serialization only
	SMCEWindow();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolBar();
	afx_msg void OnUpdateToolBar(CCmdUI* pCmdUI);

	ButtonBar    bbSMCE;
};

#endif // !defined(AFX_SMCEWINDOW_H__9A0D848B_F83C_4640_BD5B_DA40A505C2B0__INCLUDED_)
