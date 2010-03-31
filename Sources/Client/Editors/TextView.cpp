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
// TextView.cpp : implementation of the TextView class
// Created by Martin Schouwenburg 18-3-99

#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\Framewin.h"
#include "Client\Base\BaseView2.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\TextDoc.h"
#include "Client\Editors\TextView.h"
#include "Headers\messages.h"
#include "Client\Base\ZappToMFC.h"
#include "Headers\Hs\Script.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////
// TextView

static DWORD WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

IMPLEMENT_DYNCREATE(TextView, BaseView2)

BEGIN_MESSAGE_MAP(TextView, BaseView2)
	//{{AFX_MSG_MAP(TextView)
	//ON_COMMAND(ID_EDIT_UNDO, OnEditUndo);
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_FILE_PRINT, OnPrint)
	ON_COMMAND(ID_EDIT_DELETE, OnDelete)
//	ON_COMMAND(ID_EDIT_SELECT_ALL)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, OnUpdateFindReplace)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateDelete)	
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdatePrint)		
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_DIRECT, OnUpdatePrintDirect)			
	ON_WM_CREATE()
	ON_MESSAGE(ILWM_DESCRIPTIONTEXT, OnDescriptionChanged)
	ON_MESSAGE(ILWM_VIEWSETTINGS,    OnViewSettings)
  ON_WM_KILLFOCUS()
  ON_WM_SETFOCUS()
	ON_WM_SIZE()
//  ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplaceCmd )
    
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TextView construction/destruction

TextView::TextView() :
  edv(NULL)
{
}

TextView::~TextView()
{
}

BOOL TextView::PreCreateWindow(CREATESTRUCT& cs)
{
 	BOOL bPreCreated = BaseView2::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

int TextView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int iRet = BaseView2::OnCreate(lpCreateStruct);

	ISTRUE(fIEqual, iRet, 0);

	return iRet;
}

void TextView::OnKillFocus( CWnd* pNewWnd )
{
	BaseView2::OnKillFocus(pNewWnd);
	FrameWindow *frm = dynamic_cast<FrameWindow *>( GetParentFrame());
	if ( frm )
		frm->HandleAccelerators(true);
 }

void TextView::OnSetFocus( CWnd* pNewWnd )
{
	BaseView2::OnSetFocus(pNewWnd);
	FrameWindow *frm = dynamic_cast<FrameWindow *>( GetParentFrame());
	if ( frm )
		frm->HandleAccelerators(false);
}

// sets the text and size of window
void TextView::OnInitialUpdate() 
{
	edv = new EditView;

	edv->Create(NULL, "edv", WS_CHILD | WS_VISIBLE, CRect(0,0,100,100), this, 647);

	CreateDescriptionBar(); 

	BaseView2::OnInitialUpdate();

	TextDocument *scrdoc = GetDocument();
	ISTRUE(fINotEqual, scrdoc, (TextDocument *)NULL);

	CFont *fnt = IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium);
	scrdoc->AddView(edv);
	CEdit &ed = edv->GetEditCtrl();
	ed.SetFont(fnt);
	ed.SetTabStops(15);

	if (scrdoc->fReadOnly()) 
	{
		ed.SetReadOnly(TRUE);
		descBar.SetReadOnly(true);
	}

	string sTxt = scrdoc->sGetText();
	ed.SetWindowText(sTxt.c_str()); // set text in main edit
	descBar.SetText(scrdoc->sObjectDescription()); // set description

	ed.SetFocus();

	scrdoc->SetModifiedFlag(false);
}


/////////////////////////////////////////////////////////////////////////////
// TextView drawing

void TextView::OnDraw(CDC* pDC)
{
	TextDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// TextView diagnostics

#ifdef _DEBUG
void TextView::AssertValid() const
{
	BaseView2::AssertValid();
}

void TextView::Dump(CDumpContext& dc) const
{
	BaseView2::Dump(dc);
}
#endif //_DEBUG

TextDocument* TextView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(TextDocument)));
	return dynamic_cast<TextDocument *>(m_pDocument);
}

/////////////////////////////////////////////////////////////////////////////
// TextView message handlers
void TextView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	BaseView2::OnActivateView(bActivate, pActivateView, pDeactiveView);
}


void TextView::OnPrint()
{
	edv->PostMessage(WM_COMMAND, ID_FILE_PRINT);
}

void TextView::OnEditCopy()
{
	CEdit& ed = edv->GetEditCtrl();
	ed.Copy();
}

void TextView::OnEditPaste()
{
	CEdit& ed = edv->GetEditCtrl();
	ed.Paste();
}

void TextView::OnEditCut()
{
	CEdit& ed = edv->GetEditCtrl();
	ed.Cut();
}

void TextView::OnDelete()
{
	edv->GetEditCtrl().Clear();
}


void TextView::OnUpdateDelete(CCmdUI *cmd)
{
		edv->OnUpdateDelete(cmd);
}

void TextView::OnUpdatePrint(CCmdUI *cmd)
{
		cmd->Enable(TRUE);
}

void TextView::OnUpdatePrintDirect(CCmdUI *cmd)
{
		cmd->Enable(TRUE);
}

