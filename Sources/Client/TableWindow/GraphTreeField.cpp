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
// GraphTreeField.cpp: implementation of the GraphTreeField class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\TableWindow\GraphTreeField.h"

BEGIN_MESSAGE_MAP(GraphTreeField, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
  ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


GraphTreeField::GraphTreeField(CWnd* wnd, CRect rect, String str, GraphTreeItem* grti, NotifyFieldFinished nfldfin)
: gti(grti), nff(nfldfin)
{
  Create(WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|WS_VISIBLE, rect, wnd, 100);
  SetWindowText(str.c_str());
  SetFocus();
}

GraphTreeField::~GraphTreeField()
{
}

BOOL GraphTreeField::PreTranslateMessage(MSG* pMsg)
{
// prevent that RETURN, ESC, TAB are eaten by IsDialogMessage() check
// but allow other accelerators to function
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
	    case VK_TAB:
		  case VK_ESCAPE: 
			case VK_RETURN:
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

void GraphTreeField::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) 
	{
    case VK_RETURN:
      (gti->*nff)();
      return;
    case VK_ESCAPE: 
      SetModify(FALSE);
      (gti->*nff)();
      return;
    default:
      CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
  }
};

void GraphTreeField::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) 
	{
    case VK_TAB:
    case VK_ESCAPE: 
    case VK_RETURN:
			break;
		default:
			CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
};

void GraphTreeField::OnKillFocus(CWnd *)
{
  SetModify(FALSE);
  (gti->*nff)();
}

