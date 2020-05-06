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
// FormBaseWnd.cpp: implementation of the FormBaseWnd class.
//
//////////////////////////////////////////////////////////////////////

#define FORMBASEWND_C
#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "FormBaseWnd.h"
#include "Client\ilwis.h"
#include "Engine\Base\Algorithm\Random.h"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"

const UINT id_apply = 8503;

BEGIN_MESSAGE_MAP(FormBaseWnd, CWnd)
	//{{AFX_MSG_MAP(FormBaseWnd)
  ON_WM_DRAWITEM()
  ON_WM_MEASUREITEM()
  ON_WM_SETCURSOR()
  ON_COMMAND(IDOK, OnOK)
  ON_COMMAND(IDCANCEL, OnCancel)
  ON_COMMAND(IDHELP, OnHelp)
  ON_COMMAND(id_apply, apply)
  ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


FormBaseWnd::FormBaseWnd(CWnd* parent, const String& sTitle, int fbs, int _dwStyle)
: FormBase(parent, sTitle, fbs)
, fDataChanged(false)
, dwStyle(_dwStyle)
{
	dummyWindow=new CWnd(); 
	dummyWindow->Create(NULL, "dummy", WS_CHILD, CRect(0,0,100,100), GetDesktopWindow(), 101);
	_windsp = new zDisplay(dummyWindow);

	Init();
	fbs |= fbsSHOWALWAYS;
	feLastInserted = root; 
}

FormBaseWnd::~FormBaseWnd()
{
	if (_windsp)
	{
		delete _windsp;
		_windsp = 0;
	}
	if (dummyWindow)
	{
		delete dummyWindow;
		dummyWindow = 0;
	}
} 

void FormBaseWnd::CreateForm()
{
  TRACE0("FormBaseWnd::CreateForm() has to be overruled");
}

CWnd* FormBaseWnd::wnd()
{
  return this;
}

void FormBaseWnd::shutdown(int)
{
  GetParentFrame()->PostMessage(WM_CLOSE);
}

void FormBaseWnd::OnOK()
{
    FormEntry *pfe = CheckData();
    if (pfe) 
    {
        MessageBeep(MB_ICONEXCLAMATION);
        pfe->SetFocus();
				return;
    }
    _fOkClicked = true;
    if (fHideOnOk()) 
      GetParentFrame()->ShowWindow(SW_HIDE);
    exec();
    shutdown(IDOK);
}

afx_msg void FormBaseWnd::OnCancel()
{
    _fOkClicked = false;
    shutdown(IDCANCEL);
}

afx_msg void FormBaseWnd::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT dis )
{
    CWnd::OnDrawItem(nIDCtl, dis);
    root->CheckDrawItem(dis);
}

afx_msg void FormBaseWnd::OnSetFocus( CWnd* pOldWnd )
{
    ilwapp->setHelpItem(htp());
}

afx_msg BOOL FormBaseWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!CWnd::OnSetCursor(pWnd, nHitTest, message))
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return FALSE;
	}
	else
		return TRUE;
}

afx_msg void FormBaseWnd::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT mi )
{
	TEXTMETRIC tm;
	CWindowDC dc(CWnd::GetDesktopWindow());
	CFont *fntOld = dc.SelectObject(fnt);
	dc.GetTextMetrics(&tm);
	mi->itemHeight = tm.tmHeight;
	dc.SelectObject(fntOld);
	if (mi->itemHeight < 16)
		mi->itemHeight = 16;
}

void FormBaseWnd::OnHelp()
{
	IlwWinApp()->showHelp(help);
}

int FormBaseWnd::exec() 
{ 
  root->StoreData(); // fill destinations with current form entry values
  return 0; 
}  

