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
/* $Log: /ILWIS 3.0/FormElements/formbase.cpp $
* 
* 26    16-10-01 15:16 Retsios
* Memory leak (CFont cleanup)
* 
* 25    2/24/01 15:19 Retsios
* Repair memory leaks
* 
* 24    14-07-00 4:29p Martin
* sGetCurDir (from IlwisWinApp) instead of sCurDir
* 
* 23    14/07/00 15:28 Willem
* Copy and Print now only use one empty line after Page title
* 
* 22    14/07/00 15:26 Willem
* Preparing text for printing adds both CR and LF (they are both needed)
* 
* 21    5-07-00 9:36 Koolhoven
* Application Forms now use Show and Define buttons instead of OK
* 
* 20    4/07/00 16:18 Willem
* The Print() and Copy() function now accept a title text
* 
* 19    26-06-00 11:58a Martin
* added flag for signaling end of the initialization of creation of a
* form
* 
* 18    11-05-00 9:52 Koolhoven
* If the parent of a form is a valid window (not 0) but not (yet)
* visible, the mainwindow is made the parent
* 
* 17    7-02-00 11:42 Hendrikse
* Bevel is destroyed in remove childeren. 
* 
* 16    26/01/00 15:59 Willem
* Removed some memory leaks
* 
* 15    13-12-99 16:24 Koolhoven
* In FrameWindow use OnCommandHelp() instead of OnHelp() to prevent
* stealing of the F1 key.
* IlwisDocument sets now OnHelp() active to enable (a.o.) the Help
* buttons in the proerty pages.
* FormBaseDialog maps OnCommandHelp() on its OnHelp()
* FormBasePropertyPage now also implements OnHelp()
* 
* 14    29-11-99 10:25 Wind
* adapted for UNC (use now SetCurrentDirectory and GetCurrentDirectory)
* 
* 13    23-11-99 2:59p Martin
* all buttons are now down under
* 
* 12    23/11/99 11:26 Willem
* - Printing a form now gives a correct message when finished; also no
* rubbish is printed anymore
* - Removed form class: GeneralDataEditForm (also the dependents in
* flddom.*)
* 
* 11    21-10-99 10:17a Martin
* font sizes are now correctly calculated
* 
* 10    11-10-99 11:35a Martin
* added copy for text elements (came from frmbasedlg)
* 
* 9     14-09-99 2:19p Martin
* added support for printing and copying the forms contents
* 
* 8     23-06-99 3:53p Martin
* changes needed to rebuild forms without recreating them
* 
* 7     14-06-99 9:47a Martin
* Changed the making of the default buttons (OK, cancel, help)
* 
* 6     3-06-99 11:51 Koolhoven
* Use GetDesktopWindow() instead of AfxGetMainWnd() for DummyWindow
* because desktop window is alwyas available
* 
* 5     16-04-99 15:02 Koolhoven
* sfFORM instead of sfForm
* 
* 4     4/09/99 10:33a Martin
* Font errors and a constructor errors
* 
* 3     4/01/99 12:12p Martin
* Standard font come from IlwisApp. This is a const font, caused several
* problems
// Revision 1.6  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.5  1997/09/04 10:13:45  Wim
// Slightly rewritten Init(),
// should not make any difference.
//
// Revision 1.4  1997-08-29 19:35:14+02  Wim
// delete SysMen in destructor of FormBase
//
/* formbase.c 
FormBase
(c) Computer Department ITC
/  by Jelle Wind (developed from 1st implementation by Wim Koolhoven), dec. 1993
Last change:  WK   12 Aug 98    5:17 pm
*/
#define FORMBASE_CPP

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Engine\Base\StlExtensions.h"
#include "Headers\Hs\Mainwind.hs"

#define wDialogType(fSysModal) \
	(fSysModal ? WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_SYSMODAL : \
	WS_CAPTION | WS_SYSMENU | DS_MODALFRAME)


const CFont *FormBase::fntDflt = 0;


//-----[ FormWithDest ]-----------------------------------------------------------
FormWithDest::~FormWithDest()
{
}

int FormWithDest::exec() 
{ 
	root->StoreData(); // fill destinations with current form entry values
	return 0; 
}  

//-----[ FormWithParam ]-----------------------------------------------------------
//FormBaseDialog:: FormBaseDialog(parent, sTitle, pl, IlwWinApp()->Context()->svl()->fGet("form"), false, true) {}

int FormWithParm::exec() 
{ 
	root->StoreData(); // fill destinations with current form entry values
	return 0; 
}

