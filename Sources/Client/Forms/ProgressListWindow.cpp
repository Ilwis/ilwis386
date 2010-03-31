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
/*
	Last change:  WK   14 Jan 99   10:25 am
*/

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\FormElements\ReportListView.h"
#include "Client\Forms\ProgressListWindow.h"
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"
#include "Headers\Htp\Ilwis.htp"
#include "Headers\Hs\Userint.hs"
#include "Headers\Hs\Userint.hs"
#include "Headers\messages.h"

BEGIN_MESSAGE_MAP( ProgressListWindow, CFrameWnd )
  ON_MESSAGE(ID_ILWISREP, OnIlwisReportMsg)
  ON_WM_SYSCOMMAND()
  ON_COMMAND(IDCANCEL, OnCancel)
  ON_COMMAND(IDHELP, OnHelp)
  ON_WM_ERASEBKGND()
  ON_WM_CLOSE()
  ON_WM_SIZE()
  ON_WM_TIMER()
  ON_NOTIFY(NM_CLICK, ID_REPLIST, OnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int ProgressListWindow::iNrReportWindows = 0;
const int iDEFHEIGHT=150;
const int iDEFWIDTH=560;

ProgressListWindow::ProgressListWindow() : 
	CFrameWnd(),
	threadHandle(0),
	fShown(false),
	iFirstTime(iUNDEF)
{
	iMyNumber = ++iNrReportWindows; // Thread Safe !!
	fUpdateLoop = false;
	Create();
}

void ProgressListWindow::Create()
{
	int style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
	
	if ( CFrameWnd::Create(NULL, "Operation progress", style ,CRect(0,0,0,0), GetDesktopWindow()) != 0)
		Init();
}

ProgressListWindow::~ProgressListWindow()
{
	fUpdateLoop = false;
}

BOOL ProgressListWindow::DestroyWindow()
{
	IlwisSettings settings("ProgressWindow");
	rlvTrqList.SaveSettings(settings);
	CRect rct;
	GetWindowRect(&rct);
	settings.SetValue("Window", rct);
	
	fUpdateLoop = false;
	Sleep(500); // give the running thread time to disapear	

	return CFrameWnd::DestroyWindow();
}

void ProgressListWindow::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if ( rlvTrqList.GetSelectedCount() != 0 )		
	{
		butAbort.EnableWindow(TRUE)	;
	}		
}

BOOL ProgressListWindow::OnEraseBkgnd(CDC* pDC)
{
   CBrush backBrush(::GetSysColor(COLOR_3DFACE));
   CBrush* pOldBrush = pDC->SelectObject(&backBrush);
   CRect rect;
   pDC->GetClipBox(&rect);     // Erase the area needed.
   pDC->PatBlt(rect.left, rect.top, rect.Width(),
   rect.Height(), PATCOPY);
   pDC->SelectObject(pOldBrush);
   return TRUE;

}

LONG ProgressListWindow::OnIlwisReportMsg(WPARAM wP, LPARAM lp)
{
	int iProgressID = HIWORD(wP);
	int iType = LOWORD(wP);
	if ( iProgressID == 0 	)
		return 1;
	switch(iType)
	{
		case ILWREP_TITLE:
			SetTitle(iProgressID);
			break;
		case ILWREP_UPDATE:
			Update(iProgressID);
			break;
		case ILWREP_TEXT:
			SetText(iProgressID);
			break;
	}
	return 1;
}

