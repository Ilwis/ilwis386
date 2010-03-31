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
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Representation\Rprgrad.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmpict.h"
#include "Client\Editors\Representation\RepresentationDoc.h"
#include "Headers\Hs\Represen.hs"
#include "Headers\Hs\DAT.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
   
//-- [RepresentationClassDoc ]--------------------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(RepresentationClassDoc, RepresentationDoc)

BEGIN_MESSAGE_MAP(RepresentationClassDoc, RepresentationDoc)
	//{{AFX_MSG_MAP(RepresentationClassDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RepresentationClassDoc construction/destruction

RepresentationClassDoc::RepresentationClassDoc()
{
	// TODO: add one-time construction code here

}

BOOL RepresentationClassDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
  ISTRUE(fINotEqual, lpszPathName, (LPCTSTR)0);
  ISTRUE(fINotEqual, lpszPathName[0], (char)0);

	if (!RepresentationDoc::OnOpenDocument(lpszPathName))
		return FALSE;

	try
	{
		FileName fn(lpszPathName);
		Representation rprClass(fn);
		DomainSort *pds = rprClass->dm()->pdsrt();
		DomainPicture *pdp = rprClass->dm()->pdp();
	}		
	catch (const ErrorObject& err)
	{
		err.Show();
		return FALSE;
	}		

  return TRUE;
}

RepresentationClassDoc::~RepresentationClassDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// RepresentationClassDoc diagnostics

#ifdef _DEBUG
void RepresentationClassDoc::AssertValid() const
{
	RepresentationDoc::AssertValid();
}

void RepresentationClassDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

