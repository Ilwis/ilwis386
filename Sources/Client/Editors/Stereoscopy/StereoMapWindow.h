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
// StereoMapWindow.h: interface for the StereoMapWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOMAPWINDOW_H__EEA3F0A7_6844_4646_BA6E_AA607A79FFC1__INCLUDED_)
#define AFX_STEREOMAPWINDOW_H__EEA3F0A7_6844_4646_BA6E_AA607A79FFC1__INCLUDED_

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

#if !defined(AFX_EPISPLITTERWINDOW_H__2A42A06D_EF78_4FED_A4B9_08E260E707F3__INCLUDED_)
#include "Client\Editors\Stereoscopy\EpiSplitterWindow.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class PreStereoMateView;
class MapCompositionDoc;
class MakeEpipolarFormView;
class MakeEpipolarDocument;
class StereoPair;

class IMPEXP StereoMapWindow: public DataWindow
{
public:
	void UpdateStatusText();
protected:
	StereoMapWindow();
	virtual ~StereoMapWindow();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg void OnClose();
  void OnNewStereoPair();
	void OnOpenStereoPair();
	void OnExitShowStereoPair();
	void OnUpdateExitShowStereopair(CCmdUI* pCmdUI);
	void OnQuit();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
  EpiSplitterWindow wndSplitter; 
  PreStereoMateView *vwLeft, *vwRight;
  MapCompositionDoc *docLeft, *docRight;
	MakeEpipolarFormView *frmLeft, *frmRight;
	MakeEpipolarDocument *mkDocLeft, *mkDocRight;
	ButtonBar bbStereoscopy;
	void OnStereoscopyBar();
	void OnUpdateStereoscopyBar(CCmdUI* pCmdUI);
	void OnActivateLeftView();
	void OnUpdateActiveLeftView(CCmdUI* pCmdUI);
	void OnActivateRightView();
	void OnUpdateActiveRightView(CCmdUI* pCmdUI);
	void OnInfoWindowBar();
	void OnUpdateInfoWindowBar(CCmdUI* pCmdUI);
	void OnConfigure();
	CEdit edTxt;
	CFont fnt;
	CToolBar bbTxt;

private:
	StereoPair stp;
	void StereoPairUpdated();
	void RefreshMaps(const String& sLeftMap, const String& sRightMap);
	void RemoveViews();
	void CopyLeftColorsToRightView();
	bool fInitiallyUpdated;
 
	DECLARE_DYNCREATE(StereoMapWindow)
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_STEREOMAPWINDOW_H__EEA3F0A7_6844_4646_BA6E_AA607A79FFC1__INCLUDED_)
