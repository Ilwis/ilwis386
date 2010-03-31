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
// DragRectTool.cpp: implementation of the DragRectTool class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\DragRectTool.h"
#include "Client\Mapwindow\ZoomableView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DragRectTool::DragRectTool(ZoomableView* zv, CCmdTarget* target, NotifyRectProc proc, CRect rct)
	: MapPaneViewTool(zv)
	, cmt(target)
	, np(proc)
	, rect(rct)
	, fDown(false)
{
} 

DragRectTool::~DragRectTool()
{
}

void DragRectTool::DrawMove()
{
	CRect rct = rect;
	rct += pMove;
	CClientDC cdc(mpv);
	CGdiObject* penOld = cdc.SelectStockObject(WHITE_PEN);
	CGdiObject* brOld = cdc.SelectStockObject(NULL_BRUSH);
	cdc.SetROP2(R2_XORPEN);
	cdc.Rectangle(&rct);
	cdc.SelectObject(penOld);
	cdc.SelectObject(brOld);
	cdc.SetROP2(R2_COPYPEN);
}

void DragRectTool::OnMouseMove(UINT nFlags, CPoint point)
{
	if (fDown) {
    DrawMove();
		pMove = point;
		pMove -= pStart;
		DrawMove();
	}
}

void DragRectTool::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!rect.PtInRect(point)) {
		MessageBeep(-1);
		Stop();
		return;
	}
	pStart = point;
	pMove = CPoint(0,0);
	fDown = true;	
  DrawMove();
	mpv->SetCapture();
}

void DragRectTool::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (fDown) {
		OnMouseMove(nFlags, point);
		fDown = false;
    DrawMove();
    CRect rct = rect;
    rct += pMove;
		(cmt->*np)(rct);
		ReleaseCapture();
	}
}



