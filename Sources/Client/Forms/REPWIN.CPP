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
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"

#define REPWIN_C
#include "Client\Forms\repwin.h"
//#include "Headers\xercesc\util\Platforms\Win32\resource.h"
#include "Headers\Hs\Userint.hs"
#include "Headers\messages.h"

BEGIN_MESSAGE_MAP( ReportWindow, CDialog )
	//{{AFX_MSG_MAP( MainWindow )
  ON_MESSAGE(ID_ILWISREP, OnIlwisReportMsg)
  ON_WM_SYSCOMMAND()
  ON_COMMAND(IDCANCEL, OnCancel)
  ON_COMMAND(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int ReportWindow::iNrReportWindows = 0;

ReportWindow::ReportWindow(Tranquilizer* tranq) : 
	CDialog(),
  cslTranq(&tranq->csTranq, TRUE),
	threadHandle(0),
	fShown(false)
{
	iMyNumber = ++iNrReportWindows; // Thread Safe !!
	fUpdateLoop = false;
	Create(tranq);
}

void ReportWindow::Create(Tranquilizer* tranq)
{
	tr = tranq;
	struct dlgt
	{
		DLGTEMPLATE dlg;
		short cMenu[1];
		short cClass[1];
		short cCaption[1];
	} dlgTranq;
	dlgTranq.dlg.style = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	dlgTranq.dlg.dwExtendedStyle = 0;
	dlgTranq.dlg.cdit = 0;
	dlgTranq.dlg.x = 0;
	dlgTranq.dlg.y = 0;
	dlgTranq.dlg.cx = 0;
	dlgTranq.dlg.cy = 0;
	dlgTranq.cMenu[0] = 0;
	dlgTranq.cClass[0] = 0;
	dlgTranq.cCaption[0] = 0;
	CreateIndirect(&dlgTranq, GetDesktopWindow());
}

void ReportWindow::Stop()
{
	fUpdateLoop = false;
	if (threadHandle != INVALID_HANDLE_VALUE && threadHandle != 0)
		::WaitForSingleObject(threadHandle, 5000);
	threadHandle = 0;
}

ReportWindow::~ReportWindow()
{
	--iNrReportWindows;
}

LONG ReportWindow::OnIlwisReportMsg(WPARAM wP, LPARAM lp)
{
	switch(wP)
		{
			case ILWREP_TITLE:
				SetTitle();
				break;
			case ILWREP_UPDATE:
				Update();
				break;
			case ILWREP_TEXT:
				SetText();
				break;
		}
	return 1;
}

BOOL ReportWindow::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Disable the Close button, is not allowed for tranquilizers
	CMenu* menu = GetSystemMenu(false);
	menu->RemoveMenu(SC_CLOSE, MF_BYCOMMAND);
	menu->RemoveMenu(SC_SIZE, MF_BYCOMMAND);
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
	
	CString sStop = SUIStop.c_str();
	CString sHelp = SUIHelp.c_str();
	CSize sizStop = cdc.GetTextExtent(sStop);
	CSize sizHelp = cdc.GetTextExtent(sHelp);
	CSize sizButton;
	if (sizStop.cx > sizHelp.cx)
		sizButton = sizStop;
	else
		sizButton = sizHelp;
	sizButton.cx += 20;
	sizButton.cy += 10;
	int iY = sizButton.cy;
	int iX = sizButton.cx;
	int iHeight = 3.1 * iY;
	int iWidth = 3 * iHeight;
	int iHalfWidth = iWidth / 2;
	
	CPoint pBut(iHalfWidth - 1.6 * iX, 2 * iY);
	CRect rectBut(pBut, sizButton);
	rectBut.DeflateRect(3,3);
	if (!tr->fNoStopButton)
	{		
		butAbort.Create(sStop, WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
			rectBut, this, IDCANCEL);
		butAbort.SetFont(fnt);
	}		
	
	CPoint pShift( tr->fNoStopButton ? iX : 2 * iX, 0);
	rectBut += pShift;
	butHelp.Create(sHelp, WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
		rectBut, this, IDHELP);
	butHelp.SetFont(fnt);
	
	CPoint pText(0, 0.25 * iY);
	CSize sizText(iWidth, 0.5 * iY);
	CRect rectText(pText, sizText);
	stText.Create("", WS_CHILD|WS_VISIBLE|SS_CENTER|SS_NOPREFIX, rectText, this, 100);
	stText.SetFont(fnt);
	
	CRect rectPrgr(iX/2, iY, iWidth - iX/2, 1.5 * iY);
	prCtrl.Create(WS_CHILD | PBS_SMOOTH | WS_BORDER  , rectPrgr, this, 101);
	prCtrl.SetRange(0,1000);
	
	tr->cs.Lock();
	SetWindowText(tr->sTitle.sVal());
	stText.SetWindowText(tr->sTranqText.sVal());
	tr->cs.Unlock();
	
	int iXScreen = GetSystemMetrics(SM_CXFULLSCREEN);
	int iYScreen = GetSystemMetrics(SM_CYFULLSCREEN);
	int iMove = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER); // Bas Retsios: Added this line here to eventually remove the CheckPosition procedure if the code below is sufficient
	
	int iXW, iYW, iWW, iHW;
	iWW = iWidth;
	iHW = iHeight+iY;
	iYW = (iYScreen - iHW)/2;
	iXW = (iXScreen - iWW)/2;
	
	// Bas Retsios, 15 February 2001. Purpose: solve bug 1774
	// Correct the position of the ReportWindow before SetWindowPos
	// Intention of the calculation is that iXW and iYW satisfy the following:
	// a) Preferred display location is screen-centre
	// b) Two or more Report Windows should not overlap completely (if possible)
	// c) Minimum position for (iXW,iYW) = (iMove,iMove) and not (0,0)
	// d) Maximum position for (iXW,iYW): the window should be inside the screen
	iXW = iMove + (iXW + (iMyNumber - 2) * iMove) % (iXScreen - iMove - iWW);
	iYW = iMove + (iYW + (iMyNumber - 2) * iMove) % (iYScreen - iMove - iHW);
	
	SetWindowPos(0,iXW,iYW,iWW,iHW,SWP_HIDEWINDOW);
	//CheckPosition(iXW, iYW, iWW, iHW); // Bas Retsios, 15 February 2001: commented out / replaced by above calculation, because although CheckPosition is supposed to work, it rarely does, probably because WindowFromPoint.doesn't work as expected
	time(&iFirstTime);
	
	SetTitle();
	return TRUE;
}

