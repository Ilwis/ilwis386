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
/* FormBaseDialog
*/

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\StlExtensions.h"
#include "Client\Help\ChmFinder.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mainwind.hs"
#include "Engine\Base\Algorithm\Random.h"
#include "Headers\htmlhelp.h"
#include <afxpriv.h>
#include "Client\ilwis.h"

const UINT id_apply = 8503;

BEGIN_MESSAGE_MAP(FormBaseDialog, CDialog)
	ON_WM_DRAWITEM()
	ON_WM_SYSCOMMAND()
	ON_WM_MEASUREITEM()
	ON_COMMAND(IDHELP, OnHelp)
	ON_COMMAND(id_apply, apply)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_SIZE()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()    

const unsigned long defaultStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_3DLOOK;


FormBaseDialog::FormBaseDialog(CWnd* parent, const String& sTitle, bool fShowAlways, bool fMod, bool fHideOnOk)
: FormBase(parent, sTitle, fShowAlways, fMod, fHideOnOk)
, m_fResizable(false)
{
	iImg = 0;
	DWORD style = defaultStyle | ( fModal() ? DS_MODALFRAME : 0);
	if (CreateDialogTemplate(par, sTitle, style) == 0)
		throw ErrorObject("XXCould not create dialog");
	if (fModal() ) 
		Init();
}

FormBaseDialog::FormBaseDialog(CWnd* parent, const String& sTitle, int styl, DWORD extraWindowsStyles)
: FormBase(parent, sTitle, styl)
, m_fResizable((extraWindowsStyles & WS_THICKFRAME) != 0)
{
	iImg = 0;
	DWORD style = defaultStyle |  ( fModal() ? DS_MODALFRAME : 0);
	if ( extraWindowsStyles != 0)
		style |= extraWindowsStyles;
	if (CreateDialogTemplate(par, sTitle, style) == 0)
		throw ErrorObject("XXCould not create dialog");
	if (fModal() )
		Init();
}


FormBaseDialog::FormBaseDialog(CWnd* parent, const String& sTitle, ParmList *plDefault, bool fShowAlways, bool fMod, bool fHideOnOk)
: FormBase(parent, sTitle, plDefault, fShowAlways, fMod, fHideOnOk)
, m_fResizable(false)
{
	iImg = 0;
	DWORD style = defaultStyle |  ( fModal() ? DS_MODALFRAME : 0);
	if (CreateDialogTemplate(par, sTitle, style)==0)
		throw ErrorObject("XXCould not create dialog");
	if(fModal())
		Init();
}

FormBaseDialog::~FormBaseDialog()
{
	if (_windsp) // throw away dummies
	{ 
		delete _windsp;
		_windsp=NULL;
	}
	if (dummyWindow) // remove dummy window
	{
		dummyWindow->DestroyWindow();
		delete dummyWindow;
		dummyWindow=NULL;
	}
	if (!fModal())
		DestroyWindow();
}

CWnd* FormBaseDialog::wnd()
{
	return this;
}

bool FormBaseDialog::CreateDialogTemplate(CWnd* p, const String& sTitle, DWORD style)
{
	dlgFormBase.dlg.style = style;
	dlgFormBase.dlg.dwExtendedStyle = 0;
	dlgFormBase.dlg.cdit = 0;
	dlgFormBase.dlg.x = 0;
	dlgFormBase.dlg.y = 0;
	dlgFormBase.dlg.cx = 0;
	dlgFormBase.dlg.cy = 0;
	dlgFormBase.cMenu[0] = 0;
	dlgFormBase.cClass[0] = 0;
	dlgFormBase.cCaption[0] = 0;
	if ( fModal() )
		return InitModalIndirect(&dlgFormBase, p)!=0;
	return CreateIndirect(&dlgFormBase, p) !=0;
}

