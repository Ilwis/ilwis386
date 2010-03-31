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
#if !defined(AFX_MAPCOMPOSITIONINPLACEFRAME_H__9360F1F8_B916_11D3_B838_00A0C9D5342F__INCLUDED_)
#define AFX_MAPCOMPOSITIONINPLACEFRAME_H__9360F1F8_B916_11D3_B838_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapCompositionInPlaceFrame.h : header file
//

#if !defined(AFX_LAYERTREEBAR_H__4E781484_676B_11D3_B7DC_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\LayerTreeBar.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// MapCompositionInPlaceFrame frame

class IMPEXP MapCompositionInPlaceFrame : public COleIPFrameWnd
{
public:
	MapCompositionInPlaceFrame();           
	virtual ~MapCompositionInPlaceFrame();

	virtual BOOL OnCreateControlBars(CWnd* pWndFrame, CWnd* pWndDoc);	
	virtual HMENU GetInPlaceMenu();
	//{{AFX_VIRTUAL(MapCompositionInPlaceFrame)
	//}}AFX_VIRTUAL
protected:
	COleResizeBar m_wndResizeBar;
	COleDropTarget m_dropTarget;
	LayerTreeBar ltb;
	ButtonBar bbMapWindow;
	ScaleBar barScale;
	CMenu men, menDataLayer;
	//{{AFX_MSG(MapCompositionInPlaceFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnInitMenu(CMenu* pMenu);
	//}}AFX_MSG
	DECLARE_DYNCREATE(MapCompositionInPlaceFrame)
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPCOMPOSITIONINPLACEFRAME_H__9360F1F8_B916_11D3_B838_00A0C9D5342F__INCLUDED_)