void ProgressListWindow::Init()
{
	// Disable the Close button, is not allowed for tranquilizers
	CMenu* menu = GetSystemMenu(false);
	menu->RemoveMenu(SC_CLOSE, MF_BYCOMMAND);
	menu->RemoveMenu(SC_MAXIMIZE, MF_BYCOMMAND);

	// remove separator also
	int iCnt = menu->GetMenuItemCount() - 1;
	if (menu->GetMenuItemID(iCnt) == 0) // last menuitem is a separator
		menu->RemoveMenu(iCnt, MF_BYPOSITION);

	
	// Add Ilwis icon	
	HICON hIconLogo = IlwWinApp()->LoadIcon("LogoIcon");
	SetIcon(hIconLogo, TRUE);

	int iImg = IlwWinApp()->iImage("ilwis");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco, FALSE);

	// Use user preferred font
	CFont *fnt = IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium);	
	CClientDC cdc(this);

	CRect rctl(0, 0, iDEFWIDTH, iDEFHEIGHT - 40);
	rlvTrqList.Create(this, rctl);
	rlvTrqList.SetFont(fnt);
	

	CSize sizHelp = cdc.GetTextExtent(SUIHelp.scVal());
	
	CRect rct;
	IlwisSettings settings("ProgressWindow");	
	rct = settings.rctValue("Window");


	bool fOutsideBottomRight = false;
	CWnd* wnd = GetDesktopWindow();
	if (wnd != NULL)
	{
		CDC* dc = wnd->GetDC();
		if (dc != NULL)
		{
			HDC hdc = dc->m_hDC;
			if (hdc != NULL)
			{
				int iXSize = GetDeviceCaps(hdc, HORZRES);
				int iYSize = GetDeviceCaps(hdc, VERTRES);
				fOutsideBottomRight = rct.BottomRight().x > iXSize || rct.BottomRight().y > iYSize;
			}
			wnd->ReleaseDC(dc);
		}
	}

	if (!fOutsideBottomRight && rct.TopLeft().x >= 0 && rct.TopLeft().y >= 0 && rct.Height() > 0 && rct.Width() > 0)
		MoveWindow(&rct);
	else
		MoveWindow(0,0,  iDEFWIDTH,iDEFHEIGHT + sizHelp.cy * 2);

	butAbort.Create(SUIStop.scVal(), WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON, CRect(0,0,0,0), this, IDCANCEL);	
	butAbort.SetFont(fnt);
	butHelp.Create(SUIHelp.scVal(), WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,  CRect(0,0,0,0), this, IDHELP);
	butHelp.SetFont(fnt);
	butAbort.EnableWindow(FALSE);
	
	OnSize(-1, -1, -1);

	SetWindowText(SUITitleProgress.scVal());
		
	ShowWindow(SW_HIDE);

	SetTimer(1, 8000, 0);


}

void ProgressListWindow::OnSize(UINT nType, int cx, int cy)
{
	if ( rlvTrqList.GetSafeHwnd() != 0 && 
		butHelp.GetSafeHwnd() != 0 && 
		butAbort.GetSafeHwnd() != 0 &&
		cx != 0 &&
		cy != 0)
	{

		CFont *fnt = IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium);	
		CClientDC cdc(this);

		CString sStop = SUIStop.scVal();
		CString sHelp = SUIHelp.scVal();
		CSize sizStop = cdc.GetTextExtent(sStop);
		CSize sizHelp = cdc.GetTextExtent(sHelp);
		CSize sizButton;
		if (sizStop.cx > sizHelp.cx)
			sizButton = sizStop;
		else
			sizButton = sizHelp;
		sizButton.cx += 20;
		sizButton.cy += 10;
		
		CRect rct;
		GetClientRect(rct);
		CRect rctList(CPoint(0,0), CSize(rct.Width(), rct.Height() - sizHelp.cy * 2.1));
		rlvTrqList.MoveWindow(rctList);
	
	
		CPoint pHelp(rct.Width() -  sizHelp.cx * 2 , rct.Height() - sizHelp.cy * 1.7);
		CPoint pStop(pHelp.x - sizButton.cx * 1.5, pHelp.y);
		
		CRect rectBut(pStop, sizButton);
		butAbort.MoveWindow(rectBut);

	 	rectBut = CRect(pHelp, sizButton);
		butHelp.MoveWindow(rectBut);
	}		
}

