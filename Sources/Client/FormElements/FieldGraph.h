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
// FieldGraph.h: interface for the FieldGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIELDGRAPH_H__E62D8985_1D8F_452C_815D_E3D491C4EFC0__INCLUDED_)
#define AFX_FIELDGRAPH_H__E62D8985_1D8F_452C_815D_E3D491C4EFC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client\FormElements\fentbase.h"

#include "Client\FormElements\SimpleGraphWindow.h"

class SimpleFunction;

class SimpleGraphWindowWrapper : public SimpleGraphWindow, public BaseZapp
{
public:
	SimpleGraphWindowWrapper(FormEntry *f);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP();
};

class _export FieldGraph : public FormEntry  
{
public:
	FieldGraph(FormEntry* parent);
	virtual ~FieldGraph();
	void create();             // overriden
	void show(int sw);         // overriden
	void StoreData();
	void Enable();
	void Disable();
	void SetFunction(SimpleFunction * funPtr);
	void Replot();
	void SetBorderThickness(int l, int t, int r, int b);

private:
	SimpleGraphWindow *sgw;
};

#endif // !defined(AFX_FIELDGRAPH_H__E62D8985_1D8F_452C_815D_E3D491C4EFC0__INCLUDED_)
