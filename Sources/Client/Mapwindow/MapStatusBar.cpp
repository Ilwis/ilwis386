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
// MapStatusBar.cpp: implementation of the MapStatusBar class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\MapStatusBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(MapStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(MapStatusBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


MapStatusBar::MapStatusBar()
	: dr(0)
{
	sbiCur = sbiNONE;
	fRowBeforeCol = true;
}

MapStatusBar::~MapStatusBar()
{
}

int MapStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rct(0,0,0,0);
	prctl.Create(WS_CHILD|PBS_SMOOTH, rct, this, 1); 	
	Init(sbiNONE);
	return 0;
}

void MapStatusBar::Init(StatusBarItem sbi)
{
	sbiCur |= sbi;
	unsigned int ind[7] = { 0, 0, 0, 0, 0, 0, 0 };
	int iNr = 1;
	if (sbiROWCOL & sbiCur) {
		iRowCol = iNr;
		iNr += 1;
	}
	else
		iRowCol = -1;
	if (sbiCOORD & sbiCur) {
		iCoord = iNr;
		iNr += 1;
	}
	else
		iCoord = -1;
	if (sbiRELCOORD & sbiCur) {
		iRelCoord = iNr;
		iNr += 1;
	}
	else 
		iRelCoord = -1;
	if (sbiLATLON & sbiCur) {
		iLatLon = iNr;
		iNr += 1;
	}
	else
		iLatLon = -1;
	SetIndicators(ind, iNr);

	SetPaneInfo(0, 0, SBPS_STRETCH|SBPS_NOBORDERS, 100);
	HFONT hFont = (HFONT)SendMessage(WM_GETFONT);
	CClientDC dcScreen(NULL);
	HGDIOBJ hOldFont = NULL;
	if (hFont != NULL)
		hOldFont = dcScreen.SelectObject(hFont);

	if (iRowCol > 0) {
		int iWidth = dcScreen.GetTextExtent("9999,9999").cx;
		SetPaneInfo(iRowCol, 0, SBPS_NORMAL, iWidth);
	}
	int iWidth = dcScreen.GetTextExtent("(123456789.12,123456789.12)").cx;
	if (iCoord > 0) 
		SetPaneInfo(iCoord, 0, SBPS_NORMAL, iWidth);
	if (iRelCoord > 0) 
		SetPaneInfo(iRelCoord, 0, SBPS_NORMAL, iWidth);
	if (iLatLon > 0) {
		int iWidth = dcScreen.GetTextExtent("12°34'56.78\" N,123°45'67.89\" E").cx;
		SetPaneInfo(iLatLon, 0, SBPS_NORMAL, iWidth);
	}
	if (hOldFont != NULL)
		dcScreen.SelectObject(hOldFont);
	ResizeProgressCtrl();
}

void MapStatusBar::ShowRowCol(RowCol rc)
{
	if (iRowCol < 0) 
		Init(sbiROWCOL);
	String str;
	if (fRowBeforeCol)
		str = String("%li,%li", rc.Row, rc.Col);
	else
		str = String("%li,%li", rc.Col, rc.Row);
	SetPaneText(iRowCol, str.scVal());
}

void MapStatusBar::ShowCoord(const String& str)
{
	if (iCoord < 0)
		Init(sbiCOORD);
	SetPaneText(iCoord, str.scVal());
}

void MapStatusBar::ShowRelCoord(const String& str)
{
	if (iRelCoord < 0)
		Init(sbiRELCOORD);
	SetPaneText(iRelCoord, str.scVal());
}

void MapStatusBar::ShowLatLon(LatLon ll)
{
	if (iLatLon < 0) {
		if (ll.fUndef()) 
			return;
		Init(sbiLATLON);
	}
	String str = ll.sValue();
	SetPaneText(iLatLon, str.scVal());
}

void MapStatusBar::OnUpdateCmdUI(CFrameWnd*, BOOL)
{
	if (dr)
		UpdateProgressCtrl();
}

void MapStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	CStatusBar::OnSize(nType, cx, cy);
	ResizeProgressCtrl();
}

void MapStatusBar::ResizeProgressCtrl()
{
	CRect rct;
	GetItemRect(0, &rct);
	if (rct.right > 100)
		rct.right = 100;
	prctl.MoveWindow(&rct);
}

void MapStatusBar::UpdateProgressCtrl()
{
	Drawer* drw = const_cast<Drawer*>(dr); // dr is volatile
	if (0 == drw)
		return;
	prctl.SetRange32(drw->riTranquilizer.iLo(), drw->riTranquilizer.iHi());
	prctl.SetPos(drw->iTranquilizer);
}

void MapStatusBar::SetActiveDrawer(Drawer* drw)
{ 
	bool fWasActive = 0 != dr;
	dr = drw; 
	if (0 == drw) {
		prctl.ShowWindow(SW_HIDE);
		if (fWasActive)
			KillTimer(1);
	}
	else {
		UpdateProgressCtrl();
		prctl.ShowWindow(SW_SHOW);
		if (!fWasActive)
			SetTimer(1,250,0);
	}
}

void MapStatusBar::OnTimer(UINT nIDEvent) 
{
	if (dr)
		UpdateProgressCtrl();
}