BOOL FormBaseDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_szinitialSize = CSize(0,0);

	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

	if (fModal())
	{
		if (_windsp) // dummies may be thrown away as they are no longer needed
		{ 
			delete _windsp;
			_windsp=NULL;
		}
		if (dummyWindow) // dummy window must be removed
		{
			dummyWindow->DestroyWindow();
			delete dummyWindow;
			dummyWindow=NULL;
		}
	}

	PRECONDITION(fINotEqual, m_hWnd, (HWND)0);

	feLastInserted = root; 
	SendMessage(WM_SETREDRAW, 0, 0);
	CMenu* SysMen = GetSystemMenu(FALSE);
	SetCallBack(0);
	if (SysMen) 
	{    
		int iCount = GetMenuItemCount(SysMen->m_hMenu);
		int iPos = 0;
		unsigned int id;
		for (int i = 0; i < iCount; ++i) 
		{
			id = GetMenuItemID(SysMen->m_hMenu,iPos);
			if (id == SC_MOVE || id == SC_CLOSE || (m_fResizable && (id == SC_SIZE))|| ((fbs & fbsCancelHasCLOSETEXT) && (id == SC_MINIMIZE || id == SC_RESTORE))) // fbsCancelHasCLOSETEXT is from Geonetcast Toolbox
				iPos += 1;
			else
				DeleteMenu(SysMen->m_hMenu, iPos, MF_BYPOSITION);
		}
		SysMen->AppendMenu(MF_SEPARATOR, -1);
		SysMen->AppendMenu( MF_STRING, ID_COPY, TR("Copy").c_str());
		SysMen->AppendMenu( MF_STRING, ID_PRINT, TR("Print").c_str());
	}

	_windsp = new zDisplay(this);
	ShowWindow(SW_HIDE);
	if (!fModal()) //modeless dialogs must call Init now, modal will call it after creation
		Init();
	else
		CreateDefaultPositions();
	POSTCONDITION(fINotEqual, m_hWnd, (HWND)0);

	return TRUE;
}

void FormBaseDialog::create()
{
	if (fModal())
		DoModal();
	else
		CreateDefaultPositions();
}

afx_msg void FormBaseDialog::OnOK()
{
	if ( fNoCheckData == false) {
		FormEntry *pfe = CheckData();
		if (pfe) 
		{
			MessageBeep(MB_ICONEXCLAMATION);
			pfe->SetFocus();
			return;
		}
	}
	_fOkClicked = true;
	exec();		
	if (fHideOnOk()) 
		ShowWindow(SW_HIDE);
	if ( fbs & fbsHIDEONCLOSE )
	{
		ShowWindow(SW_HIDE);
		return;
	}			
	shutdown(IDOK);
}

afx_msg void FormBaseDialog::OnCancel()
{
	_fOkClicked = false;
	shutdown(IDCANCEL);
}


afx_msg void FormBaseDialog::OnSysCommand( UINT nID, LPARAM lParam )
{
	if (nID == ID_COPY)
		Copy();
	else if (nID == ID_PRINT)
		Print();

	CDialog::OnSysCommand(nID, lParam);
}

afx_msg void FormBaseDialog::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT mi )
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

afx_msg void FormBaseDialog::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT dis )
{
	CWnd::OnDrawItem(nIDCtl, dis);
	root->CheckDrawItem(dis);
}

afx_msg void FormBaseDialog::OnSetFocus( CWnd* pOldWnd )
{
	ilwapp->setHelpItem(htp());
}

void FormBaseDialog::shutdown(int iReturn)
{
	if (fModal())
		EndDialog(iReturn);
	else
		delete this;
	//DestroyWindow();
}

