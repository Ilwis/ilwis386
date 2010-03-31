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
#ifndef REPRESENTATIONCLASSVIEW_H
#define REPRESENTATIONCLASSVIEW_H

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

#define iBARWIDTH 100

class RepresentationWindow;

//---[ RepresentationClassView ]----------------------------------------------------- 
class IMPEXP RepresentationClassView : public RepresentationView
{
	friend class RepresentationClassLB;
	friend class RepresentationWindow;
public:
	RepresentationClassView();
	virtual ~RepresentationClassView();

	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;

#endif
	int                 iGetColumnWidth();

protected:
	DECLARE_DYNCREATE(RepresentationClassView)
	RepresentationClassDoc* GetDocument();

	virtual void        LoadState(IlwisSettings& settings);
	virtual void        SaveState(IlwisSettings& settings);

	void                OnCustomize();
	void                OnRaster();
	void                OnPolygon();
	void                OnSegment();
	void                OnUndo();
	void                OnPoint();
	void                OnUpdateDrawMode(CCmdUI *cmd);
	void                OnEditSingleItem();
	void                OnColorGridBar();
//	void                OnUpdateColorGridBar(CCmdUI *cmd);
	void                OnColorIntensityBar();
//	void                OnUpdateColorIntensityBar(CCmdUI *cmd);
	void                OnUpdateControlBar(CCmdUI* pCmdUI);
	void                OnRprClassBar();
	void                OnEditMultiple();
	void                OnSize( UINT nType, int cx, int cy );
	virtual void        OnInitialUpdate();
	int	                OnCreate(LPCREATESTRUCT lpCreateStruct);
//	void                OnUpdateDescriptionBar(CCmdUI *cmd);
	void                OnEditSelectAll();
	void                OnUpdateEditItem(CCmdUI *cmd);
	void                OnNoOfCells();
	void                OnRefresh();

	MapRprStyleTab        tabs;
	ColorGridBar          csBar;
	ColorIntensityBar     ciBar;
	ButtonBar             m_bbTools;
	int                   iColWidth;

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version inRepresentationClassView.cpp
inline RepresentationClassDoc* RepresentationClassView::GetDocument()
   { return (RepresentationClassDoc*)m_pDocument; }
#endif

#endif REPRESENTATIONCLASSVIEW_H
