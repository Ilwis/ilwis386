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
// BaseBar.cpp: implementation of the BaseBar class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Client\Editors\Utils\BaseBar.h"


BEGIN_MESSAGE_MAP( BaseBar, CControlBar )
	//{{AFX_MSG_MAP( BaseBar )
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BaseBar::BaseBar()
: wndParent(0)
{
	iHeight = 10;
	m_dwStyle = CBRS_ALIGN_TOP | CBRS_GRIPPER | CBRS_SIZE_DYNAMIC;
}

BaseBar::~BaseBar()
{
}

BOOL BaseBar::Create(CFrameWnd* pParent, UINT nID)
{
	ASSERT_VALID(pParent);	// must have a parent
	wndParent = pParent;
	SetBorders(2,0,2,0);

	DWORD dwStyle = WS_CHILD|WS_VISIBLE;
	// create the HWND
	CRect rect; 
	rect.SetRectEmpty();
	if (!CWnd::Create("IlwisView", NULL, dwStyle, rect, pParent, nID))
		return FALSE;

	wndParent->GetClientRect(&rect);
	iLastWidth = rect.Width() + 4;

	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
	SetBarStyle(GetBarStyle() | CBRS_BORDER_3D);

	return TRUE;
}

CSize BaseBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CSize sizeResult;
	sizeResult.cx = iLastWidth;
	sizeResult.cy = iHeight;
	if (IsFloating()) {
		if (dwMode & (LM_MRUWIDTH | LM_LENGTHY))
			sizeResult.cx = iLastWidth;
		else {
			if (nLength > 0)
				sizeResult.cx = nLength;
			if (dwMode & LM_COMMIT)
				iLastWidth = sizeResult.cx;
		}
	}
	else {
		CRect rct, rctParent, rctSelf;
		wndParent->GetClientRect(&rct);
		wndParent->GetWindowRect(&rctParent);
		GetWindowRect(rctSelf);
		sizeResult.cx = rct.Width() + 4 + (rctParent.left - rctSelf.left);
		iLastWidth = sizeResult.cx;
	}

	return sizeResult;
}

void BaseBar::OnPaint()
{
	Default();
}

void BaseBar::OnNcPaint() 
{
	EraseNonClient();
}
				 
LRESULT BaseBar::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

BOOL BaseBar::OnEraseBkgnd(CDC* pDC) 
{
	return (BOOL)Default();
}

void BaseBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// calculate border space (will add to top/bottom, subtract from right/bottom)
	CRect rect; rect.SetRectEmpty();
	BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
	CControlBar::CalcInsideRect(rect, bHorz);

	// adjust non-client area for border space
	lpncsp->rgrc[0].left += rect.left;
	lpncsp->rgrc[0].top += rect.top;
	lpncsp->rgrc[0].right += rect.right;
	lpncsp->rgrc[0].bottom += rect.bottom;
}

void BaseBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
}
