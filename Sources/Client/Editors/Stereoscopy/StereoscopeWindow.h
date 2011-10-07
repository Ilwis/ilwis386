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
// StereoscopeWindow.h: interface for the StereoscopeWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOSCOPEWINDOW_H__3CC849FA_08EF_41F5_8007_38FAE012F03B__INCLUDED_)
#define AFX_STEREOSCOPEWINDOW_H__3CC849FA_08EF_41F5_8007_38FAE012F03B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef DataWindow_H
#include "Client\Base\datawind.h"
#endif

#if !defined(AFX_STEREOPAIR_H__B57B6765_CE6C_40E4_AFC6_0D8657C9968C__INCLUDED_)
#include "Engine\Stereoscopy\StereoPair.h"
#endif

#if !defined(AFX_BUTTONBAR_H__054E1556_3E6F_11D3_9CE3_00A0C9D5320B__INCLUDED_)
#include "Client\Base\ButtonBar.h"
#endif

#if !defined(AFX_STEREOSCOPESPLITTERWINDOW_H__E4AA5030_1E41_4DD6_A5C8_9E630380E8D6__INCLUDED_)
#include "Client\Editors\Stereoscopy\StereoscopeSplitterWindow.h"
#endif

#if !defined(AFX_SCALEBAR_H__BB72FBC4_4BCE_11D3_B7BC_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\ScaleBar.h"
#endif

#if !defined(AFX_GeneralBar_H__A9C4CEB4_A34D_11D3_B821_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Utils\GeneralBar.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class StereoscopePaneView;
class MapCompositionDoc;

class IMPEXP StereoscopeWindow : public DataWindow  
{
public:
	bool fXoffsetLocked();
	int iXoffsetDelta();
	void SetXoffsetDelta(int);
	bool fRequestMasterLock();
	void ReleaseMasterLock();
	ButtonBar bbStereoscope;
protected:
	StereoscopeWindow();
	virtual ~StereoscopeWindow();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg void OnClose();
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnMoving(UINT nSide, LPRECT lpRect);
	afx_msg LRESULT OnUpdate(WPARAM, LPARAM);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnOpenStereoPair();
	void OnStereoscopeBar();
	void OnUpdateStereoscopeBar(CCmdUI* pCmdUI);
	void OnLeftRightStereoBar();
	void OnUpdateLeftRightStereoBar(CCmdUI* pCmdUI);
	void OnUnlockHScroll();
	void OnUpdateUnlockHScroll(CCmdUI* pCmdUI);
	void OnActivateLeftView();
	void OnUpdateActiveLeftView(CCmdUI* pCmdUI);
	void OnActivateRightView();
	void OnUpdateActiveRightView(CCmdUI* pCmdUI);
	void OnUpdateEdit(CCmdUI* pCmdUI);
	void OnUpdateMapOperation(CCmdUI* pCmdUI);
	
	void OnLayerManagement();
	void OnUpdateLayerManagement(CCmdUI* pCmdUI);
	afx_msg void OnOverviewWindow();
	afx_msg void OnUpdateOverviewWindow(CCmdUI* pCmdUI);
	/*
	afx_msg void OnScaleControl();
	afx_msg void OnUpdateScaleControl(CCmdUI* pCmdUI);*/
	afx_msg void OnInitMenu(CMenu* pMenu); 
	StereoscopeSplitterWindow wndSplitter;
  StereoscopePaneView *vwLeft, *vwRight;
  MapCompositionDoc *docLeft;
	MapCompositionDoc *docRight;
	CMenu menPropLayer, menEditLayer, menDataLayer;
  HMENU hMenFile, hMenEdit;
	GeneralBar ltbLeft, ltbRight;
	//ScaleBar barScale;
private:
	StereoPair stp;
	void StereoPairUpdated();
	void RefreshMaps(const String& sLeftMap, const String& sRightMap);
	bool _fXoffsetLocked;
	int _iXoffsetDelta; // positive value means distance between maps should increase
	LONG iWindowMiddle;
	bool fMasterLocked;

	DECLARE_DYNCREATE(StereoscopeWindow)
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_STEREOSCOPEWINDOW_H__3CC849FA_08EF_41F5_8007_38FAE012F03B__INCLUDED_)
