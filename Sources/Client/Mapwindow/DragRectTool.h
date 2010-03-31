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
// DragRectTool.h: interface for the DragRectTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAGRECTTOOL_H__BCA0EC23_2C3C_478C_9F2E_2D9C42633FB6__INCLUDED_)
#define AFX_DRAGRECTTOOL_H__BCA0EC23_2C3C_478C_9F2E_2D9C42633FB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_MAPPANEVIEWTOOL_H__BB72FBCA_4BCE_11D3_B7BC_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\MapPaneViewTool.h"
#endif


typedef void (CCmdTarget::*NotifyRectProc)(CRect);


class DragRectTool: public MapPaneViewTool   
{
public:
	DragRectTool(ZoomableView*, CCmdTarget*, NotifyRectProc, CRect); 
	virtual ~DragRectTool();

	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	void DrawMove();
	CCmdTarget* cmt;
	NotifyRectProc np;
	CRect rect, rectBG;
	CPoint pStart, pMove;
	bool fDown;
	CDC dcMove;
};

#endif // !defined(AFX_DRAGRECTTOOL_H__BCA0EC23_2C3C_478C_9F2E_2D9C42633FB6__INCLUDED_)
