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
// InPlaceEdit.cpp

#include "Headers\toolspch.h"
#include "Client\Editors\Utils\InPlaceEdit.h"

IMPLEMENT_DYNCREATE(InPlaceEdit, CEdit)

BEGIN_MESSAGE_MAP(InPlaceEdit, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_NCDESTROY()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

InPlaceEdit::InPlaceEdit(int iItem, int iSubItem, const CString& sOriginal)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_fESC = false;
	m_sOriginal = sOriginal;
}

int InPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText(m_sOriginal);
	AdjustSize();
	SetFocus();
	SetSel( 0, -1 );  // Select all text
	return 0;
}

void InPlaceEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();

	delete this;
}

BOOL InPlaceEdit::OnEraseBkgnd(CDC* pDC) 
{
	if (::IsWindowVisible(GetSafeHwnd() ) )
	{
		SetFocus();
		SetSel( 0, -1 );  // Select all text
		return TRUE;
	}

	return CEdit::OnEraseBkgnd(pDC);
}

void InPlaceEdit::OnSetFocus(CWnd* pNewWnd)
{
	SetFocus();
	SetSel( 0, -1 );  // Select all text
}

void InPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	CString str;
	if (m_fESC)
		str = m_sOriginal;
	else
		GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.lParam = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), 
					(LPARAM)&dispinfo );

	DestroyWindow();
}

void InPlaceEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if (nChar == VK_ESCAPE)
			m_fESC = true;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Fit the edit box close around the text to edit
// Allow a bit of space at the end
// If there is no text yet, the maximum size is set
void InPlaceEdit::AdjustSize()
{
	CString str;

	GetWindowText( str );
	if (str.GetLength() == 0)
		return;

	CWindowDC dc(this);
	CFont *pFont = GetParent()->GetFont();
	CFont *pFontDC = dc.SelectObject( pFont );
	CSize size = dc.GetTextExtent( str );
	dc.SelectObject( pFontDC );

	// Add some elbow room; this is also the minimum space
	// in case of an empty string
	size.cx += dc.GetTextExtent("XXX", 3).cx;

	// Get client rect
	CRect rect, parentrect;
	GetClientRect( &rect );
	GetParent()->GetClientRect( &parentrect );

	// Transform rect to parent coordinates
	ClientToScreen( &rect );
	GetParent()->ScreenToClient( &rect );

	// if possible resize the CEdit
	if ( size.cx + rect.left < rect.right )
		rect.right = rect.left + size.cx;

	MoveWindow( &rect );
}

BOOL InPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
// prevent that RETURN, ESC, TAB are eaten by IsDialogMessage() check
// but allow other accelerators to function
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
//	    case VK_TAB:
		  case VK_ESCAPE: 
			case VK_RETURN:
			case VK_DELETE:
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

