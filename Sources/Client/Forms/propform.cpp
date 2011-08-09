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
#include "Client\Base\ilwis30.h"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\Userint.hs"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Headers\Htp\Ilwis.htp"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Engine\Base\StlExtensions.h"
#include <afxdisp.h>        // MFC Automation classes
#include "Client\Forms\PropForm.h"

CPoint IlwisPropertySheet::m_cpTopLeft = CPoint(0, 0);

BEGIN_MESSAGE_MAP(IlwisPropertySheet, CPropertySheet)
	ON_WM_SYSCOMMAND()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COPY, Copy)
	ON_COMMAND(ID_PRINT, Print)
END_MESSAGE_MAP()    

// Class IlwisPropertySheet
IlwisPropertySheet::IlwisPropertySheet(const String& sCaption)
	: CPropertySheet(sCaption.c_str())
{
}

BOOL IlwisPropertySheet::OnInitDialog()
{
	BOOL fResult = CPropertySheet::OnInitDialog();

	HICON hIconLogo = IlwWinApp()->LoadIcon("LogoIcon");
	SetIcon(hIconLogo, TRUE);

	int iImg = IlwWinApp()->iImage("ilwis");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco, FALSE);

	CMenu* SysMen = GetSystemMenu(FALSE);
	if (SysMen)
	{
		SysMen->AppendMenu(MF_SEPARATOR, -1);
		SysMen->AppendMenu(MF_STRING, ID_COPY,  TR("Copy").c_str());
		SysMen->AppendMenu(MF_STRING, ID_PRINT, TR("Print").c_str());
	}

	CRect rect;
	GetWindowRect(&rect);
	rect = CRect(cpTopLeftNew(rect.Size()), rect.Size());
		MoveWindow(&rect);

	return fResult;
}

void IlwisPropertySheet::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (point.x == -1 && point.y == -1) 
		return;

	CMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu(MF_STRING, ID_COPY,  TR("Copy").c_str());
	menu.AppendMenu(MF_STRING, ID_PRINT, TR("Print").c_str());

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	menu.Detach();
}

void IlwisPropertySheet::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == ID_COPY)
		CopyAllPages();
	else if (nID == ID_PRINT)
		PrintAllPages();

	CPropertySheet::OnSysCommand(nID, lParam);
}

void IlwisPropertySheet::Copy()
{
	FormBasePropertyPage *page = (FormBasePropertyPage*)GetActivePage();
	if (page)
		page->Copy();
}

void IlwisPropertySheet::Print()
{
	FormBasePropertyPage *page = (FormBasePropertyPage*)GetActivePage();
	if (page)
		page->Print();
}
  
bool fNoAmpers(char c)
{
	return c != '&';
}

void IlwisPropertySheet::CopyAllPages()
{
	if (GetPageCount() <= 0)
		return;

	zClipboard cb(this);
    cb.clear();

	String sTxt;
	for (int i = 0; i < GetPageCount(); i++)
	{
		FormBasePropertyPage *page = (FormBasePropertyPage*)GetPage(i);
		if (page)
		{
			sTxt &= String("\r\n%S %s\r\n", TR("Page:"), page->m_psp.pszTitle);
			sTxt &= page->sText();
		}
	}
	// remove & from text
	String sOut(' ', sTxt.size());
	copy_if(sTxt.begin(), sTxt.end(), sOut.begin(), fNoAmpers);

    cb.add(sOut.sVal());
}

