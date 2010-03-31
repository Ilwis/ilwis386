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
// IlwisExpressionDocTemplate.cpp : implementation file
//

// handling of parameters -showas and -edit
// still have to be implemented
// needed is a function similar to MatchDocType

#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\BaseDocTemplate.h"
#include "Client\Base\IlwisExpressionDocTemplate.h"
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
// IlwisExpressionDocTemplate

IMPLEMENT_DYNAMIC(IlwisExpressionDocTemplate, BaseDocTemplate)

IlwisExpressionDocTemplate::IlwisExpressionDocTemplate(
    String sExprTy,
    CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, 
    CRuntimeClass* pViewClass)
:	BaseDocTemplate(pDocClass, pFrameClass, pViewClass),
  sExprType(sExprTy)
{
  sExprType.toLower();
}

IlwisExpressionDocTemplate::~IlwisExpressionDocTemplate()
{
}

/////////////////////////////////////////////////////////////////////////////
// IlwisExpressionDocTemplate diagnostics

#ifdef _DEBUG
void IlwisExpressionDocTemplate::AssertValid() const
{
	CMultiDocTemplate::AssertValid();
}

void IlwisExpressionDocTemplate::Dump(CDumpContext& dc) const
{
	CMultiDocTemplate::Dump(dc);
}
#endif //_DEBUG

CDocTemplate::Confidence IlwisExpressionDocTemplate::MatchDocType(LPCTSTR lpszPathName,
	CDocument*& rpDocMatch)
{
	ASSERT(lpszPathName != NULL);
	rpDocMatch = NULL;

	// go through all documents
	POSITION pos = GetFirstDocPosition();
	while (pos != NULL)
	{
		CDocument* pDoc = GetNextDoc(pos);
		if (AfxComparePath(pDoc->GetPathName(), lpszPathName))
		{
			rpDocMatch = pDoc;
			return yesAlreadyOpen;
		}
	}

  bool fExtOk = false;
    return fTypeOk(lpszPathName) ? yesAttemptNative : noAttempt;
  return noAttempt;
}

bool IlwisExpressionDocTemplate::fTypeOk(LPCTSTR lpszPathName)
{
	String sExpr(lpszPathName);
  sExpr.toLower();
	int iWhere = sExpr.find(sExprType);
  return 0 == iWhere;
}

BOOL IlwisExpressionDocTemplate::GetDocString(CString& sRes, enum DocStringIndex index) const // get one of the info strings
{
	return BaseDocTemplate::GetDocString(sRes, index);
}

