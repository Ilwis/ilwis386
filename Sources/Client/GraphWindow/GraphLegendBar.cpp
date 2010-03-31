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
// GraphLegendBar.cpp: implementation of the GraphLegendBar class.
//
//////////////////////////////////////////////////////////////////////


#include "Client\Headers\formelementspch.h"
#include "Client\GraphWindow\GraphLegendBar.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\FormElements\syscolor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( GraphLegendBar, CMiniFrameWnd )
	//{{AFX_MSG_MAP( GraphLegendBar )
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



GraphLegendBar::GraphLegendBar()
: gd(0)
{
}

GraphLegendBar::~GraphLegendBar()
{
}

BOOL GraphLegendBar::Create(CWnd* pParent, GraphDrawer* dr)
{
	gd = dr;
	CRect rect(10,10,200,100);
	return CMiniFrameWnd::Create(NULL, "Legend", WS_POPUP|WS_CAPTION|WS_THICKFRAME, rect, pParent);
}

void GraphLegendBar::OnPaint()
{
	CPaintDC dc(this);

  zRect rect;
	GetClientRect(rect);
  Color clrText = SysColor(COLOR_WINDOWTEXT);
	Color clrBack = SysColor(COLOR_WINDOW);
	CBrush br(clrBack);
	CPen penClr(PS_SOLID,1,clrText);
	CPen* penOld = dc.SelectObject(&penClr);
	CBrush* brOld = dc.SelectObject(&br);
	dc.Rectangle(rect);

  dc.SetTextColor(clrText);  
	dc.SetBkMode(TRANSPARENT);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
 
	int iHeight = tm.tmHeight;
	int iX = iHeight;
	int iY = -iHeight / 2;
	int iCount = gd->iLayers();

  for (int i=0; i < iCount; ++i)
  {
    if (!gd->agl[i]->fShow)
      continue;
    iY += iHeight;
		CPen pen(PS_SOLID, 3, gd->agl[i]->color);
		dc.SelectObject(&pen);
    dc.MoveTo(0.5 * iX, iY + iHeight / 2);
    dc.LineTo(1.5 * iX, iY + iHeight / 2);
		String sName = gd->agl[i]->sName();
		dc.TextOut(2 * iX, iY, sName.scVal());

		dc.SelectObject(&penClr);
	}

	dc.SelectObject(penOld);
	dc.SelectObject(brOld);
}

void GraphLegendBar::OnSize(UINT nType, int cx, int cy)
{
	Invalidate();
}
