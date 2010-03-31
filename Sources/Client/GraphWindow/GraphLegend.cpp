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
// GraphLegend.cpp: implementation of the GraphLegend class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\GraphWindow\GraphLegend.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\FormElements\syscolor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GraphLegend::GraphLegend(GraphDrawer* gd)
: grdr(gd), fShow(true)
{
  rect = CRect(100,100,100,100);
}

GraphLegend::~GraphLegend()
{
}

void GraphLegend::draw(CDC* cdc) 
{
  if (!fShow)
    return;
  
	CPen pen(PS_SOLID,0,Color(0,0,0));
	Color clrWhite = SysColor(COLOR_WINDOW);
  CBrush brWhite(clrWhite);
	CPen* penOld = cdc->SelectObject(&pen);
  CBrush* brOld = cdc->SelectObject(&brWhite);
	cdc->Rectangle(&rect);
	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
  
  const int iH = grdr->gvw->iCharHeight + 4;
  const int iW = 3 * iH;

  rect.right = rect.left;
  rect.bottom = rect.top;
	CFont* fntOld =	cdc->SelectObject(grdr->gvw->fnt);
  int i = 0;
  for (int iLayer = grdr->iLayers()-1; iLayer >= 0; --iLayer)
  {
    GraphLayer* gl = grdr->agl[iLayer];
    if (!gl->fShow)
      continue;
    CPoint ptLayer(rect.left + 5, rect.top + 5 + i * iH);
    ++i;
    enum { eLINE, eSIMPLELINE, ePOINT, eBLOCK } eLegType;
    CartesianGraphLayer* cgl = dynamic_cast<CartesianGraphLayer*>(gl);
    if (0 != cgl)
      switch (cgl->cgt)
      {
        case cgtContineous:
          eLegType = eLINE;
          break;
        case cgtStep:
        case cgtNeedle:
          eLegType = eSIMPLELINE;
          break;
        case cgtBar:
          eLegType = eBLOCK;
          break;
        case cgtPoint:
          eLegType = ePOINT;
          break;
      }
    else // rose diagram
      eLegType = eSIMPLELINE;

    switch (eLegType)
    {
    case eLINE:
      {
        CPoint p1, p2;
        p1 = ptLayer;
        p1.y += iH / 2;
        p2 = p1;
        p2.x += iW;
        gl->line.drawLine(cdc, p1, p2);
      } break;
    case eSIMPLELINE:
      {
  			CPen pen(PS_SOLID, 1, gl->color);
  			penOld = cdc->SelectObject(&pen);
        CPoint pt = ptLayer;
        pt.y += iH / 2;
				cdc->MoveTo(pt);
        pt.x += iW;
				cdc->LineTo(pt);
		    cdc->SelectObject(penOld);
      } break;
      case eBLOCK:
      {
        Color color = gl->color;
        if (gl->fRprColor)
          color = SysColor(COLOR_WINDOW);
  			CBrush br(color);
  			CPen pen(PS_SOLID, 1, Color(0,0,0));
	      penOld = cdc->SelectObject(&pen);
			  brOld = cdc->SelectObject(&br);
        CRect rct;
        rct.top = ptLayer.y + 4;
        rct.bottom = ptLayer.y + iH - 4;
        rct.left = ptLayer.x + 4;
        rct.right = rct.left + iW - 4;
        cdc->Rectangle(&rct);
     		cdc->SelectObject(penOld);
		    cdc->SelectObject(brOld);
      } break;
      case ePOINT:
      {
        CPoint pt;
        pt.x = ptLayer.x + iW / 2;
        pt.y = ptLayer.y + iH / 2;
				gl->smb.drawSmb(cdc, 0, pt);
      } break;
    }
    cdc->SetTextAlign(TA_LEFT|TA_TOP);
    String sText = gl->sTitle;
    CPoint pt;
    pt.x = ptLayer.x + iW + 5;
    pt.y = ptLayer.y;
		cdc->TextOut(pt.x, pt.y, sText.scVal(), sText.length());
    CSize sz = cdc->GetTextExtent(sText.scVal(), sText.length());
    pt.x += sz.cx + 5;
    pt.y += sz.cy + 5;
    if (rect.right < pt.x)
      rect.right = pt.x;
    if (rect.bottom < pt.y)
      rect.bottom = pt.y;
  }
	cdc->SelectObject(fntOld);
}

void GraphLegend::SaveSettings(const FileName& fn, const String& sSection)
{
	ObjectInfo::WriteElement(sSection.scVal(), "Visible", fn, fShow);
	ObjectInfo::WriteElement(sSection.scVal(), "Rectangle", fn, rect);
}

void GraphLegend::LoadSettings(const FileName& fn, const String& sSection)
{
	ObjectInfo::ReadElement(sSection.scVal(), "Visible", fn, fShow);
	ObjectInfo::ReadElement(sSection.scVal(), "Rectangle", fn, rect);
}
