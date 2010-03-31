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
// ColorIntensityBar.cpp : implementation of the ColorIntensityBar class
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
#include "Engine\Domain\dmsort.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Utils\colorScroll.h"
#include "Client\Editors\Utils\colorIntensity.h"
#include "Client\Editors\Representation\ColorIntensityBar.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\syscolor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---[ ColorIntensityBar ]--------------------------------------------------------------------
BEGIN_MESSAGE_MAP(ColorIntensityBar, CSizingControlBar)
    //{{AFX_MSG_MAP(ColorIntensityBar)
	ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

ColorIntensityBar::ColorIntensityBar()
{
	color=RGB(0,0,0);
}

ColorIntensityBar::~ColorIntensityBar()
{}


BOOL ColorIntensityBar::Create(CView* v, Color clr)
{
	view = v;
	CFrameWnd* pParent = view->GetParentFrame();
	ASSERT(pParent);

	BOOL iRet = CSizingControlBar::Create(SEDTitleColorIntensity.scVal(), pParent, CSize(iBARWIDTH, iBARWIDTH*2), TRUE, ID_COLORINTENSITYBAR);

	ASSERT_VALID(this);

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_ANY);

	CRect rct;
	GetClientRect(rct);
	colIntensity.Create(CRect(rct.left + 5, rct.top +15, rct.right -15,rct.bottom -5), this, clr,ID_COLORINTENSITY);
	m_szHorz = CSize(iBARWIDTH*2, 110);
	colIntensity.SetPostOffice( pParent);

	return iRet;
}

void ColorIntensityBar::SetColor(COLORREF clr)
{
	colIntensity.SetColor(clr, FALSE);
}

void ColorIntensityBar::OnSize(UINT nType, int cx, int cy)
{
	if ( !GetSafeHwnd()) return;
	if ( !colIntensity.GetSafeHwnd()) return;

	CRect rct;
	GetClientRect(&rct);
	if ( IsHorzDocked() )
	{
		colIntensity.SetOrientation(false);
		m_szFloat = rct.Size();
	}
	if ( IsVertDocked() )
	{
		colIntensity.SetOrientation(true);
		m_szFloat = rct.Size();
	}

	colIntensity.Resize(nType, cx -2, cy);

	Invalidate();
}