void FormBaseWnd::create()
{
	DWORD dwExStyle = 0;
	CreateEx(dwExStyle, "IlwisView", "FormBaseWnd", dwStyle, CRect(0,0,0,0), par, 0);
    PRECONDITION(fINotEqual, _windsp, (zDisplay *)NULL);
    PRECONDITION(fINotEqual, root, (FormEntry *)NULL);

    SetCursor(LoadCursor(NULL, IDC_WAIT));
    ilwapp->setHelpItem(htp());
    try 
    {
    short i, j;
    root->psn->SetCol(0);
    for (i=0; i<20; i++) 
    {
        j = 0;
        root->psn->MaxWidthOfColumn(i, j);
    }
    root->psn->SetPos();
    if (npChanged() != 0)
        root->SetCallBackForAll(npChanged());
    root->create();
    // determine for size

    DWORD dwBaseUnits = GetDialogBaseUnits();
    WORD wBaseX = LOWORD(dwBaseUnits);
    WORD wBaseY = HIWORD(dwBaseUnits);
    zDimension dimOutSide(wBaseX * 5 / 4, wBaseY * 5 / 8);


    int iMaxX = root->psn->iWidth + root->psn->iBndLeft; // + root->psn->iBndRight;
    int iMaxY = root->psn->iHeight + root->psn->iBndUp + root->psn->iBndDown;

    // find widest of TR("Cancel"), TR("OK") and TR("Help")
    zDimension dimButton;

		CWindowDC dc(CWnd::GetDesktopWindow());
		CFont *fntOld;

    if (fnt != 0)
        fntOld = dc.SelectObject(fnt);
	zDimension d1;
	if ( fbs & fbsCancelHasCLOSETEXT)
		d1 = (zDimension)dc.GetTextExtent(TR("Close").c_str(), TR("Close").length());
	else
		d1 = (zDimension)dc.GetTextExtent(TR("Cancel").c_str(), TR("Cancel").length());
	zDimension d2;
	if ( fbs & fbsOKHASCLOSETEXT)
		d2 = (zDimension)dc.GetTextExtent(TR("Close").c_str(), TR("Close").length());
	else
		d2 = (zDimension)dc.GetTextExtent(TR("OK").c_str(), TR("OK").length());
	zDimension d3 = (zDimension)dc.GetTextExtent(TR("Help").c_str(), TR("Help").length());
    if (fnt != 0)
			dc.SelectObject(fntOld);
    dimButton = d1.width() > d2.width() ? d1 : d2;
    if (dimButton.width() < d3.width()) dimButton = d3;
    dimButton.width() = dimButton.width() + 20;
    dimButton.height() = dimButton.height() + 10;
    #define DISTBUT 8
    zDimension dimBut3;  // dimension for three buttons
    dimBut3.width() = dimButton.width() + 2 * dimOutSide.width();
    dimBut3.height() = 3 * dimButton.height() + 2 * ( DISTBUT + dimOutSide.height());

    String sHelp;
    help = htp();
    sHelp = TR("Help");
    bool fButtonsRight = (0 == (fbs & fbsBUTTONSUNDER));
    int iButtons = 0;
    if (0 == (fbs & fbsNOOKBUTTON)) iButtons += 1;     // OK
    if (0 == (fbs & fbsNOCANCELBUTTON)) iButtons += 1; // Cancel
    if (help != "") iButtons += 1;                      // Help

    zDimension dimForm;
	if (0 == iButtons) {
        dimForm.width() = iMaxX;
        dimForm.height() = iMaxY;
	} else if (fButtonsRight) 
    {
        // determine width of form entries next to buttons:
        int iMaxXBut = 0;
        // root->MaxX(&iMaxXBut, dimBut3.height()); // disabled !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // take maximum for size of form
        dimForm.width() = max(iMaxX, iMaxXBut + dimBut3.width());
        dimForm.height() = max(iMaxY, dimBut3.height());
    }
    else 
    {
        dimForm.width() = max(iMaxX, iButtons * dimButton.width() + (iButtons+1) * dimOutSide.width());
        dimForm.height() = iMaxY + dimButton.height() + dimOutSide.height();
    }
//    dimForm.height() += GetSystemMetrics(SM_CYDLGFRAME)*2;

	PositionWindow(dimForm);
      
    CPoint pntButton;
    if (fButtonsRight) 
    {
        pntButton.x = dimForm.width() - dimButton.width() - dimOutSide.width();
        pntButton.y = dimOutSide.height();
    }
    else 
    {
        pntButton.x = (dimForm.width() - iButtons * ( dimButton.width() + dimOutSide.width()/2)) - dimOutSide.width() / 2;
        pntButton.y = iMaxY;
    }

	if (butOK.GetSafeHwnd()==NULL)
		if ( fbs & fbsOKHASCLOSETEXT)
			butOK.Create(TR("Close").c_str(), BS_DEFPUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, IDOK);
		else		
			butOK.Create(TR("OK").c_str(), BS_DEFPUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, IDOK);  

    if (0 == (fbs & fbsNOOKBUTTON)) 
    {
        butOK.ShowWindow(SW_SHOWNA);
        butOK.SetFocus();
    }
    else 
    {
        butOK.ShowWindow(SW_HIDE);
        butOK.EnableWindow(FALSE);
    }
    butOK.SetFont(fnt);
  
    if (fButtonsRight) 
    {
        pntButton.y += dimButton.height() + DISTBUT;
    }
    else 
    {
        if (0 == (fbs & fbsNOOKBUTTON))
            pntButton.x += dimButton.width() + dimOutSide.width()/2;
    }

	if (butCancel.GetSafeHwnd() == NULL) {
		String txt = fbs & fbsApplyButton ? TR("Apply") : (( fbs & fbsCancelHasCLOSETEXT) ? TR("Close") : TR("Cancel"));
		UINT id = fbs & fbsApplyButton ? id_apply : IDCANCEL;
		butCancel.Create(txt.sVal(), BS_PUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, id); 
	}

    if (0 == (fbs & fbsNOCANCELBUTTON)) 
        butCancel.ShowWindow(SW_SHOW);
    else 
    {
        butCancel.ShowWindow(SW_HIDE);
        butCancel.EnableWindow(FALSE);;
    }
    butCancel.SetFont(fnt);
 
    if (help != "") 
    {
        if (fButtonsRight) 
        {
            pntButton.y += dimButton.height() + DISTBUT;
        } 
        else 
        {
            if (0 == (fbs & fbsNOCANCELBUTTON))
                pntButton.x += dimButton.width() + dimOutSide.width()/2;
        }
				if ( butHelp.GetSafeHwnd()==NULL)
					butHelp.Create(sHelp.sVal(), WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, IDHELP); 

        butHelp.SetFont(fnt);
    } 
		bool fBevel = (0 == (fbs & fbsNOBEVEL));
		if ( fBevel )
		{
			if ( fButtonsRight )
				bevel.Create(this, CRect(6, 8, 
				                   dimForm.width() - dimButton.width() - 1.5 * dimOutSide.width(), 
													 dimForm.height() - 
													 GetSystemMetrics(SM_CYDLGFRAME)*2 - GetSystemMetrics(SM_CYCAPTION) - 10),
													 true); 
	    else
				bevel.Create(this, CRect(6, 8,  dimForm.width() - 5, pntButton.y - 8), true); 
		}


    root->Show();
    root->CallCallBacks();
    FormEntry *pfe = feDefaultFocus();
    if (pfe) 
        pfe->SetFocus();
    SetCursor(LoadCursor(NULL, IDC_ARROW));

    if (!fShowAlways() && 0==pfe) 
    {
        _fOkClicked = true;
        exec();
        return;
    }  
    else 
    {
		//SetForegroundWindow();
		// ShowWindow(SW_SHOW);
		//SendMessage(WM_SETREDRAW, 1, 0);
		//InvalidateRect(0, true);
    }
 
    POSTCONDITION(fIGreater, dimForm.width(), 1L);
    POSTCONDITION(fIGreater, dimForm.height(), 1L);
		fEndInitialization = true;

    #undef DISTBUT
    }  
    catch (ErrorObject& err) 
    {
        _fOkClicked = false;
        err.Show();
        shutdown();
        return;
    }
}

