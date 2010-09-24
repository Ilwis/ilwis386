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
// BaseDocTemplate.cpp : implementation file
//

// handling of parameters -showas and -edit
// still have to be implemented
// needed is a function similar to MatchDocType

#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\BaseDocTemplate.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\Framewin.h"
#include "Headers\Hs\Mainwind.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// defined in afximpl.h:
BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);


/////////////////////////////////////////////////////////////////////////////
// BaseDocTemplate

IMPLEMENT_DYNAMIC(BaseDocTemplate, CMultiDocTemplate)

BaseDocTemplate::BaseDocTemplate(
								 CRuntimeClass* pDocClass,
								 CRuntimeClass* pFrameClass, 
								 CRuntimeClass* pViewClass)
								 :	CMultiDocTemplate(1, pDocClass, pFrameClass, pViewClass)
{
}

BaseDocTemplate::~BaseDocTemplate()
{
}

/////////////////////////////////////////////////////////////////////////////
// BaseDocTemplate diagnostics

#ifdef _DEBUG
void BaseDocTemplate::AssertValid() const
{
	CMultiDocTemplate::AssertValid();
}

void BaseDocTemplate::Dump(CDumpContext& dc) const
{
	CMultiDocTemplate::Dump(dc);
}
#endif //_DEBUG

CDocument* BaseDocTemplate::OpenDocumentFile(CDocument* pDocument, BOOL bMakeVisible)
{
	ASSERT_VALID(pDocument);
	BOOL bAutoDelete = pDocument->m_bAutoDelete;
	pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
	CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
	AfxGetThread()->m_pMainWnd = pFrame;
	pDocument->m_bAutoDelete = bAutoDelete;
	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete pDocument;       // explicit delete on error
		return NULL;
	}
	ASSERT_VALID(pFrame);
	InitialUpdateFrame(pFrame, pDocument, bMakeVisible);

	return pDocument;
}