void FormBaseDialog::CreateDefaultPositions()
{
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


		CRect rct(0,0,3,5);
		MapDialogRect(&rct); // Outside area
		zDimension dimOutSide=rct.Size();

		int iMaxX = root->psn->iWidth + root->psn->iBndLeft ;//+ root->psn->iBndRight;
		int iMaxY = root->psn->iHeight + root->psn->iBndUp + root->psn->iBndDown / 2;

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
		if (fbs & fbsAPPLIC) {
			zDimension dShow = (zDimension)dc.GetTextExtent(TR("&Show").c_str(), TR("&Show").length());
			zDimension dDef = (zDimension)dc.GetTextExtent(TR("&Define").c_str(), TR("&Define").length());
			if (fbs & fbsCALC) 
				dDef = (zDimension)dc.GetTextExtent(TR("&Calculate").c_str(), TR("&Calculate").length());
			if (dimButton.width() < dShow.width()) dimButton = dShow;
			if (dimButton.width() < dDef.width()) dimButton = dDef;
		}
		dimButton.width() = dimButton.width() + 20;
		dimButton.height() = dimButton.height() + 8;
		const int DISTBUT = 8;
		zDimension dimBut3;  // dimension for three buttons
		if ((fbs & fbsNOOKBUTTON) && (fbs & fbsNOCANCELBUTTON))
			dimButton.cx = dimButton.cy = 0;

		dimBut3.width() = dimButton.width() + 2 * dimOutSide.width();
		dimBut3.height() = 3 * dimButton.height() + 2 * ( DISTBUT + dimOutSide.height());

		String sHelp;
		sHelp = TR("Help");
	
		bool fButtonsRight = (0 == (fbs & fbsBUTTONSUNDER));
		int iButtons = 0;
		if (0 == (fbs & fbsNOOKBUTTON)) iButtons += 1;     // OK
		if (0 == (fbs & fbsNOCANCELBUTTON)) iButtons += 1; // Cancel
		if (0 != (fbs & fbsAPPLIC)) iButtons = 3;          // Application: Show, Define, Cancel
		if (help != "" ) iButtons += 1;                      // Help

		zDimension dimForm;
		if (fButtonsRight) 
		{
			// determine width of form entries next to buttons:
			int iMaxXBut = 0;
			root->MaxX(&iMaxXBut, dimBut3.height());
			// take maximum for size of form
			dimForm.width() = max(iMaxX + 5, iMaxXBut + dimBut3.width() + 5);
			dimForm.height() = max(iMaxY, dimBut3.height());
		}
		else 
		{
			dimForm.width() = max(iMaxX + 8, iButtons * dimButton.width() + (iButtons+1) * dimOutSide.width() + 8);
			dimForm.height() = iMaxY + dimButton.height() + dimOutSide.height();
		}
		dimForm.height() += GetSystemMetrics(SM_CYDLGFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);

		// set position of form
		CPoint pnt(100 + random(75) ,100 + random(25));
		CRect wrect;
		GetWindowRect(&wrect);
		if (0 != par) 
			pnt = wrect.TopLeft() + pnt;

		//int pixWidth=dc.GetDeviceCaps(HORZRES);
		//int pixHeight=dc.GetDeviceCaps(VERTRES);
		CRect rectWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);
		int pixHeight = rectWorkArea.bottom; 
		int pixWidth = rectWorkArea.right; 

		if (pnt.x + dimForm.width() + 10 > pixWidth)
			pnt.x = max(0, pixWidth-10-dimForm.width());
		if (pnt.y + dimForm.height() + 20 > pixHeight)
			pnt.y = max(0, pixHeight-20-dimForm.height());

		CPoint pntButton;
		if (fButtonsRight) 
		{
			pntButton.x = dimForm.width() - dimButton.width() - dimOutSide.width();
			pntButton.y = dimOutSide.height();
		}
		else 
		{
			pntButton.x = (dimForm.width() - iButtons * (dimButton.width() + dimOutSide.width()/2)) - dimOutSide.width() / 2;
			pntButton.y = iMaxY;
		}

		if (fbs & fbsAPPLIC) {
			if (butShow.GetSafeHwnd()==NULL)
				butShow.Create(TR("&Show").c_str(), BS_DEFPUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, IDYES);  
			pntButton.x += dimButton.width() + dimOutSide.width()/2;
			if (butDefine.GetSafeHwnd()==NULL)
				if (fbs & fbsCALC)
					butDefine.Create(TR("&Calculate").c_str(), BS_PUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, IDNO); 
				else
					butDefine.Create(TR("&Define").c_str(), BS_PUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, IDNO);  
			butShow.ShowWindow(SW_SHOWNA);
			butShow.SetFocus();
			butShow.SetFont(fnt);
			butDefine.ShowWindow(SW_SHOWNA);
			butDefine.SetFont(fnt);
		}
		else 
		{
			if (butOK.GetSafeHwnd()==NULL)
				if ( fbs & fbsOKHASCLOSETEXT)
					butOK.Create(TR("Close").c_str(), BS_DEFPUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, IDOK);
				else		
					butOK.Create(TR("OK").c_str(), BS_DEFPUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, IDOK);  

			if (0 == (fbs & fbsNOOKBUTTON)) 
			{
				butOK.ShowWindow(SW_SHOWNA	);
				//butOK.SetFocus();
			}
			else 
			{
				butOK.ShowWindow(SW_HIDE);
				butOK.EnableWindow(FALSE);;
			}
			butOK.SetFont(fnt);
		}

		if (fButtonsRight) 
		{
			pntButton.y += dimButton.height() + DISTBUT;
		}
		else 
		{
			if (0 == (fbs & fbsNOOKBUTTON))
				pntButton.x += dimButton.width() + dimOutSide.width()/2;
		}

		if (0 == (fbs & fbsNOCANCELBUTTON)) {
			if (butCancel.GetSafeHwnd() == NULL){
				String txt = fbs & fbsApplyButton ? TR("Apply") : (( fbs & fbsCancelHasCLOSETEXT) ? TR("Close") : TR("Cancel"));
				UINT id = fbs & fbsApplyButton ? id_apply : IDCANCEL;
				butCancel.Create(txt.sVal(), BS_PUSHBUTTON|WS_TABSTOP, CRect(pntButton, dimButton), this, id); 
			}
			butCancel.ShowWindow(SW_SHOW);
			butCancel.SetFont(fnt);
		} 
		else 
		{
			if (butCancel.GetSafeHwnd() != NULL) {
				butCancel.ShowWindow(SW_HIDE);
				butCancel.EnableWindow(FALSE);;
			}
		}

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
				dimForm.width() - dimButton.width() - 1.5 * dimOutSide.width() + 5, 
				dimForm.height() - 
				GetSystemMetrics(SM_CYDLGFRAME)*2 - GetSystemMetrics(SM_CYCAPTION) - 10),
				true); 
			else
				bevel.Create(this, CRect(6, 8,  dimForm.width() - 4, pntButton.y - 5), true); 
		}

		root->Show();
		root->CallCallBacks();
		FormEntry *pfe = feDefaultFocus();

		SetCursor(LoadCursor(NULL, IDC_ARROW));

		if (!fShowAlways() && 0==pfe) 
		{
			_fOkClicked = true;
			exec();
			shutdown(IDOK);
			return;
		}  
		SetWindowText(sCaption.c_str());
		const CWnd* wTop = &wndTop;
		if (fbs & fbsTOPMOST)
			wTop = &wndTopMost;
		if (m_fResizable)
			dimForm += CSize(10,10); // THICKFRAME
		SetWindowPos(wTop, pnt.x, pnt.y, dimForm.width() + 7 , dimForm.height(), SWP_SHOWWINDOW);
		SetForegroundWindow();
		SendMessage(WM_SETREDRAW, 1, 0);
		InvalidateRect(0, true);
		//deleteOnClose(!fModal());

		if (pfe) 
			pfe->SetFocus();
		else if (butOK.GetSafeHwnd())
			butOK.SetFocus();
		else if (butShow.GetSafeHwnd())
			butShow.SetFocus();

		POSTCONDITION(fIGreater, dimForm.width(), 1L);
		POSTCONDITION(fIGreater, dimForm.height(), 1L);
		fEndInitialization = true; // usefull to prevent callbacks

	}  
	catch (ErrorObject& err) 
	{
		_fOkClicked = false;
		err.Show();
		shutdown();
		return;
	}
	if ( m_fResizable )
		AddControls();
}

