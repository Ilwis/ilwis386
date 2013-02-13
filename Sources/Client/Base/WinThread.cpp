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
// WinThread.cpp : implementation file
//

#include "Headers\stdafx.h"
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\WinThread.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisMDIDocTemplate.h"
#include <afxdisp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WinThread

BEGIN_MESSAGE_MAP(WinThread, CWinThread)
	//{{AFX_MSG_MAP(WinThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// internal MFC functions used here
// copied from afximpl.h
BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
BOOL AFXAPI AfxResolveShortcut(CWnd* pWnd, LPCTSTR pszShortcutFile,
	LPTSTR pszPath, int cchPath);


WinThread::WinThread(CDocManager *pdm, const String& fn, const ParmList& pl) :
 m_pDocManager(pdm),
 m_pDocTemplate(0),
 m_rtcType(0)
{
	 String sCom = const_cast<ParmList &>(pl).sCmd();
	 parms.Fill(sCom);
	 String rest = const_cast<ParmList&>(pl).sGet("extras");
	 Array<String> parts;
	 Split(rest,parts, ",");
	 String fullname = FileName(fn).sFile;
	 for(unsigned int i = 0; i < parts.size(); ++i) {
		FileName fnextra(parts[i]);
		fullname += "+" + parts[i];
	 }
	 strFileName = fullname + FileName(fn).sExt;
}	

WinThread::WinThread(CDocManager* pdm, LPCTSTR lpszFileName, IlwisDocument::OpenType opentype)           
: m_pDocManager(pdm)
, m_pDocTemplate(0)
, m_rtcType(0)
, ot(opentype)
{
	// it seems lpszFileName contains ONE QUOTED OR UNQUOTED filename, NO PARAMS, or an expression like MatrixVarCov(name)
	sDfltDir = IlwWinApp()->sGetCurDir();
	String sFileExpr = lpszFileName;
	size_t iStart = sFileExpr.find('(');
	size_t iEnd = sFileExpr.find(')');
	size_t iPoint = sFileExpr.find('.');
	bool fExpr = iStart > 1 && iStart < iEnd && iPoint < iEnd;
	if (!fExpr && lpszFileName)
	{
		FileName fn = IlwisObjectPtr::fnCheckPath(lpszFileName);
		strFileName = fn.sFullPathQuoted(); // Full Path, With Quotes (strFileName is passed to functions that split it with ParmList)
		strFileName += fn.sSectionPostFix; // ????
	}
	else // think of MatrixVarCov(name)
		strFileName = sFileExpr;
}

WinThread::WinThread(IlwisDocTemplate* pdt, LPCTSTR lpszFileName, IlwisDocument::OpenType opentype)           
: m_pDocTemplate(pdt)
, m_pDocManager(0)
, m_rtcType(0)
, ot(opentype)
{
	// it seems lpszFileName contains a list of files, QUOTED if necessary and optionally parameters, or an expression like MatrixVarCov(name)
	sDfltDir = IlwWinApp()->sGetCurDir();
	String sFileExpr = lpszFileName;
	size_t iStart = sFileExpr.find('(');
	size_t iEnd = sFileExpr.find(')');
	size_t iPoint = sFileExpr.find('.');
	bool fExpr = iStart > 1 && iStart < iEnd && iPoint < iEnd;
	if (!fExpr && lpszFileName) 
	{
		ParmList pm(lpszFileName);
		strFileName = "";
		for (int iFile = 0; iFile < pm.iFixed(); iFile++) // strip off parameters
		{
			FileName fn = IlwisObjectPtr::fnCheckPath(pm.sGet(iFile));
			if (strFileName.length() > 0)
				strFileName += " ";
			strFileName += fn.sFullPathQuoted(); // fill string with Quoted Full Path FileNames
		}
	}
	else // think of MatrixVarCov(name)
		strFileName = sFileExpr;
}

WinThread::WinThread(CRuntimeClass* rtc, const String& sCmd)           
: m_pDocTemplate(0)
, m_pDocManager(0)
, m_rtcType(rtc)
, ot(IlwisDocument::otNORMAL)
, strFileName(sCmd)
{
	sDfltDir = IlwWinApp()->sGetCurDir();
}

