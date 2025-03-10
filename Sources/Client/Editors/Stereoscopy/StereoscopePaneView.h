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
// StereoscopePaneView.h: interface for the StereoscopePaneView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOSCOPEPANEVIEW_H__6EC52FB1_4618_47DE_B03F_2EFBCEC7620A__INCLUDED_)
#define AFX_STEREOSCOPEPANEVIEW_H__6EC52FB1_4618_47DE_B03F_2EFBCEC7620A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_MAPPANEVIEW_H__8A842673_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\MapPaneView.h"
#endif

#undef IMPEXP
#ifdef STEREOSCOPY_EXPORTS
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class StereoscopeWindow;

class StereoscopePaneView : public MapPaneView  
{
public:
	StereoscopePaneView();
	virtual ~StereoscopePaneView();

	void SetSiblingPane(StereoscopePaneView*);
	void SetLeft(bool);

protected:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void OnEntireMap();
	void OnNcPaint();
	virtual void OnInitialUpdate();
	virtual void AreaSelected(CRect);
	virtual void ZoomOutAreaSelected(CRect);
	virtual void PanMove(CPoint pt);
	virtual int vertPixMove(long iDiff, bool fPreScroll=true);
	virtual int horzPixMove(long iDiff, bool fPreScroll=true);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg void OnNoneEdit();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnPanArea();
	afx_msg void OnNoTool();
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void SetXPosSibling(CoordBounds & cbZoom2);
	void PanMoveY(int y, CoordBounds & cbZoom2);
	double rGetXOffsetSibling();

private:
	StereoscopePaneView * spvSiblingPane;
	StereoscopeWindow * swParent;
	bool fCoupledVertScroll;
	bool fLeft; // "we" must know this to calculate with the delta
	bool fActive;

	DECLARE_DYNCREATE(StereoscopePaneView)
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_STEREOSCOPEPANEVIEW_H__6EC52FB1_4618_47DE_B03F_2EFBCEC7620A__INCLUDED_)
