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
// PointEditor.h: interface for the PointEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTEDITOR_H__3DE28B44_390F_11D3_B79A_00A0C9D5342F__INCLUDED_)
#define AFX_POINTEDITOR_H__3DE28B44_390F_11D3_B79A_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PointEditor: public DigiEditor    
{
public:
	PointEditor(MapPaneView*, PointMap);
	virtual ~PointEditor();
  virtual IlwisObject obj() const;
//  virtual void PreDraw(); // called before all other draw routines
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
	virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual bool OnLButtonDown(UINT nFlags, CPoint point);
	virtual bool OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual bool OnLButtonUp(UINT nFlags, CPoint point);
	virtual bool OnMouseMove(UINT nFlags, CPoint point);
	virtual bool OnContextMenu(CWnd* pWnd, CPoint point);
  virtual int Edit(const Coord&);
  virtual void EditFieldOK(Coord, const String&);
	virtual zIcon icon() const;
	virtual String sTitle() const;
private:
  enum enumMode { modeSELECT, modeMOVE, modeADD,
		  modeMOVING } mode;
  void Mode(enumMode);
  int drawPoint(long iNr);
  zRect rectPoint(long iNr);
	void AreaSelected(CRect);
  bool fCopyOk();
  bool fEditOk();
  bool fPasteOk();
  void drawSelect(Coord c = crdUNDEF);
  void drawSelectPnt(zDisplay*, long iRec);
  void switchSelect(Coord);
  void AddPnt();
  bool DeselectAll();
  int DigInit(Coord);
  int AddPoint(Coord);
  int EditPoint(Coord);
  int EditPointInit(Coord);
  int EditPointCrd(Coord);
  int EditPointValue(Coord);
  int SelPoint(Coord);
  void UndoAll();
	void EditAttrib(int iRec);
  //FileName fnSave;
  PointMap mp;
  Array<char> aSelect;
  Symbol smb;
  bool fText, fCtrl, fShft, fFindUndefs;
  Color colText, colBack, colFindUndef;
  CFont fnt;
	int iFmtPnt, iFmtDom;
  long iActNr;
  zCursor curEdit, curPntEdit, curPntMove, curPntMoving;
  Coord crdValue;

	//{{AFX_VIRTUAL(PointEditor)
	//}}AFX_VIRTUAL
	//{{AFX_MSG(PointEditor)
	afx_msg void OnCopy();
	void OnUpdateCopy(CCmdUI* pCmdUI);
	void OnUpdateEdit(CCmdUI* pCmdUI);
	afx_msg void OnPaste();
	void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnClear();
	afx_msg void OnCut();
  void OnExit();
	void OnUpdateMode(CCmdUI* pCmdUI);
	afx_msg void OnSelectMode();
	afx_msg void OnMoveMode();
	afx_msg void OnInsertMode();
	afx_msg void OnFindUndefs();
	afx_msg void OnConfigure();
  void OnSetBoundaries();
	void OnAddPoint();
  void OnUndoAllChanges();
	void OnFileSave();
	void OnUpdateFileSave(CCmdUI* pCmdUI);
	void OnSelectAll();
//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_POINTEDITOR_H__3DE28B44_390F_11D3_B79A_00A0C9D5342F__INCLUDED_)
