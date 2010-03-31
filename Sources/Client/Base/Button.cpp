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
#define BUTTON_CPP

#include "Client\Base\ZappToMFC.h"
#include "Client\FormElements\feposit.h"
#include "client\formelements\fentbase.h"

BEGIN_MESSAGE_MAP(ZappButton, CButton)
  ON_WM_KILLFOCUS()
  ON_WM_SETFOCUS()
  ON_CONTROL_REFLECT(BN_CLICKED, OnButtonClicked)
  ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

ZappButton::ZappButton(FormEntry *f, CWnd *w, const CRect& siz, DWORD style, const char* sQuest, int id) :
		          CButton(),
							BaseZapp(f)
{
	Create(sQuest, style, *siz, w, id);
}

ZappButton::~ZappButton()
{
	DestroyWindow();
}

afx_msg void ZappButton::OnKillFocus( CWnd* pNewWnd )
{
  CButton::OnKillFocus(pNewWnd);
//  fProcess(Event(WM_KILLFOCUS));
}

afx_msg void ZappButton::OnSetFocus( CWnd* pNewWnd )
{
  CButton::OnSetFocus(pNewWnd);
//  fProcess(Event(WM_SETFOCUS));
}

afx_msg void ZappButton::OnButtonClicked()
{
  fProcess(ButtonClickEvent(GetDlgCtrlID(), m_hWnd));
}

afx_msg void ZappButton::OnRButtonUp(UINT nFlags, CPoint point)
{
	fProcess(MouseRBUpEvent(nFlags, point));
}

afx_msg void ZappButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	if (_fe) {      
		DrawItemEvent ev(0, lpDrawItemStruct);
		_fe->DrawItem(&ev);
	}
}