CDocument* BaseDocTemplate::OpenDocumentFile(
	LPCTSTR lpszPathName, BOOL bMakeVisible)
{
	// largely copied from CMultiDocTemplate::OpenDocumentFile
	CDocument* pDocument = CreateNewDocument();
	if (pDocument == NULL)
	{
		TRACE0("CDocTemplate::CreateNewDocument returned NULL.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT_VALID(pDocument);

	BOOL bAutoDelete = pDocument->m_bAutoDelete;
	pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
	CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
	AfxGetThread()->m_pMainWnd = pFrame;
	pDocument->m_bAutoDelete = bAutoDelete;
	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete pDocument;       // explicit delete on error
		return NULL;
	}
	ASSERT_VALID(pFrame);

	if (lpszPathName == NULL)
	{
		// create a new document - with default document name
		SetDefaultTitle(pDocument);

		// avoid creating temporary compound file when starting up invisible
		if (!bMakeVisible)
			pDocument->m_bEmbedded = TRUE;

		if (!pDocument->OnNewDocument())
		{
			// user has be alerted to what failed in OnNewDocument
			TRACE0("CDocument::OnNewDocument returned FALSE.\n");
			pFrame->DestroyWindow();
			return NULL;
		}

		// it worked, now bump untitled count
		m_nUntitledCount++;
	}
	else
	{
		// open an existing document
		CWaitCursor wait;
		if (!pDocument->OnOpenDocument(lpszPathName))
		{
			// user has be alerted to what failed in OnOpenDocument
			TRACE0("CDocument::OnOpenDocument returned FALSE.\n");
			pFrame->DestroyWindow();
			return NULL;
		}
		pDocument->SetModifiedFlag(FALSE);
	}

	InitialUpdateFrame(pFrame, pDocument, bMakeVisible);

	return pDocument;
}

IlwisDocument* BaseDocTemplate::OpenDocumentFile( IlwisDocument::OpenType ot, LPCTSTR lpszPathName)
{
	// largely copied from CMultiDocTemplate::OpenDocumentFile
	IlwisDocument* pDocument = dynamic_cast<IlwisDocument*>(CreateNewDocument());
	if (pDocument == NULL)
	{
		TRACE0("CDocTemplate::CreateNewDocument returned NULL.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT_VALID(pDocument);

	BOOL bAutoDelete = pDocument->m_bAutoDelete;
	pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
	CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
	AfxGetThread()->m_pMainWnd = pFrame;
	pDocument->m_bAutoDelete = bAutoDelete;
	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete pDocument;       // explicit delete on error
		return NULL;
	}
	ASSERT_VALID(pFrame);

	if (lpszPathName == NULL)
	{
		// create a new document - with default document name
		SetDefaultTitle(pDocument);

		if (!pDocument->OnNewDocument())
		{
			// user has be alerted to what failed in OnNewDocument
			TRACE0("CDocument::OnNewDocument returned FALSE.\n");
			pFrame->DestroyWindow();
			return NULL;
		}

		// it worked, now bump untitled count
		m_nUntitledCount++;
	}
	else
	{
		// open an existing document
		CWaitCursor wait;
		if (!pDocument->OnOpenDocument(lpszPathName, ot))
		{
			// user has be alerted to what failed in OnOpenDocument
			TRACE0("IlwisDocument::OnOpenDocument returned FALSE.\n");
			// when thread is closing, hWnd is invalid, prevent crash
			if (::IsWindow(pFrame->m_hWnd))
				pFrame->DestroyWindow();
			return NULL;
		}
		pDocument->SetModifiedFlag(FALSE);
	}

	InitialUpdateFrame(pFrame, pDocument, TRUE);

	return pDocument;
}

IlwisDocument* BaseDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, ParmList& pm)
{
	// largely copied from CMultiDocTemplate::OpenDocumentFile
	IlwisDocument* pDocument = dynamic_cast<IlwisDocument*>(CreateNewDocument());
	if (pDocument == NULL)
	{
		TRACE0("CDocTemplate::CreateNewDocument returned NULL.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT_VALID(pDocument);

	BOOL bAutoDelete = pDocument->m_bAutoDelete;
	pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
	CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
	AfxGetThread()->m_pMainWnd = pFrame;
	pDocument->m_bAutoDelete = bAutoDelete;
	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete pDocument;       // explicit delete on error
		return NULL;
	}
	ASSERT_VALID(pFrame);

	if (lpszPathName == NULL)
	{
		// create a new document - with default document name
		SetDefaultTitle(pDocument);

		if (!pDocument->OnNewDocument())
		{
			// user has be alerted to what failed in OnNewDocument
			TRACE0("CDocument::OnNewDocument returned FALSE.\n");
			pFrame->DestroyWindow();
			return NULL;
		}

		// it worked, now bump untitled count
		m_nUntitledCount++;
	}
	else
	{
		// open an existing document
		CWaitCursor wait;
		if (!pDocument->OnOpenDocument(lpszPathName, pm))
		{
			// user has be alerted to what failed in OnOpenDocument
			TRACE0("IlwisDocument::OnOpenDocument returned FALSE.\n");
			// when thread is closing, hWnd is invalid, prevent crash
			if (::IsWindow(pFrame->m_hWnd))
				pFrame->DestroyWindow();
			return NULL;
		}
		pDocument->SetModifiedFlag(FALSE);
	}
	if ( !pm.fExist("noshow"))
		InitialUpdateFrame(pFrame, pDocument, TRUE);
	else
	{
		pDocument->OnCloseDocument();
		return NULL;
	}		

	return pDocument;
}

void BaseDocTemplate::InitialUpdateFrame(CFrameWnd* pFrame, 
										 CDocument* pDocument, BOOL bMakeVisible)
{
	IlwisDocument* pid = dynamic_cast<IlwisDocument*>(pDocument);
	if (pid)
		pFrame->SetIcon(pid->icon(), TRUE);
	FrameWindow* pfrm = dynamic_cast<FrameWindow*>(pFrame);
	ASSERT_VALID(pfrm);
	if (pfrm)
		pfrm->InitialUpdate(pDocument, bMakeVisible);
}


CFrameWnd* BaseDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
{
	// largely copied from CDocTemplate::CreateNewFrame
	if (pDoc != NULL)
		ASSERT_VALID(pDoc);
	// create a frame wired to the specified document

	//	ASSERT(m_nIDResource != 0); // must have a resource ID to load from
	CCreateContext context;
	context.m_pCurrentFrame = pOther;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = m_pViewClass;
	context.m_pNewDocTemplate = this;

	if (m_pFrameClass == NULL)
	{
		TRACE0("Error: you must override CDocTemplate::CreateNewFrame.\n");
		ASSERT(FALSE);

		return NULL;
	}
	CFrameWnd* pFrame = (CFrameWnd*)m_pFrameClass->CreateObject();
	if (pFrame == NULL)
	{
		TRACE1("Warning: Dynamic create of frame %hs failed.\n",
			m_pFrameClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);

	if (context.m_pNewViewClass == NULL)
		TRACE0("Warning: creating frame with no default view.\n");

	String clsName("ILWIS");
	if (!pFrame->Create(NULL, clsName.scVal(), WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE, CFrameWnd::rectDefault,
		::AfxGetMainWnd(), NULL, 0, &context)) 
	{
		TRACE0("Warning: BaseDocTemplate couldn't create a frame.\n");
		// frame will be deleted in PostNcDestroy cleanup
		return NULL;
	}
	// it worked !
	return pFrame;
}

CDocTemplate::Confidence BaseDocTemplate::MatchDocType(LPCTSTR lpszPathName,
													   CDocument*& rpDocMatch)
{
	return noAttempt;
}

CDocTemplate::Confidence BaseDocTemplate::MatchDocType(const FileName& fnFO, CDocument*& rpDocMatch, const String& sMethod)
{
	return noAttempt;
}

bool BaseDocTemplate::fTypeOk(LPCTSTR lpszPathName)
{
	return false;
}

BOOL BaseDocTemplate::GetDocString(CString& sRes, enum DocStringIndex index) const // get one of the info strings
{
	switch (index) {
		case windowTitle:        // default window title
			return FALSE;
		case docName:            // user visible name for default document
			return FALSE;
		case fileNewName:        // user visible name for FileNew
			sRes = sObjectType.scVal();
			return TRUE;
			return FALSE;
			// for file based documents:
		case filterName:         // user visible name for FileOpen
			sRes = sObjectType.scVal();
			return TRUE;
			// for file based documents with Shell open support:
		case regFileTypeId:      // REGEDIT visible registered file type identifier
			sRes = "ILWIS";			
			return TRUE;
		case regFileTypeName:    // Shell visible registered file type name
			return FALSE;
		default:
			return FALSE;
	}
}

void BaseDocTemplate::SetObjectType(const String& s)
{
	sObjectType = s;
}

String BaseDocTemplate::sGetObjectType() const
{
	return sObjectType;
}

