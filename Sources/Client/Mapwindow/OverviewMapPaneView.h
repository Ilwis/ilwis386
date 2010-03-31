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
// OverviewMapPaneView.h: interface for the OverviewMapPaneView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OVERVIEWMAPPANEVIEW_H__18CC7D1D_7E50_4CFA_9535_FBE9C7C73EDE__INCLUDED_)
#define AFX_OVERVIEWMAPPANEVIEW_H__18CC7D1D_7E50_4CFA_9535_FBE9C7C73EDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SIMPLEMAPPANEVIEW_H__954F2C00_C0DA_49FF_9D5A_E12B3F3B59BA__INCLUDED_)
#include "Client\Mapwindow\SimpleMapPaneView.h"
#endif

class OverviewMapPaneView : public SimpleMapPaneView 
{
public:
	_export OverviewMapPaneView();
	virtual ~OverviewMapPaneView();
	virtual void OnInitialUpdate(); 
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:  
  void OnSize(UINT nType, int cx, int cy);
	virtual void OnDraw(CDC* pDC); 
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	void OnContextMenu(CWnd* pWnd, CPoint point);
  void OnSelectArea();
  void UpdateSelf();
	void OnLButtonDown(UINT nFlags, CPoint point);
private:
  void AreaSelected(CRect rect);
  bool hasWMSDrawer();
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_OVERVIEWMAPPANEVIEW_H__18CC7D1D_7E50_4CFA_9535_FBE9C7C73EDE__INCLUDED_)