void FormBaseDialog::OnHelp()
{
	IlwWinApp()->showHelp(help);
}

LRESULT FormBaseDialog::OnCommandHelp(WPARAM, LPARAM lParam)
{
	OnHelp();
	return TRUE;
}

bool NoAmpers(char c)
{
	return c != '&';
}

void FormBaseDialog::Copy()
{
	//    zClipboard cb(this);
	//    cb.clear();
	//    String sTxt = sText();
	//    // remove & from text
	//		String sOut(' ', sTxt.size());
	//    copy_if(sTxt.begin(), sTxt.end(), sOut.begin(), NoAmpers);
	//    cb.add(sOut.sVal());
	FormBase::Copy();
}

void FormBaseDialog::Print()
{
	FormBase::Print();
}

LRESULT FormBaseDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	try {	
		return CDialog::WindowProc(message, wParam, lParam);
	}
	catch (ErrorObject& err) {
		err.Show();
	}
	catch(CException* err)
	{
		MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
		err->Delete();
	}
	return DefWindowProc(message, wParam, lParam);
}

void FormBaseDialog::apply() {
}

void FormBaseDialog::addResizableFormEntry(FormEntry * fe) {
	m_resizableFormEntries.push_back(fe);
}

void FormBaseDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (!m_fResizable)
		return;
	if (!root || !root->frm()->wnd())
		return;

	if ( m_szinitialSize == CSize(0,0) ) {
		CRect rect;
		GetClientRect(&rect);
		m_szinitialSize = CSize(rect.Width(), rect.Height());
	} else if ( (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED) && cx > 0 && cy > 0 ) {
		CRect rect;
		GetClientRect(&rect);
		updateControls(rect.Width(), rect.Height());
	}
}

