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
// colorIntensity.cpp: implementation of the colorIntensity class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Client\Editors\Utils\colorScroll.h"
#include "Client\Editors\Utils\colorIntensity.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\syscolor.h"

//---[ ColorIntensity ]------------------------------------------------------------------
BEGIN_MESSAGE_MAP(ColorIntensity, CWnd)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
//	ON_MESSAGE(ILWM_SETCOLOR, OnSetColor)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ColorIntensity::ColorIntensity() :
 fVertical(true)
{

}

ColorIntensity::~ColorIntensity()
{

}

BOOL ColorIntensity::OnEraseBkgnd(CDC* dc)
{
	CRect rect;
	GetClientRect(rect);

	CBrush br(GetSysColor(COLOR_BTNFACE));
    dc->FillRect(&rect, &br);

	return TRUE;
}

BOOL ColorIntensity::Create(CRect& rct, CWnd *par, COLORREF iC, UINT nID) 
{
	outerRect=rct;
	color = iC;
	CDC *cdc = par->GetWindowDC();
	ISTRUE(fINotEqual, cdc, (CDC *)NULL);

	CSize cz = cdc->GetTextExtent("gk");
	CWnd::Create(NULL, "ColorIntensity", WS_CHILD | WS_VISIBLE  , rct, par, nID);
	
	CFont *fnt=const_cast<CFont *> ( IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium));

	CRect scrollRect(2, cz.cy + 6, 20, rct.Height());
	cscbRedV.Create(WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_AUTOTICKS | TBS_LEFT , scrollRect, this, ID_COLORINTENSITY_REDV);
	cscbRedV.ShowWindow(fVertical ? SW_SHOW : SW_HIDE);

	cscbRedH.Create(WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS | TBS_TOP , scrollRect, this, ID_COLORINTENSITY_REDH);
	cscbRedH.ShowWindow(fVertical ? SW_HIDE : SW_SHOW);

	String s("255");
	cz = cdc->GetTextExtent(s.scVal());
	stRed.Create(s.scVal(), WS_VISIBLE | WS_CHILD, 
		           CRect(scrollRect.CenterPoint().x - cz.cx/2 + 1, 0, scrollRect.CenterPoint().x + cz.cx/2, cz.cy - 2 ), this);
	stRed.SetFont(fnt);
	stRed.MoveWindow(CRect(5, 5, 25, 25));

	scrollRect.OffsetRect(rct.Width()*0.4, 0);
	cscbGreenV.Create(WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_AUTOTICKS | TBS_LEFT, scrollRect, this, ID_COLORINTENSITY_GREENV);
	cscbGreenV.ShowWindow(fVertical ? SW_SHOW : SW_HIDE);

	cscbGreenH.Create(WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS | TBS_TOP , scrollRect, this, ID_COLORINTENSITY_GREENH);
	cscbGreenH.ShowWindow(fVertical ? SW_HIDE : SW_SHOW);

	cz = cdc->GetTextExtent(s.scVal());
	stGreen.Create(s.scVal(), WS_VISIBLE | WS_CHILD, 
		           CRect(scrollRect.CenterPoint().x - cz.cx/2, 0, scrollRect.CenterPoint().x + cz.cx/2, cz.cy -2 ), this);
	stGreen.SetFont(fnt);

	scrollRect.OffsetRect(rct.Width()*0.4, 0);
	cscbBlueV.Create(WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_AUTOTICKS| TBS_LEFT , scrollRect, this, ID_COLORINTENSITY_BLUEV);
	cscbBlueV.ShowWindow(fVertical ? SW_SHOW : SW_HIDE);

	cscbBlueH.Create(WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS | TBS_TOP , scrollRect, this, ID_COLORINTENSITY_BLUEH);
	cscbBlueH.ShowWindow(fVertical ? SW_HIDE : SW_SHOW);

	cz = cdc->GetTextExtent(s.scVal());
	stBlue.Create(s.scVal(), WS_VISIBLE | WS_CHILD, 
		           CRect(scrollRect.CenterPoint().x - cz.cx/2, 0, scrollRect.CenterPoint().x + cz.cx/2, cz.cy - 2 ), this);
	stBlue.SetFont(fnt);

	wndPostOffice = par;
	
	SetColor(iC);
	return TRUE;
}

