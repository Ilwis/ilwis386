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
// ScaleBar.cpp : implementation file
//

#include "Headers\toolspch.h"
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\ScaleBar.h"
#include "Headers\Hs\Mapwind.hs"
#include "Client\Base\Framewin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScaleBar

BEGIN_MESSAGE_MAP(ScaleBar, CToolBar)
	ON_EN_SETFOCUS(ID_SCALEBAR,OnSetFocus)
	ON_EN_KILLFOCUS(ID_SCALEBAR,OnKillFocus)
	ON_COMMAND(IDOK,UseScale)
	//{{AFX_MSG_MAP(ScaleBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ScaleBar::ScaleBar()
{
	fActive = false;

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	// Since design guide says toolbars are fixed height so is the font.
	logFont.lfHeight = -12;
	logFont.lfWeight = FW_BOLD;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	lstrcpy(logFont.lfFaceName, "MS Sans Serif");
	fnt.CreateFontIndirect(&logFont);
}

ScaleBar::~ScaleBar()
{
}

void ScaleBar::Create(CWnd* pParent)
{
	int iWidth = 80;

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |
		              CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY;
	CRect rectBB;
	rectBB.SetRect(2,1,2,1);
	CToolBar::CreateEx(pParent, dwCtrlStyle, dwStyle, rectBB, ID_SCALEBAR);
	UINT ai[2];
	ai[0] = ID_SCALEBAR;
	ai[1] = ID_SEPARATOR;
	SetButtons(ai,2);
	CRect rect;
	GetItemRect(0, &rect);
	SetButtonInfo(1, ID_SCALEBAR,	TBBS_SEPARATOR, iWidth - rect.Width());

	rect.top = 3;
	rect.bottom -= 2;
	rect.right = rect.left + iWidth;
	ed.Create(WS_VISIBLE|WS_CHILD|WS_BORDER,rect,this,ID_SCALEBAR);
	ed.SetFont(&fnt);
	SendMessage(DM_SETDEFID,IDOK);

	EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	SetBarStyle(GetBarStyle()|CBRS_GRIPPER|CBRS_BORDER_3D);

	SetWindowText(TR("Scale").c_str());
}

void ScaleBar::OnUpdateCmdUI(CFrameWnd* pParent, BOOL)
{
	if (fActive)
		return;
	CView* vw = pParent->GetActiveView();
	MapPaneView* mpv = dynamic_cast<MapPaneView*>(vw);
	if (0 == mpv) {
		ed.EnableWindow(FALSE);
		return;
	}
	double rScale = mpv->rScaleShow();
	if (rUNDEF == rScale)	{
//		ed.ShowWindow(SW_HIDE);
		ed.SetWindowText(TR("No Scale").c_str());
		ed.EnableWindow(FALSE);
	}
	else {
		String sTxt("1:%.f", rScale);
		ed.SetWindowText(sTxt.c_str());
//		ed.ShowWindow(SW_SHOW);
		ed.EnableWindow(TRUE);
	}
}

void ScaleBar::OnSetFocus()
{
	fActive = true;
	FrameWindow* fw = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (fw)
		fw->HandleAccelerators(false);
}

void ScaleBar::OnKillFocus()
{
	fActive = false;
	FrameWindow* fw = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (fw)
		fw->HandleAccelerators(true);
}


void ScaleBar::UseScale() // called by ScaleBarEdit
{
	CString str;
	ed.GetWindowText(str);
	if (str.GetLength() > 2 && str[0] == '1' && str[1] == ':')
		str = str.Mid(2);
	bool fOk = true;
	double rScale;
	if (-1 != str.Find(':'))
		fOk = false;
	if (fOk) {
		rScale = atof(str);
		if (rScale < 1)
			fOk = false;
		if (rScale > 1e10)
			fOk = false;
	}
	if (fOk) {
		CWnd* wnd = GetOwner();
		CFrameWnd* fw = dynamic_cast<CFrameWnd*>(wnd);
		CView* vw = fw->GetActiveView();
		MapPaneView* mpv = dynamic_cast<MapPaneView*>(vw);
		if (0 == mpv)
			return;
		mpv->UseScale(rScale);
	}
	else
		MessageBeep(-1);
}


