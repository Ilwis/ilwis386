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
// FormBasePropertyPage.cpp: implementation of the FormBasePropertyPage class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"
#include "Headers\Htp\Ilwis.htp"
#include "prsht.h" 
#include "Headers\Hs\Userint.hs"

BEGIN_MESSAGE_MAP(FormBasePropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(FormBasePropertyPage)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

FormBasePropertyPage::FormBasePropertyPage(const string& sTitle)
{
	SetHelpTopic(htpContentsIlwis);
	dummyWindow=new CWnd(); 
	dummyWindow->Create(NULL, "dummy", WS_CHILD, CRect(0,0,100,100), GetDesktopWindow(), 101);
	_windsp = new zDisplay(dummyWindow);
	Construct((LPCTSTR)0);
	m_strCaption = sTitle.c_str();
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;

	dlgFormBase.dlg.style = WS_CHILD;
	dlgFormBase.dlg.dwExtendedStyle = 0;
	dlgFormBase.dlg.cdit = 0;
	dlgFormBase.dlg.x = 0;
	dlgFormBase.dlg.y = 0;
	dlgFormBase.dlg.cx = 0;
	dlgFormBase.dlg.cy = 0;
	dlgFormBase.cMenu[0] = 0;
	dlgFormBase.cClass[0] = 0;
	dlgFormBase.cCaption[0] = 0;
	m_psp.dwFlags |= PSP_DLGINDIRECT;
	m_psp.pResource = (DLGTEMPLATE*)&dlgFormBase;

	Init();
	root->psn->SetBound(12, 12, 12, 12);
}

FormBasePropertyPage::~FormBasePropertyPage()
{
	if (_windsp) // cleanup dummies
  { 
    delete _windsp;
    _windsp=NULL;
  }
  if (dummyWindow) // cleanup dummy window
  {
		dummyWindow->DestroyWindow();
    delete dummyWindow;
    dummyWindow=NULL;
  }
}

CWnd* FormBasePropertyPage::wnd()
{
	return this;
}

void FormBasePropertyPage::create()
{
  PRECONDITION(fINotEqual, _windsp, (zDisplay *)NULL);
  PRECONDITION(fINotEqual, root, (FormEntry *)NULL);

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

		int iMaxX = root->psn->iWidth + root->psn->iBndLeft; 
		int iMaxY = root->psn->iHeight + root->psn->iBndUp;

		LONG dbu = GetDialogBaseUnits();
		int baseunitX = LOWORD(dbu) - 2;
		int baseunitY = HIWORD(dbu) - 4;
		dlgFormBase.dlg.cx = (iMaxX * 4) / baseunitX;
		dlgFormBase.dlg.cy = (iMaxY * 8) / baseunitY;


	}  
  catch (ErrorObject& err) 
  {
    _fOkClicked = false;
    err.Show();
    return;
  }
}

void FormBasePropertyPage::OnOK()
{
  FormEntry *pfe = CheckData();
  if (pfe) 
  {
    MessageBeep(MB_ICONEXCLAMATION);
    pfe->SetFocus();
  }
  _fOkClicked = true;
  exec();
}

BOOL FormBasePropertyPage::OnApply()
{
  FormEntry *pfe = CheckData();
  if (pfe) 
  {
    MessageBeep(MB_ICONEXCLAMATION);
    pfe->SetFocus();
  }
  _fOkClicked = true;
  _fApplyClicked = true;
  exec();
  
  return TRUE;  // Accept changes
}

int FormBasePropertyPage::exec()
{ 
  root->StoreData(); // fill destinations with current form entry values
  return 0; 
}  

int FormBasePropertyPage::Copy()
{
	String sPageTitle = String("\r\n%S %s\r\n", SUIMsgPage, m_psp.pszTitle);
	return FormBase::Copy(sPageTitle);
}

int FormBasePropertyPage::Print()
{
	String sPageTitle = String("\r\n%S %s\r\n", SUIMsgPage, m_psp.pszTitle);
	return FormBase::Print(sPageTitle);
}
  
void FormBasePropertyPage::OnCancel()                             
{
	_fOkClicked = false;
}

BOOL FormBasePropertyPage::OnKillActive()
{
	return fValidData();
}

void FormBasePropertyPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT dis)
{
  CPropertyPage::OnDrawItem(nIDCtl, dis);
  root->CheckDrawItem(dis);
}

void FormBasePropertyPage::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT mi)
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

