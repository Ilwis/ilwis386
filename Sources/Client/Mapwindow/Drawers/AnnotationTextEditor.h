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
// AnnotationTextEditor.h: interface for the AnnotationTextEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANNOTATIONTEXTEDITOR_H__5C9849A5_50B2_11D3_B7C4_00A0C9D5342F__INCLUDED_)
#define AFX_ANNOTATIONTEXTEDITOR_H__5C9849A5_50B2_11D3_B7C4_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AnnotationTextEditor: public Editor  
{
public:
	AnnotationTextEditor(MapPaneView*, const AnnotationText&);
	virtual ~AnnotationTextEditor();
  virtual IlwisObject obj() const;
	virtual zIcon icon() const;
	virtual String sTitle() const;
//  virtual void PreDraw(); // called before all other draw routines
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
  virtual int Edit(const Coord&);
	virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual bool OnLButtonDown(UINT nFlags, CPoint point);
	virtual bool OnLButtonUp(UINT nFlags, CPoint point);
	virtual bool OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual bool OnMouseMove(UINT nFlags, CPoint point);
	virtual bool OnContextMenu(CWnd* pWnd, CPoint point);
  void setDirty(long iNr);
private:
  int SelectArea(zRect rect);
  bool DeselectAll();
  zPoint* p4Point(long iNr); // has to be deleted!
  zPoint pPoint(long iNr);
  long iPoint(zPoint);
  bool fInside(long iPoint, zPoint pnt);
  void drawActPoint();
	bool fCopyOk();
	bool fPasteOk();
	void AddText(Coord crd, double rRow, double rCol);
private:
  AnnotationText atx;
  Array<char> aSelect;
  bool fCtrl, fShft;
  Symbol smb;
  double rSc;
	int iFmtPnt, iFmtDom;
  long iActNr;
  zPoint *pPoly, pAct, pCurr;
  enum enumMode { modeSELECT, modeMOVING } mode;

	//{{AFX_MSG(AnnotationTextEditor)
	void OnOptions();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
  void OnSelectAll();
	void OnUpdateCopy(CCmdUI* pCmdUI);
	void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnClear();
	afx_msg void OnCut();
	void OnAddText();
  void OnChangeFont();
  void OnChangeSize();
  void OnChangeBold();
  void OnChangeItalic();
  void OnChangeUnderline();
  void OnChangeColor();
  void OnChangeJust();
  void OnChangeTransparent();
  void OnChangeRotation();
	void OnUpdateMode(CCmdUI* pCmdUI);
//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_ANNOTATIONTEXTEDITOR_H__5C9849A5_50B2_11D3_B7C4_00A0C9D5342F__INCLUDED_)