void ColorIntensity::SetOrientation(bool _fV)
{
	fVertical = _fV;
	cscbRedH.ShowWindow(fVertical ? SW_HIDE : SW_SHOW);
	cscbRedV.ShowWindow(fVertical ? SW_SHOW : SW_HIDE);
	cscbBlueH.ShowWindow(fVertical ? SW_HIDE : SW_SHOW);
	cscbBlueV.ShowWindow(fVertical ? SW_SHOW : SW_HIDE);
	cscbGreenV.ShowWindow(fVertical ? SW_SHOW : SW_HIDE);
	cscbGreenH.ShowWindow(fVertical ? SW_HIDE : SW_SHOW);
}

void ColorIntensity::Resize(UINT nType, int cx, int cy)
{
	CDC *cdc = GetParent()->GetWindowDC();
	ISTRUE(fINotEqual, cdc, (CDC *)NULL);
	CSize cz = cdc->GetTextExtent("1000");

	CRect rct(0,0,cx, cy - 4);

	MoveWindow(rct);
	if ( fVertical)
	{
		CRect scrollRect(3, cz.cy - 2, rct.Width() * 0.33, rct.Height());	

		cscbRedV.MoveWindow(scrollRect);
		stRed.MoveWindow(CRect(scrollRect.CenterPoint().x - cz.cx/2 + 1, 5, scrollRect.CenterPoint().x + cz.cx/2, cz.cy - 2 ));
		scrollRect.OffsetRect(rct.Width()*0.33, 0);
		cscbGreenV.MoveWindow(scrollRect);
		scrollRect.OffsetRect(rct.Width()*0.33, 0);
		cscbBlueV.MoveWindow(scrollRect);
	}
	else
	{
		CRect scrollRect(cz.cx + 10, 3, rct.Width() - 5, rct.Height() * 0.33);	
		cscbRedH.MoveWindow(scrollRect);
		stRed.MoveWindow(CRect(0, 0, scrollRect.Width(), scrollRect.Height())); //scrollRect.CenterPoint().y - cz.cy/2 + 1, cz.cx + 2, scrollRect.CenterPoint().y + cz.cy/2));

		scrollRect.OffsetRect(0, rct.Height()*0.33);
		cscbGreenH.MoveWindow(scrollRect);
		scrollRect.OffsetRect(0, rct.Height()*0.33);
		cscbBlueH.MoveWindow(scrollRect);

	}
	cscbRedH.ShowWindow(fVertical ? SW_HIDE : SW_SHOW);
	cscbRedV.ShowWindow(fVertical ? SW_SHOW : SW_HIDE);
	cscbBlueH.ShowWindow(fVertical ? SW_HIDE : SW_SHOW);
	cscbBlueV.ShowWindow(fVertical ? SW_SHOW : SW_HIDE);
	cscbGreenV.ShowWindow(fVertical ? SW_SHOW : SW_HIDE);
	cscbGreenH.ShowWindow(fVertical ? SW_HIDE : SW_SHOW);

	SetColor(color);

	Invalidate();
}

void ColorIntensity::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pSB)
{
	CSliderCtrl *pScrollBar = (CSliderCtrl *)pSB;
	if (nSBCode == SB_LINEDOWN)
		nPos = min(255, pScrollBar->GetPos() + 1);
	if (nSBCode == SB_LINEUP )
		nPos = max( 0, pScrollBar->GetPos() - 1 );
  if ( nSBCode == SB_PAGEDOWN )
		nPos = min(255, pScrollBar->GetPos() + 50);
  if ( nSBCode == SB_PAGEUP )
		nPos = max(0, pScrollBar->GetPos() - 50);

	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION || 
		  nSBCode == SB_LINEDOWN   || nSBCode == SB_LINEUP ||
			nSBCode == SB_PAGEDOWN   || nSBCode == SB_PAGEUP )
	{
		switch(pScrollBar->GetDlgCtrlID())
		{
			case ID_COLORINTENSITY_REDV:
				color = Color(255 - nPos, GetGValue(color), GetBValue(color));
				break;
			case ID_COLORINTENSITY_GREENV:
				color = Color(GetRValue(color), 255 - nPos, GetBValue(color)); 
				break;
			case ID_COLORINTENSITY_BLUEV:
				color = Color(GetRValue(color), GetGValue(color), 255 - nPos); 
				break;
			case ID_COLORINTENSITY_REDH:
				color = Color(255 - nPos, GetGValue(color), GetBValue(color));
				break;
			case ID_COLORINTENSITY_GREENH:
				color = Color(GetRValue(color), 255 - nPos, GetBValue(color)); 
				break;
			case ID_COLORINTENSITY_BLUEH:
				color = Color(GetRValue(color), GetGValue(color), 255 - nPos); 
				break;
		}
		pScrollBar->SetPos(nPos);
		SetColor(color);
	}
}