FormBase::FormBase() :
par(0),
fbs(fbsBUTTONSUNDER),
fnt(NULL),
_windsp(NULL),
_fOkClicked(false),
_npChanged(0),
_htp(0),
sCaption(),
root(NULL),
plDflt(0),
dummyWindow(NULL),
fEndInitialization(false),
fNoCheckData(false),
feLastInserted(0)
{
}

FormBase::FormBase(CWnd *p, const String& sTitle, bool fShowAlways, bool fMod, bool fHideOnOk) :
par(p),
fbs(fbsBUTTONSUNDER),
fnt(NULL),
_windsp(NULL),
_fOkClicked(false),
_npChanged(0),
_htp(0),
sCaption(sTitle),
root(NULL),
plDflt(0),
dummyWindow(NULL),
fEndInitialization(false),
fNoCheckData(false),
feLastInserted(0)
{
	if (fShowAlways) fbs |= fbsSHOWALWAYS;
	if (fMod)        fbs |= fbsMODAL;
	if (fHideOnOk)   fbs |= fbsHIDEONOK;
	if (0 != par && IsWindow(par->m_hWnd) && !par->IsWindowVisible())
		par = AfxGetApp()->GetMainWnd();
}


FormBase::FormBase(CWnd* p, const String& sTitle, int style) :
par(p),
fbs(fbsBUTTONSUNDER | style),
fnt(NULL),
_windsp(NULL),
_fOkClicked(false),
_npChanged(0),
_htp(0),
plDflt(0),
sCaption(sTitle),
root(NULL),
dummyWindow(NULL),
fEndInitialization(false),
fNoCheckData(style & fbsNoCheckData),
feLastInserted(0)
{
	if (0 != par && IsWindow(par->m_hWnd) && !par->IsWindowVisible())
		par = AfxGetApp()->GetMainWnd();
}

FormBase::FormBase(CWnd* p, const String& sTitle, ParmList *plDefault, bool fShowAlways, bool fMod, bool fHideOnOk) :
par(p),
fbs(fbsBUTTONSUNDER),
fnt(NULL),
_windsp(NULL),
_fOkClicked(false),
sCaption(sTitle),
root(NULL),
_npChanged(0),
_htp(0),
plDflt(0),
dummyWindow(NULL)
{
	if (fShowAlways)  fbs |= fbsSHOWALWAYS;
	if (fMod) fbs |= fbsMODAL;
	if (fHideOnOk) fbs |= fbsHIDEONOK;
	plDflt = plDefault;
	if (0 != par && IsWindow(par->m_hWnd) && !par->IsWindowVisible())
		par = AfxGetApp()->GetMainWnd();
	fNoCheckData = false;
}

void FormBase::Init()
{
	if (fModal())
	{
		dummyWindow=new CWnd(); // dummy is needed for the creation sequence with modal dialogs.
		dummyWindow->Create(NULL, "dummy", WS_CHILD , CRect(0,0,100,100), CWnd::GetDesktopWindow(), 101);
		_windsp = new zDisplay(dummyWindow);
	} 
	if (0 == root)
		root = new FormEntry(this);
	ilwapp = IlwWinApp()->Context();
	if (FormBase::fntDflt==0) // NB dit moet verplaatst worden naar hoofd programma, wegens afwezigheid hiervan
	{                         // nu tijdelijk hier
		FormBase::fntDflt=ILWISAPP->GetFont(IlwisWinApp::sfFORM);
	}
	if (fntDflt == 0) 
	{
		fnt = windsp()->font();
		rFontRatio = 1;
	}
	else 
	{
		CFont *basefnt = windsp()->font();
		LOGFONT logfnt;
		basefnt->GetLogFont(&logfnt);
		double rOldHeight = logfnt.lfHeight;
		//			this const cast is needed to retrieve information of the font. does not change a thing
		(const_cast<CFont *>(fntDflt))->GetLogFont(&logfnt);
		double rNewHeight = logfnt.lfHeight;
		rFontRatio = abs(rNewHeight / rOldHeight);

		if (fnt)
			delete fnt;
		fnt = new CFont();
		fnt->CreateFontIndirect(&logfnt);
	}
	_iDefFldHeight = 25.0 * rFontRatio;
	_iDefFldWidth = 40.0 * rFontRatio;
	_iDefFldOffSet = 70.0 * rFontRatio;

}

bool FormBase::fModal() const
{
	return fbs & fbsMODAL ? true : false;
}

bool FormBase::fShowAlways() const
{
	return fbs & fbsSHOWALWAYS ? true : false;;
}

