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
// DomDoc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Engine\Domain\dmsort.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Domain\DomDoc.h"
#include "Engine\Base\AssertD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DomainDoc

IMPLEMENT_DYNCREATE(DomainDoc, IlwisDocument)

DomainDoc::DomainDoc()
{
}

DomainDoc::~DomainDoc()
{
}


BEGIN_MESSAGE_MAP(DomainDoc, IlwisDocument)
	//{{AFX_MSG_MAP(DomainDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_COPY_AS, OnUpdateSaveCopyAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


IlwisObject DomainDoc::obj() const
{
  return dom;
}

Domain DomainDoc::dm() const
{
  return dom;
}


/////////////////////////////////////////////////////////////////////////////
// DomainDoc diagnostics

#ifdef _DEBUG
void DomainDoc::AssertValid() const
{
	IlwisDocument::AssertValid();
}

void DomainDoc::Dump(CDumpContext& dc) const
{
	IlwisDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// DomainDoc commands

BOOL DomainDoc::OnNewDocument()
{
  // not allowed to create an empty DomainDoc
  return FALSE;
/*
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
*/
}

BOOL DomainDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!IlwisDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	FileName fn(lpszPathName);
    
	dom = Domain(fn);

	if (!dom.fValid())
		return FALSE;
//    SetTitle(dom->sDescription.sVal());
	return TRUE;
}

zIcon DomainDoc::icon() const
{
  return zIcon("DomainIcon");
}

void DomainDoc::OnUpdateSaveCopyAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fCIStrEqual(obj()->fnObj.sExt, ".dom"));
}
