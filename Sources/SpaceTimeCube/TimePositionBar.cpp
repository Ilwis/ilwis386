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
// TimePositionBar.cpp: implementation of the TimePositionBar class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "TimePositionBar.h"
#include "SpaceTimeCubeTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

BEGIN_MESSAGE_MAP(TimeSliderCtrl, CSliderCtrl)
	ON_WM_VSCROLL_REFLECT()
END_MESSAGE_MAP()

TimeSliderCtrl::TimeSliderCtrl()
: CSliderCtrl()
, spaceTimeCube(0)
{
}

void TimeSliderCtrl::SetSpaceTimeCube(SpaceTimeCube * _spaceTimeCube)
{
	spaceTimeCube = _spaceTimeCube;
	SetPos(sliderRange - spaceTimeCube->GetTime() * sliderRange);
}

void TimeSliderCtrl::VScroll(UINT nSBCode, UINT nPos)
{
	if (spaceTimeCube != 0 && nSBCode == TB_THUMBTRACK)
		spaceTimeCube->SetTime((sliderRange - nPos) / (double)sliderRange);
}

BEGIN_MESSAGE_MAP( TimePositionBar, CSizingControlBar )
	//{{AFX_MSG_MAP( TimePositionBar )
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

TimePositionBar::TimePositionBar()
{
}

TimePositionBar::~TimePositionBar()
{
}

BOOL TimePositionBar::Create(CWnd* pParent)
{
	CRect rect;
	pParent->GetWindowRect(rect);
	if (!CSizingControlBar::Create(TR("Time Position").c_str(), pParent, CSize(25, rect.Height()), TRUE, 123))
		return FALSE;
	m_dwSCBStyle |= SCBS_SHOWEDGES;

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);

	//CSize cz = cdc->GetTextExtent("gk");
	//CRect scrollRect(2, cz.cy + 6, 20, rct.Height());
	//CRect scrollRect;
	//GetWindowRect(scrollRect);
	//CRect scrollRect (1, 1, 30, 100);
	CRect scrollRect(0, 0, 0, 0);
	slider.Create(WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_NOTICKS | TBS_RIGHT, scrollRect, this, ID_STC_TIMEPOSITIONSLIDER);
	slider.SetRange(0, sliderRange);
	slider.SetPos(sliderRange);
	//slider.SetTicFreq(20);
	//slider.ShowWindow(SW_SHOW);

	return TRUE;
}

void TimePositionBar::OnSize(UINT nType, int cx, int cy)
{
	if (::IsWindow(slider.GetSafeHwnd()))
		//slider.SetWindowPos(this, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
		slider.MoveWindow(0, 0, cx, cy);
}

void TimePositionBar::SetSpaceTimeCube(SpaceTimeCube * _spaceTimeCube)
{
	slider.SetSpaceTimeCube(_spaceTimeCube);
}