void ProgressListWindow::OnCancel()
{
	if ( rlvTrqList.iNumberOfItems() == 1)
		rlvTrqList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);	
	if ( rlvTrqList.GetSelectedCount() > 0 )
	{
		POSITION pos = rlvTrqList.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return ; // nothing selected

		int id = rlvTrqList.GetNextSelectedItem(pos);
		unsigned short iProgressID =  rlvTrqList.iGetRelevantProgressID(id);
		Stop(iProgressID);
		if ( rlvTrqList.iNumberOfItems() > 1)
			butAbort.EnableWindow(FALSE);
	}
}

void ProgressListWindow::OnHelp()
{
	if ( rlvTrqList.GetSelectedCount() > 0)
		rlvTrqList.ShowHelp();
	else
	{
		String sHelpFile (ChmFinder::sFindChmFile("ilwis.chm"));

		::HtmlHelp(::GetDesktopWindow(), sHelpFile.sVal(), HH_HELP_CONTEXT, htpProgressManager);
		::HtmlHelp(0, sHelpFile.sVal(), HH_DISPLAY_TOC, 0);		
	}		
}

void ProgressListWindow::OnSysCommand(UINT nID, LPARAM lParam)
{
  switch (nID & 0xFFF0) {
    case SC_MINIMIZE:
//      UpdateIconic();
      break;
    case SC_RESTORE:
//      SetTitle();
      break;
  }
  CFrameWnd::OnSysCommand(nID, lParam);
}

void ProgressListWindow::SetTitle(unsigned short iProgressId)
{
	if ( fDelayedShow())
		ShowWindow(SW_SHOWNA);
	rlvTrqList.SetTitle(iProgressId);
}

void ProgressListWindow::SetText(unsigned short iProgressId)
{
	if ( fDelayedShow())
		ShowWindow(SW_SHOWNA);
	rlvTrqList.SetTextOnly(iProgressId);
}

UINT ProgressListWindow::UpdateInThread(LPVOID data)
{
	ProgressListWindow *rw = (ProgressListWindow *)data;
	HWND reportHandle = rw->m_hWnd;
	

	while(rw->fUpdateLoop )
	{
		Sleep(100);
		rw->UpdateItems();
		if ( rw->fDelayedShow() )
			rw->ShowWindow(SW_SHOWNA);
	}

 	return 1;
}

void ProgressListWindow::UpdateItems()
{
	rlvTrqList.UpdateItems(this);
}

void ProgressListWindow::Update(int iID)
{
	if ( fUpdateLoop )
	{ 
		rlvTrqList.Update(iID);
	}
	else
	{
		fUpdateLoop = true;
		CWinThread *thread = ::AfxBeginThread(ProgressListWindow::UpdateInThread, (void *)this);
		threadHandle = thread->m_hThread;
	}
}

void ProgressListWindow::Start(Tranquilizer *tr)
{
	rlvTrqList.AddTopItem(tr);
	if ( iFirstTime == iUNDEF )
		time(&iFirstTime);
	bool fEnable = rlvTrqList.iNumberOfItems() == 1 || rlvTrqList.GetSelectedCount() > 0 ;
	butAbort.EnableWindow(fEnable);
}

void ProgressListWindow::Stop(unsigned short iProgressID)
{
	fUpdateLoop = false;
	rlvTrqList.RemoveItem(iProgressID);
	if ( rlvTrqList.iNumberOfItems() == 0)
		iFirstTime = iUNDEF;
}

bool ProgressListWindow::fDelayedShow()
{
	if ( iFirstTime == iUNDEF)
		return false;
	
	if (fShown)
		return true;
	time_t iSecondTime;
	time(&iSecondTime);
	double rTime = difftime(iSecondTime, iFirstTime);
	fShown = rTime > 2 ;
	return fShown;		
}

void ProgressListWindow::OnTimer(UINT iIDEvent)
{
	if ( rlvTrqList.iNumberOfItems() == 0 )
	{
		ShowWindow(SW_HIDE);
		iFirstTime = iUNDEF;
	}		
}

