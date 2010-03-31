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
#ifndef RPRCLASS_LB_H
#define RPRCLASS_LB_H

class RepresentationClassView;
class RepresentationClassLB;

class RepresentationClassLB : public CListBox
{
public:
	enum DrawMode { opRAS, opPOL, opSEG, opPNT };
	enum ndNameDisplay { ndName, ndNameCode, ndCode };
	
	RepresentationClassLB();
	~RepresentationClassLB();
	
	BOOL Create(CTabCtrl *parent, RepresentationClassView *);
	void   InitUndo();
	void   SetUndo(int id, Color clr);

	void   SetDrawMode(RepresentationClassLB::DrawMode dm);
	const  RepresentationClassLB::DrawMode GetDrawMode();
	
	void                DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct );
	void                MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	bool                fCanUndo();
	void                Undo();
	void                SetColor(Color clr);
	void                SetNameDisplayState(ndNameDisplay nState);
	void                Refresh();
	void                OnSelChange();
	
#ifdef _DEBUG
	void AssertValid() const;
	void Dump(CDumpContext& dc) const;
#endif
	
private:
	void                OnDoubleClick();
	void                OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
	void                OnMouseMove( UINT nFlags, CPoint point );
	void                OnContextMenu( CWnd* pWnd, CPoint point );
	void                OnEditMultiple();
	void                OnUndo();
	void                OnCustomize();
	void                OnSelectAll();
	
	RepresentationClassView *view;
	zFontInfo           *info;
	DomainSort          *ds;
	DrawMode            opt;
	Representation      _rpr;
	int                 iOldIndex;  // previously selected color item
	
	// Memory for undo Undo "edit color":
	int                 iUndoIndex; // index of color that has been changed
	Color               clrUndo;    // Color before edit
	ndNameDisplay       ndNameState;
	
	DECLARE_MESSAGE_MAP();
	
};

class MapRprStyleTab : public CTabCtrl
{
public:
	~MapRprStyleTab();

	int Create(RepresentationClassView *v);
	void OnSize( UINT nType, int cx, int cy );
	void OnTabPressed( NMHDR * pNotifyStruct, LRESULT* result );
	afx_msg void OnSysColorChange( );
	
	RepresentationClassLB rprClassLB;
	
	DECLARE_MESSAGE_MAP();
private:
	CImageList m_il;
};

#endif //RPRCLASS_LB_H