bool FormBase::fHideOnOk() const
{
	return fbs & fbsHIDEONOK ? true : false;;
}

void FormBase::Prepare()
{

}

FormBase::~FormBase()
{
	if (fnt)
	{
		delete fnt;
		fnt = 0;
	}
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
	delete root;
	String sDir = IlwWinApp()->sGetCurDir();
	SetCurrentDirectory(sDir.sVal());
}

FormEntry* FormBase::feDefaultFocus()
{
	FormEntry* fe;
	for (int i=0; i < childlist().iSize(); i++) 
	{
		fe = childlist()[i]->CheckData();
		if (fe) 
			return fe;
	}
	return 0;
}

void FormBase::Add(Parm *prm)
{
	if (prm != 0) 
	{
		prm->Replace(plDflt);
		pl.Add(prm);
	}
}

FormEntry* FormBase::CheckData()
{ 
	FormEntry *pfe;
	for (short i=0; i < childlist().iSize(); i++) 
	{
		pfe = childlist()[i]->CheckData();
		if (pfe)
			return pfe;
	}
	return 0;
}

void FormBase::RemoveChildren()
{
	Array<FormEntry*>& children = childlist();

	for (short i=0; i<children.iSize(); i++)
		delete children[i];
	children.Resize(0);

	butOK.DestroyWindow();
	butCancel.DestroyWindow();
	butHelp.DestroyWindow();
	bevel.DestroyWindow();

	root->psn->Init(); 
	root->psn->SetBound(25, 25, 25, 25);
}

void FormBase::RecalculateForm()
{
	RemoveChildren();
	_npChanged = NULL;
	_fOkClicked = false;
	create();
}


String FormBase::sText()
{
	return root->sChildrensText();
}

bool fNotAmpers(char c)
{
	return c != '&';
}

int FormBase::Copy(const String& sPageTitle)
{
	zClipboard cb(wnd());
	cb.clear();
	String sTxt;
	if (sPageTitle.length() > 0)
		sTxt = String("%S\r\n", sPageTitle);
	sTxt &= sText();

	// remove & from text
	String sOut(' ', sTxt.size());
	copy_if(sTxt.begin(), sTxt.end(), sOut.begin(), fNotAmpers);
	cb.add(sOut.sVal());

	return 1;
}

int FormBase::Print(const String& sPageTitle)
{
	String sOut;
	if (sPageTitle.length() > 0)
		sOut = String("%S\r\n", sPageTitle);
	sOut &= sText();

	// remove & from text
	String sTxt(' ', sOut.size());
	copy_if(sOut.begin(), sOut.end(), sTxt.begin(), fNotAmpers); // remove '&'
	replace(sTxt.begin(), sTxt.end(), '\t', ' '); // replace \t

	CPrintDialog dlg(FALSE); 
	dlg.DoModal();
	HDC pdc = dlg.GetPrinterDC();
	if ( pdc == 0 ) return 0;

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
			par->MessageBox(_T(TR("Could not start page").c_str()));         
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

			while(sTxt != "")
			{
				String sHead = sTxt.sHead("\r");
				dcPrinter.TextOut(5 * sz.cx ,( 5 + i ) * sz.cy , sHead.c_str(), sHead.size());
				sTxt = sTxt.sTail("\n");
				++i;
			}
			dcPrinter.EndPage();         
			dcPrinter.EndDoc();
			dcPrinter.SelectObject(oldfnt);
		}   
	}

	return 1;
}

void FormBase::shutdown(int)
{
	TRACE0("Error: Overrule FormBase::shutdown()");
}

void FormBase::OnOK()
{
	if ( fbsNoCheckData == false) {
		FormEntry *pfe = CheckData();
		if (pfe) 
		{
			MessageBeep(MB_ICONEXCLAMATION);
			pfe->SetFocus();
		}
	}
	_fOkClicked = true;
	//    if (fHideOnOk()) ShowWindow(SW_HIDE);
	exec();
	shutdown(IDOK);
}

void FormBase::EnableOK() 
{ 
	if (butOK) 
		butOK.EnableWindow(TRUE); 
	if (butShow) 
		butShow.EnableWindow(TRUE); 
	if (butDefine) 
		butDefine.EnableWindow(TRUE); 
}

void FormBase::DisableOK()    
{ 
	if (butOK) 
		butOK.EnableWindow(FALSE); 
	if (butShow) 
		butShow.EnableWindow(FALSE); 
	if (butDefine) 
		butDefine.EnableWindow(FALSE); 
}

