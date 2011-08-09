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
// ButtonBar.cpp : implementation file
//

#include "Headers\stdafx.h"
#include "Client\Base\ilwis30.h"
#include "Client\Base\ButtonBar.h"
#include "Client\ilwis.h"
#include "Engine\Base\System\Appcont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// constants

const int MAXBUTTONS = 100;
const COLORREF clrMask = RGB(192, 192, 192);

// ButtonBar

ButtonBar::ButtonBar()
{
	m_ilButtons.Create(16, 15, ILC_MASK, 0, 4);
	m_iSeparWidth = 8;
}

ButtonBar::~ButtonBar()
{
	UnloadButtons();
}


BEGIN_MESSAGE_MAP(ButtonBar, CToolBar)
	//{{AFX_MSG_MAP(ButtonBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ButtonBar message handlers

bool ButtonBar::Create(CWnd* pParent, const FileName& fnButtons, const string& sTitle, int iID)
{
	ASSERT_VALID(pParent);	// must have a parent

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE;

	CRect rect;
	rect.SetRect(2,1,2,1);

	if (!CToolBar::CreateEx(pParent, dwCtrlStyle, dwStyle, rect, iID))
		return FALSE;

// Repeat setting the Control bar styles: for one reason or another they are not properly
// initialized when calling Create(). The CBRS_BORDER_3D style is necessary to eliminate the
// garbage pixels when the TBSTYLE_FLAT is applied
	SetBarStyle(GetBarStyle() | CBRS_GRIPPER /*| CBRS_TOOLTIPS */| CBRS_FLYBY | CBRS_BORDER_3D | CBRS_SIZE_DYNAMIC);

	if (!LoadButtons(fnButtons))
		return FALSE;

	EnableDocking(CBRS_ALIGN_ANY);
	ASSERT_VALID(this);

	// caption during floating
	if (sTitle != "")
		SetWindowText(sTitle.c_str());
	else
		SetWindowText(TR("Tool Bar").c_str());

	return TRUE;
}
// LoadButtons will only load upto MAXBUTTONS buttons; this includes the separators!
bool ButtonBar::LoadButtons(const FileName &fnButtons)
{
	String sP, sID, sBmpName;
	UINT iID;
	FileName fn = fnButtons;
	fn.Dir(IlwWinApp()->Context()->sIlwDir() + "\\Resources\\But");
	File fil(fn);
	
	UINT BarButtons[MAXBUTTONS];  // specify space for 35 buttons
	int iIndex = 0;
	Array<String> as;
	m_ilButtons.SetImageCount(0);

	while (!fil.fEof() && iIndex < MAXBUTTONS)
	{
		fil.ReadLnAscii(sP);

		as.Reset();
		Split(sP, as, " \t");
		if (as.iSize() == 0)
			iID = ID_SEPARATOR;
		else {
			sID = as[0];
			iID = sID.iVal();
			if (iID == iUNDEF)
				continue;
		
			sBmpName = as[1].sTrimSpaces();
		}

		if (iID != ID_SEPARATOR)
		{
			CBitmap bmp;
			LoadIlwisButtonBitmap(sBmpName.c_str(), bmp);

			// the mask color currently is fixed (rgb(192,192,192) == lightgray)
			m_ilButtons.Add(&bmp, clrMask);  // the Add function makes a copy of the bitmap (see ImageList_Add)
		}

		BarButtons[iIndex++] = iID;
	}

	GetToolBarCtrl().SetImageList(&m_ilButtons);
	SetButtons(BarButtons, iIndex);

	return TRUE;
}

void ButtonBar::UnloadButtons()
{
	IMAGEINFO ifo;
	for (int i=0; i < m_ilButtons.GetImageCount(); ++i)
	{
		m_ilButtons.GetImageInfo(i, &ifo); // Find the bitmap
		m_ilButtons.Remove(i); // First detach it from GDI
		UnloadIlwisButtonBitmap(ifo.hbmImage); // Then delete it as required
	}
}

void ButtonBar::SetSeparatorWidth(int iNewWidth)
{
	if (iNewWidth > 0)
		m_iSeparWidth = iNewWidth;
}

int ButtonBar::iGetSeparatorWidth()
{
	return m_iSeparWidth;
}

void ButtonBar::AddButton(int iIndex, int iID, CBitmap &bmp)
{
	int iNew = m_ilButtons.Add(&bmp, clrMask);  // the Add function makes a copy of the bitmap (see ImageList_Add)
	SetButtonInfo(iNew, iID, TBBS_BUTTON, iIndex);
}

void ButtonBar::AddSeparator(int iIndex)
{
	SetButtonInfo(iIndex, ID_SEPARATOR, TBBS_SEPARATOR, iIndex);
}
