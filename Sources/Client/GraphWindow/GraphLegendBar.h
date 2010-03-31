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
// GraphLegendBar.h: interface for the GraphLegendBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHLEGENDBAR_H__4C4A2CB4_0979_11D5_B946_00A0C9D5342F__INCLUDED_)
#define AFX_GRAPHLEGENDBAR_H__4C4A2CB4_0979_11D5_B946_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class GraphDrawer;

class GraphLegendBar: public CMiniFrameWnd
{
public:
	GraphLegendBar();
	virtual ~GraphLegendBar();
	BOOL Create(CWnd* pParent, GraphDrawer*);
protected:
	GraphDrawer* gd;
	//{{AFX_MSG(GraphLegendBar)
	void OnPaint();
	void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_GRAPHLEGENDBAR_H__4C4A2CB4_0979_11D5_B946_00A0C9D5342F__INCLUDED_)
