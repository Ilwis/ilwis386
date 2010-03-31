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
// colorScroll.cpp: implementation of the colorScroll class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Client\Editors\Utils\colorScroll.h"
#include "Client\Editors\Utils\colorIntensity.h"

//---[ ColorScrollBar ]------------------------------------------------------------------
BEGIN_MESSAGE_MAP(ColorScrollBar, CSliderCtrl)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ColorScrollBar::ColorScrollBar()
{
}

ColorScrollBar::~ColorScrollBar()
{}

BOOL ColorScrollBar::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CSliderCtrl::Create(dwStyle, rect, pParentWnd, nID);
	SetRange(0, 255);
	SetTicFreq(20);

	return TRUE;
}

HBRUSH ColorScrollBar::CtlColor( CDC* pDC, UINT nCtlColor )
{
	if (nCtlColor == CTLCOLOR_STATIC )
	{
		Color col;
		switch(GetDlgCtrlID())
		{
			case ID_COLORINTENSITY_REDV:
				col = RGB(255,0,0);
				break;
			case ID_COLORINTENSITY_GREENV:
				col = RGB(0, 255,0); 
				break;
			case ID_COLORINTENSITY_BLUEV:
				col = RGB(0, 0,255);
				break;
			case ID_COLORINTENSITY_REDH:
				col = RGB(255,0,0);
				break;
			case ID_COLORINTENSITY_GREENH:
				col = RGB(0, 255,0); 
				break;
			case ID_COLORINTENSITY_BLUEH:
				col = RGB(0, 0,255);
				break;
		}
		if ( brushCtl.GetSafeHandle())
			brushCtl.DeleteObject();
		brushCtl.CreateSolidBrush(col);
		return brushCtl;
	}
	return CSliderCtrl::OnCtlColor(pDC, this, nCtlColor);

}