WinThread::~WinThread()
{
	IlwWinApp()->Context()->RemoveThreadLocalVars();
}

BOOL WinThread::InitInstance()
{
	struct OpeningDoc {
		OpeningDoc()
		{ IlwWinApp()->iOpeningDoc++; }
		~OpeningDoc()
		{ IlwWinApp()->iOpeningDoc--; }
	} od;
	
	m_pMainWnd = NULL;
	IlwWinApp()->Context()->InitThreadLocalVars();

	// make UI-threads more responsive - removed 23/11/00 by Wim, because too general
	//	SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	::OleInitialize(NULL);

	try {
		char* sFile = strFileName.sVal();
		if ("" == strFileName) {
			IlwWinApp()->SetCurDir(sDfltDir);
			sFile = 0;
		}
		else
			IlwWinApp()->SetCurDir(FileName(sFile).sPath());

		CDocument* doc = 0;
		if (m_pDocTemplate)
			doc = m_pDocTemplate->OpenDocumentFile(sFile, true);
		else if (m_pDocManager && parms.iSize() > 0 )			
			doc = OpenDocumentFile(m_pDocManager, sFile, parms);
		else if (m_pDocManager)	
			doc = OpenDocumentFile(m_pDocManager, sFile, ot);
		else if (m_rtcType)	
		{
			ParmList pm(strFileName);

			strFileName = pm.sGet(0);  // make sure we have only the name, remove/ignore flags and options
			m_rtcType->CreateObject();
			// no option to send parameters through yet, could come later
			return TRUE;
		}
		if (0 == doc) 
			return FALSE;
		String str = doc->GetPathName();
		if (str != "") {
			FileName fn(sFile);
			IlwWinApp()->SetCurDir(fn.sPath());
		}
		return TRUE;
	}
	catch (std::exception& err) {
		const char *txt = err.what();
		String mes("%s, unknown cause", txt);
		ErrorObject errObj(mes);
		errObj.Show();
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
	return FALSE;
}

int WinThread::ExitInstance()
{
	::OleUninitialize();
	return CWinThread::ExitInstance();
}

String WinThread::sFileName() const
{
  return strFileName;
}

CDocument* WinThread::OpenDocumentFile(CDocManager* pdm, LPCTSTR lpszFileName, IlwisDocument::OpenType ot)
{
// largely copied from CDocManager::OpenDocumentFile(LPCTSTR lpszFileName)
	// find the highest confidence
	POSITION pos = pdm->GetFirstDocTemplatePosition();
	CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
	CDocTemplate* pBestTemplate = NULL;
	CDocument* pOpenDocument = NULL;

	while (pos != NULL)
	{
		CDocTemplate* pTemplate = pdm->GetNextDocTemplate(pos);
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		CDocTemplate::Confidence match;
		ASSERT(pOpenDocument == NULL);
		if ( FileName(lpszFileName).sExt == ".mpl") {
			int kk=0;
			++kk;
		}
		BaseDocTemplate *templ = dynamic_cast<BaseDocTemplate *>(pTemplate);
		if ( templ && templ->sGetObjectType() == "ILWIS Animation" &&  ot ==  IlwisDocument::otANIMATION) {
			pBestTemplate = pTemplate;
			break;     
		}
		if ( templ && templ->sGetObjectType() == "Color Composite" &&  ot ==  IlwisDocument::otCOLORCOMP) {
			pBestTemplate = pTemplate;
			break;     
		}
		if ( templ && templ->sGetObjectType() == "ILWIS CollectionLayer" &&  ot ==  IlwisDocument::otCOLLECTION) {
			pBestTemplate = pTemplate;
			break;     
		}
		if ( FileName(lpszFileName).sExt == ".mpl"){ // temporary until colorcompisite has its own map
			continue;     
		}
		match = pTemplate->MatchDocType(lpszFileName, pOpenDocument);
		if (match > bestMatch)
		{
			if ( dynamic_cast<CMultiDocTemplate *>(pTemplate) != NULL && bestMatch > CDocTemplate::noAttempt)
				continue;
			bestMatch = match;
			pBestTemplate = pTemplate;
		}
		if (match == CDocTemplate::yesAlreadyOpen)
			break;      // stop here
	}

	if (pOpenDocument != NULL)
	{
		POSITION pos = pOpenDocument->GetFirstViewPosition();
		if (pos != NULL)
		{
			CView* pView = pOpenDocument->GetNextView(pos); // get first one
			HWND hWndTop = 0;
			for (HWND hWnd = pView->GetSafeHwnd(); hWnd != NULL; hWnd = ::GetParent(hWnd))
				hWndTop = hWnd;
      ::ShowWindow(hWndTop, SW_SHOWNORMAL);
      ::SetForegroundWindow(hWndTop);
		}
		else
		{
			IlwisDocTemplate* idt = dynamic_cast<IlwisDocTemplate*>(pBestTemplate);
			if (idt)
				return idt->OpenDocumentFile(pOpenDocument);
			else
				TRACE0("Error: Can not find a view for document to activate.\n");
		}
		return pOpenDocument;
	}

	if (pBestTemplate == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
		return NULL;
	}
	IlwisMDIDocTemplate* mdit = dynamic_cast<IlwisMDIDocTemplate*>(pBestTemplate);
	if (mdit) 
	{
		return NULL;
	}
	IlwisDocTemplate* idt = dynamic_cast<IlwisDocTemplate*>(pBestTemplate);
	if (idt)
		return idt->OpenDocumentFile(ot, lpszFileName);
	else
		return pBestTemplate->OpenDocumentFile(lpszFileName);
}


CDocument* WinThread::OpenDocumentFile(CDocManager* pdm, LPCTSTR lpszFileName, ParmList& pm)
{
// largely copied from CDocManager::OpenDocumentFile(LPCTSTR lpszFileName)
	// find the highest confidence
	POSITION pos = pdm->GetFirstDocTemplatePosition();
	CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
	CDocTemplate::Confidence match = CDocTemplate::noAttempt;
	CDocTemplate* pBestTemplate = NULL;
	CDocument* pOpenDocument = NULL;
	String sMethod = pm.sGet("method");	

	while (pos != NULL)
	{
		CDocTemplate* pTemplate = pdm->GetNextDocTemplate(pos);
		BaseDocTemplate* pBaseTemplate = dynamic_cast<BaseDocTemplate *>(pTemplate);		
		match = CDocTemplate::noAttempt;

		ASSERT(pOpenDocument == NULL);
		if ( pBaseTemplate )
			match = pBaseTemplate->MatchDocType(lpszFileName, pOpenDocument, sMethod);
		else {
			//match = pTemplate->MatchDocType(lpszFileName, pOpenDocument);
		}
		if (match > bestMatch )
		{
			bestMatch = match;
			pBestTemplate = pTemplate;
		}
		if (match == CDocTemplate::yesAlreadyOpen)
			break;      // stop here
	}

	if (pOpenDocument != NULL)
	{
		POSITION pos = pOpenDocument->GetFirstViewPosition();
		if (pos != NULL)
		{
			CView* pView = pOpenDocument->GetNextView(pos); // get first one
			HWND hWndTop = 0;
			for (HWND hWnd = pView->GetSafeHwnd(); hWnd != NULL; hWnd = ::GetParent(hWnd))
				hWndTop = hWnd;
      ::ShowWindow(hWndTop, SW_SHOWNORMAL);
      ::SetForegroundWindow(hWndTop);
		}
		else
		{
			IlwisDocTemplate* idt = dynamic_cast<IlwisDocTemplate*>(pBestTemplate);
			if (idt)
				return idt->OpenDocumentFile(pOpenDocument);
			else
				TRACE0("Error: Can not find a view for document to activate.\n");
		}
		return pOpenDocument;
	}

	if (pBestTemplate == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
		return NULL;
	}
	IlwisMDIDocTemplate* mdit = dynamic_cast<IlwisMDIDocTemplate*>(pBestTemplate);
	if (mdit) 
	{
		return NULL;
	}
	IlwisDocTemplate* idt = dynamic_cast<IlwisDocTemplate*>(pBestTemplate);
	BaseDocTemplate *tplIlwis = dynamic_cast<BaseDocTemplate *>(pBestTemplate);
	if ( tplIlwis)
		return tplIlwis->OpenDocumentFile(lpszFileName, pm);
	return pBestTemplate->OpenDocumentFile(lpszFileName);
}
