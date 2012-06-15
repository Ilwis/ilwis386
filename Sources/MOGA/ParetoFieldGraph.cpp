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
// FieldGraph.cpp: implementation of the FieldGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "ParetoFieldGraph.h"
#include "ParetoGraphFunction.h"

ParetoGraphWindow::ParetoGraphWindow(FormEntry *f)
: SimpleGraphWindowWrapper(f)
{
}

void ParetoGraphWindow::DrawFunction(CDC* pDC, const SimpleFunction * pFunc)
{
	// create and select a thin, blue pen
	CPen penBlue;
	penBlue.CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	CPen* pOldPen = pDC->SelectObject(&penBlue);

	// Draw the function here
	if (pFunc)
	{
		// Also plot the anchor points
		CBrush brushGreen (RGB(0, 128, 0));
		CBrush brushRed (RGB(128, 0, 0));
		int iSelectedAnchor = ((ParetoGraphFunction*)pFunc)->iGetAnchorNr();
		for (int i = 0; i < pFunc->vAnchors().size(); ++i)
		{
			double rAnchorX = (pFunc->vAnchors().at(i)).X;
			double rAnchorY = (pFunc->vAnchors().at(i)).Y;
			int iAnchorX = iXToScreen(rAnchorX);
			int iAnchorY = iYToScreen(rAnchorY);
			CRect rect (iAnchorX, iAnchorY, iAnchorX, iAnchorY);
			rect.InflateRect(2, 2, 3, 3); // a 5x5 pixel square
			if (i == iSelectedAnchor) {
				pDC->FrameRect(rect, &brushRed);
				rect.InflateRect(1, 1, 1, 1); // a 7x7 pixel square
				pDC->FrameRect(rect, &brushRed);
			} else
				pDC->FrameRect(rect, &brushGreen);
		}
	}

	// put back the old objects

	pDC->SelectObject(pOldPen);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ParetoFieldGraph::ParetoFieldGraph(FormEntry* parent)
: FieldGraph(parent)
{
}

void ParetoFieldGraph::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);

	sgw = new ParetoGraphWindow(this);
	sgw->Create(NULL,	"Graph", WS_CHILD | WS_VISIBLE, CRect(pntFld, dimFld), _frm->wnd(), Id());

  CreateChildren();
}

