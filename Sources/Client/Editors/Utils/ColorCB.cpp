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
#include "Headers\toolspch.h"
#include "Client\Editors\Utils\ColorCB.h"


BEGIN_MESSAGE_MAP(ColorCB, CComboBox)
END_MESSAGE_MAP()

ColorCB::ColorCB()
{
	// Add the colors to the array
	 
	colors.push_back( RGB(0, 0, 0));
	colors.push_back( RGB( 84, 84, 84)),
	colors.push_back( RGB( 128,128,128)),
	colors.push_back( RGB( 168,168,168)),
	colors.push_back( RGB( 255,255,255)),
	colors.push_back( RGB( 255,  0,  0)),
	colors.push_back( RGB(   0,255,  0)),
	colors.push_back( RGB(   0,  0,255)),
	colors.push_back( RGB(   0,255,255)),
	colors.push_back( RGB( 255,  0,255)),
	colors.push_back( RGB( 255,255,  0)),
	colors.push_back( RGB( 255,136,  0)),
	colors.push_back( RGB( 255, 68,  0)),
	colors.push_back( RGB( 216,168,  0)),
	colors.push_back( RGB( 164, 40, 40)),
	colors.push_back( RGB( 244,164, 96)),
	colors.push_back( RGB( 140, 68, 20)),
	colors.push_back( RGB( 255, 20,148)),
	colors.push_back( RGB( 160, 32,240)),
	colors.push_back( RGB( 124,252,  0)),
	colors.push_back( RGB(  48,216, 56)),
	colors.push_back( RGB(  48,128, 76)),
	colors.push_back( RGB( 116,222,120)),
	colors.push_back( RGB( 108,142, 36)),
	colors.push_back( RGB(   0,176, 20)),
	colors.push_back( RGB(   0, 86, 44)),
	colors.push_back( RGB(  84,148,132)),
	colors.push_back( RGB(  24,204,224)),
	colors.push_back( RGB(  30,144,255)),
	colors.push_back( RGB(  64,104,225)),
	colors.push_back( RGB( 116,160,255)),
	colors.push_back( RGB(  48, 48,100)),
	colors.push_back( RGB( 125, 125, 125));
}

ColorCB::~ColorCB()
{
}


BOOL ColorCB::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= CBS_OWNERDRAWFIXED | CBS_DROPDOWNLIST | WS_VSCROLL ;
	BOOL iRet = CComboBox::Create(dwStyle, rect, pParentWnd, nID);

	for (int nColors = 0; nColors < colors.size(); nColors++)
		AddString("");

	return iRet;
}

void ColorCB::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
		
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CRect rect(&(lpDrawItemStruct->rcItem));
	
	CBrush brush(colors[lpDrawItemStruct->itemID]);
	rect.InflateRect(-1, -1	);
	dc.FillRect(rect, &brush);

	CBrush frameBrush(RGB(0, 0, 0));
	dc.FrameRect(rect, &frameBrush);

	if ( lpDrawItemStruct->itemID == colors.size() - 1)
	{
		CPen pen(PS_SOLID, 1, RGB(125,125,125));
		CFont fnt;
		fnt.CreateFont(rect.Height(), 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                  VARIABLE_PITCH | FF_DONTCARE,"MS Sans Serif");
		CPen *op = dc.SelectObject(&pen);
		CFont *of = dc.SelectObject(&fnt);
		int iOldMode = dc.SetBkMode(TRANSPARENT);
		dc.TextOut(rect.left, rect.top, CString("Custom"));
		dc.SelectObject(op);
		dc.SelectObject(of);
		dc.SetBkMode(iOldMode);
	}

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		dc.DrawFocusRect(rect);	
	dc.Detach();
	
}

void ColorCB::MeasureItem(LPMEASUREITEMSTRUCT mi)
{
	CDC *dc = GetWindowDC();
	CSize sz = dc->GetTextExtent("gk");
	mi->itemHeight=sz.cy;
}

COLORREF ColorCB::GetColor()
{
	int iSel = GetCurSel();
	if (iSel != -1)
		return colors[iSel];

	return -1;
}

void ColorCB::CustomColor()
{
	CColorDialog frmCS(colors[colors.size() - 1], CC_ANYCOLOR, this) ;
  if (frmCS.DoModal()==IDOK) 
  {
    COLORREF c = frmCS.GetColor();
		colors[colors.size() - 1 ] = c;
	}
}
