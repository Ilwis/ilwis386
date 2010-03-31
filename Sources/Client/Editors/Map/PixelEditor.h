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
// PixelEditor.h: interface for the PixelEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIXELEDITOR_H__9A61F155_3370_11D3_B792_00A0C9D5342F__INCLUDED_)
#define AFX_PIXELEDITOR_H__9A61F155_3370_11D3_B792_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PixelEditor: public Editor  
{
public:
	PixelEditor(MapPaneView*, Map, int iMAXSEL=100000);
	virtual ~PixelEditor();
  virtual IlwisObject obj() const;
  virtual RangeReal rrStretchRange() const;
  virtual void PreDraw(); // called before all other draw routines
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
	virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual bool OnLButtonDown(UINT nFlags, CPoint point);
	virtual bool OnContextMenu(CWnd* pWnd, CPoint point);
  virtual int Edit(const Coord&);
  virtual void EditFieldOK(Coord, const String&);
	virtual zIcon icon() const;
	virtual String sTitle() const;
	virtual void SelectionChanged();
protected:
  int iShift(bool fMessage = false) const;
	Color clrGet(RowCol rc);
	Color clrRaw(long iRaw) const;
	Color clrVal(double rVal) const;
	void errorMaxSelect();
  void addSelect(RowCol);
  void clearSelect(RowCol);
  void switchSelect(RowCol);
  virtual void drawSelect(RowCol rc = rcUNDEF);
  int Edit(const Coord&, unsigned int htp);
private:
	void clrSelect(RowCol rc = rcUNDEF);
	void clrSelectPix(RowCol rc, int iShft);
	void drawSelectPix(CDC* cdc, RowCol rc, int iShft);
protected:
	void drawCursor();
	void removeCursor();
  Map mp;
  RowCol rcCursor;
  Array<RowCol> rcSelect;
  double rFactVisibleLimit;
	const int iMAXSEL;
private:
	CBitmap* bmTmp;
	void AreaSelected(CRect);
	int iFmtPnt, iFmtDom;
	bool fValues, fImage, fRealValues, fBool, fBit;
	DomainClass* pdc;
//	Array<Color> clrClass;
//{{AFX_VIRTUAL(PixelEditor)
	//}}AFX_VIRTUAL
	//{{AFX_MSG(PixelEditor)
	afx_msg void OnCopy();
	void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnPaste();
	void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnEdit();
	afx_msg void OnClear();
	afx_msg void OnCut();
//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PIXELEDITOR_H__9A61F155_3370_11D3_B792_00A0C9D5342F__INCLUDED_)
