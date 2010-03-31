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
// GraphWindow.h: interface for the GraphWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHWINDOW_H__C9C89751_C48C_47E9_90FA_B96E8BE6A096__INCLUDED_)
#define AFX_GRAPHWINDOW_H__C9C89751_C48C_47E9_90FA_B96E8BE6A096__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef DataWindow_H
#include "Client\Base\datawind.h"
#endif
#if !defined(AFX_GeneralBar_H__A9C4CEB4_A34D_11D3_B821_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Utils\GeneralBar.h"
#endif
#if !defined(AFX_BUTTONBAR_H__054E1556_3E6F_11D3_9CE3_00A0C9D5320B__INCLUDED_)
#include "Client\Base\ButtonBar.h"
#endif


#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP GraphWindow : public DataWindow 
{
public:
	GraphWindow();
	virtual ~GraphWindow();
private:
	GeneralBar gbGraphTree;
	ButtonBar bbGraph;
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
  void OnGraphManagement();
	void OnUpdateGraphManagement(CCmdUI* pCmdUI);
	void OnButtonBar();
	void OnUpdateButtonBar(CCmdUI* pCmdUI);
  void OnButtonBarMenuDropDown(NMHDR* nmhdr, LRESULT* result);
  DECLARE_DYNCREATE(GraphWindow)
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_GRAPHWINDOW_H__C9C89751_C48C_47E9_90FA_B96E8BE6A096__INCLUDED_)
