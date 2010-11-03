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
// SimpleMapPaneView.h: interface for the SimpleMapPaneView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEMAPPANEVIEW_H__954F2C00_C0DA_49FF_9D5A_E12B3F3B59BA__INCLUDED_)
#define AFX_SIMPLEMAPPANEVIEW_H__954F2C00_C0DA_49FF_9D5A_E12B3F3B59BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ZOOMABLEVIEW_H__26187CDC_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\ZoomableView.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class Editor;
class MapCompositionDoc;
class InfoLine;

namespace ILWIS {
	class AbstractMapDrawer;
	class BaseMapEditor;
}

class IMPEXP SimpleMapPaneView : public ZoomableView 
{
	friend class AreaSelector;
public:
	SimpleMapPaneView();
	virtual ~SimpleMapPaneView();
	MapCompositionDoc* GetDocument();
	virtual void OnInitialUpdate();     
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	double rScaleShow();	// returns 1:.... value
	void Coord2RowCol(Coord crd, double& rRow, double &rCol);
	Coord crdPnt(zPoint pnt);
	zPoint pntPos(Coord crd);
	ILWIS::BaseMapEditor* editGet() ;
	void createEditor(ILWIS::AbstractMapDrawer *drw);

	InfoLine* info;

protected:  
	virtual void OnDraw(CDC* pDC); 
	BOOL OnEraseBkgnd(CDC* pDC);
	void SetDirty();	
	void SetDirtySilent();
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMeasureDist();
	void OnUpdateMeasureDist(CCmdUI* pCmdUI);
	afx_msg void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	bool volatile fDirty;
	bool volatile fRedrawing;
	bool volatile fDrawStop;
	bool fStarting;
	bool fDrawAlsoWhenLoading; 
	CoordWithCoordSystem cwcsButtonDown;
	CDC* dcView; // dcView is our own CDC and should be cleaned up at next cycle
	ILWIS::BaseMapEditor *edit;
private:  
	void RedrawInThread();
	static UINT RedrawPaneInThread(LPVOID);
	void Draw();
	void RequestRedraw();
	CWinThread * drawThread;
	bool fDrawRequest;
	bool fStopDrawThread;
	bool fResizing;
	CCriticalSection csResizing;
	CCriticalSection csThread;
	static UINT DrawInThread(LPVOID);
	static void MoveMouse(short xInc, short yInc) ;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	FrameWindow* fwPar;
	CCriticalSection csDcView;
	CBitmap* bmView; // bmView contains our own bm and should be cleaned up at next cycle
	HBITMAP hBmOld; // The old bitmap that popped out of SelectObject and after reinserting
	// frees our own bm from GDI. Note that we prefer a handle and not a
	// CBitmap* because the handles are permanent and the TempGDIObjects not
	HMODULE hmodMsimg32;
	typedef BOOL (WINAPI *AlphaBlendFunc)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
	AlphaBlendFunc alphablendfunc;
	CDC * pDC;

	DECLARE_DYNCREATE(SimpleMapPaneView)
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SIMPLEMAPPANEVIEW_H__954F2C00_C0DA_49FF_9D5A_E12B3F3B59BA__INCLUDED_)