void FormBaseWnd::RecalculateForm()
{
	RemoveChildren();
	_fOkClicked = false;
	CreateForm();
	FormBaseWnd::create();
  ShowWindow(SW_SHOW);
  SendMessage(WM_SETREDRAW, 1, 0);
  InvalidateRect(0, true);
}

BOOL FormBaseWnd::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	// allow tooltip messages to be filtered
	if (CWnd::PreTranslateMessage(pMsg))
		return TRUE;

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	// since 'IsDialogMessage' will eat frame window accelerators,
	//   we call all frame windows' PreTranslateMessage first
	pFrameWnd = GetParentFrame();   // start with first parent frame
	while (pFrameWnd != NULL)
	{
		// allow owner & frames to translate before IsDialogMessage does
		if (pFrameWnd->PreTranslateMessage(pMsg))
			return TRUE;

		// try parent frames until there are no parent frames
		pFrameWnd = pFrameWnd->GetParentFrame();
	}

	// filter both messages to dialog and from children
	return PreTranslateInput(pMsg);
} 

void FormBaseWnd::PositionWindow(const zDimension& dimForm)
{
	// determine current size of the client area as if no scrollbars present
	CRect rectClient;
	GetWindowRect(rectClient);
	CRect rect = rectClient;
	CalcWindowRect(rect);
	rectClient.left += rectClient.left - rect.left;
	rectClient.top += rectClient.top - rect.top;
	rectClient.right -= rect.right - rectClient.right;
	rectClient.bottom -= rect.bottom - rectClient.bottom;
	rectClient.OffsetRect(-rectClient.left, -rectClient.top);
	ASSERT(rectClient.left == 0 && rectClient.top == 0);

	// determine desired size of the view
	CRect rectView(0, 0, dimForm.width(), dimForm.height());
	CalcWindowRect(rectView, CWnd::adjustOutside);
	rectView.OffsetRect(-rectView.left, -rectView.top);
	ASSERT(rectView.left == 0 && rectView.top == 0);

	SetWindowText(sCaption.c_str());
	const CWnd* wTop = &wndTop;
	if (fbs & fbsTOPMOST)
		wTop = &wndTopMost;

	if (dwStyle & WS_CHILD) {
		SetWindowPos(wTop, 0, 0, rectView.Width() + 7 , rectView.Height(), SWP_NOMOVE | SWP_SHOWWINDOW);
	} else {
		CPoint pnt(100 + random(75) ,100 + random(25));
		pnt = rectClient.TopLeft() + pnt;
		SetWindowPos(wTop, pnt.x, pnt.y, rectView.Width() + 7 , rectView.Height(), SWP_SHOWWINDOW);
	}
}

bool FormBaseWnd::fDataHasChanged()
{
	return fDataChanged;
}

void FormBaseWnd::DataHasChanged(bool fV)
{
	fDataChanged = fV;
}

int FormBaseWnd::DataChanged(Event *)
{
	DataHasChanged(true);
	return 1;
}

void FormBaseWnd::apply() {
}