void ReportWindow::CheckPosition(int& iX, int& iY, int& iW, int& H)
{
	// prevent the code to find its own window
//	ShowWindow(SW_HIDE);  

	int iMove = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER);
	int iXScreen = GetSystemMetrics(SM_CXFULLSCREEN);
	int iYScreen = GetSystemMetrics(SM_CYFULLSCREEN);

	CWnd* pw = WindowFromPoint(CPoint(iX,iY));
	// Is there a window at the top left position of this window?
	while ( pw != 0 )
	{
		pw = WindowFromPoint(CPoint(iX,iY) + CSize(iMove, iMove));
		if (pw != 0)
		{
			CRect rectBack;
			pw->GetWindowRect(&rectBack);
			rectBack.SetRect(rectBack.TopLeft(), rectBack.TopLeft());
			rectBack.InflateRect(iMove, iMove);
			if (rectBack.PtInRect(CPoint(iX, iY))) 
			{
				iX = iX + iMove < iXScreen ? iX + iMove : iMove;
				iY = iY + iMove < iYScreen ? iY + iMove : iMove;
				continue;
			}
		}
		break;
	}

//	ShowWindow(SW_SHOW); // redisplay the window
}

void ReportWindow::OnCancel()
{
  tr->SetAborted();
}

void ReportWindow::OnHelp()
{
	IlwWinApp()->showHelp(help);
}

void ReportWindow::OnSysCommand(UINT nID, LPARAM lParam)
{
  switch (nID & 0xFFF0) {
    case SC_MINIMIZE:
      UpdateIconic();
      break;
    case SC_RESTORE:
      SetTitle();
      break;
  }
  CDialog::OnSysCommand(nID, lParam);
}

void ReportWindow::SetTitle()
{
  tr->cs.Lock();
 	DelayedShow();
  SetWindowText(tr->sTitle.sVal());
  tr->cs.Unlock();
}

