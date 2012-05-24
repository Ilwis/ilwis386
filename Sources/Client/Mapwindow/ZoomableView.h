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
#if !defined(AFX_ZOOMABLEVIEW_H__26187CDC_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
#define AFX_ZOOMABLEVIEW_H__26187CDC_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZoomableView.h : header file
//

class FrameWindow;
class MapPaneViewTool;
class DataWindow;
class WMSMapDrawer;

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

#define SCROLL_SIZE 10000

/////////////////////////////////////////////////////////////////////////////
// ZoomableView view

//#include "Engine\Drawers\SelectionRectangle.h"

class MapTools : public map<int, MapPaneViewTool *> {
public:
	virtual ~MapTools();
	void OnMouseMove(UINT nFlags, CPoint point, int state=0);
	void OnLButtonDblClk(UINT nFlags, CPoint point, int state=0);
	void OnLButtonDown(UINT nFlags, CPoint point, int state=0);
	void OnLButtonUp(UINT nFlags, CPoint point, int state=0);
	void OnRButtonDblClk(UINT nFlags, CPoint point, int state=0);
	void OnRButtonDown(UINT nFlags, CPoint point, int state=0);
	void OnRButtonUp(UINT nFlags, CPoint point, int state=0);
	bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);


	void OnEscape();
	void reset(bool force=false);
};

class IMPEXP ZoomableView : public CView
{
public:
	enum Case{cNone, cEntire, cZoomIn, cZoomOut, cTranslate, cResize, cRotate, cZoom};
	ZoomableView();           // protected constructor used by dynamic creation
	virtual ~ZoomableView();
	FrameWindow* fwParent();
	DataWindow* dwParent();
	double rScale() const;
	long scale(long,bool fInv=false) const; // Windows coordinates -> internal RowCol
	double scale(double,bool fInv=false) const; // Windows coordinates -> internal RowCol
	RowCol rcPos(zPoint p) const;  // Windows coordinates -> internal RowCol
	RowCol rcOffset() const { return RowCol(iYpos, iXpos); }
	zPoint pntPos(RowCol rc) const;	// internal RowCol -> Windows coordinates
	zPoint pntPos(double rRow, double rCol) const;
	MinMax mmRect(zRect);   // Windows coordinates -> internal RowCol
	virtual MinMax mmBounds();
	zDimension dimPos(RowCol rc); // internal RowCol -> Windows coordinates
	zDimension dimPos(MinMax mm); // internal RowCol -> Windows coordinates
	zRect rctPos(MinMax mm); // internal RowCol -> Windows coordinates
	void Pnt2RowCol(zPoint p, double& rRow, double &rCol);
	MinMax mmVisibleMapArea() const;

	//MapPaneViewTool* as;   // eg. AreaSelector
	MapTools tools;
	//{{AFX_VIRTUAL(ZoomableView)
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL
	int vertLineMove(int);
	int horzLineMove(int);
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnNoTool();
	void noTool(int iTool= 0, bool force = false);
	bool addTool(MapPaneViewTool *tool, int id);
	void AreaSelected(CRect);
	void ShowArea(const MinMax& mmWish);
	void ShowArea(double rScale, long iXpos, long iYpos);
	bool fShowScrollBars() const;
	void ShowScrollBars(bool fYesNo);	
	virtual void SetDirty();
	void ZoomOutAreaSelected(CRect);
	void changeStateTool(int id, bool isActive);

	int vertPageMove(int);
	int vertTop();
	int vertBottom();
	int vertThumbPos(int nPos);
	int vertPixMove(long iDiff, bool fPreScroll=true);
	int horzPageMove(int);
	int horzLeft();
	int horzRight();
	int horzThumbPos(int nPos);
	int horzPixMove(long iDiff, bool fPreScroll=true);

	void setScrollBars();  // uses iXmin, iXmax, iXpos
	void CalcFalseOffsets();
	void CalcMax();
	WMSMapDrawer *getWMSDrawer();

	//{{AFX_MSG(ZoomableView)
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNoTool(CCmdUI* pCmdUI);
	afx_msg void OnSelectArea();
	afx_msg void OnPanArea();
	afx_msg void OnUpdatePanArea(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnReDraw();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void selectArea(CCmdTarget* target, NotifyRectProc proc, const String& cursor, const Color& clr, bool keepDimensions=true);
	//}}AFX_MSG
	void ZoomInPnt(zPoint);
	void ZoomOutPnt(zPoint);
	void viewRotate(const CPoint& pnt, UINT message);
	void viewTranslate(const CPoint& pnt, UINT message);
	void viewZoom(const CPoint& pnt, UINT message);
	void movePoint(const CPoint& pnt, UINT message);
	void PanMove(CPoint pt);


	long iXpos, iYpos;
	long iXfalseOffset, iYfalseOffset;
	long iXpage, iYpage, iXsize, iYsize;
	long iXmin, iXmax, iYmin, iYmax;
	zDimension dim;
	double _rScale;
	bool fAdjustSize;
	int iActiveTool;
	bool fScrollBarsVisible;
	CPoint beginMovePoint;
	Case mode;

	long xOld,yOld;

protected:
	void RecenterZoomHorz(CoordBounds & cbZoom, const CoordBounds & cbMap);
	void RecenterZoomVert(CoordBounds & cbZoom, const CoordBounds & cbMap);

	DECLARE_DYNCREATE(ZoomableView)
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZOOMABLEVIEW_H__26187CDC_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
