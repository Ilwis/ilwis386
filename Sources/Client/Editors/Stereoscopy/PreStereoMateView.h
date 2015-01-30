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
// PreStereoMateview.h: interface for the PreStereoMateview class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESTEREOMATEVIEW_H__2F378AF7_A6C3_4B57_9A69_CBB6E11F0C73__INCLUDED_)
#define AFX_PRESTEREOMATEVIEW_H__2F378AF7_A6C3_4B57_9A69_CBB6E11F0C73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#if !defined(AFX_SIMPLEMAPPANEVIEW_H__954F2C00_C0DA_49FF_9D5A_E12B3F3B59BA__INCLUDED_)
//#include "Client\Mapwindow\SimpleMapPaneView.h"
//#endif

#if !defined(AFX_MAPPANEVIEW_H__8A842673_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\MapPaneView.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class MakeEpipolarDocument;
class Symbol;

class IMPEXP PreStereoMateView : public MapPaneView  
{
	friend class MakeEpipolarDocument;
public:
	void SetEpipolarDocument(MakeEpipolarDocument *);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void drawSymbols(volatile bool* fDrawStop);
	Color colFidMarks; // customizable items are public: "Customize" is implemented in StereoMapWindow
	Color colPrincPnts;
	Color colScalePnts;
	Color colUserPrincPnts;

protected:
	PreStereoMateView();
	virtual ~PreStereoMateView();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	void OnNcPaint();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

	DECLARE_DYNCREATE(PreStereoMateView)
	DECLARE_MESSAGE_MAP()
private:
	MakeEpipolarDocument * med;
	void SetDirty(Coord crd);
  zCursor curSetFiducial01, curSetFiducial02, curSetFiducial03, curSetFiducial04, curSetPP,
		curSetTPP, curSetOFP01, curSetOFP02, curNormal;
	bool fActive;
};



#endif // !defined(AFX_PRESTEREOMATEVIEW_H__2F378AF7_A6C3_4B57_9A69_CBB6E11F0C73__INCLUDED_)