void FormBaseDialog::WidthOfResizableColumn(FormEntry * fe, short iColNr, short& iWidth)
// gets the maximum of the minimum width values of each positioner in column iColNr, of the resizable FormElements
// then check it's child positioners
{
    if ((fe->psn->iCol == iColNr) && (fe->psn->iMinWidth > iWidth) && (find(m_resizableFormEntries.begin(), m_resizableFormEntries.end(), fe) != m_resizableFormEntries.end())) 
        iWidth = fe->psn->iMinWidth;
	for (short i = 0; i < fe->childlist().iSize(); i++)
		WidthOfResizableColumn(fe->childlist()[i], iColNr, iWidth);
}

void FormBaseDialog::HeightOfResizableColumn(FormEntry * fe, short iColNr, short& iHeight)
// gets the maximum of the minimum height values of each positioner in column iColNr, of the resizable FormElements
// then check it's child positioners
{
    if ((fe->psn->iCol == iColNr) && (find(m_resizableFormEntries.begin(), m_resizableFormEntries.end(), fe) != m_resizableFormEntries.end())) 
        iHeight += fe->psn->iMinHeight;
	for (short i = 0; i < fe->childlist().iSize(); i++)
		HeightOfResizableColumn(fe->childlist()[i], iColNr, iHeight);
}