void ReportWindow::SetText()
{
  tr->cs.Lock();
	DelayedShow();
  stText.SetWindowText(tr->sTranqText.sVal());
  tr->cs.Unlock();
  prCtrl.ShowWindow(SW_HIDE);
}

void ReportWindow::UpdateIconic()
{
  tr->cs.Lock();
//  String s("%S - %S", tr->sTitle.sVal(), sUpdate());
//  Rewritten as follows because String ("%S - %S" ..) gives unknown error in strng.cpp
	String s(" - %S", sUpdate());
	s = tr->sTitle.sVal() + s;
  SetWindowText(s.sVal());
  tr->cs.Unlock();
}

String ReportWindow::sUpdate()
{
  ILWISSingleLock sl(&tr->cs,TRUE, SOURCE_LOCATION);
	if ( tr->fGetOnlyGauge() )
		return tr->sTranqText;
  if (tr->iVal == iUNDEF) 
    return tr->sTranqText;
  else if (tr->iMax == 0) 
    return String("%S %li", tr->sTranqText, tr->iVal);
  else 
    return String("%S %li (%4.1f%%)",
               tr->sTranqText, tr->iVal,
               (tr->iVal * 100.0) / tr->iMax);
}

UINT ReportWindow::UpdateInThread(LPVOID data)
{
	ReportWindow *rw = (ReportWindow *)data;
	HWND reportHandle = rw->m_hWnd;

	Tranquilizer *tr = rw->tr;
  
	while(rw->fUpdateLoop && !(tr->fAbort))
	{
		Sleep(100);
		::PostMessage(reportHandle, ID_ILWISREP, ILWREP_UPDATE, 0); 
	}

 	return 1;
}

void ReportWindow::Update()
{
	if ( fUpdateLoop )
	{ 
		if (!fShown) {
			DelayedShow();
			if (!fShown)
				return;
		}
		if (!IsIconic())
		{
	 	  tr->cs.Lock();
			String s = sUpdate();
			long iVal = tr->iVal;
			long iMax = tr->iMax;
			tr->cs.Unlock();

			stText.SetWindowText(s.sVal());

			if ((iMax > 0) && (iVal != iUNDEF))
			{
				double rVal = iVal;
				rVal /= iMax;
				rVal *= 1000;
				prCtrl.ShowWindow(SW_SHOWNA);
				prCtrl.SetPos(rVal);
			}
		  else
				prCtrl.ShowWindow(SW_HIDE);
		}
		else
			UpdateIconic();
	}
	else
	{
		fUpdateLoop = true;
		CWinThread *thread = ::AfxBeginThread(ReportWindow::UpdateInThread, (void *)this);
		threadHandle = thread->m_hThread;
	}
}


void ReportWindow::DelayedShow()
{
	if (fShown)
		return;
	bool fShow = false;
	time_t iSecondTime;
	time(&iSecondTime);
	if (!tr->fDelayedShow)
		fShow = true;
	else {
		double rTime = difftime(iSecondTime, iFirstTime);
		if (rTime > 1.8)
			fShow = true;
		else {
 			tr->cs.Lock();
			long iVal = tr->iVal;
			long iMax = tr->iMax;
			tr->cs.Unlock();
			if ((iMax > 0) && (iVal > 0)) {
				iVal += 1; 
				iMax += 1;
				iVal += iMax / 20;
				if (rTime * iMax / iVal > 2)
					fShow = true;
			}
		}
	}
	if (fShow) {
		int iHtp = tr->htp.iTopic;
		if (iHtp <= 0)
			butHelp.EnableWindow(FALSE);
#ifdef _DEBUG		
    SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE /*| SWP_NOZORDER*/ | SWP_SHOWWINDOW);
#else
    SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE /*| SWP_NOZORDER*/ | SWP_SHOWWINDOW);
#endif
		fShown = true;
	}
}

//--[ ReportThread ]------------------------------------------------------------------
//ReportThread::ReportThread(Tranquilizer *tr) :
//  trq(tr)
//{}
//
//BOOL ReportThread::InitInstance()
//{
//  repWin.Create(trq);
//	return TRUE;
//}
//
//int ReportThread::ExitInstance()
//{
//	return CWinThread::ExitInstance();
//}