void TextView::OnUpdatePaste(CCmdUI *cmd)
{
	TextDocument *scrdoc = GetDocument();
	zClipboard cb(this);
	if ( scrdoc->fReadOnly() || cb.isTextAvail() == 0)
		cmd->Enable(FALSE);
}

void TextView::OnUpdateFindReplace(CCmdUI *cmd)
{
	TextDocument *scrdoc=GetDocument();
	if ( scrdoc->fReadOnly())
		cmd->Enable(FALSE);
}

//-------------------------------------------------------------------
BEGIN_MESSAGE_MAP(EditView, CEditView)
	//{{AFX_MSG_MAP(TextView)
	ON_COMMAND(ID_EDIT_DELETE, OnDelete)
//	ON_COMMAND(ID_EDIT_SELECT_ALL)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateDelete)	
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdatePrint)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

EditView::EditView()
{
	m_bPrintSelection = FALSE;
	m_nNextSelCharToBePrint = m_nFirstNonSelChar = 0;
	odt = new COleDropTarget;
}

EditView::~EditView()
{
	delete odt;
}

void EditView::OnInitialUpdate()
{
	odt->Register(this);
}

BOOL EditView::OnPreparePrinting(CPrintInfo* pInfo)
 {

	if (!AfxGetApp()->GetPrinterDeviceDefaults(&pInfo->m_pPD->m_pd) ||
		 pInfo->m_pPD->m_pd.hDevMode == NULL)
		return DoPreparePrinting(pInfo);	
	CEdit& ed =GetEditCtrl();
 	ed.GetSel(m_nNextSelCharToBePrint,
                    m_nFirstNonSelChar);
	if (m_nNextSelCharToBePrint != m_nFirstNonSelChar)
  {
 		pInfo->m_pPD->m_pd.Flags &= ~PD_NOSELECTION;

 		pInfo->m_pPD->m_pd.Flags |= PD_SELECTION;
  }

	HGLOBAL	hDevMode = pInfo->m_pPD->m_pd.hDevMode;
	HGLOBAL hDevNames = pInfo->m_pPD->m_pd.hDevNames;

	if ( hDevMode && hDevNames )
	{	
		DEVMODE* pDevMode = (DEVMODE*) ::GlobalLock(hDevMode);
		DEVNAMES* pDevNames = (DEVNAMES*) ::GlobalLock(hDevNames);


		LPCSTR pstrDriverName = ((LPCSTR) pDevNames)+pDevNames->wDriverOffset;
		LPCSTR pstrDeviceName = ((LPCSTR) pDevNames)+pDevNames->wDeviceOffset;
		LPCSTR pstrOutputPort = ((LPCSTR) pDevNames)+pDevNames->wOutputOffset;

		CDC dcPrinter;
		if (dcPrinter.CreateDC(pstrDriverName, pstrDeviceName, pstrOutputPort, NULL))
		{
			CFont* pOldFont = dcPrinter.SelectObject(GetPrinterFont());
			CalcPageCount(&dcPrinter, pInfo);
			dcPrinter.SelectObject(pOldFont);
		}	
	}

	BOOL fValue = CEditView::OnPreparePrinting(pInfo);

 	m_bPrintSelection = pInfo->m_pPD->PrintSelection();

 return fValue; //rvalue;
 }

void EditView::CalcPageCount(CDC* pDC, CPrintInfo* pInfo)
{
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int m_cyChar = tm.tmHeight;	// Get the font height
	int m_cxChar = tm.tmAveCharWidth;

	// figure out the page width and height
	// remember that the physical page size isn't completely
	// printable--subtract the nonprintable area from it.

	int iPageWidth = pDC->GetDeviceCaps(PHYSICALWIDTH)
			- 2 * (pDC->GetDeviceCaps(PHYSICALOFFSETX));
	int m_iPageHeight = pDC->GetDeviceCaps(PHYSICALHEIGHT)
			- 2 * (pDC->GetDeviceCaps(PHYSICALOFFSETY))
			- 5 * m_cyChar;  // header size: is currently fixed

	CDC dcScr;
	dcScr.CreateDC("DISPLAY", NULL, NULL, NULL);
	int m_iScreenDPI = dcScr.GetDeviceCaps(LOGPIXELSY);
	int m_iPrintDPI = pDC->GetDeviceCaps(LOGPIXELSY);

	// find out how high our font is
	// figure out how many lines of that font fit on a page

	int iHeight = 5 * m_cyChar / 4;
 	int iPages;

	iPages = (GetEditCtrl().GetLineCount() * iHeight + (m_iPageHeight - 1)) / m_iPageHeight - 1;
	int m_iItemsPerPage = m_iPageHeight / iHeight;

	// set up printing info structure

	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(iPages);
		
	return;
}


void EditView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	if (m_bPrintSelection)
  {
		pInfo->m_bContinuePrinting =
    m_nNextSelCharToBePrint < m_nFirstNonSelChar;
  }
	else
  {
   CEditView::OnPrepareDC(pDC, pInfo);
  }
}


