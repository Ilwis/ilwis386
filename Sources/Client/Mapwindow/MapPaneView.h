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
#if !defined(AFX_MAPPANEVIEW_H__8A842673_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
#define AFX_MAPPANEVIEW_H__8A842673_E359_11D2_B73E_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapPaneView.h : header file
//
#if !defined(AFX_SIMPLEMAPPANEVIEW_H__954F2C00_C0DA_49FF_9D5A_E12B3F3B59BA__INCLUDED_)
#include "Client\Mapwindow\SimpleMapPaneView.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class FrameWindow;
class MapWindow;
class MapPaneViewTool;
class RecordBar;
class COleDropTarget;
class PixelInfoBar;

/////////////////////////////////////////////////////////////////////////////
// MapPaneView view

class IMPEXP MapPaneView : public SimpleMapPaneView
{
public:
	MapPaneView();          
	virtual ~MapPaneView();
	void UpdateFrame();
	MapWindow* mwParent();

	void EditNamedLayer(const FileName& fn);
	virtual void OnInitialUpdate();     // first time after construct	

	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

  void UseScale(double rScale);
	void GetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	bool fBusyDrawing() { return fRedrawing; }
	BOOL AddFiles(vector<FileName>& afn, bool asAnimation);
	void ShowRecord(const Ilwis::Record& rec);
	void ZoomInOn(Coord crd, double rDist);
	afx_msg void OnEntireMap();
  afx_msg LRESULT OnUpdate(WPARAM, LPARAM);
protected:
	void SetDirty();	
  LRESULT OnViewSettings(WPARAM wP, LPARAM lP);
	// Generated message map functions
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPixelEdit();
	afx_msg void OnPointEdit();
	afx_msg void OnSegmentEdit();
	afx_msg void OnPolygonEdit();
	afx_msg void OnNoneEdit();
	afx_msg void OnGeoRefEdit();
	afx_msg void OnCoordSysEdit();
	afx_msg void OnUpdateGeoRefEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCoordSysEdit(CCmdUI* pCmdUI);
	afx_msg void OnAdjustSize();
	afx_msg void OnUpdateAdjustSize(CCmdUI* pCmdUI);
	afx_msg void OnDefaultScale();
  afx_msg void OnScaleOneToOne();
	afx_msg void OnUpdateScaleOneToOne(CCmdUI* pCmdUI);
	afx_msg void OnSetDoublClickAction();
	afx_msg void OnSetDoubleClickAction(UINT nID);
	afx_msg void OnUpdateDoubleClickAction(CCmdUI* pCmdUI);
	afx_msg void OnPaste();
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnCopy();
	afx_msg void OnCreatePntMap();
	afx_msg void OnCreateSegMap();
	afx_msg void OnCreateRasMap();
	afx_msg void OnCreateGeoRef();
	afx_msg void OnCreateCoordSys();
	afx_msg void OnCreateSampleSet();
	afx_msg void OnCreateSubMap();
	afx_msg void OnUpdateEntireMap(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT timerID);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void OnShowRecordView();
	void OnUpdateShowRecordView(CCmdUI* pCmdUI);

private:
	enum DoubleClickAction { dcaRECORD, dcaRPR, dcaACTION };
	DoubleClickAction dca;
	BOOL EditCopy(CRect mRect, int nReduceResCount);
	BOOL SetMemDcPixelFormat(HDC hMemDC, BOOL bUseAPI = FALSE);
	MapWindow* mwPar;
	RecordBar* recBar;
	COleDropTarget* odt;
	static UINT AddFilesInThread(LPVOID pParam);
	DECLARE_DYNCREATE(MapPaneView)
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPPANEVIEW_H__8A842673_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
