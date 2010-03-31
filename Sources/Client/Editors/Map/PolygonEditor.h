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
// PolygonEditor.h: interface for the PolygonEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYGONEDITOR_H__8B508FC4_3E73_11D3_B7A6_00A0C9D5342F__INCLUDED_)
#define AFX_POLYGONEDITOR_H__8B508FC4_3E73_11D3_B7A6_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PolygonEditor: public DigiEditor  
{
	friend class PolConfigForm;
public:
	PolygonEditor(MapPaneView*, PolygonMap);
	virtual ~PolygonEditor();
  virtual IlwisObject obj() const;
	virtual zIcon icon() const;
	virtual String sTitle() const;
	virtual bool OnContextMenu(CWnd* pWnd, CPoint point);
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
	virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual bool OnLButtonDown(UINT nFlags, CPoint point);
	virtual bool OnLButtonUp(UINT nFlags, CPoint point);
	virtual bool OnMouseMove(UINT nFlags, CPoint point);
  virtual int Edit(const Coord&);
  virtual void EditFieldOK(Coord, const String&);
private:
  void drawSegment(CDC*, Geometry *);
  void drawPolygon(CDC*, ILWIS::Polygon *pol);
  void drawPolygonBoundary(CDC*, ILWIS::Polygon *pol);
  void drawPol(CDC*, ILWIS::Polygon *pol);
  void drawPolUndef(CDC*, ILWIS::Polygon *pol);
  void drawCoords(CDC*, Color clr);
  void SetDirty(const CoordBounds&, bool fEraseBackground=false);
  void SetDirty(ILWIS::Polygon *, bool fEraseBackground=false);
  void SetDirty(Geometry *, bool fEraseBackground=false);
	void UnselectAll();

  int AddPolygons(Coord);
  int SimpleEditPolygons(Coord);
	int EditValue(Coord);
	void EditAttrib(int iRec);

  enum enumMode { modeSELECT, modeMOVE, modeADD,
		  modeISLAND, modeBOUNDARY,
		  modeMOVING, modeNODEMOVING,
		  modeTOP,
		  modeMERGE } mode;
  PolygonMap pm;
  ILWIS::Polygon *pol;
  SList<ILWIS::Polygon *> polList;
  zCursor curEdit, curSegEdit, curSegMove, curSegMoving;
  bool fDigBusy, fRetouching, fFindUndefs;
  Color col, colRetouch, colDeleted, colFindUndef;
  Coord* coords;
  long iNrCoords;
  long iActCrd;
  Coord cNode;
  Coord crdNode;
  int iSnapPixels;
  bool fAutoSnap, fShowNodes, fUndelete;
  int iFmtPnt, iFmtDom;

	//{{AFX_VIRTUAL(PolygonEditor)
	//}}AFX_VIRTUAL
	//{{AFX_MSG(PolygonEditor)
	void OnUpdateEdit(CCmdUI* pCmdUI);
	afx_msg void OnCopy();
	void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnFindUndefs();
	void OnUpdateMode(CCmdUI* pCmdUI);
  afx_msg void OnConfigure();
	void OnCreateLabels();
	void OnApplyLabels();
	void OnExtractBoundaries();
  void OnUndoAllChanges();
	void OnFileSave();
	void OnUpdateFileSave(CCmdUI* pCmdUI);
	void OnSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_POLYGONEDITOR_H__8B508FC4_3E73_11D3_B7A6_00A0C9D5342F__INCLUDED_)