void FormBaseDialog::updateControls(int cx, int cy)
{
	if ( m_controls.size() == 0 )
		return;
	if ( !root || !root->frm()->wnd() )
		return;
	if ( m_szinitialSize.cx <= 0 || m_szinitialSize.cy <= 0 )
		return;

	int iTotalXScalable = 0;
	int iTotalYScalable = 0;

	for (short i=0; i<20; i++) {
		short iWidth = 0;
		short iHeight = 0;
		WidthOfResizableColumn(root, i, iWidth);
		HeightOfResizableColumn(root, i, iHeight);
		iTotalXScalable += iWidth;
		iTotalYScalable = max(iTotalYScalable,iHeight);
	}

	for (std::vector<FormEntry*>::iterator it = m_resizableFormEntries.begin(); it != m_resizableFormEntries.end(); ++it) {
		(*it)->psn->iWidth = (*it)->psn->iMinWidth + (cx - m_szinitialSize.cx) * (*it)->psn->iMinWidth / iTotalXScalable;
		(*it)->psn->iHeight = (*it)->psn->iMinHeight + (cy - m_szinitialSize.cy) * (*it)->psn->iMinHeight / iTotalYScalable;
	}

	root->psn->SetPos();
	UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
	for (std::map<HWND,FormEntry*>::iterator it = m_controls.begin(); it != m_controls.end(); ++it) {
		HWND hwnd = it->first;
		FormEntry* fe = it->second;
		if (find(m_resizableFormEntries.begin(), m_resizableFormEntries.end(), fe) != m_resizableFormEntries.end()) {
			::SetWindowPos(hwnd, 0, fe->psn->iPosX, fe->psn->iPosY, fe->psn->iWidth, fe->psn->iHeight, flags);
		} else if (dynamic_cast<StaticTextSimple*>(fe) != 0) {
			zDimension dimTxt( fe->psn->iWidth, fe->psn->iMinHeight );
			CPoint pntTxt(fe->psn->iPosX, fe->psn->iPosY);
			pntTxt.y += (fe->psn->iHeight - dimTxt.height()) / 2; // center vertically
			::SetWindowPos(hwnd, 0, pntTxt.x, pntTxt.y, dimTxt.width(), dimTxt.height(), flags);
		} else {
			CRect rect;
			::GetWindowRect(hwnd,&rect);
			::SetWindowPos(hwnd, 0, fe->psn->iPosX, fe->psn->iPosY, rect.Width(), rect.Height(), flags);
		}
	}

	bool fBevel = (0 == (fbs & fbsNOBEVEL));
	if (fBevel) {
		CRect rect = rectBevel;
		rect.right = rectBevel.right + cx - m_szinitialSize.cx;
		rect.bottom = rectBevel.bottom + cy - m_szinitialSize.cy;
		::SetWindowPos(bevel.m_hWnd, 0, rect.left, rect.top, rect.Width(), rect.Height(), flags);
	}
	for (std::map<CWnd*,CRect>::iterator it = rectButtons.begin(); it != rectButtons.end(); ++it) {
		CRect rect = it->second;
		rect.left += cx - m_szinitialSize.cx;
		rect.right += cx - m_szinitialSize.cx;
		rect.top += cy - m_szinitialSize.cy;
		rect.bottom += cy - m_szinitialSize.cy;
		::SetWindowPos(it->first->m_hWnd, 0, rect.left, rect.top, rect.Width(), rect.Height(), flags);
	}

	InvalidateRect(0, true);
}

bool FormBaseDialog::AddControls()
{
	if (!root || !root->frm()->wnd())
		return false;
	CWnd* cwnd = root->frm()->wnd();
	CWnd* pChildWnd = cwnd->GetWindow(GW_CHILD);
	while( pChildWnd != NULL && ::IsWindow(pChildWnd->m_hWnd) ) {
		AddControl(pChildWnd);
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
	return true;
}

FormEntry * FormBaseDialog::FindFormEntry(FormEntry * fe, const CRect & rect, bool fStrict) {
	if (rect.left == fe->psn->iPosX && (fStrict ? (rect.top == fe->psn->iPosY) : (abs(rect.top - fe->psn->iPosY) < 10)) && (rect.Width() == fe->psn->iWidth || rect.Width() == fe->psn->iMinWidth))
		return fe;
	else {
		Array<FormEntry*>& children = fe->childlist();
		for (int i = 0; i < children.size(); ++i) {
			FormEntry * feChild = FindFormEntry(children[i], rect, fStrict);
			if (feChild != 0)
				return feChild;
		}
	}
	return 0;
}

bool FormBaseDialog::AddControl(CWnd* pControl)
{
	ASSERT( pControl != NULL );

	CWnd* cwnd = root->frm()->wnd();
	if (!cwnd)
		return false;

	if ( !::IsWindow(pControl->m_hWnd) )
		return false;

	CRect rect;
	pControl->GetWindowRect(&rect);
	cwnd->ScreenToClient(&rect);

	map<HWND,FormEntry*>::iterator result = m_controls.find(pControl->m_hWnd);
	if ( result == m_controls.end() ) {
		if (pControl == &bevel) {
			rectBevel = rect;
		} else if (pControl == &butOK || pControl == &butCancel || pControl == &butDefine || pControl == &butHelp || pControl == &butShow) {
			rectButtons[pControl] = rect;
		} else {
			FormEntry * fe = FindFormEntry(root,rect,true);
			if (fe == 0)
				fe = FindFormEntry(root,rect,false); // StaticText is repositioned to have the text centered vertically
			if (fe != 0)
				m_controls.insert(std::pair<HWND,FormEntry*>(pControl->m_hWnd,fe));
		}
	}

	return true;
}