BOOL FormBasePropertyPage::OnInitDialog()
{
	CWaitCursor curWait;
  CPropertyPage::OnInitDialog();

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
  feLastInserted = root; 
  _windsp = new zDisplay(this);

  root->SetCallBackForAll((NotifyProc)&FormBasePropertyPage::DataChanged);
	root->create();
  root->Show();
  root->CallCallBacks();
  FormEntry *pfe = feDefaultFocus();
  if (pfe) 
    pfe->SetFocus();
	SetModified(false);

	return TRUE;
}

void FormBasePropertyPage::shutdown(int)
{
}

int FormBasePropertyPage::DataChanged(Event*)
{
	SetModified();
	return 0;
}

void FormBasePropertyPage::OnHelp()
{
	String sModName = htp().sModName;
	String sHelpFile;
	if (sModName != "")
		sHelpFile = sModName;
	else
		sHelpFile = "ilwis.chm";

	sHelpFile = ChmFinder::sFindChmFile(sHelpFile);

	::HtmlHelp(::GetDesktopWindow(), sHelpFile.sVal(), HH_HELP_CONTEXT, abs((int)htp().iTopic));
}

BEGIN_MESSAGE_MAP(FormBaseWizardPage, FormBasePropertyPage)
	//{{AFX_MSG_MAP(FormBaseWizardPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

FormBaseWizardPage::FormBaseWizardPage(const string& sTitle) 
: FormBasePropertyPage(sTitle), fEnable(true), ppPrevious(0) 
{
}

FormBaseWizardPage::~FormBaseWizardPage()
{
}

LRESULT FormBaseWizardPage::OnWizardBack()
{
  _fOkClicked = false;

	CPropertySheet* ps = (CPropertySheet*)GetParent();
	int iP = ps->GetPageIndex(this);
  for (int i=iP-1; i >= 0; i--) {
		FormBaseWizardPage* pp = dynamic_cast<FormBaseWizardPage*>(ps->GetPage(i));
		if (pp->fPPEnabled()) {
			ps->SetActivePage(pp);
			break;
		}
	}	
  return -1;
}

LRESULT FormBaseWizardPage::OnWizardNext()
{
  FormEntry *pfe = CheckData();
  if (pfe) 
  {
    MessageBeep(MB_ICONEXCLAMATION);
    pfe->SetFocus();
		return -1;
  }
  _fOkClicked = true;
  exec();
	
	CPropertySheet* ps = (CPropertySheet*)GetParent();
	int iP = ps->GetPageIndex(this);
  for (int i=iP+1; i < ps->GetPageCount(); i++) {
		FormBaseWizardPage* pp = dynamic_cast<FormBaseWizardPage*>(ps->GetPage(i));
		if (pp->fPPEnabled()) {
			ps->SetActivePage(pp);
			break;
		}
	}	
  return -1;
}

BOOL FormBaseWizardPage::OnWizardFinish()
{
  FormEntry *pfe = CheckData();
  if (pfe) 
  {
    MessageBeep(MB_ICONEXCLAMATION);
    pfe->SetFocus();
		return FALSE;
  }
  _fOkClicked = true;
  exec();
	return TRUE;	
}

BOOL FormBaseWizardPage::OnSetActive()
{
	FormBasePropertyPage::OnSetActive();
	CPropertySheet* ps = (CPropertySheet*)GetParent();
	int iP = ps->GetPageIndex(this);
	if (0 == iP)
		ps->SetWizardButtons(PSWIZB_NEXT); 
	else if (ps->GetPageCount() == iP + 1)
		ps->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH); 
	else
		ps->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT); 
  root->CallCallBacks();
	return TRUE;
}

BOOL FormBaseWizardPage::OnKillActive()
{
	return TRUE; // validation is done in an other place
}

void FormBaseWizardPage::EnablePreviousPage()
{
	if ( ppPrevious )
	{
		SetPPDisable();
		ppPrevious->SetPPEnable();
		CPropertySheet* ps = (CPropertySheet*)GetParent();
		ps->SetActivePage(ppPrevious)	;		
	}		
}

void FormBaseWizardPage::SetPreviousPage(FormBaseWizardPage *page, bool fEnable)
{
	ppPrevious = page;
	if ( fEnable && ppPrevious )
	{
		EnablePreviousPage();
	}		
}

void FormBaseWizardPage::SwitchToNextPage(FormBaseWizardPage *page)
{
	page->SetPreviousPage(this);
	page->SetPPEnable();
	CPropertySheet* ps = (CPropertySheet*)GetParent();	
	ps->SetActivePage(page);

}
