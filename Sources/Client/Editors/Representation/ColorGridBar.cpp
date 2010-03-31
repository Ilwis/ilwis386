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
// ColorGridBar.cpp : implementation of the ColorGridBar class
//
#include "Client\Headers\formelementspch.h"
#include "Headers\Hs\Represen.hs"
#include "Headers\Hs\Editor.hs"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Headers\Hs\COLORS.hs"
#include "Engine\Base\Algorithm\Random.h"
#include "Client\Base\ZappToMFC.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\ilwis.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Utils\ColorCB.h"
#include "Client\Editors\Utils\ColorGrid.h"
#include "Client\Editors\Representation\ColorGridBar.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\syscolor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---[ ColorGridBar ]------------------------------------------------------------------
static const int ID_COLORGRID = 300;

BEGIN_MESSAGE_MAP(ColorGridBar, CSizingControlBar)
	ON_WM_SIZE()
END_MESSAGE_MAP()

ColorGridBar::ColorGridBar()
{
	color=RGB(0,0,0);
}

ColorGridBar::~ColorGridBar()
{}

BOOL ColorGridBar::Create(CView* v)
{
	view = v;
	CFrameWnd* pParent = view->GetParentFrame();
	ASSERT(pParent);

	BOOL iRet = CSizingControlBar::Create(SEDTitleColorGridBar.scVal(), pParent, CSize(iBARWIDTH+20, iBARWIDTH+20), TRUE, ID_COLORGRIDBAR);

	ASSERT_VALID(this);

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_ANY);
	CRect rct;
	GetClientRect(&rct);
	colGrid.Create(WS_CHILD | WS_VISIBLE, CRect(rct.left + 5, rct.top +15, rct.right -15,rct.bottom -5), this, ID_COLORGRID,15);
	colGrid.SetPostOffice(pParent);

	return iRet;
}

void ColorGridBar::SelectColor(COLORREF clr)
{
	colGrid.SelectColor(clr);
}

void ColorGridBar::OnSize(UINT nType, int cx, int cy)
{
	colGrid.OnSize(nType, cx-2, cy+2);
	Invalidate();
}

void ColorGridBar::SaveSettings(const String& sKey)
{
	colGrid.SaveSettings(sKey);
}

void ColorGridBar::LoadSettings(const String& sKey)
{
	colGrid.LoadSettings(sKey);
}
