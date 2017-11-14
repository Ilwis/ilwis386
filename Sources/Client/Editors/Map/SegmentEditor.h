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
// SegmentEditor.h: interface for the SegmentEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEGMENTEDITOR_H__607F0836_3AAC_11D3_B79E_00A0C9D5342F__INCLUDED_)
#define AFX_SEGMENTEDITOR_H__607F0836_3AAC_11D3_B79E_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SegmentEditor: public DigiEditor  
{
friend class SegConfigForm;
public:
	SegmentEditor(MapPaneView*, SegmentMap);
	virtual ~SegmentEditor();
  virtual IlwisObject obj() const;
  virtual RangeReal rrStretchRange() const;
  virtual Color clrRaw(long iRaw) const;
  virtual Color clrVal(double rVal) const;
  virtual int draw(volatile bool* fDrawStop);
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
	virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual bool OnLButtonDown(UINT nFlags, CPoint point);
	virtual bool OnLButtonUp(UINT nFlags, CPoint point);
	virtual bool OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual bool OnRButtonDown(UINT nFlags, CPoint point);
	virtual bool OnMouseMove(UINT nFlags, CPoint point);
	virtual bool OnContextMenu(CWnd* pWnd, CPoint point);
  virtual int Edit(const Coord&);
  virtual void EditFieldOK(Coord, const String&);
	virtual zIcon icon() const;
	virtual String sTitle() const;
private:
  bool fCopyOk();
  bool fPasteOk();
  enum enumMode { modeSELECT, modeMOVE, modeADD,
		  modeENTERING, modeMOVING, modeSPLIT } mode;
  void Mode(enumMode);
  int AddSegments(Coord);
  int BeginSegment(Coord);
  int SnapSegment(Coord);
  int SnapSegmentAccept(Coord);
  int SnapSplitSegment(Coord);
  int SnapSplitSegmentAccept(Coord);
  int BeginSegmentMouse(Coord);
  int SnapSegmentMouse(Coord);
  int SnapSplitSegmentMouse(Coord);
  int MovePivot(Coord);
  int EnterPoints(Coord);
  int EndSegment(Coord);
  int SnapEndSegment(Coord);
  int SnapEndSegmentMouse(Coord);
  int SnapSplitEndSegmentMouse(Coord);
  int AutoEndSegmentMouse(Coord);
  int SnapAccept(Coord);
  int SnapSplit(Coord);
  int StartEnteringPoints(Coord);
  int DeleteLastPoint(Coord);
  int MovePoints(Coord);
  int TakePoint(Coord);
  int TakePointMouse(Coord);
  int MoveCursorPoint(Coord);
  int NewPosPoint(Coord);
  int NewPosPointMouse(Coord);
  int SnapPoint(Coord);
  int SnapPointMouse(Coord);
  int SnapSplitPoint(Coord);
  int SnapSplitPointAccept(Coord);
  int SnapSplitPointMouse(Coord);
  int MergeSplit(Coord);
  void AskMerge(Coord crdNode);
  int AskSplit(ILWIS::Segment *, long iAfter, Coord crdAt); // splitted: 0, cancelled: 1
  void SetActNode(Coord);
  void drawActNode();
  void drawSegment(ILWIS::Segment *, bool fExact);
  int drawCoords(Color clr);
  void drawActNode(CDC* cdc);
  void drawSegment(CDC* cdc, ILWIS::Segment *, bool fExact);
  int drawCoords(CDC* cdc, Color clr);
  int TunnelSegment();
  bool fCheckSelf(Tranquilizer&, long iStartSeg=0); // returns true if ok
  bool fCheckConnected(Tranquilizer&, long iStartSeg=0); // returns true if ok
  bool fCheckIntersects(Tranquilizer&, long iStartSeg=0); // returns true if ok
  bool fCheckCodeConsistency(Tranquilizer&, long iStartSeg=0); // returns true if ok
	bool fCheckClosedSegments(Tranquilizer&, long iStartSeg=0);
	void EditAttrib(int iRec);
  void UndoAll();
	void ZoomInOnError();
  FileName fnSave;
  SegmentMap sm;
  ILWIS::Segment *currentSeg;
  long iActCrd;
  vector<Coord> coords;
  bool fDigBusy;
  bool fRetouching, fUndelete, fFindUndefs;
  zCursor curEdit, curSegEdit, curSegMove, curSegMoving,
          curSegSplit, curSegSplitting;
  Color col, colRetouch, colDeleted, colFindUndef;
  int iSnapPixels;
  bool fAutoSnap, fShowNodes;
  SList<ILWIS::Segment *> segList;
  enum { drwDOMAIN, drwPRIMARY, drwNORMAL } drw;
  int iFmtPnt, iFmtDom;
  bool fOnlySelected;
  Coord crdActNode;
  String sMask;
	long iLastTick;
	//{{AFX_VIRTUAL(SegmentEditor)
	//}}AFX_VIRTUAL
	//{{AFX_MSG(SegmentEditor)
	afx_msg void OnSetVal();
	void OnUpdateSetVal(CCmdUI* pCmdUI);
	afx_msg void OnCopy();
	void OnUpdateCopy(CCmdUI* pCmdUI);
	void OnUpdateEdit(CCmdUI* pCmdUI);
	afx_msg void OnPaste();
	void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnClear();
	void OnUpdateClear(CCmdUI* pCmdUI);
	afx_msg void OnCut();
	void OnUpdateMode(CCmdUI* pCmdUI);
	afx_msg void OnSelectMode();
	afx_msg void OnMoveMode();
	afx_msg void OnInsertMode();
	afx_msg void OnSplitMode();
	afx_msg void OnUnDelSeg();
	afx_msg void OnFindUndefs();
  afx_msg void OnConfigure();
  void OnCheckSelf();
  void OnCheckConnected();
  void OnCheckIntersects();
  void OnCheckCodeConsistency();
	void OnCheckClosedSegments();
  void OnRemoveRedundantNodes();
  void OnPolygonize();
  void OnUndoAllChanges();
	void OnFileSave();
	void OnUpdateFileSave(CCmdUI* pCmdUI);
	void OnSegPack();
	void OnUpdateSegPack(CCmdUI* pCmdUI);
	void OnSetBoundaries();
	void OnSelectAll();
//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SEGMENTEDITOR_H__607F0836_3AAC_11D3_B79E_00A0C9D5342F__INCLUDED_)
