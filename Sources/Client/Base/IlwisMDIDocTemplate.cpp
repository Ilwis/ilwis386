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
#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"
#include "Client\ilwis.h"
#include "Client\Base\BaseDocTemplate.h"
#include "Client\Base\Res.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Base\IlwisMDIDocTemplate.h"
#include "Client\Base\Framewin.h"
#include "Client\MainWindow\Catalog\CatalogFrameWindow.h"
#include "Headers\Hs\Mainwind.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(IlwisMDIDocTemplate, IlwisDocTemplate)

IlwisMDIDocTemplate::IlwisMDIDocTemplate(
    const char* pcExt, 
    const char* pcType, 
	  const char* pcObjectName,
    CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, 
    CRuntimeClass* pViewClass) :
  IlwisDocTemplate(pcExt, pcType, pcObjectName, pDocClass, pFrameClass, pViewClass)
{
}

IlwisMDIDocTemplate::~IlwisMDIDocTemplate()
{
}

CFrameWnd* IlwisMDIDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
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
	CMDIChildWnd *wndMDI = dynamic_cast<CMDIChildWnd *>(pFrame);

	if (wndMDI == NULL)
	{
		TRACE1("Warning: Dynamic create of frame %hs failed.\n",
			m_pFrameClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CFrameWnd, wndMDI);

	if (context.m_pNewViewClass == NULL)
		TRACE0("Warning: creating frame with no default view.\n");

  if (!wndMDI->Create(NULL, "ILWIS", WS_CHILD|WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE, CFrameWnd::rectDefault,
      (CMDIFrameWnd *)IlwWinApp()->GetMainWnd(), &context)) 
  {
	  TRACE0("Warning: IlwisDocTemplate couldn't create a frame.\n");
	  // frame will be deleted in PostNcDestroy cleanup
	  return NULL;
  }
	CWnd* pMainWnd = AfxGetThread()->m_pMainWnd;
	// it worked !
	return wndMDI;
}

void IlwisMDIDocTemplate::InitialUpdateFrame(CFrameWnd* pFrame, 
          CDocument* pDocument, BOOL bMakeVisible)
{
	IlwisDocument* pid = dynamic_cast<IlwisDocument*>(pDocument);
	CatalogFrameWindow* pfrm = dynamic_cast<CatalogFrameWindow*>(pFrame);
	HICON ic = pid->icon();
	if (pid)
		pFrame->SetIcon(ic, TRUE);
	ASSERT_VALID(pfrm);
	if (pfrm)
		pfrm->InitialUpdateFrame(pDocument, bMakeVisible);
}

CDocument* IlwisMDIDocTemplate::OpenDocumentFile(
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
	//AfxGetThread()->m_pMainWnd = pFrame;
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
		//FileName fn(lpszPathName);
		if (!pDocument->OnOpenDocument(lpszPathName)) //fn.sFullPathQuoted().c_str()))
		{
			// user has be alerted to what failed in OnOpenDocument
			TRACE0("CDocument::OnOpenDocument returned FALSE.\n");
			//pFrame->DestroyWindow();
			return NULL;
		}
		pDocument->SetModifiedFlag(FALSE);
	}

 	InitialUpdateFrame(pFrame, pDocument, bMakeVisible);

	return pDocument;
}

#ifdef _DEBUG
void IlwisMDIDocTemplate::AssertValid() const
{
	IlwisDocTemplate::AssertValid();
}

void IlwisMDIDocTemplate::Dump(CDumpContext& dc) const
{
	IlwisDocTemplate::Dump(dc);
}
#endif //_DEBUG
