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
/* DigitizerInfoWindow
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   18 Feb 97    5:22 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Client\ilwis.h"
#include "Client\Editors\Digitizer\DIGINFO.H"
#include "Client\Editors\Digitizer\DIGREF.H"
#include "Client\Editors\Digitizer\DIGITIZR.H"

static const int iBut1ID = 500;

BEGIN_MESSAGE_MAP(DigitizerInfoWindow, CMiniFrameWnd)
	ON_BN_CLICKED(iBut1ID, ClickBut1) 
	ON_BN_CLICKED(iBut1ID + 1, ClickBut2)
	ON_BN_CLICKED(iBut1ID + 2, ClickBut3) 
	ON_BN_CLICKED(iBut1ID + 3, ClickBut4) 
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

DigitizerInfoWindow::DigitizerInfoWindow()
  : CMiniFrameWnd(),
  cLast(crdUNDEF)
{
}

int DigitizerInfoWindow::Create(CWnd *parent)
{
	CMiniFrameWnd::Create(NULL, "Digitizer", WS_VISIBLE | WS_CAPTION|WS_SYSMENU, CRect( 0,0,100,100), parent);
	CFont *fnt = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);

	SetFont(fnt);
	CDC *dc = GetDC();
	CSize sz = dc->GetTextExtent("gk");
	int iXSize = sz.cx * 10;
	int iYSize = sz.cy * 8;
	CRect rct(0,0, iXSize, iYSize);
	this->ClientToScreen(&rct);
	MoveWindow(rct);
  int i;
  //backgroundColor(SysColor(COLOR_WINDOW));
  stTitle = new CStatic;
	stTitle->Create("", WS_CHILD | WS_VISIBLE | SS_LEFT, CRect(10, 3, 150, sz.cy), this);
	stTitle->SetFont(fnt);
  for (i = 0; i < 4; ++i) 
	{
    String s("%d", i);	// should use GetPrivateString
    st[i] = new CStatic;
		sz = dc->GetTextExtent(s.c_str());
		st[i]->Create(s.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, CRect( 10, 10 + (i + 1)* sz.cy, 10 + sz.cx * sz.cx, 10 + (i + 2) * sz.cy), this );
		st[i]->SetFont(fnt);
    but[i] = new CButton;
		but[i]->Create("", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON , CRect(30, 10 + (i + 1) * sz.cy, iXSize, 10 + (i + 2) * sz.cy), this, iBut1ID + i);
		but[i]->SetFont(fnt);
  }

	return 1;
}

DigitizerInfoWindow::~DigitizerInfoWindow()
{
  int i;
	if (stTitle)
  {
		delete stTitle;
		stTitle = 0;
		for (i = 0; i < 4; ++i) 
		{
			delete but[i];
			delete st[i];
		}
  }
}

BOOL DigitizerInfoWindow::OnEraseBkgnd( CDC* pDC )
{
	CBrush brush(GetSysColor(COLOR_3DFACE));
	CRect rct;
	GetClientRect(&rct);
	pDC->FillRect(rct, &brush);

	return TRUE;
}

CButton* DigitizerInfoWindow::button(int i)  //  i = 1,2,3,4
{
  if (i > 0 && i <= 4)
    return but[i-1];
  else
    return 0;  
}

void DigitizerInfoWindow::SetCoord(const CoordWithCoordSystem& c)
{
  if (c.c().x != cLast.c().x || c.c().y != cLast.c().y) 
	{
    String sNew("%10.1f %10.1f", c.c().x, c.c().y);
    stTitle->SetWindowText(sNew.c_str());
    cLast = c;
  }
}

void DigitizerInfoWindow::ClickButton(int iBut)
{
	FormUsingDigitizer* mrf = IlwWinApp()->dig()->mrf;
	if (mrf) {
		mrf->ProcessInfo(cLast.c().x, cLast.c().y, -1);
		mrf->ProcessInfo(cLast.c().x, cLast.c().y, iBut);
	}
	else		
		IlwWinApp()->SendUpdateCoordMessages(CoordMessage(100+iBut),&cLast);
}


void DigitizerInfoWindow::ClickBut1()
{
  ClickButton(0);
}

void DigitizerInfoWindow::ClickBut2()
{
  ClickButton(1);
}

void DigitizerInfoWindow::ClickBut3()
{
  ClickButton(2);
}

void DigitizerInfoWindow::ClickBut4()
{
  ClickButton(3);
}


CStatic* DigitizerInfoWindow::title()
{
  return stTitle;
}

void DigitizerInfoWindow::OnClose()
{
	ShowWindow(SW_HIDE);
}