void ColorIntensity::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pSB)
{
	CSliderCtrl *pScrollBar = (CSliderCtrl *)pSB;
	if (nSBCode == SB_LINEDOWN)
		nPos = min(255, pScrollBar->GetPos() + 1);
	if (nSBCode == SB_LINEUP )
		nPos = max( 0, pScrollBar->GetPos() - 1 );
  if ( nSBCode == SB_PAGEDOWN )
		nPos = min(255, pScrollBar->GetPos() + 50);
  if ( nSBCode == SB_PAGEUP )
		nPos = max(0, pScrollBar->GetPos() - 50);

	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION || 
		  nSBCode == SB_LINEDOWN   || nSBCode == SB_LINEUP ||
			nSBCode == SB_PAGEDOWN   || nSBCode == SB_PAGEUP )
	{
		switch(pScrollBar->GetDlgCtrlID())
		{
			case ID_COLORINTENSITY_REDH:
				color = Color(255 - nPos, GetGValue(color), GetBValue(color));
				break;
			case ID_COLORINTENSITY_GREENH:
				color = Color(GetRValue(color), 255 - nPos, GetBValue(color)); 
				break;
			case ID_COLORINTENSITY_BLUEH:
				color = Color(GetRValue(color), GetGValue(color), 255 - nPos); 
				break;
		}
		pScrollBar->SetPos(nPos);
		SetColor(color);
	}
}

/*
void ColorIntensity::OnSetColor(WPARAM wParm, LPARAM lPar)
{
	SetColor(wParm, false);
}
*/

void ColorIntensity::ChangeCtrlStyle( long lStyle, BOOL bSetBit)
{
	long    lStyleOld;
	CRect   rect;

	lStyleOld = GetWindowLong( cscbGreenV.GetSafeHwnd(), GWL_STYLE );
	if ( bSetBit )
		lStyleOld |= lStyle;
	else
		lStyleOld &= ~lStyle;
	SetWindowLong( cscbGreenV.GetSafeHwnd(), GWL_STYLE, lStyleOld );
	cscbGreenV.GetWindowRect(&rect);
	cscbGreenV.InvalidateRect(&rect);
}

void ColorIntensity::SetText(const CSliderCtrl& bar, const String& s, CStatic& text)
{
	CDC *dc = GetWindowDC();
	CRect rct, rct2;
	CSize cz = dc->GetTextExtent(s.scVal());

	text.GetWindowRect(&rct);
  ScreenToClient(&rct);
	InvalidateRect(rct);
	rct.InflateRect(32,0);

	bar.GetWindowRect(&rct);
	ScreenToClient(&rct);
	CRect rect;
	if ( fVertical)
		rect = CRect(rct.CenterPoint().x - cz.cx/2 + 1, 0, rct.CenterPoint().x + cz.cx/2, cz.cy - 2);
	else
		rect = CRect(rct.left - cz.cx - 10 , rct.CenterPoint().y - cz.cy/2 + 1, rct.left, rct.CenterPoint().y + cz.cy/2);
	text.MoveWindow(&rect);
	text.SetWindowText(s.scVal());
}

void ColorIntensity::SetColor(COLORREF col, bool fPostMessage)
{
	color=col;
	if ( fVertical )
	{
		cscbRedV.SetPos(255 - GetRValue(col));
		cscbGreenV.SetPos(255 - GetGValue(col));
		cscbBlueV.SetPos(255 - GetBValue(col));

		String s(" %d", GetRValue(col));
		SetText(cscbRedV, s, stRed);
		s = String(" %d", GetGValue(col));
		SetText(cscbGreenV, s, stGreen);
		s = String(" %d" , GetBValue(col));
		SetText(cscbBlueV, s, stBlue);
	}
	else
	{
		cscbRedH.SetPos(255 - GetRValue(col));
		cscbGreenH.SetPos(255 - GetGValue(col));
		cscbBlueH.SetPos(255 - GetBValue(col));

		String s(" %d", GetRValue(col));
		SetText(cscbRedH, s, stRed);
		s = String(" %d", GetGValue(col));
		SetText(cscbGreenH, s, stGreen);
		s = String(" %d" , GetBValue(col));
		SetText(cscbBlueH, s, stBlue);
	}

	if (fPostMessage)
		wndPostOffice->PostMessage(ILWM_SETCOLOR, (COLORREF) color, (LPARAM) TRUE);
}

void ColorIntensity::OnMouseMove( UINT nFlags, CPoint point )
{
	wndPostOffice->PostMessage(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

void ColorIntensity::SetPostOffice(CWnd *wnd)
{
	wndPostOffice = wnd;
}