void IlwisPropertySheet::PrintAllPages()
{
	if (GetPageCount() <= 0)
		return;

	String sTxt;
	for (int i = 0; i < GetPageCount(); i++)
	{
		FormBasePropertyPage *page = (FormBasePropertyPage*)GetPage(i);
		if (page)
		{
			sTxt &= String("\r\n%S %s\r\n", TR("Page:"), page->m_psp.pszTitle);
			sTxt &= page->sText();
		}
	}
	// remove & from text
	String sOut(' ', sTxt.size());
	replace(sTxt.begin(), sTxt.end(), '\t', ' '); // replace \t
	copy_if(sTxt.begin(), sTxt.end(), sOut.begin(), fNoAmpers);

	// The printer code has been added here, to avoid printing
	// the info of each property page on a separate page
	CPrintDialog dlg(FALSE); 
	dlg.DoModal();
	HDC pdc = dlg.GetPrinterDC();
	if ( pdc == 0 )
		return;

	CDC dcPrinter;
	dcPrinter.Attach(dlg.GetPrinterDC());
	// call StartDoc() to begin printing   
	DOCINFO docinfo;
	memset(&docinfo, 0, sizeof(docinfo));   
	docinfo.cbSize = sizeof(docinfo);

	String sPrtFrom = TR("Printing Form");
	docinfo.lpszDocName = _T(sPrtFrom.c_str());
	// if it fails, complain and exit gracefully
	if (dcPrinter.StartDoc(&docinfo) < 0)   
	{
		throw ErrorObject(TR("Printer wouldn't initalize").c_str());   
	}
	else
	{
		// start a page      
		if (dcPrinter.StartPage() < 0)      
		{
			MessageBox(_T(TR("Could not start page").c_str()));         
			dcPrinter.AbortDoc();
		}
		else      
		{         // actually do some printing
			CFont *oldfnt, *fnt = ILWISAPP->GetFont(IlwisWinApp::sfFORM);
			CFont pointFont;
			pointFont.CreatePointFont(120, "MS Sans Serif", &dcPrinter);

			oldfnt = dcPrinter.SelectObject(&pointFont);
			int i=0;
			CSize sz = dcPrinter.GetTextExtent("gk");

			while(sOut != "")
			{
				String sHead = sOut.sHead("\r");
				dcPrinter.TextOut(5 * sz.cx ,( 5 + i ) * sz.cy , sHead.c_str(), sHead.size());
				sOut = sOut.sTail("\n");
				++i;
			}
			dcPrinter.EndPage();         
			dcPrinter.EndDoc();
			dcPrinter.SelectObject(oldfnt);
		}   
	}
}
  
CPoint IlwisPropertySheet::cpTopLeftNew(const CSize& size)
{
	long iMove = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER);
	long iXScreen = GetSystemMetrics(SM_CXFULLSCREEN);
	long iYScreen = GetSystemMetrics(SM_CYFULLSCREEN);

	m_cpTopLeft.Offset(iMove, iMove);
	if (m_cpTopLeft.y + size.cy >= iYScreen || m_cpTopLeft.x + size.cx >= iXScreen)
		m_cpTopLeft = CPoint(iMove, iMove);

	return m_cpTopLeft;
}

bool IlwisPropertySheet::CheckPosition(CRect& rect)
{
	long iMove = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER);
	long iXScreen = GetSystemMetrics(SM_CXFULLSCREEN);
	long iYScreen = GetSystemMetrics(SM_CYFULLSCREEN);
	long iX = rect.left;
	long iY = rect.top;
	long iWidth = rect.Width();
	long iHeight = rect.Height();

	bool fChanged = false;

	CWnd* pw = WindowFromPoint(CPoint(iX,iY));
	// Is there a window at the top left position of this window?
	while ( pw != 0 )
	{
		pw = WindowFromPoint(CPoint(iX,iY) + CSize(iMove, iMove));

		if (pw != 0 )
		{
			CRect rectBack;
			pw->GetWindowRect(&rectBack);
			rectBack.SetRect(rectBack.TopLeft(), rectBack.TopLeft());
			rectBack.InflateRect(iMove, iMove);
			if (rectBack.PtInRect(CPoint(iX, iY))) 
			{
				fChanged = true;
				iX = iX + iMove < iXScreen ? iX + iMove : iMove;
				iY = iY + iMove < iYScreen ? iY + iMove : iMove;
				continue;
			}
		}
		break;
	}
	if (fChanged)
		rect.SetRect(iX, iY, iX + iWidth, iY + iHeight);

	return fChanged;
}
