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
// PanTool.cpp: implementation of the PanTool class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Client\Mapwindow\PanTool.h"
#include "Client\Mapwindow\ZoomableView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PanTool::PanTool(ZoomableView* zv, CCmdTarget* target, NotifyMoveProc proc, CRect rct)
	: MapPaneViewTool(zv)
	, cmt(target)
	, np(proc)
	, rect(rct)
	, fDown(false)
{
	SetCursor(zCursor("HandCursor"));

	CRect rectView;
	zv->GetClientRect(&rectView);
	if (rect.top < rectView.top) 
		rect.top = rectView.top;
	if (rect.bottom > rectView.bottom) 
		rect.bottom = rectView.bottom;
	if (rect.left < rectView.left)
		rect.left = rectView.left;
	if (rect.right > rectView.right)
		rect.right = rectView.right;

	int iWidth = rect.Width();
	int iHeight = rect.Height();
	CClientDC cdc(zv);
	CBitmap bm;
	bm.CreateCompatibleBitmap(&cdc,iWidth,iHeight);
	dcMove.CreateCompatibleDC(&cdc);
	dcMove.SelectObject(&bm);
	dcMove.BitBlt(0,0,iWidth,iHeight,&cdc,rect.left,rect.top,SRCCOPY);

	bm.Detach();
	rectBG = rect;
}

PanTool::~PanTool()
{
}

void PanTool::DrawMove()
{
	CClientDC cdc(mpv);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rect);
	cdc.SelectClipRgn(&rgn);
	CGdiObject* penOld = cdc.SelectStockObject(WHITE_PEN);
	CGdiObject* brOld = cdc.SelectStockObject(WHITE_BRUSH);
	cdc.Rectangle(&rect);
	rectBG = rect;
	rectBG += pMove;
//	dcBG.BitBlt(0,0,rectBG.Width(),rectBG.Height(),&cdc,rectBG.left,rectBG.top,SRCCOPY);
	cdc.BitBlt(rectBG.left,rectBG.top,rectBG.Width(),rectBG.Height(),&dcMove,0,0,SRCCOPY);
	cdc.SelectObject(penOld);
	cdc.SelectObject(brOld);
	cdc.SelectClipRgn(0);
}

void PanTool::OnMouseMove(UINT nFlags, CPoint point)
{
	if (fDown) {
		pMove = point;
		pMove -= pStart;
		DrawMove();
	}
}

void PanTool::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!rect.PtInRect(point)) {
		MessageBeep(-1);
		Stop();
		return;
	}
	pStart = point;
	pMove = CPoint(0,0);
	rectBG = rect;
	CClientDC cdc(mpv);

	dcMove.BitBlt(0,0,rectBG.Width(),rectBG.Height(),&cdc,rectBG.left,rectBG.top,SRCCOPY);
	fDown = true;	
	mpv->SetCapture();
}

void PanTool::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (fDown) {
		OnMouseMove(nFlags, point);
		fDown = false;
		(cmt->*np)(pMove);
		ReleaseCapture();
	}
}

