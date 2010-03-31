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
// FlatButton.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Utils\FlatButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FlatZappButton

FlatZappButton::FlatZappButton(FormEntry *f, CWnd *w, 
							   const CRect& siz, DWORD style, const char* sQuest, int id)
   : ZappButton(f, w, siz, style, sQuest, id)
{
	m_MouseOnButton = FALSE;

	m_cxIcon = 0;
	m_cyIcon = 0;
} // End of FlatZappButton


FlatZappButton::~FlatZappButton()
{
} // End of ~FlatZappButton


BEGIN_MESSAGE_MAP(FlatZappButton, ZappButton)
    //{{AFX_MSG_MAP(FlatZappButton)
	ON_WM_CAPTURECHANGED()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void FlatZappButton::SetIcon(HICON hIcon)
{
	ZappButton::SetIcon(hIcon);
  
	ICONINFO ii;

	// Get icon dimension
	ZeroMemory(&ii, sizeof(ICONINFO));
	::GetIconInfo(hIcon, &ii);

	m_cxIcon = (BYTE)(ii.xHotspot * 2);
	m_cyIcon = (BYTE)(ii.yHotspot * 2);

	RedrawWindow();
} // End of SetIcon

void FlatZappButton::SetCaption(String sCaption)
{
	// This loop is necessary to disallow windows to draw an accelerator
	// and instead just display the '&'. The DrawText DT_NOPREFIX style added appears to have no effect
	for (int i = 0; i < sCaption.length(); i++)
	{
		if (sCaption[i] == '&')
			m_sCaption += '&';
		m_sCaption += sCaption[i];
	}
}

void FlatZappButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd* pWnd;    // Active window
	CWnd* pParent; // Window containing the button

	ZappButton::OnMouseMove(nFlags, point);

	// If the mouse enter the button with the left button pressed
	// then do nothing
	if (nFlags & MK_LBUTTON && !m_MouseOnButton) return;

	pWnd = GetActiveWindow();
	pParent = GetOwner();

	if ((GetCapture() != this) && 
		(pWnd != NULL && pParent != NULL)) 
	{
		m_MouseOnButton = TRUE;
		SetCapture();
		Invalidate();
	}
	else
	{
		POINT p2 = point;
		ClientToScreen(&p2);
		CWnd* wndUnderMouse = WindowFromPoint(p2);
		if (wndUnderMouse != this)
		{
			// Redraw only if mouse goes out
			if (m_MouseOnButton)
			{
				m_MouseOnButton = FALSE;
				Invalidate();
			}
			// If user is NOT pressing left button then release capture!
			if (!(nFlags & MK_LBUTTON)) ReleaseCapture();
		}
	}
} // End of OnMouseMove


void FlatZappButton::OnKillFocus(CWnd * pNewWnd)
{
	ZappButton::OnKillFocus(pNewWnd);

	if (m_MouseOnButton)
	{
		m_MouseOnButton = FALSE;
		Invalidate();
	}
} // End of OnKillFocus


void FlatZappButton::OnCaptureChanged(CWnd *pWnd) 
{
	if (m_MouseOnButton)
	{
		if (GetCapture() != 0)
			ReleaseCapture();
		Invalidate();
	}

	// Call base message handler
	ZappButton::OnCaptureChanged(pWnd);
} // End of OnCaptureChanged


void FlatZappButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CPen *pOldPen;
	BOOL bIsPressed  = (lpDIS->itemState & ODS_SELECTED);
	BOOL bIsFocused  = (lpDIS->itemState & ODS_FOCUS);
	BOOL bIsDisabled = (lpDIS->itemState & ODS_DISABLED);

	CRect itemRect = lpDIS->rcItem;

	// Prepare draw... paint button's area with background color
	CBrush br(::GetSysColor(COLOR_BTNFACE));
	pDC->FillRect(&itemRect, &br);

	// Draw pressed button
	if (bIsPressed)
	{
		CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // Bianco
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Grigio scuro

		// Line at top and at left borders: Dark gray line
		pOldPen = pDC->SelectObject(&penBtnShadow);
		pDC->MoveTo(itemRect.left, itemRect.bottom-1);
		pDC->LineTo(itemRect.left, itemRect.top);
		pDC->LineTo(itemRect.right, itemRect.top);

		// Line at bottom and right: White line
		pDC->SelectObject(pOldPen);
		pOldPen = pDC->SelectObject(&penBtnHiLight);
		pDC->MoveTo(itemRect.left, itemRect.bottom-1);
		pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
		pDC->LineTo(itemRect.right-1, itemRect.top-1);
		
		pDC->SelectObject(pOldPen);
	}
	else // ...else draw non pressed button
	{
		CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White
		CPen pen3DLight(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));       // Light gray
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		CPen pen3DDKShadow(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black

		if (m_MouseOnButton)
		{
			// Line at top and at left borders: White line
			pOldPen = pDC->SelectObject(&penBtnHiLight);
			pDC->MoveTo(itemRect.left, itemRect.bottom-1);
			pDC->LineTo(itemRect.left, itemRect.top);
			pDC->LineTo(itemRect.right, itemRect.top);

			// Line at bottom and right: Dark gray line
			pDC->SelectObject(pOldPen);
			pOldPen = pDC->SelectObject(&penBtnShadow);
			pDC->MoveTo(itemRect.left, itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1, itemRect.top-1);
			
			pDC->SelectObject(pOldPen);
		}
	}

	CRect captionRect = lpDIS->rcItem;

	// Draw the icon
	if (GetIcon() != NULL)
	{
		DrawTheIcon(pDC, &lpDIS->rcItem, &captionRect, bIsPressed, bIsDisabled);
	}

	// Write the button title (if any)
	if (!m_sCaption.IsEmpty())
	{
		if (bIsPressed)
			captionRect.OffsetRect(1, 1);
    
		pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));

		// Center text
		CRect centerRect = captionRect;

		pDC->DrawText(m_sCaption, captionRect, DT_SINGLELINE | DT_CALCRECT/* | DT_NOPREFIX*/);
		captionRect.OffsetRect((centerRect.Width() - captionRect.Width())/2, (centerRect.Height() - captionRect.Height())/2);

		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawState(captionRect.TopLeft(), captionRect.Size(), (LPCTSTR)m_sCaption, (bIsDisabled ? DSS_DISABLED : DSS_NORMAL), 
					   TRUE, 0, (CBrush*)NULL);
	}
} // End of DrawItem


void FlatZappButton::DrawTheIcon(CDC* pDC, RECT* rcItem, CRect* captionRect, BOOL IsPressed, BOOL IsDisabled)
{
	CRect iconRect = rcItem;

	if (m_sCaption.IsEmpty())
	{
		// Center the icon horizontally
		iconRect.left += ((iconRect.Width() - m_cxIcon)/2);
	}
	else
	{
		// L'icona deve vedersi subito dentro il focus rect
		iconRect.left += 5;
		captionRect->left += m_cxIcon + 5;
	}
	// Center the icon vertically
	iconRect.top += ((iconRect.Height() - m_cyIcon)/2);
    
	// If button is pressed then press the icon also
	if (IsPressed) iconRect.OffsetRect(1, 1);
	// Ole'!
	pDC->DrawState(iconRect.TopLeft(), 
	               iconRect.Size(), 
	               GetIcon(), 
	               (IsDisabled ? DSS_DISABLED : DSS_NORMAL), 
	               (CBrush*)NULL);
} // End of DrawTheIcon


void FlatZappButton::PreSubclassWindow() 
{
	SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW);

	ZappButton::PreSubclassWindow();
} // End of PreSubclassWindow


LRESULT FlatZappButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_LBUTTONDBLCLK)
	{
		message = WM_LBUTTONDOWN;
	}
	return ZappButton::DefWindowProc(message, wParam, lParam);
} // End of DefWindowProc


void FlatZappButton::PaintBk(CDC * pDC)
{
	CClientDC clDC(GetParent());
	CRect rect;
	CRect rect1;

	GetClientRect(rect);

	GetWindowRect(rect1);
	GetParent()->ScreenToClient(rect1);

	CDC dcBk;
	CBitmap bmpBk, *pbmpOldBk = 0;
	if (dcBk.m_hDC == NULL)
	{
		dcBk.CreateCompatibleDC(&clDC);
		bmpBk.CreateCompatibleBitmap(&clDC, rect.Width(), rect.Height());
		pbmpOldBk = dcBk.SelectObject(&bmpBk);
		dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), &clDC, rect1.left, rect1.top, SRCCOPY);
	}

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dcBk, 0, 0, SRCCOPY);
  if (0 != pbmpOldBk)
    dcBk.SelectObject(pbmpOldBk);
} // End of PaintBk