void EditView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	if (m_bPrintSelection)
	{
		m_nNextSelCharToBePrint =	PrintInsideRect(pDC, pInfo->m_rectDraw,
		m_nNextSelCharToBePrint, m_nFirstNonSelChar);
	}
	else
	{
		CEditView::OnPrint(pDC, pInfo);
	}
}

DROPEFFECT EditView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	bool fOk = false;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
	    FileName fn(sFileName);
			// for the time being only accept from own directory
			// so just indication, no action yet
			if (fn.sExt == ".isl")
				return DROPEFFECT_COPY;
			else
				return DROPEFFECT_NONE;
		}
	}
	return DROPEFFECT_NONE;
}


BOOL EditView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
//	if (!pDataObject->IsDataAvailable(CF_HDROP))
//		return FALSE;
//
//	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
//	HDROP hDrop = (HDROP)GlobalLock(hnd);
//	int iFiles = 0;
//	vector<FileName> afn;
//	if (hDrop) {
//		char sFileName[MAX_PATH+1];
//		iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
//		for (int i = 0; i < iFiles; ++i) {
//			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
//	    afn.push_back(FileName(sFileName));
//		}
//		GlobalUnlock(hDrop);
//	}
//	GlobalFree(hnd);
//	if ( afn.size() > 0)
//	{
//		String sFiles;
//		CatalogDocument *doc = GetDocument();
//		const CString& cstr = doc->GetPathName();
//		for(vector<FileName>::iterator cur = afn.begin(); cur != afn.end(); ++cur)
//		{
//			String sPath = (*cur).sPath().toLower();
//			if ( sPath != String(cstr).toLower() ) // do not drop files in the same dir
//				sFiles += (*cur).sFullPathQuoted() + " ";
//		}
//
//		if ( sFiles == "") return FALSE;
//		String sCmd = String("copy %S %s -quiet", sFiles, cstr);
//		char* str = sCmd.sVal();
//		//IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
//		CWaitCursor cwr;
//		CommandHandler::CopyObjects(sCmd, 0, this, true);
//		doc->UpdateAllViews(0);
//	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/// Commands
const string TextView::sText() const
{
		CString sTxt;
	  CEdit& ed = edv->GetEditCtrl();
		ed.GetWindowText(sTxt);
		return string(sTxt);
}

String TextView::sGetDescription()
{
	return descBar.sGetText();
}

LRESULT TextView::OnDescriptionChanged(WPARAM wP, LPARAM lP)
{
	//return BaseView::OnDescriptionChanged(wP, lP);
	TextDocument *scrdoc = GetDocument();
	scrdoc->SetModifiedFlag();

	return 1;

}

LRESULT TextView::OnViewSettings(WPARAM wP, LPARAM lP)
{
	return BaseView2::OnViewSettings(wP, lP);
}

void TextView::LoadState(IlwisSettings& settings)
{
	BaseView2::LoadState(settings);
}

void TextView::SaveState(IlwisSettings& settings)
{
	BaseView2::SaveState(settings);
}

void TextView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	if ( GetSafeHwnd() == NULL) return;
	CEdit& ed = edv->GetEditCtrl();
	if ( ed. GetSafeHwnd() == NULL) return;

	BaseView2::OnUpdate(pSender, lHint, pHint);

	TextDocument *scrdoc = GetDocument();

	if (scrdoc->fReadOnly()) 
	{
		ed.SetReadOnly(TRUE);
		descBar.SetReadOnly(true);
	}
	
	string sTxt = scrdoc->sGetText();
	ed.SetWindowText(sTxt.c_str()); // set text in main edit
	descBar.SetText(scrdoc->sObjectDescription()); // set description

}

void TextView::OnSize( UINT nType, int cx, int cy )
{
	if ( !edv) return;
	CEdit& ed = edv->GetEditCtrl();
	if ( ed.GetSafeHwnd() != NULL)
		ed.MoveWindow(0,0,cx,cy);
}

void TextView::OnEditFind()
{
	edv->PostMessage(WM_COMMAND, ID_EDIT_FIND);
}

void TextView::OnEditReplace()
{
	edv->PostMessage(WM_COMMAND, ID_EDIT_REPLACE);
}

void EditView::OnDelete()
{
	CEdit& ed = GetEditCtrl();
	int iStart, iEnd;
	ed.GetSel(iStart, iEnd);
	if (iStart == iEnd)
    return;
	ed.Clear();
}


void EditView::OnUpdateDelete(CCmdUI *cmd)
{
	int iStart, iEnd;
	CEdit& ed = GetEditCtrl();
	ed.GetSel(iStart, iEnd);
	bool fDelPossible = iStart != iEnd;
	cmd->Enable(fDelPossible);
}

void EditView::OnUpdatePrint(CCmdUI *cmd)
{
	//TextDocument *scrdoc=GetDocument();
	//if ( scrdoc->fReadOnly())
		cmd->Enable(TRUE);
}

BOOL TextView::PreTranslateMessage(MSG* pMsg)
{
	CString str;
  if (pMsg->message == WM_KEYDOWN)
	{
		if (!fHandleAccelerator)
			return FALSE;
	}
  return BaseView2::PreTranslateMessage(pMsg);
}
