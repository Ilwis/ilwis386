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
// InfoLine.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\InfoLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// InfoLine

InfoLine::InfoLine(CWnd* wnd)
: wParent(wnd)
{
	CRect rect(0,0,0,0);
	CreateEx(0, "InfoLine", "", WS_POPUP|WS_BORDER|SS_CENTER, rect, wnd, 0);
}

InfoLine::~InfoLine()
{
}


BEGIN_MESSAGE_MAP(InfoLine, CWnd)
	//{{AFX_MSG_MAP(InfoLine)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// InfoLine message handlers

BOOL InfoLine::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.lpszClass = "InfoLine";
	cs.dwExStyle = 0;
	cs.style = WS_POPUP|WS_BORDER|SS_CENTER;
	return CWnd::PreCreateWindow(cs);
}

void InfoLine::text(zPoint pnt, const String &s)
{
	CClientDC cdc(this);
  zRect rect, rectOld;
//	GetParent()->GetWindowRect(&rect);
//rect.left() += pnt.x + 2;
//rect.bottom() = rect.top() + pnt.y - 2;
	CString str = s.c_str();
	CSize siz = cdc.GetTextExtent(str);
  rect.left() = pnt.x + 2;
  rect.bottom() = pnt.y - 2;
  rect.top()   = rect.bottom() - siz.cy - 2;
  rect.right() = rect.left() + siz.cx + 4;
	wParent->ClientToScreen(&rect);
	GetWindowRect(&rectOld);
  if (rect != rectOld)
    MoveWindow(rect);
  if (sText != s) {
    sText = s;
	if (sText != "")
		SetWindowText(s.c_str());
	else
		ShowWindow(SW_HIDE);
  }
  if (sText != "") {
	  ShowWindow(SW_SHOWNA);
	  UpdateWindow();
  }
}
